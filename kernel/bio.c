// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET 13

struct {
  struct spinlock bucket[NBUCKET];
  struct spinlock eviction;
  struct buf buf[NBUF];
  struct buf head[NBUCKET];
  uint next_victim;
} bcache;

static uint bhash(uint blockno)
{
  return blockno % NBUCKET;
}

void binit(void)
{
  initlock(&bcache.eviction, "bcache.eviction");
  for(int i = 0; i < NBUCKET; i++){
    initlock(&bcache.bucket[i], "bcache.bucket");
    bcache.head[i].next = 0;
  }

  for(int i = 0; i < NBUF; i++){
    struct buf *b = &bcache.buf[i];
    initsleeplock(&b->lock, "buffer");
    int bucket = i % NBUCKET;
    b->bucket = bucket;
    b->next = bcache.head[bucket].next;
    bcache.head[bucket].next = b;
  }
  bcache.next_victim = 0;
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf *bget(uint dev, uint blockno)
{
  uint target = bhash(blockno);
  acquire(&bcache.bucket[target]);
  for(struct buf *b = bcache.head[target].next; b; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.bucket[target]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  release(&bcache.bucket[target]);

  acquire(&bcache.eviction);
  acquire(&bcache.bucket[target]);

  // A concurrent miss may have installed the block while we waited.
  for(struct buf *b = bcache.head[target].next; b; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.bucket[target]);
      release(&bcache.eviction);
      acquiresleep(&b->lock);
      return b;
    }
  }

  for(int offset = 0; offset < NBUF; offset++){
    uint index = (bcache.next_victim + offset) % NBUF;
    struct buf *b = &bcache.buf[index];
    uint old = b->bucket;
    if(old != target)
      acquire(&bcache.bucket[old]);

    if(b->refcnt == 0){
      struct buf *prev = &bcache.head[old];
      while(prev->next && prev->next != b)
        prev = prev->next;
      if(prev->next == b){
        prev->next = b->next;
        b->dev = dev;
        b->blockno = blockno;
        b->valid = 0;
        b->refcnt = 1;
        b->bucket = target;
        b->next = bcache.head[target].next;
        bcache.head[target].next = b;
        bcache.next_victim = (index + 1) % NBUF;

        if(old != target)
          release(&bcache.bucket[old]);
        release(&bcache.bucket[target]);
        release(&bcache.eviction);
        acquiresleep(&b->lock);
        return b;
      }
    }

    if(old != target)
      release(&bcache.bucket[old]);
  }

  release(&bcache.bucket[target]);
  release(&bcache.eviction);
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  uint bucket = b->bucket;
  acquire(&bcache.bucket[bucket]);
  b->refcnt--;
  release(&bcache.bucket[bucket]);
}

void bpin(struct buf *b) {
  uint bucket = b->bucket;
  acquire(&bcache.bucket[bucket]);
  b->refcnt++;
  release(&bcache.bucket[bucket]);
}

void bunpin(struct buf *b) {
  uint bucket = b->bucket;
  acquire(&bcache.bucket[bucket]);
  b->refcnt--;
  release(&bcache.bucket[bucket]);
}
