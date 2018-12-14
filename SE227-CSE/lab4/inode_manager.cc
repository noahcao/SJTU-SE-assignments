#include <time.h>
#include "inode_manager.h"

// ######################### disk layer #######################

disk::disk()
{
  bzero(blocks, sizeof(blocks));
}

void
disk::read_block(blockid_t id, char *buf)
{
  if(id < 0 || id >= BLOCK_NUM || buf == NULL){
    printf("\tdisk::read_block: error! block_id invalid\n");
    exit(0);
  }
  memcpy(buf, blocks[id], BLOCK_SIZE);
}

void
disk::write_block(blockid_t id, const char *buf)
{
  if(id < 0 || id >= BLOCK_NUM || buf == NULL){
    printf("\tdisk::write_block: error! block_id invalid\n");
    exit(0);
  }
  memcpy(blocks[id], buf, BLOCK_SIZE);
}

// ########################## block layer #######################

// Allocate a free disk block.
blockid_t
block_manager::alloc_block()
{
  blockid_t block_id = 0;
  int count = 0;
  bool got_free_block = false;
  //get the first bitmap block
  blockid_t bitmap_block_id = BBLOCK(IBLOCK(INODE_NUM,sb.nblocks)+1);
  char buf[BLOCK_SIZE];
  // loop every bit map block
  for(; bitmap_block_id <= BBLOCK(BLOCK_NUM); bitmap_block_id ++){
    // loop every bit in one block
    d->read_block(bitmap_block_id, buf);
    for(int offset = 0; offset < BLOCK_SIZE * 8; offset++){
      count++;
      int pos_in_block = offset / 8;
      int pos_in_byte = 7 - offset % 8;
      if((buf[pos_in_block] & (1 << pos_in_byte)) == 0){
        // set this bit to 1
        buf[pos_in_block] = buf[pos_in_block] | (1 << pos_in_byte);
        d->write_block(bitmap_block_id, buf);
        block_id = count; 
        got_free_block = true;
        fprintf(stderr,"alloc count %d\n",count);
        break;
      }
      if(count == BLOCK_NUM){
        //no free block found
        printf("\tbm::alloc_block: no free block found\n");
        return 0;
      }
    }
    // got a free block
    if(got_free_block) break;
  }
  return block_id;
}

void
block_manager::free_block(uint32_t id)
{
  char buf[BLOCK_SIZE];

  //get corresponding bit map
  d->read_block(BBLOCK(id), buf);
  int pos_in_block = ((id - 1) % BPB) / 8;

  // bit position in corresponding byte
  int pos_in_byte = 7 - ((id - 1) % BPB) % 8;
  //
  buf[pos_in_block] = buf[pos_in_block] & (~(1 << pos_in_byte));
  d->write_block(BBLOCK(id), buf);
  return;
}

// The layout of disk should be like this:
// |<-sb->|<-free block bitmap->|<-inode table->|<-data->|

block_manager::block_manager() {
    d = new disk();

  // format the disk
  sb.size = BLOCK_SIZE * BLOCK_NUM;
  sb.nblocks = BLOCK_NUM;
  sb.ninodes = INODE_NUM;

  char buf[BLOCK_SIZE];
  memset(buf, -1, BLOCK_SIZE);
  int n = BBLOCK(IBLOCK(INODE_NUM, BLOCK_NUM));

  // first n-1 part
  for(int i = BBLOCK(1); i < n; i++){
    d->write_block(i, buf);
  }

  //last part

  // last byte position in block
  int pos_in_byte = (IBLOCK(INODE_NUM, BLOCK_NUM) - BPB*(n - BBLOCK(1))) / 8;

  // last bit position in block
  int pos_in_bit = (IBLOCK(INODE_NUM, BLOCK_NUM) - BPB*(n - BBLOCK(1))) % 8;
  memset(buf, 0, BLOCK_SIZE);
  memset(buf, -1, pos_in_byte);
  if(pos_in_bit > 0){
    char tmp_byte = 0;
    for(int i = 0; i < pos_in_bit; i++){
      tmp_byte |= 1 << (7 - i);
    }
    buf[pos_in_byte] = tmp_byte;
  }
  d->write_block(n, buf);

}

void
block_manager::read_block(uint32_t id, char *buf)
{
  d->read_block(id, buf);
}

void
block_manager::write_block(uint32_t id, const char *buf)
{
  d->write_block(id, buf);
}

// ######################## inode layer #####################

inode_manager::inode_manager()
{
  bm = new block_manager();
  uint32_t root_dir = alloc_inode(extent_protocol::T_DIR);
  if (root_dir != 1) {
    printf("\tim: error! alloc first inode %d, should be 1\n", root_dir);
    exit(0);
  }
}

