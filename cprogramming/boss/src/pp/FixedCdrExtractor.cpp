// ##########################################
// Source file : FixedCdrExtractor.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "FixedCdrExtractor.h"

//##ModelId=48522DC70000
int FixedCdrExtractor::getNextSrcCdr(unsigned char *&srcCdrBuffer)
{
    int cdrSize;
    int len = 0;

    //CDMA nortel话单文件多条定长cdr的特殊性
    char Buffer[10], tmp1[10], flag[10];
    string::size_type pos;

    if (m_CdrSize.size() == 1)
    {
        if(atoi(m_CdrSize[0].c_str()) == 0)        //cdr_size FIX,0,0,16,binary,1
        {
           cdrSize = getcdrSize(m_BlockOffset + m_BlockBuffer);
        }
        else
        {
           cdrSize = atoi(m_CdrSize[0].c_str());    //cdr_size 2048
        }
    }

    if (m_CdrSize.size() > 1)                       //多种有标识符的定长cdr记录的处理
    {

        Buffer[0] = 0;
        tmp1[0]   = 0;

        vector<string>::iterator iter;
        for (iter = m_CdrSize.begin(); iter != m_CdrSize.end(); iter++)
        {
            pos = (*iter).find(':');

            len = pos / 2;

            memcpy(Buffer, m_BlockOffset + m_BlockBuffer, len);

            char s[] ={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

            for (int i = 0; i < len; i++)
                {
                    flag[i * 2]     = s[(Buffer[i]&0xf0)>>4];
                    flag[i * 2 + 1] =s[Buffer[i]&0x0f];
                }
                flag[(len - 1) * 2 + 2] = 0;

            if( (*iter).compare(0, pos, flag) == 0)
            {
                cdrSize = atoi(((*iter).substr(pos + 1, (*iter).size() - pos - 1)).c_str());
                break;
            }
        }
        // 找不到cdr flag 表示读完了一块
        if ( iter == m_CdrSize.end() )
        {
            return 0;
        }
    }

    if ((m_RealBlockLength - m_BlockOffset) < cdrSize)
    {
        return 0;                       // 返回"0"表示本块内的记录已经取完
    }
    else if (cdrSize <= 0)
    {
        return 0;                       // 返回"0"表示本块内的记录已经取完
    }
    
    if(cdrSize > m_iCdrBufferSize)
    {
		if( srcCdrBuffer != NULL)
			free(srcCdrBuffer);
		srcCdrBuffer = (unsigned char *)malloc(cdrSize);
		m_iCdrBufferSize = cdrSize;
    }
    
    memcpy(srcCdrBuffer, m_BlockOffset + m_BlockBuffer, cdrSize);
    m_BlockOffset += cdrSize;

    return cdrSize;                     // 返回原始记录长度

}

//##ModelId=48522DCE003E
bool FixedCdrExtractor::initialize(const string &theCdrParameter, const int theBlockHeaderSize)
{
    // 参数theCdrParameter有三种可能：1. cdr长度; 2. 长度表达式(例："FIX,0,0,16,binary,1") 3: 多模块无序定长(例："F7:290|FA:6|FB:6|FC:6|FD:6|FE:10")
	// 第一种是定长，第二、三种是变长，实际长度从原始话单文件中读取，读取方法如表达式指定
	char *tok;
    char tmp[80];
    vector<string> vectortmp;
    vector<string>::iterator iter;
    //CDMA nortel话单文件多条定长cdr的特殊性
    int size;
    string::size_type pos;

    m_BlockHeaderSize   = theBlockHeaderSize;

    m_CdrSize.clear();
    // CDMA nortel话单文件多条定长cdr的特殊性
    if (isdigit(theCdrParameter[0]))
    {
        size = atoi(theCdrParameter.c_str());
        if (size > 0)
        {
            m_CdrSize.push_back(theCdrParameter);
        }
        else
        {
            theErrorMessages->insert(PP_ERROR_PARAMETER, "cdr size error!");
            return false;
        }
    }
    else if (theCdrParameter.compare(0,3,"FIX") == 0)
    {
        m_CdrSize.push_back("0");
        // example : tmp is "0,0,16,binary,1"
        strcpy(tmp, theCdrParameter.c_str());

        strtok(tmp , ",");
        tok = strtok(NULL, ",");
        if (tok != NULL)
        {
            m_StartByte = atoi(tok);                            // 得到开始字节
        }
        tok = strtok(NULL, ",");                                // 跳过开始比特
        tok = strtok(NULL, ",");
        if (tok != NULL)
        {
            m_Length = atoi(tok) / 8;                           // 得到字段长度
        }
        tok = strtok(NULL, ",");
        if (tok != NULL)
        {
            m_dataType = tok;                                   // 得到数据类型
            if(m_dataType.substr(1, 3) == "BCD")
            {
                tok = strtok(NULL, ",");
                if (tok != NULL && strlen(tok) == 1)
                {
                    m_padding = tok[0];                         // 得到BCD填充字符
                }
            }
            else if (m_dataType == "Binary")
            {
                tok = strtok(NULL, ",");
                if(tok != NULL)
                {
                    m_order = atoi(tok);                        // 得到高低位交换标志
                }
            }
        }
    }
    else
    {
        if ((pos = theCdrParameter.find(":")) != string::npos)
        {
            vectortmp.clear();StringUtil::split(theCdrParameter, "|", vectortmp);
            m_CdrSize = vectortmp;
        }
    }

    m_fielInfo.m_padding = m_padding;
    m_fielInfo.m_dataType = m_dataType;
    m_fielInfo.m_order=m_order;

    
    return true;
}

//##ModelId=48522DD9034B
int FixedCdrExtractor::getcdrSize(const unsigned char *srcCdrBuffer)
{
    m_fielInfo.setSrcValue(srcCdrBuffer + m_StartByte,m_Length);
    
    m_DataFormat.getFmtValue(&m_fielInfo);
    
    return atoi(m_fielInfo.m_FieldDesValue==NULL?"0":m_fielInfo.m_FieldDesValue);

}


