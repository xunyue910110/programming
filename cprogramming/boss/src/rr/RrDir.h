/**
*   排重目录文件管理类.
*   @author 高将飞 <gaojf@lianchuang.com>
*           韩愈   <hany@lianchuang.com>
*   @version 1.0
*   @date 2008-08-08
*/
#ifndef _RR_DIR_H_INCLUDE_20080808_
#define _RR_DIR_H_INCLUDE_20080808_
#include "base/Directory.h"

class RrDir : public Directory
{
  public:
    /**
     * 扫描当前目录取得文件列表
     * @param sortBy 使用的排序方法
     * @param level 处理深度
     * @return 0 - OK, other - error
     */
    int scanFiles(const vector<string> &r_dealedFileList,const SortFlags sortby= SF_NONE,const int level = 1);   
    /**
     * 从列表中取文件
     * @param shortname 返回的文件名
     * @param len 最大文件名长度
     * @param fileTime 文件时间
     * @return = 0 无文件, >0 得到一个文件, <0 发生错误
     * add by gaojf@lianchuang.com 2008-8-8 10:42
    */
    int getFile(char *r_shortname, const int r_len ,time_t& r_fileTime);
    
    void  setDealFlag(); //将当前处理的文件打上已处理标记
    void  rewind();      //是下一个处理的文件为第一个未处理文件
    
  protected:
    size_t        m_firstNoDealPos; //第一个未处理的下标 
    size_t        m_curFilePos;     //当前正在处理的文件下标
    size_t        m_curFileNum;     //该次扫描到的文件数
    vector<bool>  m_dealFlagList;   //是否已处理标记:false 未处理、true已处理
};
#endif //_RR_DIR_H_INCLUDE_20080808_
