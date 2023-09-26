<template>
  <div class="q-pa-md">

    <q-form
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
import { ref, Ref, computed, ComputedRef } from 'vue'
import { useQuasar, format, QSpinnerGears } from 'quasar'
const $q = useQuasar()
const title: Ref<null> = ref(null)
const question: Ref<null> = ref(null)
const proposals: Ref<null> = ref(null)
const duration: Ref<null> = ref(null)
const durationInSeconds: ComputedRef<number | null> = computed(() => {
  return duration.value === null ? null : 3600 * duration.value
})
const accept: Ref<boolean> = ref(false)

function createProposal (val: string, done: any) {
  done(format.capitalize(val.toLowerCase()), 'add-unique')
}
</script>
