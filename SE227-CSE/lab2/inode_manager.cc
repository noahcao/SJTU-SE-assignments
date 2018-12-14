#include "inode_manager.h"
#include <iostream>
#include <bitset>

using namespace std;

// disk layer -----------------------------------------

disk::disk()
{
  bzero(blocks, sizeof(blocks));
}

void
disk::read_block(blockid_t id, char *buf)
{
  if (id < 0 || id >= BLOCK_NUM || buf == NULL)
    return;

  memcpy(buf, blocks[id], BLOCK_SIZE);
}

void
disk::write_block(blockid_t id, const char *buf)
{
  if (id < 0 || id >= BLOCK_NUM || buf == NULL)
    return;

  memcpy(blocks[id], buf, BLOCK_SIZE);
}

// block layer -----------------------------------------

// Allocate a free disk block.
blockid_t
block_manager::alloc_block()
{
  /*
   * your code goes here.
   * note: you should mark the corresponding bit in block bitmap when alloc.
   * you need to think about which block you can start to be allocated.
   */
  
  char bitmap[BLOCK_SIZE];
  int pos;
  int found_free = 0;

  int block_index = BBLOCK(IBLOCK(sb.ninodes, sb.nblocks) + 1); // index of first block after inode table

  for(;block_index <= BBLOCK(BLOCK_NUM); block_index++){
    // read corresponding bitmap
    d->read_block(block_index, bitmap);

    // traverse every bit pos to find a 0
    for(pos = 0; pos < BPB; pos++){
      char abyte = bitmap[pos / 8];
      char abit = abyte & (((char)1 << (7 - pos%8)));
      //cout << "&&&&&&& byte and bit &&&&&" << endl;
      //cout << bitset<sizeof(char)*8>(abyte) << endl;
      //cout << bitset<sizeof(char)*8>(abit) << endl;
      
      if(abit == 0){
        // found a free block
        // cout << "before: " << bitset<sizeof(char)*8>((char)bitmap[pos/8]) << endl;
        // cout << "pos: " << pos <<  "  block_index: " << block_index << endl;
        // cout << "padding: " << bitset<sizeof(char)*8>((char)((0x1 << (7 - pos%8)))) << endl;
        bitmap[pos/8] = abyte | (((char)1 << (7 - pos%8)));
        // cout << "after: " << bitset<sizeof(char)*8>((char)bitmap[pos/8]) << endl;
        d->write_block(block_index, bitmap);
        d->read_block(block_index, bitmap);
        // cout << "after after: " << bitset<sizeof(char)*8>((char)bitmap[pos/8]) << endl;
        // cout << "********** found free block: " << block_index << endl;
        found_free = 1;
        break;
      }
    }

    if(found_free == 1){
      break;
    }
  }

  if(found_free == 1){
    // cout << "return pos: " << (block_index - BBLOCK(1)) * BPB + pos + 1 << endl;
    return (block_index - BBLOCK(1)) * BPB + pos + 1 + IBLOCK(sb.ninodes, sb.nblocks);
  }
  else{
    return 0;
  }
}

void
block_manager::free_block(uint32_t id)
{
  /* 
   * your code goes here.
   * note: you should unmark the corresponding bit in the block bitmap when free.
   */
  if(id <= 0 || id > BLOCK_NUM){
    printf("free_block: block id is out of range: %d\n", id);
    return;
  }
  // cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^ free block : " << id << endl;
  char bitmap[BLOCK_SIZE];
  d->read_block(BBLOCK(id), bitmap);

  int pos = (id - 1) % BPB;
  char abyte = bitmap[pos / 8];
  bitmap[pos/8] = abyte & ((char)1 << (7 - pos%8));
  d->write_block(BBLOCK(id), bitmap);


  return;
}

