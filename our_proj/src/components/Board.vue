<template>
  <div class="Board">
    <h1>{{title}}</h1>
    <div id="intro">这个部分是用来对已经提交的poster信息进行展示，可供有意向的用户进行选择。
    </div>


    
  </div>

</template>

<script>
  export default {
    name: 'board',
    data () {
      return {
        title: '公告板'
      }
    }
  }
</script>


