#include "kernel/types.h"
#include "user/user.h"

int main(int argc, const char* argv[]) {

	stop_syslog();

	exit(0);
}