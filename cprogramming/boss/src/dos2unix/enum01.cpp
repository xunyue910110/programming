#include <iostream>
#include <stdio.h>
using namespace std;
class Hello 
{
  public:
      enum Status {S_READY = 0, S_RUN = 2, S_DEAD = 3};
        void setStatus(Status);
  private:
          Status s;
};

int main(int argc ,char *argv[])
{
  Hello::Status hs;
  hs = Hello::S_READY;

  Hello hello;
  hello.setStatus(hs);


  /*
  enum Status {S_1 = -1, S_2 = 2, S_3 = 3 };
	Status s;
  //s = 2;
	s= S_1;	
	cout << "s=" << s << endl;

  int ss = S_1;
  printf("ss = %d\n", ss);

	s=S_2;
	cout << "s=" << s << endl;

	cout << "S_1=" << S_1 << endl;
	cout << "S_2=" << S_2 << endl;
	cout << "S_3=" << S_3 << endl;

	string s1="13243";
	cout << "s1=" << s1.size() << endl;	
  */
	return 0;
}


