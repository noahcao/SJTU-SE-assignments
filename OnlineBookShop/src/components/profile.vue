<template>
  <div id="profilePage">
  <div class="container" id="pageContainer">
    <div class="row">
      　　
      <div class="fence col-xs-2 col-sm-2 col-md-2 col-lg-2" style="padding:0px">

        <div class="Title">
          Your Profile
        </div>

        <ul class="nav nav-tabs nav-stacked" id="leftList">
          <li class="leftCard" @click="showOrderDetails"
              style="background-color: lightcyan">
            <a href='#'>Orders</a></li>
          <li class="leftCard" @click="showProfilePanel">
            <a href='#'>Profile</a></li>
          <li class="leftCard" style="background-color: lightcyan" @click="showManagePanel"><a href='#'>Manage</a></li>
          <li class="leftCard" @click="DetailCards=Message">
            <a href='#'>MessageBox</a></li>
          <li class="btn btn-danger" style="height: 115px;width: 100%;padding-top: 30px"
          @click="logOut">Log Out</li>
        </ul>


      </div>
      　　
      <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4 fence" style="padding:0px">
        <div class="Title">
          List
        </div>

        <ul class="nav nav-tabs nav-stacked" style="overflow: scroll;height:650px;
            background-color: snow" v-if="showOrders">
          <li v-for="detail in this.orderlist" class="midCard">
            <div class="container" style="height: 100%;width: 100%;padding: 0px">
              <div class="row" style="height: 100%;width: 100%;margin: 0px">
                <div class="fence col-xs-10 col-sm-10 col-md-10 col-lg-10"
                     style="height: 100%">
                  <br>
                  <span>时间：{{detail[0]["time"]}}</span><br>
                  <span>价格：{{totalprice(detail)}}</span><br>
                </div>
                <div class="fence col-xs-2 col-sm-2 col-md-2 col-lg-2"
                     style="height: 100%;padding: 0px">
                  <button class="glyphicon glyphicon-chevron-right"
                          @click="showDetails(detail)" style="height:99%;width: 100%"></button>
                </div>
              </div>
            </div>
          </li>
        </ul>

        <div id="userprofile" v-if="showProfile">
          <div class="form-group" style="align-content: center;margin: 20px">
            <label class="col-md-2 control-label">头像</label>
            <img v-bind:src="usericon" alt="140x1400"
                 style="width:200px;height:200px;" class="img-circle" />
            <input style="margin-top: 30px;margin-left: 40px" accept="image/*" type="file" class="inline"
                   id="icon" @change="triggerFile($event)">
            <div style="padding: 40px;margin-left: 50px">
              <button @click="insertUserImg">上传</button>
              <button @click="getUserImg" style="margin-left: 20px">更新</button>
            </div>

            <div>
              <input v-model="newpassword">
              <button @click="modifyPassword">修改密码</button>
            </div>
          </div>
        </div>

        <div id="managepanel" v-if="showManage" style="text-align: center;">
            <button @click="getUserList">get user list</button>
          <ul style="text-align: center;overflow: scroll;height: 600px;margin-top: 20px" v-if="showUserList">
            <li  v-for="user in userList" style="margin-top: 10px;border: solid">
              <span>用户id：{{user["id"]}}</span><br>
              <span>用户状态：{{user["admin"]}}</span><br>
              <span>用户名：{{user["username"]}}</span><br>
              <span>用户密码：{{user["password"]}}</span><br>
              <button @click="ban(user['id'])">封禁</button>
              <button @click="free(user['id'])">解禁</button>
              <br>
              <p></p>
            </li>
          </ul>
        </div>



      </div>
      　　
      <div class="col-xs-6 col-sm-6 col-md-6 col-lg6 fence" style="overflow: scroll;padding:0px">
        <div class="Title">
          Details
        </div>
        <div style="height:20%;background-color: ghostwhite;border: double;text-align: center" v-for="book in detailbooklist">
          <br>Bookname: {{book["bookname"]}}
          <br>Number: {{book["number"]}}
          <br>Price: {{book["price"]}}
        </div>
      </div>
    </div>
  </div>
  </div>
</template>

