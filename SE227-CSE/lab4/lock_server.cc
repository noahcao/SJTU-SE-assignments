// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
  nacquire (0)
{
  pthread_mutex_init(&mutex, NULL);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  r = nacquire;
  return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  pthread_mutex_lock(&mutex);

  while (lock_list.find(lid) != lock_list.end() && lock_list[lid] == true) {
        pthread_cond_wait(&cond_list[lid], &mutex);
  }
  lock_list[lid] = true;
  if (cond_list.find(lid) == cond_list.end()) {
        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        cond_list[lid] = cond;
    }
  pthread_mutex_unlock(&mutex);
  return ret;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
	pthread_mutex_lock(&mutex);
  if (lock_list.find(lid) == lock_list.end()) {
        pthread_mutex_unlock(&mutex);
        return lock_protocol::NOENT;
  }

  lock_list[lid] = false;  

  pthread_cond_signal(&cond_list[lid]);  
  pthread_mutex_unlock(&mutex);
  return ret;
}
