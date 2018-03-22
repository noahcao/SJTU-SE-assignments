<template>
  <div id="mainPage">

    <!-- Part1: The top navigator bar -->
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
          <a class="navbar-brand" href="#">OnlineBookShop</a>
        </div>

        <!-- Collect the nav links, forms, and other content for toggling -->
        <div class="collapse navbar-collapse" id="topNav">
          <ul class="nav navbar-nav">
            <li style="font-weight: bold"><a href="#">Sign in<span class="sr-only">(current)</span></a></li>
            <li class="dropdown">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown" role="button" aria-haspopup="true" aria-expanded="false">图书类型<span class="caret"></span></a>
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
              <input type="text" class="form-control" placeholder="图书信息">
            </div>
            <button type="submit" class="btn btn-default">Search</button>
          </form>
          <ul class="nav navbar-nav navbar-right">
            <button type="submit" class="btn btn-info" style="margin-top: 6px" @click="showCart=!showCart">Shopping Cart</button>
          </ul>
        </div><!-- /.navbar-collapse -->
      </div><!-- /.container-fluid -->
    </nav>

    <!-- Part0: scope of shopping cart-->
    <div class="container" v-if="showCart" id="shoppingCart">
      <div id="cartHeader">
        购物车
      </div>
      <transition name="shop">
          <div class="dialog-content" v-for="book in cart" style="background-color: lightsalmon;line-height: 30px">
            <div>
              {{book.name}}
            </div>
          </div>
        <button class="modal-close">ddasdsada</button>
      </transition>
    </div>


    <!-- Part2: Header Block -->

    <div id="header">
      <div id="toolbar">
          <button type="button" class="btn btn-primary">Refresh</button>
          <button type="button" class="btn btn-primary" @click="addToCart">Add to cart</button>
          <div class="btn-group" style="float: right">
            <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
              Export
              <span class="caret"></span>
            </button>
            <ul class="dropdown-menu">
              <li><a href="#">.JSON</a></li>
              <li><a href="#">.XML</a></li>
              <li><a href="#">.CSV</a></li>
              <li><a href="#">.PDF</a></li>
              <li><a href="#">.TXT</a></li>
              <li><a href="#">.SQL</a></li>
            </ul>
          </div>
      </div>
    </div>


    <!-- Part3: Table-->
    <div id="Table" >
      <table data-toggle="table" class="table table-bordered" style="margin: 1.5%;
         width:97%;align-self: center">
        <thead>
        <tr>
          <th><input type="checkbox" v-model="checkAll"></th>
          <th>Book</th>
          <th>Author</th>
          <th v-on:click="sortBy('price')">Price</th>
          <th>Press</th>
          <th v-on:click="sortBy('sales')">Sales</th>
        </tr>
        <tr v-for="book in books">
          <td><input type="checkbox" v-model="book.checked"></td>
          <td>{{book.name}}</td>
          <td>{{book.author}}</td>
          <td>{{book.price}}</td>
          <td>{{book.press}}</td>
          <td>{{book.sales}}</td>
        </tr>
        </thead>
      </table>
    </div>

  </div>

</template>

<style>
  .cell-edit-color{
    color:#2db7f5;
    font-weight: bold;
  }
  #header{
    margin: 10px;
    padding: 5px;
  }
  #shoppingCart{
    height: 350px;
    border: solid;
    width: 40%;
    margin-top: 10%;
    margin-left: 30%;
    position: absolute;
    border-radius: 8px;
    background-color: whitesmoke;
    padding: 0;
  }
  #cartHeader{
    border-top-right-radius: 5px;
    border-top-left-radius: 5px;
    background-color: orange;
    width: 100%;
    margin: 0;
  }
</style>

<script>

  export default{
    data() {
      return {
        books: [
          {"name":"赵伟","author":"156*****1987","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"李伟","author":"182*****1538","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"孙伟","author":"161*****0097","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"周伟","author":"197*****1123","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"吴伟","author":"183*****6678","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"赵伟","author":"156*****1987","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"李伟","author":"182*****1538","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"孙伟","author":"161*****0097","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"周伟","author":"197*****1123","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false},
          {"name":"吴伟","author":"183*****6678","price":"钢琴、书法、唱歌","press":"华夏出版社","sales":1000,"checked":false}
        ],

        sorttype:1,
        sortparam:"",
        arrayData:[],
        arrayDataAll:[],
        newRow:{},
        checkAll: false,
        checkedNum: 0,
        cart: [],
        showCart:false
      }
    },
    methods:{

      // 单元格编辑回调
      cellEditDone(newValue,oldValue,rowIndex,rowData,field){

        this.tableData[rowIndex][field] = newValue;

        // 接下来处理你的业务逻辑，数据持久化等...
      },

      sortChange(params){
        console.log(params)
      },
      sortBy: function(sortparam){
          this.sortparam = sortparam;
          this.sorttype = this.sorttype == -1 ? 1: -1;
      },
      check(book){
        if(book.checked){
            book.checked = !book.checked;
            this.checkedNum -= 1;
        }
        else{
            book.checked = !book.checked;
            this.checkedNum += 1;
        }
      },
      addToCart(){
        if(this.checkAll){
            for(var i = 0; i < this.books.length; i++){
                this.cart.push(this.books[i]);
            }
        }
        else{
            for(var i = 0; i < this.books.length; i++){
                if(this.books[i]["checked"]){
                    this.cart.push(this.books[i]);
                }
            }
        }
        this.checkAll = false;
        for(var i = 0; i < this.books.length; i++){
            this.books[i]["checked"] = false;
        }
      }
    }
  }
</script>
