#ifndef __ROAMGSMATTRIBPROC_H__
#define __ROAMGSMATTRIBPROC_H__

#include "config-all.h"
#include "AttribCMProc.h"

class RoamGsmAttribProc:public AttribCMProc
{
  public:
    virtual int execute();

  protected:
  
  	//bool interNationalFeeAdjust();

};

bool interHeadCmp(const string &left,const string& right)
{
	return (left.length() > right.length());
}

#endif
