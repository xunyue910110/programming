#include "ValidStaff.h"

/********************************************************************
*函数:   	CheckLogStaff(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能:		给流程管理系统提供访问接口，输入输出参数为FML域
********************************************************************/
extern "C" void CheckLogStaff(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	string str_staffid,str_passwd,str_IP, str_eparchy, str_SecFlag, str_SecPasswd;
	string str_FirstReturnState, str_OperSecondFlag, str_Input_PasswdMode;
	bool b_result = false;
	int count = 0;
	str_staffid = inBuf.GetString("LOGIN_ID");
	str_passwd = inBuf.GetString("LOGIN_PASSWD");
		
	str_eparchy = inBuf.GetString("TRADE_EPARCHY_CODE");
	
	//Add By Zhu_ZhiMin On : 2006-09-21 Remark : 增加调用双因素认证的相关参数信息获取
	if (inBuf.IsFldExist("DEAL_FLAG"))
	    str_SecFlag = inBuf.GetString("DEAL_FLAG");
	else
	    str_SecFlag = "0";              //默认是不调用双因素认证
	    
	if (inBuf.IsFldExist("X_NEW_PASSWD"))
	    str_SecPasswd = inBuf.GetString("X_NEW_PASSWD");
	else
	    str_SecPasswd = "";
	    
	if (inBuf.IsFldExist("ANSWER_STATE"))
	    str_FirstReturnState = inBuf.GetString("ANSWER_STATE");
	else
	    str_FirstReturnState = "";
	    
	if (inBuf.IsFldExist("X_TAGCHAR"))
	    str_OperSecondFlag = inBuf.GetString("X_TAGCHAR");
	else
	    str_OperSecondFlag = "1";
    
    //Add By Zhu_ZhiMin On : 2007-10-19 Remark : 增加随机密码的校验
    if (inBuf.IsFldExist("PASS_FLAG"))
	    str_Input_PasswdMode = inBuf.GetString("PASS_FLAG");
	else 
	    str_Input_PasswdMode = "0" ;
	
	ValidStaff * TValidStaff = new ValidStaff(str_staffid,str_passwd,str_eparchy,
	                                          str_SecFlag, str_SecPasswd, 
	                                          str_FirstReturnState, str_OperSecondFlag,
	                                          str_Input_PasswdMode);
    try
    {		
		count = inBuf.GetCount("IN_IP");
		for(int i = 0; i<count; i++)
		{
		    TValidStaff->v_IP.push_back(inBuf.GetString("IN_IP", i));
		}
		
		outBuf = TValidStaff->CheckLogStaff();
	}
	catch(Exception &ex)
	{
		outBuf.SetInt("X_RESULTCODE",ex.getCode());
		outBuf.SetString("X_RESULTINFO","登陆认证业务处理时资料获取出错:"+ex.toString());
	}
	delete TValidStaff;
	return;
}

