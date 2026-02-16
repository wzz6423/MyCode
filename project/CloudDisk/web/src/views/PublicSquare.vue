<template>
  <div class="public-square">
    <div class="page-header">
      <h3>共享文件</h3>
      <el-input
        v-model="keyword"
        placeholder="搜索公共文件..."
        clearable
        style="width: 280px"
        :prefix-icon="Search"
        @keyup.enter="onSearch"
        @clear="loadFiles"
      />
    </div>

    <div v-loading="loading" class="file-grid">
      <el-empty v-if="!loading && files.length === 0" description="暂无公共文件" />
      <div
        v-for="file in files"
        :key="file.file_id"
        class="file-card"
      >
        <div class="card-icon">
          <el-icon :size="40" :color="fileTypeColor(file.file_name)">
            <component :is="fileTypeIcon(file.file_name)" />
          </el-icon>
        </div>
        <div class="card-info">
          <div class="card-name" :title="file.file_name">{{ file.file_name }}</div>
          <div class="card-owner" v-if="file.owner_nickname">
            <el-avatar :size="20" :src="file.owner_avatar || ''" />
            <span>{{ file.owner_nickname }}</span>
          </div>
          <div class="card-meta">
            <span>{{ formatFileSize(file.file_size) }}</span>
            <span>{{ formatDate(file.created_at) }}</span>
          </div>
        </div>
        <div class="card-actions">
          <el-button type="primary" size="small" @click="onReceive(file)">
            <el-icon><Download /></el-icon>接收文件
          </el-button>
        </div>
      </div>
    </div>

    <div class="pagination" v-if="total > pageSize">
      <el-pagination
        v-model:current-page="page"
        :page-size="pageSize"
        :total="total"
        layout="total, prev, pager, next"
        @current-change="loadFiles"
      />
    </div>

    <!-- Receive dialog -->
    <el-dialog v-model="showReceive" title="接收文件" width="420px" align-center @open="loadFolderTree">
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
            <template #default="{ node, data }">
              <div class="folder-node">
                <el-icon><Folder /></el-icon>
                <span>{{ data.file_name }}</span>
              </div>
            </template>
          </el-tree>
          <el-empty v-if="!folderTreeLoading && folderTreeData.length === 0" description="暂无子文件夹" :image-size="60" />
          <div v-if="folderTreeLoading" class="folder-tree-loading">
            <el-icon class="is-loading"><Loading /></el-icon>
            <span>加载中...</span>
          </div>
        </div>
      </div>
      <template #footer>
        <el-button @click="showReceive = false">取消</el-button>
        <el-button type="primary" @click="doReceive" :loading="receiving">确认接收</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { Search, FolderOpened, Folder, Loading } from '@element-plus/icons-vue'
import { ElMessageBox } from 'element-plus'
import { getPublicFiles, getFileList, receiveFile } from '../api/file'
import { useUserStore } from '../stores/user'
import { notifySuccess, notifyWarning } from '../utils/notify'
import { formatFileSize, formatDate, fileTypeIcon, fileTypeColor } from '../utils/format'
import type { FileItem } from '../types'

const userStore = useUserStore()

const files = ref<FileItem[]>([])
const loading = ref(false)
const keyword = ref('')
const page = ref(1)
const pageSize = ref(20)
const total = ref(0)

const showReceive = ref(false)
const savePath = ref('/')
const receiving = ref(false)
const currentFile = ref<FileItem | null>(null)

const folderTreeData = ref<any[]>([])
const folderTreeLoading = ref(false)
const treeProps = { label: 'file_name', children: 'children', isLeaf: 'leaf' }

async function loadFolderTree() {
  savePath.value = '/'
  folderTreeData.value = []
  folderTreeLoading.value = true
  try {
    const res = await getFileList('/', 1, 1000)
    const folders = (res.data.files || []).filter((f: FileItem) => f.is_dir)
    folderTreeData.value = folders.map(f => ({ ...f, leaf: false }))
  } finally {
    folderTreeLoading.value = false
  }
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

onMounted(() => {
  loadFiles()
})

async function loadFiles() {
  loading.value = true
  try {
    const res = await getPublicFiles(page.value, pageSize.value)
    files.value = res.data.files || []
    total.value = res.data.total || 0
  } finally {
    loading.value = false
  }
}

function onSearch() {
  page.value = 1
  loadFiles()
}

function onReceive(file: FileItem) {
  currentFile.value = file
  savePath.value = '/'
  showReceive.value = true
}

async function doReceive() {
  if (!currentFile.value?.public_id) {
    notifyWarning('无效的分享')
    return
  }

  // Same-user check
  if (currentFile.value.user_id && currentFile.value.user_id === userStore.user?.user_id) {
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
      (f: FileItem) => f.file_name === currentFile.value!.file_name
    )
    if (existing) {
      const result = await ElMessageBox.prompt(
        `目标目录已存在同名文件「${currentFile.value.file_name}」，请修改文件名`,
        '文件名冲突',
        {
          confirmButtonText: '确定',
          cancelButtonText: '取消',
          inputValue: currentFile.value.file_name,
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
    await receiveFile(currentFile.value.public_id, savePath.value, finalFileName || undefined)
    notifySuccess('文件接收成功')
    showReceive.value = false
  } finally {
    receiving.value = false
  }
}
</script>

<style scoped>
.public-square {
  padding: 0;
}
.page-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 20px;
  background: rgba(255, 255, 255, 0.5);
  backdrop-filter: blur(16px) saturate(1.3);
  -webkit-backdrop-filter: blur(16px) saturate(1.3);
  border-radius: 10px;
  border: 1px solid rgba(255, 255, 255, 0.4);
  margin-bottom: 12px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
}
.page-header h3 {
  margin: 0;
  font-size: 18px;
  color: #333;
}
.file-grid {
  display: grid;
  grid-template-columns: repeat(auto-fill, minmax(280px, 1fr));
  gap: 16px;
  min-height: 200px;
}
.file-card {
  background: rgba(255, 255, 255, 0.45);
  backdrop-filter: blur(16px) saturate(1.3);
  -webkit-backdrop-filter: blur(16px) saturate(1.3);
  border-radius: 12px;
  padding: 20px;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 12px;
  transition: all 0.25s ease;
  border: 1px solid rgba(255, 255, 255, 0.5);
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
}
.file-card:hover {
  box-shadow: 0 6px 20px rgba(0, 0, 0, 0.1);
  transform: translateY(-2px);
}
.card-icon {
  padding: 12px;
}
.card-info {
  text-align: center;
  width: 100%;
}
.card-name {
  font-size: 14px;
  font-weight: 500;
  color: #333;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
.card-owner {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  margin-top: 6px;
  font-size: 12px;
  color: #666;
}
.card-owner .el-avatar {
  flex-shrink: 0;
}
.card-meta {
  display: flex;
  justify-content: center;
  gap: 16px;
  margin-top: 6px;
  font-size: 12px;
  color: #999;
}
.card-actions {
  margin-top: 4px;
}
.pagination {
  display: flex;
  justify-content: center;
  padding: 16px 0;
}

.folder-picker {
  display: flex;
  flex-direction: column;
  gap: 10px;
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
  max-height: 280px;
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
.folder-tree-loading {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  padding: 16px;
  color: #999;
  font-size: 13px;
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
