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
                        <strong>author</strong>:  {{book["author"]}}
                        <br>
                        <strong>price</strong>:  {{book["price"]}}
                      </p>
                    </div>
                  </div>
                </div>
              </div>

          <p style="text-align: center">
            <button class="btn btn-success" style="margin: auto">Pay</button>
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

<style scoped>
  #bookListInCart{
    width: 100%;
    height: 230px;
    overflow: scroll;
    padding: 0px;
    text-align: center;
    margin: 30px 0px 10px 0px;
  }
  .card{
    width: 100%;
    background-color: lightskyblue;
    border-top: solid deepskyblue;
  }
  #booksInCart{
    border:solid 5px;
    border-radius: 8px;
    height: 20%;
    width: 80%;
    z-index:99;
    background-color: lightsalmon
  }
  #bookDescribe{
    background-color: whitesmoke;
    font-size: smaller;
    line-height:22px;
    padding: 2px;
    margin-bottom: 0px;
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
  import global_ from '../assets/books'
  import logo from './../assets/logo.png'
  export default{
    name: 'booTable',
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
        searchInfo: "",
        bookInCart: this.$store.state.bookInCart,
      }
    },
    methods:{
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
      addToCart(){
        if(this.checkAll){
          for(var i = 0; i < this.books.length; i++){
            this.$store.state.bookInfoInCart.push(this.books[i]);
            this.$store.state.bookInCart += 1;
          }
        }
        else{
          for(var i = 0; i < this.books.length; i++){
            if(this.books[i]["checked"]){
              this.$store.state.bookInfoInCart.push(this.books[i]);
              this.$store.state.bookInCart += 1;
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
        let newListInCart = [];

        for(var i = 0; i < this.$store.state.bookInfoInCart.length; i++){
            if(this.$store.state.bookInfoInCart[i]["name"] != book["name"]){
                newListInCart.push(book);
            }
        }
        this.$store.commit('newBooksInCart', newListInCart);
      }
    }
  }
</script>
