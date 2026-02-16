<template>
  <div class="file-manager">
    <!-- Breadcrumb -->
    <div class="breadcrumb-bar">
      <el-breadcrumb separator="/">
        <el-breadcrumb-item
          v-for="(seg, idx) in pathSegments"
          :key="idx"
          @click="navigateTo(idx)"
        >
          <span class="breadcrumb-link">{{ seg.label }}</span>
        </el-breadcrumb-item>
      </el-breadcrumb>
    </div>

    <!-- Toolbar -->
    <div class="toolbar">
      <div class="toolbar-left">
        <el-button type="primary" @click="showUpload = true">
          <el-icon><Upload /></el-icon>上传文件
        </el-button>
        <el-button @click="onCreateFolder">
          <el-icon><FolderAdd /></el-icon>新建文件夹
        </el-button>
        <el-select
          v-model="filterStatus"
          placeholder="全部文件"
          clearable
          style="width: 130px"
          @change="onFilterChange"
        >
          <el-option label="全部文件" value="" />
          <el-option label="已分享" value="shared" />
          <el-option label="已公开" value="public" />
        </el-select>
        <el-date-picker
          v-model="filterDateRange"
          type="daterange"
          range-separator="至"
          start-placeholder="开始日期"
          end-placeholder="结束日期"
          value-format="YYYY-MM-DDTHH:mm:ssZ"
          style="width: 280px"
          :disabled-date="disableDate"
          @change="onFilterChange"
        />
      </div>
      <div class="toolbar-right">
        <el-input
          v-model="searchKeyword"
          placeholder="搜索当前目录..."
          clearable
          style="width: 240px"
          :prefix-icon="Search"
          @keyup.enter="onSearch"
          @clear="onClearSearch"
        />
      </div>
    </div>

    <!-- Batch Action Bar -->
    <transition name="batch-slide">
      <div v-if="selectedFiles.length > 0" class="batch-bar">
        <span class="batch-info">已选择 {{ selectedFiles.length }} 项</span>
        <div class="batch-actions">
          <el-button size="small" type="primary" @click="onBatchDownload">
            <el-icon><Download /></el-icon>批量下载
          </el-button>
          <el-button size="small" type="primary" @click="onBatchShare">
            <el-icon><Share /></el-icon>批量分享
          </el-button>
          <el-button size="small" type="warning" @click="onBatchPublic">
            <el-icon><Upload /></el-icon>批量公开
          </el-button>
          <el-button size="small" type="danger" @click="onBatchDelete">
            <el-icon><Delete /></el-icon>批量删除
          </el-button>
        </div>
      </div>
    </transition>

    <!-- File List -->
    <FileList
      :files="fileStore.files"
      :loading="fileStore.loading"
      @open-dir="onOpenDir"
      @preview="onPreview"
      @download="onDownload"
      @share="onShareClick"
      @unshare="onUnshare"
      @delete="onDelete"
      @toggle-public="onTogglePublic"
      @selection-change="onSelectionChange"
    />

    <!-- Pagination -->
    <div class="pagination" v-if="fileStore.total > fileStore.size">
      <el-pagination
        v-model:current-page="fileStore.page"
        :page-size="fileStore.size"
        :total="fileStore.total"
        layout="total, prev, pager, next"
        @current-change="onPageChange"
      />
    </div>

    <!-- Upload Dialog -->
    <el-dialog v-model="showUpload" title="上传文件" width="520px" align-center>
      <FileUpload :parent-path="fileStore.currentPath" @done="onUploadDone" />
    </el-dialog>

    <!-- Share Dialog -->
    <ShareDialog
      v-model="showShare"
      :file-id="shareTarget.file_id"
      :file-name="shareTarget.file_name"
      @done="loadFiles"
    />

    <!-- Public Dialog -->
    <ShareDialog
      v-model="showPublic"
      :file-id="shareTarget.file_id"
      :file-name="shareTarget.file_name"
      :is-public="true"
      @done="loadFiles"
    />

    <!-- File Preview -->
    <FilePreview
      v-model="showPreview"
      :file-id="previewFile.file_id"
      :file-name="previewFile.file_name"
      @download="onDownload({ file_id: previewFile.file_id, file_name: previewFile.file_name } as FileItem)"
    />
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, watch } from 'vue'
import { useRoute } from 'vue-router'
import { Search, Download, Share, Delete } from '@element-plus/icons-vue'
import { ElMessageBox } from 'element-plus'
import { notifySuccess, notifyInfo } from '../utils/notify'
import { useFileStore } from '../stores/file'
import { downloadFile, deleteFile, publicFile, unpublicFile, unshareFile, createFolder, batchDownload, shareFile } from '../api/file'
import type { FileItem } from '../types'
import FileList from '../components/FileList.vue'
import FileUpload from '../components/FileUpload.vue'
import ShareDialog from '../components/ShareDialog.vue'
import FilePreview from '../components/FilePreview.vue'

