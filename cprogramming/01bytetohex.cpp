#include <stdlib.h>
#include <string.h>
#include <stdio.h>
inline void halfByteToHex(unsigned char hb, char*& hexBuffer)
{
  if (hb < 10) {
  *hexBuffer++ = '0' + hb;
  } else {
  *hexBuffer++ = 'A' + (hb - 10);
  }
}

inline void bytesToHex(unsigned char* bytes, size_t len, char*& hexBuffer)
{
for (size_t i = 0; i < len; i++) {
  halfByteToHex((bytes[i] & 0xF0) >> 4, hexBuffer);
  halfByteToHex((bytes[i] & 0x0F), hexBuffer);
}
}


int main(){
 unsigned char cstr[] = "abcd";
 char buff[9];
 char *p = buff;
 bytesToHex((unsigned char*)cstr, strlen((char*)cstr), p);
 buff[9] = '\0';
 printf("%s\n", buff);
}
