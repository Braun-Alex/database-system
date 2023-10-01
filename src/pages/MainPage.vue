<template>
  <div class="q-pa-md" v-show="!tableView">
      <div class="q-pa-md q-gutter-md">
        <q-btn :loading="!retrievingFinished" square color="blue" icon="refresh" icon-right="refresh" glossy
               @click="showDatabases()" no-caps>
          Переглянути наявні бази даних
        </q-btn>

        <q-btn label="Створити нову базу даних" square color="green" icon="add_circle" icon-right="add_circle" glossy
               @click="newDatabasePrompt = true" no-caps />

        <q-dialog v-model="newDatabasePrompt" persistent>
          <q-card>
            <q-card-section>
              <div class="text-h6">Вкажіть назву нової бази даних</div>
            </q-card-section>

            <q-card-section class="q-pt-none">
              <q-input clearable maxlength="50" dense v-model="databaseName" autofocus @keyup.enter="newDatabasePrompt = false" />
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
                <q-icon name="settings_applications" color="black" />
              </q-item-section>

              <q-item-section top class="col-2 gt-sm">
                <q-item-label class="q-mt-sm">
                  {{ database }}
                </q-item-label>
              </q-item-section>

              <q-item-section top side>
                <div class="text-grey-8 q-gutter-xs">
                  <q-btn round color="teal" icon="travel_explore" @click="showTables(database)">
                    <q-tooltip>Переглянути базу даних</q-tooltip>
                  </q-btn>
                  <q-btn round color="amber" icon="edit" @click="renamingDatabasePrompt = true">
                    <q-tooltip>Перейменувати базу даних</q-tooltip>
                  </q-btn>
                  <q-dialog v-model="renamingDatabasePrompt" persistent>
                    <q-card>
                      <q-card-section>
                        <div class="text-h6">Вкажіть назву нової бази даних</div>
                      </q-card-section>

                      <q-card-section class="q-pt-none">
                        <q-input clearable maxlength="50" dense v-model="databaseName" autofocus
                                 @keyup.enter="renamingDatabasePrompt = false" />
                      </q-card-section>

                      <q-card-actions align="right" class="text-primary">
                        <q-btn flat label="Відмінити" v-close-popup no-caps />
                        <q-btn flat label="Перейменувати базу даних" v-close-popup no-caps @click="renameDatabase(database)" />
                      </q-card-actions>
                    </q-card>
                  </q-dialog>
                  <q-btn round color="red" icon="cancel" @click="deleteDatabase(database)">
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
    <div class="q-pa-md q-gutter-md">
      <q-btn label="Створити нову таблицю" square color="green" icon="add_circle" icon-right="add_circle" glossy
             @click="newTablePrompt = true" no-caps />

      <q-btn label="Повернутися" square style="background: #1FA39A; color: white" icon="keyboard_backspace"
             icon-right="keyboard_backspace" glossy @click="tableView = false" no-caps />

      <q-dialog v-model="newTablePrompt" persistent>
        <q-card>
          <q-form @submit="createTable()">
          <q-card-section>
            <div class="text-h6">Вкажіть дані для створення таблиці</div>
          </q-card-section>

          <q-card-section class="q-pt-none">
            <q-input hint="Назва таблиці" clearable maxlength="50" dense v-model="tableName" autofocus
                     @keyup.enter="newTablePrompt = false" lazy-rules :rules="[
                       val => val && val.length > 0 || 'Назва таблиці не може бути порожньою'
                     ]" />

            <q-select
              hint="Атрибути таблиці у форматі 'значення': 'тип'"
              v-model="columns"
              use-input
              use-chips
              multiple
              hide-dropdown-icon
              input-debounce="0"
              lazy-rules
              @new-value="createColumn"
              :rules="[
                val => val && val.length > 0 || 'Перелік атрибутів таблиці не може бути порожнім'
        ]"
            />
          </q-card-section>

          <q-card-actions align="right" class="text-primary">
            <q-btn flat label="Відмінити" v-close-popup no-caps />
            <q-btn flat label="Створити таблицю" type="submit" :v-close-popup="tableName.length > 0 && (columns as string).length" no-caps @click="createTable()" />
          </q-card-actions>
          </q-form>
        </q-card>
      </q-dialog>

      <transition
        appear
        enter-active-class="animated fadeIn"
        leave-active-class="animated fadeOut"
      >
        <q-list bordered separator class="rounded-borders">

          <q-item v-for="table in allTables" :key="table">
            <q-item-section avatar top>
              <q-icon name="dataset_linked" color="black" />
            </q-item-section>

            <q-item-section top class="col-2 gt-sm">
              <q-item-label class="q-mt-sm">
                {{ table }}
              </q-item-label>
            </q-item-section>

            <q-item-section top side>
              <div class="text-grey-8 q-gutter-xs">
                <q-btn round color="teal" icon="manage_search" @click="showRows(table)">
                  <q-tooltip>Переглянути таблицю</q-tooltip>
                </q-btn>
                <q-btn round color="amber" icon="edit" @click="renamingTablePrompt = true">
                  <q-tooltip>Перейменувати таблицю</q-tooltip>
                </q-btn>
                <q-dialog v-model="renamingTablePrompt" persistent>
                  <q-card>
                    <q-card-section>
                      <div class="text-h6">Вкажіть назву нової таблиці</div>
                    </q-card-section>

                    <q-card-section class="q-pt-none">
                      <q-input dense v-model="tableName" autofocus @keyup.enter="renamingTablePrompt = false" />
                    </q-card-section>

                    <q-card-actions align="right" class="text-primary">
                      <q-btn flat label="Відмінити" v-close-popup no-caps />
                      <q-btn flat label="Перейменувати таблицю" v-close-popup no-caps @click="renameTable(table)" />
                    </q-card-actions>
                  </q-card>
                </q-dialog>
                <q-btn round color="red" icon="cancel" @click="deleteTable(table)">
                  <q-tooltip>Видалити таблицю</q-tooltip>
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
const retrievingFinished: Ref<boolean> = ref<boolean>(true)
const newDatabasePrompt: Ref<boolean> = ref<boolean>(false)
const newTablePrompt: Ref<boolean> = ref<boolean>(false)
const renamingDatabasePrompt: Ref<boolean> = ref<boolean>(false)
const renamingTablePrompt: Ref<boolean> = ref<boolean>(false)
const tableView: Ref<boolean> = ref<boolean>(false)
const databaseName: Ref<string> = ref<string>('')
const tableName: Ref<string> = ref<string>('')
const allDatabases: Ref<string[]> = ref<string[]>([])
const allTables: Ref<string[]> = ref<string[]>([])
const currentDatabase: Ref<string> = ref<string>('')
const columns: Ref<null> = ref(null)
import axios from 'axios'
import { useQuasar } from 'quasar'
const $q = useQuasar()

interface Column {
  type: string,
  name: string
}

function createColumn (val: string, done: any) {
  done(val.toLowerCase(), 'add-unique')
}

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
    allTables.value = response.data.Tables
    currentDatabase.value = databaseName
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