int inode_manager::min(int n1, int n2)
{
  return n1 > n2 ? n2 : n1;
}

int inode_manager::max(int n1, int n2)
{
  return n1 > n2 ? n1 : n2;
}

/* Create a new file.
 * Return its inum. */
uint32_t
inode_manager::alloc_inode(uint32_t type)
{
  inode_t *node;
  uint32_t inum = 1;
  char buf[BLOCK_SIZE];

  
  // search for empty inode 
  for(inum = 1; inum <= INODE_NUM; inum++){
    bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
    node = (struct inode *)buf + (inum - 1) % IPB;

    // I assume that type = 0 refer to this inode is free
    if(node->type == 0){
      break;
    }
    // no empty inode found
    if(inum == INODE_NUM){
      printf("\tim:alloc_inode: error! no empty inode found\n");
      return 0;
    }
  }
  //initial inode
  node->type = type;
  node->size = 0;
  node->atime = (unsigned int)time(NULL);
  node->mtime = (unsigned int)time(NULL);
  node->ctime = (unsigned int)time(NULL);

  bm->write_block(IBLOCK(inum, bm->sb.nblocks), buf);

  return inum;
}

void
inode_manager::free_inode(uint32_t inum)
{
  inode_t* node = get_inode(inum);
  if(node == NULL){
    printf("\tim::free_inode: remove nothing\n");
    exit(0);
  }
  node->type = 0;
  put_inode(inum, node);
  free(node);
  return;
}


/* Return an inode structure by inum, NULL otherwise.
 * Caller should release the memory. */
struct inode* 
inode_manager::get_inode(uint32_t inum)
{
  struct inode *ino, *ino_disk;
  char buf[BLOCK_SIZE];
  printf("\tim: get_inode %d\n", inum);

  if (inum >= INODE_NUM) {
    printf("\tim: inum out of range\n");
    return NULL;
  }

  bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
  // printf("%s:%d\n", __FILE__, __LINE__);

  ino_disk = (struct inode*)buf + inum%IPB;
  if (ino_disk->type == 0) {
    printf("\tim: inode not exist\n");
    return NULL;
  }

  ino = (struct inode*)malloc(sizeof(struct inode));
  *ino = *ino_disk;

  return ino;
}

void
inode_manager::put_inode(uint32_t inum, struct inode *ino)
{
  char buf[BLOCK_SIZE];
  struct inode *ino_disk;

  printf("\tim: put_inode %d\n", inum);
  if (ino == NULL)
    return;

  bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
  ino_disk = (struct inode*)buf + inum%IPB;
  *ino_disk = *ino;
  bm->write_block(IBLOCK(inum, bm->sb.nblocks), buf);
}

#define MIN(a,b) ((a)<(b) ? (a) : (b))

/* Get all the data of a file by inum. 
 * Return alloced data, should be freed by caller. */
void
inode_manager::read_file(uint32_t inum, char **buf_out, int *size)
{
  //exception handler
  if(inum > INODE_NUM){
    printf("\tim::read_file: invalid inum %d\n", inum);
    exit(0);
  }
  if(buf_out == NULL){
    printf("\tim::read_file: null pointer buf_out\n");
    exit(0);
  }
  if(size == NULL){
    printf("\tim::read_file: null pointer size\n");
    exit(0);
  }

  struct inode* node  = get_inode(inum);

  if(node == NULL){
    printf("\tim::read_file: no inode got\n");
    exit(0);
  }

  // get size
  *size = node->size;

  *buf_out = (char *)malloc(node->size);
  int nblocks = (node->size)/BLOCK_SIZE;
  char buf[BLOCK_SIZE];

  if((node->size)%BLOCK_SIZE != 0){
    nblocks ++;
  }

  //direct block reading 
  int direct_nblocks = min(NDIRECT, nblocks);
  int i = 0;

  for(; i < direct_nblocks; i++){
    // handling last block
    if(i != nblocks - 1){
      bm->read_block(node->blocks[i], *buf_out + BLOCK_SIZE * i);
    }
    else{
      // partial copy
      bm->read_block(node->blocks[i], buf);
      memcpy(*buf_out + BLOCK_SIZE * i, buf, node->size - BLOCK_SIZE * i);
    }
  }

  //indirect block reading
  if(nblocks > NDIRECT){
    //get blocks of indirect block
    blockid_t indirect_blocks[BLOCK_SIZE/sizeof(blockid_t)];
    bm->read_block(node->blocks[NDIRECT], (char*)indirect_blocks);

    for(; i < nblocks; i++){
      
      // handling last block
      if(i != nblocks - 1){
        bm->read_block(indirect_blocks[i - NDIRECT], *buf_out + BLOCK_SIZE*i);
      }
      else{
        // partial copy
        bm->read_block(indirect_blocks[i - NDIRECT], buf);
        memcpy(*buf_out + BLOCK_SIZE * i, buf, node->size - BLOCK_SIZE * i);
      }
    }
  }
  
  //update time
  node->atime = (unsigned int)time(NULL);
  node->mtime = (unsigned int)time(NULL);
  node->ctime = (unsigned int)time(NULL);
  put_inode(inum, node);
  free(node);
  return;
}



