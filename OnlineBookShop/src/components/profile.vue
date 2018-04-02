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
          <li v-for="detail in DetailCards" class="midCard">
            <div class="container" style="height: 100%;width: 100%;padding: 0px">
              <div class="row" style="height: 100%;width: 100%;margin: 0px">
                <div class="fence col-xs-10 col-sm-10 col-md-10 col-lg-10"
                     style="height: 100%">
                  <br>
                  <span>书名：{{detail["name"]}}</span><br>
                  <span>购买日期：{{detail["date"]}}</span><br>
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
      　　
      <div class="col-xs-6 col-sm-6 col-md-6 col-lg6 fence" style="padding:0px">
        <div class="Title">
          Details
        </div>
        <div style="height:650px;background-color: ghostwhite;border: double">
          <img :src="detailPic" style="height: 100%;width: 100%">
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
      }
    },
    methods:{
      showDetails(detail){
        this.detailPic = detail["img"];
      },
      logOut(){
        this.$store.commit("logOut");
      }
    }
  }

</script>

<style>
  @import "./../css/admin.css";
</style>