/********************************************************************
*函数：		ValidStaff类地构造函数
*功能：		完成登陆认证变量地初始化
*参数：		login_staffid：	登陆员工ID
					login_password：	登陆员工密码
					login_IP：	登陆IP
					login_eparchy：	登陆地市编码
********************************************************************/
ValidStaff::ValidStaff(const string &login_staffid,const string &login_password,const string &login_eparchy,
                        const string &str_DealFlag, const string &str_Second_Passwd, 
                        const string &str_Answer_State, const string &str_Oper_Second,
                        const string &str_Input_PasswdMode)
{
	pcon = jdb::getConnect();
	str_staffid = login_staffid;
	str_passwd = login_password;
	str_EparchyCode = login_eparchy;
	str_SecFlag = str_DealFlag;
    str_SecPasswd = str_Second_Passwd;
    str_FirstReturnState = str_Answer_State;
	str_OperSecondFlag = str_Oper_Second;
	str_InputPasswdMode = str_Input_PasswdMode;
	
	str_AreaCode = "";
	str_DepartId = "";
	i_ResultCode = -1;
	str_ResultInfo = "登陆员工资料认证：";
}
/********************************************************************
*函数：		ValidStaff类的析构函数
*功能：
********************************************************************/
ValidStaff::~ValidStaff()
{

}
/********************************************************************
*函数：		CheckLogStaff函数
*功能：		登陆工号合发性判断总控函数，由其完成对各个私有函数地调用
*返回：		将登陆结果通过域返回给CheckLogStaff地接口函数
********************************************************************/
CFmlBuf ValidStaff::CheckLogStaff()
{
	bool b_CheckLog = false;
	
	//Add By Zhu_ZhiMin On : 2006-09-14 Remark :  增加安全认证的接口
	string strCallSecIntf = "0";            //默认不调用双因素认证
	string strCallExternalIntf = "0";       //默认不调用外围接口
	string strCallDecryptFlag = "1";        //默认是需要加密和解密的(默认需要加密)
	string strCallExtSysInt = "0";          //确定当前工号是否调用外部系统接口，即是否存在工号映射配置(默认不调用)
	
	string strCallLoginMessage = "0";       //默认登录后不发送短信
	string strCallSendMessage = "0";        //默认工号锁定不发送短信
	
    JTable * TValidSecIntf = pcon->CreateJTable();
	try
	{
		TValidSecIntf->AddSql("SELECT NVL(PARA_VALUE1, '0') PARA_VALUE1 FROM td_s_syspara_deflist ");
		TValidSecIntf->AddSql("WHERE EPARCHY_CODE=(SELECT rsvalue2 FROM td_m_depart ");
		TValidSecIntf->AddSql("                     WHERE depart_id=(SELECT depart_id FROM  td_m_staff WHERE staff_id = :1)) "); 
		TValidSecIntf->AddSql("  AND PARA_ATTR=1000 AND PARA_CODE1='STAFF_CALL_SEC_INTF' AND PARA_CODE2='0' ");
		TValidSecIntf->SetParam(":1",str_staffid);
		
		TValidSecIntf->ExecSelect();
		
        while(TValidSecIntf->Fetch())
		{
		    strCallSecIntf = TValidSecIntf->GetString("PARA_VALUE1");
        }
        
        if (strCallSecIntf == "")
            strCallSecIntf = "0";
        
        
        //Add By Zhu_ZhiMin On : 2007-05-30 Remark :  增加是否判断工号限定的参数配置
        TValidSecIntf->Clear() ;
        TValidSecIntf->AddSql("SELECT NVL(PARA_VALUE1, '0') PARA_VALUE1, NVL(PARA_VALUE2, '1') PARA_VALUE2 FROM td_s_syspara_deflist ");
		TValidSecIntf->AddSql("WHERE EPARCHY_CODE=(SELECT rsvalue2 FROM td_m_depart ");
		TValidSecIntf->AddSql("                     WHERE depart_id=(SELECT depart_id FROM  td_m_staff WHERE staff_id = :1)) "); 
		TValidSecIntf->AddSql("  AND PARA_ATTR=1000 AND PARA_CODE1='STAFF_ID_LIMIT_EXTSYS' AND PARA_CODE2='0' ");
		TValidSecIntf->SetParam(":1",str_staffid);
		
		TValidSecIntf->ExecSelect();
		
        while(TValidSecIntf->Fetch())
		{
		    strCallExternalIntf = TValidSecIntf->GetString("PARA_VALUE1");
		    strCallDecryptFlag = TValidSecIntf->GetString("PARA_VALUE2");
        }
        
        if (strCallExternalIntf == "")
            strCallExternalIntf = "0";      //默认不调用
        
        if (strCallDecryptFlag == "")
            strCallDecryptFlag = "1";

        //Add By Zhu_ZhiMin On : 2007-10-26 Remark :  增加登录成功后和工号锁定是否发送短信的配置参数
        TValidSecIntf->Clear() ;
		
		TValidSecIntf->AddSql("SELECT PARA_VALUE3, PARA_VALUE4 FROM TD_S_SYSPARA_DEFLIST ");
    	TValidSecIntf->AddSql("WHERE  EPARCHY_CODE=(SELECT rsvalue2 FROM td_m_depart ");
		TValidSecIntf->AddSql("                     WHERE depart_id=(SELECT depart_id FROM  td_m_staff WHERE staff_id = :1)) "); 
		TValidSecIntf->AddSql("  AND PARA_ATTR=2000 AND PARA_CODE1='STAFF_PASSWD_SOX' AND PARA_CODE2='0' ");
    	TValidSecIntf->SetParam(":1", str_staffid);
		
		TValidSecIntf->ExecSelect();
		
        while(TValidSecIntf->Fetch())
		{
		    strCallLoginMessage = TValidSecIntf->GetString("PARA_VALUE3");
		    strCallSendMessage = TValidSecIntf->GetString("PARA_VALUE4");
        }
        
        if (strCallLoginMessage == "")
            strCallLoginMessage = "0" ;    //默认不调用
        
        if (strCallSendMessage == "")
            strCallSendMessage = "0";      //默认不调用
       
       
    }
    catch(Exception &ex)
    {
	    delete TValidSecIntf;
		THROW_AC(BaseException,ex,VALIDPASSWD_ERR,"获取校验安全认证接口参数异常" + ex.toString());
	}
	delete TValidSecIntf;
    
    printf("---------------------str_SecFlag=[%s]--------------", str_SecFlag.c_str());
    printf("---------------------strCallSecIntf=[%s]--------------", strCallSecIntf.c_str());
    printf("---------------------strCallExternalIntf=[%s]--------------", strCallExternalIntf.c_str());

    if (str_SecFlag == "C")
    {
        i_ResultCode = 0;
        B_TemBuf.SetInt("X_RESULTCODE", i_ResultCode, 0);
        B_TemBuf.SetString("X_RESULTINFO", "获取参数成功！", 0);
        B_TemBuf.SetString("RSRV_TAG1", strCallSecIntf, 0);
        B_TemBuf.SetString("RSRV_TAG2", strCallExternalIntf, 0);
        B_TemBuf.SetString("RSRV_STR19", strCallLoginMessage, 0);
        B_TemBuf.SetString("RSRV_STR20", strCallSendMessage, 0);
        return B_TemBuf;
    }
    

	/*员工身份认证*/
	try
	{
		b_CheckLog = ValidStaffid();
	}
	catch(Exception &ex)
	{
		THROW_AC(BaseException,ex,VALIDSTAFF_ERR,"登陆员工身份认证异常：");
	}
	if(b_CheckLog == false)
	{
		i_ResultCode = -1;
		str_ResultInfo = "登陆员工身份验证出错：" + str_ResultInfo;
		THROW_C(BaseException,VALIDSTAFF_ERR,str_ResultInfo);
	}	
	
	//Add By Zhu_ZhiMin On : 2007-05-30 Remark : 获取限制信息
	//int iLimitCount = 0 ;             //保存限制数量
	int i_ExternalSysType = -1;         //保存外围系统的类型编码
	string strUseStaffId = "";          //保存呼叫中心的工号
	string strUseStaffPasswd = "";      //保存呼叫中心的密码
	string strUseStaffSn = "";          //保存呼叫中心的密码
	int iUseExternalSysType = -1;       //保存外呼系统的类型编码
	int iOrder = 0;                     //保存序号信息
    if (strCallExternalIntf == "1")
    {
        //查询是否有限制，暂时支持单个员工和部门
        JTable * TCheckStaffLimit = pcon->CreateJTable();
        JTable * TCheckStaffRelation = pcon->CreateJTable();
        
        try
    	{
    		TCheckStaffLimit->AddSql("SELECT LIMIT_ID,EXTERNAL_SYS_TYPE FROM td_m_staff_limit ");
    		TCheckStaffLimit->AddSql(" WHERE ((depart_id=:1 AND limit_type='1') OR (STAFF_ID=:2 AND limit_type='0' )) ");
    		TCheckStaffLimit->AddSql("  AND valid_flag='0' AND start_date<=SYSDATE AND end_date>SYSDATE ORDER BY external_sys_type "); 
    		
    		TCheckStaffLimit->SetParam(":1",str_DepartId);
    		TCheckStaffLimit->SetParam(":2",str_staffid);
    		
    		TCheckStaffLimit->ExecSelect();

            while(TCheckStaffLimit->Fetch())
    		{
    		    i_ExternalSysType = -1;
    		    strUseStaffId = "";
    		    i_ExternalSysType = TCheckStaffLimit->GetInt("EXTERNAL_SYS_TYPE");
    		    
    		    if (i_ExternalSysType != -1)
                {
                    //Modified By Zhu_ZhiMin On : 2007-08-30 Remark : 创建新的对象获取数据
                    //TCheckStaffLimit->Clear();
                    
                    TCheckStaffRelation->Clear();
                    TCheckStaffRelation->AddSql("SELECT TRIM(USE_STAFF_ID) USE_STAFF_ID, STAFF_PASSWD, SERIAL_NUMBER, EXTERNAL_SYS_TYPE ");
        		    TCheckStaffRelation->AddSql(" FROM td_m_staff_id_relation ");
        		    //TCheckStaffRelation->AddSql("WHERE staff_id=:1 AND external_sys_type='0'");
        		    TCheckStaffRelation->AddSql(" WHERE staff_id=:1 AND external_sys_type=:2 ");
        		
        		    TCheckStaffRelation->SetParam(":1", str_staffid);
        		    TCheckStaffRelation->SetParam(":2", i_ExternalSysType);
        		    
        		    TCheckStaffRelation->ExecSelect();
                    
                    string strTempStaffPasswd = "";
                    bool b_TempDecryptResult = false ;  		
                    while(TCheckStaffRelation->Fetch())
            		{
            		    strUseStaffId = TCheckStaffRelation->GetString("USE_STAFF_ID");
            		    strTempStaffPasswd = TCheckStaffRelation->GetString("STAFF_PASSWD");
            		    strUseStaffSn = TCheckStaffRelation->GetString("SERIAL_NUMBER");
            		    iUseExternalSysType = TCheckStaffRelation->GetInt("EXTERNAL_SYS_TYPE");
            		    //解密(调用服务挂死，暂不加密)
            		    /*if (strCallDecryptFlag == "1")
            		    {
            		        b_TempDecryptResult = CryptUtil::decrypt(strTempStaffPasswd.c_str(), strUseStaffPasswd);
                		    if (b_TempDecryptResult == false)
                            {
                                delete TCheckStaffLimit;
                		        THROW_C(BaseException,VALIDPASSWD_ERR,"解密工号失败，被解密的密文不合法！");
                            }
                        }
                        else*/
                        strUseStaffPasswd = strTempStaffPasswd;
                    }
                
                    if (strUseStaffId == "")
                    {
                        i_ResultCode = -19;
                        //Modified By Zhu_ZhiMin On : 2007-08-15 Remark : 增加外围系统类型返回
                        //B_TemBuf.SetString("EXTERNAL_SYS_TYPE", strUseExternalSysType, iOrder);
                        //B_TemBuf.SetInt("EXTERNAL_SYS_TYPE", i_ExternalSysType, iOrder);
                        //B_TemBuf.SetInt("RSRV_NUM2", i_ExternalSysType, iOrder); 
                        //Modified By Zhu_Zhi On : 2007-08-30
                        B_TemBuf.SetInt("RSRV_NUM2", i_ExternalSysType, 0);
                        
                        B_TemBuf.SetInt("X_RESULTCODE", i_ResultCode, 0);
                        if (i_ExternalSysType == 0)
                            B_TemBuf.SetString("X_RESULTINFO", "获取外围工号信息未发现记录，请维护您在客服系统中的投诉工号和密码等信息！", 0);
                        else if (i_ExternalSysType == 1)
                            B_TemBuf.SetString("X_RESULTINFO", "获取外围工号信息未发现记录，请维护您在客服系统中的外呼工号和密码等信息！", 0);
                        else
                            B_TemBuf.SetString("X_RESULTINFO", "获取外围工号信息未发现记录，请维护您在外围系统中的工号和密码等信息！", 0);
                        delete TCheckStaffRelation;
                        delete TCheckStaffLimit;
                        return B_TemBuf;
                    }
                    
                    //只有呼叫中心的才的调用接口校验
                    if (i_ExternalSysType == 0)
                        strCallExtSysInt = "1" ; //需要调用接口校验
                    
                    B_TemBuf.SetString("USE_STAFF_ID", strUseStaffId, iOrder);
	                B_TemBuf.SetString("USER_PASSWD", strUseStaffPasswd, iOrder);
	                B_TemBuf.SetString("BIND_SERIAL_NUMBER", strUseStaffSn, iOrder);
	                B_TemBuf.SetInt("EXTERNAL_SYS_TYPE", iUseExternalSysType, iOrder);
	                iOrder++;
	                
                }   //End if (str_ExternalSysType != "")
            }       //End while(TCheckStaffLimit->Fetch())
            
            B_TemBuf.SetInt("RSRV_NUM1",iOrder-1);
            
        }           //End try
        catch(Exception &ex)
    	{
    	    delete TCheckStaffRelation;
    	    delete TCheckStaffLimit;
    		THROW_AC(BaseException,ex,VALIDPASSWD_ERR,"检测工号限制信息异常" + ex.toString());
    	}
    	delete TCheckStaffRelation;
    	delete TCheckStaffLimit;
    }               //End if (strCallExternalIntf == "1")

	//在配置不调用双因素认证省份和调用但网络连接有问题时，检测本地密码
	int i_CheckLog = -1;
    if (strCallSecIntf == "0" || str_SecFlag == "0")
    {
    	/*登陆员工密码认证*/
    	try
    	{
    		i_CheckLog = ValidPasswd();
    	}
    	catch(Exception &ex)
    	{
    		THROW_AC(BaseException,ex,VALIDPASSWD_ERR,"登陆员工密码认证异常：");
    	}
    	
    	//Modified By Zhu_ZhiMin On : 2006-05-19 Remark :  增加密码检测的错误类型判断
    	/*if(b_CheckLog == false)
    	{
    		i_ResultCode = -2;
    		str_ResultInfo = "登陆员工密码出错:" + str_ResultInfo;
    		THROW_C(BaseException,VALIDPASSWD_ERR,str_ResultInfo);
    	}*/
    	//Modified By Zhu_ZhiMin On : 2007-09-17 Remark :  增加密码初始化检测的错误类型判断
    	//if (i_CheckLog == -1 || i_CheckLog == -8)
    	if (i_CheckLog == -1 || i_CheckLog == -8 || i_CheckLog == -44)
    	{
    		if (i_CheckLog == -8)
    		    i_ResultCode = -8;
    		else if (i_CheckLog == -44) 
    		    i_ResultCode = -44;
    		else
    		    i_ResultCode = -2;
    		    
    		str_ResultInfo = "登陆员工密码验证出错：" + str_ResultInfo;
    		
    		B_TemBuf.SetInt("X_RESULTCODE",i_ResultCode,0);
    		B_TemBuf.SetString("X_RESULTINFO", str_ResultInfo, 0);
    		return B_TemBuf;
    		//THROW_C(BaseException,VALIDPASSWD_ERR,str_ResultInfo);
    	}
    	else
    	    i_ResultCode = i_CheckLog;
    }
	    
	/*登陆员工登陆IP认证*/
	try
	{
		b_CheckLog = ValidIP();
	}
	catch(Exception &ex)
	{
		THROW_AC(BaseException,ex,VALIDIP_ERR,"登陆员工登陆IP认证异常：");
	}
	if(b_CheckLog == false)
	{
		i_ResultCode = -3;
		str_ResultInfo = "登陆员工IP验证出错：" + str_ResultInfo;
		THROW_C(BaseException,VALIDIP_ERR,str_ResultInfo);
	}

	/*登陆员工登陆时间认证*/
	try
	{
		b_CheckLog = ValidDate();
	}
	catch(Exception &ex)
	{
		THROW_AC(BaseException,ex,VALIDDATE_ERR,"登陆员工登陆时间认证异常：");
	}
	if(b_CheckLog == false)
	{
		i_ResultCode = -4;
		str_ResultInfo = "登陆员工登陆时间验证非法" + str_ResultInfo;
		THROW_C(BaseException,VALIDDATE_ERR,str_ResultInfo);
	}

	/*登陆员工登陆地市校验*/
	try
	{
		b_CheckLog = GetStaffInfo();
	}
	catch(Exception &ex)
	{
		THROW_AC(BaseException,ex,GETSTAFFINFO_ERR,"登陆员工登陆地市异常：");
	}
	if(b_CheckLog == false)
	{
		i_ResultCode = -5;
		str_ResultInfo = "登陆员工登陆地市验证非法：" + str_ResultInfo;
		THROW_C(BaseException,GETSTAFFINFO_ERR,str_ResultInfo);
	}

    if (i_ResultCode == -9)
    {
        i_ResultCode = i_CheckLog;
    }
    else
	    i_ResultCode = 0;
	
	    
    //if (strCallSecIntf == "1")
    //Modified By Zhu_ZhiMin On : 2006-09-21 Remark : 增加网络是否正常的标记判断
    string str_IntfHintInfo = "";
    radius_conf_t conf;
    
    if (strCallSecIntf == "1" && str_SecFlag == "1" && str_OperSecondFlag != "2")
    {
    	try
    	{
    		int iCheckInit = 0;
    		char strFilePath[100] = "../etc/link_radius.conf";
    		iCheckInit = init(strFilePath, &conf);
    		
    		char strReturn_State[20];
    		memset(strReturn_State, 0, sizeof(strReturn_State));
    		char str_InStaff_Id[20]  ;
    		memset(str_InStaff_Id, 0, sizeof(str_InStaff_Id));
    		
    		printf("---------------------iCheckInit11=[%d]--------------", iCheckInit);     		 
    		 
    		if(iCheckInit == 0)
        	{
            	strcpy(str_InStaff_Id, str_staffid.c_str());
            	//char str_InPasswd[50] = "lc";
            	char str_InPasswd[50] ;
            	memset(str_InPasswd, 0, sizeof(str_InPasswd));
            	
            	strcpy(str_InPasswd, str_passwd.c_str());
            	char strTypeName[10] = "BOSS";
            	
            	printf("---------------------str_InStaff_Id12=[%s]--------------", str_InStaff_Id);
            	printf("---------------------str_InPasswd12=[%s]--------------", str_InPasswd);
            	printf("---------------------strTypeName12=[%s]--------------", strTypeName);
            	
            	iCheckInit = UserAuth(&conf, str_InStaff_Id, str_InPasswd, strTypeName, strReturn_State);
            	
            	printf("---------------------strReturn_State12=[%s]--------------", strReturn_State);
            	printf("-----------iCheckInit12=[%d]---------", iCheckInit);
            	
            	/*string strRange;
        		stringstream stm;
                stm << iCheckInit;
                stm >> strRange;
                THROW_C(BaseException, GETSTAFFINFO_ERR, "返回结果：[" + strRange + "]");*/
            }
            
            if (iCheckInit != 0 && iCheckInit < 10 && iCheckInit != 1 &&
                iCheckInit != 2 && iCheckInit != 4 && iCheckInit != 5 && iCheckInit != 6)
            {
                
                if (iCheckInit == 3)
                    str_IntfHintInfo = "验证失败，用户为非法用户！";                
                else if (iCheckInit == 7)
                    str_IntfHintInfo = "数据包长度不匹配，或者越界！";
                else if (iCheckInit == 8)
                    str_IntfHintInfo = "响应数据包和请求数据包校验不匹配！";
                else if (iCheckInit == 9)
                    str_IntfHintInfo = "响应数据包和请求数据包ID不匹配！";                
                else
                    str_IntfHintInfo = "发生未知错误！";
                    
                str_ResultInfo = "第一次调安全认证接口失败：" + str_IntfHintInfo ;
                i_ResultCode = -21;
                THROW_C(BaseException, GETSTAFFINFO_ERR, str_ResultInfo);
            }
            else
            {
                i_ResultCode = 0;                
                
                if (iCheckInit == 1)
                    str_IntfHintInfo = "无法打开配置文件或者配置文件不存在！";
                else if (iCheckInit == 2)
                    str_IntfHintInfo = "Radius服务器信息无效！";
                else if (iCheckInit == 4)
                    str_IntfHintInfo = "认证服务器超时！";
                else if (iCheckInit == 5)
                    str_IntfHintInfo = "等待响应超时！";
                else if (iCheckInit == 6)
                    str_IntfHintInfo = "接收数据包失败！";
                else if (iCheckInit == 10)
                    str_IntfHintInfo = "发送数据包到Radius服务器失败！";
                else if (iCheckInit == 11)
                    str_IntfHintInfo = "创建socket失败！";
                else if (iCheckInit == 12)
                    str_IntfHintInfo = "端口绑定失败！";
                else if (iCheckInit == 13)
                    str_IntfHintInfo = "配置文件没有有效服务器信息！";
                
                if (iCheckInit == 111)
                {
                    B_TemBuf.SetString("ANSWER_STATE", strReturn_State, 0);
                    B_TemBuf.SetString("OPER_FLAG", "2", 0);                    
                }
                else
                {
                    B_TemBuf.SetString("ANSWER_STATE", "", 0);
                    B_TemBuf.SetString("OPER_FLAG", "1", 0);
                    i_CheckLog = -99;
                }
            }
            
            /*char str_InPasswd2[50]= "t1q2";
            char strTypeName[10] = "BOSS";
            
            iCheckInit = Second_Auth(&conf, str_InStaff_Id, str_InPasswd2, strTypeName, strReturn_State); 
            printf("-----------iCheckInit一起=[%d]---------", iCheckInit);*/
    	}
    	catch(Exception &ex)
    	{
    		THROW_AC(BaseException,ex,GETSTAFFINFO_ERR,"第一次调安全认证接口异常：");
    	}
	}           //End if (strCallSecIntf == "1")
	
	//Add By Zhu_ZhiMin On : 2006-09-21 Remark : 增加第二次登录认证
	if (strCallSecIntf == "1" && str_SecFlag == "1" && str_OperSecondFlag == "2")
	{
	    try
    	{
    		radius_conf_t conf2;
    		int iCheckInit = 0;
    		char strFilePath2[100] = "../etc/link_radius.conf";
    		iCheckInit = init(strFilePath2, &conf2);
    		printf("-----------iCheckInit21=[%d]---------", iCheckInit); 
    		
    		//memset( &conf2, 0, sizeof(radius_conf_t) );
    		//memcpy( &conf2, &conf_bak,  sizeof(radius_conf_t) );
    		 
    		if(iCheckInit == 0)
        	{
            	char str_InStaff_Id[20]  ;
            	memset(str_InStaff_Id, 0, sizeof(str_InStaff_Id));
            	
            	strcpy(str_InStaff_Id, str_staffid.c_str());
            	//char str_InPasswd[50] = "lc";
            	char str_InPasswd[50] ;
            	memset(str_InPasswd, 0, sizeof(str_InPasswd));
            	strcpy(str_InPasswd, str_SecPasswd.c_str());
            	
            	char strTypeName[10] = "BOSS";
            	char strReturn_State[20];
            	memset(strReturn_State, 0, sizeof(strReturn_State));
            	strcpy(strReturn_State, str_FirstReturnState.c_str());
            	
            	printf("---------------------str_InStaff_Id21=[%s]--------------", str_InStaff_Id);
            	printf("---------------------str_InPasswd21=[%s]--------------", str_InPasswd);
            	printf("---------------------strTypeName21=[%s]--------------", strTypeName);
            	printf("---------------------strReturn_State21=[%s]--------------", strReturn_State);
            	
            	//radius_server_t *server_temp = conf2.server;
            	//printf( "hostname: %s, timeout: %d, tries: %d \n", server_temp-> hostname, server_temp-> timeout, server_temp-> tries );
            	//printf( "strReturn_State: %s \n", strReturn_State );
            	
            	//strcpy(strReturn_State, "1" );
            	iCheckInit = Second_Auth(&conf2, str_InStaff_Id, str_InPasswd, strTypeName, strReturn_State); 
            	
            	printf("---------------------iCheckInit22=[%d]--------------", iCheckInit);         	
            }
            
            if (iCheckInit != 0)
            {               
                if (iCheckInit == 1)
                    str_IntfHintInfo = "无法打开配置文件或者配置文件不存在！";
                else if (iCheckInit == 2)
                    str_IntfHintInfo = "Radius服务器信息无效！";
                else if (iCheckInit == 3)
                    str_IntfHintInfo = "验证失败，用户为非法用户！";
                else if (iCheckInit == 4)
                    str_IntfHintInfo = "认证服务器超时！";
                else if (iCheckInit == 5)
                    str_IntfHintInfo = "等待响应超时！";
                else if (iCheckInit == 6)
                    str_IntfHintInfo = "接收数据包失败！";
                else if (iCheckInit == 7)
                    str_IntfHintInfo = "数据包长度不匹配，或者越界！";
                else if (iCheckInit == 8)
                    str_IntfHintInfo = "响应数据包和请求数据包校验不匹配！";
                else if (iCheckInit == 9)
                    str_IntfHintInfo = "响应数据包和请求数据包ID不匹配！";
                else if (iCheckInit == 10)
                    str_IntfHintInfo = "发送数据包到Radius服务器失败！";
                else if (iCheckInit == 11)
                    str_IntfHintInfo = "创建socket失败！";
                else if (iCheckInit == 12)
                    str_IntfHintInfo = "端口绑定失败！";
                else if (iCheckInit == 13)
                    str_IntfHintInfo = "配置文件没有有效服务器信息！";
                else
                    str_IntfHintInfo = "发生未知错误！";
                    
                str_ResultInfo = "第二次调安全认证接口失败：" + str_IntfHintInfo ;
                i_ResultCode = -22;                
                THROW_C(BaseException, GETSTAFFINFO_ERR, str_ResultInfo);
            }
            else
            {
                i_ResultCode = 0;
                B_TemBuf.SetString("OPER_FLAG", "0", 0);
            }
    	}
    	catch(Exception &ex)
    	{
    		THROW_AC(BaseException,ex,GETSTAFFINFO_ERR,"第二次调安全认证接口异常：");
    	}
    	
    	B_TemBuf.SetInt("X_RESULTCODE",i_ResultCode,0);
    	B_TemBuf.SetString("X_RESULTINFO",str_ResultInfo+"登陆成功！",0);
    	return B_TemBuf;
	}
	
	/*完成登陆认证获取参数*/
	B_TemBuf.SetString("STAFF_NAME",str_StaffName,0);
	B_TemBuf.SetString("DEPART_ID",str_DepartId,0);
	B_TemBuf.SetString("CITY_CODE",str_AreaCode,0);
	B_TemBuf.SetString("DEPART_CODE",str_DepartCode,0);
	B_TemBuf.SetString("DEPART_NAME",str_DepartName,0);
	B_TemBuf.SetInt("X_RESULTCODE",i_ResultCode,0);
	
	//Add By Zhu_ZhiMin On : 2006-09-22 Remark ：增加是否调用双因素认证的标记返回
	B_TemBuf.SetString("ADD_TAG", strCallSecIntf,0);
	
	B_TemBuf.SetString("RSRV_TAG3", strCallExtSysInt, 0);
	
	//Add By Zhu_ZhiMin On : 2007-05-30 Remark : 增加呼叫中心的工号和密码
	//Modified By Zhu_ZhiMin On : 2007-08-13 Remark : 返回多条信息
	//B_TemBuf.SetString("USE_STAFF_ID", strUseStaffId,0);
	//B_TemBuf.SetString("USER_PASSWD", strUseStaffPasswd,0);
	//B_TemBuf.SetString("BIND_SERIAL_NUMBER", strUseStaffSn,0);

	if (strCallSecIntf == "0" || str_SecFlag == "0")
	    B_TemBuf.SetString("OPER_FLAG", "0", 0);
	
	if (i_CheckLog == -9)
	    B_TemBuf.SetString("X_RESULTINFO", str_HintInfo, 0);
	else if (i_CheckLog == -99)
	{
	    B_TemBuf.SetInt("X_RESULTCODE",i_CheckLog,0);
	    B_TemBuf.SetString("X_RESULTINFO", "第一次调安全认证接口异常：" + str_IntfHintInfo + "开始启用BOSS本地认证！", 0);
	}
	else
	    B_TemBuf.SetString("X_RESULTINFO",str_ResultInfo+"登陆成功！",0);
	return B_TemBuf;
}
/********************************************************************
*函数：		ValidStaffid()
*功能：		登陆员工身份校验函数（认证同时获取参数）
*返回：		bool返回
********************************************************************/
bool ValidStaff::ValidStaffid()
{
	//Add By Zhu_ZhiMin On : 2007-10-08 Remark : 增加相关参数标记信息的获取
	str_LogSerialNumberFlag = "0" ;         //默认员工表资料不需要手机号码即可登录
	str_LogRamPasswdFlag = "0" ;            //默认无密码时不需要随机密码或者不显示随机密码
	str_LogSmsSendFlag = "0" ;              //默认登录后不需要发送短信
	
	
	
	JTable * TValidStaff = pcon->CreateJTable();
	try
	{
    	//获取员工登录是否需要手机号码的配置
    	TValidStaff->Clear();
    	TValidStaff->AddSql("SELECT PARA_VALUE1,PARA_VALUE2,PARA_VALUE3 FROM TD_S_SYSPARA_DEFLIST ");
    	TValidStaff->AddSql("WHERE  EPARCHY_CODE=(SELECT rsvalue2 FROM td_m_depart ");
		TValidStaff->AddSql("                     WHERE depart_id=(SELECT depart_id FROM  td_m_staff WHERE staff_id = :1)) "); 
		TValidStaff->AddSql("  AND PARA_ATTR=2000 AND PARA_CODE1='STAFF_PASSWD_SOX' AND PARA_CODE2='0' ");
    	TValidStaff->SetParam(":1", str_staffid);
    	TValidStaff->ExecSelect();
    	if(!TValidStaff->Fetch())
    	{
    	    str_LogSerialNumberFlag = "0";
    	    str_LogRamPasswdFlag = "0" ;
    	    str_LogSmsSendFlag = "0" ;
    	}
    	else
    	{
    	    str_LogSerialNumberFlag = TValidStaff->GetString("PARA_VALUE1");
    	    str_LogRamPasswdFlag = TValidStaff->GetString("PARA_VALUE2");
    	    str_LogSmsSendFlag = TValidStaff->GetString("PARA_VALUE3");
    	}	
    	
    	TValidStaff->Clear();
    	
    	//Modified By Zhu_ZhiMin On : 2007-10-18 Remark : 增加员工手机号码信息的返回并校验
    	//TValidStaff->AddSql("SELECT staff_name,dimission_tag,depart_id FROM td_m_staff WHERE staff_id =:1");
    	
    	TValidStaff->AddSql("SELECT STAFF_NAME,DIMISSION_TAG,DEPART_ID,SERIAL_NUMBER FROM td_m_staff WHERE staff_id =:1");
    	TValidStaff->SetParam(":1",str_staffid);
    	TValidStaff->ExecSelect();
    	if(!TValidStaff->Fetch())
    	{
    		str_ResultInfo = "无此员工号！";
    		delete TValidStaff;
    		return false;
    	}
    	else
    	{
    		if(TValidStaff->GetString("DIMISSION_TAG")=="1")
    		{
    			str_ResultInfo = "此员工已经离职！";
    			delete TValidStaff;
    			return false;
    		}
    		else
    		{
    			str_StaffName = TValidStaff->GetString("STAFF_NAME");
    			str_DepartId = TValidStaff->GetString("DEPART_ID");
    			
    			//Add By Zhu_ZhiMin On : 2007-10-18 Remark : 增加员工是否有移动号码的判断
    			string str_SerialNumber = TValidStaff->GetString("SERIAL_NUMBER");
    			if (str_LogSerialNumberFlag == "1" && str_SerialNumber == "")
    			{
    			    str_ResultInfo = "该工号的资料信息中未登记手机号码，系统拒绝登录！";
    			    delete TValidStaff;
    			    return false;
    			}
    			if (str_LogSerialNumberFlag == "1" && (str_SerialNumber.length() != 11 || str_SerialNumber.substr(0, 1) != "1"))
    			{
    			    str_ResultInfo = "该工号资料信息中登记的手机号码不合法，系统拒绝登录！";
    			    delete TValidStaff;
    			    return false;
    			}
    			//End By Zhu_ZhiMin On : 2007-10-18
    			
    			//Add By Zhu_ZhiMin On : 2006-05-21 Remark : 增加员工工号是否禁用的判断
    			TValidStaff->Clear();
    			TValidStaff->AddSql("SELECT OPERED_STAFF_ID, REMARK FROM tl_m_rightlog ");
    			TValidStaff->AddSql(" WHERE right_code ='STAFF_UNLOG_FLAG' AND opered_staff_id=:1 ");
    			TValidStaff->AddSql("   AND right_attr='S' AND right_type='7000' AND use_tag='2'姝ゅ憳宸ュ彿锛