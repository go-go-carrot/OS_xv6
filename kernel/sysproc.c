#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL

/*
  1. 3 args: VA of the 1st user page, # of pages to check, user address to a buffer to store the results into a bitmask
  2. bitmask: a DS that uses 1bit/pg and where the 1st page corresponds to the LSB
  3. need to parse args using argaddr() & argint
  4. setting upper limit on the # of pages scanned is OK
  5. use walk() to find right PTEs (kernel/vm.c)
  6. PTE_A: access bit, define it in kerenl/riscv.h, use RISC-V manual
  7. Be sure to clear PTE_A after checking it is set
  8. use vmprint() to debug pgtbls.
  9. output bitmask: easier to store a temp buff in the kernel & copy it to the user (copyout()) after filling it with the right bits
*/

/*
  1. copyout():
  2. 
  3. PTE_A: page access bit
*/

/* from vm.c 
pte_t* walk(pagetable_t pagetable, uint64 va, int alloc){
  if(va >= MAXVA)
    panic("walk");

  for(int level = 2; level > 0; level--){
    pte_t *pte = &pagetable[PX(level, va)];
    if(*pte & PTE_V){
      pagetable = (pagetable_t)PTE2PA(*pte);
    }
    else{
      if(!alloc || (pagetable = (pde_t*)kalloc()) == 0)
        return 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  return &pagetable[PX(0, va)];
}
*/
/* page access using walk in vm.c */
uint64 pgaccess(void *page, int num, void* store){
  struct proc* p = myproc();
  
  if(!p)
    return 1;

  pagetable_t pgtbl = p->pagetable;
  int temp = 0;

  for(int i = 0; i < num; i++){
    pte_t* pte;
    pte = walk(pgtbl, (uint64)page + (uint64)PGSIZE * i, 0);
    if(pte && ((*pte) & PTE_A)){
      temp |= 1 << i; // bitwise OR

      // clear PTE_A
      *pte ^= PTE_A;
    }
  }
    //copyout
    return copyout(pgtbl, (uint64)store, (char*)&temp, sizeof(int));
}


int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  
  uint64 buffer, temp;
  int num;

  if(argaddr(0, &buffer) < 0) 
    return -1;
  if(argint(1, &num) < 0)
    return -1;
  if(argaddr(2, &temp) < 0)
    return -1;

  return pgaccess((void*)buffer, num, (void*)temp);
  //return 0;
}
#endif




uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
