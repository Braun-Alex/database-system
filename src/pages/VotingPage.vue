<template>
  <div class="q-pa-md q-gutter-md">
    <q-btn :loading="!retrievingFinished" square color="secondary" glossy @click="retrievePolls()" no-caps>
      Оновити список голосувань
    </q-btn>

    <transition
      appear
      enter-active-class="animated fadeIn"
      leave-active-class="animated fadeOut"
    >
    <q-list bordered separator class="rounded-borders">

      <q-item v-for="poll in allPolls" :key="poll.whenCreated">
        <q-item-section avatar top>
          <q-icon name="account_tree" color="black" />
        </q-item-section>

        <q-item-section top class="col-2 gt-sm">
          <q-item-label class="q-mt-sm">
            {{ String(poll.author.slice(0, 5) + '...' + poll.author.slice(-4)) }}
            <q-tooltip delay="1500">EVM-адреса автора</q-tooltip>
          </q-item-label>
        </q-item-section>

        <q-item-section top class="col-2 gt-sm">
          <q-item-label class="q-mt-sm">
            {{ date.formatDate(poll.whenCreated, 'DD.MM.YYYYTHH:mm:ss') }}
            <q-tooltip delay="1500">Дата створення</q-tooltip>
          </q-item-label>
        </q-item-section>

        <q-item-section top>
          <q-item-label lines="1">
            <span class="text-weight-medium">
              {{ poll.title }}
              <q-tooltip delay="1500">Назва голосування</q-tooltip>
            </span>
          </q-item-label>
          <q-item-label caption lines="1">
            {{ poll.question }}
            <q-tooltip delay="1500">Поставлене запитання</q-tooltip>
          </q-item-label>
          <q-item-label lines="1" class="q-mt-xs text-body2 text-weight-bold text-teal">
            <span>{{ getPollTime(poll.whenCreated, poll.duration) }}</span>
            <q-tooltip delay="1500">Скільки ще триватиме голосування</q-tooltip>
          </q-item-label>
        </q-item-section>

        <q-item-section center>
          <q-item-label lines="1">
            <span class="text-weight-medium">
              {{ poll.countOfVoters % 10 === 1 ? poll.countOfVoters +
              ' учасник' : (poll.countOfVoters % 10 !== 0 &&
              poll.countOfVoters % 10 <= 5 ? poll.countOfVoters +
              ' учасники' : poll.countOfVoters + ' учасників') }}
              <q-tooltip delay="1500">Кількість учасників</q-tooltip>
            </span>
          </q-item-label>
        </q-item-section>

        <q-item-section top side>
          <div class="text-grey-8 q-gutter-xs">
            <q-btn round color="teal" :icon="poll.finished ? 'verified' : 'ballot'"
                   @click="poll.finished ? getResults(poll) : vote(poll)">
              <q-tooltip>{{ poll.finished ? 'Результати' : 'Проголосувати' }}</q-tooltip>
            </q-btn>
          </div>
        </q-item-section>
      </q-item>

    </q-list>
    </transition>
    <q-inner-loading
      :showing="!retrievingFinished"
      color="teal"
      label="Ініціалізація даних..."
      label-class="text-teal"
    />
  </div>
</template>

<script setup lang="ts">
import { inject, ref, Ref, watch } from 'vue'
import { useQuasar, date, QSpinnerGears } from 'quasar'
import { ethers } from 'ethers'

interface Item {
  label: string,
  value: string,
  color: string,
  disable: boolean
}

interface Poll {
  id: number,
  title: string,
  question: string,
  proposals: string[],
  author: string,
  whenCreated: number,
  duration: number,
  countOfVoters: number,
  finished: boolean
}

const $q = useQuasar()
const contractAddress: string | undefined = process.env.VUE_APP_BALLOT_CONTRACT
const allPolls: Ref<Poll[]> = ref<Poll[]>([])
const watchPolls: Ref<Poll[]> = ref<Poll[]>([])
const retrievingFinished: Ref<boolean> = ref<boolean>(true)
const walletConnected: Ref<boolean> = inject<Ref<boolean>>('walletConnected') as Ref<boolean>
const search: Ref<string> = inject<Ref<string>>('search') as Ref<string>
const pollIdentifier: Ref<string> = inject<Ref<string>>('pollIdentifier') as Ref<string>
const authorAddress: Ref<string> = inject<Ref<string>>('authorAddress') as Ref<string>
const searchPressed: Ref<boolean> = inject<Ref<boolean>>('searchPressed') as Ref<boolean>

