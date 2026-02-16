<template>
  <div class="profile-page">
    <!-- Profile Info Card -->
    <div class="profile-card">
      <div class="profile-card-avatar">
        <el-avatar :size="72" :src="userStore.user?.avatar_url || undefined" :style="!userStore.user?.avatar_url ? { background: 'linear-gradient(135deg, #356bfb, #7c5cf5)', fontSize: '26px' } : {}">
          {{ userStore.user?.nickname?.charAt(0) || '?' }}
        </el-avatar>
      </div>
      <div class="profile-card-info">
        <h3 class="profile-card-name">{{ userStore.user?.nickname || '未设置昵称' }}</h3>
        <p class="profile-card-desc">{{ userStore.user?.description || '这个人很懒，什么都没写~' }}</p>
        <div class="profile-card-meta">
          <span v-if="userStore.user?.phone"><el-icon><Phone /></el-icon>{{ userStore.user.phone }}</span>
          <span v-if="userStore.user?.email"><el-icon><Message /></el-icon>{{ userStore.user.email }}</span>
          <span v-if="userStore.user?.created_at"><el-icon><Calendar /></el-icon>{{ userStore.user.created_at.slice(0, 10) }} 加入</span>
        </div>
      </div>
    </div>

    <!-- Avatar Section -->
    <div class="section">
      <h4>头像</h4>
      <div class="avatar-section">
        <div class="avatar-wrapper" @click="triggerAvatarUpload">
          <el-avatar :size="80" :src="userStore.user?.avatar_url || undefined" :style="!userStore.user?.avatar_url ? { background: '#409eff', fontSize: '28px' } : {}">
            {{ userStore.user?.nickname?.charAt(0) || '?' }}
          </el-avatar>
          <div class="avatar-overlay">
            <el-icon :size="20"><Camera /></el-icon>
          </div>
        </div>
        <input ref="avatarInput" type="file" accept="image/*" hidden @change="onAvatarChange" />
        <span class="avatar-tip">点击头像更换</span>
      </div>
    </div>

    <!-- Nickname -->
    <div class="section">
      <h4>昵称</h4>
      <div class="inline-edit">
        <el-input v-model="nickname" style="width: 280px" maxlength="20" />
        <el-button type="primary" @click="saveNickname" :loading="saving.nickname">保存</el-button>
      </div>
    </div>

    <!-- Description -->
    <div class="section">
      <h4>个人简介</h4>
      <div class="inline-edit">
        <el-input v-model="description" type="textarea" :rows="3" style="width: 400px" maxlength="200" show-word-limit />
        <el-button type="primary" @click="saveDescription" :loading="saving.description">保存</el-button>
      </div>
    </div>

    <!-- Phone -->
    <div class="section">
      <h4>手机号</h4>
      <div class="bind-info">
        <span v-if="userStore.user?.phone" class="bound">已绑定: {{ userStore.user.phone }}</span>
        <span v-else class="unbound">未绑定</span>
        <el-button size="small" @click="showPhoneDialog = true">
          {{ userStore.user?.phone ? '更换' : '绑定' }}
        </el-button>
      </div>
    </div>

    <!-- Email -->
    <div class="section">
      <h4>邮箱</h4>
      <div class="bind-info">
        <span v-if="userStore.user?.email" class="bound">已绑定: {{ userStore.user.email }}</span>
        <span v-else class="unbound">未绑定</span>
        <el-button size="small" @click="showEmailDialog = true">
          {{ userStore.user?.email ? '更换' : '绑定' }}
        </el-button>
      </div>
    </div>

    <!-- Password -->
    <div class="section">
      <h4>修改密码</h4>
      <el-form label-width="80px" style="max-width: 400px">
        <el-form-item label="旧密码">
          <el-input v-model="pwdForm.old_password" type="password" show-password />
        </el-form-item>
        <el-form-item label="新密码">
          <el-input v-model="pwdForm.new_password" type="password" show-password />
        </el-form-item>
        <el-form-item>
          <el-button type="primary" @click="savePassword" :loading="saving.password">修改密码</el-button>
        </el-form-item>
      </el-form>
    </div>

    <!-- Phone Dialog -->
    <el-dialog v-model="showPhoneDialog" title="绑定手机号" width="420px" align-center>
      <el-form label-width="80px">
        <el-form-item label="手机号">
          <el-input v-model="phoneForm.new_phone" placeholder="请输入新手机号" />
        </el-form-item>
        <el-form-item label="验证码">
          <div class="code-row">
            <el-input v-model="phoneForm.verify_code" placeholder="验证码" />
            <el-button :disabled="phoneCountdown > 0" @click="sendPhoneCode">
              {{ phoneCountdown > 0 ? `${phoneCountdown}s` : '发送' }}
            </el-button>
          </div>
        </el-form-item>
        <el-form-item label="密码">
          <el-input v-model="phoneForm.password" type="password" placeholder="请输入当前密码" show-password />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="showPhoneDialog = false">取消</el-button>
        <el-button type="primary" @click="savePhone" :loading="saving.phone">确认</el-button>
      </template>
    </el-dialog>

    <!-- Danger Zone -->
    <div class="section danger-section">
      <h4>注销账号</h4>
      <p class="danger-tip">注销后，您的所有文件、分享记录将被永久删除且无法恢复。</p>
      <el-button type="danger" @click="showDeleteDialog = true">注销账号</el-button>
    </div>

    <!-- Delete Account Dialog -->
    <el-dialog v-model="showDeleteDialog" title="确认注销账号" width="420px" align-center>
      <el-alert type="error" :closable="false" show-icon style="margin-bottom: 16px">
        此操作不可逆，所有数据将被永久删除！
      </el-alert>
      <el-form label-width="80px">
        <el-form-item label="密码">
          <el-input v-model="deletePassword" type="password" placeholder="请输入当前密码确认注销" show-password />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="showDeleteDialog = false">取消</el-button>
        <el-button type="danger" @click="confirmDeleteAccount" :loading="deletingAccount">确认注销</el-button>
      </template>
    </el-dialog>

    <!-- Email Dialog -->
    <el-dialog v-model="showEmailDialog" title="绑定邮箱" width="420px" align-center>
      <el-form label-width="80px">
        <el-form-item label="邮箱">
          <el-input v-model="emailForm.new_email" placeholder="请输入新邮箱" />
        </el-form-item>
        <el-form-item label="验证码">
          <div class="code-row">
            <el-input v-model="emailForm.verify_code" placeholder="验证码" />
            <el-button :disabled="emailCountdown > 0" @click="sendEmailCode">
              {{ emailCountdown > 0 ? `${emailCountdown}s` : '发送' }}
            </el-button>
          </div>
        </el-form-item>
        <el-form-item label="密码">
          <el-input v-model="emailForm.password" type="password" placeholder="请输入当前密码" show-password />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="showEmailDialog = false">取消</el-button>
        <el-button type="primary" @click="saveEmail" :loading="saving.email">确认</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive, watch, onMounted } from 'vue'
