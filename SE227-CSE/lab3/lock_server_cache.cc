// the caching lock server implementation

#include "lock_server_cache.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "lang/verify.h"
#include "handle.h"
#include <iostream>
#include <fstream>
#include "tprintf.h"

using namespace std;

lock_server_cache::lock_server_cache()
{
  VERIFY(pthread_mutex_init(&server_mutex, NULL) == 0);
}


int lock_server_cache::acquire(lock_protocol::lockid_t lid, std::string id, 
                               int &)
{
  int r;
  lock_protocol::status ret = lock_protocol::OK;
  map<lock_protocol::lockid_t, lock_entry>::iterator it;
  pthread_mutex_lock(&server_mutex);
  it = locktable.find(lid);
  if(it == locktable.end()){
    it = locktable.insert(make_pair(lid, lock_entry())).first;
  }
  
  bool revoke = false;

  switch(it->second.state){
    
    // case 1: the lock is free
    case FREE:{
      it->second.state = LOCKED;
      it->second.owner = id;
      break;
    }

    // case 2: the lock is locked
    case LOCKED:{
      it->second.state = LOCKED_WAIT;
      it->second.waitset.insert(id);
      revoke = true;
      ret = lock_protocol::RETRY;
      break;
    }

    // case 3: the lock is locked and wait
    case LOCKED_WAIT:{
      it->second.waitset.insert(id);
      ret = lock_protocol::RETRY;
      break;
    }

    // case 4: the block is under retrying
    case RETRYING:{
      if(it->second.waitset.count(id)){
        it->second.waitset.erase(id);
        it->second.owner = id;
        if(it->second.waitset.size()){
          it->second.state = LOCKED_WAIT;
          revoke = true;
        }
        else it->second.state = LOCKED;
      }
      else{
        it->second.waitset.insert(id);
        ret = lock_protocol::RETRY;
      }
      break;
    }

    default: break;
  }

  pthread_mutex_unlock(&server_mutex);
  if(revoke) handle(it->second.owner).safebind()->call(rlock_protocol::revoke, lid, r);

  return ret;
}

int 
lock_server_cache::release(lock_protocol::lockid_t lid, std::string id, 
         int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  bool retry = false;
  map<lock_protocol::lockid_t, lock_entry>::iterator it;
  pthread_mutex_lock(&server_mutex);
  it = locktable.find(lid);

  if(it == locktable.end()) return lock_protocol::NOENT;

  string client_name;

  switch(it->second.state){
    // case 1: the lock is free
    case FREE:{
      ret = lock_protocol::IOERR;
      break;
    }

    // case 2: the lock is locked
    case LOCKED:{
      it->second.state = FREE;
      it->second.owner = "";
      break;
    }

    // case 3: lock is locked and wait
    case LOCKED_WAIT:{
      it->second.state = RETRYING;
      it->second.owner = "";
      client_name = *it->second.waitset.begin();
      retry = true;
      break;
    }

    // case 4: the lock is under retrying
    case RETRYING:{
      ret = lock_protocol::IOERR;
      break;
    }

    default: break;
  }

  pthread_mutex_unlock(&server_mutex);
  if(retry){
    handle(client_name).safebind()->call(rlock_protocol::retry, lid, r);
  }
  return ret;
}

lock_protocol::status
lock_server_cache::stat(lock_protocol::lockid_t lid, int &r)
{
  tprintf("stat request\n");
  r = nacquire;
  return lock_protocol::OK;
}

