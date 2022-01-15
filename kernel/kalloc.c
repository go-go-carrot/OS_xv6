// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

#define PA2IDX(pa) (((uint64)pa) >> 12)

void freerange(void *pa_start, void *pa_end);
uint64 get_refcnt_idx(uint64);
char ref_increment(uint64, char);
void rcinit();

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  // Lab 5: Copy-on-Write
  //char *ref_cnt; // reference count
  //uint64 ref_start; // offset: physical memory address at which to start allocation
  // end of Lab 5 code
} kmem;

struct {
  struct spinlock lock;
  int count[PGROUNDUP(PHYSTOP) / PGSIZE];
} refcnt;

void
kinit()
{
  rcinit(); // Lab 5: Copy-on-Write
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}


// Lab 5: Copy-on-Write
// setting reference count: # of pgtbls that refer to the page
// okay to keep counts in a fixed-size int array

// initialize all pages and set refcnt to 0
void rcinit(){
  initlock(&refcnt.lock, "refcnt");

  acquire(&kmem.lock);

  for(int i = 0; i < PGROUNDUP(PHYSTOP) / PGSIZE; i++)
    refcnt.count[i] = 0;
 
  release(&kmem.lock);
}

void increase_rc(void *pa){
  acquire(&refcnt.lock);
  
  refcnt.count[PA2IDX(pa)]++;
  
  release(&refcnt.lock);
}

void decrease_rc(void *pa){
  acquire(&refcnt.lock);
  
  refcnt.count[PA2IDX(pa)]--;
  
  release(&refcnt.lock);
}

int get_rc(void *pa){
  acquire(&refcnt.lock);
  
  int rc = refcnt.count[PA2IDX(pa)];
  
  release(&refcnt.lock);
  
  return rc;
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);

  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    increase_rc((void*)p);
    kfree(p);
  }
}

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

  decrease_rc(pa);
  if(get_rc(pa) > 0)
    return;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  
  // Lab 5: Copy-on-Write
  
  /* char num = ref_increment((uint64)pa, (char)-1);

  // if refcnt != 0
  if(num > 0)
    return;
  
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
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  // Lab 5: Copy-on-Write
  // kalloc allocates pa
  // if refcnt of pa is invalid -> panic
  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    //kmem.ref_cnt[get_refcnt_idx((uint64)r)] = 1; // w/o lock
    increase_rc((void*)r);
  }

  // end of Lab 5 code

  return (void*)r;
}
