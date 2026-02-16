<template>
  <el-dialog
    v-model="visible"
    :title="fileName"
    width="80%"
    align-center
    destroy-on-close
    class="preview-dialog"
    @close="onClose"
  >
    <div class="preview-body" v-loading="loading">
      <!-- Image -->
      <img
        v-if="previewType === 'image'"
        :src="previewUrl"
        class="preview-image"
        @load="loading = false"
        @error="onError"
      />

      <!-- Video -->
      <video
        v-else-if="previewType === 'video'"
        :src="previewUrl"
        controls
        class="preview-video"
        @loadeddata="loading = false"
        @error="onError"
      />

      <!-- Audio -->
      <div v-else-if="previewType === 'audio'" class="preview-audio-wrapper">
        <div class="audio-icon">
          <el-icon :size="64" color="#a855f7"><Headset /></el-icon>
        </div>
        <audio
          :src="previewUrl"
          controls
          class="preview-audio"
          @loadeddata="loading = false"
          @error="onError"
        />
      </div>

      <!-- PDF -->
      <iframe
        v-else-if="previewType === 'pdf'"
        :src="previewUrl"
        class="preview-pdf"
        @load="loading = false"
        @error="onError"
      />

      <!-- Text / Code -->
      <div v-else-if="previewType === 'text'" class="preview-text-wrapper">
        <pre class="preview-text"><code>{{ textContent }}</code></pre>
      </div>

      <!-- Load Error -->
      <div v-else-if="previewType === 'error'" class="preview-unsupported">
        <el-icon :size="48" color="#f56c6c"><WarningFilled /></el-icon>
        <p>文件加载失败，请重试或下载查看</p>
        <el-button type="primary" @click="$emit('download')">下载文件</el-button>
      </div>

      <!-- Unsupported -->
      <div v-else-if="previewType === 'unsupported'" class="preview-unsupported">
        <el-icon :size="48" color="#909399"><Document /></el-icon>
        <p>该文件类型暂不支持预览</p>
        <el-button type="primary" @click="$emit('download')">下载文件</el-button>
      </div>
    </div>
  </el-dialog>
</template>

<script setup lang="ts">
import { ref, watch, computed } from 'vue'
import { WarningFilled } from '@element-plus/icons-vue'
import { getFilePreviewUrl } from '../api/file'

const props = defineProps<{
  modelValue: boolean
  fileId: string
  fileName: string
}>()

const emit = defineEmits<{
  'update:modelValue': [val: boolean]
  download: []
}>()

const visible = computed({
  get: () => props.modelValue,
  set: (val) => emit('update:modelValue', val),
})

const loading = ref(true)
const textContent = ref('')
const loadError = ref(false)

const imageExts = ['jpg', 'jpeg', 'png', 'gif', 'bmp', 'svg', 'webp', 'ico']
const videoExts = ['mp4', 'webm', 'ogg']
const audioExts = ['mp3', 'wav', 'ogg', 'aac', 'flac', 'm4a']
const textExts = [
  'txt', 'md', 'json', 'xml', 'yaml', 'yml', 'toml', 'ini', 'conf', 'cfg', 'log',
  'js', 'ts', 'vue', 'jsx', 'tsx', 'html', 'htm', 'css', 'scss', 'less',
  'py', 'go', 'java', 'c', 'cpp', 'h', 'hpp', 'rs', 'rb', 'php', 'sh', 'bat',
  'sql', 'graphql', 'proto', 'env', 'gitignore', 'dockerfile',
]

const ext = computed(() => {
  if (!props.fileName) return ''
  return props.fileName.split('.').pop()?.toLowerCase() || ''
})

const previewType = computed(() => {
  if (!props.fileId) return 'unsupported'
  if (loadError.value) return 'error'
  const e = ext.value
  if (imageExts.includes(e)) return 'image'
  if (videoExts.includes(e)) return 'video'
  if (audioExts.includes(e)) return 'audio'
  if (e === 'pdf') return 'pdf'
  if (textExts.includes(e)) return 'text'
  return 'unsupported'
})

const previewUrl = computed(() => {
  if (!props.fileId) return ''
  return getFilePreviewUrl(props.fileId)
})

watch(() => props.modelValue, (val) => {
  if (val) {
    loading.value = true
    loadError.value = false
    textContent.value = ''
    if (previewType.value === 'text') {
      fetchText()
    } else if (previewType.value === 'unsupported') {
      loading.value = false
    }
  }
})

async function fetchText() {
  try {
    const res = await fetch(previewUrl.value)
    if (!res.ok) throw new Error('fetch failed')
    const text = await res.text()
    // Limit display to 500KB to avoid freezing
    textContent.value = text.length > 512000 ? text.slice(0, 512000) + '\n\n... (文件过大，仅显示前500KB)' : text
  } catch {
    loadError.value = true
  } finally {
    loading.value = false
  }
}

function onError() {
  loading.value = false
  loadError.value = true
}

function onClose() {
  emit('update:modelValue', false)
}
</script>

<style scoped>
.preview-body {
  min-height: 200px;
  display: flex;
  align-items: center;
  justify-content: center;
}

.preview-image {
  max-width: 100%;
  max-height: 70vh;
  border-radius: 8px;
  object-fit: contain;
}

.preview-video {
  max-width: 100%;
  max-height: 70vh;
  border-radius: 8px;
  outline: none;
}

.preview-audio-wrapper {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 24px;
  padding: 40px 0;
}

.audio-icon {
  width: 120px;
  height: 120px;
  border-radius: 50%;
  background: rgba(168, 85, 247, 0.08);
  display: flex;
  align-items: center;
  justify-content: center;
}

.preview-audio {
  width: 100%;
  max-width: 480px;
}

.preview-pdf {
  width: 100%;
  height: 70vh;
  border: none;
  border-radius: 8px;
}

.preview-text-wrapper {
  width: 100%;
  max-height: 70vh;
  overflow: auto;
  background: #fafafa;
  border-radius: 8px;
  border: 1px solid var(--color-border);
}

.preview-text {
  margin: 0;
  padding: 16px;
  font-size: 13px;
  line-height: 1.6;
  font-family: 'Cascadia Code', 'Fira Code', 'JetBrains Mono', Consolas, monospace;
  white-space: pre-wrap;
  word-break: break-all;
  color: #333;
}

.preview-unsupported {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 12px;
  padding: 40px 0;
  color: #909399;
}

.preview-unsupported p {
  margin: 0;
  font-size: 14px;
}
</style>