watch(searchPressed, () => {
  if (search.value !== '') {
    searchPoll(search.value, 'combined')
  } else if (pollIdentifier.value !== '') {
    searchPoll(pollIdentifier.value, 'id')
  } else if (authorAddress.value !== '') {
    searchPoll(authorAddress.value, 'address')
  } else {
    allPolls.value = watchPolls.value
  }
})

function searchPoll (searchData: string, searchType: string) {
  retrievingFinished.value = false
  if (searchType === 'combined') {
    const searchValue: string = search.value.toLowerCase()
    allPolls.value = watchPolls.value.filter((poll: Poll) => {
      return poll.title.toLowerCase().lastIndexOf(searchValue) !== -1 ||
        poll.question.toLowerCase().lastIndexOf(searchValue) !== -1
    })
  } else if (searchType === 'id') {
    allPolls.value = watchPolls.value.filter((poll: Poll) => {
      return poll.id.toString(10) === pollIdentifier.value
    })
  } else if (searchType === 'address') {
    allPolls.value = watchPolls.value.filter((poll: Poll) => {
      return poll.author === authorAddress.value
    })
  }
  retrievingFinished.value = true
}

function retrievePolls () {
  retrievingFinished.value = false
  allPolls.value = []
  // @ts-expect-error Window.ethers not TS
  if (typeof window.ethereum !== 'undefined') {
    // @ts-expect-error Window.ethers not TS
    const provider = new ethers.providers.Web3Provider(window.ethereum)
    const contract = new ethers.Contract(
      contractAddress as string,
      abi,
      provider
    )
    contract.getAllPolls().then((data: any) => {
      data.forEach((poll: any) => {
        allPolls.value.push({
          id: allPolls.value.length,
          title: poll.title,
          question: poll.question,
          proposals: poll.proposals,
          author: poll.author,
          whenCreated: poll.whenCreated * 1000,
          duration: poll.duration,
          countOfVoters: poll.countOfVoters,
          finished: isFinished(poll.whenCreated * 1000, poll.duration)
        })
      })
      watchPolls.value = allPolls.value
    }).catch((error: any) => {
      $q.notify({
        type: 'negative',
        message: 'Виникла помилка відображення списку голосувань: ' + error.message
      })
    }).finally(() => {
      retrievingFinished.value = true
    })
  } else {
    $q.notify({
      type: 'negative',
      message: 'MetaMask не встановлено'
    })
  }
}

function vote (poll: Poll) {
  if (!walletConnected.value) {
    $q.notify({
      type: 'negative',
      message: 'Вам потрібно під\'єднати гаманець MetaMask для голосування он-чейн'
    })
  } else {
    const pollItems: Item[] = []
    poll.proposals.forEach((proposal: string) => {
      pollItems.push({ label: proposal, value: proposal, color: 'teal', disable: false })
    })
    $q.dialog({
      title: '#' + poll.id + ': ' + poll.title,
      message: poll.question,
      options: {
        type: 'radio',
        model: poll.proposals[0],
        items: pollItems
      },
      ok: {
        glossy: true,
        label: 'Проголосувати',
        color: 'positive',
        noCaps: true
      },
      cancel: {
        glossy: true,
        label: 'Закрити',
        color: 'negative',
        noCaps: true
      },
      persistent: true
    }).onOk(choice => {
      $q.dialog({
        title: 'Підтвердження',
        message: 'Ви впевнені у своєму виборі?',
        ok: {
          glossy: true,
          label: 'Так',
          color: 'positive',
          noCaps: true
        },
        cancel: {
          glossy: true,
          label: 'Ні',
          color: 'negative',
          noCaps: true
        },
        persistent: true
      }).onOk(() => {
        // @ts-expect-error Window.ethers not TS
        if (typeof window.ethereum !== 'undefined') {
          // @ts-expect-error Window.ethers not TS
          const provider = new ethers.providers.Web3Provider(window.ethereum)
          const signer = provider.getSigner()
          const contract = new ethers.Contract(
            contractAddress as string,
            abi,
            signer
          )
          $q.loading.show({
            spinner: QSpinnerGears,
            message: 'Підписання транзакції...'
          })
          try {
            contract.vote(
              poll.id, choice).then((transaction: any) => {
              $q.loading.hide()
              $q.loading.show({
                spinner: QSpinnerGears,
                message: 'Обробка транзакції...'
              })
              transaction.wait().then(() => {
                $q.notify({
                  type: 'positive',
                  icon: 'cloud_done',
                  message: 'Ви успішно проголосували он-чейн'
                })
              }).catch((error: any) => {
                $q.notify({
                  type: 'negative',
                  message: 'У процесі обробки транзакції виникла помилка: ' + error.message
                })
              }).finally(() => {
                $q.loading.hide()
              })
            }).catch((error: any) => {
              $q.loading.hide()
              const reason: string = error.message
              if (reason.lastIndexOf('Poll has been finished!') !== -1) {
                $q.notify({
                  type: 'negative',
                  message: 'Голосування вже завершено'
                })
              } else if (reason.lastIndexOf('Voter has already voted!') !== -1) {
                $q.notify({
                  type: 'negative',
                  message: 'Ви вже проголосували у даному голосуванні'
                })
              } else {
                $q.notify({
                  type: 'negative',
                  message: 'Підписання транзакції було відхилено'
                })
              }
            })
          } catch (error: any) {
            $q.notify({
              type: 'negative',
              message: 'Виникла помилка у створенні голосування: ' + error.message
            })
          }
        } else {
          $q.notify({
            type: 'negative',
            message: 'MetaMask не встановлено'
          })
        }
      }).onCancel(() => {
        $q.notify({
          type: 'negative',
          message: 'Голосування було відхилено'
        })
      })
    })
  }
}

