enum proc_state {
  UNUSED_STATE,
  USED_STATE,
  SLEEPING_STATE,
  RUNNABLE_STATE,
  RUNNING_STATE,
  ZOMBIE_STATE
};

struct procinfo {
  char proc_name[16];
  enum proc_state state;
  int parent_pid; 
};