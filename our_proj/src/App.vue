<template>
    <div id="app">
      <div id='backboard'>
        <h1 style="padding-top: 2%;padding-bottom: 1%">{{title}}</h1>
        <div id="board" style="background: rgba(255,255,255,0)">
          <div id="navigator" style="background: rgba(255,255,255,0);height: 9%;width: 96%">
            <nav class="navbar navbar-default" role="navigation" style="background: rgba(255,255,255,0); border-bottom: solid lightgray">
              <div class="container-fluid">
               <div>
                <ul class="nav navbar-nav" style="width: 50%; align-content: center;margin-left: 30%">
                  <li><a href="#" @click="newPosterVisible=true" >发起新活动</a></li>
                  <li><a href="#">搜索活动</a></li>
                  <li class="dropdown">
                    <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                      分类查看
                      <b class="caret"></b>
                    </a>
                    <ul class="dropdown-menu">
                      <li><a v-on:click="showStudy" href="#">约学习</a></li>
                      <li><a v-on:click="showSport" href="#">约运动</a></li>
                      <li><a v-on:click="showOutDoor" href="#">约户外</a></li>
                      <li><a v-on:click="showOthers" href="#">约其他</a></li>
                    </ul>
                  </li>
                </ul>
            </div>
              </div>
          </nav>
          <div class="container" style="position: absolute;height: 420px;border:solid mediumpurple 3px; border-radius: 10px;width: 500px;margin-left: 30%;margin-right: 30%;z-index:9" v-if="newPosterVisible">
            <div class="container" v-if="newPosterVisible"></div>
            <transition name="test">
              <div class="dialog-content" v-if="newPosterVisible">
                <p class="dialog-close" @click="cancelCommitPoster">x</p>
                <br>
                <div id="newActTitle">
                  <h3>发起新活动</h3><br>
                </div>
                请输入新活动的名称：<input v-model="newActivity.title"><br><br>
                您的姓名：<input v-model="newActivity.name">
                <br><br>
                联系方式：<input v-model="newActivity.contact">
                <br><br>
                请选择活动类型：
                <select v-model="selected">
                  <option v-for="item in items" v-bind:value="item.value">{{item.text}}</option>
                </select>
                <br><br>
                活动日期：
                <select v-model="newActivity.month">
                  <option v-for="month in newActivity.months" v-bind:value="month">{{month}}</option>
                </select>
                <select v-model="selected">
                  <option v-for="day in newActivity.days" v-bind:value="day">{{day}}</option>
                </select>
                <br><br>
                活动时间：
                <input style="width:50px" v-model="newActivity.start_hour"> ：
                <input style="width:50px" v-model="newActivity.start_minu"> --
                <input style="width:50px" v-model="newActivity.end_hour"> ：
                <input style="width:50px" v-model="newActivity.end_minu">
                <br><br>
                <button type="button" @click="commitPoster" class="btn btn-success">提交</button>
                <button type="button" @click="cancelCommitPoster" class="btn btn-danger">取消</button>
                <br><br>
              </div>
            </transition>
          </div>
        </div>
      </div>
        <br><br><br>
        <div class="container" id="showBoard">
            <div class="row" style="background: rgba(255,255,255,0)">
              <div class="col-xs-4" id="studyboard">
                <div class="card" style="border-radius: 10px;height: 100%;">
                  <div class="card-block" style="border-radius: 8px;">
                    <h4 class="card-title" id="studyboardtitle">约学习</h4>
                    <p class="card-text" style="margin: 10px">部分"约学习"邀请展示如下：</p>
                    <div class="card" style="border: solid;height: 100%;margin-right: 3%;margin-left: 3%">
                      {{Posters.studyPoster[0].name}}
                    </div>
                  </div>
                </div>
              </div>

              <div class="col-xs-4" id="sportboard">
                <div class="card" style="border-radius: 10px;height: 100%;">
                  <div class="card-block" style="border-radius: 8px;">
                    <h4 class="card-title" id="sportboardtitle">约运动</h4>
                    <p class="card-text" style="margin: 10px">Some quick example text to build on the card title and make up the bulk of the card's content.</p>
                    <a href="#" class="btn btn-primary">Button</a>
                  </div>
                </div>
              </div>
              <div class="col-xs-4" id="outdoorboard">
                <div class="card" style="border-radius: 10px;height: 100%;">
                  <div class="card-block" style="border-radius: 8px;">
                    <h4 class="card-title" id="outdoorboardtitle">约户外</h4>
                    <p class="card-text" style="margin: 10px">Some quick example text to build on the card title and make up the bulk of the card's content.</p>
                    <a href="#" class="btn btn-primary">Button</a>
                  </div>
                </div>
              </div>
            </div>
        </div>
    </div>
  </div>
