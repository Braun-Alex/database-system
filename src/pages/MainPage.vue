<template>
  <div class="q-pa-md">
      <div class="q-pa-md q-gutter-md">
        <q-btn :loading="!retrievingFinished" square color="blue" glossy @click="retrieveDatabases()" no-caps>
          Переглянути наявні бази даних
        </q-btn>

        <q-btn :loading="!retrievingFinished" square color="green" glossy @click="createDatabase()" no-caps>
          Створити нову базу даних
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

              <q-item-section top class="col-2 gt-sm">
                <q-item-label class="q-mt-sm">
                  {{ database }}
                  <q-tooltip delay="1500">Дата створення</q-tooltip>
                </q-item-label>
              </q-item-section>

              <q-item-section top side>
                <div class="text-grey-8 q-gutter-xs">
                  <q-btn round color="teal" icon="ballot">
                    <q-tooltip>Переглянути базу даних</q-tooltip>
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
  </div>
</template>

<script setup lang="ts">
import { ref, Ref } from 'vue'
const allDatabases: Ref<string[]> = ref<string[]>([])
import axios from 'axios'
import { date, useQuasar } from 'quasar'
const $q = useQuasar()
const retrievingFinished: Ref<boolean> = ref<boolean>(true)

function retrieveDatabases () {
  retrievingFinished.value = false
  axios.get('http://localhost:8080/show').then((response: any) => {
    allDatabases.value = response.data.allDatabases
  }).catch((error) => {
    $q.notify({
      type: 'negative',
      message: 'API сервер не працює. Причина: ' + error.message
    })
  }).finally(() => {
    retrievingFinished.value = true
  })
}
</script>