const route = useRoute()
const fileStore = useFileStore()

const showUpload = ref(false)
const showShare = ref(false)
const showPublic = ref(false)
const shareTarget = ref<{ file_id: string; file_name: string }>({ file_id: '', file_name: '' })
const searchKeyword = ref('')
const selectedFiles = ref<FileItem[]>([])
const filterStatus = ref('')
const filterDateRange = ref<[string, string] | null>(null)
const showPreview = ref(false)
const previewFile = ref<{ file_id: string; file_name: string }>({ file_id: '', file_name: '' })

const epochStart = new Date(0)
function disableDate(date: Date) {
  return date > new Date() || date < epochStart
}

function onSelectionChange(files: FileItem[]) {
  selectedFiles.value = files
}

async function onBatchDelete() {
  if (selectedFiles.value.length === 0) return
  try {
    await ElMessageBox.confirm(
      `确定删除选中的 ${selectedFiles.value.length} 个文件？`,
      '批量删除',
      { type: 'warning', confirmButtonText: '删除', cancelButtonText: '取消', confirmButtonClass: 'el-button--danger' }
    )
    for (const file of selectedFiles.value) {
      await deleteFile(file.file_id)
    }
    notifySuccess(`已删除 ${selectedFiles.value.length} 个文件`)
    selectedFiles.value = []
    loadFiles()
  } catch { /* cancelled */ }
}

function onBatchDownload() {
  if (selectedFiles.value.length === 0) return
  batchDownload(selectedFiles.value.map(f => f.file_id))
  notifySuccess(`已开始打包下载 ${selectedFiles.value.length} 个文件`)
}

async function onBatchShare() {
  if (selectedFiles.value.length === 0) return
  if (selectedFiles.value.length === 1) {
    onShareClick(selectedFiles.value[0])
    return
  }
  const realFiles = selectedFiles.value
  if (realFiles.length === 0) return
  try {
    const result = await ElMessageBox.prompt(
      `将为 ${realFiles.length} 个文件/文件夹分别创建分享链接`,
      '批量分享',
      { confirmButtonText: '分享', cancelButtonText: '取消', inputPlaceholder: '留言（可选）' }
    ) as any
    const message = result?.value ?? result ?? ''
    const shareIds: string[] = []
    for (const file of realFiles) {
      const res = await shareFile(file.file_id, { message, expire_hours: 72, max_count: 0 })
      shareIds.push(res.data.share_id)
    }
    const links = shareIds.map(id => `${location.origin}/share/${id}`).join('\n')
    await ElMessageBox.alert(links, `已创建 ${shareIds.length} 个分享链接`, {
      confirmButtonText: '关闭',
      dangerouslyUseHTMLString: false,
    })
    loadFiles()
  } catch { /* cancelled */ }
}

