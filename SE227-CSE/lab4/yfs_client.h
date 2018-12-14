#ifndef yfs_client_h
#define yfs_client_h

#include <string>

#include "lock_protocol.h"
#include "lock_client.h"

//#include "yfs_protocol.h"
#include "extent_client.h"
#include <vector>


class yfs_client {
  extent_client *ec;
  lock_client *lc;
 public:

  typedef unsigned long long inum;
  enum xxstatus { OK, RPCERR, NOENT, IOERR, EXIST };
  typedef int status;

  struct fileinfo {
    unsigned long long size;
    unsigned long atime;
    unsigned long mtime;
    unsigned long ctime;
  };
  struct dirinfo {
    unsigned long atime;
    unsigned long mtime;
    unsigned long ctime;
  };
  struct symlinkinfo {
    unsigned long long size;
    unsigned long atime;
    unsigned long mtime;
    unsigned long ctime;
  };
  struct dirent {
    std::string name;
    yfs_client::inum inum;
  };

 private:
  static std::string filename(inum);
  static inum n2i(std::string);

 public:
  yfs_client(std::string, std::string);

  bool isfile(inum);
  bool _isfile(inum);

  bool isdir(inum);
  bool _isdir(inum);
  bool isdir_r(inum inum);

  bool issymlink(inum);
  bool _issymlink(inum);

  int getfile(inum, fileinfo &);
  int _getfile(inum, fileinfo &);

  int getdir(inum, dirinfo &);
  int _getdir(inum, dirinfo &);

  int getsymlink(inum, symlinkinfo &);
  int _getsymlink(inum, symlinkinfo &);

  void acquire(inum);
  void release(inum);
  void lockme(inum);
  void releaseme(inum);
  int savedir(inum, std::list<dirent> &);

  int setattr(inum, size_t);
  int _setattr(inum, size_t);

  int create(inum, const char *, mode_t, inum &);
  int _create(inum, const char *, mode_t, inum &);

  int lookup(inum, const char *, bool &, inum &);
  int _lookup(inum, const char *, bool &, inum &);

  int mkdir(inum , const char *, mode_t , inum &);
  int _mkdir(inum , const char *, mode_t , inum &);

  int write(inum, size_t, off_t, const char *, size_t &);
  int _write(inum, size_t, off_t, const char *, size_t &);

  int read(inum, size_t, off_t, std::string &);
  int _read(inum, size_t, off_t, std::string &);
  
  int readdir(inum, std::list<dirent> &);
  int _readdir(inum, std::list<dirent> &);

  int unlink(inum,const char *);
  int _unlink(inum,const char *);

  
  int symlink(const char *link, inum parent, const char *name, inum &ino);
  int _symlink(const char *link, inum parent, const char *name, inum &ino);

  int readlink(inum ino, std::string &path);
  int _readlink(inum ino, std::string &path);

  int saveentry(inum , const char *, inum);
  int writedir(inum, std::list<dirent>&);

  // lockless
  int lookup_r(inum, const char *, bool &, inum &);
  int readdir_r(inum, std::list<dirent> &);
  /** you may need to add symbolic link related methods here.*/
};

#endif 