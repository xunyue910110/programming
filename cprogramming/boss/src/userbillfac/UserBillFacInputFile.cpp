#include "UserBillFacInputFile.h"
#include "appfrm/ErrorHandle.h"

extern ErrorMessages *const theErrorMessages;

UserBillFacInputFile::UserBillFacInputFile()
{
}


UserBillFacInputFile::~UserBillFacInputFile()
{
}


bool UserBillFacInputFile::init(const string& strInputFilePath,
                              const map<string, string>& mapUnifiedPayRelation)
{
	m_fileName = "";
	m_filePath = strInputFilePath;
	m_seperate = ",";
	m_localOrRemoteTag = '0';

	m_fileStream.close();
	m_fileStream.clear();

	m_mapUnifiedPayRelation = mapUnifiedPayRelation;

	return true;
}


bool UserBillFacInputFile::openFile(const string& strFileName,
                                    const char localOrRemoteTag,
                                    AccountLogInfo* pAccountLogInfo)
{
    //初始化
	m_fileName = "";
	m_localOrRemoteTag = '0';
	m_fileStream.close();
	m_fileStream.clear();

	//赋值
	m_fileName = strFileName;

	m_localOrRemoteTag = localOrRemoteTag;

	m_pAccountLogInfo = pAccountLogInfo;

	//打开文件
	m_fileStream.open((m_filePath + m_fileName).c_str(), ios::in);
	if (m_fileStream.rdstate() != ios::goodbit)
	{
		theErrorMessages->insert(ERROR_OPEN_FILE,
                                m_filePath + m_fileName + "--open file error");
        m_fileStream.close();
		return false;
	}
	else
	{
		return true;
	}

}


