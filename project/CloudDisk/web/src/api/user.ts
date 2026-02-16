import request from './request'
import type { ApiResponse, User } from '../types'

export function getVerifyCode(type: 'phone' | 'email', target: string) {
  return request.post<any, ApiResponse<{ verify_code_id: string }>>('/user/verify/code', { type, target })
}

export function register(data: {
  type: 'nickname' | 'phone' | 'email'
  target?: string
  password: string
  nickname: string
  verify_code_id?: string
  verify_code?: string
}) {
  return request.post<any, ApiResponse<{}>>('/user/register', data)
}

export function login(data: { type: 'nickname' | 'phone' | 'email'; target: string; password: string }) {
  return request.post<any, ApiResponse<{ token: string; user: User }>>('/user/login', data)
}

export function getProfile() {
  return request.get<any, ApiResponse<User>>('/user/profile')
}

export function updateAvatar(file: File) {
  const form = new FormData()
  form.append('avatar', file)
  return request.put<any, ApiResponse<{ avatar_url: string }>>('/user/avatar', form)
}

export function updateNickname(nickname: string) {
  return request.put<any, ApiResponse<{}>>('/user/nickname', { nickname })
}

export function updateDescription(description: string) {
  return request.put<any, ApiResponse<{}>>('/user/description', { description })
}

export function updatePassword(old_password: string, new_password: string) {
  return request.put<any, ApiResponse<{}>>('/user/password', { old_password, new_password })
}

export function updatePhone(data: {
  new_phone: string
  verify_code_id: string
  verify_code: string
  password: string
}) {
  return request.put<any, ApiResponse<{}>>('/user/phone', data)
}

export function updateEmail(data: {
  new_email: string
  verify_code_id: string
  verify_code: string
  password: string
}) {
  return request.put<any, ApiResponse<{}>>('/user/email', data)
}

export function deleteAccount(password: string) {
  return request.delete<any, ApiResponse<{}>>('/user/account', { data: { password } })
}
