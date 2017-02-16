// ##########################################
// Source file : CompositeFieldExtractRule.cpp
// System      : Mobile Billing System
// Version     : 0.1.52
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010813
// Update      : 20020607
// Copyright(C): 2002 by Yang Zhang, Linkage.
// ##########################################

#include <stdlib.h>

#include "CompositeFieldExtractRule.h"

//##ModelId=3B7B67FB01A7
CompositeFieldExtractRule::CompositeFieldExtractRule():FieldExtractRule()
{
	m_theLayer_				= 0;
	m_CompositeFieldSelf_	= 0;
	m_myToken               = 0;
	m_SendLog_				= SendLog::getInstance();
}

//##ModelId=3B7887F9005A
CompositeFieldExtractRule::~CompositeFieldExtractRule()
{
	for ( int i=0 ; i<m_theChildField_.size()  ; i++ )
	{
		delete m_theChildField_[i];
	}
	m_theChildField_.clear();
	delete m_CompositeFieldSelf_;
}

//##ModelId=3B7887F8003A
bool CompositeFieldExtractRule::initialize(vector<string> *argname, FieldExtractRule *aParentsField, const int aFloor)
{
	char Tmp[30];
	FieldExtractRule *FieldExtRuleTmp,*FieldExtRuleTmp1,*CompositeFieldTmp;
	vector<string> theArgname,selfArgname;
	string x,aParantsName,endString,theFieldType,sectionName;

	aParantsName = aParentsField->m_SectionName_;
	while (argname->size()>0)
	{
		FieldExtRuleTmp = 0;
		CompositeFieldTmp = 0;
		theArgname.clear();
		selfArgname.clear();
		x 		 = *( argname->begin() );

		argname->erase(argname->begin());			//删除已处理的配置信息
		if ( x.compare(0,1,"<")==0 ) 				//得到section名称
		{
			sectionName	= x.substr( 1 , x.size() - 2 );
			getFieldType(sectionName,theFieldType);

			if( theFieldType.empty() && sectionName.compare("ALL") != 0 ) 
			{   
			   theFieldType=x;                                                
			}                                                                 
			createChildFieldObject(CompositeFieldTmp,theFieldType);
			if ( CompositeFieldTmp != 0 ) 
			{
				selfArgname.push_back( x.substr( 1 , x.size() - 2 ) );
				CompositeFieldTmp->initialize(&selfArgname,CompositeFieldTmp,m_theLayer_);
				CompositeFieldTmp->m_token = m_myToken;
			}
            getSectionName(sectionName);           // 删除sectionName中的空格和TAB字符
			theFieldType	= x;
		} 
		else 
		{
			sectionName	= getFieldType(x,theFieldType);
			theArgname.push_back(x);
		}
		endString	= string("</") + aParantsName + string(">");
		if ( x.compare(endString)==0 )		//是结束section
			break;
		// 跳过不能生成对象的配置信息
		if ( createChildFieldObject(FieldExtRuleTmp,theFieldType) == 0 ) 
		{
			continue;
		}

		FieldExtRuleTmp->m_theLayer_ = aParentsField->m_theLayer_ + 1;			// 得到当前层数

		// 打印调试信息
		m_SendLog_->ShowMessage(sectionName+", new a Field object,",0);
		sprintf(Tmp,"%d",FieldExtRuleTmp->m_theLayer_);
		m_SendLog_->ShowMessage( " the layer is "+string(Tmp),0 );
		m_SendLog_->ShowMessage( ". Parent is "+aParantsName+", the parent layer is ",0 );
		sprintf(Tmp,"%d",aParentsField->m_theLayer_);
		m_SendLog_->ShowMessage( string(Tmp)+"\n",0 );

		if ( FieldExtRuleTmp != 0 ) 
		{
			m_theChildField_.push_back(FieldExtRuleTmp);						// 保存子对象指针
			FieldExtRuleTmp->m_SectionName_		= sectionName;
			FieldExtRuleTmp->m_theParentsField_	= aParentsField;
            FieldExtRuleTmp1 = FieldExtRuleTmp;
 			CompositeFieldExtractRule *cp = (CompositeFieldExtractRule*)FieldExtRuleTmp1;
    		if (CompositeFieldTmp != 0) 
    		{		                                                    // 如果是CompositeField对象
				cp->initialize(argname,FieldExtRuleTmp,m_theLayer_);		    // 是组合对象
				cp->m_CompositeFieldSelf_ = CompositeFieldTmp;
				cp->m_token = m_myToken;
			} 
			else 
			{
				FieldExtRuleTmp->initialize(&theArgname,FieldExtRuleTmp,m_theLayer_);	//是一般对象
				FieldExtRuleTmp->m_token = m_myToken;
			}
		}
	}
	return true;
}

