// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);
uint64 get_refcnt_idx(uint64);
char ref_increment(uint64, char);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  // Lab 5: Copy-on-Write
  char *ref_cnt; // reference count
  uint64 ref_start; // offset: physical memory address at which to start allocation
  // end of Lab 5 code
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}


// Lab 5: Copy-on-Write
// setting reference count: # of pgtbls that refer to the page
// okay to keep counts in a fixed-size int array
int refcnt[PHYSTOP / PGSIZE];

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  // Lab 5: Copy-on-Write
  uint64 size = (uint64)(pa_end - pa_start) >> PGSHIFT;
  memset(pa_start, 1, size); // byte
  
  //
  p = (char*)pa_start + size; // sizeof(char) = 1
  p = (char*)PGROUNDUP((uint64)p);

  acquire(&kmem.lock);
  // set start point of the array as pa_start
  kmem.ref_cnt = (char*)pa_start;
  kmem.ref_start = (uint64)p;
  release(&kmem.lock);


  // maintain refcnt for every physical page
  // initialized to 1 when kalloc() allocates it
  // freerange calls kfree which reduces the value of refcnt for every pa
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    //refcnt[(uint64)p / PGSIZE] = 1;
    kfree(p);
  }
}
/*
void 
ref_increment(uint64 pa){
  // acquire lock
  acquire(&kmem.lock);
  
  int pn = pa / PGSIZE;
  if(pa > PHYSTOP || refcnt[pn] < 1)
    panic("increase reference count");
  
  // increment refcnt when fork causes a child to share the page
  refcnt[pn]++;
  
  release(&kmem.lock);
}
*/

uint64 get_refcnt_idx(uint64 pa){
  uint64 ans = (pa - kmem.ref_start) >> PGSHIFT;
  return ans;
} // get count w/o lock

char ref_increment(uint64 pa, char inc){
  acquire(&kmem.lock);

  char ans = (kmem.ref_cnt[get_refcnt_idx(pa)] += inc);

  release(&kmem.lock);

  return ans;
} // increment by refcnt


// end of Lab 5 code



// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  
  // Lab 5: Copy-on-Write
  char num = ref_increment((uint64)pa, (char)-1);

  // if refcnt != 0
  if(num > 0)
    return;

  /*
  acquire(&kmem.lock);
  
  int pn = (uint64)r / PGSIZE;
  
  if(refcnt[pn] < 1)
    panic("panic: kfree");

  // decrement refcnt of the pa when we free a page 
  refcnt[pn] -= 1;
  
  int temp = refcnt[pn];
  
  release(&kmem.lock);
  
  if(temp > 0)
    return;

  memset(pa, 1, PGSIZE);
  */
  // end of Lab 5 code

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;

  // Lab 5: Copy-on-Write
  // kalloc allocates pa
  // if refcnt of pa is invalid -> panic
  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    kmem.ref_cnt[get_refcnt_idx((uint64)r)] = 1; // w/o lock

    /*
    int pn = (uint64)r / PGSIZE;
    if(refcnt[pn])
      panic("panic: kalloc refcnt");
    //set refcnt to 1 when kalloc allocates a page
    refcnt[pn] = 1;
    kmem.freelist = r->next;
    */
  }

  release(&kmem.lock);

  // end of Lab 5 code

  return (void*)r;
}
