#include "CrossRrApp.h"

int main(int argc,char **argv)
{
    if (argc != 5)
    {
        cout << "参数个数不对" << endl;
        return 0;
    }
    CrossRrApp *crossRrApp = new CrossRrApp(argc,argv);
    int ret = crossRrApp->run();
    delete crossRrApp;
    return ret;
}
