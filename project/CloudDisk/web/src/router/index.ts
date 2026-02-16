import { createRouter, createWebHistory } from 'vue-router'

const router = createRouter({
  history: createWebHistory(),
  routes: [
    {
      path: '/login',
      name: 'Login',
      component: () => import('../views/Login.vue'),
      meta: { public: true },
    },
    {
      path: '/register',
      name: 'Register',
      component: () => import('../views/Register.vue'),
      meta: { public: true },
    },
    {
      path: '/share/:shareId',
      name: 'ShareReceive',
      component: () => import('../views/ShareReceive.vue'),
      meta: { public: true },
    },
    {
      path: '/',
      component: () => import('../components/AppLayout.vue'),
      redirect: '/files',
      children: [
        {
          path: 'files',
          name: 'Files',
          component: () => import('../views/FileManager.vue'),
        },
        {
          path: 'public',
          name: 'Public',
          component: () => import('../views/PublicSquare.vue'),
          meta: { public: true },
        },
        {
          path: 'profile',
          name: 'Profile',
          component: () => import('../views/Profile.vue'),
        },
      ],
    },
  ],
})

router.beforeEach((to, _from, next) => {
  const token = localStorage.getItem('token')
  const mockMode = localStorage.getItem('mock_mode') === 'true'
  if (!to.meta.public && !token && !mockMode) {
    next('/login')
  } else {
    next()
  }
})

export default router
