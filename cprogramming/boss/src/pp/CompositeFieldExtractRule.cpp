// ##########################################
// Source file : CompositeFieldExtractRule.cpp
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

#include "CompositeFieldExtractRule.h"
#include "FieldInfos.h"
#include "RepeatableFieldInfo.h"
#include <stdlib.h>

//##ModelId=4851CB6502FD
CompositeFieldExtractRule::CompositeFieldExtractRule(int iSubNumber):FieldExtractRule(iSubNumber)
{
    m_theLayer_             = 0;
    m_CompositeFieldSelf_   = 0;
    m_myToken               = 0;
    
    m_pMySelfFieldInfo = new RepeatableFieldInfo();
}

//##ModelId=4851CB6C004E
CompositeFieldExtractRule::~CompositeFieldExtractRule()
{
    for ( int i = 0; i < m_theChildField_.size(); i++ )
    {
        delete m_theChildField_[i];
    }
    m_theChildField_.clear();
    delete m_CompositeFieldSelf_;
    
    if(m_pMySelfFieldInfo!=NULL)
    {
    	delete m_pMySelfFieldInfo;
    }
}

//##ModelId=4851CB8A0399
bool CompositeFieldExtractRule::initialize(vector<string> *argname
			    							, FieldExtractRule *aParentsField
			    							, const int aFloor
			    							, const bool isRepeatable
			    							, RepeatableFieldInfo *pCompsiteFieldInfo)
{
    FieldExtractRule *pFieldExtRuleTmp, *pFieldExtRuleTmp1, *pCompositeFieldSelf;
    vector<string> theArgname, selfArgname;
    string x, aParantsName, endString, theFieldType, sectionName;
    char *t_tmp, *t_tmp1;
    bool isRepeatableLocal = false;

    aParantsName = aParentsField->m_SectionName_;
    while (argname->size() > 0)
    {
        pFieldExtRuleTmp = 0;
        pCompositeFieldSelf = 0;
        theArgname.clear();
        selfArgname.clear();
        x = *(argname->begin());

        argname->erase(argname->begin());               //删除已处理的配置信息
        if (x.compare(0, 1, "<") == 0)                  //得到section名称,复合字段的情况
        {
            sectionName = x.substr(1, x.size() - 2);
            getFieldType(sectionName, theFieldType);

            if(theFieldType.empty() && sectionName.compare("ALL") != 0)
            {
               theFieldType = x;
            }
            createChildFieldObject(pCompositeFieldSelf, theFieldType, isRepeatableLocal);
            if (pCompositeFieldSelf != 0)
            {
                selfArgname.push_back(x.substr(1, x.size() - 2));
                pCompositeFieldSelf->initialize(&selfArgname
                									, pCompositeFieldSelf
                									, m_theLayer_
                									, false
                									, m_pMySelfFieldInfo);
                pCompositeFieldSelf->m_token = m_myToken;
            }
            getSectionName(sectionName);                // 删除sectionName中的空格和TAB字符
            theFieldType = x;
        }
        else
        {
            sectionName = getFieldType(x, theFieldType);  //得到字段提取类型
            theArgname.push_back(x);                    //theArgname存放的是配置文件该段的一行
        }
        endString = string("</") + aParantsName + string(">");
        if (x.compare(endString) == 0)                  // 是结束section
        {
            break;
        }
        // 跳过不能生成对象的配置信息
        if (createChildFieldObject(pFieldExtRuleTmp, theFieldType, isRepeatableLocal) == 0)
        {
            continue;
        }

        pFieldExtRuleTmp->m_theLayer_ = aParentsField->m_theLayer_ + 1;                  // 得到当前层数

        // 打印调试信息
    #ifdef _DEBUG_
	    char Tmp[30];
        m_PpLog_->ShowMessage(sectionName + ", new a Field object,", 0);
        sprintf(Tmp, "%d", pFieldExtRuleTmp->m_theLayer_);
        m_PpLog_->ShowMessage(" the layer is " + string(Tmp), 0);
        m_PpLog_->ShowMessage(". Parent is " + aParantsName + ", the parent layer is ", 0);
        sprintf(Tmp, "%d", aParentsField->m_theLayer_);
        m_PpLog_->ShowMessage(string(Tmp) + "\n", 0);
	#endif
        if (pFieldExtRuleTmp != 0)
        {
            m_theChildField_.push_back(pFieldExtRuleTmp);                                // 保存子对象指针
            pFieldExtRuleTmp->m_SectionName_     = sectionName;
            pFieldExtRuleTmp->m_theParentsField_ = aParentsField;
            pFieldExtRuleTmp1 = pFieldExtRuleTmp;
            CompositeFieldExtractRule *pCompositeFieldExtractRule = (CompositeFieldExtractRule*)pFieldExtRuleTmp1;
            if (pCompositeFieldSelf != 0)
            {                                                                           // 如果是CompositeField对象
                char cTmps[2];
                memset(cTmps, 0, 2);
 
                cTmps[0] = m_myToken;
                string strToken = cTmps;
                pCompositeFieldExtractRule->setToken(strToken);
                pCompositeFieldExtractRule->initialize(argname
                											, pFieldExtRuleTmp
                											, m_theLayer_
                											, isRepeatableLocal|isRepeatable);                  // 是组合对象
                pCompositeFieldExtractRule->m_CompositeFieldSelf_ = pCompositeFieldSelf;
                pCompositeFieldExtractRule->m_token = m_myToken;
            }
            else
            {
                pFieldExtRuleTmp->initialize(&theArgname
                									, pFieldExtRuleTmp
                									, m_theLayer_
                									, isRepeatable); //是一般对象
                pFieldExtRuleTmp->m_token = m_myToken;
            }
        }
    }
    return true;
}

