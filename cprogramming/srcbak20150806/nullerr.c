#include <stdio.h>

void test(void)
{
	int *i = NULL;
	*i = 3;
}

int main(void){
	printf("hello world\n");
	test();
	return 0;
}
