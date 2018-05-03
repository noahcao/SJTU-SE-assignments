import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex);

const state={
  count: 1,
  bookInCart: 0,
  bookInfoInCart: [],
  showCart: false,
  signedIn: false,
  username: "",
  password: "",
  userid: -1,
  searchInfo: "",
  users:[
    {"ID": 515260910022, "username": "Caojinkun", "password": "thisisthepass"},
    {"ID": 13162096831, "username": "MyPhone", "password": "515260910022"}
  ],
  Orders:[
    {"name": "刺杀骑士团长", "price": 88.2, "author": "Xie TianYi",
      "press": "SJTU Press", "img": "@/../static/books/book1.png", "date": "2018/1/1", "tag":"order"},
    {"name": "日俄战争", "price": 88.2, "author": "Xie TianYi",
      "press": "SJTU Press", "img": "@/../static/books/book2.png", "date": "2018/1/1", "tag":"order"},
    {"name": "李敖自传", "price": 88.2, "author": "Xie TianYi",
      "press": "SJTU Press", "img": "@/../static/books/book3.png", "date": "2018/1/1", "tag":"order"},
    {"name": "见识", "price": 88.2, "author": "Xie TianYi",
      "press": "SJTU Press", "img": "@/../static/books/book4.png", "date": "2018/1/1", "tag":"order"},
    {"name": "人间词话", "price": 88.2, "author": "Xie TianYi",
      "press": "SJTU Press", "img": "@/../static/books/book5.png", "date": "2018/1/1", "tag":"order"},
    {"name": "岛上书店", "price": 88.2, "author": "Xie TianYi",
      "press": "SJTU Press", "img": "@/../static/books/book6.png", "date": "2018/1/1", "tag":"order"}
  ],
  Message: [
    {"title": "快来还花呗！", "sender": "Alibaba", "date": "2018/1/1",
      "content": "快来还花呗呀！！！！！！！", "tag":"message"},
    {"title": "快来还花呗！", "sender": "Alibaba", "date": "2018/1/1",
      "content": "快来还花呗呀！！！！！！！", "tag":"message"},
    {"title": "快来还花呗！", "sender": "Alibaba", "date": "2018/1/1",
      "content": "快来还花呗呀！！！！！！！", "tag":"message"},
    {"title": "快来还白条！", "sender": "JD.com", "date": "2018/1/1",
      "content": "快来还白条呀！！！！！！！", "tag":"message"},
    {"title": "快来还白条！", "sender": "JD.com", "date": "2018/1/1",
      "content": "快来还白条呀！！！！！！！", "tag":"message"},
    {"title": "快来还白条！", "sender": "JD.com", "date": "2018/1/1",
      "content": "快来还白条呀！！！！！！！", "tag":"message"}
  ]
}

const mutations={
  newBooksInCart(state, newList){
    state.bookInfoInCart = newList;
    state.bookInCart = newList.length;
  },
  logIn(username, password){
    state.signedIn = true;
    state.username = username;
    state.password = password;
  },
  logOut(){
    state.signedIn = false;
    state.username = "";
    state.password = "";
  },
  buyOneLess(book_input){
    var book = book_input;
    for(var i = 0; i < state.bookInfoInCart.length; i++){
      alert(book["name"]);
      alert(state.bookInfoInCart[i]["name"]);
      if(book["name"] == state.bookInfoInCart[i]["name"]){
        alert("here");
        state.bookInfoInCart[i]["number"] -= 1;
        state.bookInCart -= 1;
      }
    }
  }
}



const registor={
  users:[
    {"ID": 515260910022, "username": "Caojinkun", "password": "thisisthepass"},
    {"ID": 13162096831, "username": "MyPhone", "password": "515260910022"}
  ],
  type:[
    'Manager', 'Client'
  ]
}

export default new Vuex.Store({
  state, mutations, registor
})
