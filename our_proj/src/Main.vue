<template>
  <div id="app">
    <h1>{{title}}</h1>
    <div style="padding-left: 20px;padding-right: 20px" id="intro">
      这个网页是用于服务交大的学生，帮助他们对自己需要的多人活动（比如一同打球、自习、户外活动等）寻找伙伴。
    </div>
    <div id="signature">
      <strong>Find source code of webpage on <a href="https://github.com/noahcao/front_end_homework">Github</a></strong>
    </div>
    <div id='table'>
      <p>
        <router-link to="/Board">test</router-link>
      </p>
      <router-view/>
    </div>
  </div>
</template>


<script>
  export default {
    name: 'app',
    data () {
      return {
        title: '约在交大',
        name: 'Homepage',
        fields: 'Computer Vision'
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
  }
  h1{
    font-family: "Arial","Microsoft YaHei",sans-serif;
    text-align: center;
    font-size: 300%;
  }
  #navi{
    text-align: center;
    word-spacing: 5px;
    font-size: 150%;
    margin-top: 20px;
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
  }
  #signature{
    position: absolute;
    top: 20px;
    right: 40px;
  }
</style>
