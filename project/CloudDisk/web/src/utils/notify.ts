import { reactive } from 'vue'

export type NotifyType = 'success' | 'info' | 'warning' | 'error'

export interface NotifyItem {
  id: number
  type: NotifyType
  message: string
  duration: number
}

let nextId = 0

export const notifications = reactive<NotifyItem[]>([])

export function notify(type: NotifyType, message: string, duration = 5000) {
  const id = nextId++
  notifications.push({ id, type, message, duration })
  if (duration > 0) {
    setTimeout(() => removeNotify(id), duration)
  }
}

export function removeNotify(id: number) {
  const idx = notifications.findIndex(n => n.id === id)
  if (idx !== -1) notifications.splice(idx, 1)
}

export function clearAllNotify() {
  notifications.splice(0, notifications.length)
}

export const notifySuccess = (msg: string, dur?: number) => notify('success', msg, dur)
export const notifyInfo = (msg: string, dur?: number) => notify('info', msg, dur)
export const notifyWarning = (msg: string, dur?: number) => notify('warning', msg, dur)
export const notifyError = (msg: string, dur?: number) => notify('error', msg, dur)
