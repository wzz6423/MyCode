import request from './request'
import type { ApiResponse, FileItem, PageResult } from '../types'

export function uploadFile(
  file: File,
  parentPath: string,
  uniqueFlag: string,
  onProgress?: (percent: number) => void
) {
  const form = new FormData()
  form.append('file', file)
  form.append('parent_path', parentPath)
  form.append('unique_flag', uniqueFlag)
  return request.post<any, ApiResponse<FileItem>>('/file/upload', form, {
    onUploadProgress(e) {
      if (e.total && onProgress) {
        onProgress(Math.round((e.loaded / e.total) * 100))
      }
    },
  })
}

export function downloadFile(fileId: string) {
  const token = localStorage.getItem('token')
  const link = document.createElement('a')
  link.href = `/api/v1/file/download/${fileId}?token=${token}`
  link.target = '_blank'
  document.body.appendChild(link)
  link.click()
  document.body.removeChild(link)
}

export function deleteFile(fileId: string) {
  return request.delete<any, ApiResponse<{}>>(`/file/${fileId}`)
}

export function getFileList(
  parentPath: string,
  page = 1,
  size = 20,
  filter?: { status?: string; start_time?: string; end_time?: string }
) {
  const params: Record<string, any> = { parent_path: parentPath, page, size }
  if (filter?.status) params.filter = filter.status
  if (filter?.start_time) params.start_time = filter.start_time
  if (filter?.end_time) params.end_time = filter.end_time
  return request.get<any, ApiResponse<PageResult<FileItem>>>('/file/list', { params })
}

export function searchFiles(keyword: string, page = 1, size = 20) {
  return request.get<any, ApiResponse<PageResult<FileItem>>>('/file/search', {
    params: { keyword, page, size },
  })
}

export function shareFile(fileId: string, data: { message: string; expire_hours: number; max_count: number }) {
  return request.post<any, ApiResponse<{ share_id: string }>>(`/file/${fileId}/share`, data)
}

export function unshareFile(fileId: string) {
  return request.delete<any, ApiResponse<{}>>(`/file/${fileId}/share`)
}

export function publicFile(fileId: string, data: { expire_hours: number; max_count: number }) {
  return request.post<any, ApiResponse<{ share_id: string }>>(`/file/${fileId}/public`, data)
}

export function unpublicFile(fileId: string) {
  return request.delete<any, ApiResponse<{}>>(`/file/${fileId}/public`)
}

export function getPublicFiles(page = 1, size = 20) {
  return request.get<any, ApiResponse<PageResult<FileItem>>>('/file/public', {
    params: { page, size },
  })
}

export function getFilePreviewUrl(fileId: string): string {
  const token = localStorage.getItem('token')
  return `/api/v1/file/download/${fileId}?preview=1&token=${token}`
}

export function getShareInfo(shareId: string) {
  return request.get<any, ApiResponse<{
    share_id: string
    share_type: number
    user_id: string
    message: string
    file_name: string
    file_size: number
    expire_at: string | null
    created_at: string
  }>>(`/share/${shareId}`)
}

export function receiveFile(shareId: string, savePath: string, fileName?: string) {
  const body: Record<string, string> = { save_path: savePath }
  if (fileName) body.file_name = fileName
  return request.post<any, ApiResponse<{ file_id: string }>>(`/file/receive/${shareId}`, body)
}

export function createFolder(folderName: string, parentPath: string) {
  return request.post<any, ApiResponse<FileItem>>('/file/folder', { folder_name: folderName, parent_path: parentPath })
}

export function batchDownload(fileIds: string[]) {
  const token = localStorage.getItem('token')
  // Use fetch to POST and trigger download
  fetch('/api/v1/file/batch-download', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Authorization': `Bearer ${token}`,
    },
    body: JSON.stringify({ file_ids: fileIds }),
  })
    .then(res => res.blob())
    .then(blob => {
      const url = URL.createObjectURL(blob)
      const link = document.createElement('a')
      link.href = url
      link.download = 'batch_download.zip'
      document.body.appendChild(link)
      link.click()
      document.body.removeChild(link)
      URL.revokeObjectURL(url)
    })
}
