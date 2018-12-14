#include "namenode.h"
#include "extent_client.h"

#include <sys/stat.h>
#include <unistd.h>
#include "threader.h"

using namespace std;

void NameNode::init(const string &extent_dst, const string &lock_dst) {
  ec = new extent_client(extent_dst);
  lc = new lock_client_cache(lock_dst);
  yfs = new yfs_client(extent_dst, lock_dst);

  NewThread(this, &NameNode::DetectHeartbeat, &registry);
}

list<NameNode::LocatedBlock> NameNode::GetBlockLocations(yfs_client::inum ino) {
    list<NameNode::LocatedBlock> locatedBlocks;
    list<blockid_t> block_ids;

    ec->get_block_ids(ino, block_ids);
    extent_protocol::attr a;
    ec->getattr(ino, a);

    int block_num = block_ids.size();
    int file_size = a.size;

    list<blockid_t>::iterator iter = block_ids.begin();
    int block_ind = 0;

    for(; iter != block_ids.end(); iter++){
        blockid_t bid  = *iter;
        uint64_t block_offset = block_ind * BLOCK_SIZE;
        uint64_t block_size;
        if(block_ind + 1 == block_num){
            // the last block, may not full of data
            block_size = file_size - block_offset;
        }
        else{
            block_size = BLOCK_SIZE;
        }
        LocatedBlock new_locblock = LocatedBlock(
            bid, block_offset, block_size, GetDatanodes());
        locatedBlocks.push_back(new_locblock);
        block_ind++;
    }

    return locatedBlocks;
}

bool NameNode::Complete(yfs_client::inum ino, uint32_t new_size) {
    ec->complete(ino, new_size);
    lc->release(ino);
    return true;
}


NameNode::LocatedBlock NameNode::AppendBlock(yfs_client::inum ino) {
    extent_protocol::attr a;
    ec->getattr(ino, a);
    blockid_t new_block_id;
    ec->append_block(ino, new_block_id);

    int tail_size = a.size % BLOCK_SIZE;
    int offset = (tail_size==0) ? a.size : (a.size-tail_size+BLOCK_SIZE);

    return LocatedBlock(new_block_id, offset, BLOCK_SIZE, GetDatanodes());
}

bool NameNode::Rename(yfs_client::inum src_dir_ino, string src_name, yfs_client::inum dst_dir_ino, string dst_name) {
    // step 1: unlink former filename
    yfs_client::inum ino_out;
    bool found = false;

    std::list<yfs_client::dirent> list;
    yfs->readdir_r(src_dir_ino, list);
    std::list<yfs_client::dirent>::iterator iter = list.begin();
    for(; iter != list.end(); iter++){
        if(iter->name == src_name){
            ino_out = iter->inum;
            found = true;
            break;
        }
    }
    if(!found) return false; // original path is not found
    else{
        list.erase(iter);
        yfs->savedir(src_dir_ino, list);
    }

    if(modified_inodes.find(ino_out) != modified_inodes.end()){
        modified_inodes.erase(ino_out);
    }
    // step 2: create new entry for reference
    found = false;
    yfs_client::inum ino_num;
    yfs->lookup_r(dst_dir_ino, dst_name.c_str(), found, ino_num);

    if(found) return false; // name already existed

    // save dir
    yfs->saveentry(src_dir_ino, dst_name.c_str(), ino_out);

    modified_inodes.insert(dst_dir_ino);
    modified_inodes.insert(ino_out);
    return true;
}

bool NameNode::Mkdir(yfs_client::inum parent, string name, mode_t mode, yfs_client::inum &ino_out) {
    yfs->mkdir(parent, name.c_str(), mode, ino_out);
    modified_inodes.insert(parent);
    modified_inodes.insert(ino_out);
    return true;
}

