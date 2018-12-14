// RPC stubs for clients to talk to lock_server, and cache the locks
// see lock_client.cache.h for protocol details.

#include "lock_client_cache.h"
#include "rpc.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "tprintf.h"
#include <unistd.h>


int lock_client_cache::last_port = 0;

lock_client_cache::lock_client_cache(std::string xdst, 
				     class lock_release_user *_lu)
  : lock_client(xdst), lu(_lu)
{
  srand(time(NULL)^last_port);
  rlock_port = ((rand()%32000) | (0x1 << 10));
  char hname[100];
  VERIFY(gethostname(hname, sizeof(hname)) == 0);
  std::ostringstream host;
  host << hname << ":" << rlock_port;
  id = host.str();
  last_port = rlock_port;
  rpcs *rlsrpc = new rpcs(rlock_port);
  rlsrpc->reg(rlock_protocol::revoke, this, &lock_client_cache::revoke_handler);
  rlsrpc->reg(rlock_protocol::retry, this, &lock_client_cache::retry_handler);
  pthread_mutex_init(&mutex, NULL);
}

lock_protocol::status
lock_client_cache::acquire(lock_protocol::lockid_t lid)
{
  int ret = lock_protocol::OK;
  //tprintf("client(%d): enter acquire\n", rlock_port);
  pthread_mutex_lock(&mutex);
  //tprintf("client(%d): hold the mutex\n", rlock_port);
  
  // check the existence of the lock
  if(lock_cache.find(lid) == lock_cache.end()){
    lock_cache[lid].stat = lock::NONE;
    lock_cache[lid].revoked = false;
    lock_cache[lid].retry = false;
    lock_cache[lid].has_wait = false;
    pthread_cond_t release_cond = PTHREAD_COND_INITIALIZER;
    lock_cache[lid].release_cond = release_cond;
    pthread_cond_t retry_cond = PTHREAD_COND_INITIALIZER;
    lock_cache[lid].retry_cond = retry_cond;
    pthread_cond_t wait_cond = PTHREAD_COND_INITIALIZER;
    lock_cache[lid].wait_cond = wait_cond;
  }
  lock& lock = lock_cache[lid];
  while(1){
    if(lock.stat == lock::NONE){
        int r;
        lock.stat = lock::AQUIRING;
        lock.retry = false;
        //tprintf("client(%d): release the mutex\n", rlock_port);
        pthread_mutex_unlock(&mutex);
        //tprintf("client(%d): rpc acquire prepare\n", rlock_port);
        r = cl->call(lock_protocol::acquire, lid, id, r);
        //tprintf("client(%d): rpc acquire done\n", rlock_port);
        pthread_mutex_lock(&mutex);
        //tprintf("client(%d): hold the mutex\n", rlock_port);
        if(r == lock_protocol::OK){
          lock.stat = lock::LOCKED;
          lock.tid = pthread_self();

          //tprintf("client(%d): get the lock from server\n", rlock_port);
          pthread_mutex_unlock(&mutex);

          //tprintf("client(%d): rpc acquire ret in aquire\n", rlock_port);
          return ret;
        }
        else if(r == lock_protocol::RETRY){
          if(!lock.retry){
            //tprintf("client(%d)(%lld): #### wait in the retry_cond aquire 1\n", rlock_port, lid);
            pthread_cond_wait(&lock.retry_cond, &mutex);
            //tprintf("client(%d): #### WAKE UP in the retry_cond\n", rlock_port);
          }
        }
    }
    else if(lock.stat == lock::FREE){
      lock.stat = lock::LOCKED;
      lock.tid = pthread_self();
      //tprintf("client(%d): get the lock from local\n", rlock_port);
      //tprintf("client(%d): release the mutex\n", rlock_port);
      pthread_mutex_unlock(&mutex);
      return lock_protocol::OK;
    }
    else if(lock.stat == lock::LOCKED){
      //tprintf("client(%d): #### wait in the wait_cond acquire 1\n", rlock_port);
      pthread_cond_wait(&lock.wait_cond, &mutex);
      //lock.has_wait = true;
      //tprintf("client(%d): #### WAKE UP in the wait_cond\n", rlock_port);
    }
    else if(lock.stat == lock::RELEASING){
      //tprintf("client(%d): #### wait in the release_cond\n", rlock_port);
      pthread_cond_wait(&lock.release_cond, &mutex);
      //tprintf("client(%d): #### WAKE UP in the release_cond\n", rlock_port);
    }
    else if(lock.stat == lock::AQUIRING){
      if(lock.retry){
        lock.retry = false;
        int r;
        //tprintf("client(%d): release the mutex\n", rlock_port);
        pthread_mutex_unlock(&mutex);

        //tprintf("client(%d): rpc acquire prepare\n", rlock_port);
        r = cl->call(lock_protocol::acquire, lid, id, r);
        //tprintf("client(%d): rpc acquire done\n", rlock_port);
        pthread_mutex_lock(&mutex);
        //tprintf("client(%d): hold the mutex\n", rlock_port);
        if(r == lock_protocol::OK){
          lock.stat = lock::LOCKED;
          //tprintf("client(%d): get the lock from server(retry)\n", rlock_port);
          lock.tid = pthread_self();
          //tprintf("client(%d): rpc acquire ret in aquire\n", rlock_port);

          //tprintf("client(%d): release the mutex\n", rlock_port);
          pthread_mutex_unlock(&mutex);
          return ret;
        }
        else if(r == lock_protocol::RETRY){
          if(!lock.retry){
            //tprintf("client(%d)(%lld): #### wait in the retry_cond aqcuire 2\n", rlock_port, lid);
            pthread_cond_wait(&lock.retry_cond, &mutex);
            //tprintf("client(%d): #### WAKE UP in the retry_cond\n", rlock_port);
          }
        }
      }
      else{
        //tprintf("client(%d): #### wait in the wait_cond aqcuire 2\n", rlock_port);
        //lock.has_wait = true;
        pthread_cond_wait(&lock.wait_cond, &mutex);
        //tprintf("client(%d): #### WAKE UP in the wait_cond\n", rlock_port);
      }
    }
  }
  //tprintf("client(%d): rpc acquire ret in normal\n", rlock_port);

  //tprintf("client(%d): release the mutex\n", rlock_port);
  pthread_mutex_unlock(&mutex);
  return lock_protocol::OK;
}

