export interface User {
  user_id: string
  nickname: string
  description: string
  phone: string
  email: string
  avatar_url: string
  created_at: string
}

export interface FileItem {
  file_id: string
  user_id?: string
  file_name: string
  file_size: number
  file_type: string
  parent_path: string
  unique_flag: string
  is_dir: boolean
  share_id?: string
  public_id?: string
  owner_nickname?: string
  owner_avatar?: string
  created_at: string
  updated_at: string
}

export interface ShareInfo {
  share_id: string
  file_id: string
  file_name: string
  message: string
  expire_hours: number
  max_count: number
  current_count: number
  created_at: string
}

export interface ApiResponse<T> {
  code: number
  message: string
  data: T
}

export interface PageResult<T> {
  files: T[]
  total: number
}
