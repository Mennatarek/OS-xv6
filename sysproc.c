#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"
#include "spinlock.h"


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;
  
  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;
  
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_settickets(void){
  int tickets;
 if(argint(0, &tickets) < 0)
    return -1;
   else if (1 > tickets)
    return -1;
       else
        proc->tickets=tickets;
        return 0;
}


struct ptable_t {
  struct spinlock lock;
  struct proc proc[NPROC];
};
extern struct ptable_t ptable;

int
sys_getpinfo(void){
  struct pstat *pstat={0};
  if(argptr(0,(void*)&pstat,sizeof(struct pstat)) < 0)
    return -1;
acquire(&(ptable.lock));
    int processnumber=0;
    struct proc *p;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if (p->state == UNUSED)
            pstat->inuse[processnumber]=0;
        else
            pstat->inuse[processnumber]=1;

        pstat->hticks[processnumber]= p->highRunnedTimes;
        pstat->lticks[processnumber]=p->lowRunnedTimes; 
        pstat->pid[processnumber]=p->pid;
        processnumber++;
    }
    release(&(ptable.lock));
    return 0;
}
