<template>
  <div id="app">
      <navigator style="margin-top:0px"></navigator>

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
