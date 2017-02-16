/*从数据库中获取相应的ID号码*/

#ifndef GetSeqID_H
#define GetSeqID_H

#include <string.h>
#include <stdio.h>
#include "udbi.h"
#include "AuthSecuError.h"

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
