
// ##########################################
// Source file : FieldConvertRule_64.cpp
// System      : Mobile Billing System
// Version     : 0.1.6
// Language        : ANSI C++
// OS Platform : HPUX
// Authors     : li zhao
// E_mail      : lizhao@lianchuang.com
// Create      : 20080903
// Update      : 20080925
// Copyright(C): 2008 by li zhao, Linkage.
// ##########################################

//############################################### 
//# 规则名称：rule_62                            #
//# 功能：用于OCS，将OCS账目项字段各项拆开*10，累计成ALL_FEE，如：
//#     RULE_64:TariffInfo
//# 或继续将改变的费用项字段各项拆开*10，累计成USE_FEE
//#     RULE_64:TariffInfo;Balanceinfo
//###############################################

#include <vector>
#include <string.h>
#include "FieldConvertRule_64.h"
#include "FieldInfos.h"

//##ModelId=485614B302CE
bool FieldConvertRule_64::initialize(string ruleText)
{
   vector<string> tmpStrs,tmpStrs1;
        int len,ndx;
        char cTmp[100];

      //splitStrByToken(ruleText,";",tmpStrs);
      StringUtil::split(ruleText,";",tmpStrs);

        if( tmpStrs.size() < 2)
        {
                //小于1个分号的,不符合规则的书写格式
                return false;
        }

                // 提取规则中的信息
            tmpStrs[0] = tmpStrs[0].substr(8);
            if (tmpStrs[0].compare(0,5,"FIELD") == 0)
            {
                m_callType = "ALL";
                m_conditionFieldName = "ALL";
            }
            else if (tmpStrs[0].compare(0,9,"CALL_TYPE") == 0)
            {
                       // splitStrByToken(tmpStrs[0],"=",tmpStrs1);
       tmpStrs1.clear();StringUtil::split(tmpStrs[0],"=",tmpStrs1);

                m_callType = tmpStrs1[1];
                m_conditionFieldName = "ALL";
            }
            else
            {
                 // 得到条件中的运算符
                 
            strcpy(cTmp,tmpStrs[0].c_str());
            if(strstr(cTmp,"!=")!=NULL) 
            {
                m_conditionExp = "!=";
            } 
            else if(strstr(cTmp,">=")!=NULL) 
            {
                m_conditionExp = ">=";
            } 
            else if(strstr(cTmp,"<=")!= NULL)
            {
                m_conditionExp = "<=";
            }
            else if(strstr(cTmp,"=") != NULL)
            {
                m_conditionExp = "=";
            }
            else 
            {
                return false;
            }
            // 根据前面求出的运算符,分割条件字段,取得运算符左右键值
                           //  splitStrByToken(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);
            tmpStrs1.clear();StringUtil::split(tmpStrs[0],m_conditionExp.c_str(),tmpStrs1);

            if(tmpStrs1.size()!=2)
            {
                    return false;       
            }
            else
            {
              m_conditionFieldName = tmpStrs1[0];
                    m_conditionFieldValue = tmpStrs1[1];
                    m_callType = "ALL";
            }   
            }
        
        m_acctField = tmpStrs[1];
        //租费扣费的单，累帐应该采用实扣字段(改变的费用项字段)累帐，不能使用应扣字段(账目项)累帐；
        ismon=false;
        if(tmpStrs.size()>=3)
        {
                ismon = true;
                m_useField = tmpStrs[2];
        }
        
        return true;
}

