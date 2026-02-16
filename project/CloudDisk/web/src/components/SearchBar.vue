<template>
  <div class="search-bar">
    <el-input
      v-model="keyword"
      placeholder="搜索文件..."
      clearable
      :prefix-icon="Search"
      @input="onInput"
      @clear="$emit('search', '')"
      @keyup.enter="$emit('search', keyword)"
    />
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import { Search } from '@element-plus/icons-vue'

const emit = defineEmits<{
  search: [keyword: string]
}>()

const keyword = ref('')
let timer: ReturnType<typeof setTimeout> | null = null

function onInput() {
  if (timer) clearTimeout(timer)
  timer = setTimeout(() => {
    emit('search', keyword.value)
  }, 300)
}
</script>

<style scoped>
.search-bar {
  width: 100%;
}
</style>
