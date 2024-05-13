#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h" 

void go_dir(char* drname) {
  if (chdir(drname) != 0) {
    printf("Cant go to directory %s\n", drname);
    exit(1);
  }
} 

void create_go_dir(char* drname) {
  if (mkdir(drname) != 0) {
    printf("Cant create directory %s\n", drname);
    exit(1);
  }
  go_dir(drname);
}

void create_file(char* fname, char* content) {
  int fd = open(fname, O_CREATE | O_WRONLY);
  if (fd < 0) {
    printf("Cant create file %s\n", fname);
    exit(1);
  }
  int n = strlen(content);
  if (write(fd, content, n) != n) {
    printf("Cant write content to file %s\n", fname);
    exit(1);
  }
  if (close(fd) != 0) {
    printf("Cant close file %s\n", fname);
    exit(1);
  }
}

void create_link(char* lnkname, char* fname) {
  if (symlink(fname, lnkname) != 0) {
    printf("Cant create link %s to file %s\n", lnkname, fname);
    exit(1);
  }
}

void build_and_return() {
  create_go_dir("dir");
  create_file("f1", "File 1 content");
  create_go_dir("d1");
  create_go_dir("d2");
  create_file("f2", "File 2 content");
  create_link("l2a", "/dir/d1/d2/f2");
  create_link("l2r", "f2");
  create_link("l1r", "../../f1");
  create_link("lra", "l2a");
  create_link("laa", "/dir/d1/d2/l2a");
  create_link("lrr", "l2r");
  create_link("lar", "/dir/d1/d2/l2r");
  create_link("lsf", "lsf");
  create_link("lp1", "lp2");
  create_link("lp2", "lp3");
  create_link("lp3", "lp1");
  create_link("ia", "/dir/rnd");
  create_link("ir1", "f1");
  create_link("ir2", "../../f2");
  create_link("ir3", "d3/d4/f2");
  create_link("l3r", "d3/d4/f3");
  create_go_dir("d3");
  create_go_dir("d4");
  create_file("f3", "File 3 content");

  if (chdir("../../../../../") != 0) {
    printf("Cant return to root dir\n");
    exit(1);
  }
}

void do_ls() {
  int pid = fork();
  if (pid  == 0) {
    char* argc[2] = {"/ls", 0};
    if (exec("/ls", argc) != 0) {
      printf("exec error\n");
      exit(1);
    }
    exit(0);
  } else {
    int ret;
    if (wait(&ret) != pid) {
      printf("wait error\n");
      exit(1);
    }
    if (ret != 0) {
      printf("error in child\n");
      exit(1);
    }
  }
}

void go_and_do_ls(char* drname) {
  go_dir(drname);
  printf("done chdir to %s\n", drname);
  do_ls();
}

void check_correct_link(char* lname) {
  int fd = open(lname, 0);
  if (fd < 0) {
    printf("Link %s must me correcct, but open fails\n", lname);
    exit(1);
  } else {
    printf("Link %s was opened succesfully\n", lname);
    close(fd);
  }

}

void check_incorrect_link(char* lname) {
  int fd = open(lname, 0);
  if (fd >= 0) {
    printf("Link %s must me incorrecct, but can be opened\n", lname);
    close(fd);
    exit(1);
  } else
    printf("Link %s wasnt opened as it was expected\n", lname);
}

void ls_check_and_return() {
  do_ls();
  go_and_do_ls("dir");
  go_and_do_ls("d1");
  go_and_do_ls("d2");
  go_and_do_ls("d3");
  go_and_do_ls("d4");
  go_dir("../../");

  printf("returned to dir d2\n");

  check_correct_link("l2a");
  check_correct_link("l2r");
  check_correct_link("l1r");
  check_correct_link("lra");
  check_correct_link("lrr");
  check_correct_link("lar");
  check_incorrect_link("lsf");
  check_incorrect_link("lp1");
  check_incorrect_link("lp2");
  check_incorrect_link("lp3");
  check_incorrect_link("ila");
  check_incorrect_link("ir1");
  check_incorrect_link("ir2");
  check_correct_link("l3r");

  go_dir("../../../");
}

void check_content_equal(char* fname1, char* fname2) {
  const int N = 512;
  int fd1, fd2;
  char buf1[N], buf2[N];

  if ((fd1 = open(fname1, O_RDONLY)) < 0) {
    printf("Cant open file %s to read content\n", fname1);
    exit(1);
  }
  if ((fd2 = open(fname2, O_RDONLY)) < 0) {
    printf("Cant open file %s to read content\n", fname1);
    close(fd1);
    exit(1);
  }

  int n1 = read(fd1, buf1, N);
  if (n1 < 0 || n1 == N) {
    printf("Incorrect read from file %s\n", fname1);
    close(fd1);
    close(fd2);
  }
  buf1[n1] = 0;
  close(fd1);

  int n2 = read(fd2, buf2, N);
  if (n2 < 0 || n2 == N) {
    printf("Incorrect read from file %s\n", fname2);
    close(fd2);
  }
  buf2[n2] = 0;
  close(fd2);

  if (strcmp(buf1, buf2) != 0) {
    printf("Content of files %s and %s differes :(\n", fname1, fname2);
    exit(1);
  }
  printf("Contents of files %s and %s are equal!\n", fname1, fname2);
}

void check_contents_and_return() {
  go_dir("dir/d1/d2");
  check_content_equal("l2a", "f2");
  check_content_equal("l2r", "f2");
  check_content_equal("l1r", "../../f1");
  check_content_equal("lra", "f2");
  check_content_equal("laa", "f2");
  check_content_equal("lrr", "f2");
  check_content_equal("lar", "f2");
  check_content_equal("l3r", "d3/d4/f3");
  go_dir("../../../");
}

//plan:

//dir
// |- f1
// |- d1
//    |- f2
//    |- l2a -> /dir/d1/d2/f2
//    |- l2r -> f2
//    |- l1r -> ../../f1
//    |- lra -> l2a
//    |- laa -> /dir/d1/d2/l2a
//    |- lrr -> l2r
//    |- lar -> /dir/d1/d2/l2r
//    |- lsf -> lsf
//    |- lp1 -> lp2
//    |- lp2 -> lp3
//    |- lp3 -> lp1
//    |- ia -> /dir/rnd
//    |- ir1 -> f1
//    |- ir2 -> ../../f2
//    |- ir3 -> d3/d4/f2
//    |- l3r -> d3/d4/f3
//    |- d3
//       |-d3
//         |- d4
//           |- f3

int
main(int argc, char *argv[])
{
  build_and_return();

  ls_check_and_return();

  check_contents_and_return();

  printf("Test finished successfully!\n");

  exit(0);
}