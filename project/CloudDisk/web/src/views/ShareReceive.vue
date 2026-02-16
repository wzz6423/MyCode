<template>
  <div class="share-receive-page">
    <div class="share-card" v-loading="loading">
      <!-- Error state -->
      <div v-if="error" class="share-error">
        <el-icon :size="48" color="#f56c6c"><CircleCloseFilled /></el-icon>
        <h3>{{ error }}</h3>
        <el-button type="primary" @click="$router.push('/login')">返回登录</el-button>
      </div>

      <!-- Share info -->
      <template v-else-if="shareInfo">
        <div class="share-header">
          <el-icon :size="48" :color="fileTypeColor(shareInfo.file_name)">
            <component :is="fileTypeIcon(shareInfo.file_name)" />
          </el-icon>
          <h2>{{ shareInfo.file_name }}</h2>
          <div class="share-meta">
            <span>{{ formatFileSize(shareInfo.file_size) }}</span>
            <span v-if="shareInfo.expire_at">有效期至 {{ formatDate(shareInfo.expire_at) }}</span>
            <span v-else>永久有效</span>
          </div>
          <div v-if="shareInfo.message" class="share-message">
            <el-icon><ChatDotRound /></el-icon>
            <span>{{ shareInfo.message }}</span>
          </div>
        </div>

        <!-- Not logged in -->
        <div v-if="!isLoggedIn" class="share-actions">
          <p class="login-hint">登录后即可接收此文件</p>
          <el-button type="primary" size="large" @click="goLogin">登录 / 注册</el-button>
        </div>

        <!-- Logged in: folder picker + receive -->
        <div v-else class="share-actions">
          <div class="folder-picker">
            <div class="folder-picker-label">选择保存位置</div>
            <div class="folder-picker-selected">
              <el-icon><FolderOpened /></el-icon>
              <span>{{ savePath }}</span>
            </div>
            <div class="folder-tree-wrapper">
              <div
                class="folder-tree-item root"
                :class="{ active: savePath === '/' }"
                @click="savePath = '/'"
              >
                <el-icon><FolderOpened /></el-icon>
                <span>根目录</span>
              </div>
              <el-tree
                v-if="folderTreeData.length > 0"
                :data="folderTreeData"
                :props="treeProps"
                node-key="file_id"
                :expand-on-click-node="false"
                :highlight-current="true"
                lazy
                :load="loadFolderChildren"
                @node-click="onFolderSelect"
              >
                <template #default="{ data }">
                  <div class="folder-node">
                    <el-icon><Folder /></el-icon>
                    <span>{{ data.file_name }}</span>
                  </div>
                </template>
              </el-tree>
            </div>
          </div>
          <el-button type="primary" size="large" @click="doReceive" :loading="receiving">
            接收文件
          </el-button>
        </div>
      </template>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, computed } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { ElMessageBox } from 'element-plus'
import { FolderOpened, Folder, ChatDotRound, CircleCloseFilled } from '@element-plus/icons-vue'
import { getShareInfo, receiveFile, getFileList } from '../api/file'
import { useUserStore } from '../stores/user'
import { notifySuccess } from '../utils/notify'
import { formatFileSize, formatDate, fileTypeIcon, fileTypeColor } from '../utils/format'
import type { FileItem } from '../types'

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()

const loading = ref(true)
const error = ref('')
const shareInfo = ref<any>(null)
const receiving = ref(false)
const savePath = ref('/')
const folderTreeData = ref<any[]>([])
const treeProps = { label: 'file_name', children: 'children', isLeaf: 'leaf' }

const isLoggedIn = computed(() => !!localStorage.getItem('token'))

const shareId = computed(() => route.params.shareId as string)

onMounted(async () => {
  try {
    const res = await getShareInfo(shareId.value)
    shareInfo.value = res.data
    if (isLoggedIn.value) {
      loadFolderTree()
    }
  } catch (e: any) {
    const msg = e?.response?.data?.message || e?.message || ''
    if (msg.includes('not found')) {
      error.value = '分享不存在或已被取消'
    } else if (msg.includes('expired')) {
      error.value = '分享已过期'
    } else if (msg.includes('maximum')) {
      error.value = '分享已达到最大接收次数'
    } else {
      error.value = '无法获取分享信息'
    }
  } finally {
    loading.value = false
  }
})

function goLogin() {
  router.push(`/login?redirect=/share/${shareId.value}`)
}

async function loadFolderTree() {
  try {
    const res = await getFileList('/', 1, 1000)
    const folders = (res.data.files || []).filter((f: FileItem) => f.is_dir)
    folderTreeData.value = folders.map(f => ({ ...f, leaf: false }))
  } catch { /* ignore */ }
}

