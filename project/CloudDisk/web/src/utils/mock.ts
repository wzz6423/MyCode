import type { User, FileItem } from '../types'

// Check if we're in mock mode
export function isMockMode(): boolean {
  return localStorage.getItem('mock_mode') === 'true'
}

export function enableMockMode() {
  localStorage.setItem('mock_mode', 'true')
  localStorage.setItem('token', 'mock-token-for-demo')
}

export function disableMockMode() {
  localStorage.removeItem('mock_mode')
}

// Mock user data (matches User interface: no updated_at field)
export const mockUser: User = {
  user_id: 'mock-user-001',
  nickname: '演示用户',
  description: '这是一个演示账号，所有操作均为模拟',
  phone: '13800000000',
  email: 'demo@clouddisk.com',
  avatar_url: '',
  created_at: '2024-01-01T00:00:00Z',
}

// Mock file data - ~15 diverse files (matches FileItem interface)
export const mockFiles: FileItem[] = [
  { file_id: 'f1', file_name: '项目文档.pdf', file_type: 'pdf', file_size: 2048576, parent_path: '/', unique_flag: 'uf1', is_dir: false, created_at: '2024-06-01T10:00:00Z', updated_at: '2024-06-01T10:00:00Z' },
  { file_id: 'f2', file_name: '风景照片.jpg', file_type: 'image', file_size: 5242880, parent_path: '/', unique_flag: 'uf2', is_dir: false, share_id: 'share-001', created_at: '2024-06-02T14:30:00Z', updated_at: '2024-06-02T14:30:00Z' },
  { file_id: 'f3', file_name: '会议录音.mp3', file_type: 'audio', file_size: 10485760, parent_path: '/', unique_flag: 'uf3', is_dir: false, created_at: '2024-06-03T09:15:00Z', updated_at: '2024-06-03T09:15:00Z' },
  { file_id: 'f4', file_name: '演示视频.mp4', file_type: 'video', file_size: 52428800, parent_path: '/', unique_flag: 'uf4', is_dir: false, public_id: 'pub-001', created_at: '2024-06-04T16:45:00Z', updated_at: '2024-06-04T16:45:00Z' },
  { file_id: 'f5', file_name: '备份文件.zip', file_type: 'archive', file_size: 104857600, parent_path: '/', unique_flag: 'uf5', is_dir: false, created_at: '2024-06-05T11:20:00Z', updated_at: '2024-06-05T11:20:00Z' },
  { file_id: 'f6', file_name: '代码仓库.tar.gz', file_type: 'archive', file_size: 31457280, parent_path: '/', unique_flag: 'uf6', is_dir: false, created_at: '2024-06-06T08:00:00Z', updated_at: '2024-06-06T08:00:00Z' },
  { file_id: 'f7', file_name: '设计稿.png', file_type: 'image', file_size: 8388608, parent_path: '/', unique_flag: 'uf7', is_dir: false, created_at: '2024-06-07T13:10:00Z', updated_at: '2024-06-07T13:10:00Z' },
  { file_id: 'f8', file_name: '学习笔记.md', file_type: 'text', file_size: 15360, parent_path: '/', unique_flag: 'uf8', is_dir: false, created_at: '2024-06-08T20:30:00Z', updated_at: '2024-06-08T20:30:00Z' },
  { file_id: 'f9', file_name: '数据分析.xlsx', file_type: 'excel', file_size: 1048576, parent_path: '/', unique_flag: 'uf9', is_dir: false, share_id: 'share-002', public_id: 'pub-002', created_at: '2024-06-09T07:45:00Z', updated_at: '2024-06-09T07:45:00Z' },
  { file_id: 'f10', file_name: '个人简历.docx', file_type: 'word', file_size: 524288, parent_path: '/', unique_flag: 'uf10', is_dir: false, created_at: '2024-06-10T15:00:00Z', updated_at: '2024-06-10T15:00:00Z' },
  { file_id: 'f11', file_name: '工作文件夹', file_type: '', file_size: 0, parent_path: '/', unique_flag: 'uf11', is_dir: true, created_at: '2024-06-11T09:00:00Z', updated_at: '2024-06-11T09:00:00Z' },
  { file_id: 'f12', file_name: '旅行照片合集.zip', file_type: 'archive', file_size: 209715200, parent_path: '/', unique_flag: 'uf12', is_dir: false, public_id: 'pub-003', created_at: '2024-05-25T18:30:00Z', updated_at: '2024-05-25T18:30:00Z' },
  { file_id: 'f13', file_name: '开源项目说明.pdf', file_type: 'pdf', file_size: 3145728, parent_path: '/', unique_flag: 'uf13', is_dir: false, public_id: 'pub-004', created_at: '2024-05-20T12:00:00Z', updated_at: '2024-05-20T12:00:00Z' },
  { file_id: 'f14', file_name: '产品原型.pptx', file_type: 'ppt', file_size: 6291456, parent_path: '/', unique_flag: 'uf14', is_dir: false, created_at: '2024-06-12T11:30:00Z', updated_at: '2024-06-12T11:30:00Z' },
  { file_id: 'f15', file_name: '配置文件.json', file_type: 'text', file_size: 2048, parent_path: '/工作文件夹', unique_flag: 'uf15', is_dir: false, created_at: '2024-06-13T14:00:00Z', updated_at: '2024-06-13T14:00:00Z' },
  { file_id: 'f16', file_name: '照片', file_type: '', file_size: 0, parent_path: '/', unique_flag: 'uf16', is_dir: true, created_at: '2024-06-14T09:00:00Z', updated_at: '2024-06-14T09:00:00Z' },
  { file_id: 'f17', file_name: '文档', file_type: '', file_size: 0, parent_path: '/工作文件夹', unique_flag: 'uf17', is_dir: true, created_at: '2024-06-15T09:00:00Z', updated_at: '2024-06-15T09:00:00Z' },
  { file_id: 'f18', file_name: '报告.docx', file_type: 'word', file_size: 102400, parent_path: '/工作文件夹/文档', unique_flag: 'uf18', is_dir: false, created_at: '2024-06-16T10:00:00Z', updated_at: '2024-06-16T10:00:00Z' },
]

