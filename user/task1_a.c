#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int pid = fork();
  if(pid > 0){
    int parent_pid = getpid();
    printf("parent pid=%d child pid=%d\n", parent_pid, pid);
    int ret;
    pid = wait(&ret);
    printf("child %d is done, returned: %d\n", pid, ret);
    exit(0);  
  } else if(pid == 0){
    sleep(100);
    exit(1);
  } else {
    printf("fork error\n");
  }
  exit(0);
}
