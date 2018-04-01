<template>

<div id="adminPage">
  <profile v-if="$store.state.signedIn"></profile>
  <sign v-if="!$store.state.signedIn"></sign>
</div>

</template>

<script>
  import profile from "@/components/profile.vue"
  import sign from "@/components/sign.vue"
export default{
    data(){
        return{
            signedIn: this.$store.state.signedIn
        }
    },
    components:{
        profile, sign
    }
}


</script>

<style>
  @import "./../css/admin.css";

</style>
