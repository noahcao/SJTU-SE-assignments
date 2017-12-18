/**
 * Created by noahcao on 2017/11/19.
 */
import App from '@/App'
import Vue from 'vue'
import Router from 'vue-router'
import Det from '@/components/Det'
import Seg from '@/components/Seg'
import General from '@/components/General'
import Pose from '@/components/Pose'
import Track from '@/components/Track'
import Video from '@/components/Video'

describe('App.vue',() => {
  describe('Template', () => {
    it('should render a component', () => {
      Vue.use(Router)
      const vm = new Vue(App).$mount()
      expect(vm.title).toBe('Intelligent Vision')
      expect(vm.name).toBe('Homepage')
      expect(vm.fields).toBe('Computer Vision')
    })
  })
})