<script>
  export default{
    data(){
      return{
        DetailCards:[],
        Orders: this.$store.state.Orders,
        Message: this.$store.state.Message,
        detailPic: "../../static/white.png",
        userid: 1,
        orderlist: {},
        detailbooklist: [],
        showOrders: false,
        showProfile: false,
        iconUpload: null,
        usericon: null,
        showManage: false,
        userList: [],
        showUserList: false,
        newpassword: ""
      }
    },
    mounted(){
      this.freshOrder();
      this.usericon = this.$store.state.usericon;
      this.$store.state.signedIn = (window.localStorage.getItem("signedin") == "signed");
      this.$store.state.userid = window.localStorage.getItem("userid");
      this.$store.state.username = window.localStorage.getItem("username");
    },
    methods:{
      totalprice(orderlist){
        var price = 0;
        for(var i = 0; i < orderlist.length; i++){
          price += orderlist[i]["price"];
        }
        return price;
      },
      showOrderDetails(){
        this.showOrders = true;
        this.DetailCards = this.Orders;
        this.showProfile = false;
        this.showManage = false;
      },
      showProfilePanel(){
        this.showProfile = true;
        this.showOrders = false;
        this.showManage = false;
      },
      showManagePanel(){
        if(window.localStorage.getItem("admin") == 1){
          this.showManage = true;
        }
        this.showProfile = false;
        this.showOrders = false;
      },
      triggerFile(event){
        alert("select a icon");
        this.iconUpload = event.target.files;
        console.log(this.iconUpload);
      },
      freshOrder(){
        var userid = window.localStorage.getItem("userid");
        var orderlist = {};
        this.$http.post("/getorders", {"userid": userid})
          .then((response) => {
            this.orders = response.data.orders;
            for(var i = 0; i < this.orders.length; i++){
              console.log(this.orders[i]);
              var time = this.orders[i]["time"];
              console.log(time);
              if(orderlist[time]){
                orderlist[time].push(this.orders[i]);
              }
              else{
                orderlist[time] = [];
                orderlist[time].push(this.orders[i]);
              }
              console.log(orderlist);
            }
            this.orderlist = orderlist;
            console.log(response.data)
          });
      },
      showDetails(detail){
        this.detailbooklist = [];
        for(var i = 0; i < detail.length; i++){
          console.log("ID is:" + detail[i]);
          var imgpath;
          console.log("Detail here TAG1");
          console.log(detail[i]);
          this.$http.post('/getbook', {"id": detail[i]["bookid"]})
            .then((response)=>{
              console.log(response.data);
              console.log("Detail here TAG2");
              console.log(detail[i]);
              imgpath = response.data["img"];
            })
          this.detailbooklist.push(detail[i]);
        }
        //console.log("detail" + this.detailbooklist);
      },
      logOut(){
        this.$store.commit("logOut");
        window.localStorage.setItem("signedin", "unsigned");
      },
      insertUserImg(){
        alert("add a icon!");
        if(this.iconUpload == null){
          alert("No img selected to be uploaded!");
          return;
        }
        var userid = window.localStorage.getItem("userid");
        var res = new FileReader();
        console.log(this.iconUpload[0]);
        res.readAsDataURL(this.iconUpload[0]);
        res.onload=()=>{
          this.$http.post('/inserticon', {
            userid: userid,
            img: res.result
          }).then((response) => {
            this.$store.state.usericon = response.img;
          })
        }
      },
      getUserImg(){
        var userid = window.localStorage.getItem("userid");
        this.$http.post('/queryicon', {"userid": userid})
          .then((response) => {
            this.$store.state.usericon = response.img;
            console.log(response.data);
            this.$store.state.usericon = response.data.img;
          })
        this.usericon = this.$store.state.usericon;
      },
      getUserList(){
        var userListGot = [];
        this.userList= [];
        this.$http.post('/getuserlist')
          .then((response) => {
            console.log(response.data.userList);
            userListGot = response.data.userList;
            for(var i = 0; i < userListGot.length; i++){
              if(userListGot[i]["admin"] != 1){
                this.userList.push(userListGot[i]);
              }
            }
          })
        this.showUserList = true;
      },
      modifyPassword(){
        alert(this.newpassword);
        var userid = window.localStorage.getItem("userid");
        this.$http.post('/changepw', {"id": userid, "password": this.newpassword})
          .then((response) => {
            alert("change the pw!");
          })
        this.newpassword="";
      },
      ban(userid){
        this.$http.post('/banfree', {"id": userid, "admin": -1})
          .then((response) => {
            alert("BAN!");
            this.getUserList();
          })
      },
      free(userid){
        this.$http.post('/banfree', {"id": userid, "admin": 0})
          .then((response) => {
            alert("FREE!");
            this.getUserList();
          })
      }
    }
  }

</script>

<style>
  @import "./../css/admin.css";
</style>
