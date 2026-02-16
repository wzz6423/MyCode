<template>
  <div
    class="notify-container"
    @mouseenter="hovered = true"
    @mouseleave="hovered = false"
  >
    <!-- Clear all button -->
    <div v-if="notifications.length > 1" class="notify-header">
      <button class="notify-clear-all" @click="clearAllNotify">
        全部清除 ({{ notifications.length }})
      </button>
    </div>
    <div class="notify-stack">
      <TransitionGroup name="notify">
        <div
          v-for="(item, idx) in visibleItems"
          :key="item.id"
          class="notify-toast"
          :class="[
            `notify-toast--${item.type}`,
            { 'notify-toast--stacked': !expanded && idx >= MAX_VISIBLE }
          ]"
          :style="getStackStyle(idx)"
        >
          <span class="notify-icon">{{ iconMap[item.type] }}</span>
          <span class="notify-message">{{ item.message }}</span>
          <button class="notify-close" @click="removeNotify(item.id)">&times;</button>
          <div
            v-if="item.duration > 0"
            class="notify-progress"
            :style="{ animationDuration: `${item.duration}ms` }"
          ></div>
        </div>
      </TransitionGroup>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed, ref, watch } from 'vue'
import { notifications, removeNotify, clearAllNotify } from '../utils/notify'

const MAX_VISIBLE = 3
const hovered = ref(false)

const expanded = computed(() => {
  return hovered.value || notifications.length <= MAX_VISIBLE
})

const visibleItems = computed(() => {
  if (expanded.value) return notifications
  // Show first 3 + up to 2 stacked peek cards behind
  const peekCount = Math.min(notifications.length - MAX_VISIBLE, 2)
  return notifications.slice(0, MAX_VISIBLE + peekCount)
})

function getStackStyle(idx: number) {
  if (expanded.value || idx < MAX_VISIBLE) return {}
  const offset = idx - MAX_VISIBLE + 1
  return {
    transform: `scale(${1 - offset * 0.05}) translateY(${-offset * 6}px)`,
    opacity: 1 - offset * 0.25,
    zIndex: -offset,
    pointerEvents: 'none' as const,
  }
}

// Auto-collapse when notifications change
watch(() => notifications.length, () => {
  // reactivity handles it via hovered
})

const iconMap: Record<string, string> = {
  success: '\u2713',
  info: '\u2139',
  warning: '\u26A0',
  error: '\u2715',
}
</script>

<style scoped>
.notify-container {
  position: fixed;
  top: 20px;
  right: 20px;
  z-index: 9999;
  display: flex;
  flex-direction: column;
  gap: 10px;
  max-width: 360px;
}

.notify-stack {
  position: relative;
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.notify-toast {
  display: flex;
  align-items: center;
  padding: 14px 16px;
  border-radius: 10px;
  background: rgba(255, 255, 255, 0.55);
  backdrop-filter: blur(16px);
  -webkit-backdrop-filter: blur(16px);
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.1);
  border: 1px solid rgba(255, 255, 255, 0.6);
  border-left: 4px solid #409eff;
  position: relative;
  overflow: hidden;
  min-width: 280px;
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

.notify-toast--stacked {
  margin-top: -48px;
  transform-origin: top center;
}

.notify-toast--success {
  border-left-color: #67c23a;
}

.notify-toast--info {
  border-left-color: #409eff;
}

.notify-toast--warning {
  border-left-color: #e6a23c;
}

.notify-toast--error {
  border-left-color: #f56c6c;
}

.notify-icon {
  flex-shrink: 0;
  width: 24px;
  height: 24px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 13px;
  margin-right: 10px;
  color: #fff;
}

.notify-toast--success .notify-icon {
  background: #67c23a;
}

.notify-toast--info .notify-icon {
  background: #409eff;
}

.notify-toast--warning .notify-icon {
  background: #e6a23c;
}

.notify-toast--error .notify-icon {
  background: #f56c6c;
}

.notify-message {
  flex: 1;
  font-size: 14px;
  color: #111;
  font-weight: 500;
  line-height: 1.4;
  word-break: break-word;
}

.notify-close {
  flex-shrink: 0;
  background: none;
  border: none;
  font-size: 18px;
  color: #999;
  cursor: pointer;
  padding: 0 0 0 10px;
  line-height: 1;
  transition: color 0.2s;
}

.notify-close:hover {
  color: #333;
}

.notify-progress {
  position: absolute;
  bottom: 0;
  left: 0;
  height: 3px;
  width: 100%;
  transform-origin: left;
  animation: notify-shrink linear forwards;
}

.notify-toast--success .notify-progress {
  background: #67c23a;
}

.notify-toast--info .notify-progress {
  background: #409eff;
}

.notify-toast--warning .notify-progress {
  background: #e6a23c;
}

.notify-toast--error .notify-progress {
  background: #f56c6c;
}

@keyframes notify-shrink {
  from {
    transform: scaleX(1);
  }
  to {
    transform: scaleX(0);
  }
}

/* Transition animations */
.notify-enter-active {
  transition: all 0.3s ease-out;
}

.notify-leave-active {
  transition: all 0.3s ease-in;
}

.notify-enter-from {
  opacity: 0;
  transform: translateX(80px);
}

.notify-leave-to {
  opacity: 0;
  transform: translateX(80px);
}

.notify-move {
  transition: transform 0.3s ease;
}

.notify-header {
  display: flex;
  justify-content: flex-end;
}

.notify-clear-all {
  background: rgba(255, 255, 255, 0.65);
  backdrop-filter: blur(12px);
  -webkit-backdrop-filter: blur(12px);
  color: #606266;
  border: 1px solid rgba(255, 255, 255, 0.7);
  border-radius: 14px;
  padding: 5px 16px;
  font-size: 12px;
  cursor: pointer;
  transition: all 0.3s ease;
  white-space: nowrap;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
}

.notify-clear-all:hover {
  background: rgba(255, 255, 255, 0.85);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.12);
  color: #333;
}
</style>
