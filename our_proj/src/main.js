// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from './App'
import Routers from './router'
import Main from './Main'
import $ from 'jquery'

Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router: Routers,
  template: '<App/>',
  components: { App }
})
/*
new Vue({
  el: '#app',
  router: Routers,
  template: '<Main/>',
  components: { Main }
})*/
