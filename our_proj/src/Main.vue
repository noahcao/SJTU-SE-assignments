<template>
  <div id="app">
    <h1>{{title}}</h1>
    <div style="padding-left: 20px;padding-right: 20px" id="intro">
      这个网页是用于服务交大的学生，帮助他们对自己需要的多人活动（比如一同打球、自习、户外活动等）寻找伙伴。
    </div>
    <div id="signature">
      <strong>Find source code of webpage on <a href="https://github.com/noahcao/front_end_homework">Github</a></strong>
    </div>

    <div id='table' v-if="routershow">
        <p>
         <router-link to="/Board">test</router-link>
          <router-link to="/Search">search</router-link>
        </p>
        <router-view/>
    </div>
    <div>
      <button v-on:click="say('hi')">Say hi</button>
    </div>
    <div role="dialog" class="modal fade" id="layer">
      <div class="modal-dialog">
        <div class="modal-content">
          <div class="modal-header">
            <button type="button" class="close" data-dismiss="modal">
              <span>x</span>
            </button>
            <h4 class="modal-title">确认删除吗？</h4>
          </div>
          <div class="modal-body text-right">
            <button class="btn btn-primary btn-sm" data-dismiss="modal">取消</button>
            <button class="btn btn-danger btn-sm" data-dismiss="modal" v-on:click="deleteMsg(nowIndex)">确认</button>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>


<script>
  import Vue from 'vue'
  export default {
    name: 'app',
    data () {
      return {
        title: '约在交大',
        name: 'Homepage',
        fields: 'Computer Vision',
        routershow: true
      }
    },
    methods:{
      say: function (message) {
      this.routershow = !this.routershow

    }
    }
  };
  window.onload = function(){
    new Vue({
      el: ".container",
      data: {
        username:'',
        age:'',
        nowIndex: -100,
        myData: [

        ]
      },
      methods: {
        add: function(){
          this.myData.push({
            name: this.username,
            age: this.age
          });

          this.username = '';
          this.age = '';
        },
        deleteMsg: function(n){
          if(n==-1){
            this.myData = [];
          }else{
            this.myData.splice(n,1);
          }
        }
      }
    })
  }
</script>