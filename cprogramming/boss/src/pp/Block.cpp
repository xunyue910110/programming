// ##########################################
// Source file : Block.cpp
// System      : Mobile Billing System
// Version     : NG1.0
// Language    : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#include "Block.h"
//##ModelId=4855BC140148
bool Block::initialize(vector <string> aBlockFields, int aBlockHeadSize, const string theBlockParameter, int &BlockSize)
{
    char *tok;
    char tmp[80];

    m_BlockHeaderSize_ = aBlockHeadSize;

    if(theBlockParameter.compare(0,3,"FIX") == 0)
    {
        BlockSize = 0;
        // example : tmp is "0,0,16,binary,1"
        strcpy(tmp, theBlockParameter.c_str());
        strtok(tmp, ",");
        tok = strtok(NULL, ",");
        if (tok != NULL)
        {
            m_StartByte = atoi(tok);                    //得到开始字节
        }
        tok = strtok(NULL, ",");                        //跳过开始比特
        tok = strtok(NULL, ",");
        if (tok != NULL)
        {
            m_Length = atoi(tok) / 8;                   //得到字段长度
        }
        tok = strtok(NULL, ",");
        if (tok != NULL)
        {
            m_dataType = tok;                           //得到数据类型
            if(m_dataType.substr(1, 3) == "BCD")
            {
                tok = strtok(NULL, ",");
                if (tok != NULL && strlen(tok) == 1)
                {
                    m_padding = tok[0];                 //得到BCD填充字符
                }
            }
            else if (m_dataType == "Binary")
            {
                tok = strtok(NULL, ",");
                if(tok != NULL)
                {
                    m_order = atoi(tok);                //得到高低位交换标志
                }
            }
        }
    }
    else
    {
        BlockSize = atoi(theBlockParameter.c_str());
    }
    
    m_fielInfo.m_padding = m_padding;
    m_fielInfo.m_dataType = m_dataType;
    m_fielInfo.m_order=m_order;

    if ( BlockSize >= 0 )
    {
        return true;
    }
    else
    {
        theErrorMessages->insert(PP_ERROR_PARAMETER, "block size error!");
        return false;
    }
    return true;
}

//##ModelId=4855BC2102CE
void Block::getBlockHeadField()
{
}

//##ModelId=4855BC2D02EE
int Block::getBlockSize(const unsigned char *srcCdrBuffer)
{
    m_fielInfo.setSrcValue(srcCdrBuffer + m_StartByte,m_Length);
    
    m_DataFormat.getFmtValue(&m_fielInfo);
    
    return atoi(m_fielInfo.m_FieldDesValue==NULL?"0":m_fielInfo.m_FieldDesValue);
}

