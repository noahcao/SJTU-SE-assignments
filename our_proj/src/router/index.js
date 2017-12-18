import Vue from 'vue'
import Router from 'vue-router'
import Search from '../components/Search'
import Board from '../components/Board'
import Poster from '../components/Poster'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/Search',
      name: '/Search',
      component: Search
    },
    {
      path: '/Board',
      name: 'Board',
      component: Board
    },
    {
      path: '/Poster',
      name: 'Poster',
      component: Poster
    }
  ]
})
