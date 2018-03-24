<template>
  <nav class="navbar navbar-default">
    <div class="container-fluid">
      <!-- Brand and toggle get grouped for better mobile display -->
      <div class="navbar-header">
        <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#topNav" aria-expanded="false">
          <span class="sr-only">Toggle navigation</span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
        </button>
        <a class="navbar-brand" href="#">
          OnlineBookShop
        </a>
      </div>

      <!-- Collect the nav links, forms, and other content for toggling -->
      <div class="collapse navbar-collapse" id="topNav">
        <ul class="nav navbar-nav">
          <li style="font-weight: bold"><a href="#">Sign in<span class="sr-only">(current)</span></a></li>
          <li class="dropdown">
            <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">
              图书类型<span class="caret"></span></a>
            <ul class="dropdown-menu">
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
        <form class="navbar-form navbar-left">
          <div class="form-group">
            <input type="text" class="form-control" placeholder="图书信息" v-model="searchInfo">
          </div>
          <button type="submit" class="btn btn-default" @click="searchBook">Search</button>
        </form>
        <ul class="nav navbar-nav navbar-right">
          <button type="submit" class="btn btn-info" style="margin-top: 6px" @click="showCart=!showCart">Shopping Cart ({{bookInCart}})</button>
        </ul>
      </div><!-- /.navbar-collapse -->
    </div><!-- /.container-fluid -->
  </nav>

</template>

<script>
  export default{
      data(){
          return{
            searchInfo: "",
            showCart: false,
            bookInCart: 0
          }
      },
    methods:{
          searchBook(){
              alert("search a book");
          }
    }
  }

</script>
