#ifndef _PROCESSORALL_H_INCLUDE_200800716_
#define _PROCESSORALL_H_INCLUDE_200800716_

#include "config-all.h"
#include "FilterGlobalInfo.h"

class Processorall
{
  public:
    bool initialize(FilterGlobalInfo *r_pFilterInfo);
    int process();

  protected:
    FilterGlobalInfo  *m_pFilterInfo;
    FilterCdr         *m_pcdr;
    FilterConfig      *m_pConfig;
    ProcessorRule     *m_pProcessorRule;

};

#endif //_PROCESSORALL_H_INCLUDE_200800716_