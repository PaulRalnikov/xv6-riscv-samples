#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_allocmtx(){
	int desc = allocmutex();
	if (desc < 0) return -1;
	if (addmtx(desc) < 0) return -2;
	return desc;
}

uint64
sys_acquiremtx() {
	int desc;
	argint(0, &desc);
	return acquiremutex(desc);
}

uint64
sys_releasemtx() {
	int desc;
	argint(0, &desc);
	return releasemutex(desc);
}

uint64
sys_freemtx() {
	int desc;
	argint(0, &desc);
	return freemutex(desc);
}