</div>
</div>
</template>


<script>
  import global_ from './Global'
  import Vue from 'vue'
  export default {
    name: 'app',
    data () {
      return {
        Posters: global_.Posters,
        studyPoster: global_.Posters.studyPoster,
        title: '约在交大',
        name: 'Homepage',
        fields: 'Computer Vision',
        routershow: true,
        items:[{text:'约学习',value:'约学习'},{text:'约运动',value:'约运动'},{text:'约户外',value:'约户外'},{text:'其他',value:'其他'}],
        selected:'',
        datum: this.datum,
        newPosterVisible: false,
        selected:'' ,
        newActivity:{
          contact: "",
          name: "",
          type: 0,      // 1：约学习 2：约户外 3：约运动 4：其他
          title: "",
          month: 0,
          day: 0,
          start_hour: 0,
          end_hour:0,
          start_minu: 0,
          end_minu: 0,
          months: [1,2,3,4,5,6,7,8,9,10,11,12],
          days: [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31]
        }
      }
    },
    methods:{
      showOutDoor: function () {
        alert("dddd")
      },
      showStudy: function(){
        alert("study posters")
      },
      showSport: function(){
        alert("show sport posters!")
      },
      showOthers: function(){
        alert("")
      },
      commitPoster:function(){
        if(selected == "约学习"){
          this.newActivity.type = 1;
        }
        else if(selected == "约户外"){
          this.newActivity.type = 2;
        }
        else if(selected == "约运动"){
          this.newActivity.type = 3;
        }
        else if(selected == "其他"){
          this.newActivity.type = 4;
        }
      },
      cancelCommitPoster:function(){
        this.newPosterVisible = false
        this.initNewPoster()
      },
      initNewPoster:function(){
        this.newActivity.contact= ""
        this.newActivity.name= ""
        this.newActivity.type= ""
        this.newActivity.title= "",
        this.newActivity.month= "月份"
        this.newActivity.day= "日期"
        this.newActivity.start_hou= ""
        this.newActivity.end_hour= ""
        this.newActivity.start_minu= ""
        this.newActivity.end_minu= ""
      }
    }
  }
</script>

<style>
  #app {
    font-family: 'Avenir', Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    color: #2c3e50;
    margin: 20px;
    text-align: center;
    background-color: rgba(1,1,1,0);
  }
  div {
    background: whitesmoke;
    text-align: center;
  }
  body{
    height: 100%;
    width: 100%;
    overflow: auto;
    background-size: 100%;
    background: whitesmoke;
    background-image: url("./assets/back.jpg");
    background-repeat: no-repeat;
    background-size: 100%, 100%;
  }
  #backboard{
    height: 600px;
    margin-bottom: 5%;
    margin-top: 5%;
    background: rgba(255, 255, 255, 0.8)
  }
  #navigator{
    width: 40%;
    height: 60px;
    margin: auto;
    position: absolute;
  }
  #showBoard{
    height: 73%;
    width: 98%;
    margin-left: 1%;
    margin-right: 1%;
    margin-top: 1%;
    background-color: whitesmoke;
    background: rgba(255, 255, 255, 0.2)
  }
  .dialog-wrap{
    border: solid gray;
    background-color: white;
    height: 500px;
  }
  .dialog-close{
    width: 20px;
    float: right;
  }
  #studyboard{
    width: 30%;
    border: solid;
    height: 420px;
    border-radius: 10px;
    padding: 0px;
    margin-left: 2.5%;
  }
  #outdoorboard{
    width: 30%;
    border: solid;
    height: 420px;
    border-radius: 10px;
    padding: 0px;
    margin-right: 2.5%;
  }
  #sportboard{
    width: 30%;
    border: solid;
    margin-right: 2.5%;
    margin-left: 2.5%;
    height: 420px;
    border-radius: 10px;
    padding: 0px;
  }
  #sportboardtitle{
    border-bottom: solid;
    background-color: lightskyblue;
    margin: 0px;
    padding: 10px;
    border-top-left-radius: 7px;
    border-top-right-radius: 7px
  }
  #studyboardtitle{
    border-bottom: solid;
    background-color: coral;
    margin: 0px;
    padding: 10px;
    border-top-left-radius: 7px;
    border-top-right-radius: 7px
  }
  #outdoorboardtitle{
    border-bottom: solid;
    background-color: greenyellow;
    margin: 0px;
    padding: 10px;
    border-top-left-radius: 7px;
    border-top-right-radius: 7px
  }
</style>
