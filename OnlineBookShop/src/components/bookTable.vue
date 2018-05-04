<template>
  <div id="booktable">
    <!-- Part1: The top navigator bar -->
    <!-- Part0: scope of shopping cart-->
    <div class="container" id="shoppingCart"  v-if="$store.state.showCart">
      <transition name="test">
        <div class="dialog-content" v-if="$store.state.showCart">
          <p class="dialog-close">
            <button class="bth btn-danger" @click="$store.state.showCart=!$store.state.showCart" style="font-size: xx-small;float: right">
              x </button>
          </p>
          <div style="text-align: center;">
            <h4 >Books in Cart</h4>
          </div>

              <div class="modal-body" id="bookListInCart">
                <div v-for="book in $store.state.bookInfoInCart" style="width: 100%;">
                  <div class="card">
                    <div class="card-block">
                      <div class="card-text">
                        <strong>{{book["name"]}}</strong>
                        <button style="float: right" @click="deleteBookInCart(book)">x</button>
                      </div>
                      <p v-if="$store.state.showCart" id="bookDescribe" style="margin-top: 5px">
                        <div class="container" id="blockInCart">
                          <div class="row">
                            <div class="col-xs-2 col-sm-2 col-md-2 col-lg-2" style="padding:0px">
                              <img :src="book.img" style="width: 100%">
                            </div>
                            <div class="col-xs-10 col-sm-10 col-md-10 col-lg-10" style="padding:0px">
                              <strong>author</strong>:  {{book["author"]}}
                              <br>
                              <strong>price</strong>:  {{book["price"]}}
                              <br>
                              <strong>number</strong>:  {{book["number"]}}
                              <a class="glyphicon glyphicon-plus"
                                 @click="buyOneMore(book)"></a>
                              <a class="glyphicon glyphicon-minus"
                                 @click="buyOneLess(book)"></a>
                              <br>
                              <strong>total price:</strong>: {{mul(book["number"], book["price"])}}
                            </div>
                          </div>
                       </div>
                      </p>
                    </div>
                  </div>
                </div>
              </div>


          <p style="text-align: center">Total Price: {{totalPriceInCart}}</p>
          <p style="text-align: center">
            <button class="btn btn-success" style="margin: auto" @click="pay">Pay</button>
          </p>
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
              Name: <input v-model="newBookInfo['name']">
            </div>
            <br><br>
            <div style="display: inline" class="newBookCol">
              Author: <input v-model="newBookInfo['author']">
            </div>
            <br><br>
            <div style="display: inline" class="newBookCol">
              Press: <input v-model="newBookInfo['press']">
            </div>
            <br><br>
            <div style="display: inline" class="newBookCol">
              Price: <input v-model="newBookInfo['price']">
            </div><br><br>
            <div style="display: inline" class="newBookCol">
              Number: <input v-model="newBookInfo['number']">
            </div><br><br>
            <div style="display: inline" class="newBookCol">
              Year: <input v-model="newBookInfo['year']">
            </div>
          </div><br>
          <p style="text-align: center">
            <button class="btn btn-success" style="margin: auto" @click="submitNewBook">Submit</button>
          </p>
        </div>
      </transition>
    </div>


    <!-- Part2: Header Block -->
    <div id="myCarousel" class="carousel slide" style="height: 100px">
      <!-- 轮播（Carousel）指标 -->
      <ol class="carousel-indicators">
        <li data-target="#myCarousel" data-slide-to="0" class="active"></li>
        <li data-target="#myCarousel" data-slide-to="1"></li>
        <li data-target="#myCarousel" data-slide-to="2"></li>
      </ol>
      <!-- 轮播（Carousel）项目 -->
      <div class="carousel-inner">
        <div class="item active">
          <img src="../../static/roll1.jpg" alt="First slide" style="width: 100%;height: 98px">
        </div>
        <div class="item">
          <img src="../../static/roll2.jpeg" alt="Second slide" style="width: 100%;height: 98px">
        </div>
        <div class="item">
          <img src="../../static/roll3.jpg" alt="Third slide" style="width: 100%;height: 98px">
        </div>
      </div>
      <!-- 轮播（Carousel）导航 -->
      <a class="carousel-control left" href="#myCarousel"
         data-slide="prev"><span _ngcontent-c3="" aria-hidden="true" class="glyphicon glyphicon-chevron-left"></span></a>
      <a class="carousel-control right" href="#myCarousel"
         data-slide="next"><span _ngcontent-c3="" aria-hidden="true" class="glyphicon glyphicon-chevron-right"></span></a>
    </div>

    <div style="height: 50px;margin: 10px;padding-left:20px;padding-right: 20px">
      <div class="form-group" style="width: 60%;float: left">
        <input type="text" class="form-control" placeholder="Describe book" v-model="$store.state.searchInfo"
               style="width: 50%;margin-left: 40%">
      </div>
      <button type="submit" class="btn btn-default" @click="searchBook">Search</button>
    </div>


    <div id="header">
      <div id="toolbar">
        <button type="button" class="btn btn-primary" @click="newBook=!newBook" v-if="this.$store.state.isadmin">Add</button>
        <button type="button" class="btn btn-warning" @click="deleteBooks" v-if="this.$store.state.isadmin">Delete</button>
        <button type="button" class="btn btn-primary" @click="addToCart">Purchase</button>
        <div style="display: inline;margin-left: 20%">
          <input v-model="bottomPrice" placeholder="lowest" style="width: 10%">-
          <input v-model="topPrice" placeholder="highest" style="width: 10%">
        </div>
        <button type="button" class="btn btn-primary" @click="filterByPrice">Filter</button>
        <div class="btn-group" style="float: right">
          <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
            Export
            <span class="caret"></span>
          </button>
          <ul class="dropdown-menu">
            <li><a href="#" @click="jsonDownload">.JSON</a></li>
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
      <table class="table table-striped" style="margin: 1.5%;
         width:97%;align-self: center">
        <thead>
        <tr>
          <th></th>
          <th style="text-align: center">Book</th>
          <th style="text-align: center">Author</th>
          <th data-sortable="true" style="text-align: center">
            Price
            <div class="btn glyphicon glyphicon-chevron-up arrowButton" @click="sortBy('price', true)"></div>
            <div class='btn glyphicon glyphicon-chevron-down arrowButton' @click="sortBy('price', false)"></div>
          </th>
          <th style="text-align: center">Press</th>
          <th data-sortable="true" style="text-align: center">Sales
            <div class="btn glyphicon glyphicon-chevron-up arrowButton" @click="sortBy('sales', true)"></div>
            <div class='btn glyphicon glyphicon-chevron-down arrowButton' @click="sortBy('sales', false)"></div>
          </th>
        </tr>



        <tr v-for="book in displayBooks" style="text-align: center">
          <td><input type="checkbox" v-model="book.checked"></td>
          <td contentEditable="true" v-model="book.name">
            <div class="container" style="width:100%">
              <div class="row">
                <div class="col-xs-2 col-sm-2 col-md-2 col-lg-2" style="padding:0px">
                  <img :src="book.img" id="bookImg">
                </div>
                <div class="col-xs-10 col-sm-10 col-md-10 col-lg-10" style="padding:0px">
                  {{book.name}}
                </div>
              </div>
            </div>
          </td>
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