bool UserBillFacInputFile::getUserBill(UserKey& userKey, vector<UserBill> &vecUserBill)
{
	//初始化输出参数
	userKey.m_id             = '0';
	userKey.m_idType         = '0';
	//userKey.m_userId         = '0';

	vecUserBill.clear();

	//定义局部变量并赋初始值
	char   t_buffer[100+1];

	T_SERVER_ID  t_id;
	char         t_idType;
	T_SERVER_ID  t_userId;
	int          t_detailItemCode;
	long         t_discountFee;
	int          t_discountId;
	char         t_billTypeCode;
	int          t_priority;
	long         t_adjustInsId;
	long         t_effectId;//add by xuf 2010-1-23 10:12:02

	char*  p;
	string t_str;

	bool   IsValidLine = false;
	
	UserBill userBill;
	vector<UserBill>::iterator                itrUserBill;
	
	memset(t_buffer,0,sizeof(t_buffer)); 
	
	//if (m_lastUserId != "")
  	if(m_lastId != "")	
  {
    userKey.m_id             = m_lastId;    
		userKey.m_idType         = m_lastIdType;    
		//userKey.m_userId         = m_lastUserId ;
		   
		userBill.m_userId         = m_lastUserId;
		userBill.m_detailItemCode = m_lastDetailItemCode; 
		userBill.m_billTypeCode   = m_lastBillTypeCode;
		userBill.m_priority       = m_lastPriority;	                         
    userBill.m_discountFee    = m_lastDiscountFee;
    userBill.m_discountId     = m_lastDiscountId;
    userBill.m_adjustInsId    = m_lastAdjustInsId;
    userBill.m_effectId       = m_lastEffectId; //add by xuf 2010-1-23 10:18:21
                                 
        vecUserBill.push_back(userBill);
        
        if (m_fileStream.eof())
        {
            m_lastId               = "" ;
            m_lastIdType           = '0';
            m_lastUserId           = "" ;
            
            m_lastDetailItemCode   =  0 ;
            m_lastBillTypeCode     = '0';
            m_lastPriority         =  0 ;
            m_lastDiscountFee      =  0 ;
            m_lastDiscountId       =  0 ;
            m_lastAdjustInsId      =  0 ;
            m_lastEffectId         =  0 ;//add by xuf 2010-1-23 10:18:28
        }    
  }

  while (!m_fileStream.eof())
  {  
    //获取一行记录信息
    memset(t_buffer,0,sizeof(t_buffer));
    p = NULL;
	  m_fileStream.getline(t_buffer,100);

	  p = t_buffer;

		//获取群组标识
		IsValidLine = false;
		p = strtok( p, m_seperate.c_str() );
		if ( p != NULL )
		{
			 t_id = p;
			 IsValidLine = true;
		}
		//cout << t_id << endl;
		//获取标识类型
		IsValidLine = false;
		p = strtok( NULL, m_seperate.c_str() );
		if ( p != NULL )
		{
			 t_idType = p[0];
			 IsValidLine = true;
		}
		//cout << t_idType << endl;
    //获取用户标识
		if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if ( p != NULL )
			 {
			 	  t_userId = p;
			 	  IsValidLine = true;
			 }
		}
		//cout << t_userId << endl;
    //明细帐目编码
		if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if (p != NULL)
			 {
			   	t_detailItemCode = atoi(p);
			 	  IsValidLine = true;
			 }
		}
		//cout << t_detailItemCode << endl;
		//帐务优惠金额
		if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if (p != NULL)
			 {
			 	  t_discountFee = atol(p);
			 	  IsValidLine = true;
			 }
		}
		//cout << t_discountFee << endl;
		//资费编码
		if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if (p != NULL)
			 {
			 	  t_discountId = atoi(p);
			 	  IsValidLine = true;
			 }
		}
		//cout << t_discountId << endl;
		//帐单类型编码
		if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if (p != NULL)
			 {
			 	  t_billTypeCode = p[0];
			 	  IsValidLine = true;
			 }
		}
		//cout << t_billTypeCode << endl;
		//优惠优先级
		if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if (p != NULL)
			 {
			 	  t_priority = atoi(p);
          IsValidLine = true;
			 }
		}
		//cout << t_priority << endl;
		//帐前调整实例
		if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if (p != NULL)
			 {
			   	t_adjustInsId = atol(p);
          IsValidLine = true;
			 }
		}
		//cout << t_adjustInsId << endl;
    
    //优惠作用ID 
    if (IsValidLine)
		{
			 IsValidLine = false;
			 p = strtok( NULL, m_seperate.c_str() );
			 if (p != NULL)
			 {
			   	t_effectId = atol(p);
          IsValidLine = true;
			 }
		}
		//cout << t_effectId << endl;

		 //判断记录是否有效
    if (IsValidLine)
    {
    	 if ((t_detailItemCode>MAX_ITEMID)||(t_detailItemCode<0))
       {
         IsValidLine = false;
       }
    }

    if (strlen(t_buffer) != 0)
    {
        //累计记录总数
       m_pAccountLogInfo->m_recordNum++;
    }

    //记录无效，累计错误记录数
    if (!IsValidLine)
    {
       if (strlen(t_buffer) != 0)
       {
        //记录有错误，写日志
          m_pAccountLogInfo->m_errorRecordNum++;
       }
       
       if (m_fileStream.eof())
       {
          m_lastId               = "" ;
          m_lastIdType           = '0';
          
          m_lastUserId           = "" ;
          m_lastDetailItemCode   =  0 ;
          m_lastBillTypeCode     = '0';
          m_lastPriority         =  0 ;
          m_lastDiscountFee      =  0 ;
          m_lastDiscountId       =  0 ;
          m_lastAdjustInsId      =  0 ;
          m_lastEffectId         =  0 ; //add by xuf 2010-1-23 10:18:37
       
          break;
       }
       else
       {
           continue;
       }
    }
       
    //如果是老用户,或者首次读取,则直接插入值
    //if (((t_id == m_lastId)&&(t_idType == m_lastIdType)&&(t_userId == m_lastUserId))
    //	   || m_lastUserId == "")
    if(((t_id == m_lastId)&&(t_idType == m_lastIdType))|| m_lastId == "")//modify by zhouq
    {
      userKey.m_id             = t_id;
		  userKey.m_idType         = t_idType;
		  //userKey.m_userId         = t_userId ;
      
      
      
      	userBill.m_userId         = t_userId;
		    userBill.m_detailItemCode = t_detailItemCode;
		    userBill.m_billTypeCode   = t_billTypeCode;
		    userBill.m_priority       = t_priority;
        userBill.m_discountFee    = t_discountFee;
        userBill.m_discountId     = t_discountId;
        userBill.m_adjustInsId    = t_adjustInsId;
        userBill.m_effectId       = t_effectId;//add by xuf 2010-1-23 10:19:06
        
	      //排重
	      int findnum1 = 0;
	      for (itrUserBill=vecUserBill.begin();
	           itrUserBill != vecUserBill.end();++itrUserBill )
	      {
	      	if ( ((*itrUserBill).m_userId == userBill.m_userId)&&
	      		   ((*itrUserBill).m_detailItemCode == userBill.m_detailItemCode)&&
	       		   ((*itrUserBill).m_billTypeCode == userBill.m_billTypeCode)&&
	       		   ((*itrUserBill).m_priority == userBill.m_priority)&&
	       		   ((*itrUserBill).m_discountId == userBill.m_discountId)&&
	       		   ((*itrUserBill).m_adjustInsId == userBill.m_adjustInsId)&&
	       		   ((*itrUserBill).m_effectId == userBill.m_effectId)) //add by xuf 2010-1-23 10:20:19
	       	{
	       		++findnum1;
	       		//(*itrUserBill).m_discountFee += userBill.m_discountFee;
				(*itrUserBill).m_discountFee = userBill.m_discountFee;	 //批价已经做过用户级合并，此处需要排重      			       		
	       	}
	      }
	      if ( findnum1 == 0 )
	      {
	      	vecUserBill.push_back(userBill);
	      }
      
	     //不排重
	     //vecUserBill.push_back(userBill);


      if (m_fileStream.eof())
      {
       	m_lastId               = "" ;
      	m_lastIdType           = '0';
      	m_lastUserId           = "" ;
      	m_lastDetailItemCode   =  0 ;
      	m_lastBillTypeCode     = '0';
      	m_lastPriority         =  0 ;

      	m_lastDiscountFee      =  0 ;
      	m_lastDiscountId       =  0 ;
      	m_lastAdjustInsId      =  0 ;
        m_lastEffectId         =  0 ;//add by xuf 2010-1-23 10:14:31
        
        break;
      }

      m_lastId               =  t_id;
      m_lastIdType           =  t_idType;
      m_lastUserId           =  t_userId;
      m_lastDetailItemCode   =  t_detailItemCode;
      m_lastBillTypeCode     =  t_billTypeCode;
      m_lastPriority         =  t_priority;
      m_lastDiscountFee      =  t_discountFee;
      m_lastDiscountId       =  t_discountId;
      m_lastAdjustInsId      =  t_adjustInsId;
      m_lastEffectId         =  t_effectId;//add by xuf 2010-1-23 10:15:04
    }
     //如果是新用户，记录最后值，跳出
    else
    {
      m_lastId               =  t_id;
      m_lastIdType           =  t_idType;
      m_lastUserId           =  t_userId;
      m_lastDetailItemCode   =  t_detailItemCode;
      m_lastBillTypeCode     =  t_billTypeCode;
      m_lastPriority         =  t_priority;
      m_lastDiscountFee      =  t_discountFee;
      m_lastDiscountId       =  t_discountId;
      m_lastAdjustInsId      =  t_adjustInsId;
      m_lastEffectId         =  t_effectId;// add by xuf 2010-1-23 10:15:24

      break;
    }
  } // end of while

  //获取到费用，返回正确
  if (!vecUserBill.empty())
  {
     return true;
  }

  return false;  //默认返回false

}


void UserBillFacInputFile::closeFile()
{
	//清空
	m_fileName = "";
	m_localOrRemoteTag = '0';

	//关闭文件
	m_fileStream.close();
	m_fileStream.clear();
}

