<template>
  <div id="header">
  <nav class="navbar navbar-default" id="topNav">
    <div class="container-fluid">
      <!-- Brand and toggle get grouped for better mobile display -->
      <div class="navbar-header">
        <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#topNav" aria-expanded="false">
          <span class="sr-only">Toggle navigation</span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
        </button>
        <a class="navbar-brand" href="#" style="padding: 0px">
          <router-link to="/homePage">
            <img src="./../assets/logo_big.png" id="logoPic">
          </router-link>
        </a>
      </div>

      <!-- Collect the nav links, forms, and other content for toggling -->
      <div class="collapse navbar-collapse">
        <ul class="nav navbar-nav" style="width: 30%">
          <li style="font-weight: bold;width: 50%"><a href="#">
            My Account
            <span class="sr-only">(current)</span></a></li>
          <li class="dropdown" style="width: 30%">
            <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true"
               aria-expanded="false">Books<span class="caret"></span></a>
            <ul class="dropdown-menu" id="bookTypes">
              <li><router-link to="/bookTable">全部类目</router-link></li>
              <li><a href="#">人文社科</a></li>
              <li><a href="#">教辅教材</a></li>
              <li><a href="#">文学小说</a></li>
              <li><a href="#">科学前沿</a></li>
              <li><a href="#">儿童图书</a></li>
              <li role="separator" class="divider"></li>
              <li><a href="#">特价图书</a></li>
              <li role="separator" class="divider"></li>
              <li><a href="#">需求提交</a></li>
            </ul>
          </li>
        </ul>
        <form class="navbar-form navbar-left" style="width: 30%;padding: 0px">
          <div class="form-group" style="width: 60%;float: left">
            <input type="text" class="form-control" placeholder="Describe book" v-model="searchInfo" style="width: 100%">
          </div>
          <button type="submit" class="btn btn-default" @click="searchBook" style="float: right">Search</button>
        </form>
        <ul class="nav navbar-nav navbar-right">
          <button type="submit" class="btn btn-info" style="margin-top: 8px" @click="showCart=!showCart">Shopping Cart ({{bookInCart}})</button>
        </ul>
      </div><!-- /.navbar-collapse -->
    </div><!-- /.container-fluid -->
  </nav>
  </div>
</template>

<script>
  import bookTable from '@/components/bookTable.vue'
  import homePage from '@/components/homePage.vue'
  export default{
      data(){
          return{
            searchInfo: "",
            showCart: false,
            bookInCart: 0,
          }
      },
      methods:{
          searchBook(){
              alert("search a book");
          }
      },
      components:{
          bookTable, homePage
      }
  }

</script>

<style>

  #header{
    z-index: 99;
    padding: 0px;
  }
  #topNav{
    margin: 0px;
  }
  #logoPic{
    height: 90%;
  }
</style>
