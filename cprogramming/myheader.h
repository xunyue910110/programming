#ifndef __MYHEADER_H
#define __MYHEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define swap(a, b) {typeof(a) temp = a; a = b; b = temp; }

/*
void swap(int *a, int *b)
{
    int temp = *a; *a = *b; *b = temp;
}
*/

void printArray(int array[], int n);
#endif
