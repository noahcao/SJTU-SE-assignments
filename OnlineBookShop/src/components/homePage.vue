<template>

  <div id="homepage">

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

        <div id="partition">
          <img src="../../static/partition_long.png" style="width: 100%">
        </div>

      <div class="carousel-wrap" id="recommader">

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
              logo:'./../assets/logo.png'
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
      }
    }
  }

</script>

<style scoped>
  #carousel{
    width: 90%;
    margin-left: 5%;
  }
  #partition{
    width: 100%;
    height: 30px;
    background-color: red;
    margin: 20px 0px 20px 0px;
  }
  #recommader{
    height: 100px;
    margin-top: 20px;
    width: 40%;
    margin-left: 30%;
    margin-right: 30%;
  }
  .swrapImg{
    width: 100%;

  }
  .swrapImg_4{
    width: 50%;
    height: 100%;
  }
  #homepage{
    padding-top: 20px;
    height: 800px;
  }
  .carousel-wrap {
    position: relative;
    height: 400px;
    width: 80%;
    margin-left: 10%;
    overflow: hidden;
    background-color: #fff;
  }
  .slide-ul {
    width: 100%;
    height: 100%;
  }
  li {
    position: absolute;
    width: 100%;
    height: 100%;
  }
  img {
    height: 100%;
  }
  .carousel-items {
    position: absolute;
    z-index: 10;
    top: 380px;
    width: 100%;
    margin: 0 auto;
    text-align: center;
    font-size: 0;
  }
  span {
    display: inline-block;
    height: 6px;
    width: 30px;
    margin: 0 3px;
    background-color: #b2b2b2;
    cursor: pointer;
  }
  .list-enter-to {
    transition: all 1s ease;
    transform: translateX(0);
  }

  .list-leave-active {
    transition: all 1s ease;
    transform: translateX(-100%)
  }

  .list-enter {
    transform: translateX(100%)
  }

  .list-leave {
    transform: translateX(0)
  }



</style>
