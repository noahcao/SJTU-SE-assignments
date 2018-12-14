// lock client interface.

#ifndef lock_client_cache_h

#define lock_client_cache_h

#include <string>
#include "lock_protocol.h"
#include "rpc.h"
#include "lock_client.h"
#include "lang/verify.h"

class lock_release_user {
 public:
  virtual void dorelease(lock_protocol::lockid_t) = 0;
  virtual ~lock_release_user() {};
};

class lock_client_cache : public lock_client {
  // note the state of lock 
  enum lock_state {NONE, LOCKED, FREE, ACQUIRING, RELEASING};
  struct lock_entry{
    bool revoked;
    bool retry;
    lock_state state;
    pthread_cond_t wait_threads;  // queue for threads that are waiting
    pthread_cond_t release_threads; // queue for threads that are to be releasing locks
    pthread_cond_t retry_threads; // querue for threads for next trying
  };

 private:
  class lock_release_user *lu;
  int rlock_port;
  std::string hostname;
  std::string id;
  pthread_mutex_t client_lock;
  std::map<lock_protocol::lockid_t, lock_entry> locktable;

 public:
  static int last_port;
  lock_client_cache(std::string xdst, class lock_release_user *l = 0);
  virtual ~lock_client_cache() {};
  lock_protocol::status acquire(lock_protocol::lockid_t);
  lock_protocol::status release(lock_protocol::lockid_t);
  rlock_protocol::status revoke_handler(lock_protocol::lockid_t, 
                                        int &);
  rlock_protocol::status retry_handler(lock_protocol::lockid_t, 
                                       int &);
};


#endif
