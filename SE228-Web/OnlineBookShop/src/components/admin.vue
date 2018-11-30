<template>

<div id="adminPage" style="padding: 0px">
  <profile v-if="$store.state.signedIn"></profile>
  <homePage v-if="!$store.state.signedIn"></homePage>
</div>

</template>

<script>
  import profile from "@/components/profile.vue"
  import sign from "@/components/sign.vue"
  import homePage from "@/components/homePage.vue"
export default{
    data(){
        return{
            signedIn: this.$store.state.signedIn
        }
    },
    components:{
        profile, sign, homePage
    },
    mounted(){
      this.$store.state.signedIn = (window.localStorage.getItem("signedin") == "signed");
      this.$store.state.userid = window.localStorage.getItem("userid");
      this.$store.state.username = window.localStorage.getItem("username");

  }
}


</script>

<style>
  @import "./../css/admin.css";

</style>