bool NameNode::Create(yfs_client::inum parent, string name, mode_t mode, yfs_client::inum &ino_out) {   
    // check if existed
    bool found = false;
    yfs_client::inum ino_num;
    
    yfs->lookup_r(parent, name.c_str(), found, ino_num);
    if(found){
        return false;
    }
    // create file
    ec->create(extent_protocol::T_FILE, ino_out);
    lc->acquire(ino_out);
    // save dir
    yfs->saveentry(parent, name.c_str(), ino_out);

    modified_inodes.insert(parent);
    modified_inodes.insert(ino_out);
    return true;
}

bool NameNode::Isfile(yfs_client::inum ino) {
  extent_protocol::attr a;
  if (ec->getattr(ino, a) != extent_protocol::OK)  return false;
  if (a.type == extent_protocol::T_FILE) return true;
  return false;
}

bool NameNode::Isdir(yfs_client::inum ino) {
  extent_protocol::attr a;
  if (ec->getattr(ino, a) != extent_protocol::OK) return false;
  if (a.type == extent_protocol::T_DIR) return true;
  return false;
}

bool NameNode::Getfile(yfs_client::inum ino, yfs_client::fileinfo &info) {
  extent_protocol::attr a;
  if (ec->getattr(ino, a) != extent_protocol::OK) return false;

  info.atime = a.atime;
  info.mtime = a.mtime;
  info.ctime = a.ctime;
  info.size = a.size;
  return true;
}

bool NameNode::Getdir(yfs_client::inum ino, yfs_client::dirinfo &info) {
  extent_protocol::attr a;
  if (ec->getattr(ino, a) != extent_protocol::OK) {
      return false;
  }
  info.atime = a.atime;
  info.mtime = a.mtime;
  info.ctime = a.ctime;
  return true;
}

bool NameNode::Readdir(yfs_client::inum ino, std::list<yfs_client::dirent> &dir) {
  std::string buf;
  ec->get(ino, buf);
  std::stringstream ss;
  ss << buf;
  while(true){
      yfs_client::dirent entry;
      if(!std::getline(ss, entry.name, '/')){
          break;
      }
      ss >> entry.inum;
      // remove the '/' between inum and latter filename
      ss.get();
      dir.push_back(entry);
  }
  return true;
}

bool NameNode::Unlink(yfs_client::inum parent, string name, yfs_client::inum ino) {
  yfs_client::inum ino_out;
  bool found = false;
  std::list<yfs_client::dirent> list;
  Readdir(parent, list);
  std::list<yfs_client::dirent>::iterator iter = list.begin();
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
      yfs->savedir(parent, list);
  }
  if(modified_inodes.find(ino_out) != modified_inodes.end()){
    modified_inodes.erase(ino_out);
  }
  return found;
}

list<DatanodeIDProto> NameNode::GetDatanodes(){
  list<DatanodeIDProto> survived_nodes;
  map<DatanodeIDProto,time_t>::iterator it = registry.begin();
  for(; it != registry.end(); it++){
    survived_nodes.push_back(it->first);
  }
  return survived_nodes;
}

void NameNode::DatanodeHeartbeat(DatanodeIDProto id){
  time_t curr_time = time(NULL);
  if (registry.find(id) != registry.end()) registry[id] = curr_time;
  return;
}


void NameNode::RegisterDatanode(DatanodeIDProto id){
    set<yfs_client::inum>::iterator it;
    for (it = modified_inodes.begin(); it != modified_inodes.end(); it++)
    {
        list<NameNode::LocatedBlock> replicate_block;
        replicate_block = GetBlockLocations(*it);
        list<NameNode::LocatedBlock>::iterator iter;
        for (iter = replicate_block.begin(); iter!= replicate_block.end(); iter++ )
        {
            ReplicateBlock(iter->block_id, master_datanode, id);
        }
    }
    time_t curr_time = time(NULL);
    registry[id] = curr_time;
}


void NameNode::DetectHeartbeat(map<DatanodeIDProto, time_t> *registry){
  while(true){
      time_t curr_time = time(NULL);
      map<DatanodeIDProto,time_t>::iterator it;
      for (it = registry->begin();it != registry->end(); it++){
          if ((curr_time-it->second)> 4){
              it = registry->erase(it);
          }
      }
      sleep(4);
  }
}