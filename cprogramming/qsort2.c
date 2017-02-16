#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
#define LINE 100
int bsearch_cmp1(const void* elem1, const void* elem2){
	return strcmp((char *)elem1, (char *)elem2);
}
int main(){
	char bs[][LINE] = {
			"dfafd",
			"fdafdafda",
			"daefda",
			"derew",
			"fewewe",
			"fdaere"
	};
	qsort(bs, sizeof(bs)/sizeof(bs[0]), sizeof(bs[0]), bsearch_cmp1);
	//cout<<sizeof(bs)<<" "<<sizeof(bs[0])<<" "<<sizeof(bs)/sizeof(bs[0])<<endl;
	for(unsigned int i = 0; i < sizeof(bs)/sizeof(bs[0]); i++){
		printf("%s\n", bs[i]);
	}
}
