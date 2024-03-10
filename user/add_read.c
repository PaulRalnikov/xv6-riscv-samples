#include "kernel/types.h"
#include "user/user.h"

void readToEnd() {
  char c;
  int cc;
  for(;;) {
    cc = read(0, &c, 1);
    if (cc < 1 || c == '\n') break;
  }
}

void errorMessage(const char* msg) {
  write(2, msg, strlen(msg));
  exit(1);
}

void errorMessageAndReadToEnd(const char* msg) {
  readToEnd();
  errorMessage(msg);
}


int main(int argc, char *argv[])
{
  if (argc != 1)
    errorMessageAndReadToEnd("Incorrect number of arguments, expected 0\n");
  int x = 0, cc, cntRead = 0;
  char c;

  for (;;) {
    cc = read(0, &c, 1);
    if (cc < 1) errorMessage("Incorrect format\n");
    if (c >= '0' && c <= '9') {
      x = x * 10 + c - '0';
      cntRead++;
    } else if (c == ' ') break;
    else if (c == '\n') errorMessage("Expected two integers, but given only one\n");
    else errorMessageAndReadToEnd("Incorrect format\n");
  }
  if (cntRead == 0) errorMessageAndReadToEnd("First number not found\n");

  int y = 0;
  cntRead = 0;
  for (;;) {
    cc = read(0, &c, 1);
    if (cc < 1) break;
    if (c >= '0' && c <= '9') {
      y = y * 10 + c - '0';
      cntRead++;
    } else if (c == '\n') break;
    else errorMessageAndReadToEnd("Incorrect format\n");
  }
  if (cntRead == 0) errorMessage("Second number not found\n");
  
  printf("%d\n", x + y);
  exit(0);
}
