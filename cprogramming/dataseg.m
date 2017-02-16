#import <stdio.h>   // for printf()
#import <stdlib.h>  // for exit()
// gcc -arch x86_64 -g -o dataseg dataseg.m
// gcc -arch i386 -g -o dataseg-32 dataseg.m
// about 8K doubles. lives in the initialized data segment.
static double rdoubles[] = {
      0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0,
          10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0,
                    1017.0, 1018.0, 1019.0, 1020.0, 1021.0,
                  1022.0, 1023.0
};
// one meg, all zeros.  Lives in the uninitialzed data segment
static char buffer[1048576]; 
int main (void) {
      printf ("hi!  %ld bytes of doubles, %ld bytes of buffer\n",
                      sizeof(rdoubles), sizeof(buffer));
          return 0;
} // main
