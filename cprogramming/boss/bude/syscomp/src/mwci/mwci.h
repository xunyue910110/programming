#include "mwci/CCallerFactory.h"

//-------------
// sample code
//-------------
//void main()
//{
//
//	//env
//	tuxputenv("APPDIR=G:\\beahome\\tuxedo8.1\\samples\\atmi\\simpapp");
//	tuxputenv("TUXCONFIG=G:\\beahome\\tuxedo8.1\\samples\\atmi\\simpapp\\tuxconfig");
//	tuxputenv("FIELDTBLS32=sample.fml32");
//	tuxputenv("FLDTBLDIR32=G:\\beahome\\tuxedo8.1\\samples\\atmi\\simpapp");
//	tuxputenv("TUXCALLER_DEBUG=Y");
//	int iValue;
//	IMWCaller *theCaller;
//
//  try{
// 		theCaller=CCallerFactory::CreateCaller(0);
//  	CFmlBuf theBuf=CFmlBuf(SIZE_4K),OutBuf(SIZE_4K);
//		theCaller->Connect("192.168.8.11","4000");
//		iValue=1234;
//
//		theBuf.AppFldValue("VALUE",&iValue);
//		theBuf.AppFldValue("LOGICALNAME","look at me");
//
//		theCaller->AsyCall("LIST",theBuf);
//		theCaller->AsyGetReply(OutBuf);
//		printf("\n");
//		OutBuf.Print();
//		delete theCaller;
//	}catch(MWException &a)
//	{
//		cout<<"exception:"<<a.message<<endl;
//	}
//}
