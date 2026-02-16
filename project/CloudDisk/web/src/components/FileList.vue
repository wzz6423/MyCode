<template>
  <div class="file-list">
    <el-table
      :data="files"
      v-loading="loading"
      @selection-change="$emit('selectionChange', $event)"
      style="width: 100%"
      empty-text="暂无文件"
    >
      <el-table-column type="selection" width="40" />
      <el-table-column label="文件名" min-width="280">
        <template #default="{ row }">
          <div class="file-name-cell" @click="onClickName(row)">
            <el-icon :size="22" :color="row.is_dir ? '#f0c040' : fileTypeColor(row.file_name)">
              <component :is="row.is_dir ? 'Folder' : fileTypeIcon(row.file_name)" />
            </el-icon>
            <span class="file-name" :class="{ 'is-dir': row.is_dir }">{{ row.file_name }}</span>
          </div>
        </template>
      </el-table-column>
      <el-table-column label="大小" width="120">
        <template #default="{ row }">
          {{ row.is_dir ? '-' : formatFileSize(row.file_size) }}
        </template>
      </el-table-column>
      <el-table-column label="类型" width="100">
        <template #default="{ row }">
          {{ row.is_dir ? '文件夹' : (row.file_type || '-') }}
        </template>
      </el-table-column>
      <el-table-column label="修改时间" width="170">
        <template #default="{ row }">
          {{ formatDate(row.updated_at || row.created_at) }}
        </template>
      </el-table-column>
      <el-table-column label="操作" width="280" fixed="right">
        <template #default="{ row }">
          <el-button
            link
            type="primary"
            @click="$emit('download', row)"
          >
            <el-icon><Download /></el-icon>下载
          </el-button>
          <el-button
            v-if="!row.share_id"
            link
            type="primary"
            @click="$emit('share', row)"
          >
            <el-icon><Share /></el-icon>分享
          </el-button>
          <el-button
            v-else
            link
            type="warning"
            @click="$emit('unshare', row)"
          >
            <el-icon><Share /></el-icon>取消分享
          </el-button>
          <el-button
            link
            :type="row.public_id ? 'warning' : 'success'"
            @click="$emit('togglePublic', row)"
          >
            <el-icon><Promotion /></el-icon>{{ row.public_id ? '取消公开' : '公开' }}
          </el-button>
          <el-button
            link
            type="danger"
            @click="$emit('delete', row)"
          >
            <el-icon><Delete /></el-icon>删除
          </el-button>
        </template>
      </el-table-column>
    </el-table>
  </div>
</template>

<script setup lang="ts">
import type { FileItem } from '../types'
import { formatFileSize, formatDate, fileTypeIcon, fileTypeColor } from '../utils/format'

defineProps<{
  files: FileItem[]
  loading: boolean
}>()

const emit = defineEmits<{
  openDir: [file: FileItem]
  preview: [file: FileItem]
  download: [file: FileItem]
  share: [file: FileItem]
  unshare: [file: FileItem]
  delete: [file: FileItem]
  togglePublic: [file: FileItem]
  selectionChange: [files: FileItem[]]
}>()

function onClickName(row: FileItem) {
  if (row.is_dir) {
    emit('openDir', row)
  } else {
    emit('preview', row)
  }
}
</script>

<style scoped>
.file-name-cell {
  display: flex;
  align-items: center;
  gap: 8px;
  cursor: pointer;
}
.file-name {
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}
.file-name.is-dir:hover {
  color: var(--color-primary);
}
</style>
