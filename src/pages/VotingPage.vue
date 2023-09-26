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
</script>
