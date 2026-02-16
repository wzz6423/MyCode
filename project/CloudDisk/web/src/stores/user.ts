import { defineStore } from 'pinia'
import { ref } from 'vue'
import type { User } from '../types'
import { login as apiLogin, getProfile } from '../api/user'
import { isMockMode, mockUser, disableMockMode } from '../utils/mock'
import router from '../router'

export const useUserStore = defineStore('user', () => {
  const token = ref(localStorage.getItem('token') || '')
  const user = ref<User | null>(null)

  async function login(type: 'nickname' | 'phone' | 'email', target: string, password: string) {
    const res = await apiLogin({ type, target, password })
    token.value = res.data.token
    user.value = res.data.user
    localStorage.setItem('token', res.data.token)
  }

  async function fetchProfile() {
    if (isMockMode()) {
      user.value = { ...mockUser }
      return
    }
    const res = await getProfile()
    user.value = res.data
  }

  function logout() {
    token.value = ''
    user.value = null
    localStorage.removeItem('token')
    disableMockMode()
    router.push('/login')
  }

  return { token, user, login, fetchProfile, logout }
})
