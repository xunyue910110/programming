// ##########################################
// Source file : Ppmain.cpp
// System      : Mobile Billing System
// Version     : 1.28
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010820
// Update      : 20010928
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "config-all.h"

#include <unistd.h>
#include "ErrorHandle.h"
#include "PreProcApplication.h"

USING_NAMESPACE(std)

ErrorMessages g_errormessages;
ErrorMessages * const theErrorMessages = &g_errormessages;

//##ModelId=3B6E3CFF010C
int main(int argc,char* argv[])
{
	int isShow ;
	string theDataSource,theSrcName,xx;

   theDataSource  = argv[1];
   switch (argc) {
   case 2:
      isShow 	= 1;
      break;
   case 3:
      isShow 	= 1;
      xx       = argv[2];
      break;
   case 4:
      xx       = argv[2];
		isShow	= atoi(argv[3]) ;
	   break;
	default:
	   cout<<"argv error!"<<endl;
      return 0;
	}

	PreProcApplication thePreProcApplication(theDataSource,isShow);
	if ( thePreProcApplication.initialize() == false )
		return -1;

   if(xx.empty())
      xx=theSrcName;

   thePreProcApplication.run(xx);

	return 0;
}
