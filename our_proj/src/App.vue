  <template>
    <div id="app">
      <div id='backboard'>
        <br>
        <h1>{{title}}</h1>
        <br>
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
                <p class="dialog-close" @click="newPosterVisible=false">x</p>
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
                <input style="width:80px" v-model="newActivity.start"> ：
                <input style="width:80px" v-model="newActivity.end">
                <br><br> 
                <button type="button" class="btn btn-success">提交</button>
                <button type="button" class="btn btn-danger">取消</button>
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
  import Vue from 'vue'
  export default {
    name: 'app',
    data () {
      return {
        title: '约在交大',
        name: 'Homepage',
        fields: 'Computer Vision',
        routershow: true,
        items:[{text:'约学习',value:'约学习'},{text:'约运动',value:'约运动'},{text:'约户外',value:'于户外'},{text:'其他',value:'其他'}],
        selected:'',
        datum: this.datum,
        newPosterVisible: false,
        selected:'' ,
        background: {
          backimage: "url(" + require('./assets/SJTUbackground.gif') + ")",
          backrepeat: "no-repeat",
          backsize: "100%, 100%"
        },
        newActivity:{
          contact: "",
          name: "",
          type: "",
          title: "",
          month: "月份",
          day: "日期",
          start: "开始时间",
          end: "结束时间",
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
