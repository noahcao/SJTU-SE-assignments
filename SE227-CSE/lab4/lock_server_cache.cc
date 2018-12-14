// the caching lock server implementation

#include "lock_server_cache.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "lang/verify.h"
#include "handle.h"
#include "tprintf.h"


lock_server_cache::lock_server_cache():
  nacquire (0)
{
  pthread_mutex_init(&mutex, NULL);
}


int lock_server_cache::acquire(lock_protocol::lockid_t lid, std::string id, 
                               int &)
{
  pthread_mutex_lock(&mutex);
  tprintf("server(%s): aqcuire lock(%lld)\n",id.c_str() ,lid);
  lock_protocol::status ret = lock_protocol::OK;
  std::string lock_host;
  //construct rpc call with client if needed
  // if(clients.find(id) == clients.end()){
  //   handle h(id);
  //   rpcc *cl = h.safebind();
  //   if(cl){
  //     clients[id] = cl;
  //   } else {
  //     tprintf("server(%s): bind rpc client failed\n", id.c_str());
  //   }
  // }
  // construct the info of lock if needed
  if(locks.find(lid) == locks.end()){
    locks[lid].aquired = false;
    locks[lid].id = id;
  }
  lock& lock = locks[lid];
  if(lock.aquired){
    tprintf("server(%s): aquire a aquired lock, retry\n", id.c_str());
    lock.wait_list.insert(id);
    lock_host = lock.id;
    ret = lock_protocol::RETRY;
    pthread_mutex_unlock(&mutex);
    //rlock_protocol::status rret = clients[lock_host]->call(rlock_protocol::revoke, lid, rret);
    int rret;
		rret = handle(lock_host).safebind()->call(rlock_protocol::revoke, lid, rret);
    tprintf("server(%s): rpc(%s) revoke lock(%lld)\n",id.c_str() ,lock_host.c_str() ,lid);
    // tprintf("server(%s): rret = %d\n",id.c_str() ,rret);
    // tprintf("server(%s): rlock_protocol::OK = %d\n",id.c_str() ,rlock_protocol::OK);
    //VERIFY(rret == rlock_protocol::OK);
    return ret;
  }
  else{
    tprintf("server(%s): aquire a free lock\n", id.c_str());
    lock.aquired = true;
    lock.id = id;
    ret = lock_protocol::OK;
    lock.wait_list.erase(id);
    if (lock.wait_list.size()) {
        lock_host = lock.id;
        pthread_mutex_unlock(&mutex);
        rlock_protocol::status rret = handle(lock_host).safebind()->call(rlock_protocol::revoke, lid, rret);
        tprintf("server(%s): rpc(%s) revoke lock(%lld)\n",id.c_str() ,lock_host.c_str() ,lid);
        VERIFY(rret == rlock_protocol::OK);
        return ret;
    }
  }
  
  pthread_mutex_unlock(&mutex);
  return lock_protocol::OK;
}

int 
lock_server_cache::release(lock_protocol::lockid_t lid, std::string id, 
         int &r)
{
  pthread_mutex_lock(&mutex);
  tprintf("server(%s): release lock(%lld)\n",id.c_str() ,lid);
  lock_protocol::status ret = lock_protocol::OK;
  if(locks.find(lid) == locks.end()){
    pthread_mutex_unlock(&mutex);
    return lock_protocol::NOENT; 
  }
  lock& lock = locks[lid];
  if(!lock.aquired || lock.id != id){
    pthread_mutex_unlock(&mutex);
    return lock_protocol::NOENT;  
  }
  lock.aquired = false;
  lock.wait_list.erase(id);
  if(lock.wait_list.empty()){
    pthread_mutex_unlock(&mutex);
    return ret;
  }
  std::string client_id = *(lock.wait_list.begin());
  lock.wait_list.erase(lock.wait_list.begin());
  pthread_mutex_unlock(&mutex);
  int rret;
	rret = handle(client_id).safebind()->call(rlock_protocol::retry, lid, rret);
  tprintf("server(%s): rpc(%s) retry lock(%lld)\n",id.c_str() ,client_id.c_str() ,lid);
  //tprintf("server(%s): rret = %d\n",id.c_str() ,rret);
  
  return ret;
}

lock_protocol::status
lock_server_cache::stat(lock_protocol::lockid_t lid, int &r)
{
  tprintf("stat request\n");
  r = nacquire;
  return lock_protocol::OK;
}