lock_protocol::status
lock_client_cache::release(lock_protocol::lockid_t lid)
{
  pthread_mutex_lock(&mutex);
  //tprintf("client(%d): hold the mutex\n", rlock_port);
  //tprintf("client(%d): rpc release prepare\n", rlock_port);
  if(lock_cache.find(lid) == lock_cache.end()){

    //tprintf("client(%d): release the mutex\n", rlock_port);
    pthread_mutex_unlock(&mutex);
    return lock_protocol::RPCERR;
  }
  //tprintf("client(%d): rpc release check done\n", rlock_port);
  lock& lock = lock_cache[lid];
  if(lock.stat != lock::LOCKED || lock.tid != pthread_self()){

    //tprintf("client(%d): release the mutex\n", rlock_port);
    pthread_mutex_unlock(&mutex);
    return lock_protocol::RPCERR;
  }
  // special case
  // if(lock.has_wait){
  //   //tprintf("client(%d)(%lld): release local\n", rlock_port, lid);
  //   lock.stat = lock::FREE;
  //   // //tprintf("client(%d): ### signal retry1\n", rlock_port);
  //   // pthread_cond_broadcast(&lock.retry_cond);
  //   pthread_cond_broadcast(&lock.wait_cond);
  //   pthread_mutex_unlock(&mutex);
  //   return lock_protocol::OK;
  // }
  if(lock.revoked){
    lock.stat = lock::RELEASING;
    lock.revoked = false;
    //tprintf("client(%d): release the mutex\n", rlock_port);
    pthread_mutex_unlock(&mutex);
    //tprintf("client(%d): rpc before lu dorelease\n", rlock_port);
    //lu->dorelease(lid);
    //tprintf("client(%d): rpc after lu dorelease\n", rlock_port);
    
    lock_protocol::status ret = cl->call(lock_protocol::release, lid, id, ret);
    //tprintf("client(%d): release to server\n", rlock_port);
    pthread_mutex_lock(&mutex);
    //tprintf("client(%d): hold the mutex\n", rlock_port);
    lock.stat = lock::NONE;
    pthread_cond_broadcast(&lock.release_cond);
    //tprintf("client(%d): rpc release done in revoke\n", rlock_port);
    //tprintf("client(%d): release the mutex\n", rlock_port);
    pthread_mutex_unlock(&mutex);
    return ret;
  }
  else{
    //tprintf("client(%d)(%lld): release local\n", rlock_port, lid);
    lock.stat = lock::FREE;
    //tprintf("client(%d): release to local\n", rlock_port);
    // //tprintf("client(%d): ### signal retry1\n", rlock_port);
    // pthread_cond_broadcast(&lock.retry_cond);
    pthread_cond_broadcast(&lock.wait_cond);
  }
  //tprintf("client(%d): rpc release done\n", rlock_port);
  //tprintf("client(%d): release the mutex\n", rlock_port);
  pthread_mutex_unlock(&mutex);
  return lock_protocol::OK;

}

