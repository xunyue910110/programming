#include "AcctBillFacInputFile.h"
#include "appfrm/ErrorHandle.h"

extern ErrorMessages *const theErrorMessages;

AcctBillFacInputFile::AcctBillFacInputFile()
{
}


AcctBillFacInputFile::~AcctBillFacInputFile()
{
}


bool AcctBillFacInputFile::init(const string& strInputFilePath,
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


bool AcctBillFacInputFile::openFile(const string& strFileName,
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


bool AcctBillFacInputFile::getAcctBill(
                               AcctKey& acctKey, vector<AcctBill> &vecAcctBill)
{
	//初始化输出参数
	acctKey.m_acctId = INVALID_ACCTID;
	acctKey.m_userId = INVALID_USERID;    
    
	vecAcctBill.clear();
	
	//定义局部变量并赋初始值
	char   t_buffer[100+1];
	
	T_SERVER_ID  t_userId;
	T_SERVER_ID  t_acctId;

    String<4>   t_eparchyCode;
	long    t_itemcode; 
	long    t_initfee;       
	long    t_fee;                
	long    t_adiscout;           
	long    t_adjustBefore;  
	long    t_acctday;
	  
	char*  p;
	string t_str;
	
	bool   IsValidLine = false;
	
	AcctBill acctBill;
	
	//本地还是异地标志对于一次调用来说是固定的
	//acctKey.m_tag = m_localOrRemoteTag;   
	
	memset(t_buffer,0,sizeof(t_buffer)); 
	
	if (m_lastStrUserId != "")
  {
    	  acctKey.m_acctId     = m_lastStrAcctId;
    	  acctKey.m_userId     = m_lastStrUserId;    
        
        acctBill.m_eparchyCode      = m_lastEparchyCode;
        acctBill.m_detailItemCode   = m_lastItemCode;
        acctBill.m_initfee          = m_lastInitFee;
        acctBill.m_fee              = m_lastFee;
        acctBill.m_adiscount        = m_lastAdiscount;
        acctBill.m_adjustBefore     = m_lastadjustBefore;
        acctBill.m_acctDay = m_lastacctDay;
                                 
        vecAcctBill.push_back(acctBill);
        
        if (m_fileStream.eof())
        {
            m_lastStrAcctId    = "";
            m_lastStrUserId    = "";

            m_lastEparchyCode  = "";  
            m_lastItemCode     = 0;
            m_lastInitFee      = 0;
            m_lastFee          = 0;
            m_lastAdiscount    = 0;
            m_lastadjustBefore = 0;
	     m_lastacctDay = 0;
            
        }    
  }
    
    while (!m_fileStream.eof())
    {
    	//获取一行记录信息 
      memset(t_buffer,0,sizeof(t_buffer));     
      p = NULL;
		  m_fileStream.getline(t_buffer,100);
		
		  p = t_buffer;
		
		//获取账户标识
		IsValidLine = false;
		p = strtok( p, m_seperate.c_str() );
		if ( p != NULL )
		{
			t_acctId = p;
			IsValidLine = true;
		}
		//cout << t_acctId << endl;
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
		
    //获取地州编码  
		if (IsValidLine)
		{
			IsValidLine = false;
			p = strtok( NULL, m_seperate.c_str() );
			if ( p != NULL )
			{
				t_eparchyCode = p;
				IsValidLine = true;
			}
		}
		//cout << t_eparchyCode << endl;
		
    //明细帐目编码
		if (IsValidLine)
		{
			IsValidLine = false;
			p = strtok( NULL, m_seperate.c_str() );
			if (p != NULL)
			{
				t_itemcode = atoi(p);
				IsValidLine = true;
			}
		}
		//cout << t_itemcode << endl;
		//账户级优惠前费用
		if (IsValidLine)
		{
			IsValidLine = false;
			p = strtok( NULL, m_seperate.c_str() );
			if (p != NULL)
			{
				t_initfee = atol(p);
				IsValidLine = true;
			}
		}
		//cout << t_initfee << endl;
		//帐目金额
		if (IsValidLine)
		{
			IsValidLine = false;
			p = strtok( NULL, m_seperate.c_str() );
			if (p != NULL)
			{
				t_fee = atol(p);
				IsValidLine = true;
			}
		}
		//cout << t_fee << endl;
		//帐务优惠
		if (IsValidLine)
		{
			IsValidLine = false;
			p = strtok( NULL, m_seperate.c_str() );
			if (p != NULL)
			{
				t_adiscout = atol(p);
				IsValidLine = true;
			}
		}
		//cout << t_adiscout << endl;
		//帐前调整金额  
		if (IsValidLine)
		{
			IsValidLine = false;
			p = strtok( NULL, m_seperate.c_str() );
			if (p != NULL)
			{
				t_adjustBefore = atol(p);
                IsValidLine = true;
			}
		}
		//cout << t_adjustBefore << endl;
		//ACCT_DAY  
		if (IsValidLine)
		{
			IsValidLine = false;
			p = strtok( NULL, m_seperate.c_str() );
			if (p != NULL)
			{
				t_acctday= atol(p);
                   IsValidLine = true;
			}
		}
		//cout << t_acctday << endl;
		
		
		//判断记录是否有效
        if (IsValidLine)
        {
        	if ((t_itemcode>MAX_ITEMID)||(t_itemcode<0))
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
              m_lastStrAcctId    = "";
              m_lastStrUserId    = "";
              m_lastEparchyCode  = "";  
	            m_lastInitFee          = 0;
	            m_lastFee          = 0;
	            m_lastAdiscount    = 0;
	            m_lastadjustBefore = 0;
		     m_lastacctDay=0;
                break;                       
            }
            else
            {
                continue;
            }
        }
        
        //如果是老用户,或者首次读取,则直接插入值
        if ( ((t_userId == m_lastStrUserId)&&(t_acctId == m_lastStrAcctId))
           || m_lastStrUserId == "")
        {
            acctKey.m_acctId     = t_acctId; 
            acctKey.m_userId     = t_userId;      
	    	                 
	    	                        
	    	//add of jsunicom
	    	acctBill.m_eparchyCode = t_eparchyCode;
	    	//end of add    
	        acctBill.m_detailItemCode   = t_itemcode;
	        acctBill.m_initfee          = t_initfee;
	        acctBill.m_fee              = t_fee;
	        acctBill.m_adiscount        = t_adiscout;
	        acctBill.m_adjustBefore     = t_adjustBefore;
	        acctBill.m_acctDay   = t_acctday ;
	                                 
	        vecAcctBill.push_back(acctBill); 
            
            if (m_fileStream.eof())
            {
                m_lastStrAcctId    = "";
                m_lastStrUserId    = "";
                
                m_lastEparchyCode  = "";
	            m_lastItemCode     = 0;
	            m_lastInitFee      = 0;
	            m_lastFee          = 0;
	            m_lastAdiscount    = 0;
	            m_lastadjustBefore = 0;
		     m_lastacctDay=0;
                
              break;                      
            }

            m_lastStrAcctId    = t_acctId;
            m_lastStrUserId    = t_userId;     
            
            m_lastEparchyCode  = t_eparchyCode;       
            m_lastItemCode     = t_itemcode;
            m_lastInitFee      = t_initfee;    
            m_lastFee          = t_fee;         
            m_lastAdiscount    = t_adiscout;    
            m_lastadjustBefore = t_adjustBefore;
	     m_lastacctDay   = t_acctday ;
        }
        //如果是新用户，记录最后值，跳出
        else
        {
            m_lastStrAcctId    = t_acctId;
            m_lastStrUserId    = t_userId;        
              
            m_lastEparchyCode  = t_eparchyCode;    
            m_lastItemCode     = t_itemcode;
            m_lastInitFee      = t_initfee;    
            m_lastFee          = t_fee;         
            m_lastAdiscount    = t_adiscout;    
            m_lastadjustBefore = t_adjustBefore;
            m_lastacctDay   = t_acctday ;	
            break;                       
        }                                
    } // end of while                    
                                         
    //获取到费用，返回正确               
    if (!vecAcctBill.empty())
    {        
        return true;
    }
    
    return false;  //默认返回false
}


void AcctBillFacInputFile::closeFile()
{
	//清空
	m_fileName = "";
	m_localOrRemoteTag = '0';
	
	//关闭文件
	m_fileStream.close();
	m_fileStream.clear();
}

