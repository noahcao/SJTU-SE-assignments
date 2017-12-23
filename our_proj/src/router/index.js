import Vue from 'vue'
import Router from 'vue-router'
import Board from '../components/Board'
import NewPoster from '../components/NewPoster'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/NewPoster',
      name: '/NewPoster',
      component: NewPoster
    },
    {
      path: '/Board',
      name: 'Board',
      component: Board
    }
  ]
})
