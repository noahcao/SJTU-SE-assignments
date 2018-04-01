<template>

  <div style="background-color: white;height: 800px;
  padding-top: 20%;padding-left: 20%">
    <div class="container" id="logBlock">
      <button @click="logIn">sign</button>
    </div>
  </div>

</template>

<script>
  export default{
      methods:{
          logIn(){
            this.$store.commit('logIn', "caojinkun", "12345");
          },
          logOut(){
            this.$state.commit('logOut');
          }
      }
  }
</script>

<style>
  #logBlock{
    height: 40%;
    width: 40%;
  }
</style>

