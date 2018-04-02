<template>

  <div id="homepage">


      <div class="container" id="logBlock" v-if="!$store.state.signedIn">
        <div id="welcome">
          <h2 style="text-align: center">Welcome to BookBar</h2>
        </div>
        <div id="inputLines">
          <div class="row">
            <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4" style="text-align: center;
            padding: 0 10px 0 0;height:40px">
              <span class="label label-success labelInput">Username</span>
            </div>
            <div class="col-xs-8 col-sm-8 col-md-8 col-lg-8" style="padding: 0">
              <input v-model="username" class="input" style="width: 100%">
            </div>
          </div>
          <br><br><br>
          <div class="row">
            <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4" style="text-align: center;
            padding: 0 10px 0 0;height:40px">
              <span class="label label-success labelInput">Password</span>
            </div>
            <div class="col-xs-8 col-sm-8 col-md-8 col-lg-8" style="padding: 0">
              <input v-model="password" class="input" style="width: 100%">
            </div>
          </div>
        </div>
        <div class="container" style="width: 80%">
          <div class="row" >
            <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4">
              <button class="btn btn-success" @click="signIn">Sign In
              </button>
            </div>
            <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4">

            </div>
            <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4">
              <button class="btn btn-info" @click="signOn">Sign On
              </button>
            </div>
          </div>
        </div>
      </div>

    <!-- <div id="partition">
      <img src="../../static/partition_long.png" style="width: 100%">
    </div>

<div class="carousel-wrap" id="carousel">

  <transition-group tag="ul" class='slide-ul' name="list">
    <li v-for="(list,index) in slideList" :key="index" v-show="index===currentIndex"
        @mouseenter="stop" @mouseleave="go" style="margin-left: -40px">
      <a :href="list.clickUrl">
        <img :src="list.image" :alt="list.desc" class="swrapImg">
      </a>
    </li>
  </transition-group>

  <div class="carousel-items">
        <span v-for="(item,index) in slideList.length" :class="{'active':index===currentIndex}"
              @mouseover="change(index)"></span>
  </div>

</div>

<div class="carousel-wrap" id="recommader">

</div> -->

  </div>

</template>

<script>
  import global_ from '../assets/books'
  export default{
      data(){
          return{
              slideList: [
                  {
                  "clickUrl": "#",
                  "desc": "nhwc",
                  "image": "../../static/ad1.jpeg"
                  },
                  {
                  "clickUrl": "#",
                  "desc": "hxrj",
                  "image": "../../static/ad2.jpeg"
                  },
                  {
                  "clickUrl": "#",
                  "desc": "rsdh",
                  "image": "../../static/ad3.jpeg"
                  },
                  {
                  "clickUrl": "#",
                  "desc": "nhwc",
                  "image": "../../static/ad4.jpeg"
                  },
                  {
                  "clickUrl": "#",
                  "desc": "nhwc",
                  "image": "../../static/ad5.jpeg"
                  },
                  {
                  "clickUrl": "#",
                  "desc": "nhwc",
                  "image": "../../static/ad6.jpeg"
                  }
                ],
              featuredBooks_1: global_.featuredBooks[0],
              featuredBooks_2: global_.featuredBooks[1],
              featuredBooks_3: global_.featuredBooks[2],
              featuredBooks_4: global_.featuredBooks[3],
              currentIndex: 0,
              currentIndex_1: 0,
              currentIndex_2: 0,
              currentIndex_3: 0,
              currentIndex_4: 0,
              timer: '',
              logo:'./../assets/logo.png',
              username: "",
              password: ""
          }
      },
    methods:{
      created(){
        this.$nextTick(() => {
          this.timer = setInterval(() => {
            this.autoPlay()
          }, 4000)
        })
      },
      go() {
        this.timer = setInterval(() => {
          this.autoPlay()
        }, 4000)
      },
      stop() {
        clearInterval(this.timer)
        this.timer = null
      },
      change(index) {
        this.currentIndex = index
      },
      autoPlay() {
        this.currentIndex++
        if (this.currentIndex > this.slideList.length - 1) {
          this.currentIndex = 0
        }
      },
      signIn(){
          for(var i = 0; i < this.$store.state.users.length; i++){
            if(this.$store.state.users[i]["username"] == this.username){
                if(this.$store.state.users[i]["password"] == this.password){
                    alert("Welcome to Book Bar! " + this.username);
                    this.$store.state.signedIn = true;
                    this.username = "";
                    this.password = "";
                    return;
                }
                else{
                    alert("Incorrect password received!");
                    this.username = "";
                    this.password = "";
                    return;
                }
            }
          }
          this.username = "";
          this.password = "";
          alert("No corresponding user exists!");
      },
      signOn(){
          var newUser = {};
          newUser["username"] = this.username;
          newUser["password"] = this.password;
          this.$store.state.users.push(newUser);
          alert("You've created a new account! Welcome, "+ this.username);
          this.username = "";
          this.password = "";
      }
    }
  }

</script>

<style scoped>
  @import "./../css/homePage.css";
</style>