<script>
  import global_ from '../assets/books'
  export default{
    name: 'booTable',
    data() {
      return {
        newBookInfo:{
          "name": "",
          "price": "",
          "press": "",
          "author": "",
          "number": "",
          "sales": 0,
          "year": ""
        },
        logo: '../assets/logo.png',
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
        bookidInCart: [],
        bookInCart: this.$store.state.bookInCart,
        bookList: [],
      }
    },
    mounted(){
      this.freshCart();
      this.$store.state.isadmin = (window.localStorage.getItem("admin") == 1);
    },
    computed:{
      totalPriceInCart: function(){
        var price = 0;
        for(var i = 0;  i < this.$store.state.bookInfoInCart.length; i++){
          var book = this.$store.state.bookInfoInCart[i];
          price += book["price"] * book["number"];
        }
        return price;
      }
    },
    methods:{
      freshCart(){
        this.$http.post('/getbooks')
          .then((response) => {
            this.books = response.data.books;
            for(var i = 0; i < this.books.length; i++){
              this.books["checked"] = false;
            }
            this.displayBooks = this.books;
            this.$store.state.signedIn = (window.localStorage.getItem("signedin") == "signed");
            this.$store.state.userid = window.localStorage.getItem("userid");
            this.$store.state.username = window.localStorage.getItem("username");
            console.log("already get books info!");
            if(this.$store.state.signedIn){
              // get cart list for the logged-in user
              this.$http.post('/getcart', {"userid": this.$store.state.userid})
                .then((response) => {
                  this.bookidInCart = response.data;
                  this.$store.state.bookInfoInCart = response.data.booksInCart;
                  this.$store.state.bookInCart = this.$store.state.bookInfoInCart.length;
                })
            }
          });
      },
      comparePrice(obj1, obj2){
        return obj1["price"] - obj2["price"];
      },
      compareSales(obj1, obj2){
        return obj1["sales"] - obj2["sales"];
      },
      sortBy(item, isInc){
        let newBookList = this.displayBooks;
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
      mul(var1, var2){
        return var1 * var2;
      },
      addNumberToCart(number, book, bookList){
          for(var i = 0; i < bookList.length; i++){
              if(number > 0){
                if(bookList[i]["name"] == book["name"]){
                  if(bookList[i].hasOwnProperty("number")){
                    bookList[i]["number"] += number;
                  }
                  else{
                    bookList[i]["number"] = number;
                  }
                  this.$store.state.bookInfoInCart = bookList;
                  return;
                }
              }
              else{
                if(bookList[i]["name"] == book["name"]){
                    if(bookList[i]["number"] > number){
                        bookList[i]["number"] -= number;
                      this.$store.state.bookInfoInCart = bookList;
                     }
                     else{
                        this.deleteBookInCart(book);
                    }
                    return;
                }
              }
          }
          if(number > 0){
            book["number"] = number;
            bookList.push(book);
            this.$store.state.bookInfoInCart = bookList;
          }
      },
      buyOneMore(book){
        var userid = window.localStorage.getItem("userid");
        var bookid = book["id"];
        this.$http.post("/onemorecart", {"userid": userid, "bookid": bookid})
          .then((response) => {
            console.log(response.data);
            console.log("add a book to cart");
          });
        for(var i = 0; i < this.$store.state.bookInfoInCart.length; i++){
          if(this.$store.state.bookInfoInCart[i]["name"] == book["name"]){
              var newList = [];
              for(var j = 0; j < this.$store.state.bookInfoInCart.length; j++){
                if(i != j){
                  newList.push(this.$store.state.bookInfoInCart[j]);
                }
                else{
                  this.$store.state.bookInfoInCart[j]["number"] += 1;
                  newList.push(this.$store.state.bookInfoInCart[j]);
                }
              }
              this.$store.state.bookInfoInCart = newList;
              return;
            }
        }

      },
      buyOneLess(book){
        var userid = window.localStorage.getItem("userid");
        var bookid = book["id"];
        this.$http.post("/onelesscart", {"userid": userid, "bookid": bookid})
          .then((response) => {
            console.log("delete a book to cart");
          });
        for(var i = 0; i < this.$store.state.bookInfoInCart.length; i++){
            if(this.$store.state.bookInfoInCart[i]["name"] == book["name"]){
                if(this.$store.state.bookInfoInCart[i]["number"] == 1){
                    this.deleteBookInCart(book);
                    return;
                }
                else{
                    var newList = [];
                    for(var j = 0; j < this.$store.state.bookInfoInCart.length; j++){
                        if(i != j){
                            newList.push(this.$store.state.bookInfoInCart[j]);
                        }
                        else{
                            this.$store.state.bookInfoInCart[j]["number"] -= 1;
                            newList.push(this.$store.state.bookInfoInCart[j]);
                        }
                    }
                    this.$store.state.bookInfoInCart = newList;
                    return;
                }
            }
        }
      },
      pay(){
        var userid = window.localStorage.getItem("userid");
        this.$http.post('/pay', {"userid": userid})
          .then((response) => {
            this.$store.state.bookInfoInCart = [];
            this.$store.state.bookInCart = 0;
            this.freshCart();
          });
      },
      addToCart(){
          for(var i = 0; i < this.books.length; i++){
            if(this.books[i]["checked"]){
              this.addNumberToCart(1, this.books[i], this.$store.state.bookInfoInCart);
              this.$store.state.bookInCart += 1;
              var bookid = this.books[i]["id"];
              var userid = this.$store.state.userid;
              this.$http.post('/addcart', {userid: userid, bookid: bookid, number: 1})
                .then((response) => {
                  this.freshCart();
                });
            }
          }
        for(var i = 0; i < this.books.length; i++){
          this.books[i]["checked"] = false;
        }
      },
      deleteBooks(){
        var newBooks = [];
        for(var i = 0; i < this.books.length; i++){
          if(!this.books[i]["checked"]) newBooks.push(this.books[i]);
          else{
            console.log(this.books[i]);
            console.log(this.books[i]["id"]);
            this.$http.post('/delbook', {"id": this.books[i]["id"]})
              .then((response) => {
                this.freshCart();
              });
          }
        }
        this.books = newBooks;
        this.displayBooks = this.books;
      },
      filterByPrice(){
        var bottomPrice = this.bottomPrice;
        var topPrice = this.topPrice;
        var newBookList = [];
        if((this.bottomPrice=="")&&(this.topPrice=="")){
          this.displayBooks = this.books;
          return true;
        }
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
      submitNewBook(){
        this.$http.post('addbook', {"name": this.newBookInfo["name"],
          "author": this.newBookInfo["author"], "price": this.newBookInfo["price"],
        "number": this.newBookInfo["number"], "year": this.newBookInfo["year"],
        "press": this.newBookInfo["press"], "sales": this.newBookInfo["sales"]})
          .then((response) => {
            console.log("add a new book into booklist.");
          });

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
        for(var i = 0; i < this.books.length; i++){
          var book = this.books[i];
          if(((book["name"].search(this.$store.state.searchInfo) != -1) ||
            (book["press"].search(this.$store.state.searchInfo) != -1)
            || (book["author"].search(this.$store.state.searchInfo) != -1))){
            newDisplayBooks.push(book);
          }
        }
        this.displayBooks = newDisplayBooks;
      },
      funDownload (content, filename) {
        var eleLink = document.createElement('a')
        eleLink.download = filename
        eleLink.style.display = 'none'
        var blob = new Blob([content])
        eleLink.href = URL.createObjectURL(blob)
        document.body.appendChild(eleLink)
        eleLink.click()
        document.body.removeChild(eleLink)
      },
      jsonDownload () {
        var json = JSON.stringify(this.books)
        this.funDownload(json, 'books.json')
      },
      deleteBookInCart(book){
        var userid = window.localStorage.getItem("userid");
        var bookid = book["id"];
        this.$http.post('delcart', {"userid": userid, "bookid": bookid})
          .then((response) => {
            console.log("delete a book from cart!");
          });
        let newListInCart = [];

        for(var i = 0; i < this.$store.state.bookInfoInCart.length; i++){
            if(this.$store.state.bookInfoInCart[i]["name"] != book["name"]){
                newListInCart.push(this.$store.state.bookInfoInCart[i]);
            }
            else{
                if(book["number"]>1){
                    book["number"] -= 1;
                    newListInCart.push(book);

                }
            }
        }
        this.$store.commit('newBooksInCart', newListInCart);
      }
    }
  }
</script>

<style scoped>
  @import "./../css/bookTable.css";
</style>
