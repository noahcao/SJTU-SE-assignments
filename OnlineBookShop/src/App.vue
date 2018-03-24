<template>
  <div id="app">
    <navigator></navigator>
    <bookTable></bookTable>
  </div>
</template>
<style>
</style>

<script>
  import global_ from './assets/books'
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