import { Camera, Phone, Message, Calendar } from '@element-plus/icons-vue'
import { useUserStore } from '../stores/user'
import { notifySuccess, notifyWarning } from '../utils/notify'
import {
  updateAvatar, updateNickname, updateDescription,
  updatePassword, updatePhone, updateEmail, getVerifyCode, deleteAccount,
} from '../api/user'

const userStore = useUserStore()

const nickname = ref('')
const description = ref('')
const avatarInput = ref<HTMLInputElement>()

const saving = reactive({
  nickname: false,
  description: false,
  password: false,
  phone: false,
  email: false,
})

const pwdForm = reactive({ old_password: '', new_password: '' })

const showPhoneDialog = ref(false)
const phoneForm = reactive({ new_phone: '', verify_code: '', verify_code_id: '', password: '' })
const phoneCountdown = ref(0)

const showEmailDialog = ref(false)
const emailForm = reactive({ new_email: '', verify_code: '', verify_code_id: '', password: '' })
const emailCountdown = ref(0)

const showDeleteDialog = ref(false)
const deletePassword = ref('')
const deletingAccount = ref(false)

onMounted(() => {
  if (userStore.user) {
    nickname.value = userStore.user.nickname || ''
    description.value = userStore.user.description || ''
  }
})

watch(() => userStore.user, (u) => {
  if (u) {
    nickname.value = u.nickname || ''
    description.value = u.description || ''
  }
})

function triggerAvatarUpload() {
  avatarInput.value?.click()
}

async function onAvatarChange(e: Event) {
  const file = (e.target as HTMLInputElement).files?.[0]
  if (!file) return
  try {
    const res = await updateAvatar(file)
    if (userStore.user) {
      userStore.user.avatar_url = res.data.avatar_url
    }
    notifySuccess('头像更新成功')
  } catch { /* handled */ }
}

async function saveNickname() {
  if (!nickname.value.trim()) {
    notifyWarning('昵称不能为空')
    return
  }
  saving.nickname = true
  try {
    await updateNickname(nickname.value)
    if (userStore.user) userStore.user.nickname = nickname.value
    notifySuccess('昵称已更新')
  } finally {
    saving.nickname = false
  }
}

async function saveDescription() {
  saving.description = true
  try {
    await updateDescription(description.value)
    if (userStore.user) userStore.user.description = description.value
    notifySuccess('简介已更新')
  } finally {
    saving.description = false
  }
}

async function savePassword() {
  if (!pwdForm.old_password || !pwdForm.new_password) {
    notifyWarning('请填写完整')
    return
  }
  saving.password = true
  try {
    await updatePassword(pwdForm.old_password, pwdForm.new_password)
    pwdForm.old_password = ''
    pwdForm.new_password = ''
    notifySuccess('密码已修改')
  } finally {
    saving.password = false
  }
}

function startCountdown(counter: typeof phoneCountdown) {
  counter.value = 60
  const timer = setInterval(() => {
    counter.value--
    if (counter.value <= 0) clearInterval(timer)
  }, 1000)
}

async function sendPhoneCode() {
  if (!phoneForm.new_phone) {
    notifyWarning('请输入手机号')
    return
  }
  const res = await getVerifyCode('phone', phoneForm.new_phone)
  phoneForm.verify_code_id = res.data.verify_code_id
  startCountdown(phoneCountdown)
  notifySuccess('验证码已发送')
}

