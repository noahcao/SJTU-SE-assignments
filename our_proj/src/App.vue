  <template>
    <div id="app">
      <div id='backboard'>
        <br>
        <h1>{{title}}</h1>
        <br>
        {{Posters.otherPoster[0].name}}
        <div id="board">
          <div id="navigator">
            <nav class="navbar navbar-default" role="navigation">
              <div class="container-fluid"> 
                <div class="navbar-header">
                 <a class="navbar-brand" href="#">导航菜单</a>
               </div>
               <div>
                <ul class="nav navbar-nav">
                  <li><a href="#" @click="newPosterVisible=true">新活动</a></li>
                  <li><a href="#">搜索活动</a></li>
                  <li class="dropdown">
                    <a href="#" class="dropdown-toggle" data-toggle="dropdown">
                      分类查看
                      <b class="caret"></b>
                    </a>
                    <ul class="dropdown-menu">
                      <li><a href="#">约学习</a></li>
                      <li><a href="#">约运动</a></li>
                      <li><a v-on:click="showOutDoor" href="#">约户外</a></li>
                    </ul>
                  </li>
                </ul>
              </div>
            </div>
          </nav>
          <div class="dialog-wrap" v-if="newPosterVisible">
            <div class="dialog-cover" v-if="newPosterVisible"></div>
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
          end_hour: 0,
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
        alert("dddddd")
      },
      commitPoster:function(){
        alert("commit a new poster!")
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
  h1{
    font-family: "Arial","Microsoft YaHei",sans-serif;
    text-align: center;
    font-size: 300%;
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
  .background{
    position:absolute; top:0; left:0;   
    position: relative;
    z-index:-1;  
    　　filter:blur(10px);
    　  over-flow: hidden;   
  }
  #backboard{
    height: 500px;
    margin-bottom: 10px;
  }
  #navigator{
    width: 60%;
    height: 60px;
    margin: auto;
    position: absolute;
    align-self: center;
    margin-left: 20%;
    margin-right: 20%;
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
</style>
