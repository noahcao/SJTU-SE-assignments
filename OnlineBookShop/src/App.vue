<template>
  <div id="app">
    <navigator style="z-index:99"></navigator>
    <router-view></router-view>
  </div>
</template>

<style>
</style>

<script>
  import homePage from '@/components/homePage.vue'
  import navigator from '@/components/navigator.vue'
  import bookTable from '@/components/bookTable.vue'
  export default{
    name: 'app',
    components:{
        homePage, navigator, bookTable
    }
  }
</script>