function getResults (poll: Poll) {
  // @ts-expect-error Window.ethers not TS
  if (typeof window.ethereum !== 'undefined') {
    // @ts-expect-error Window.ethers not TS
    const provider = new ethers.providers.Web3Provider(window.ethereum)
    const contract = new ethers.Contract(
      contractAddress as string,
      abi,
      provider
    )
    contract.getResults(poll.id).then((results: number[]) => {
      const pollItems: Item[] = []
      poll.proposals.forEach((proposal: string, index: number) => {
        const choice: string = results[index] % 10 === 1 ? 'голос' : (
           results[index] % 10 !== 0 && results[index] % 10 <= 5 ? 'голоси' : 'голосів'
        )
        pollItems.push(
          {
            label: "Варіант " + "\"" + proposal + "\" набрав " + results[index] + " " + choice,
            value: proposal,
            color: 'teal',
            disable: true
          })
      })
      $q.dialog({
        title: '#' + poll.id + ': ' + poll.title + '. Результати',
        message: "Запитання було наступним: " + "\"" + poll.question + "\". Голоси розподілилися таким чином:",
        options: {
          type: 'toggle',
          model: poll.proposals[0],
          items: pollItems
        },
        ok: {
          glossy: true,
          label: 'Зрозуміло',
          color: 'info',
          noCaps: true
        },
        cancel: false
      })
    }).catch((error: any) => {
      const reason: string = error.message
      if (reason.lastIndexOf('Poll has not been finished yet!') !== -1) {
        $q.notify({
          type: 'negative',
          message: 'Голосування ще триває'
        })
      } else {
        $q.notify({
          type: 'negative',
          message: 'Виникла помилка відображення результату голосування: ' + reason
        })
      }
    })
  } else {
    $q.notify({
      type: 'negative',
      message: 'MetaMask не встановлено'
    })
  }
}

function isFinished (whenCreated: number, duration: number) {
  const dateOfCreating = new Date(whenCreated)
  const dateOfClosing = date.addToDate(dateOfCreating, { seconds: duration })
  const currentDate = Date.now()
  return currentDate > dateOfClosing.getTime()
}

function getPollTime (whenCreated: number, duration: number) {
  if (isFinished(whenCreated, duration)) return 'Завершено'
  const dateOfCreating = new Date(whenCreated)
  const dateOfClosing = date.addToDate(dateOfCreating, { seconds: duration })
  const currentDate = Date.now()
  let answer = 'Триватиме ще більше '
  const differenceInHours = date.getDateDiff(dateOfClosing, currentDate, 'hours') - 1
  if (differenceInHours > 0) {
    answer += differenceInHours % 10 === 1 ? differenceInHours +
      ' години' : differenceInHours + ' годин'
  } else {
    const differenceInMinutes = date.getDateDiff(dateOfClosing, currentDate, 'minutes') - 1
    if (differenceInMinutes > 0) {
      answer += differenceInMinutes % 10 === 1 ? differenceInMinutes +
        ' хвилини' : differenceInMinutes + ' хвилин'
    } else {
      const differenceInSeconds = date.getDateDiff(dateOfClosing, currentDate, 'seconds') - 1
      answer += differenceInSeconds % 10 === 1 ? differenceInSeconds +
        ' секунди' : differenceInSeconds + ' секунд'
    }
  }
  return answer
}
</script>
