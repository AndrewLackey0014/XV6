#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_2
int
sys_pgaccess(void)
{
  // lab 2: your code here.
  uint64 start_va; //starting virtual address
	int num_pages; //number pages to check
	uint64 uaddress_bits; //user address

  //parse arguments from stack
	argaddr(0, &start_va);
	argint(1, &num_pages);
	argaddr(2, &uaddress_bits);

  int result = 0;	
	pte_t* pte_walk; 
	pte_t pte;
	pagetable_t pagetable = myproc()->pagetable;
  //vmprint(pagetable);
	for(int i = 0; i < num_pages; i++) {
		pte_walk = walk(pagetable, start_va, 0, 0);
    //vmprint(pagetable);
		pte = *pte_walk;
		if(pte & PTE_A) {
		  (*pte_walk) = pte & ~(PTE_A);
			result |= (1 << i);
		}
		start_va += PGSIZE;
	}
  //vmprint(pagetable);
	copyout(pagetable, uaddress_bits, (char *)&result, sizeof(result));
  return 0;
}


int
sys_mkhugepg(void)
{
  uint64 addr;

  argaddr(0, &addr);

  // lab 2: your code here.
  struct proc *curproc = myproc();

  // Create the new huge page
  char *hpg = kalloc_huge();

  // Copy the data from the 512 pages to the new huge page
  for (int i = 0; i < 512; i++) {
    pte_t *pte = walk(curproc->pagetable, addr + i*PGSIZE, 1, 0);
    if (pte == 0)
      break;
    //printf("we are in the for loop");
    memmove(hpg + i*PGSIZE, (char*)PTE2PA(*pte), PGSIZE);
    kfree((char*)PTE2PA(*pte));
    *pte = PA2PTE((uint64)hpg + i*PGSIZE) | PTE_V |PTE_PGH;
    //printf("PTE_FLAGS(*pte) = %d\n", PTE_FLAGS(*pte));
  }
  //printf("we are flushing");
  // Flush the TLB
  sfence_vma();

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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
