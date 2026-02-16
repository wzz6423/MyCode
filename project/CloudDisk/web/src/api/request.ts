import axios from 'axios'
import { notifyError } from '../utils/notify'
import { isMockMode, handleMockRequest } from '../utils/mock'
import router from '../router'

const request = axios.create({
  baseURL: '/api/v1',
  timeout: 30000,
})

request.interceptors.request.use(
  (config) => {
    const token = localStorage.getItem('token')
    if (token) {
      config.headers.Authorization = `Bearer ${token}`
    }

    // Mock mode: use custom adapter to return mock data without network
    if (isMockMode()) {
      config.adapter = () => {
        const method = config.method?.toLowerCase() || 'get'
        const url = config.url || ''
        const mockData = handleMockRequest(method, url, config.data, config.params)
        return Promise.resolve({
          data: mockData,
          status: 200,
          statusText: 'OK',
          headers: {},
          config,
        })
      }
    }

    return config
  },
  (error) => Promise.reject(error)
)

request.interceptors.response.use(
  (response) => {
    const res = response.data
    if (res.code !== 0) {
      notifyError(res.message || '请求失败')
      return Promise.reject(new Error(res.message || '请求失败'))
    }
    return res
  },
  (error) => {
    if (error.response?.status === 401) {
      localStorage.removeItem('token')
      router.push('/login')
      notifyError('登录已过期，请重新登录')
    } else {
      notifyError(error.response?.data?.message || error.message || '网络错误')
    }
    return Promise.reject(error)
  }
)

export default request
