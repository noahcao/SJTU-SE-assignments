<template>
  <div class="hello">
    <div v-html="msg"></div>
    <label for="r1">修改颜色</label>
    <input type="checkbox" v-model="class1" id="r1">
    <br><br>
    <div v-bind:class="{'class1': class1}">
      directiva v-bind:class
    </div>
    <input type="checkbox" v-on:click="reverseMsg"></input>
    <button v-on:click="changeok">控制内容显示</button>
    <br>
    <p v-if="seen">
      看见了
    </p>
    <p v-else>
      看不见了吧！
    </p>
    <input v-model="input">
    <p>{{input}}</p>
    <button v-on:click="reverseMsg">反转输入内容</button>
    <div>
      {{message | upper}}
      <br>
      {{message | capitalize | upper}}
    </div>
    <p v-for="user in users">
      {{user.name}}
    </p>
  </div>
</template>

<script>
  export default {
    name: 'hello',
    data () {
      return {
        msg: '<h1>欢迎来到菜鸟教程</h1>',
        class1: true,
        seen: true,
        input: 'input information',
        message: 'test_string',
        ok: true,
        users: [
          {name: 'CaoJinkun'},
          {name: 'LiLinghui'}
          ]
      }
    },
    methods:{
        reverseMsg: function(){
            this.input = this.input.split('').reverse().join('')
        },
        changeok: function(){
            this.seen = !(this.seen)
        }
    },
    filters:{
        capitalize: function(value){
            if(!value) return ''
            value = value.toString()
            return value.charAt(0).toUpperCase() + value.slice(1)
        },
        upper: function(value){
            if(!value) return ''
            value = value.toString()
            return value.charAt(3).toUpperCase() + value.slice(0,3) + value.slice(3)
      }
    }
  }
</script>

<style>
  .class1{
    background: #555;
    color: #fff;
  }
</style>
