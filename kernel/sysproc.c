#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
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

  backtrace();

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

// Lab4: Traps - Alarm
// add two system calls (sigreturn, sigalarm)
// also added to kernel/defs.h
// see kernel/trap.c for more: when handling clock interrupt in usertrap and the process needs to be alarmed, save current trap frame in etpfm and call the handler
// handler address is in trapframe->epc, return to user space and execute the function

uint64
sys_sigalarm(void){
  struct proc *p = myproc(); 
  int interval; // should I declare these or can I just use p->interval instead?
  uint64 handler;

  if(argint(0, &interval) < 0) // &(p->interval)
    return -1;

  if(argaddr(1, &handler) < 0) // &(p->handler)
    return -1;

  // see kernel/proc.h
  // pass the value to current process
  p->alarm_interval = interval; 
  p->alarm_handler = handler;

  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc *p = myproc();
  memmove(p->trapframe, &(p->etpfm), sizeof(struct trapframe));
  p->alarm_passed = 0;
  
  return 0;
}
