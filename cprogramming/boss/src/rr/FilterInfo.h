/**
*   过滤信息结构定义类.
*   @author 高将飞 <gaojf@lianchuang.com>
*          
*   @version 1.0
*   @date 2008-08-08
*/
#ifndef FILTERINFO_H_HEADER_INCLUDED_B76432A4
#define FILTERINFO_H_HEADER_INCLUDED_B76432A4

#include "base/PString.h"

class FilterInfo
{
  public:
    PString m_fid;   //排重字段信息（包括完全相等的部分）
    long m_beginTime; //通话起始时间
    long m_endTime;   //通话终止时间
    int m_filePos;   //该文件在处理时m_srcFileNames的位置
    #ifdef _DEBUG_
      string m_fileName;//源文件名
      size_t m_line;    //在文件中行数
    #endif 
    int m_rrFlag  ; //排重标记：0 未经排重，1已经过排重
    int m_uniqFlag; //-1: 不需排重 0: 非重单、1 文件内重单、
                    // 2：内存文件间重单 3: 历史文件间重单 
  public:
    FilterInfo();
    FilterInfo(const FilterInfo &r_right);
    virtual ~FilterInfo();
    FilterInfo& operator=(const FilterInfo &r_right);
};

class HisFidInfo
{
  public:
    PString m_fid;       //排重字段信息（包括完全相等的部分）
    long    m_beginTime; //通话起始时间
    long    m_endTime;   //通话终止时间
  public:
    HisFidInfo();
    virtual ~HisFidInfo();
    HisFidInfo(const HisFidInfo &r_right);
    HisFidInfo& operator=(const HisFidInfo &r_right);
    HisFidInfo& operator=(const FilterInfo &r_right);
    friend ostream& operator<<(ostream &r_os,const HisFidInfo &r_obj)
    {
      r_os<<r_obj.m_fid<<","
          <<r_obj.m_beginTime<<","
          <<r_obj.m_endTime;
      return r_os;
    }
};

//定义过滤信息指针比较方法类,<
class CmpPFilterInfo
{
	public:
		virtual bool operator() (const FilterInfo *r_left, const FilterInfo *r_right) const;
};
//定义过滤信息指针比较方法类,<
class CmpPHisFidInfo_Time
{
	public:
		virtual bool operator() (const HisFidInfo *r_left, const HisFidInfo *r_right) const;
};

#endif /* FILTERINFO_H_HEADER_INCLUDED_B76432A4 */
