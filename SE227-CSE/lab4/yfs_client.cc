// yfs client.  implements FS operations using extent and lock server
#include "yfs_client.h"
#include "extent_client.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lock_client_cache.h"

using namespace std;

yfs_client::yfs_client(std::string extent_dst, std::string lock_dst)
{
  ec = new extent_client(extent_dst);
  lc = new lock_client_cache(lock_dst);
  if (ec->put(1, "") != extent_protocol::OK)
      printf("error init root dir\n"); // XYB: init root dir
}

void yfs_client::lockme(inum inum){
	lc->acquire(inum);
}

void yfs_client::releaseme(inum inum){
	lc->release(inum);
}

int
yfs_client::savedir(inum dir, std::list<dirent> &list)
{
    int r = OK;

    std::string buf;
    std::stringstream ss;

    // format dirents
    for(std::list<dirent>::iterator iter = list.begin(); iter != list.end(); iter ++){
        ss << iter->name;
        ss << '/';
        ss << iter->inum;
        ss << '/';
    }
    buf = ss.str();
    ec->put(dir, buf);
    return r;
}

int yfs_client::saveentry(inum parent, const char *name, inum inum){
    int r = OK;
    std::list<dirent> list;
    _readdir(parent, list);
    dirent entry;
    entry.inum = inum;
    entry.name = name;
    list.push_back(entry);
    savedir(parent, list);
    return r;
}


void yfs_client::acquire(inum inum) {
    fprintf(stderr,"acquire lock: %lld\n", inum);fflush(stderr);
    lc->acquire(inum);
    fprintf(stderr,"acquire lock success: %lld\n", inum);fflush(stderr);
}

void yfs_client::release(inum inum) {
    fprintf(stderr,"release lock: %lld\n", inum);fflush(stderr);
    lc->release(inum);
    fprintf(stderr,"release lock success: %lld\n", inum);fflush(stderr);
}


yfs_client::inum
yfs_client::n2i(std::string n)
{
    std::istringstream ist(n);
    unsigned long long finum;
    ist >> finum;
    return finum;
}

std::string
yfs_client::filename(inum inum)
{
    std::ostringstream ost;
    ost << inum;
    return ost.str();
}

bool yfs_client::isfile(inum inum){
	lockme(inum);
	bool result = _isfile(inum);
	releaseme(inum);
	return result;
}

bool
yfs_client::_isfile(inum inum)
{   
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        return false;
    }

    if (a.type == extent_protocol::T_FILE) {
        printf("isfile: %lld is a file\n", inum);
        return true;
    } 
    printf("isfile: %lld is a dir\n", inum);
    return false;
}

bool yfs_client::issymlink(inum inum){
	lockme(inum);
	bool ret = _issymlink(inum);
	releaseme(inum);
	return ret;
}

bool yfs_client::_issymlink(inum inum)
{
    // Oops! is this still correct when you implement symlink?
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        return false;
    }

    if (a.type == extent_protocol::T_SYMLINK) {
        printf("isfile: %lld is a file\n", inum);
        return true;
    } 
    printf("isfile: %lld is a dir\n", inum);
    return false;
}

bool yfs_client::isdir(inum inum){
	lockme(inum);
	bool ret = _isdir(inum);
	releaseme(inum);
	return ret;
}

bool
yfs_client::_isdir(inum inum)
{
    // Oops! is this still correct when you implement symlink?
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        return false;
    }

    if (a.type == extent_protocol::T_DIR) {
        printf("isfile: %lld is a file\n", inum);
        return true;
    } 
    printf("isfile: %lld is a dir\n", inum);
    return false;
}

int yfs_client::getfile(inum inum, fileinfo &fin){
	lockme(inum);
	int ret = _getfile(inum, fin);
	releaseme(inum);
	return ret;
}

int
yfs_client::_getfile(inum inum, fileinfo &fin)
{
    int r = OK;

    printf("getfile %016llx\n", inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        r = IOERR;
        goto release;
    }

    fin.atime = a.atime;
    fin.mtime = a.mtime;
    fin.ctime = a.ctime;
    fin.size = a.size;
    printf("getfile %016llx -> sz %llu\n", inum, fin.size);

release:
    return r;
}

int yfs_client::getsymlink(inum inum, symlinkinfo &symlink){
	lockme(inum);
	int ret = _getsymlink(inum, symlink);
	releaseme(inum);
	return ret;
}

