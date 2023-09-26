<template>
  <div class="q-pa-md">

    <q-form
      @submit="onSubmit"
      @reset="onReset"
      class="q-gutter-md text-center"
    >

      <q-input
        outlined
        v-model="title"
        label="Назва опитування"
        hint="Назва має відображати зміст опитування"
        clearable
        counter
        lazy-rules
        autogrow
        maxlength="100"
        :rules="[
          val => val && val.length > 0 || 'Поле назви опитування не може бути порожньою',
          val => val.length >= 2 || 'Назва опитування не може бути такою короткою'
          ]"
      />

      <q-input
        outlined
        v-model="question"
        label="Запитання"
        hint="Запитання, яке поставлено на порядок голосування"
        clearable
        counter
        lazy-rules
        autogrow
        maxlength="100"
        :rules="[
          val => val && val.length > 0 || 'Поле запитання до опитування не може бути порожнім',
          val => val.length >= 6 || 'Запитання до опитування не може бути таким коротким'
        ]"
      />

      <q-select
        label="Створіть список варіантів відповідей в опитуванні"
        hint="Учасник голосування матиме можливість обрати бажаний варіант зі списку"
        outlined
        v-model="proposals"
        use-input
        use-chips
        multiple
        hide-dropdown-icon
        input-debounce="0"
        lazy-rules
        @new-value="createProposal"
        :rules="[
          val => val && val.length > 0 || 'Список варіантей відповідей не може бути порожнім',
          val => val.length > 1 || 'Список має складатися принаймні з двох варіантей відповідей'
        ]"
      />

      <q-input
        outlined
        type="number"
        v-model="duration"
        label="Тривалість опитування"
        hint="Голосування триватиме вказану кількість годин"
        clearable
        counter
        lazy-rules
        autogrow
        maxlength="5"
        :rules="[
          val => val !== null && val !== '' || 'Поле тривалості опитування не може бути порожньою',
          val => val > 0 && durationInSeconds % 3600 === 0 || 'Поле тривалості опитування має бути додатнім цілим числом',
          val => val.toString().lastIndexOf('.') === -1 || 'Поле тривалості опитування не може містити крапки'
        ]"
      />

      <q-toggle v-model="accept"
                label="Я погоджуюся з тим, що голосування буде створено он-чейн"
                unchecked-icon="clear" checked-icon="check" :color="accept ? 'green' : 'red'"
                keep-color />

      <div>
        <q-btn push label="Створити голосування" type="submit" color="primary" no-caps />
        <q-btn push label="Відкликати дані" type="reset" color="warning" class="q-ml-sm" no-caps />
      </div>

    </q-form>

  </div>
</template>

<script setup lang="ts">
import { ref, Ref, inject, computed, ComputedRef } from 'vue'
import { useQuasar, format, QSpinnerGears } from 'quasar'
import { ethers } from 'ethers'
const $q = useQuasar()
const title: Ref<null> = ref(null)
const question: Ref<null> = ref(null)
const proposals: Ref<null> = ref(null)
const duration: Ref<null> = ref(null)
const durationInSeconds: ComputedRef<number | null> = computed(() => {
  return duration.value === null ? null : 3600 * duration.value
})
const accept: Ref<boolean> = ref(false)
const walletConnected: Ref<boolean> = inject<Ref<boolean>>('walletConnected') as Ref<boolean>
const contractAddress: string | undefined = process.env.VUE_APP_BALLOT_CONTRACT

function onSubmit () {
  if (!accept.value) {
    $q.notify({
      type: 'negative',
      message: 'Вам потрібно надати згоду'
    })
  } else if (!walletConnected.value) {
    $q.notify({
      type: 'negative',
      message: 'Вам потрібно під\'єднати гаманець MetaMask для створення голосування он-чейн'
    })
  } else {
    $q.dialog({
      title: 'Підтвердження',
      message: 'Ви впевнені, що хочете створити голосування?',
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
          contract.create(
            title.value,
            question.value,
            proposals.value,
            durationInSeconds.value).then((transaction: any) => {
            $q.loading.hide()
            $q.loading.show({
              spinner: QSpinnerGears,
              message: 'Обробка транзакції...'
            })
            transaction.wait().then(() => {
              $q.notify({
                type: 'positive',
                icon: 'cloud_done',
                message: 'Голосування було успішно створено он-чейн'
              })
            }).catch((error: any) => {
              $q.notify({
                type: 'negative',
                message: 'У процесі обробки транзакції виникла помилка: ' + error.message
              })
            }).finally(() => {
              $q.loading.hide()
            })
          }).catch(() => {
            $q.loading.hide()
            $q.notify({
              type: 'negative',
              message: 'Підписання транзакції було відхилено'
            })
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
        message: 'Створення голосування було відхилено'
      })
    })
  }
}

function onReset () {
  $q.dialog({
    title: 'Підтвердження',
    message: 'Ви впевнені, що хочете відкликати дані?',
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
    title.value = null
    question.value = null
    proposals.value = null
    duration.value = null
    accept.value = false
    $q.notify({
      type: 'positive',
      message: 'Дані було відкликано'
    })
  }).onCancel(() => {
    $q.notify({
      type: 'negative',
      message: 'Відкликання даних було відхилено'
    })
  })
}

function createProposal (val: string, done: any) {
  done(format.capitalize(val.toLowerCase()), 'add-unique')
}
</script>