async function onBatchPublic() {
  const realFiles = selectedFiles.value.filter(f => !f.public_id)
  if (realFiles.length === 0) {
    notifyInfo('所选文件均已公开')
    return
  }
  try {
    await ElMessageBox.confirm(
      `将公开 ${realFiles.length} 个文件，确定继续？`,
      '批量公开',
      { confirmButtonText: '公开', cancelButtonText: '取消' }
    )
    for (const file of realFiles) {
      await publicFile(file.file_id, { expire_hours: 0, max_count: 0 })
    }
    notifySuccess(`已公开 ${realFiles.length} 个文件`)
    selectedFiles.value = []
    loadFiles()
  } catch { /* cancelled */ }
}

const pathSegments = computed(() => {
  const parts = fileStore.currentPath.split('/').filter(Boolean)
  const segments = [{ label: '全部文件', path: '/' }]
  let acc = '/'
  for (const p of parts) {
    acc += p + '/'
    segments.push({ label: p, path: acc })
  }
  return segments
})

onMounted(() => {
  const kw = route.query.keyword as string
  if (kw) {
    searchKeyword.value = kw
    fileStore.search(kw)
  } else {
    loadFiles()
  }
})

watch(() => route.query.keyword, (kw) => {
  if (kw) {
    searchKeyword.value = kw as string
    fileStore.search(kw as string)
  }
})

function loadFiles() {
  fileStore.fetchFiles()
}

function navigateTo(idx: number) {
  const seg = pathSegments.value[idx]
  searchKeyword.value = ''
  fileStore.fetchFiles(seg.path)
}

function onOpenDir(file: FileItem) {
  const newPath = fileStore.currentPath === '/'
    ? `/${file.file_name}/`
    : `${fileStore.currentPath}${file.file_name}/`
  searchKeyword.value = ''
  fileStore.fetchFiles(newPath)
}

function onDownload(file: FileItem) {
  downloadFile(file.file_id)
}

function onShareClick(file: FileItem) {
  shareTarget.value = { file_id: file.file_id, file_name: file.file_name }
  showShare.value = true
}

function onPreview(file: FileItem) {
  previewFile.value = { file_id: file.file_id, file_name: file.file_name }
  showPreview.value = true
}

async function onTogglePublic(file: FileItem) {
  if (file.public_id) {
    try {
      await ElMessageBox.confirm(`确定取消公开「${file.file_name}」？`, '取消公开', {
        type: 'warning',
        confirmButtonText: '确定',
        cancelButtonText: '取消',
      })
      await unpublicFile(file.file_id)
      notifySuccess('已取消公开')
      loadFiles()
    } catch { /* cancelled */ }
  } else {
    shareTarget.value = { file_id: file.file_id, file_name: file.file_name }
    showPublic.value = true
  }
}

async function onDelete(file: FileItem) {
  try {
    await ElMessageBox.confirm(`确定删除「${file.file_name}」？`, '删除确认', {
      type: 'warning',
      confirmButtonText: '删除',
      cancelButtonText: '取消',
      confirmButtonClass: 'el-button--danger',
    })
    await deleteFile(file.file_id)
    notifySuccess('删除成功')
    loadFiles()
  } catch { /* cancelled */ }
}

