#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int pid = fork();
  if(pid > 0){
    int parent_pid = getpid();
    printf("parent pid=%d child pid=%d\n", parent_pid, pid);
    
    if (kill(pid) != 0) {
      printf("kill error\n");
      exit(1);
    }

    int ret;
    int wait_pid = wait(&ret);
    if (wait_pid != pid) {
      printf("wait error: returned pid%d, but %d was expected\n", wait_pid, pid);
      exit(1);
    }

    printf("child %d is done, returned: %d\n", pid, ret);
    exit(0);  
  } else if(pid == 0){
    sleep(60);
    exit(1);
  } else {
    printf("fork error\n");
    exit(1);
  }
  exit(0);
}
