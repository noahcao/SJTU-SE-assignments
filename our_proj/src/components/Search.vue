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
    <p v-for="(value, key) in strings">
    {{key}}:{{value}}
    </p>
    <div v-bind:class="[errorClass,activeClass]"></div>
    <button v-on:click="counterAdd">增加1</button>
    <p>这个按钮被点击了 {{ counter }} 次。</p>
    <button v-on:click="counterDes">减少1</button>
    <p>{{counter}}</p>
    <button  v-on:click="greet">Greet</button>
    <div>
      <p>
    <button type="button" class="btn btn-default">
        <span class="glyphicon glyphicon-sort-by-attributes"></span>
    </button>
    <button type="button" class="btn btn-default">
        <span class="glyphicon glyphicon-sort-by-attributes-alt"></span>
    </button>
    <button type="button" class="btn btn-default">
        <span class="glyphicon glyphicon-sort-by-order"></span>
    </button>
    <button type="button" class="btn btn-default">
        <span class="glyphicon glyphicon-sort-by-order-alt"></span>
    </button>
    </p>
    <button type="button" class="btn btn-default btn-lg">
          <span class="glyphicon glyphicon-user"></span> User
    </button>
    <button type="button" class="btn btn-default btn-sm">
          <span class="glyphicon glyphicon-user"></span> User
    </button>
    <button type="button" class="btn btn-default btn-xs">
          <span class="glyphicon glyphicon-user"></span> User
</button>
    </div>
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
        activeClass: 'active',
        errorClass: 'text-danger',
        counter: 0,
        users: [
          {name: 'CaoJinkun'},
          {name: 'LiLinghui'}
          ],
        strings: {
          value1: 'this is a test',
          value2: 'dddd',
          value3: 'sssadda'
        }
      }
    },
    methods:{
        reverseMsg: function(){
            this.input = this.input.split('').reverse().join('')
        },
        changeok: function(){
            this.seen = !(this.seen)
        },
        counterAdd: function(){
          this.counter += 1
        },
        counterDes: function(){
          this.counter -=1
        },
        greet: function (event) {
            // `this` 在方法里指当前 Vue 实例
             alert('Hello ' + this.message + '!')
            // `event` 是原生 DOM 事件
            if (event) {
            alert(event.target.tagName)
          }
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
  .active {
  width: 100px;
  height: 100px;
  background: green;
  }
  .text-danger {
          background: red;
        }
</style>
