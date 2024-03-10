#include "kernel/types.h"
#include "user/user.h"

void errorMessage(const char* msg) {
  write(2, msg, strlen(msg));
  exit(1);
}

void checkZero(int x, char* ptr) {
  if (x == 0 && (*ptr) != '0')
    errorMessage("Can not read one of the numbers\n");
}

char* passInteger(char* start, char* end) {
  char* ptr = start;
  while (ptr < end && *ptr >= '0' && *ptr <= '9') ptr++;
  return ptr;
}

int main(int argc, char *argv[])
{
  if (argc != 1)
    errorMessage("Incorrect number of arguments, expected 0\n");
  const int BUF_SIZE = 30;
  char buf[BUF_SIZE];
  gets(buf, BUF_SIZE);

  int realBufSize = strlen(buf);

  if (realBufSize == BUF_SIZE - 1) {
    errorMessage("Error: too large input\n");
  }
  char* firstNum = buf;
  char* secondNum = passInteger(buf, buf + realBufSize);

  if (secondNum == firstNum) errorMessage("Incorrect input\n");

  if (secondNum == buf + realBufSize)
    errorMessage("Expected two integers, but only one given\n");

  if (*secondNum != ' ')
    errorMessage("Can not find space\n");
  secondNum++;

  char* end = passInteger(secondNum, buf + realBufSize);

  if (end != buf + realBufSize && *end != '\n') 
    errorMessage("Incorrect format\n");

  int x = atoi(firstNum), y = atoi(secondNum);

  checkZero(x, firstNum);
  checkZero(y, secondNum);

  printf("%d\n", add(x, y));
  exit(0);
}
