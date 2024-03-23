#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	int pid = fork();
	if (pid == 0) {
		sleep(10);
		int mtx = allocmtx();
		printf("mtx is %d\n", mtx);
		
		exit(0);
	} else {
		wait((int*)0);
		int mtx = allocmtx();
		printf("mtx is %d\n", mtx);
		
		exit(0);
	}
	
	exit(0);
}