async function loadFolderChildren(node: any, resolve: (data: any[]) => void) {
  if (node.level === 0) {
    resolve(folderTreeData.value)
    return
  }
  const parentPath = node.data.parent_path === '/'
    ? '/' + node.data.file_name
    : node.data.parent_path + '/' + node.data.file_name
  try {
    const res = await getFileList(parentPath, 1, 1000)
    const folders = (res.data.files || []).filter((f: FileItem) => f.is_dir)
    resolve(folders.map(f => ({ ...f, leaf: false })))
  } catch {
    resolve([])
  }
}

function onFolderSelect(data: any) {
  const path = data.parent_path === '/'
    ? '/' + data.file_name
    : data.parent_path + '/' + data.file_name
  savePath.value = path
}

async function doReceive() {
  // Same-user check
  if (shareInfo.value?.user_id && shareInfo.value.user_id === userStore.user?.user_id) {
    try {
      await ElMessageBox.confirm('这是你自己的文件，确定要接收一份副本吗？', '提示', {
        confirmButtonText: '确定接收',
        cancelButtonText: '取消',
        type: 'warning',
      })
    } catch { return }
  }

  // Duplicate filename check
  let finalFileName = ''
  try {
    const listRes = await getFileList(savePath.value, 1, 1000)
    const existing = (listRes.data.files || []).find(
      (f: FileItem) => f.file_name === shareInfo.value?.file_name
    )
    if (existing) {
      const result = await ElMessageBox.prompt(
        `目标目录已存在同名文件「${shareInfo.value.file_name}」，请修改文件名`,
        '文件名冲突',
        {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          inputValue: shareInfo.value.file_name,
          inputPattern: /^[^/\\:*?"<>|]+$/,
          inputErrorMessage: '文件名不合法',
        }
      ) as any
      finalFileName = result?.value ?? result
    }
  } catch (e: any) {
    if (e === 'cancel' || e?.action === 'cancel') return
  }

  receiving.value = true
  try {
    await receiveFile(shareId.value, savePath.value, finalFileName || undefined)
    notifySuccess('文件接收成功')
    router.push('/files')
  } finally {
    receiving.value = false
  }
}
</script>

<style scoped>
.share-receive-page {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  background: linear-gradient(135deg, #e8f0fe 0%, #f3e8ff 50%, #fce7f3 100%);
  padding: 20px;
}

.share-card {
  background: rgba(255, 255, 255, 0.7);
  backdrop-filter: blur(20px) saturate(1.4);
  -webkit-backdrop-filter: blur(20px) saturate(1.4);
  border-radius: 16px;
  border: 1px solid rgba(255, 255, 255, 0.5);
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.08);
  padding: 40px;
  width: 100%;
  max-width: 460px;
  min-height: 200px;
}

.share-error {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 16px;
  text-align: center;
}

.share-error h3 {
  margin: 0;
  color: #606266;
  font-weight: 500;
}

.share-header {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 12px;
  margin-bottom: 28px;
}

.share-header h2 {
  margin: 0;
  font-size: 18px;
  color: #333;
  word-break: break-all;
  text-align: center;
}

.share-meta {
  display: flex;
  gap: 16px;
  font-size: 13px;
  color: #909399;
}

.share-message {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 10px 16px;
  background: rgba(53, 107, 251, 0.06);
  border-radius: 8px;
  font-size: 14px;
  color: #606266;
  width: 100%;
}

.share-actions {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 16px;
}

.login-hint {
  margin: 0;
  font-size: 14px;
  color: #909399;
}

.folder-picker {
  width: 100%;
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.folder-picker-label {
  font-size: 14px;
  color: #606266;
  font-weight: 500;
}

.folder-picker-selected {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 12px;
  background: rgba(53, 107, 251, 0.08);
  border-radius: 8px;
  color: var(--color-primary);
  font-size: 14px;
  font-weight: 500;
}

.folder-tree-wrapper {
  max-height: 200px;
  overflow-y: auto;
  border: 1px solid var(--color-border);
  border-radius: 8px;
  padding: 6px;
  background: #fafafa;
}

.folder-tree-item.root {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 6px 10px;
  border-radius: 6px;
  cursor: pointer;
  font-size: 14px;
  color: #333;
  transition: background 0.2s;
  margin-bottom: 2px;
}

.folder-tree-item.root:hover {
  background: rgba(53, 107, 251, 0.06);
}

.folder-tree-item.root.active {
  background: rgba(53, 107, 251, 0.1);
  color: var(--color-primary);
  font-weight: 500;
}

.folder-node {
  display: flex;
  align-items: center;
  gap: 6px;
  font-size: 14px;
}

:deep(.el-tree) {
  background: transparent !important;
}

:deep(.el-tree-node__content) {
  border-radius: 6px;
  height: 32px;
}

:deep(.el-tree-node__content:hover) {
  background: rgba(53, 107, 251, 0.06) !important;
}

:deep(.el-tree--highlight-current .el-tree-node.is-current > .el-tree-node__content) {
  background: rgba(53, 107, 251, 0.1) !important;
  color: var(--color-primary);
}
</style>
