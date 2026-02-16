<template>
  <div class="file-upload">
    <el-upload
      drag
      multiple
      :auto-upload="false"
      :show-file-list="false"
      :on-change="onFileChange"
    >
      <el-icon :size="48" color="#c0c4cc"><UploadFilled /></el-icon>
      <div class="el-upload__text">将文件拖到此处，或<em>点击上传</em></div>
    </el-upload>
    <div v-if="fileQueue.length" class="upload-list">
      <div v-for="(item, idx) in fileQueue" :key="idx" class="upload-item">
        <div class="upload-info">
          <el-icon><Document /></el-icon>
          <span class="upload-name">{{ item.file.name }}</span>
          <span class="upload-size">{{ formatFileSize(item.file.size) }}</span>
        </div>
        <el-progress
          :percentage="item.progress"
          :status="item.status === 'success' ? 'success' : item.status === 'error' ? 'exception' : undefined"
          :stroke-width="4"
        />
      </div>
    </div>
    <div v-if="fileQueue.length" class="upload-actions">
      <el-button type="primary" @click="startUpload" :loading="uploading">
        开始上传
      </el-button>
      <el-button @click="clearQueue">清空列表</el-button>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref } from 'vue'
import type { UploadFile } from 'element-plus'
import { uploadFile } from '../api/file'
import { formatFileSize } from '../utils/format'

interface QueueItem {
  file: File
  progress: number
  status: 'pending' | 'uploading' | 'success' | 'error'
}

const props = defineProps<{
  parentPath: string
}>()

const emit = defineEmits<{
  done: []
}>()

const fileQueue = ref<QueueItem[]>([])
const uploading = ref(false)

function onFileChange(uploadFile: UploadFile) {
  if (uploadFile.raw) {
    fileQueue.value.push({
      file: uploadFile.raw,
      progress: 0,
      status: 'pending',
    })
  }
}

async function startUpload() {
  uploading.value = true
  for (const item of fileQueue.value) {
    if (item.status === 'success') continue
    item.status = 'uploading'
    try {
      await uploadFile(item.file, props.parentPath, '', (percent) => {
        item.progress = percent
      })
      item.status = 'success'
      item.progress = 100
    } catch {
      item.status = 'error'
    }
  }
  uploading.value = false
  emit('done')
}

function clearQueue() {
  fileQueue.value = []
}
</script>

<style scoped>
.upload-list {
  margin-top: 16px;
  max-height: 240px;
  overflow-y: auto;
}
.upload-item {
  padding: 8px 0;
  border-bottom: 1px solid #f0f0f0;
}
.upload-info {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 4px;
}
.upload-name {
  flex: 1;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
  font-size: 13px;
}
.upload-size {
  color: #999;
  font-size: 12px;
}
.upload-actions {
  margin-top: 16px;
  text-align: right;
}
</style>
