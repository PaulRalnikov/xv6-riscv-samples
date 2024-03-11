#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	int curr_len;
	struct procinfo* plist = (struct procinfo*) 0;

	for(curr_len = 1;;curr_len *= 2) {
		plist = malloc((sizeof(struct procinfo) * curr_len));

		int ret = ps_listinfo(plist, curr_len);
		if (ret >= 0 && ret <= curr_len) {
			curr_len = ret;
			break;
		}
		if (ret == -2) {
			printf("Unexpected error\n");
			exit(1);
		}

		free(plist);
	}

	static char *states[] = {
	  [UNUSED_STATE]    "unused",
	  [USED_STATE]      "used",
	  [SLEEPING_STATE]  "sleep",
	  [RUNNABLE_STATE]  "runble",
	  [RUNNING_STATE]   "run",
	  [ZOMBIE_STATE]    "zombie"
	  };

	for (int i = 0; i < curr_len; i++) {
		printf("proc number %d; name: %s \t, state: %s\t, parent pid: %d\n",
			i, plist[i].proc_name, states[plist[i].state], plist[i].parent_pid);
	}

	exit(0);
}