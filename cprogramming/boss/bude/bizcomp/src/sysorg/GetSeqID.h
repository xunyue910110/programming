/*ID号码获取*/

#ifndef GetSeqID_H
#define GetSeqID_H

#include <string.h>
#include <stdio.h>
#include "udbi/udbi.h"
#include "OrgMError.h"
//#include "common.h"
using namespace std;
using namespace UDBI;

//extern JConnect * pcon;

class GetSeqID
{
	public:
		GetSeqID(const string SeqCode,const string SeqCount);
		~GetSeqID();
		string GetDateSequencesId();
		string GetSequencesId();

	private:
		string str_SeqCode;
		string str_SeqCount;
		int i_ResultCode;
		string str_ResultInfo;
		string str_SeqNumber;
	
	public:
        JConnect * pcon;
};
#endif
