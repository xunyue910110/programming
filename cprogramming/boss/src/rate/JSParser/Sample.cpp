#include <vector>
#include <algorithm>
#include "JScriptParser.h"
using namespace std;
int main() 
{
    DataBuffer inBuf;
    DataBuffer outBuf;
    try
    {
        inBuf.newField("f1",dtDouble,10);
        inBuf.newField("f2",dtDouble,10);
        inBuf.newField("f3",dtDouble,10);
        outBuf.newField("result",dtDouble,100);
        
        inBuf.pushBack("f1",50.0);
        inBuf.pushBack("f2",100.0);
        inBuf.pushBack("f3",20.0);
      
        ExecuteScript("var result = inBuf.getValue('f1')/inBuf.getValue('f2'); outBuf.pushValue('result',result);",inBuf,outBuf);
        cout << outBuf.getDouble("result") << endl;
    }catch(CException & ex)
    {
        cout << ex.toString() <<endl;
        return -1;
    }
    return 0;
}