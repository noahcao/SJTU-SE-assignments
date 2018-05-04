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
              <input type="password" v-model="password" class="input" style="width: 100%">
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
              password: "",
              UsersInRecord: []
          }
      },
    mounted(){
        this.$store.state.signedIn = (window.localStorage.getItem("signedin") == "signed");
        this.$store.state.userid = window.localStorage.getItem("userid");
        this.$store.state.username = window.localStorage.getItem("username");
    },
    methods:{
      getdata(){
        this.$ajax({
          url: 'axiosPath',
          method: 'get',
          withCredentials: true
        })
      },
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
          this.$http.post('/getuser', {"username": this.username})
          .then((response) => {
            var user = response.data;
            if(user.password  == this.password){
              alert("Welcome to Book Bar! " + this.username);
              this.$store.state.signedIn = true;
              this.$store.state.userid = user.id;
              this.$store.state.username = this.username;
              this.userid = user.id;
              window.localStorage.setItem("username", this.username);
              window.localStorage.setItem("signedin", "signed");
              window.localStorage.setItem("userid", this.userid);
              console.log(user);
              window.localStorage.setItem("admin", user.admin);
            }
            else{
              alert("Invalid username or password!");
            }
            this.username = "";
            this.password = "";
          });
      },
      signOn(){
        if(this.username!="" && this.password!=""){
          this.$http.post('/signon', {username: this.username,
            password: this.password, img: null})
            .then((response) => {
              console.log(response.data);
              if(response.data.id != -1){
                alert(this.username + " , you've created a new account!");
              }else{
                alert("Already a duplicated username exists!");
              }
              this.username = "";
              this.password = "";
            });
        }else{
          alert("Please input valid username and password!");
        }
      }
    }
  }

</script>

<style scoped>
  @import "./../css/homePage.css";
</style>
