/* A home grown last | sort cmd pipeline
 */
#include <iostream>
#include <cstdio>
#include <unistd.h>
using namespace std;
enum { READ, WRITE };

int
main( ) {
  int      f_des[2];
  if (pipe(f_des) == -1) {
    perror("Pipe");
    return 1;
  }
  switch (fork( )) {
  case -1:
    perror("Fork");
    return 2;
  case 0:                              // In the child
    dup2( f_des[WRITE], fileno(stdout));
    close(f_des[READ] );
    close(f_des[WRITE]);
    execl("/usr/bin/last", "last", (char *) 0);
    return 3;
  default:                               // In the parent
    dup2( f_des[READ], fileno(stdin));
    close(f_des[READ] );
    close(f_des[WRITE]);
    execl("/bin/sort", "sort", (char *) 0);
    return 4;
  }
  return 0;
}

