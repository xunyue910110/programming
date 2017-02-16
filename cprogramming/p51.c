/* Using a pipe to send data from a parent to a child process
 */
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <wait.h>
#include <stdlib.h>
using namespace std;
int
main(int argc, char *argv[ ]) {
  int            f_des[2];
  int            f_des2[2];
  char    message[BUFSIZ];
  memset(message, '1', sizeof(message));
  if (argc != 2) {
    cerr << "Usage: " << *argv << " message\n";
    return 1;
  }
  if (pipe(f_des) == -1) {             // generate the pipe
    perror("Pipe");     return 2;
  }
  if (pipe(f_des2) == -1) {             // generate the pipe
    perror("Pipe");     return 2;
  }
  switch (fork( )) {
  case -1:
    perror("Fork");     return 3;
  case 0:                              // In the child
    close(f_des[1]);
    close(f_des2[0]);
    int nbytes;
    if ((nbytes = read(f_des[0], message, BUFSIZ)) != -1) {
       message[nbytes] = '\0'; 
      cout << "Message received by child: [" << message
           << "]" ;
        cout << "\n";
        close(f_des[0]);
    int len = strlen(message);
    if ((nbytes = write(f_des2[1], message, len)) !=  len)
        perror("child write"), exit(4);
    close(f_des2[1]);
//      cout.flush();
    } else {
      perror("Read");    return 4;
    }
    break;
  default:                             // In the Parent
    close(f_des[0]);
    if (write(f_des[1], argv[1], strlen(argv[1])) != -1) {
      cout << "Message sent by parent   : [" <<
           argv[1] << "]" ;
        cout << "\n";
        close(f_des[1]);
 //     cout.flush();
        wait(NULL);
    
    if ((nbytes = read(f_des2[0], message, BUFSIZ)) == -1)
    {
        perror("parent read"), exit(5);
    }
        message[nbytes] = '\0';
        printf("parent received: %s\n",  message);
        close(f_des2[0]);

    } else {
      perror("Write");   return 5;
    }
  }
  return 0;
}
