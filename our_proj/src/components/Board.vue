/* eslint-disable */
<template>
  <div id="board">
    <h1>{{title}}</h1>
    <div id="intro">这个部分是用来对已经提交的poster信息进行展示，可供有意向的用户进行选择。
    </div>
    <div>
    <ul id="example">
      <li v-for="user in users">
        {{user.firstname}}   {{user.lastname}}
      </li>
      </ul>
    </div>
  </div>

</template>

<script>
export default {
  name: 'board',
  data () {
    return {
      title: '公告板',
      name: 'Board',
      users: [
        {firstname:'ssdd', lastname:'sssfffs'},
        {firstname:'ssss', lastname:'ssss'}
      ]
    }
  }
}
</script>



