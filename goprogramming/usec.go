// usec project main.go
package main

/*
#include <stdio.h>
#include <stdlib.h>
void sayhello(){
	printf("Hello world!\n");
}
*/
import (
	"C"
)
func main() {
	C.sayhello()
}
