<template>
  <div class="register-page">
    <ParticleBackground />
    <div class="register-card">
      <div class="card-header">
        <div class="logo-mark">
          <el-icon :size="24" color="#fff"><Files /></el-icon>
        </div>
        <h2 class="title">CloudDisk</h2>
        <p class="subtitle">创建你的账号</p>
      </div>
      <el-tabs v-model="activeTab">
        <el-tab-pane label="昵称注册" name="nickname">
          <el-form @submit.prevent="onRegister">
            <el-form-item>
              <el-input
                v-model="nicknameForm.nickname"
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
                @keyup.enter="onRegister"
              />
            </el-form-item>
            <el-form-item>
              <el-button
                type="primary"
                size="large"
                class="register-btn"
                @click="onRegister"
                :loading="loading"
              >
                注册
              </el-button>
            </el-form-item>
          </el-form>
        </el-tab-pane>
        <el-tab-pane label="手机注册" name="phone">
          <div class="unsupported-hint">
            <el-icon :size="32" color="var(--color-text-4)"><Phone /></el-icon>
            <p>手机号注册暂未开放</p>
          </div>
        </el-tab-pane>
        <el-tab-pane label="邮箱注册" name="email">
          <div class="unsupported-hint">
            <el-icon :size="32" color="var(--color-text-4)"><Message /></el-icon>
            <p>邮箱注册暂未开放</p>
          </div>
        </el-tab-pane>
      </el-tabs>
      <div class="footer-link">
        已有账号？<router-link to="/login">立即登录</router-link>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive } from 'vue'
import { useRouter } from 'vue-router'
import { Phone, Lock, Message, User, Files } from '@element-plus/icons-vue'
import { notifyWarning, notifySuccess } from '../utils/notify'
import { register } from '../api/user'
import ParticleBackground from '../components/ParticleBackground.vue'

const router = useRouter()
const activeTab = ref('nickname')
const loading = ref(false)

const nicknameForm = reactive({
  nickname: '',
  password: '',
})

async function onRegister() {
  if (!nicknameForm.nickname || !nicknameForm.password) {
    notifyWarning('请填写完整信息')
    return
  }
  loading.value = true
  try {
    await register({
      type: 'nickname',
      password: nicknameForm.password,
      nickname: nicknameForm.nickname,
    })
    notifySuccess('注册成功，请登录')
    router.push('/login')
  } finally {
    loading.value = false
  }
}
</script>

<style scoped>
.register-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  position: relative;
}

.register-card {
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
    0 20px 60px rgba(0, 0, 0, 0.06),
    0 4px 16px rgba(0, 0, 0, 0.03);
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

.code-row {
  display: flex;
  gap: 12px;
  width: 100%;
}
.code-row .el-input {
  flex: 1;
}

.unsupported-hint {
  text-align: center;
  padding: 32px 0;
  color: var(--color-text-4);
  font-size: 14px;
}
.unsupported-hint p {
  margin: 12px 0 0;
}

.register-btn {
  width: 100%;
  height: 44px;
  font-size: 15px;
  font-weight: 600;
  letter-spacing: 0.3px;
  border: none !important;
  box-shadow: 0 4px 14px rgba(53, 107, 251, 0.25);
  transition: all 0.2s ease;
}
.register-btn:hover {
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
