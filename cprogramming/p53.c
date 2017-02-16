/* Using the popen and pclose I/O commands
 */
#include <iostream>
#include <cstdio>
#include <limits.h>
#include <unistd.h>
using namespace std;
int
main(int argc, char *argv[ ]) {
  FILE    *fin, *fout;
  char    buffer[PIPE_BUF];
  int     n;
  if (argc < 3) {
    cerr << "Usage " << argv << "cmd1 cmd2" << endl;
    return 1;
 }
  fin  = popen(argv[1], "r");
  fout = popen(argv[2], "w");
  fflush(fout);
  while ((n = read(fileno(fin), buffer, PIPE_BUF)) > 0)
    write(fileno(fout), buffer, n);
  pclose(fin);
  pclose(fout);
  return 0;
}
