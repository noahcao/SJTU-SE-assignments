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
    }
}


</script>

<style>
  @import "./../css/admin.css";

</style>
