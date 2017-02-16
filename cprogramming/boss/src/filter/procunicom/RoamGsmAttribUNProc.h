#ifndef __ROAMGSMATTRIBUNPROC_H__
#define __ROAMGSMATTRIBUNPROC_H__

#include "config-all.h"
#include "AttribUNProc.h"

class RoamGsmAttribUNProc:public AttribUNProc
{
  public:
    virtual int execute();

  protected:
  	void initialattr();
};

bool interHeadCmp(const string &left,const string& right)
{
	return (left.length() > right.length());
}

#endif
