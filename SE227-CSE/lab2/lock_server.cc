// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

// mutex, locks and conds are defined in "lock_server.h"

lock_server::lock_server():
  nacquire (0)
{
	pthread_mutex_init(&mutex, NULL);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  r = nacquire;
  return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r)
{
	pthread_mutex_lock(&mutex);
	
	// wait till the target lock is free
	while(locks.find(lid) != locks.end()){
		pthread_cond_wait(&conds[lid], &mutex);
	}

	// target is free now
	locks.insert(lid);

	if(conds.find(lid) == conds.end()){
		pthread_cond_t new_cond;
		pthread_cond_init(&new_cond, NULL);
		conds[lid] = new_cond;
	}

	pthread_mutex_unlock(&mutex);

	lock_protocol::status ret = lock_protocol::OK;
	return ret;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
	pthread_mutex_lock(&mutex);

	if(locks.find(lid) == locks.end()){
		// the target lock doesnot exist now
	pthread_mutex_unlock(&mutex);
	return lock_protocol::NOENT;
	}

	// realease the target lock
	locks.erase(lid);
	pthread_cond_signal(&conds[lid]);

	pthread_mutex_unlock(&mutex);

	lock_protocol::status ret = lock_protocol::OK;
	return ret;
}
