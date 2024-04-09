#include "kernel/types.h"
#include "user/user.h"

int main(int argc, const char* argv[]) {

	const int BUF_SIZE = 128 * 4096 + 1;

	char* buf = malloc(sizeof(char) * BUF_SIZE);

	int ret = dmesg(buf, BUF_SIZE);

	if (ret != 0) {
		printf("error: dmesg returned %d\n", ret);
		free(buf);
		exit(0);
	}

	printf(buf);
	free(buf);

	exit(0);
}