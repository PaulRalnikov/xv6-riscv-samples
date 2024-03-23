#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Not enought number of args; expected at least one\n");
    exit(1);
  }
  int p_fwd[2];
  int p_bwd[2];
  if (pipe(p_fwd) != 0 || pipe(p_bwd) != 0) {
    printf("pipe error\n");
    exit(1);
  }
  int pid = fork();
  if (pid > 0) {

  	if (close(p_fwd[0]) != 0 || close(p_bwd[1]) != 0) {
  		printf("parent can not close unused parts of pipes\n");
  		exit(1);
  	}

    int len = strlen(argv[1]);
    int written = write(p_fwd[1], argv[1], len);

    if (written != len) {
	    printf("error while writing argv[1]; written %d, but expected %d", written, len);

	    if (close(p_fwd[1]) != 0 || close(p_bwd[0]) != 0)
	      printf("error: parent process can not close pipes\n");

	    exit(1);
	  }

	  if (close(p_fwd[1]) != 0) { 
	  	printf("error: parent process can not close output part of forward pipe\n");
	  	exit(1);
	  }

    int my_pid = getpid();
    int cc;
    char c;

    for(;;) {

      cc = read(p_bwd[0], &c, 1);

      if (cc < 1)
      	break;

      printf("%d parent received:%c\n", my_pid, c);
    }

    if (close(p_bwd[0]) != 0) {
	    printf("error: parent process can not close input part of pipe\n");
	    exit(1);
	  }

	  exit(0);

  } else if (pid == 0) {
  	if (close(p_fwd[1]) != 0 || close(p_bwd[0]) != 0) {
  		printf("child process can not close unused parts of pipes\n");
  		exit(1);
  	}
  	char c;
  	int cc;
  	int my_pid = getpid();
    for(;;) {
      cc = read(p_fwd[0], &c, 1);

      if (cc < 1)
      	break;

      printf("%d child received:%c\n", my_pid, c);

      if (write(p_bwd[1], &c, 1) != 1) {
      	printf("error: child process can not write symbol\n");
      	if (close(p_fwd[0]) != 0 || close(p_bwd[1]) != 0)
      		printf("error: child's process cat not close pipes\n");
      	exit(1);
      }
    }
    if (close(p_fwd[0]) != 0 || close(p_bwd[1]) != 0) {
		  printf("error: child's process can not close pipes\n");
    	exit(1);
    }
    exit(0);
  } else {
    printf("fork error\n");
    exit(1);
  }
  
  exit(0);
}