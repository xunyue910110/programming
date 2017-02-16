#include <errno.h>
#include <iostream>
#include <unistd.h>

using namespace std;

//int execvp(const char *file, char *const argv[]);

int main()
{
	int ret;

	char *p1 = "a.out";
	char *p2 = "-l";

    	char **p = new char* [3];
	*p = p1;
        *(p+1) = p2;
        *(p+2) = 0;
 
	ret = execvp("1.out", p);

	cout << "errno=" << errno << endl;
	cout << "ret=" << ret << endl;

	return 0;
}