//##ModelId=3C329E5200CB
bool CompositeFieldExtractRule::setToken(string token)
{
	char tmp[10];
	
	if(token.size()<=0)
	   return true;
	
	strcpy(tmp,token.c_str());
	if(token.compare("\\t")==0) 
	{
	   m_myToken = '\t';
	} 
	else 
	{
	   m_myToken = tmp[0];
	}
	
	return true;
}

//##ModelId=3B7887F80211
bool CompositeFieldExtractRule::extract(const unsigned char *buffer, int length,
					   		 multimap<MyString,FieldInfo *> &afieldInfoMap,RecordFormat &aRecordFormat)
{
    int i,len;
	FieldExtractRule *FieldExtRuleTmp;
	FieldInfo *theFieldInfo;
	multimap<MyString,FieldInfo *> fieldInfoMapTmp;

    len=m_theChildField_.size();
 	for ( i=0 ; i<len ; i++ )
	{
      FieldExtRuleTmp=m_theChildField_[i];

		  fieldInfoMapTmp.clear();
 		  CompositeFieldExtractRule *cp = (CompositeFieldExtractRule *)FieldExtRuleTmp;

 		// 修改buffer为已求得的字段原始值(在是复合字段的情况下)
    	if( FieldExtRuleTmp->getMySelf() != 0 ) 
    	{
			// 将复合字段自己的内容提取出来,这里的m_CompositeFieldSelf_不可能是复合字段了
			(cp->m_CompositeFieldSelf_)->extract(buffer,length,fieldInfoMapTmp,aRecordFormat);

	        //modified by chenmin 02-04-25
			if(fieldInfoMapTmp.size()==0) 
			{
			    continue;
			}
	        //over by chenmin

			// 对于gprs话单中的Qos链表式的话单格式,在GasnFieldExtractRule类中,把链表
			// 结构的内容平面化,链表的值存放在'fieldMapTmp'中返回 modified by chenm 2005-3-12
			multimap<MyString,FieldInfo *>::iterator itr;
			for(itr=fieldInfoMapTmp.begin();itr!=fieldInfoMapTmp.end();++itr )
			{
				theFieldInfo=0;
				theFieldInfo=itr->second;
				
				if(theFieldInfo != 0 ) 
				{
					// 这里FieldExtRuleTmp有可能是复合字段指针,从而实现递归调用
					FieldExtRuleTmp->extract(theFieldInfo->m_FieldSrcValue,
							  theFieldInfo->m_FieldLength,afieldInfoMap,aRecordFormat);	
					delete 	theFieldInfo;	  
				}
			}
		} 
		else 
		{
			FieldExtRuleTmp->extract(buffer,length,afieldInfoMap,aRecordFormat);
		}
	}

	return true;
}

//##ModelId=3B7DD0A902AB
string CompositeFieldExtractRule::getFieldType(string srcString,string &desString)
{
	int i,len;
	char Tmp[100];

	len=srcString.size();
	strcpy(Tmp,srcString.c_str());
	for (i=0;i<len;i++) 			//跳过非空格和TAB字符直到空格和TAB字符止
	{
		if (Tmp[i]==' ' || Tmp[i]=='\t')
			break;
	}
	if (i==len) 
	{
		desString.erase();
		return desString;
	}
	srcString=srcString.substr(i,len-i);	//得到空格前的串

	len=srcString.size();
	strcpy(Tmp,srcString.c_str());
	for (i=0;i<len;i++) 			//跳过空格和TAB字符直到非空格和TAB字符止
	{
		if (Tmp[i]!=' ' && Tmp[i]!='\t')
			break;
	}
	if (i==len) 
	{
		desString.erase();
		return desString;
	}
	srcString=srcString.substr(i,len-i);	//得到空格后的串
	strcpy(Tmp,srcString.c_str());
	desString=string( strtok(Tmp,",") );	//得到逗号前的串

	return desString;
}

//##ModelId=3B7DDF4F0361
FieldExtractRule *CompositeFieldExtractRule::createChildFieldObject(FieldExtractRule *& aChildField,string aFieldType)
{
	if ( aFieldType.compare(0,1,"<")==0 && aFieldType.compare(1,1,"/")!=0 ) 	       //创建组合字段对象,modify by chenm 2002-11-13 19:16
	{
		aChildField = new CompositeFieldExtractRule;
	} 
	else if (aFieldType.compare("TOKEN")==0) 		//创建分隔处理字段对象
	{
		aChildField = new TokenFieldExtractRule;
	} 
	else 
	{
		aChildField = 0;
	}

	return aChildField;
}

//##ModelId=3B8E4DF802C5
void CompositeFieldExtractRule::getSectionName(string &aSectionName)
{
	int i,len;
	char Tmp[100];

	len=aSectionName.size();
	strcpy(Tmp,aSectionName.c_str());
	for (i=0;i<len;i++) 			                 //跳过非空格和TAB字符直到空格和TAB字符止
	{
		if (Tmp[i]==' ' || Tmp[i]=='\t')
			break;
	}
	aSectionName=aSectionName.substr(0,i);	          //得到空格前的串
}


