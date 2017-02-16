// ##########################################
// Source file : FixedFieldExtractRule.cpp
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

#include "FixedFieldExtractRule.h"
#include "PreProcApplication.h"
#include "Baf.h"
#include "base/StringUtil.h"
#include "RepeatableFieldInfo.h"
#include "Pp_Exception.h"

extern Baf g_theBaf;        //处理nortel等话单中的module

//##ModelId=485217F101D4
bool FixedFieldExtractRule::initialize(vector<string> *argname
		    							, FieldExtractRule *aParentsField
		    							, const int aFloor
		    							, const bool isRepeatable
		    							, RepeatableFieldInfo *pCompsiteFieldInfo)
{
    // example:  msisdn               FIX,29,0,96,TBCD,F
	vector<string> strResults;
	
	// 1.剥离字段名和字段数据类型的配置
	FieldExtractRule::parseConfigRule(*(argname->begin()),strResults);
	
	// 2.得到字段数据类型的子配置项	
  if(strResults.size()< 5)
  {
  	throw Pp_Exception (__FILE__, __LINE__,"解析配置记录 %s 错!",argname->begin()->c_str());		
  }	

	m_StartByte_  = atoi(strResults[1].c_str());
	m_StartBit_   = atoi(strResults[2].c_str());
	m_BitLength_  = atoi(strResults[3].c_str());
	m_dataType    = strResults[4];
  
	if(strResults.size()>5)
	{
		FieldExtractRule::getDataTypeRule(strResults[5]);
	}
	if(strResults.size()>6)
	{
		// nortel交换机,标识附加模块在话单中出现的位置
		m_position = atoi(strResults[6].c_str());	
	}
	else
	{
		m_position = 0;
	}
	
	getFieldInfoByName(isRepeatable,pCompsiteFieldInfo);
	
	return true;
}

//##ModelId=48521800033C
bool FixedFieldExtractRule::extract(const unsigned char *buffer
				    						, int length
				    						, RecordFormat &aRecordFormat)
{
    int filedLength, real_StartByte, real_len;

    // Nortel Module contents
    real_StartByte = m_StartByte_;
    if((m_FieldName_.compare(0, 4, "NORT") == 0))
    {
        if (m_StartByte_ <= (length - 2))
        {
            // begin indefinite module
            real_StartByte = g_theBaf.getLenth(buffer, length, m_FieldName_, m_StartByte_, m_position, m_BitLength_);
            if(real_StartByte == 0)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    filedLength = m_BitLength_ / 8;
    
    if (m_BitLength_ % 8 != 0)
    {
        filedLength++;
        if(filedLength <= 0)
        {
             theErrorMessages->insert(PP_ERROR_CDR_SIZE_ERROR, "文件" +
                                      m_PpLog_->getSrcFileName() + " 字段长度错误!");
             return false;
        }
        m_pFieldInfo->setSrcValue(buffer + real_StartByte, filedLength);

        // 对单字节中的长度不足一个字节的字段提取
        if( filedLength == 1 )
        {
        	  //update by dzk 2011-02-22
        	  unsigned char tmp[10];
        	  memset(tmp,0,sizeof(tmp));
        	  memcpy(tmp,m_pFieldInfo->m_FieldSrcValue,filedLength);
        	  tmp[0] = tmp[0] << m_StartBit_;
        	  tmp[0] = tmp[0] >> 8-m_BitLength_;
        	  m_pFieldInfo->setSrcValue(tmp, filedLength);
        	  
            //m_pFieldInfo->m_FieldSrcValue[0] = m_pFieldInfo->m_FieldSrcValue[0] << m_StartBit_;
            //m_pFieldInfo->m_FieldSrcValue[0] = m_pFieldInfo->m_FieldSrcValue[0] >> 8 - m_BitLength_;
        }
        else
        {
            m_pFieldInfo->m_FieldSrcValue[filedLength - 1] = (m_pFieldInfo->m_FieldSrcValue[filedLength - 1] | 0x0f);
        }
    }
    else
    {
        if(m_BitLength_ == 0)                           // 此方法用于对变长字段的子串长度的定义
        {
            filedLength = length - real_StartByte;      // 字段长度＝原长度－子串的开始长度
        }

        // 当传入的buffer长度大于0时,才对m_FieldSrcValue赋值
        // 字段开始位不能越出字串总长度(length > real_StartByte), by zaiww 20090811
        if(filedLength > 0 && length > 0 && length > real_StartByte)
        {
            //取实际长度(real_len)和配置长度(filedLength)中较小的一个，保证cpy过去的不越界，by zaiww 20090811
            real_len = length - real_StartByte;
            m_pFieldInfo->setSrcValue(buffer + real_StartByte, ( (filedLength <= real_len) ? filedLength : real_len ) );
        }
        else
        {
            filedLength = 0;
        }
    }

    return true;
}