int yfs_client::_getsymlink(inum inum, symlinkinfo &symlink){
    int r = OK;

    printf("getsymlink %016llx\n", inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        r = IOERR;
        goto release;
    }
    symlink.atime = a.atime;
    symlink.mtime = a.mtime;
    symlink.ctime = a.ctime;

release:
    return r;
}


int yfs_client::getdir(inum inum, dirinfo &din){
	lockme(inum);
	int ret = _getdir(inum, din);
	releaseme(inum);
	return ret;
}

int
yfs_client::_getdir(inum inum, dirinfo &din)
{
    int r = OK;

    printf("getdir %016llx\n", inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        r = IOERR;
        goto release;
    }
    din.atime = a.atime;
    din.mtime = a.mtime;
    din.ctime = a.ctime;

release:
    return r;
}

#define EXT_RPC(xx) do { \
    if ((xx) != extent_protocol::OK) { \
        printf("EXT_RPC Error: %s:%d \n", __FILE__, __LINE__); \
        r = IOERR; \
        goto release; \
    } \
} while (0)

// Only support set size of attr
int yfs_client::setattr(inum ino, size_t size){
	lockme(ino);
	int ret = _setattr(ino, size);
	releaseme(ino);
	return ret;
}

// Only support set size of attr
int
yfs_client::_setattr(inum ino, size_t size)
{
    if(ino <= 0 || size < 0) return IOERR;

    string content;
    
    if(ec->get(ino, content) != extent_protocol::OK){
        cout << "setattr: fail to get content from ino" << endl;
        return IOERR;
    }

    content.resize(size);
    if(ec->put(ino, content) != extent_protocol::OK){
        cout << "setattr: fail to write content" << endl;
        return IOERR;
    }

    return OK;  
}

int yfs_client::create(inum parent, const char *name, mode_t mode, inum &ino_out){
	lockme(parent);
	int ret = _create(parent, name, mode, ino_out);
	releaseme(parent);
	return ret;
}

int
yfs_client::_create(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    int r = OK;
    // check if existed
    bool found = false;
    inum ino_num;
    _lookup(parent, name, found, ino_num);
    if(found){
        release(parent);
        return EXIST;
    }

    // create file
    ec->create(extent_protocol::T_FILE, ino_out);

    // save dir
    saveentry(parent, name, ino_out);
    
    return r;
}

int
yfs_client::mkdir(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    lockme(parent);
    int ret = _mkdir(parent, name, mode, ino_out);
    releaseme(parent);
    return ret;
}

int yfs_client::_mkdir(inum parent, const char *name, mode_t mode, inum &ino_out){
    int r = OK;
    // check if existed
    bool found = false;
    inum ino_num;
    _lookup(parent, name, found, ino_num);
    if(found){
        release(parent);
        return EXIST;
    }

    // create file
    ec->create(extent_protocol::T_DIR, ino_out);

    // save dir
    r = saveentry(parent, name, ino_out);
    return r;
}

int
yfs_client::lookup(inum parent, const char *name, bool &existed, inum &ino_out)
{
    acquire(parent);
    int ret = _lookup(parent, name, existed, ino_out);
    release(parent);
    return ret;
}

int
yfs_client::_lookup(inum parent, const char *name, bool &existed, inum &ino_out)
{
    int r = OK;
    existed = false;

    std::list<dirent> list;
    _readdir(parent, list);
    for(std::list<dirent>::iterator iter = list.begin(); iter != list.end(); iter++){
        if(iter->name == name){
            ino_out = iter->inum;
            existed = true;
            break;
        }
    }
    return r;
}

int
yfs_client::readdir(inum dir, std::list<dirent> &list)
{
    lockme(dir);
    int ret = _readdir(dir, list);
    releaseme(dir);
    return ret;
}   

int
yfs_client::_readdir(inum dir, std::list<dirent> &list)
{
    int r = OK;
    if(!_isdir(dir)){
        fprintf(stderr,"_readdir: input inode is not a dir\n");
    }
    std::string buf;
    ec->get(dir, buf);
    std::stringstream ss;
    ss << buf;

    while(true){
        dirent entry;
        if(!std::getline(ss, entry.name, '/')){
            break;
        }
        ss >> entry.inum;
        // remove the '/' between inum and latter filename
        ss.get();
        list.push_back(entry);
        fprintf(stderr,"readdir: inum: %d name:%s\n", entry.inum, entry.name.c_str());
        fflush(stdout);
    }
    return r;
}

int
yfs_client::read(inum ino, size_t size, off_t off, std::string &data)
{
    lockme(ino);
    int ret = _read(ino, size, off, data);
    releaseme(ino);
    return ret;
}

