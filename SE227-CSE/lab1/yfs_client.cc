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
#include <string>

using namespace std;

yfs_client::yfs_client()
{
    ec = new extent_client();

}

yfs_client::yfs_client(std::string extent_dst, std::string lock_dst)
{
    ec = new extent_client();
    if (ec->put(1, "") != extent_protocol::OK)
        printf("error init root dir\n"); // XYB: init root dir
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

bool
yfs_client::isfile(inum inum)
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

bool
yfs_client::isdir(inum inum)
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

bool yfs_client::issymlink(inum inum)
{
    // Oops! is this still correct when you implement symlink?
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        return false;
    }

    if (a.type == extent_protocol::T_SLINK) {
        printf("isfile: %lld is a file\n", inum);
        return true;
    } 
    printf("isfile: %lld is a dir\n", inum);
    return false;
}

int
yfs_client::getfile(inum inum, fileinfo &fin)
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

int
yfs_client::getdir(inum inum, dirinfo &din)
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

int yfs_client::getsymlink(inum inum, symlinkinfo &symlink){
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


#define EXT_RPC(xx) do { \
    if ((xx) != extent_protocol::OK) { \
        printf("EXT_RPC Error: %s:%d \n", __FILE__, __LINE__); \
        r = IOERR; \
        goto release; \
    } \
} while (0)


// Only support set size of attr
int
yfs_client::setattr(inum ino, size_t size)
{
    if(ino <= 0){
        cout << "setattr: illegal inum :" << ino << endl;
        return IOERR;
    }

    if(size < 0){
        cout << "setattr: illegal size input: size=" << size << endl;
        return IOERR;
    }

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

int
yfs_client::create(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    bool exist;
    inum tmp_num;

    if(lookup(parent, name, exist, tmp_num) == IOERR) return EXIST;
    if(exist) return EXIST;

    // try to create the file
    if(ec->create(extent_protocol::T_FILE, ino_out) != extent_protocol::OK){
        cout << "create: fail to create the file" << endl;
        return IOERR;
    }

    if(saveentry(parent, name, ino_out) == IOERR){
        return IOERR;
    }
    return OK;
}

int yfs_client::writedir(inum dir, list<dirent>& entries){
    ostringstream ost;

    list<dirent>::iterator it;
    for(it = entries.begin(); it != entries.end(); it++){
        ost << it->name;
        ost << '\0';    // legal seperator
        ost << it->inum;
    }

    // write entries info into file
    if(ec->put(dir, ost.str()) != extent_protocol::OK){
        cout << "writedir: fail to write enrties info into dir" << endl;
        return IOERR;
    }

    return OK;
}

int yfs_client::saveentry(inum parent, const char *name, inum inum){
    list<dirent> entries;

    if(readdir(parent, entries) != OK){
        cout << "save entry: fail to read the parent dir" << endl;
        return IOERR;
    }

    dirent entry;
    entry.name = name;
    entry.inum = inum;
    entries.push_back(entry);

    if(writedir(parent, entries) != OK){
        cout << "saveentry: failt to write into parent dir" << endl;
        return IOERR;
    }

    return OK;
}

int
yfs_client::mkdir(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    bool exist;
    inum tmp_num;

    if(lookup(parent, name, exist, tmp_num) == IOERR) return EXIST;
    if(exist) return EXIST;

    // try to create file
    if(ec->create(extent_protocol::T_DIR, ino_out) != extent_protocol::OK){
        cout << "create: fail to create a dir" << endl;
        return IOERR;
    }

    if(saveentry(parent, name, ino_out) == IOERR) return IOERR;

    return OK;
}

int
yfs_client::lookup(inum parent, const char *name, bool &found, inum &ino_out)
{
    list<dirent> entries;

    if(readdir(parent, entries) == IOERR){
        cout << "lookup: IOERR when read a directory" << endl;
        return IOERR;
    }

    found = false;

    list<dirent>::iterator it = entries.begin();
    for(; it != entries.end(); it++){
        if(it->name == name){
            found = true;
            ino_out = it->inum;
        }
    }

    return  OK;
}

int
yfs_client::readdir(inum dir, std::list<dirent> &list)
{
    string content;
    dirent entry;
    
    if(ec->get(dir, content) != extent_protocol::OK) return IOERR;

    // read entries under the dir
    list.clear();
    istringstream ist(content);
    
    while(getline(ist, entry.name, '\0')){
        ist >> entry.inum;
        list.push_back(entry);
    }

    return OK;
}

int
yfs_client::read(inum ino, size_t size, off_t off, std::string &data)
{
    if(ino <= 0){
        cout << "read: illegal inum : " << ino << endl;
        return IOERR;
    }

    if(size < 0){
        cout << "read: illegal content size" << endl;
        return IOERR;
    }

    if(off < 0){
        cout << "read: off is negative" << endl;
        return IOERR;
    }

    extent_protocol::attr atr;

    if(ec->getattr(ino, atr) != extent_protocol::OK){
        cout << "read: fail to get attribute" << endl;
        return IOERR;
    }
    if(off >= atr.size){
        cout << "read: offset is not smaller than the size" << endl;
        return IOERR;
    }

    // read from the file
    string content;

    if(ec->get(ino, content) != extent_protocol::OK){
        cout << "read: failt to get file content" << endl;
        return IOERR;
    }

    data = content.substr(off, size);
    return OK;
}

int
yfs_client::write(inum ino, size_t size, off_t off, const char *data,
        size_t &bytes_written)
{

    if(ino <= 0){
        cout << "write: illegal inode number" << endl;
        return IOERR;
    }

    if(size < 0){
        cout << "write: size if negative" << endl;
        return IOERR;
    }

    if(off < 0){
        cout << "off is negative" << endl;
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

int yfs_client::unlink(inum parent,const char *name)
{
    list<dirent> entries;

    if(readdir(parent, entries) != OK){
        cout << "unlink: fail to read the parent dir" << endl;
        return IOERR;
    }

    bool found = false;
    list<dirent>::iterator it;
    for(it = entries.begin(); it != entries.end(); it++){
        if(it->name == name){
            found = true;
            break;
        }
    }

    if(!found){
        cout << "unlink: not found the target link file" << endl;
        return IOERR;
    }

    // remove the file
    if(ec->remove(it->inum) != extent_protocol::OK){
        cout << "unlink: fail to remove the link file" << endl;
        return IOERR;
    }

    // remove the entry info the parent dir
    entries.erase(it);
    if(writedir(parent, entries) != OK){
        cout << "unlink: fail to rewrite the entries info in dir" << endl;
        return IOERR;
    }

    return OK;
}

int yfs_client::symlink(inum parent, const char* linkname, const char* filename, inum& ino_out){
    cout << "begin create a link" << endl;
    if(parent <= 0){
        cout << "symlink : illegal parent" << endl;
        return IOERR;
    }

    // try to create a new file 
    if(ec->create(extent_protocol::T_SLINK, ino_out) != extent_protocol::OK){
        cout << "symlink: fail to create a link file" << endl;
        return IOERR;
    }

    // write the target path to link file
    if(ec->put(ino_out, linkname) != extent_protocol::OK){
        cout << "symlink: fail to write into link file" << endl;
        return IOERR;
    }

    // write entries into the directory
    if(saveentry(parent, filename, ino_out) != extent_protocol::OK){
        cout << "symlink: fail to write entries into the dir" << endl;
        return IOERR;
    }

    cout << "create a link!" << endl;
    return OK;

}

int yfs_client::readlink(inum ino, string& path){
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