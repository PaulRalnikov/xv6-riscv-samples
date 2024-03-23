#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "mutex.h"

struct mutex mtx[NMUTEX];

void
mutexinit(void)
{
  struct mutex* m;
  for (m = mtx; m < &mtx[NMUTEX]; m++) {
    initlock(&m->protect_lock, "spin lock");
    initsleeplock(&m->func_lock, "sleep lock");
    m->cnt_procs = 0;  
  }
}

//searches for free mutex in table
//returns index of free mutex on success
//returns -1 in all mutexes are allocated
int
allocmutex(void) {
  for (int i = 0; i < NMUTEX; i++) {
    acquire(&(mtx[i].protect_lock));
    if (mtx[i].cnt_procs == 0) {
      mtx[i].cnt_procs++;
      release(&mtx[i].protect_lock);
      return i;
    }
    release(&mtx[i].protect_lock);
  }
  return -1;
}

//return -1 on error, 0 on success
int
acquiremutex(int desc)
{
  if (desc < 0 || desc >= NMUTEX) return -1;
  acquiresleep(&mtx[desc].func_lock);
  
  return 0;
}

//returns -1 on error, 0 on success
int releasemutex(int desc)
{
  if (desc < 0 || desc >= NMUTEX) return -1;
  
  releasesleep(&mtx[desc].func_lock);
  return 0;
}


// increases number of procs uses mtx[desc]
// return 0 in success, -1 on error
int usemutex(int desc) {
	if (desc < 0 || desc > NMUTEX) return -1;
	acquire(&mtx[desc].protect_lock);
	mtx[desc].cnt_procs++;
	release(&mtx[desc].protect_lock);
	return 0;
}

//returns 1 if mtx[desc] is hold, 0 otherwise and -1 on error
int holdingmutex(int desc) {
	if (desc < 0 || desc >= NMUTEX) return -1;
	int r;
	acquire(&mtx[desc].protect_lock);
	r = holdingsleep(&mtx[desc].func_lock);
	release(&mtx[desc].protect_lock);
	return r;
}

//free mutex by its descriptor
//returns 0 on success, negative number on error
int
freemutex(int desc) {
  int holds = holdingmutex(desc);
  if (holds < 0 || (holds == 1 && releasemutex(desc) < 0)) return -2;

  acquire(&mtx[desc].protect_lock);
  mtx[desc].cnt_procs--;    
  release(&mtx[desc].protect_lock);
  return 0;
}