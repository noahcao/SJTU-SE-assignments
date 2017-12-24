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
    expect(typeof app.newActivity.contact).toBe('string')
    expect(typeof app.newActivity.name).toBe('string')
    expect(typeof app.newActivity.title).toBe('string')
    expect(typeof app.newActivity.month).toBe('number')
    expect(typeof app.newActivity.day).toBe('number')
    expect(typeof app.newActivity.start_hour).toBe('number')
    expect(typeof app.newActivity.start_minu).toBe('number')
    expect(typeof app.newActivity.end_hour).toBe('number')
    expect(typeof app.newActivity.end_minu).toBe('number')
    expect(typeof app.searchString).toBe('string')
  })
})

