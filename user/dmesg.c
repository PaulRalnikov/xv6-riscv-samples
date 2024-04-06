#include "kernel/types.h"
#include "user/user.h"

int main(int argc, const char* argv[]) {

	const int BUF_SIZE = 128 * 4096 + 1;

	char* buf = malloc(sizeof(char) * BUF_SIZE);

	if (dmesg(buf, BUF_SIZE) != 0) {
		printf("error in dmesg\n");
		free(buf);
		exit(0);
	}

	printf(buf);
	free(buf);

	exit(0);
}