#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int p[2];
  if (pipe(p) != 0) {
    printf("pipe error\n");
    exit(1);
  }

  int pid = fork();
  if(pid > 0){
    if (close(p[0]) != 0) {
      printf("error: parent process can not close input channel\n");
      
      if (close(p[1]) != 0)
        printf("error: parent process can not close output channel\n");

      exit(1);
    }

    for (int i = 1; i < argc; i++) {
      int len = strlen(argv[i]);
      int written = write(p[1], argv[i], len);

      if (written != len) {
        printf("error while writing argv[%d]; written %d, but expected %d", i, written, len);
        
        if (close(p[1]) != 0)
          printf("error: parent process can not close output channel\n");
        
        exit(1);
      }

      if (write(p[1], "\n", 1) != 1) {
        printf("error while writing \\n after argv[%d]\n", i);

        if (close(p[1]) != 0)
          printf("error: parent process can not close output channel\n");
        
        exit(1);
      }
    }
    if (close(p[1]) != 0) {
      printf("error: parent process can not close output channel\n");
      exit(1);
    }
    
    int wait_pid = wait((int*) 0);
    if (wait_pid != pid) {
      printf("wait error: returned pid%d, but %d was expected\n", wait_pid, pid);
      exit(1);
    }

    exit(0);  
  } else if(pid == 0){
    if (close(p[1]) != 0) {
      printf("error: children process can not close output channel\n");

      if (close(p[0]) != 0)
        printf("error: children process can not close input channel\n");

      exit(1);
    }
    
    if (close(0) != 0) {
      printf("error: children process can not close standart input\n");
      exit(1);
    }
    dup(p[0]);
    if (close(p[0]) != 0) {
      printf("error: children process can not close input channel\n");
      exit(1);
    }

    char *new_argv[] = {"/wc", 0};
    if (exec("/wc", new_argv) != 0) {
      printf("exec error\n");
      exit(1);
    }
    exit(0);
  } else {
    printf("fork error\n");

    if (close(p[0]) != 0)
      printf("error: can not close input channel\n");
    if (close(p[1]) != 0)
      printf("error: can not close output channel\n");
    
    exit(1);
  }
  exit(0);
}
