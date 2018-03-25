import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex);

const state={
  count: 1
}

const mutations={
  add(state){
    state.count +=1;
  },
  reduce(state){
    state.count +=1;
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