// Mock API response helper
function mockResponse<T>(data: T) {
  return { code: 0, message: 'success', data }
}

// Handle mock API calls - returns mock response or null if not handled
export function handleMockRequest(method: string, url: string, _data?: any, params?: any): any {
  // User APIs
  if (method === 'get' && url === '/user/profile') {
    return mockResponse(mockUser)
  }
  if (method === 'put' && url.startsWith('/user/')) {
    return mockResponse({})
  }

  // File APIs
  if (method === 'get' && url === '/file/list') {
    const parentPath = params?.parent_path || '/'
    const filtered = mockFiles.filter(f => f.parent_path === parentPath)
    return mockResponse({ files: filtered, total: filtered.length })
  }
  if (method === 'get' && url === '/file/public') {
    const publicFiles = mockFiles.filter(f => f.public_id)
    return mockResponse({ files: publicFiles, total: publicFiles.length })
  }
  if (method === 'get' && url === '/file/search') {
    return mockResponse({ files: [], total: 0 })
  }
  if (method === 'post' && url === '/file/upload') {
    return mockResponse({ file_id: 'mock-new-file', file_name: 'uploaded.txt' })
  }
  if (method === 'delete' && url.startsWith('/file/')) {
    return mockResponse({})
  }

  // Share APIs
  if (method === 'post' && url.includes('/share')) {
    return mockResponse({ share_id: 'mock-share-001' })
  }
  if (method === 'post' && url.includes('/public')) {
    return mockResponse({ share_id: 'mock-public-001' })
  }
  if (method === 'delete') {
    return mockResponse({})
  }
  if (method === 'post' && url.includes('/receive/')) {
    return mockResponse({ file_id: 'mock-received-file' })
  }

  // Verify code
  if (method === 'post' && url === '/user/verify/code') {
    return mockResponse({ verify_code_id: 'mock-code-id' })
  }

  // Default fallback
  return mockResponse({})
}