rlock_protocol::status
lock_client_cache::revoke_handler(lock_protocol::lockid_t lid, 
                                  int &)
{
  //sleep(1);
  pthread_mutex_lock(&mutex);
  //tprintf("client(%d): hold the mutex\n", rlock_port);
  int ret = rlock_protocol::OK;
  if(lock_cache.find(lid) == lock_cache.end()){
    //tprintf("client(%d)::revoke_handler lock no find\n", rlock_port);
    //tprintf("client(%d): release the mutex\n", rlock_port);
    pthread_mutex_unlock(&mutex);
    return lock_protocol::RPCERR;
  }
  lock& lock = lock_cache[lid];
  if(lock.stat == lock::FREE){
    lock.stat = lock::RELEASING;
    //tprintf("client(%d)::revoke_handler release free lock\n", rlock_port);
    //tprintf("client(%d): release the mutex\n", rlock_port);
    pthread_mutex_unlock(&mutex);
    //lu->dorelease(lid);
    int r = cl->call(lock_protocol::release, lid, id, r);
    //tprintf("client(%d): release to server\n", rlock_port);
    pthread_mutex_lock(&mutex);
    //tprintf("client(%d): hold the mutex\n", rlock_port);
    lock.stat = lock::NONE;
    pthread_cond_broadcast(&lock.release_cond);
    //tprintf("client(%d)::revoke_handler release free lock done\n", rlock_port);
  }
  else{
    //tprintf("client(%d)::revoke_handler revoke locked lock\n", rlock_port);
    lock.revoked = true;
  }
  //tprintf("client(%d): release the mutex\n", rlock_port);
  pthread_mutex_unlock(&mutex);
  return ret;
}

rlock_protocol::status
lock_client_cache::retry_handler(lock_protocol::lockid_t lid, 
                                 int &)
{
  pthread_mutex_lock(&mutex);
  //tprintf("client(%d): hold the mutex\n", rlock_port);
  if(lock_cache.find(lid) == lock_cache.end()){
    //tprintf("client(%d): release the mutex\n", rlock_port);
    pthread_mutex_unlock(&mutex);
    return lock_protocol::RPCERR;
  }
  lock& lock = lock_cache[lid];
  lock.retry = true;
  //tprintf("client(%d): ### signal retry2\n", rlock_port);
  pthread_cond_signal(&lock.retry_cond);
  int ret = rlock_protocol::OK;
  //tprintf("client(%d): release the mutex\n", rlock_port);
  pthread_mutex_unlock(&mutex);
  return ret;
}



