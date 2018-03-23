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
        <div style="vertical-align: middle">Shopping Cart</div>
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
        <button type="button" class="btn btn-primary">Filter</button>
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


    <div id="Table" >
      <table data-toggle="table" data-sort-order="desc" class="table table-bordered" style="margin: 1.5%;
         width:97%;align-self: center">
        <thead>
        <tr>
          <th><input type="checkbox" v-model="checkAll"></th>
          <th>Book</th>
          <th>Author</th>
          <th data-sortable="true">
            Price
              <div class="btn glyphicon glyphicon-chevron-up arrowButton" @click="sortBy('price', true)"></div>
              <div class='btn glyphicon glyphicon-chevron-down arrowButton' @click="sortBy('price', false)"></div>
          </th>
          <th>Press</th>
          <th data-sortable="true">Sales
            <div class="btn glyphicon glyphicon-chevron-up arrowButton" @click="sortBy('sales', true)"></div>
            <div class='btn glyphicon glyphicon-chevron-down arrowButton' @click="sortBy('sales', false)"></div>
          </th>
        </tr>
        <tr v-for="book in books">
          <td><input type="checkbox" v-model="book.checked"></td>
          <td contentEditable="true" v-model="book.name">{{book.name}}</td>
          <td contentEditable="true" v-model="book.author">{{book.author}}</td>
          <td contentEditable="true" v-model="book.price">{{book.price}}</td>
          <td contentEditable="true" v-model="book.press">{{book.press}}</td>
          <td contentEditable="true" v-model="book.sales">{{book.sales}}</td>
        </tr>
        </thead>
      </table>
    </div>

  </div>

</template>

<style>
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
    background-color: cornflowerblue;
    height: 10%;
    width: 100%;
    margin: 0;
    font-family:"Times New Roman",Georgia,Serif;
    text-align: center;
    vertical-align: middle;
    font-weight: bold;
    color: white;
  }
  .arrowButton{
    border: solid;
    width:20%;
    padding: 0;
    height: 30px;
    align-items: center;
  }
</style>

<script>
  import global_ from './assets/books'

  export default{
    name: 'sort-by-multiple-columns',
    data() {
      return {
        allBooks: global_.books,
        books: global_.books,
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
      comparePrice(obj1, obj2){
          return obj1["price"] - obj2["price"];
      },
      compareSales(obj1, obj2){
          return obj1["sales"] - obj2["sales"];
      },
      sortBy(item, isInc){
        let newBookList = this.books;
        if(item == "price") newBookList.sort(this.comparePrice);
        if(item == "sales") newBookList.sort(this.compareSales);
        if(!isInc) newBookList.reverse();
        this.books = newBookList;
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
      },

    }
  }
</script>