//##ModelId=4851CBBB01D4
bool CompositeFieldExtractRule::extract(const unsigned char *buffer
				    						, int length
				    						, RecordFormat &aRecordFormat)
{
    int i,len;
    FieldExtractRule *pFieldExtRuleTmp;
    FieldInfo *tmpFiedInfo;

    len = m_theChildField_.size();
    for (i = 0; i < len; i++)
    {
        pFieldExtRuleTmp = m_theChildField_[i];

        if ((pFieldExtRuleTmp->getFieldName()).compare(SUB_MODULE) == 0)
        {
            static string theCdrSign;
            this->getCdrSign(buffer,theCdrSign,aRecordFormat);
            pFieldExtRuleTmp = 0;

            for ( int j = i; j < len; ++j )
            {
                if ( (m_theChildField_[j]->getSectionName()).compare(theCdrSign) == 0 )
                {
                    pFieldExtRuleTmp = m_theChildField_[j];
                    break;
                }
            }

            if( pFieldExtRuleTmp == 0 )
            {
                break;
            }
            else
            {
                CompositeFieldExtractRule *pCompositeFieldExtractRule = (CompositeFieldExtractRule *)pFieldExtRuleTmp;
                pCompositeFieldExtractRule->extract(buffer, length, aRecordFormat);
                // 创建名称为"Sub_Module"的字段信息
                
                tmpFiedInfo = m_pFieldInfos->getFieldInfoByName(SUB_MODULE);
                tmpFiedInfo->setDesValue(theCdrSign);
                break;
            }
        }

        CompositeFieldExtractRule *pCompositeFieldExtractRule = (CompositeFieldExtractRule *)pFieldExtRuleTmp;

        // 修改buffer为已求得的字段原始值(在是复合字段的情况下)
        if(pFieldExtRuleTmp->getMySelf() != 0)
        {
            // 将复合字段自己的内容提取出来,这里的m_CompositeFieldSelf_不可能是复合字段了
            m_pMySelfFieldInfo->reset();
            (pCompositeFieldExtractRule->m_CompositeFieldSelf_)->extract(buffer
                                                                            , length
                                                                            , aRecordFormat);

            if(m_pMySelfFieldInfo->getNumCount() == 0)
            {
                continue;
            }
 
            // 对于gprs话单中的Qos链表式的话单格式,在GasnFieldExtractRule类中,把链表
            // 结构的内容平面化,链表的值存放在'fieldMapTmp'中返回
            for(int iFieldCount=0;iFieldCount<m_pMySelfFieldInfo->m_fieldInfoVect.size(); ++iFieldCount)
            {
                // 这里FieldExtRuleTmp有可能是复合字段指针,从而实现递归调用
                pFieldExtRuleTmp->extract(m_pMySelfFieldInfo->m_fieldInfoVect[iFieldCount]->m_FieldSrcValue
                								, m_pMySelfFieldInfo->m_fieldInfoVect[iFieldCount]->m_iSrcValueLen
                								, aRecordFormat);
            }
        }
        else
        {
            pFieldExtRuleTmp->extract(buffer, length, aRecordFormat);
        }
    }

    return true;
}