async function sendEmailCode() {
  if (!emailForm.new_email) {
    notifyWarning('请输入邮箱')
    return
  }
  const res = await getVerifyCode('email', emailForm.new_email)
  emailForm.verify_code_id = res.data.verify_code_id
  startCountdown(emailCountdown)
  notifySuccess('验证码已发送')
}

async function savePhone() {
  if (!phoneForm.new_phone || !phoneForm.verify_code || !phoneForm.password) {
    notifyWarning('请填写完整')
    return
  }
  saving.phone = true
  try {
    await updatePhone({
      new_phone: phoneForm.new_phone,
      verify_code_id: phoneForm.verify_code_id,
      verify_code: phoneForm.verify_code,
      password: phoneForm.password,
    })
    await userStore.fetchProfile()
    showPhoneDialog.value = false
    notifySuccess('手机号已更新')
  } finally {
    saving.phone = false
  }
}

async function saveEmail() {
  if (!emailForm.new_email || !emailForm.verify_code || !emailForm.password) {
    notifyWarning('请填写完整')
    return
  }
  saving.email = true
  try {
    await updateEmail({
      new_email: emailForm.new_email,
      verify_code_id: emailForm.verify_code_id,
      verify_code: emailForm.verify_code,
      password: emailForm.password,
    })
    await userStore.fetchProfile()
    showEmailDialog.value = false
    notifySuccess('邮箱已更新')
  } finally {
    saving.email = false
  }
}

async function confirmDeleteAccount() {
  if (!deletePassword.value) {
    notifyWarning('请输入密码')
    return
  }
  deletingAccount.value = true
  try {
    await deleteAccount(deletePassword.value)
    showDeleteDialog.value = false
    userStore.logout()
  } catch {
    /* handled by interceptor */
  } finally {
    deletingAccount.value = false
  }
}
</script>

<style scoped>
.profile-page {
  max-width: 640px;
  margin: 0 auto;
  animation: fadeIn 0.3s ease-out;
}
.profile-card {
  display: flex;
  align-items: center;
  gap: 20px;
  background: rgba(255, 255, 255, 0.6);
  backdrop-filter: blur(16px) saturate(1.3);
  -webkit-backdrop-filter: blur(16px) saturate(1.3);
  border-radius: 16px;
  border: 1px solid rgba(255, 255, 255, 0.5);
  padding: 28px 32px;
  margin-bottom: 20px;
  box-shadow: 0 4px 16px rgba(0, 0, 0, 0.08);
}
.profile-card-info {
  flex: 1;
  min-width: 0;
}
.profile-card-name {
  margin: 0 0 6px;
  font-size: 20px;
  font-weight: 600;
  color: #1a1a1a;
}
.profile-card-desc {
  margin: 0 0 10px;
  font-size: 13px;
  color: #666;
  line-height: 1.5;
}
.profile-card-meta {
  display: flex;
  flex-wrap: wrap;
  gap: 16px;
  font-size: 12px;
  color: #999;
}
.profile-card-meta span {
  display: flex;
  align-items: center;
  gap: 4px;
}
.section {
  background: rgba(255, 255, 255, 0.5);
  backdrop-filter: blur(16px) saturate(1.3);
  -webkit-backdrop-filter: blur(16px) saturate(1.3);
  border-radius: 12px;
  border: 1px solid rgba(255, 255, 255, 0.4);
  padding: 20px 24px;
  margin-bottom: 12px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
  transition: all 0.25s ease;
}
.section:hover {
  box-shadow: 0 6px 20px rgba(0, 0, 0, 0.1);
  transform: translateY(-1px);
}
.section h4 {
  margin: 0 0 16px;
  font-size: 15px;
  color: #333;
}
.avatar-section {
  display: flex;
  align-items: center;
  gap: 16px;
}
.avatar-wrapper {
  position: relative;
  cursor: pointer;
  border-radius: 50%;
  overflow: hidden;
}
.avatar-overlay {
  position: absolute;
  inset: 0;
  background: rgba(0, 0, 0, 0.4);
  display: flex;
  align-items: center;
  justify-content: center;
  color: #fff;
  opacity: 0;
  transition: opacity 0.2s;
}
.avatar-wrapper:hover .avatar-overlay {
  opacity: 1;
}
.avatar-tip {
  font-size: 13px;
  color: #999;
}
.inline-edit {
  display: flex;
  align-items: flex-start;
  gap: 12px;
}
.bind-info {
  display: flex;
  align-items: center;
  gap: 12px;
}
.bound {
  color: #67c23a;
  font-size: 14px;
}
.unbound {
  color: #999;
  font-size: 14px;
}
.code-row {
  display: flex;
  gap: 8px;
  width: 100%;
}
.code-row .el-input {
  flex: 1;
}
.danger-section {
  border-color: rgba(245, 108, 108, 0.3);
}
.danger-tip {
  margin: 0 0 12px;
  font-size: 13px;
  color: #f56c6c;
}
</style>
