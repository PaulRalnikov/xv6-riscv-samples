#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "user/user.h"


void testSmallBufferSize() {
    printf("start test with small buffer size\n");

    static char *states[] = {
        [UNUSED_STATE]    "unused",
        [USED_STATE]      "used",
        [SLEEPING_STATE]  "sleep",
        [RUNNABLE_STATE]  "runble",
        [RUNNING_STATE]   "run",
        [ZOMBIE_STATE]    "zombie"
    };
    
    int pid = fork();
    if (pid > 0) {
        sleep(3);

        int sz = 1;
        struct procinfo* plist = (struct procinfo*) malloc(sizeof(struct procinfo) * sz);
        int ret = ps_listinfo(plist, sz);
        
        if (ret == -1) {
            free(plist);
            printf("syscall returned -1 as it was expected\n");
            exit(0);
        }

        if (ret >= 0) {
            printf("error: given limit is enought; procs are:\n");

            for (int i = 0; i < ret; i++) {
                printf("proc number %d; name: %s \t, state: %s\t, parent pid: %d\n",
                    i, plist[i].proc_name, states[plist[i].state], plist[i].parent_pid);
            }
            free(plist);

            int wait_pid = wait((int*) 0);
            if (wait_pid != pid)
                printf("wait error: returned pid%d, but %d was expected\n", wait_pid, pid);
            exit(1);
        } else {
            printf("unexpected error\n");
            free(plist);
            exit(1);
        }
    } else if (pid == 0) {
        sleep(10);
        exit(0);
    }

    printf("fork error\n");
    exit(1);    
}

void testIncorrectAddress() {
    printf("start test with incorrect address\n");
    char c = 'a';
    struct procinfo* ptr = (struct procinfo*) (&c + 100);
    int sz = 10;
    int ret = ps_listinfo(ptr, sz);

    if (ret == -2) {
        printf("syscall returned -2 as it was expected\n");
        exit(0);
    } 
    else if (ret >= 0) {
        printf("syscall ended with success (but have to be failed)\n");
        exit(1);
    }
    printf("unexpected error\n");
    exit(1);
}

void testZeroAddress() {
    printf("start test with zero address\n");
    int sz = 100;

    int ret = ps_listinfo(0, sz);
    if (ret >= 0) {
        printf("current count of procs is %d\n", ret);
        exit(0);
    }
    printf("unexpected error\n");
    exit(1); 
}

void testNegativeLimit() {
    printf("startr test with negative limit\n");
    int lim = -1;
    int sz = 10;

    struct procinfo* plist = (struct procinfo*) malloc(sizeof(struct procinfo) * sz);
    int ret = ps_listinfo(plist, lim);

    if (ret == -1) {
        printf("syscall returned -1 as is was expected\n");
        exit(0);
    }
    if (ret >= 0) {
        printf("syscall ended with success (but have to be failed)");
        exit(1);
    }
    printf("unexpected error\n");
    exit(1);
}   


//test utility for listinfo syscall
//argv[1] = 1 -> test with small buffer size
//argv[1] = 2 -> test with incorrect address
//argv[1] = 3 -> test with zero address and large count of procs
//argv[1] = 4 -> test with negative limit and correct address;
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("incorrect argc count; expected only one, but given %d\n", argc - 1);
        exit(1);
    }
    int x = atoi(argv[1]);
    if (x < 1 || x > 4) {
        printf("Incorrect args; expected onlu one number in range [1; 4]\n");
        exit(1);
    }
    if (x == 1)
        testSmallBufferSize();
    else if(x == 2)
        testIncorrectAddress();
    else if(x == 3)
        testZeroAddress();
    else if (x == 4)
        testNegativeLimit();
    exit(0);
}