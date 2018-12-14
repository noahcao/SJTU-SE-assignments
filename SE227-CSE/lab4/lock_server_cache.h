#ifndef lock_server_cache_h
#define lock_server_cache_h

#include <string>

#include <set>
#include <map>
#include "lock_protocol.h"
#include "rpc.h"
#include "lock_server.h"


class lock_server_cache {
 private:
  int nacquire;
  // my implementation
  pthread_mutex_t mutex;

  struct lock {
    bool aquired;
    std::string id;
    std::set<std::string> wait_list;
  };
  std::map<lock_protocol::lockid_t, lock> locks;
  std::map<std::string, rpcc*> clients;




 public:
  lock_server_cache();
  lock_protocol::status stat(lock_protocol::lockid_t, int &);
  int acquire(lock_protocol::lockid_t, std::string id, int &);
  int release(lock_protocol::lockid_t, std::string id, int &);
};

#endif