int
yfs_client::_read(inum ino, size_t size, off_t off, std::string &data)
{
    int r = OK;
    std::string buf;
    ec->get(ino, buf);
    data = buf.substr(off, size);
    return r;
}

int yfs_client::write(inum ino, size_t size, off_t off, const char *data, size_t &bytes_written){
	lockme(ino);
	int ret = _write(ino, size, off, data, bytes_written);
	releaseme(ino);
	return ret;
}

int
yfs_client::_write(inum ino, size_t size, off_t off, const char *data, size_t &bytes_written){
    if(ino <= 0 || size < 0 || off < 0){
        cout << "write: illegal inode_index / size / offset" << endl;
        return IOERR;
    }
    
    string content;

    if(ec->get(ino, content) != extent_protocol::OK){
        cout << "write: failt to read file content" << endl;
        return IOERR;
    }

    // write new content into the file
    if(off >= content.size()){
        // add 'holes' between old file segments and the nnew added file
        content.resize(off, '\0');
        content.append(data, size);
    }
    else{
        size_t write_size = (off + size <= content.size()) ? size : content.size() - off;
        content.replace(off, write_size, data, size);
    }

    if(ec->put(ino, content) != extent_protocol::OK){
        cout << "write: fail to write the new file" << endl;
        return IOERR;
    }
    
    return OK;
}

int
yfs_client::unlink(inum parent, const char *name)
{
    lockme(parent);
    int ret = _unlink(parent, name);
    releaseme(parent);
    return ret;
}

int
yfs_client::_unlink(inum parent, const char *name)
{
    int r = OK;
    inum ino_out;
    bool found = false;
    std::list<dirent> list;
    _readdir(parent, list);
    std::list<dirent>::iterator iter = list.begin();
    for(; iter != list.end(); iter++){
        if(iter->name == name){
            ino_out = iter->inum;
            found = true;
            break;
        }
    }
    if(found){
        ec->remove(ino_out);
        list.erase(iter);
        savedir(parent, list);
    }
    else{
        printf("yfs_client::unlink: file not found\n");
    }
    return r;
}

int
yfs_client::symlink(const char *link, inum parent, const char *name, inum &ino)
{
    lockme(parent);
    int ret = _symlink(link, parent, name, ino);
    releaseme(parent);
    return ret;
}

int
yfs_client::_symlink(const char *link, inum parent, const char *name, inum &ino)
{
    int r = OK;
    std::string buf;

    // create link file first
    ec->create(extent_protocol::T_SYMLINK, ino);
    std::cout << "create symlink\n";
    // save directory
    std::list<dirent> list;
    _readdir(parent, list);
    dirent entry;
    entry.inum = ino;
    entry.name = name;
    list.push_back(entry);
    savedir(parent, list);
    ec->put(ino, std::string(link));
    return r;
}

int yfs_client::readlink(inum ino, string& path){
	lockme(ino);
	int ret = _readlink(ino, path);
	releaseme(ino);
	return ret;
}

int yfs_client::_readlink(inum ino, string& path){
    if(ino <=  0){
        cout << "illegal inum " << ino << endl;
        return IOERR;
    }

    if(ec->get(ino, path) != extent_protocol::OK){
        cout << "readlink : fail to read link path" << endl;
        return IOERR;
    }

    return OK;
}

int
yfs_client::readdir_r(inum dir, std::list<dirent> &list)
{
    int r = OK;
    std::string buf;
    ec->get(dir, buf);
    std::stringstream ss;
    ss << buf;

    while(true){
        dirent entry;
        if(!std::getline(ss, entry.name, '/')){
            break;
        }
        ss >> entry.inum;
        // remove the '/' between inum and latter filename
        ss.get();
        list.push_back(entry);
        fprintf(stderr,"readdir: inum: %d name:%s\n", entry.inum, entry.name.c_str());
        fflush(stdout);
    }
    return r;
}

bool
yfs_client::isdir_r(inum inum)
{
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        return false;
    }

    if (a.type == extent_protocol::T_DIR) {
        printf("isdir: %lld is a dir\n", inum);
        return true;
    } 
    return false;
}

int
yfs_client::lookup_r(inum parent, const char *name, bool &found, inum &ino_out)
{
    int r = OK;
    found = false;
    std::list<dirent> list;
    readdir_r(parent, list);
    for(std::list<dirent>::iterator iter = list.begin(); iter != list.end(); iter++){
        if(iter->name == name){
            ino_out = iter->inum;
            found = true;
            break;
        }
    }
    return r;
}