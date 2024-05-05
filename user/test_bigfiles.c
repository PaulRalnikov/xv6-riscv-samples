#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char* bigfilename = "bigfile";

const int BUFSIZE = 128 * sizeof(uint64);//in bytes

const uint64 mod = 2000000011;
const uint64 mult = 57, addi = 13;

uint64 nxt(uint64 curr) {
  return (curr * mult + addi) % mod;
}

//returns 0 in success, -1 or -2 otherwise
int test_write(uint64 sz, uint64 fst) {
  int fd = open(bigfilename, O_CREATE | O_WRONLY);

  uint64 curr = fst;
  char buf[BUFSIZE];
  int currbufsz = 0;
  
  for (uint64 i = 0; i < sz; i++) {
  	if (currbufsz == BUFSIZE) {
  		if (write(fd, buf, BUFSIZE) != BUFSIZE) {
  			close(fd);
  			return -1;
  		}
  		currbufsz = 0;
  	}
  	memcpy(buf + currbufsz, (char*)(&curr), sizeof(curr));
  	currbufsz += sizeof(curr);
  	curr = nxt(curr);	
  }
  if (currbufsz != 0 && write(fd, buf, currbufsz) != currbufsz) {
  	close(fd);
  	return -1;
  }
  close(fd);
  return 0;
}

//returns 0 in success, -1 or -2 otherwise
int test_read(uint64 sz, uint64 fst) {
  int fd = open(bigfilename, O_RDONLY);

  uint64 curr = fst, readel;
  
  char buf[BUFSIZE];
  int cntuint = BUFSIZE / sizeof(uint64);
  while (sz > cntuint) {
  	if (read(fd, buf, BUFSIZE) != BUFSIZE) {
  		close(fd);
  		return -1;
  	}
  	for (int i = 0; i < cntuint; i++) {
  		readel = *((uint64*)(buf + i * sizeof(uint64)));
  		if (readel != curr) {
  			close(fd);
  			return -2;
  		}
  		curr = nxt(curr);
  	}
  	sz -= cntuint;
  }

  int btsz = sz * sizeof(uint64);	
  if (read(fd, buf, btsz) != btsz) {
		close(fd);
		return -1;
	}

	for (int i = 0; i < sz; i++) {
		readel = *((uint64*)(buf + i * sizeof(uint64)));
		if (readel != curr) {
			close(fd);
			return -2;
		}
		curr = nxt(curr);
	}
  close(fd);
  return 0;
}

uint64 my_atoi(char* s, uint32* cntbad) {
	uint64 res = 0;
	while (*s != 0) {
		if (*s < '0' || *s > '9') {
			(*cntbad)++;
			return 0;
		}
		res = res * 10 + *s++ - '0';
	}
	return res;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf(
			"Incorrect number of params; expected 2, but %d was given\n",
			argc - 1
		);
		exit(1);
	}
	uint32 cntbad = 0;
	uint64 sz = my_atoi(argv[1], &cntbad);
	if (cntbad != 0) {
		printf("Size is not an unsigned integer\n");
		exit(1);
	}

	uint64 fst = my_atoi(argv[2], &cntbad);
	if (cntbad != 0) {
		printf("First elementh of sequence is not an unsigned integer\n");
		exit(1);
	}
	printf("sz = %d, fst = %d\n", sz, fst);

	if (test_write(sz, fst) != 0) {
		printf("error while whiting\n");
		exit(-1);
	}

	printf("Data was written successfully\n");

	if (test_read(sz, fst) != 0) {
		printf("error while reading\n");
		exit(-1);
	}

	printf("Data was read successfully\n");

	exit(0);
}
