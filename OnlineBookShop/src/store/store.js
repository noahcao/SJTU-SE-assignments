import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex);

const state={
  count: 1,
  bookInCart: 0,
  bookInfoInCart: [],
  showCart: false
}

const mutations={
  newBooksInCart(state, newList){
    state.bookInfoInCart = newList;
    state.bookInCart = newList.length;
  },
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