/* alloc/free blocks if needed */
void
inode_manager::write_file(uint32_t inum, const char *buf, int size)
{
  if(inum <= 0 || inum > INODE_NUM){
    printf("read_file: inum out of range %d\n", inum);
    return;
  }

  if(buf == NULL){
    printf("write_file: buf pointer is NULL\n");
    return;
  }

  if ((size < 0) || ((unsigned)size > MAXFILE * BLOCK_SIZE)) {
    printf("write_file: size out of range %d\n", size);
    return;
  }

  struct inode *ino = get_inode(inum);
  
  if(ino->type == 0){
    printf("write_file: inode doesnot exist: %d\n", inum);
    return;
  }

  // begin writing
  blockid_t indi_block_buf[BLOCK_SIZE / sizeof(blockid_t)];
  int block_num_old = (ino->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
  int block_num_new = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

  // whether the file to be writen needs to occupy the indirect block
  int need_indirect = 0;
  if(block_num_new > NDIRECT){
    need_indirect = 1;
  }

  // whether the file before needs the indirect blocks
  int need_indirect_before = 0;
  if(block_num_old > NDIRECT){
    need_indirect_before = 1;
  }

  // number of direct blocks used before and now
  int direct_blocks_before = (need_indirect_before==1) ? NDIRECT : block_num_old;
  int direct_blocks_now = (need_indirect==1) ? NDIRECT : block_num_new;

  if(block_num_new <= block_num_old){
    // the new file is smaller or of equal size
    for(int i = 0; i < direct_blocks_now; i++){
      bm->write_block(ino->blocks[i], buf+i*BLOCK_SIZE);
    }

    if(need_indirect == 1){
      // if indirect blocks are needed
      bm->read_block(ino->blocks[NDIRECT], (char*)indi_block_buf);

      // write content to indirect blocks
      for(int i = 0; i < block_num_new - NDIRECT; i++){
        bm->write_block(ino->blocks[i], buf+i*BLOCK_SIZE);
      }
    }

    for(int i = block_num_new; i < direct_blocks_before; i++){
      // free now unused direct blocks
      bm->free_block(ino->blocks[i]);
    }

    if(block_num_old > NDIRECT){
      // free now unused indirect blocks
      for(int i = ((need_indirect == 1) ? block_num_new - NDIRECT : 0); i < block_num_old - NDIRECT; i++){
        bm->free_block(indi_block_buf[i]);
      }
      if(need_indirect == 0){
        // the indirect block is also not needed
        bm->free_block(ino->blocks[NDIRECT]);
      }
    }
  }
  else{
    // when the new file is bigger the the old one 

    // step 1: write into the old blocks
    for(int i = 0; i < direct_blocks_before; i++){
      bm->write_block(ino->blocks[i], buf + i*BLOCK_SIZE);
    }
    // allocate new direct blocks to write
    for(int i = direct_blocks_before; i < direct_blocks_now; i++){
      uint32_t new_block_index = bm->alloc_block();
      ino->blocks[i] = new_block_index;
      bm->write_block(ino->blocks[i], buf+i*BLOCK_SIZE);
    }

    // if need indirect blocks now, write into them
    if(need_indirect == 1){
      if(need_indirect_before == 1){
        bm->read_block(ino->blocks[NDIRECT], (char*)indi_block_buf);
      }
      else{
        uint32_t indi_block_num = bm->alloc_block();
        ino->blocks[NDIRECT] = indi_block_num;
      }

      // write into old indirect blocks if there are some
      for(int i = 0; i < (need_indirect_before == 1 ? block_num_old - NDIRECT : 0); i++){
        bm->write_block(indi_block_buf[i], buf+(i+NDIRECT)*BLOCK_SIZE);
      }

      // allocate new indirect blocks to write, if needed
      for(int i = (need_indirect_before == 1)?block_num_old - NDIRECT:0; i < block_num_new - NDIRECT; i++){
        uint32_t new_block_index = bm->alloc_block();
        indi_block_buf[i] = new_block_index;

        bm->write_block(indi_block_buf[i], buf+(i+NDIRECT)*BLOCK_SIZE);
      }

      // save the indirect block index
      if(need_indirect == 1){
        bm->write_block(ino->blocks[NDIRECT], (char*)indi_block_buf);
      }
    }
  }

  // update size and mtime
  unsigned int current_time = (unsigned int)time(NULL);
  ino->size = size;
  ino->mtime = current_time;
  ino->ctime = current_time;
  put_inode(inum, ino);
  free(ino);

  return;
}


void
inode_manager::getattr(uint32_t inum, extent_protocol::attr &a)
{
    if(inum < 0 || inum > INODE_NUM){
        printf("getattr: inum out of range %d\n", inum);
        return;
    }

    inode_t* node = get_inode(inum);

    if(node == NULL){
        printf("got no corresponding inode: %d\n", inum);
        return;
    }
    
    a.type = node->type;
    a.size = node->size;
    a.atime = node->atime;
    a.mtime = node->mtime;
    a.ctime = node->ctime;

    free(node);
    return;
}

void
inode_manager::remove_file(uint32_t inum)
{
    if(inum <= 0 || inum > INODE_NUM){
        printf("remove_file: inum out of range: %d\n", inum);
        return;
    }

    struct inode *ino = get_inode(inum);

    int block_used = (ino->size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int need_indirect = block_used > NDIRECT ? 1 : 0;
    int direct_blocks = (need_indirect==1) ? NDIRECT : block_used;

    // free used direct blocks
    for(int i = 0; i < direct_blocks; i++){
        bm->free_block(ino->blocks[i]);
    }

    // free used indirect blocks, if needed
    if(need_indirect == 1){
        blockid_t indi_block_buf[BLOCK_SIZE / sizeof(blockid_t)];
        bm->read_block(ino->blocks[NDIRECT], (char*)indi_block_buf);

        for(int i = 0; i < block_used - NDIRECT; i++){
            bm->free_block(indi_block_buf[i]);
        }
    }

    free_inode(inum);
    free(ino);

    return;   
}

void
inode_manager::append_block(uint32_t inum, blockid_t &bid)
{
    if(inum <= 0 || inum > INODE_NUM){
        printf("append_block: inum out of range: %d\n", inum);
        return;
    }  

    struct inode *ino = get_inode(inum);
    blockid_t new_block_id = bm->alloc_block();
    int file_size = ino->size;
    int curr_block_num = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if(curr_block_num >= NDIRECT){
        // case 2: the inode already full direct blocks, but has room for indirect blocks
        blockid_t indi_block_buf[BLOCK_SIZE / sizeof(blockid_t)];
        int curr_indi_block_num = curr_block_num - NDIRECT;
        bm->read_block(ino->blocks[NDIRECT], (char*)indi_block_buf);
        indi_block_buf[curr_indi_block_num] = new_block_id;
        bm->write_block(ino->blocks[NDIRECT], (char*)indi_block_buf);
    }
    else{
        // case 3: append the new block into the inode's direct blocks
        ino->blocks[curr_block_num] = new_block_id;
    }

    bid = new_block_id;
    ino->size += BLOCK_SIZE;
    put_inode(inum, ino);
    free(ino);
}
void
inode_manager::get_block_ids(uint32_t inum, std::list<blockid_t> &block_ids)
{
  inode_t* node = get_inode(inum);
  int old_nblocks = (node->size + BLOCK_SIZE - 1)/BLOCK_SIZE;

  if(node == NULL){
    printf("\tim::get_blocks_ids: no inode got\n");
    exit(0);
  }
  bool magic_tag = true;
  blockid_t indirect_blocks[BLOCK_SIZE/sizeof(blockid_t)];

  for(int i = 0; i < old_nblocks; i++){
    if(i < NDIRECT){
      block_ids.push_back(node->blocks[i]);
    }
    else{
      if(magic_tag){
        bm->read_block(node->blocks[NDIRECT], (char*)indirect_blocks);
        magic_tag = false;
      }
      block_ids.push_back(indirect_blocks[i - NDIRECT]);
    }
  }
  //double check
  if(block_ids.size() != old_nblocks){
    assert(0);
  }
  free(node);
}

void
inode_manager::read_block(blockid_t id, char buf[BLOCK_SIZE])
{
  bm->read_block(id, buf);
}

void
inode_manager::write_block(blockid_t id, const char buf[BLOCK_SIZE])
{
  bm->write_block(id, buf);
}

void
inode_manager::complete(uint32_t inum, uint32_t size)
{
  inode_t* node = get_inode(inum);
  if(node == NULL){
    printf("\tim::complete: no inode got\n");
    exit(0);
  }
  node->size = size;
  unsigned int present_time = (unsigned int)time(NULL);
  node->atime = present_time;
  node->mtime = present_time;
  node->ctime = present_time;
  put_inode(inum, node);
  free(node);
}
