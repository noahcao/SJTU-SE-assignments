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
      </transition>
    </div>

    <div class="container" id="addNewBook"  v-if="newBook">
      <transition name="test">
        <div class="dialog-content" v-if="newBook">
          <p class="dialog-close" style="background-color: #42b983">
            <button class="bth btn-danger" @click="newBook=!newBook" style="font-size: xx-small;float: right">
              x </button>
          </p>
          <div style="text-align: center"><h3>New Book</h3></div><br>
          <div id="newBookReg" style="text-align: center">
            <div style="display: inline" class="newBookCol">
              Name: <input id="newBookName" v-model="newBookInfo['name']">
            </div>
            <br><br>
            <div style="display: inline" class="newBookCol">
              Author: <input id="newBookAuthor" v-model="newBookInfo['author']">
            </div>
            <br><br>
            <div style="display: inline" class="newBookCol">
              Press: <input id="newBookPress" v-model="newBookInfo['press']">
            </div>
            <br><br>
            <div style="display: inline" class="newBookCol">
              Price: <input id="newBookPrice" v-model="newBookInfo['price']">
            </div>
          </div><br>
          <p style="text-align: center">
            <button class="btn btn-success" style="margin: auto" @click="submitNewBook">Submit</button>
          </p>
        </div>
      </transition>
    </div>


    <!-- Part2: Header Block -->

    <div id="header">
      <div id="toolbar">
        <button type="button" class="btn btn-primary" @click="addBook">Add</button>
        <button type="button" class="btn btn-warning" @click="deleteBooks">Delete</button>
        <button type="button" class="btn btn-primary" @click="addToCart">Purchase</button>
        <div style="display: inline;margin-left: 20%">
          <input v-model="bottomPrice" placeholder="lowest" style="width: 10%">-
          <input v-model="topPrice" placeholder="highest" style="width: 10%">
        </div>
        <button type="button" class="btn btn-primary" @click="filterByPrice">Filter</button>
        <a style="margin-left: 5%">In stock: <input type="checkbox" v-model="showInStock"></a>
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
        <tr v-for="book in displayBooks">
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
  #addNewBook{
    position: absolute;
    height: 320px;
    border:solid lightblue 3px;
    padding-top: 0px;
    border-radius: 10px;
    width: 30%;
    margin-left: 40%;
    margin-right: 40%;
    margin-top: 10%;
    z-index:9;
    background-color: white;
  }
  .newBookCol{
    margin-top: 20px;
    padding: 1px;
  }
</style>

<script>
  import global_ from './assets/books'

  export default{
    name: 'sort-by-multiple-columns',
    data() {
      return {
        newBookInfo:{
            "name": "",
            "price": "",
            "press": "",
            "author": "",
            "sales": 0,
            "checked": false
        },
        newBook: false,
        showInStock:false,
        bottomPrice: "",
        topPrice: "",
        allBooks: global_.books,
        books: global_.books,
        displayBooks: global_.books,
        sorttype:1,
        sortparam:"",
        arrayData:[],
        arrayDataAll:[],
        newRow:{},
        checkAll: false,
        checkedNum: 0,
        cart: [],
        showCart:false,
        searchInfo: "",
        bookInCart: 0
      }
    },
    methods:{

      // 单元格编辑回调
      cellEditDone(newValue,oldValue,rowIndex,rowData,field){

        this.tableData[rowIndex][field] = newValue;

        // 接下来处理你的业务逻辑，数据持久化等...
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
        this.displayBooks = newBookList;
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
      deleteBooks(){
          var newBooks = [];
          for(var i = 0; i < this.books.length; i++){
              if(!this.books[i]["checked"]) newBooks.push(this.books[i]);
          }
          this.books = newBooks;
          this.displayBooks = this.books;
      },
      search(query){

      },
      addBook(){
        this.newBook = !this.newBook;
      },
      filterByPrice(){
          var bottomPrice = this.bottomPrice;
          var topPrice = this.topPrice;
          var newBookList = [];
          for(var i = 0; i < this.books.length; i++){
              if((this.books[i]["price"] >= bottomPrice)&&(this.books[i]["price"]
                  <= topPrice)){
                  newBookList.push(this.books[i])
              }
          }
          this.displayBooks = newBookList;
          this.bottomPrice = "";
          this.topPrice = "";
      },
      showInStock(){

      },
      submitNewBook(){
          if((this.newBookInfo["name"]!="")&&(this.newBookInfo["press"]!=0)&&
            (this.newBookInfo["price"]>0)&&(this.newBookInfo["author"]!="")){
              this.books.push(this.newBookInfo);
          }
          this.newBookInfo = {
            "name": "",
            "price": "",
            "press": "",
            "author": "",
            "sales": 0,
            "checked": false
          };
          this.newBook = false;
      },
      searchBook(){
          var newDisplayBooks = [];
          for(var i = 1; i < this.books.length; i++){
              var book = this.books[i];
              if(((book["name"].search(this.searchInfo) != -1) || (book["press"].search(this.searchInfo) != -1)
                  || (book["author"].search(this.searchInfo) != -1))){
                  newDisplayBooks.push(book);
              }
          }
          this.displayBooks = newDisplayBooks;
      }
    }
  }
</script>
