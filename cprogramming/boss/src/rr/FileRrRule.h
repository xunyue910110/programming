#ifndef FILERRRULE_H_HEADER_INCLUDED_B7642AD1
#define FILERRRULE_H_HEADER_INCLUDED_B7642AD1
#include "base/PString.h"
#include <list>
class AbstractFieldSelector;

class FileRrRule
{
  public:
    PString m_filePrefix;  //文件名前缀:用于判断文件所采用的排重规则
    size_t  m_maxFidLen;   //过滤信息长度
    PString m_fileType;    //文件类型: 过滤信息文件的文件类型（文件名前缀）
    int     m_rrType;      //排重类型：-1 不排重、0 完全排重、1 完全排重＋包容排重、2 文件级排重
    char    m_delimiter;

    string m_fidSelectRule; //过滤信息提取规则
    AbstractFieldSelector* m_fidSelect; //过滤信息提取规则

    string m_beginTimeRule;  //起始时间读取规则
    AbstractFieldSelector* m_beginTimeSelect; //起始时间读取类

    string m_endTimeRule;    //终止时间读取规则
    AbstractFieldSelector* m_endTimeSelect; //终止时间读取类

    string m_rrFlagSelectRule; //排重标记读取规则
    AbstractFieldSelector* m_rrFlagSelect; //排重标记读取类
    
    int  m_fileType_merge; //为合并文件类型，读取配置后内部生成的信息
    int  m_rrFlagPos;      //排重标记位置，第几个字段
    
    size_t  m_timeDuration;//最大时间跨度
    
    static list<AbstractFieldSelector*> m_selectors;
  public:
    FileRrRule();
    virtual ~FileRrRule();
    FileRrRule(const FileRrRule &r_right);
    FileRrRule& operator=(const FileRrRule &r_right);
    friend ostream& operator<<(ostream &r_os,const FileRrRule &r_rule);
    friend bool operator<(const FileRrRule &r_left,const FileRrRule &r_right);
    friend bool operator==(const FileRrRule &r_left,const FileRrRule &r_right);
    //
    bool setRrFlagPos();
    //根据指定规则，将话单中指定字段值更改为1
    bool updateRrFlagValue(char* r_cdrStr) const;
    
    static void clear();
};



#endif /* FILERRRULE_H_HEADER_INCLUDED_B7642AD1 */
