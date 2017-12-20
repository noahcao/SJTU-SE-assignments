<template>
  <div id="app">
    <div id='backboard'>
    <br>
    <h1>{{title}}</h1>
    <div style="padding-left: 20px;padding-right: 20px" id="intro">
      这个网页是用于服务交大的学生，帮助他们对自己需要的多人活动（比如一同打球、自习、户外活动等）寻找伙伴。
    </div>
    <br>
    <div id="board">
    <div id='buttons'>
    <button  type="button" class="btn btn-default">约学习</button>
    <button  type="button" class="btn btn-default">约运动</button>
    <button  type="button" class="btn btn-default">约饭</button>
    <button  type="button" v-on:click="showOutDoor" class="btn btn-default">约户外</button>
    </div>
    </div>
<!--    
    <div id="navigator">
    <center>
    <ul class="nav nav-tabs">
        <li class="active"><a href="#">Home</a></li>
        <li><a href="#">SVN</a></li>
        <li><a href="#">iOS</a></li>
        <li><a href="#">VB.Net</a></li>
    </ul>
    </center>
    </div>
-->
<!--
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
-->    </div>
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
        routershow: true,
        background: {
          backimage: "url(" + require('./assets/SJTUbackground.gif') + ")",
          backrepeat: "no-repeat",
          backsize: "100%, 100%"
        }
      }
    },
    methods:{
      showOutDoor: function () {
          alert("dddd")
      },
      showStudy: function(){
        alert("dddddd")
      }
    }
  }
</script>

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  margin: 20px;
  text-align: center;
  background-color: rgba(1,1,1,0);
}
h1{
  font-family: "Arial","Microsoft YaHei",sans-serif;
  text-align: center;
  font-size: 300%;
}
  div {
    background: whitesmoke;
    text-align: center;
  }
  body{
    height: 100%;
    width: 100%;
    overflow: auto;
    background-size: 100%;
    background: whitesmoke;
    background-image: url("./assets/back.jpg");
    background-repeat: no-repeat;
    background-size: 100%, 100%;
  }
  .background{
    position:absolute; top:0; left:0;   
     position: relative;
     z-index:-1;  
　　filter:blur(10px);
　  over-flow: hidden;   
  }
  #backboard{
    height: 500px;
    margin-bottom: 10px;
  }
  #navigator{
    width: 600px;
    height: 45px;
    margin: auto;
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    align-self: center;
    border-bottom: solid red;
  }
</style>
