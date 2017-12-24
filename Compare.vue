<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>对比</title>
  <script src="https://cdn.bootcss.com/vue/2.2.2/vue.min.js"></script>
</head>
<body>
<div id="app">
  <ol>

    <div v-for="poster in Posters.studyPoster">
      <div v-if="输入变量 == poster.title">
        <p>poster</p>
      </div>
      <div v-else>
        <p>Sorry, we dont't find it</p>
      </div>
    </div>
  </ol>
</div>

<script>
  new Vue({
    el: '#app',
    data: {
      Posters.studyPoster: [
    { title: 'Runoob' },
    { title: 'Google' },
    { title: 'Taobao' }
  ]
  }
  })
</script>
</body>
</html>
