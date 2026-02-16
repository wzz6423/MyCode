<template>
  <div class="login-page">
    <ParticleBackground />
    <div class="login-card">
      <div class="card-header">
        <div class="logo-mark">
          <el-icon :size="24" color="#fff"><Files /></el-icon>
        </div>
        <h2 class="title">CloudDisk</h2>
        <p class="subtitle">登录你的账号</p>
      </div>
      <el-tabs v-model="activeTab" class="login-tabs">
        <el-tab-pane label="昵称登录" name="nickname">
          <el-form @submit.prevent="onLogin">
            <el-form-item>
              <el-input
                v-model="nicknameForm.target"
                placeholder="请输入昵称"
                :prefix-icon="User"
                size="large"
              />
            </el-form-item>
            <el-form-item>
              <el-input
                v-model="nicknameForm.password"
                type="password"
                placeholder="请输入密码"
                :prefix-icon="Lock"
                size="large"
                show-password
                @keyup.enter="onLogin"
              />
            </el-form-item>
            <el-form-item>
              <el-button
                type="primary"
                size="large"
                class="login-btn"
                @click="onLogin"
                :loading="loading"
              >
                登录
              </el-button>
            </el-form-item>
          </el-form>
        </el-tab-pane>
        <el-tab-pane label="手机登录" name="phone">
          <el-form @submit.prevent="onLogin">
            <el-form-item>
              <el-input
                v-model="phoneForm.target"
                placeholder="请输入手机号"
                :prefix-icon="Phone"
                size="large"
              />
            </el-form-item>
            <el-form-item>
              <el-input
                v-model="phoneForm.password"
                type="password"
                placeholder="请输入密码"
                :prefix-icon="Lock"
                size="large"
                show-password
                @keyup.enter="onLogin"
              />
            </el-form-item>
            <el-form-item>
              <el-button
                type="primary"
                size="large"
                class="login-btn"
                @click="onLogin"
                :loading="loading"
              >
                登录
              </el-button>
            </el-form-item>
          </el-form>
        </el-tab-pane>
        <el-tab-pane label="邮箱登录" name="email">
          <el-form @submit.prevent="onLogin">
            <el-form-item>
              <el-input
                v-model="emailForm.target"
                placeholder="请输入邮箱"
                :prefix-icon="Message"
                size="large"
              />
            </el-form-item>
            <el-form-item>
              <el-input
                v-model="emailForm.password"
                type="password"
                placeholder="请输入密码"
                :prefix-icon="Lock"
                size="large"
                show-password
                @keyup.enter="onLogin"
              />
            </el-form-item>
            <el-form-item>
              <el-button
                type="primary"
                size="large"
                class="login-btn"
                @click="onLogin"
                :loading="loading"
              >
                登录
              </el-button>
            </el-form-item>
          </el-form>
        </el-tab-pane>
      </el-tabs>
      <div class="footer-link">
        还没有账号？<router-link to="/register">立即注册</router-link>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import { Phone, Lock, Message, User, Files } from '@element-plus/icons-vue'
import { useUserStore } from '../stores/user'
import { enableMockMode } from '../utils/mock'
import { notifySuccess, notifyWarning } from '../utils/notify'
import ParticleBackground from '../components/ParticleBackground.vue'

const router = useRouter()
const route = useRoute()
const userStore = useUserStore()

const activeTab = ref('nickname')
const loading = ref(false)

const nicknameForm = reactive({ target: '', password: '' })
const phoneForm = reactive({ target: '', password: '' })
const emailForm = reactive({ target: '', password: '' })

const formMap: Record<string, { target: string; password: string }> = {
  nickname: nicknameForm,
  phone: phoneForm,
  email: emailForm,
}

async function onLogin() {
  const tab = activeTab.value as 'nickname' | 'phone' | 'email'
  const form = formMap[tab]
  if (!form.target || !form.password) {
    notifyWarning('请填写完整信息')
    return
  }
  if (form.target === 'test' && form.password === 'test') {
    enableMockMode()
    notifySuccess('已进入演示模式')
    router.push((route.query.redirect as string) || '/files')
    return
  }
  loading.value = true
  try {
    await userStore.login(tab, form.target, form.password)
    notifySuccess('登录成功')
    router.push((route.query.redirect as string) || '/files')
  } finally {
    loading.value = false
  }
}
</script>

<style scoped>
.login-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  position: relative;
}

.login-card {
  width: 400px;
  padding: 40px 36px 32px;
  position: relative;
  z-index: 1;
  background: rgba(255, 255, 255, 0.65);
  backdrop-filter: blur(24px);
  -webkit-backdrop-filter: blur(24px);
  border-radius: 20px;
  border: 1px solid rgba(255, 255, 255, 0.7);
  box-shadow:
    0 20px 60px rgba(0, 0, 0, 0.08),
    0 4px 16px rgba(0, 0, 0, 0.04);
  animation: cardIn 0.5s cubic-bezier(0.16, 1, 0.3, 1);
}

.card-header {
  text-align: center;
  margin-bottom: 28px;
}

.logo-mark {
  width: 48px;
  height: 48px;
  border-radius: 14px;
  background: linear-gradient(135deg, #356bfb, #7c5cf5);
  display: inline-flex;
  align-items: center;
  justify-content: center;
  box-shadow: 0 4px 12px rgba(53, 107, 251, 0.25);
  margin-bottom: 16px;
}

.title {
  margin: 0;
  font-size: 24px;
  font-weight: 700;
  color: var(--color-text-1);
  letter-spacing: -0.5px;
}

.subtitle {
  margin: 6px 0 0;
  font-size: 14px;
  color: var(--color-text-3);
}

.login-btn {
  width: 100%;
  height: 44px;
  font-size: 15px;
  font-weight: 600;
  letter-spacing: 0.3px;
  border: none !important;
  box-shadow: 0 4px 14px rgba(53, 107, 251, 0.25);
  transition: all 0.2s ease;
}
.login-btn:hover {
  transform: translateY(-1px);
  box-shadow: 0 6px 20px rgba(53, 107, 251, 0.3);
}

.footer-link {
  text-align: center;
  margin-top: 20px;
  font-size: 13px;
  color: var(--color-text-3);
}
.footer-link a {
  color: var(--color-primary);
  text-decoration: none;
  font-weight: 500;
}
.footer-link a:hover {
  text-decoration: underline;
}

:deep(.el-input__wrapper) {
  height: 44px;
}

@keyframes cardIn {
  from {
    opacity: 0;
    transform: translateY(20px) scale(0.98);
  }
  to {
    opacity: 1;
    transform: translateY(0) scale(1);
  }
}
</style>