// The layout of disk should be like this:
// |<-sb->|<-free block bitmap->|<-inode table->|<-data->|
block_manager::block_manager()
{
  d = new disk();

  // format the disk
  sb.size = BLOCK_SIZE * BLOCK_NUM;
  sb.nblocks = BLOCK_NUM;
  sb.ninodes = INODE_NUM;



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

// inode layer -----------------------------------------

inode_manager::inode_manager()
{
  bm = new block_manager();
  uint32_t root_dir = alloc_inode(extent_protocol::T_DIR);
  if (root_dir != 1) {
    printf("\tim: error! alloc first inode %d, should be 1\n", root_dir);
    exit(0);
  }
}

/* Create a new file.
 * Return its inum. */
uint32_t
inode_manager::alloc_inode(uint32_t type)
{
  /* 
   * your code goes here.
   * note: the normal inode block should begin from the 2nd inode block.
   * the 1st is used for root_dir, see inode_manager::inode_manager().
   */
  struct inode *ino;
  char buf[BLOCK_SIZE];
  uint32_t inum;
 
  // find a free inode one by one traversing
  for(inum = 1; inum < INODE_NUM; inum++){
	bm->read_block(IBLOCK(inum, bm->sb.nblocks), buf);
	ino = (struct inode*)buf + inum%IPB;
	
	if(ino->type == 0) break; // fina am empty inode
        if(inum == INODE_NUM - 1){
		printf("no empty inode\n");
		return 0;
	}
  }

  // initialize the found empty inode
  ino->type = type;
  ino->size = 0;
  unsigned int current_time = (unsigned int)time(NULL);
  ino->atime = current_time;
  ino->mtime = current_time;
  ino->ctime = current_time;

  bm->write_block(IBLOCK(inum, bm->sb.nblocks), buf);

  return inum;
}

void
inode_manager::free_inode(uint32_t inum)
{
  /* 
   * your code goes here.
   * note: you need to check if the inode is already a freed one;
   * if not, clear it, and remember to write back to disk.
   */
  if(inum <=0 || inum > INODE_NUM){
    printf("free_inode: inum out of range: %d\n", inum);
    return;
  }

  struct inode *ino = get_inode(inum);

  if(ino == NULL){
    printf("free_inode: inode to free not exists!");
    return;
  }

  // set the inode type as free (0)
  ino->type = 0;
  put_inode(inum, ino);
  free(ino);
  
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

  if (inum < 0 || inum >= INODE_NUM) {
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
  /*
   * your code goes here.
   * note: read blocks related to inode number inum,
   * and copy them to buf_Out
   */
  //cout << "begin read file **************" << endl;
  if(inum < 0 || inum > INODE_NUM){
	  printf("read_file: inum out of range: %d\n", inum);
	  return;
  } 

  if (buf_out == NULL) {
      printf("read_file: buf_out pointer is NULL\n");
      return;
  }

  if (size == NULL) {
      printf("read_file: size pointer is NULL\n");
      return;
  }

  struct inode *ino = get_inode(inum);

  if(ino == NULL){
	  printf("read_file: inode not exits: %d\n", inum);
	  return;
  }

  *buf_out = (char*)malloc(ino->size);
  char block_buf[BLOCK_SIZE];
  int block_num = BLOCK_SIZE / sizeof(blockid_t);
  blockid_t indi_block_buf[block_num];
  int block_index = (ino->size + BLOCK_SIZE - 1) / BLOCK_SIZE;

  int need_indirect = 0;
  if(block_index > NDIRECT){
    need_indirect = 1;
  } 

  // number of direct blocks to be read
  int direct_num = need_indirect ? NDIRECT : block_index;

  // read content in the direct blocks
  for(int i = 0; i < direct_num; i++){
    bm->read_block(ino->blocks[i], block_buf);

    if(i == block_index - 1){
      // only part of data in the last block belongs to the file
      memcpy(*buf_out + i*BLOCK_SIZE, block_buf, ino->size-i*BLOCK_SIZE);
    }
    else{
      memcpy(*buf_out + i*BLOCK_SIZE, block_buf, BLOCK_SIZE);
    }
  }

  // if needed, continue to read the indirect blocks
  if(need_indirect == 1){
    // copy index of blocks linked by the indirect block to a buf
    bm->read_block(ino->blocks[NDIRECT], (char*)indi_block_buf);
    
    for(int i = 0; i < block_index - NDIRECT; i++){
      bm->read_block(indi_block_buf[i], block_buf);

      if(i == block_index - NDIRECT - 1){
        // only part of data in the last block belongs to the file
        memcpy(*buf_out + (i+NDIRECT)*BLOCK_SIZE, block_buf, ino->size-(i+NDIRECT)*BLOCK_SIZE);
      }
      else{
        memcpy(*buf_out + (i+NDIRECT)*BLOCK_SIZE, block_buf, BLOCK_SIZE);
      }
    }
  }
  
  // return file size
  *size = ino->size;

  // update atime
  unsigned int current_time = (unsigned int)time(NULL);
  ino->atime = current_time;
  put_inode(inum, ino);
  free(ino);
  /*
  cout << "########### read content, size:  "  << *size << "   :";

  for(int i = 0; i < *size; i++){
    cout << (char*)buf_out[i];
  }_
  cout << endl;
  cout << "########### read content, size:  "  << *size << "   :";
  */
  return;
}

/* alloc/free blocks if needed */
void
inode_manager::write_file(uint32_t inum, const char *buf, int size)
{
  /*
   * your code goes here.
   * note: write buf to blocks of inode inum.
   * you need to consider the situation when the size of buf 
   * is larger or smaller than the size of original inode
   */
  //out << "begin write file **************" << endl;
  //cout << "############# write content: " << buf << endl;
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

  //out << "begin write in write_file *****************" << endl;

  // begin writing
  blockid_t indi_block_buf[BLOCK_SIZE / sizeof(blockid_t)];
  //cout << "ino->size: " << ino->size << endl;
  //cout << "size: " << size << endl;
  //cout << "buf: " << buf << endl;
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
    //cout << "the new file is smaller ***********" << endl;
    // the new file is smaller or of equal size
    for(int i = 0; i < direct_blocks_now; i++){
      // cout << "1. write file into : " << ino->blocks[i] << endl;
      bm->write_block(ino->blocks[i], buf+i*BLOCK_SIZE);
    }

    if(need_indirect == 1){
      // if indirect blocks are needed
      bm->read_block(ino->blocks[NDIRECT], (char*)indi_block_buf);

      // write content to indirect blocks
      for(int i = 0; i < block_num_new - NDIRECT; i++){
        // cout << "2. write file into : " << ino->blocks[i] << endl;
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
    //cout << "the new file is bigger ***********" << endl;
    // when the new file is bigger the the old one 

    // step 1: write into the old blocks
    for(int i = 0; i < direct_blocks_before; i++){
      // cout << "3. write file into : " << ino->blocks[i] << endl;
      bm->write_block(ino->blocks[i], buf + i*BLOCK_SIZE);
    }

    //cout << "allocate new blocks to write ***********" << endl;
    //cout << "direct_blocks_before: " << direct_blocks_before << endl;
    //cout << "direct_blocks_now: " << direct_blocks_now << endl;
    //cout << "block_num_old: " << block_num_old << endl;
    //cout << "block_num_now: " << block_num_new << endl;
    // allocate new direct blocks to write
    for(int i = direct_blocks_before; i < direct_blocks_now; i++){
      uint32_t new_block_index = bm->alloc_block();
      // cout << "the new file is bigger *********** " << new_block_index <<  endl;
      ino->blocks[i] = new_block_index;
      // cout << "4. write file into : " << ino->blocks[i] << endl;
      bm->write_block(ino->blocks[i], buf+i*BLOCK_SIZE);
    }
    
    //cout << "finish to allocate new blocks to write ***********" << endl;

    // if need indirect blocks now, write into them
    if(need_indirect == 1){
      //cout << "write into direct blocks **********" << endl;
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

    //cout << "end write file **************" << endl;
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
  /*
   * your code goes here.
   * note: get the attributes of inode inum.
   * you can refer to "struct attr" in extent_protocol.h
   */
  if(inum < 0 || inum > INODE_NUM){
	printf("getattr: inum out of range %d\n", inum);
	return;
  }

  struct inode *ino = get_inode(inum);
  
  if(ino == NULL) return;
   
  a.type = ino->type;
  a.atime = ino->atime;
  a.mtime = ino->mtime;
  a.ctime = ino->ctime;
  a.size = ino->size;
  
  free(ino);
  
  return;
}

void
inode_manager::remove_file(uint32_t inum)
{
  /*
   * your code goes here
   * note: you need to consider about both the data block and inode of the file
   */
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
