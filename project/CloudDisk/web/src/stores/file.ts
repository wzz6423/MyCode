import { defineStore } from 'pinia'
import { ref } from 'vue'
import type { FileItem } from '../types'
import { getFileList, searchFiles } from '../api/file'

export interface FileFilter {
  status?: string
  start_time?: string
  end_time?: string
}

export const useFileStore = defineStore('file', () => {
  const files = ref<FileItem[]>([])
  const currentPath = ref('/')
  const total = ref(0)
  const loading = ref(false)
  const page = ref(1)
  const size = ref(20)
  const filter = ref<FileFilter>({})

  async function fetchFiles(path?: string) {
    if (path !== undefined) {
      currentPath.value = path
      page.value = 1
    }
    loading.value = true
    try {
      const params: FileFilter = {}
      if (filter.value.status) params.status = filter.value.status
      if (filter.value.start_time) params.start_time = filter.value.start_time
      if (filter.value.end_time) params.end_time = filter.value.end_time
      const res = await getFileList(currentPath.value, page.value, size.value, params)
      files.value = res.data.files || []
      total.value = res.data.total || 0
    } finally {
      loading.value = false
    }
  }

  async function search(keyword: string) {
    loading.value = true
    try {
      const res = await searchFiles(keyword, page.value, size.value)
      files.value = res.data.files || []
      total.value = res.data.total || 0
    } finally {
      loading.value = false
    }
  }

  function setFilter(f: FileFilter) {
    filter.value = f
    page.value = 1
    fetchFiles()
  }

  function clearFilter() {
    filter.value = {}
    page.value = 1
    fetchFiles()
  }

  return { files, currentPath, total, loading, page, size, filter, fetchFiles, search, setFilter, clearFilter }
})
