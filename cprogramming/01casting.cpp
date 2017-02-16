#include <stdio.h>
int main() {
 char* str = "Hello world";
 *(str) = 'c';
 char * modifiable = const_cast<char*>(str);
// *(modifiable+1) = 'c';
 //printf("%x\n", str);
 //printf("%x\n", modifiable);
}
