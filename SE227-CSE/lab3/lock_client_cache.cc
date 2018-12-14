// RPC stubs for clients to talk to lock_server, and cache the locks
// see lock_client.cache.h for protocol details.
#include <unistd.h>
#include "lock_client_cache.h"
#include "rpc.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include "tprintf.h"
#include <time.h>

using namespace std;

int lock_client_cache::last_port = 0;

lock_client_cache::lock_client_cache(std::string xdst, 
				     class lock_release_user *_lu)
  : lock_client(xdst), lu(_lu)
{
  srand(time(NULL)^last_port);
  rlock_port = ((rand()%32000) | (0x1 << 10));
  const char *hname;
  // VERIFY(gethostname(hname, 100) == 0);
  hname = "127.0.0.1";
  std::ostringstream host;
  host << hname << ":" << rlock_port;
  id = host.str();
  last_port = rlock_port;
  rpcs *rlsrpc = new rpcs(rlock_port);
  rlsrpc->reg(rlock_protocol::revoke, this, &lock_client_cache::revoke_handler);
  rlsrpc->reg(rlock_protocol::retry, this, &lock_client_cache::retry_handler);
}

lock_protocol::status
lock_client_cache::acquire(lock_protocol::lockid_t lid)
{
  // the acquire function for the client instance 
  int ret = lock_protocol::OK;
  int r;
  map<lock_protocol::lockid_t, lock_entry>::iterator it;
  pthread_mutex_lock(&client_lock);
  it = locktable.find(lid);

  if(it == locktable.end()) it = locktable.insert(std::make_pair(lid, lock_entry())).first;

  while(true){
    switch(it->second.state){

      // case 1: the lock in a NONE state
      case NONE:{
        it->second.state = ACQUIRING;
        it->second.retry = false;
        pthread_mutex_unlock(&client_lock);

        ret = cl->call(lock_protocol::acquire, lid, id, r);
        pthread_mutex_lock(&client_lock);

        if(ret == lock_protocol::OK){
          // whether having acquired the lock
          it->second.state = LOCKED;
          pthread_mutex_unlock(&client_lock);
          return ret;
        }
        else{
          if(ret == lock_protocol::RETRY && !it->second.retry){
            pthread_cond_wait(&it->second.retry_threads, &client_lock);
          }
          break;
        }
      }

      // case 2: the lock is free
      case FREE:{
        it->second.state = LOCKED;
        pthread_mutex_unlock(&client_lock);
        return lock_protocol::OK;
      }

      // case 3: the lock is locked
      case LOCKED:{
        pthread_cond_wait(&it->second.wait_threads, &client_lock);
        break;
      }

      // case 4: the lock is being acquired
      case ACQUIRING:{
        if(!it->second.retry){
          pthread_cond_wait(&it->second.wait_threads, &client_lock);
        }
        else{
          it->second.retry = true;
          pthread_mutex_unlock(&client_lock);
          ret = cl->call(lock_protocol::acquire, lid, id, r);
          pthread_mutex_lock(&client_lock);
          if(ret == lock_protocol::OK){
            it->second.state = LOCKED;
            pthread_mutex_unlock(&client_lock);
            return ret;
          }
          else{
            if(ret == lock_protocol::RETRY && !it->second.retry){
              pthread_cond_wait(&it->second.retry_threads, &client_lock);
            }
          }
        }
        break;
      }

      // case 5: the lock is in releasing
      case RELEASING:
        pthread_cond_wait(&it->second.release_threads, &client_lock);
    }
  }
  return ret;
}

lock_protocol::status
lock_client_cache::release(lock_protocol::lockid_t lid)
{ 
  // release function for client instance
  int r;
  int ret = lock_protocol::OK;
  map<lock_protocol::lockid_t, lock_entry>::iterator it;
  pthread_mutex_lock(&client_lock);
  it = locktable.find(lid);
  
  if(it == locktable.end()){
    // the lock to be released can't be found
    return lock_protocol::NOENT;
  }

  if(it->second.revoked){
    // if the lock is beed revoked, then release it
    it->second.state = RELEASING;
    it->second.revoked = false;
    pthread_mutex_unlock(&client_lock);

    ret = cl->call(lock_protocol::release, lid, id, r);
    pthread_mutex_lock(&client_lock);
    it->second.state = NONE;
    pthread_cond_broadcast(&it->second.release_threads);
    pthread_mutex_unlock(&client_lock);
  }
  else{
    // release the lock
    it->second.state = FREE;
    pthread_cond_signal(&it->second.wait_threads);
    pthread_mutex_unlock(&client_lock);
  }
  return ret;
}

rlock_protocol::status
lock_client_cache::revoke_handler(lock_protocol::lockid_t lid, 
                                  int &)
{
  sleep(1);
  int ret = rlock_protocol::OK;
  int r;
  map<lock_protocol::lockid_t, lock_entry>::iterator it;
  pthread_mutex_lock(&client_lock);
  it = locktable.find(lid);
  if(it == locktable.end()){
    return lock_protocol::NOENT;
  }

  if(it->second.state == FREE){
    it->second.state = RELEASING;
    pthread_mutex_unlock(&client_lock);

    ret = cl->call(lock_protocol::release, lid, id, r);
    pthread_mutex_lock(&client_lock);
    it->second.state = NONE;
    pthread_cond_broadcast(&it->second.release_threads);
    pthread_mutex_unlock(&client_lock);
  }
  else{
    it->second.revoked = true;
    pthread_mutex_unlock(&client_lock);
  }

  return ret;
}

rlock_protocol::status
lock_client_cache::retry_handler(lock_protocol::lockid_t lid, 
                                 int &)
{
  int ret = rlock_protocol::OK;
  map<lock_protocol::lockid_t, lock_entry>::iterator it;
  pthread_mutex_lock(&client_lock);
  it = locktable.find(lid);
  if(it == locktable.end()){
    return lock_protocol::NOENT;
  }
  it->second.retry = true;
  pthread_cond_signal(&it->second.retry_threads);
  pthread_mutex_unlock(&client_lock);
  return ret;
}



