<template>
  <div id="homepage">

    <div class="carousel-wrap" id="carousel" style="border: solid;width: 80%;margin-left: 10%">

      <transition-group tag="ul" class='slide-ul' name="list">
        <li v-for="(list,index) in slideList" :key="index" v-show="index===currentIndex"
            @mouseenter="stop" @mouseleave="go" style="margin-left: -40px">
          <a :href="list.clickUrl">
            <img :src="list.image" :alt="list.desc">
          </a>
        </li>
      </transition-group>

      <div class="carousel-items">
        <span v-for="(item,index) in slideList.length" :class="{'active':index===currentIndex}" @mouseover="change(index)"></span>
      </div>
    </div>

  </div>

</template>

<script>
  export default{
      data(){
          return{
              slideList: [
                {
                  "clickUrl": "#",
                  "desc": "nhwc",
                  "image": "../assets/logo.png"
                },
                {
                  "clickUrl": "#",
                  "desc": "hxrj",
                  "image": "http://dummyimage.com/1745x492/40b7ea"
                },
                {
                  "clickUrl": "#",
                  "desc": "rsdh",
                  "image": "http://dummyimage.com/1745x492/e3c933"
                }
              ],
            currentIndex: 0,
            timer: '',
            logo:'./../assets/logo.png'
          }
      },
    methods:{
      created() {
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

<style>
  .carousel-wrap {
    position: relative;
    height: 453px;
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
    width: 100%;
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