//##ModelId=4851CC3301A5
bool CompositeFieldExtractRule::setToken(string token)
{
    char tmp[10];

    if(token.size() <= 0)
    {
       return true;
    }

    strcpy(tmp, token.c_str());
    if(token.compare("\\t") == 0)
    {
       m_myToken = '\t';
    }
    else
    {
       m_myToken = tmp[0];
    }

    return true;
}

//##ModelId=4851CC7B0109
void CompositeFieldExtractRule::getCdrSign(const unsigned char *srcCdrValue, string &theSign, RecordFormat &aRecordFormat)
{
    char signTmp1[10];
    unsigned char signTmp[10];
    int tagSize;

    theSign = "";
    AsnFormat* pAsnFormat  = (AsnFormat*) (&aRecordFormat);
    tagSize = pAsnFormat->getAsnTag((unsigned char *)srcCdrValue, signTmp);
    for(int i = 0; i < tagSize; i++)
    {
        sprintf(signTmp1, "%02X", signTmp[i]);
        theSign += signTmp1;
    }
}

//##ModelId=4851CCC90167
string CompositeFieldExtractRule::getFieldType(string srcString, string &desString)
{
    int i,len;
    char Tmp[100];

    len = srcString.size();
    strcpy(Tmp, srcString.c_str());
    for (i = 0; i < len; i++)                       //跳过非空格和TAB字符直到空格和TAB字符止
    {
        if (Tmp[i] == ' ' || Tmp[i] == '\t')
        {
            break;
        }
    }
    if (i == len)
    {
        desString.erase();
        return desString;
    }
    srcString = srcString.substr(i,len-i);          //得到空格后的串
    len = srcString.size();
    strcpy(Tmp, srcString.c_str());
    for (i = 0; i < len; i++)                       //跳过空格和TAB字符直到非空格和TAB字符止
    {
        if (Tmp[i] != ' ' && Tmp[i] != '\t')
        {
            break;
        }
    }
    if (i == len)
    {
        desString.erase();
        return desString;
    }
    srcString = srcString.substr(i, len - i);       //得到空格后的串
    strcpy(Tmp, srcString.c_str());
    desString = string(strtok(Tmp, ","));           //得到逗号前的串

    return desString;
}

//##ModelId=4851CCF4004E
FieldExtractRule *CompositeFieldExtractRule::createChildFieldObject(FieldExtractRule *& aChildField
																		, const string aFieldType
																		, bool &isRepeatableLocal)
{
    if (aFieldType.compare(0, 1, "<") == 0 && aFieldType.compare(1, 1, "/") != 0)           //创建组合字段对象
    {
        aChildField = new CompositeFieldExtractRule(m_iSubChannelNum);
    }
    else if (aFieldType.compare(FIX) == 0)            //创建定长处理字段对象
    {
        aChildField = new FixedFieldExtractRule(m_iSubChannelNum);
    }
    else if (aFieldType.compare(TOKEN) == 0)          //创建分隔处理字段对象
    {
        aChildField = new TokenFieldExtractRule(m_iSubChannelNum);
    }
    else if (aFieldType.compare(ASN) == 0)            //创建ADN变长处理字段对象
    {
        aChildField = new AsnFieldExtractRule(m_iSubChannelNum);
    }
    else if (aFieldType.compare(SASN) == 0)           //创建SASN变长处理字段对象
    {
        aChildField = new SasnFieldExtractRule(m_iSubChannelNum);
    }
    else if (aFieldType.compare(GASN) == 0)           //创建GASN变长处理字段对象
    {
        aChildField = new GasnFieldExtractRule(m_iSubChannelNum);
        isRepeatableLocal = true;
    }
    else
    {
        aChildField = 0;
    }
    return aChildField;
}

//##ModelId=4851CD2F006D
void CompositeFieldExtractRule::getSectionName(string &aSectionName)
{
    int i,len;
    char Tmp[100];

    len = aSectionName.size();
    strcpy(Tmp, aSectionName.c_str());
    for (i = 0; i < len; i++)                               //跳过非空格和TAB字符直到空格和TAB字符止
    {
        if (Tmp[i] == ' ' || Tmp[i] == '\t')
        {
            break;
        }
    }
    aSectionName = aSectionName.substr(0, i);               //得到空格前的串
}

