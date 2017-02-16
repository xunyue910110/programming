#ifndef FILTERINFOMGR_H_HEADER_INCLUDED_B764013F
#define FILTERINFOMGR_H_HEADER_INCLUDED_B764013F
#include "FilterInfo.h"
#include "MemFilterInfoMgr.h"
#include "FileFilterInfoMgr.h"
#include "RrConfig.h"

class FileRrRule;

class FilterInfoMgr
{
  public:
    bool setRepeatFlag(vector<FilterInfo>& r_fidInfoList,int r_fidCount,
                       const FileRrRule *r_fileRrRule);
    bool setRepeatFlag_equal();
    bool setRepeatFlag_contain();
    bool setRepeatFlag_file();//added by hanyu 08-11-27

    bool setHisRepeatFlag(FilterInfo& r_fidInfo);
    bool commitFilterInfo();
    bool initialization(const RrConfig &r_config);
    bool end();
  private:
    vector<FilterInfo*>  m_pFidInfoList;
    size_t m_fidCount;
    vector<FilterInfo*>  m_memFidInfo;
    int                  m_rrType;
    PString              m_fileType; 
    const FileRrRule    *m_fileRrRule;
    
    MemFilterInfoMgr     m_memFilterInfoMgr;
    FileFilterInfoMgr    m_fileFilterInfoMgr;
};



#endif /* FILTERINFOMGR_H_HEADER_INCLUDED_B764013F */
