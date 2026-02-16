<template>
  <el-container class="app-layout">
    <ParticleBackground />
    <el-header class="header">
      <div class="header-left">
        <div class="logo-mark">
          <el-icon :size="20" color="#fff"><Files /></el-icon>
        </div>
        <span class="logo-text">CloudDisk</span>
        <el-tag v-if="mockMode" type="warning" size="small" effect="dark" style="margin-left: 8px">演示模式</el-tag>
      </div>
      <div class="header-center">
        <SearchBar @search="onSearch" />
      </div>
      <div class="header-right">
        <UserAvatar
          :url="userStore.user?.avatar_url"
          :name="userStore.user?.nickname"
          :size="32"
        />
        <el-dropdown @command="onCommand" trigger="click">
          <span class="user-name">
            {{ userStore.user?.nickname || '用户' }}
            <el-icon><ArrowDown /></el-icon>
          </span>
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item command="profile">
                <el-icon><User /></el-icon>个人中心
              </el-dropdown-item>
              <el-dropdown-item command="logout" divided>
                <el-icon><SwitchButton /></el-icon>退出登录
              </el-dropdown-item>
            </el-dropdown-menu>
          </template>
        </el-dropdown>
      </div>
    </el-header>
    <el-container class="body">
      <el-aside width="200px" class="aside">
        <el-menu
          :default-active="activeMenu"
          router
          class="side-menu"
        >
          <el-menu-item index="/files">
            <el-icon><FolderOpened /></el-icon>
            <span>我的文件</span>
          </el-menu-item>
          <el-menu-item index="/public">
            <el-icon><Share /></el-icon>
            <span>共享文件</span>
          </el-menu-item>
        </el-menu>
      </el-aside>
      <el-main class="main">
        <router-view v-slot="{ Component }">
          <transition name="page-fade" mode="out-in">
            <component :is="Component" />
          </transition>
        </router-view>
      </el-main>
    </el-container>
  </el-container>
</template>

<script setup lang="ts">
import { computed, onMounted } from 'vue'
import { useRoute, useRouter } from 'vue-router'
import { useUserStore } from '../stores/user'
import { isMockMode } from '../utils/mock'
import SearchBar from './SearchBar.vue'
import UserAvatar from './UserAvatar.vue'
import ParticleBackground from './ParticleBackground.vue'

const route = useRoute()
const router = useRouter()
const userStore = useUserStore()

const activeMenu = computed(() => route.path)
const mockMode = computed(() => isMockMode())

onMounted(() => {
  if (userStore.token) {
    userStore.fetchProfile()
  }
})

function onSearch(keyword: string) {
  router.push({ path: '/files', query: { keyword } })
}

function onCommand(cmd: string) {
  if (cmd === 'profile') {
    router.push('/profile')
  } else if (cmd === 'logout') {
    userStore.logout()
  }
}
</script>

<style scoped>
.app-layout {
  height: 100vh;
  position: relative;
}

.header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  background: rgba(255, 255, 255, 0.6);
  backdrop-filter: blur(20px) saturate(1.2);
  -webkit-backdrop-filter: blur(20px) saturate(1.2);
  border-bottom: 1px solid var(--color-border);
  padding: 0 24px;
  height: 56px;
  position: relative;
  z-index: 10;
}

.header-left {
  display: flex;
  align-items: center;
  gap: 10px;
}

.logo-mark {
  width: 32px;
  height: 32px;
  border-radius: 8px;
  background: linear-gradient(135deg, #356bfb, #7c5cf5);
  display: flex;
  align-items: center;
  justify-content: center;
  box-shadow: 0 2px 8px rgba(53, 107, 251, 0.25);
}

.logo-text {
  font-size: 18px;
  font-weight: 700;
  color: var(--color-text-1);
  letter-spacing: -0.3px;
}

.header-center {
  flex: 1;
  max-width: 440px;
  margin: 0 40px;
}

.header-right {
  display: flex;
  align-items: center;
  gap: 12px;
}

.user-name {
  cursor: pointer;
  display: flex;
  align-items: center;
  gap: 4px;
  font-size: 13px;
  font-weight: 500;
  color: var(--color-text-2);
  padding: 6px 10px;
  border-radius: 8px;
  transition: all 0.15s ease;
}
.user-name:hover {
  background: var(--color-primary-bg);
  color: var(--color-primary);
}

.body {
  height: calc(100vh - 56px);
  position: relative;
  z-index: 1;
}

.aside {
  background: rgba(255, 255, 255, 0.4);
  backdrop-filter: blur(20px) saturate(1.2);
  -webkit-backdrop-filter: blur(20px) saturate(1.2);
  border-right: 1px solid var(--color-border);
  overflow-y: auto;
}

.side-menu {
  border-right: none;
  padding: 12px 4px;
  background: transparent !important;
}

:deep(.el-menu) {
  background: transparent !important;
}
:deep(.el-menu-item) {
  height: 40px;
  line-height: 40px;
  font-size: 14px;
  font-weight: 500;
  color: var(--color-text-2);
  transition: all 0.15s ease;
}
:deep(.el-menu-item:hover) {
  background: var(--color-primary-bg) !important;
  color: var(--color-primary);
}
:deep(.el-menu-item.is-active) {
  color: var(--color-primary);
  background: var(--color-primary-bg) !important;
  font-weight: 600;
}

.main {
  background: transparent;
  overflow-y: auto;
  position: relative;
  z-index: 1;
  padding: 20px;
}

/* Page transition */
.page-fade-enter-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
}
.page-fade-leave-active {
  transition: opacity 0.12s ease;
}
.page-fade-enter-from {
  opacity: 0;
  transform: translateY(6px);
}
.page-fade-leave-to {
  opacity: 0;
}
</style>
