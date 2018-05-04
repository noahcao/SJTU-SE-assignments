<template>
  <div id="profilePage">
  <div class="container" id="pageContainer">
    <div class="row">
      　　
      <div class="fence col-xs-2 col-sm-2 col-md-2 col-lg-2" style="padding:0px">

        <div class="Title">
          Your Profile
        </div>

        <ul class="nav nav-tabs nav-stacked" id="leftList">
          <li class="leftCard" @click="DetailCards=Orders" style="background-color: lightcyan">
            <a href='#'>Orders</a></li>
          <li class="leftCard"><a href='#'>Profile</a></li>
          <li class="leftCard" style="background-color: lightcyan"><a href='#'>Stars</a></li>
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
            background-color: snow">
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
      }
    },
    mounted(){
      this.freshOrder();
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
      }
    }
  }

</script>

<style>
  @import "./../css/admin.css";
</style>
