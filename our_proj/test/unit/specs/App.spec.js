/**
 * Created by noahcao on 2017/11/19.
 */
import App from '@/App'
import Vue from 'vue'
import Router from 'vue-router'


describe('App.vue',() => {
  it('test the data variables', () => {
    // test
    const app = new Vue(App).$mount()
    expect(app.title).toBe('约在交大')
    expect(typeof app.title).toBe('string')
    expect(typeof app.commitPoster).toBe('function')
    expect(typeof app.cancelCommitPoster).toBe('function')
    expect(typeof app.initNewPoster).toBe('function')
    expect(typeof app.searchPoster).toBe('function')
    for(var item in app.items){
    	expect(item.text).toBe(item.value)
    }
    expect(app.newActivity.months.length).toBe(12)
    expect(app.newActivity.days.length).toBe(31)
  })

  const Constructor = Vue.extend(App);

  // 挂载组件
  const App = new Constructor().$mount();
  
  it('test the modules', () => {
    App.input = 'test input'
  	App._watcher.run()
  	expect(App.$el.textContent).toContain('test input')
  })
})

