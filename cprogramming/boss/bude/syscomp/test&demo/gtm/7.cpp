#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	char a[10];
	char b[10];

	memset(a, 0x00, sizeof(a));
	memset(b, 0x00, sizeof(b));

	strcpy(a, "hello");
	strcpy(b, "world");

	const char * p = a;
	p = b;

	const int i =0;
	i = 2;
	return 0;
}
