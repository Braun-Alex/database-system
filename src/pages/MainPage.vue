<template>
  <div class="q-pa-md" v-show="!tableView">
      <div class="q-pa-md q-gutter-md">
        <q-btn :loading="!retrievingFinished" square color="blue" glossy @click="showDatabases()" no-caps>
          Переглянути наявні бази даних
        </q-btn>

        <q-btn label="Створити нову базу даних" square color="green" glossy @click="newDatabasePrompt = true" no-caps />

        <q-dialog v-model="newDatabasePrompt" persistent>
          <q-card>
            <q-card-section>
              <div class="text-h6">Вкажіть назву нової бази даних</div>
            </q-card-section>

            <q-card-section class="q-pt-none">
              <q-input dense v-model="databaseName" autofocus @keyup.enter="newDatabasePrompt = false" />
            </q-card-section>

            <q-card-actions align="right" class="text-primary">
              <q-btn flat label="Відмінити" v-close-popup no-caps />
              <q-btn flat label="Створити базу даних" v-close-popup no-caps @click="createDatabase()" />
            </q-card-actions>
          </q-card>
        </q-dialog>

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
                </q-item-label>
              </q-item-section>

              <q-item-section top side>
                <div class="text-grey-8 q-gutter-xs">
                  <q-btn round color="teal" icon="ballot" @click="showTables(database)">
                    <q-tooltip>Переглянути базу даних</q-tooltip>
                  </q-btn>
                  <q-btn round color="amber" icon="camera" @click="renamingDatabasePrompt = true">
                    <q-tooltip>Перейменувати базу даних</q-tooltip>
                  </q-btn>
                  <q-dialog v-model="renamingDatabasePrompt" persistent>
                    <q-card>
                      <q-card-section>
                        <div class="text-h6">Вкажіть назву нової бази даних</div>
                      </q-card-section>

                      <q-card-section class="q-pt-none">
                        <q-input dense v-model="databaseName" autofocus @keyup.enter="renamingDatabasePrompt = false" />
                      </q-card-section>

                      <q-card-actions align="right" class="text-primary">
                        <q-btn flat label="Відмінити" v-close-popup no-caps />
                        <q-btn flat label="Перейменувати базу даних" v-close-popup no-caps @click="renameDatabase(database)" />
                      </q-card-actions>
                    </q-card>
                  </q-dialog>
                  <q-btn round color="red" icon="layers_clear" @click="deleteDatabase(database)">
                    <q-tooltip>Видалити базу даних</q-tooltip>
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
  <div class="q-pa-md" v-show="tableView">
    <p>{{ allDatabases }}</p>
  </div>
</template>

<script setup lang="ts">
import { ref, Ref } from 'vue'
const retrievingFinished: Ref<boolean> = ref<boolean>(true)
const newDatabasePrompt: Ref<boolean> = ref<boolean>(false)
const renamingDatabasePrompt: Ref<boolean> = ref<boolean>(false)
const tableView: Ref<boolean> = ref<boolean>(false)
const databaseName: Ref<string> = ref<string>('')
const allDatabases: Ref<string[]> = ref<string[]>([])
import axios from 'axios'
import { useQuasar } from 'quasar'
const $q = useQuasar()

function showDatabases () {
  retrievingFinished.value = false
  axios.get('http://localhost:8080/show').then((response: any) => {
    allDatabases.value = []
    response.data.Databases.forEach((database: string) => {
      if (database !== 'template0' && database !== 'template1' && database !== 'postgres') {
        allDatabases.value.push(database)
      }
    })
  }).catch((error) => {
    $q.notify({
      type: 'negative',
      message: 'API сервер не працює. Причина: ' + error.message
    })
  }).finally(() => {
    retrievingFinished.value = true
  })
}

function createDatabase () {
  axios.get('http://localhost:8080/show').then((response: any) => {
    allDatabases.value = []
    response.data.Databases.forEach((database: string) => {
      if (database !== 'template0' && database !== 'template1' && database !== 'postgres') {
        allDatabases.value.push(database)
      }
    })
  })
  if (allDatabases.value.includes(databaseName.value)) {
    $q.notify({
      type: 'negative',
      message: 'Така база даних вже існує. Відмова у створенні'
    })
  } else {
    const formData = new FormData()
    formData.append('databaseName', databaseName.value)
    axios.post('http://localhost:8080/database/create', formData).then(() => {
      $q.notify({
        type: 'positive',
        message: 'Базу даних було успішно створено'
      })
      allDatabases.value.push(databaseName.value)
    }).catch((error) => {
      $q.notify({
        type: 'negative',
        message: 'Помилка при створенні бази даних. Причина: ' + error.message
      })
    }).finally(() => {
      databaseName.value = ''
    })
  }
}

function showTables (databaseName: string) {
  axios.get(`http://localhost:8080/database/show/${databaseName}`).then((response: any) => {
    allDatabases.value = response.data.Tables
    tableView.value = true
  }).catch((error) => {
    $q.notify({
      type: 'negative',
      message: 'API сервер не працює. Причина: ' + error.message
    })
  })
}

function renameDatabase (databasePreviousName: string) {
  axios.get('http://localhost:8080/show').then((response: any) => {
    allDatabases.value = []
    response.data.Databases.forEach((database: string) => {
      if (database !== 'template0' && database !== 'template1' && database !== 'postgres') {
        allDatabases.value.push(database)
      }
    })
  })
  if (allDatabases.value.includes(databaseName.value)) {
    $q.notify({
      type: 'negative',
      message: 'Така база даних вже існує. Відмова у перейменуванні'
    })
  } else {
    const formData = new FormData()
    formData.append('databasePreviousName', databasePreviousName)
    formData.append('databaseNewName', databaseName.value)
    axios.post('http://localhost:8080/database/rename', formData).then(() => {
      $q.notify({
        type: 'positive',
        message: 'Базу даних було успішно перейменовано'
      })
      showDatabases()
    }).catch((error) => {
      $q.notify({
        type: 'negative',
        message: 'Помилка при перейменуванні бази даних. Причина: ' + error.message
      })
    }).finally(() => {
      databaseName.value = ''
    })
  }
}

function deleteDatabase (databaseName: string) {
  axios.get('http://localhost:8080/show').then((response: any) => {
    allDatabases.value = []
    response.data.Databases.forEach((database: string) => {
      if (database !== 'template0' && database !== 'template1' && database !== 'postgres') {
        allDatabases.value.push(database)
      }
    })
  })
  if (!allDatabases.value.includes(databaseName)) {
    $q.notify({
      type: 'negative',
      message: 'Такої бази даних вже не існує. Відмова у видаленні'
    })
  } else {
    const formData = new FormData()
    formData.append('databaseName', databaseName)
    axios.post('http://localhost:8080/database/delete', formData).then(() => {
      $q.notify({
        type: 'positive',
        message: 'Базу даних було успішно видалено'
      })
      showDatabases()
    }).catch((error) => {
      $q.notify({
        type: 'negative',
        message: 'Помилка при видаленні бази даних. Причина: ' + error.message
      })
    })
  }
}
</script>