async function onCreateFolder() {
  try {
    const result = await ElMessageBox.prompt('请输入文件夹名称', '新建文件夹', {
      confirmButtonText: '创建',
      cancelButtonText: '取消',
      inputPattern: /^[^/\\:*?"<>|]+$/,
      inputErrorMessage: '文件夹名称不合法',
    }) as any
    const value = result?.value ?? result
    if (value) {
      await createFolder(value, fileStore.currentPath)
      notifySuccess('文件夹创建成功')
      loadFiles()
    }
  } catch { /* cancelled */ }
}

function onSearch() {
  if (searchKeyword.value) {
    fileStore.search(searchKeyword.value)
  } else {
    loadFiles()
  }
}

function onClearSearch() {
  loadFiles()
}

function onFilterChange() {
  fileStore.setFilter({
    status: filterStatus.value || undefined,
    start_time: filterDateRange.value?.[0] || undefined,
    end_time: filterDateRange.value?.[1] || undefined,
  })
}

async function onUnshare(file: FileItem) {
  try {
    await ElMessageBox.confirm(`确定取消分享「${file.file_name}」？`, '取消分享', {
      type: 'warning',
      confirmButtonText: '确定',
      cancelButtonText: '取消',
    })
    await unshareFile(file.file_id)
    notifySuccess('已取消分享')
    loadFiles()
  } catch { /* cancelled */ }
}

function onPageChange() {
  if (searchKeyword.value) {
    fileStore.search(searchKeyword.value)
  } else {
    loadFiles()
  }
}

function onUploadDone() {
  showUpload.value = false
  loadFiles()
}
</script>

<style scoped>
.file-manager {
  padding: 0;
  animation: fadeIn 0.3s ease-out;
}
.breadcrumb-bar {
  padding: 16px 20px;
  background: rgba(255, 255, 255, 0.5);
  backdrop-filter: blur(16px) saturate(1.3);
  -webkit-backdrop-filter: blur(16px) saturate(1.3);
  border-radius: 10px;
  border: 1px solid rgba(255, 255, 255, 0.4);
  margin-bottom: 12px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
}
.breadcrumb-link {
  cursor: pointer;
  color: #606266;
}
.breadcrumb-link:hover {
  color: var(--color-primary);
}
.toolbar {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 20px;
  background: rgba(255, 255, 255, 0.5);
  backdrop-filter: blur(16px) saturate(1.3);
  -webkit-backdrop-filter: blur(16px) saturate(1.3);
  border-radius: 10px;
  border: 1px solid rgba(255, 255, 255, 0.4);
  margin-bottom: 12px;
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
}
.toolbar-left {
  display: flex;
  gap: 8px;
}
.toolbar-right {
  display: flex;
  gap: 8px;
}
.pagination {
  display: flex;
  justify-content: center;
  padding: 16px 0;
}
:deep(.el-table) {
  border-radius: 8px;
  background: transparent !important;
  color: #1a1a1a !important;
}
:deep(.el-table tr) {
  background: transparent !important;
}
:deep(.el-table th.el-table__cell) {
  background: rgba(255, 255, 255, 0.5) !important;
  color: #1a1a1a !important;
  font-weight: 600;
  border-bottom: none !important;
}
:deep(.el-table td.el-table__cell) {
  background: rgba(255, 255, 255, 0.35) !important;
  color: #1a1a1a !important;
  border-bottom: none !important;
}
:deep(.el-table--enable-row-hover .el-table__body tr:hover > td.el-table__cell) {
  background: rgba(255, 255, 255, 0.55) !important;
}
:deep(.el-table__inner-wrapper::before) {
  display: none !important;
}
:deep(.el-table .el-table__cell) {
  border-bottom: none !important;
}
:deep(.el-table--border::after),
:deep(.el-table--group::after),
:deep(.el-table::before) {
  display: none !important;
}

/* Batch action bar */
.batch-bar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 10px 20px;
  margin-bottom: 12px;
  border-radius: 10px;
  background: rgba(255, 255, 255, 0.5);
  backdrop-filter: blur(16px) saturate(1.3);
  -webkit-backdrop-filter: blur(16px) saturate(1.3);
  border: 1px solid rgba(255, 255, 255, 0.4);
  box-shadow: 0 2px 12px rgba(0, 0, 0, 0.06);
}
.batch-info {
  font-size: 14px;
  color: var(--color-primary);
  font-weight: 500;
}
.batch-actions {
  display: flex;
  gap: 8px;
}
.batch-slide-enter-active {
  transition: all 0.3s ease;
}
.batch-slide-leave-active {
  transition: all 0.2s ease;
}
.batch-slide-enter-from,
.batch-slide-leave-to {
  opacity: 0;
  transform: translateY(-8px);
}

@keyframes fadeIn {
  from { opacity: 0; transform: translateY(8px); }
  to { opacity: 1; transform: translateY(0); }
}
</style>