//##ModelId=485614C00213
bool FieldConvertRule_64::convertField()
{
    int isize,all_fee,fee,use_fee;
        static string theCallType("CALL_TYPE"),allFee("ALL_FEE"),useFee("USE_FEE");
        FieldInfo *acctFI,*useFI,*callType,*tmpFieldInfo;
        string tmpstr,accountstr,t1;
        vector<string>  tmpStrs1,tmpStrs2;
        char tmpchar[20];
        
       // if ( (acctFI=getFieldMap(theFieldInfoMaps,m_acctField)) == 0 )
        if ( (acctFI=m_pFieldInfos->getFieldInfoByName(m_acctField.c_str())) == 0 )
        	
        {
            return false;
        }

        //if ( (callType=getFieldMap(theFieldInfoMaps,theCallType)) == 0 )
         if ( (callType=m_pFieldInfos->getFieldInfoByName(theCallType.c_str())) == 0 )
       	
        {
            return false;
        }
        
        if( m_callType.compare("ALL")!=0 )            //呼叫类型不为ALL
        {     t1=callType->m_FieldDesValue;
            if ( t1.compare(m_callType)!=0 )    //不符合呼叫类型条件
            {
                return false;
            }
        }
        
        if(ismon)
        {
               // if ( (useFI=getFieldMap(theFieldInfoMaps,m_useField)) == 0 )
               if ( (useFI=m_pFieldInfos->getFieldInfoByName(m_useField.c_str())) == 0 )

                {
                    return false;
                }
                //累计实扣费用，格式： 
                // BalanceID01:BalanceType01,Delta01,Balance01;BalanceID02:BalanceType02,Delta02,Balance02;

                      //  splitStrByToken(useFI->m_FieldDesValue,";",tmpStrs1);
       tmpStrs1.clear();StringUtil::split(useFI->m_FieldDesValue,";",tmpStrs1);
                
                isize = tmpStrs1.size();
                use_fee=0;
                for(int i=0;i<isize;i++)
                {
                       // splitStrByToken(tmpStrs1[i].c_str(),",",tmpStrs2);
       tmpStrs2.clear();StringUtil::split(tmpStrs1[i].c_str(),",",tmpStrs2);
                        
                        if(tmpStrs2.size()>=2)
                        {
                                fee = atoi(tmpStrs2[1].c_str())*10;
                                use_fee += fee;
                        }
                }
                
                memset(tmpchar,0,sizeof(tmpchar));
                sprintf(tmpchar,"%d",use_fee);
                           //if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,useFee))==0 ) 
               if ( (tmpFieldInfo=m_pFieldInfos->getFieldInfoByName(useFee.c_str())) == 0 )
              	
          {
              tmpFieldInfo = new FieldInfo(0);
                                tmpFieldInfo->m_FieldName_      = useFee;
                                theFieldInfoMaps.insert( map<string,int>::value_type(useFee.c_str(),tmpFieldInfo) );
                }
                
                tmpFieldInfo->m_FieldDesValue=tmpchar;
        }

        tmpStrs1.clear();
        tmpStrs2.clear();
                         //splitStrByToken(acctFI->m_FieldDesValue,";",tmpStrs1);
        tmpStrs1.clear();StringUtil::split(acctFI->m_FieldDesValue,";",tmpStrs1);
       
        isize = tmpStrs1.size();
        all_fee=0;
        accountstr="";
        for(int i=0;i<isize;i++)
        {
                                // splitStrByToken(tmpStrs1[i].c_str(),":",tmpStrs2);
                tmpStrs2.clear();StringUtil::split(tmpStrs1[i].c_str(),":",tmpStrs2);

                if(tmpStrs2.size()==2)
                {
                        //OCS会将免费资源放到账目项（900000以上），比如：941043 国内点对点短信条数，现需要剔除，不累帐
                        if(atoi(tmpStrs2[0].c_str())>900000)
                                continue;
                        
                        fee = atoi(tmpStrs2[1].c_str())*10;
                        all_fee += fee;
                        memset(tmpchar,0,sizeof(tmpchar));
                        
                        //若是月租类型的，直接置实扣值
                        if(ismon && isize<2)
                                sprintf(tmpchar,"%d",use_fee);
                        else
                                sprintf(tmpchar,"%d",fee);
                        accountstr=accountstr+tmpStrs2[0]+":"+tmpchar+";";
                }
        }
        
        acctFI->m_FieldDesValue=accountstr;
                
        memset(tmpchar,0,sizeof(tmpchar));
        sprintf(tmpchar,"%d",all_fee);
        //if( (tmpFieldInfo=getFieldMap(theFieldInfoMaps,allFee))==0 ) 
        if ( (tmpFieldInfo=m_pFieldInfos->getFieldInfoByName(allFee.c_str())) == 0 )
      	
  {
      tmpFieldInfo = new FieldInfo(0);
                        tmpFieldInfo->m_FieldName_      = allFee;
                        theFieldInfoMaps.insert( map<string,int>::value_type(allFee.c_str(),tmpFieldInfo) );
        }
        
        tmpFieldInfo->m_FieldDesValue=tmpchar;
        
        return true;
}

//##ModelId=485614D20157
FieldConvertRule_64::~FieldConvertRule_64()
{
}
