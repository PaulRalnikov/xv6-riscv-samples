#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char **argv) {
	proctable();
	
	int sz = (1 << 16);
	char* arr = malloc(sz);
	if (arr == 0) {
		printf("Cant create array in heap:(\n");
		exit(1);
	}
	printf("access ro arr after creating: %d\n", wasaccess(arr, sz));
	arr[0] = 'a';
	arr[1] = 'a';
	arr[sz / 2] = 'b';
	printf("access ro arr after assiging some elements: %d\n", wasaccess(arr, sz));
	printf("access ro arr after no updates: %d\n", wasaccess(arr, sz));

	int i = 2;

	printf("acces to arr[%d] after no operations: %d\n", i, wasaccess(arr + i, sizeof(char)));
	arr[i] = 1;
	printf("acces to arr[%d] after assiging: %d\n", i, wasaccess(arr + i, sizeof(char)));
	
	free(arr);

	int lsz = (1 << 11);
	char larr[lsz];

	printf("acces to larr after creating: %d\n", wasaccess(larr, lsz));
	for (int i = 0; i < lsz; i++)
		arr[i] = '?';
	printf("acces to larr after filling: %d\n", wasaccess(larr, lsz));
	printf("acces to larr after no updates: %d\n", wasaccess(larr, lsz));

	i = 10;

	printf("access to larr[%d] after no operations: %d\n", i, wasaccess(larr + i, sizeof(char)));
	larr[i] = '!';
	printf("access to larr[%d] after assiging: %d\n", i, wasaccess(larr + i, sizeof(char)));
	
	exit(0);
}