<template>
  <div class="user-avatar" @click="$emit('click')">
    <el-avatar :size="size" :src="url || undefined" :style="!url ? { background: '#409eff' } : {}">
      <span v-if="!url" class="initials">{{ initials }}</span>
    </el-avatar>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'

const props = withDefaults(defineProps<{
  url?: string
  name?: string
  size?: number
}>(), {
  url: '',
  name: '',
  size: 40,
})

defineEmits<{ click: [] }>()

const initials = computed(() => {
  if (!props.name) return '?'
  return props.name.charAt(0).toUpperCase()
})
</script>

<style scoped>
.user-avatar {
  cursor: pointer;
  display: inline-flex;
}
.initials {
  font-size: 14px;
  font-weight: 600;
  color: #fff;
}
</style>
