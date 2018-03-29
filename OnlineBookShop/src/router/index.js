import Vue from 'vue'
import Router from 'vue-router'
import homePage from '@/components/homePage'
import bookTable from '@/components/bookTable'
import admin from '@/components/admin'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/homePage',
      name: 'homePage',
      component: homePage
    },
    {
      path: '/bookTable',
      name: 'bookTable',
      component: bookTable
    },
    {
      path: '/admin',
      name: 'admin',
      component: admin
    }
  ]
})
