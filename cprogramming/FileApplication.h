#include "Application.h"
using namespace std;
 class FileApplication: public Application {
       public:
        FileApplication(){cout << "FileApplication ctor" << endl;}
        ~FileApplication(){cout << "FileApplication dector" << endl;}
       int run() { cout << "FileApplication run" << endl; return 1;}
   };

