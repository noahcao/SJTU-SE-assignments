import Vue from 'vue'
import Router from 'vue-router'
import homePage from '@/components/homePage'
import bookTable from '@/components/bookTable'

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
    }
  ]
})
