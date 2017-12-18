<template>
  <div id="app">
    <h1>{{title}}</h1>
    <div style="padding-left: 20px;padding-right: 20px" id="intro">
      This page is used to relize a easy demo of our projectt - ' Appoint at sjtu'.
    </div>
    <div id="signature">
      <strong>Find source code of webpage on <a href="https://github.com/noahcao/front_end_homework">Github</a></strong>
    </div>
  </div>
</template>

<script>
export default {
  name: 'app',
  data () {
    return {
      title: 'Appoint At SJTU',
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
