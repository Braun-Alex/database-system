<template>
  <div class="q-pa-md">
    <template>
      <div class="q-pa-md q-gutter-md">
        <q-btn :loading="!retrievingFinished" square color="secondary" glossy @click="retrieveDatabases()" no-caps>
          Оновити список голосувань
        </q-btn>

        <transition
          appear
          enter-active-class="animated fadeIn"
          leave-active-class="animated fadeOut"
        >
          <q-list bordered separator class="rounded-borders">

            <q-item v-for="database in allDatabases" :key="database">
              <q-item-section avatar top>
                <q-icon name="account_tree" color="black" />
              </q-item-section>

              <q-item-section top side>
                <div class="text-grey-8 q-gutter-xs">
                  <q-btn round color="teal" :icon="poll.finished ? 'verified' : 'ballot'"
                         @click="poll.finished ? getResults(poll) : vote(poll)">
                    <q-tooltip>{{ 'Перейти' }}</q-tooltip>
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
  </div>
</template>

<script setup lang="ts">
import { ref, Ref } from 'vue'
const k
</script>
