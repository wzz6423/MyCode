<template>
  <el-dialog
    v-model="visible"
    :title="isPublic ? '公开文件' : '分享文件'"
    width="480px"
    align-center
    @close="onClose"
  >
    <el-form label-width="100px">
      <el-form-item label="文件名">
        <span>{{ fileName }}</span>
      </el-form-item>
      <el-form-item v-if="!isPublic" label="分享留言">
        <el-input v-model="form.message" placeholder="给好友留句话..." maxlength="200" />
      </el-form-item>
      <el-form-item label="有效期">
        <el-select v-model="form.expire_hours" style="width: 100%">
          <el-option label="1 小时" :value="1" />
          <el-option label="6 小时" :value="6" />
          <el-option label="1 天" :value="24" />
          <el-option label="7 天" :value="168" />
          <el-option label="30 天" :value="720" />
          <el-option label="永久" :value="0" />
        </el-select>
      </el-form-item>
      <el-form-item label="下载次数">
        <div class="count-row">
          <el-switch v-model="unlimited" active-text="不限" inactive-text="" />
          <el-input-number v-if="!unlimited" v-model="form.max_count" :min="0" :max="9999" />
        </div>
      </el-form-item>
      <el-form-item v-if="shareLink" label="分享链接">
        <el-input v-model="shareLink" readonly>
          <template #append>
            <el-button @click="copyLink">复制</el-button>
          </template>
        </el-input>
      </el-form-item>
    </el-form>
    <template #footer>
      <el-button @click="visible = false">取消</el-button>
      <el-button type="primary" @click="onSubmit" :loading="submitting" :disabled="!!shareLink">
        {{ isPublic ? '公开' : '创建分享' }}
      </el-button>
    </template>
  </el-dialog>
</template>

<script setup lang="ts">
import { ref, reactive, watch } from 'vue'
import { shareFile, publicFile } from '../api/file'
import { notifySuccess } from '../utils/notify'

const props = defineProps<{
  modelValue: boolean
  fileId: string
  fileName: string
  isPublic?: boolean
}>()

const emit = defineEmits<{
  'update:modelValue': [val: boolean]
  done: []
}>()

const visible = ref(props.modelValue)
watch(() => props.modelValue, (v) => { visible.value = v })
watch(visible, (v) => { emit('update:modelValue', v) })

const unlimited = ref(true)

const form = reactive({
  message: '',
  expire_hours: 24,
  max_count: 10,
})

const shareLink = ref('')
const submitting = ref(false)

async function onSubmit() {
  submitting.value = true
  const maxCount = unlimited.value ? -1 : form.max_count
  try {
    let res
    if (props.isPublic) {
      res = await publicFile(props.fileId, {
        expire_hours: form.expire_hours,
        max_count: maxCount,
      })
    } else {
      res = await shareFile(props.fileId, {
        message: form.message,
        expire_hours: form.expire_hours,
        max_count: maxCount,
      })
    }
    shareLink.value = `${window.location.origin}/share/${res.data.share_id}`
    notifySuccess(props.isPublic ? '已公开' : '分享成功')
    emit('done')
  } finally {
    submitting.value = false
  }
}

function copyLink() {
  const text = shareLink.value
  if (navigator.clipboard && window.isSecureContext) {
    navigator.clipboard.writeText(text).then(() => {
      notifySuccess('链接已复制')
    })
  } else {
    // Fallback for non-HTTPS contexts
    const textarea = document.createElement('textarea')
    textarea.value = text
    textarea.style.position = 'fixed'
    textarea.style.opacity = '0'
    document.body.appendChild(textarea)
    textarea.select()
    document.execCommand('copy')
    document.body.removeChild(textarea)
    notifySuccess('链接已复制')
  }
}

function onClose() {
  shareLink.value = ''
  form.message = ''
  form.expire_hours = 24
  form.max_count = 10
  unlimited.value = true
}
</script>

<style scoped>
.count-row {
  display: flex;
  align-items: center;
  gap: 12px;
}
</style>
