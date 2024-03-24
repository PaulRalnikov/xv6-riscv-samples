struct mutex {
	struct spinlock protect_lock; //lock for safe work
	struct sleeplock func_lock; //functional work lock
	uint64 cnt_procs; // number of procs which use mutex
};