#include "ValidStaff.h"

NewAuthenClient g_NewAuthenClient;

/********************************************************************
*函数:   	CheckLogStaff(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能:		给流程管理系统提供访问接口，输入输出参数为FML域
********************************************************************/
extern "C" void CheckLogStaff(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	string str_staffid,str_passwd,str_IP, str_eparchy, str_SecFlag, str_SecPasswd;
	string str_FirstReturnState, str_OperSecondFlag;
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
	
	ValidStaff * TValidStaff = new ValidStaff(str_staffid,str_passwd,str_eparchy,
	                                            str_SecFlag, str_SecPasswd, 
	                                            str_FirstReturnState, str_OperSecondFlag);
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
ValidStaff::ValidStaff(const string login_staffid,const string login_password,const string login_eparchy,
                        const string str_DealFlag, const string str_Second_Passwd, 
                        const string str_Answer_State, const string str_Oper_Second)
{
	pcon = jdb::getConnect();
	str_staffid = login_staffid;
	str_passwd = login_password;
	str_EparchyCode = login_eparchy;
	str_SecFlag = str_DealFlag;
    str_SecPasswd = str_Second_Passwd;
    str_FirstReturnState = str_Answer_State;
	str_OperSecondFlag = str_Oper_Second;
	
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
	string strCallSecIntf = "0";        //默认不调用
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
    }
    catch(Exception &ex)
	{
	    delete TValidSecIntf;
		THROW_AC(BaseException,ex,VALIDPASSWD_ERR,"获取校验安全认证接口参数异常" + ex.toString());
	}
    
    delete TValidSecIntf;
    
    printf("---------------------str_SecFlag=[%s]--------------", str_SecFlag.c_str());
    printf("---------------------strCallSecIntf=[%s]--------------", strCallSecIntf.c_str());
    
    if (str_SecFlag == "C")
    {
        i_ResultCode = 0;
        B_TemBuf.SetInt("X_RESULTCODE", i_ResultCode, 0);
        B_TemBuf.SetString("X_RESULTINFO", "获取参数成功！", 0);
        B_TemBuf.SetString("RSRV_TAG1", strCallSecIntf, 0);
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
    	if (i_CheckLog == -1 || i_CheckLog == -8)
    	{
    		if (i_CheckLog == -8)
    		    i_ResultCode = -8;
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
		str_ResultInfo = "登陆员工登陆时间验证非法：" + str_ResultInfo;
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
    char conf1[200];
    char conf2[200];
    char strReturn_ErrMsg[500];
    
    if (strCallSecIntf == "1" && str_SecFlag == "1" && str_OperSecondFlag != "2")
    {
    	try
    	{
    		int iCheckInit = 0;
    		char strFilePath[100] = "../etc/authen.conf";
    		iCheckInit = readAuthenConfig(strFilePath, conf1, conf2);
    		
    		char strReturn_State_Out[300];
    		memset(strReturn_State_Out, 0, sizeof(strReturn_State_Out));
    		char str_InStaff_Id[20]  ;
    		memset(str_InStaff_Id, 0, sizeof(str_InStaff_Id));
    		
    		memset(strReturn_ErrMsg, 0, sizeof(strReturn_ErrMsg));
    		
    		printf("---------------------iCheckInit11=[%d]--------------", iCheckInit);     		 
    		 
    		if(iCheckInit == 0)
        	{
            	g_NewAuthenClient.setServer(conf1);
            	
            	strcpy(str_InStaff_Id, str_staffid.c_str());
            	//char str_InPasswd[50] = "lc";
            	char str_InPasswd[50] ;
            	memset(str_InPasswd, 0, sizeof(str_InPasswd));
            	
            	strcpy(str_InPasswd, str_passwd.c_str());
            	char strTypeName[10] = "BOSS";
            	
            	printf("---------------------str_InStaff_Id12=[%s]--------------", str_InStaff_Id);
            	printf("---------------------str_InPasswd12=[%s]--------------", str_InPasswd);
            	
            	iCheckInit = g_NewAuthenClient.UserAuth(str_InStaff_Id, str_InPasswd, 
            	                                        strReturn_State_Out, strReturn_ErrMsg);
            	
            	printf("---------------------strReturn_State_Out12=[%s]--------------", strReturn_State_Out);
            	printf("---------------------strReturn_ErrMsg12=[%s]--------------", strReturn_ErrMsg);
            	printf("-----------iCheckInit12=[%d]---------", iCheckInit);           	
            }
            
            if (iCheckInit != 0 && iCheckInit != 3 && iCheckInit != -2003 && iCheckInit != -2005)
            {
                
                getErrorMessage(iCheckInit, strReturn_ErrMsg);
                str_IntfHintInfo = strReturn_ErrMsg;
                    
                str_ResultInfo = "第一次调安全认证接口失败：" + str_IntfHintInfo ;
                i_ResultCode = -21;
                THROW_C(BaseException, GETSTAFFINFO_ERR, str_ResultInfo);
            }
            else
            {
                i_ResultCode = 0;                
                
                if (iCheckInit == -2003 || iCheckInit == -2005)
                {
                    getErrorMessage(iCheckInit, strReturn_ErrMsg);
                     str_IntfHintInfo = strReturn_ErrMsg;
                }
                
                if (iCheckInit == 3)
                {
                    B_TemBuf.SetString("ANSWER_STATE", strReturn_State_Out, 0);
                    B_TemBuf.SetString("OPER_FLAG", "2", 0);            //需要二次认证
                }
                else if (iCheckInit == 0)
                {
                    B_TemBuf.SetString("ANSWER_STATE", "", 0);
                    B_TemBuf.SetString("OPER_FLAG", "3", 0);            //只需要静态认证
                    //i_CheckLog = -99;
                }
                else
                {
                    B_TemBuf.SetString("ANSWER_STATE", "", 0);
                    B_TemBuf.SetString("OPER_FLAG", "1", 0);            //启动本地BOSS认证
                    i_CheckLog = -99;
                }
            }

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
    		int iCheckInit = 0;
    		//char strFilePath2[100] = "../etc/link_radius.conf";
    		char strFilePath2[100] = "../etc/authen.conf";
    		iCheckInit = readAuthenConfig(strFilePath2, conf1, conf2);
    		printf("-----------iCheckInit21=[%d]---------", iCheckInit); 
    		
    		if(iCheckInit == 0)
        	{
            	g_NewAuthenClient.setServer(conf1);
            	
            	char str_InStaff_Id[20]  ;
            	memset(str_InStaff_Id, 0, sizeof(str_InStaff_Id));
            	
            	strcpy(str_InStaff_Id, str_staffid.c_str());
            	//char str_InPasswd[50] = "lc";
            	char str_InPasswd[50] ;
            	memset(str_InPasswd, 0, sizeof(str_InPasswd));
            	strcpy(str_InPasswd, str_SecPasswd.c_str());
            	
            	char strReturn_State_In[300];
            	memset(strReturn_State_In, 0, sizeof(strReturn_State_In));
            	strcpy(strReturn_State_In, str_FirstReturnState.c_str());
            	
            	printf("---------------------str_InStaff_Id21=[%s]--------------", str_InStaff_Id);
            	printf("---------------------str_InPasswd21=[%s]--------------", str_InPasswd);
            	printf("---------------------strReturn_State_In21=[%s]--------------", strReturn_State_In);

            	iCheckInit = g_NewAuthenClient.Second_Auth(str_InStaff_Id, str_InPasswd, 
            	                                            strReturn_State_In, strReturn_ErrMsg); 
            	
            	printf("---------------------iCheckInit22=[%d]--------------", iCheckInit);         	
            }
            
            if (iCheckInit != 0)
            {               
                
                getErrorMessage(iCheckInit, strReturn_ErrMsg);
                //strcpy(str_IntfHintInfo.c_str(), strReturn_ErrMsg);
                str_IntfHintInfo = strReturn_ErrMsg;
                    
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
	JTable * TValidStaff = pcon->CreateJTable();
	try
	{
    	TValidStaff->AddSql("SELECT staff_name,dimission_tag,depart_id FROM td_m_staff WHERE staff_id =:1");
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
    			
    			//Add By Zhu_ZhiMin On : 2006-05-21 Remark : 增加员工工号是否禁用的判断
    			TValidStaff->Clear();
    			TValidStaff->AddSql("SELECT OPERED_STAFF_ID, REMARK FROM tl_m_rightlog ");
    			TValidStaff->AddSql(" WHERE right_code ='STAFF_UNLOG_FLAG' AND opered_staff_id=:1 ");
    			TValidStaff->AddSql("   AND right_attr='S' AND right_type='7000' AND use_tag='2' ");
    			
    	        TValidStaff->SetParam(":1",str_staffid);
    	        TValidStaff->ExecSelect();
    			if(TValidStaff->Fetch())
    			{
    			    str_ResultInfo = "该工号已被系统锁定，不能登录，请与管理员联系！";
    		        delete TValidStaff;
    		        return false;
    			}
    			
    			delete TValidStaff;
    			return true;
    		}
    	}
    }
    catch(Exception &ex)
	{
	    delete TValidStaff;
		THROW_AC(BaseException,ex,VALIDSTAFF_ERR,"登陆员工身份校验函数异常" + ex.toString());
	}
}
/********************************************************************
*函数：		ValidPasswd()
*功能：		登陆员工输入密码校验函数
*作者：     Zhu_ZhiMin On : 2006-05-23 Remark : 修改返回值由布尔型为整型，用来不同的判断
*返回：		int返回,0-成功，-1－失败，-8－失败，前台累计失败次数，-9－返回提示信息
********************************************************************/
//bool ValidStaff::ValidPasswd()
int ValidStaff::ValidPasswd()
{
	string str_temstaffpasswd;
	JTable * TValidPasswd = pcon->CreateJTable();
	try
	{
    	//Add By Zhu_ZhiMin On : 2006-05-19 Remark : 增加校验员工密码时间范围的参数
		TValidPasswd->AddSql("SELECT NVL(PARA_VALUE9, 90) PARA_VALUE9, NVL(PARA_VALUE10, 3) PARA_VALUE10 FROM td_s_syspara_deflist ");
		TValidPasswd->AddSql("WHERE EPARCHY_CODE=(SELECT rsvalue2 FROM td_m_depart ");
		TValidPasswd->AddSql("                     WHERE depart_id=(SELECT depart_id FROM  td_m_staff WHERE staff_id = :1)) "); 
		TValidPasswd->AddSql("  AND PARA_ATTR=1000 AND PARA_CODE1='STAFF_PASSWD_OLDDAY_COUNT' AND PARA_CODE2='0' ");
		TValidPasswd->SetParam(":1",str_staffid);
		
		TValidPasswd->ExecSelect();
		int iStaffPasswdDayCount = 90;                              //默认90天必须更新密码
		int iStaffHintDayCount = 3;                                 //默认提前3天提醒修改密码
		string strStaffPasswdDayCount = "90";
		string strStaffHintDayCount = "3";                          //默认3天
		
        while(TValidPasswd->Fetch())
		{
		    strStaffPasswdDayCount = TValidPasswd->GetString("PARA_VALUE9");
		    strStaffHintDayCount = TValidPasswd->GetString("PARA_VALUE10");
		    
		    iStaffPasswdDayCount = atol(strStaffPasswdDayCount.c_str());
		    iStaffHintDayCount = atol(strStaffHintDayCount.c_str());
        }
    	
    	TValidPasswd->Clear();
    	TValidPasswd->AddSql("SELECT STAFF_PASSWD, TRUNC(SYSDATE - NVL(update_time, to_date('1900-01-01','yyyy-mm-dd hh24:mi:ss'))) UPDATE_TIME ");
    	TValidPasswd->AddSql("  FROM tf_m_staffpasswd WHERE staff_id =:1");
    	TValidPasswd->SetParam(":1",str_staffid);
    	TValidPasswd->ExecSelect();
    	if(!TValidPasswd->Fetch())
    	{
    		str_ResultInfo = "获取不到登陆员工的密码！";
    		delete TValidPasswd;
    		//return false;
    		return -8;
    	}
    	else
    	{
    		str_temstaffpasswd = TValidPasswd->GetString("STAFF_PASSWD");
    		string strRealPasswdDay = TValidPasswd->GetString("UPDATE_TIME");
    		int iRealStaffPasswdDayCount = 0;
    		iRealStaffPasswdDayCount = atol(strRealPasswdDay.c_str());
    		
    		if(str_passwd!=str_temstaffpasswd)
    		{
    			str_ResultInfo = "用户登陆输入密码不正确！";
    			delete TValidPasswd;
    			//return false;
    			return -8;
    		}
    		//Add By Zhu_ZhiMin On : 2006-05-19 Remark : 增加密码有效期的判断
    		else if (iRealStaffPasswdDayCount > iStaffPasswdDayCount)
    		{
    		    str_ResultInfo = "用户设置密码超过[" + strStaffPasswdDayCount + "]天未更新，";
    		    str_ResultInfo = str_ResultInfo + "已经过期，不能登录，请与管理员联系！";
    			delete TValidPasswd;
    			return -1;
    	    }
    	    else if (iStaffPasswdDayCount - iRealStaffPasswdDayCount <= iStaffHintDayCount)
    	    {
    	        int iRange = iStaffPasswdDayCount - iRealStaffPasswdDayCount;
    	        string strRange = "";

    	        stringstream stm;
                stm << iRange;
                stm >> strRange;
                
    	        if (iRange == 0)
    	            str_HintInfo = "您的密码明天过期，";
    	        else
    	            str_HintInfo = "您的密码还有[" + strRange + "]天即将过期，";
    		    str_HintInfo = str_HintInfo + "请尽快设置新密码，以免影响正常的登录系统！";
    			delete TValidPasswd;
    			return -9;
    	    }
    		else
    		{
    			delete TValidPasswd;
    			return 0;
    		}
    	}
    }
    catch(Exception &ex)
	{
	    delete TValidPasswd;
		THROW_AC(BaseException,ex,VALIDPASSWD_ERR,"登陆员工输入密码校验函数异常" + ex.toString());
	}
	return 0;
}
/********************************************************************
*函数：		ValidIP()
*功能：		登陆员工IP校验函数（认证同时获取参数）
*返回：		bool返回
********************************************************************/
bool ValidStaff::ValidIP()
{
    JTable * TValidIP = pcon->CreateJTable();
    
    int iLimitTag = 0;                                              //登陆受限标记
    string str_IP = "";
    string str_StaffId_Limit = "1";                                 //默认限制
    try
    {
        //Add By Zhu_ZhiMin On : 2006-05-25 Remark : 增加参数配置
        TValidIP->Clear();
        TValidIP->AddSql("SELECT NVL(PARA_VALUE1, '1') PARA_VALUE1 FROM td_s_syspara_deflist ");
		TValidIP->AddSql("WHERE EPARCHY_CODE= (SELECT rsvalue2 FROM td_m_depart ");
		TValidIP->AddSql("                      WHERE depart_id=(SELECT depart_id FROM  td_m_staff WHERE staff_id = :1)) ");  
		TValidIP->AddSql("  AND PARA_ATTR=1000 AND PARA_CODE1='STAFF_LOGIN_STAFFID_LIMIT' AND PARA_CODE2='0' ");
		TValidIP->SetParam(":1", str_staffid);
		TValidIP->ExecSelect();
        if(!TValidIP->Fetch())
    	    str_StaffId_Limit = "1";
    	else
    	    str_StaffId_Limit = TValidIP->GetString("PARA_VALUE1");
    	
    	if (str_StaffId_Limit == "")
    	    str_StaffId_Limit = "1";
        
        for(int i=0; i<v_IP.size(); i++)
        {
            str_IP.clear();
            str_IP = v_IP[i];
//        	JTable * TValidIP = pcon->CreateJTable();
            TValidIP->Clear();
            TValidIP->AddSql("SELECT log_tag,force_tag,area_code,depart_id FROM tf_m_termipaddr  ");
            TValidIP->AddSql("WHERE net_seg =SUBSTR(:1,INSTR(:1,'.',1,2)+1,INSTR(:1,'.',1,3)-INSTR(:1,'.',1,2)-1) ");
            TValidIP->AddSql("AND addr_start <=TO_NUMBER(SUBSTR(:1,INSTR(:1,'.',1,3)+1,20)) ");
            TValidIP->AddSql("AND addr_end >= TO_NUMBER(SUBSTR(:1,INSTR(:1,'.',1,3)+1,20)) ");
            
            
            //Add By Zhu_ZhiMin On : 2006-12-31 
            //Remark : 增加IP地址前两位的校验
            TValidIP->AddSql("AND SUBSTR(ip_start, 1, INSTR(ip_start,'.',1,2)) = SUBSTR(ip_end, 1, INSTR(ip_end,'.',1,2)) ");
            TValidIP->AddSql("AND SUBSTR(ip_start, 1, INSTR(ip_start,'.',1,2)-1) = SUBSTR(:1, 1, INSTR(:1,'.',1,2)-1) ");
            
            TValidIP->AddSql("AND ROWNUM < 2");
            TValidIP->SetParam(":1",str_IP);
            TValidIP->ExecSelect();
        	if(!TValidIP->Fetch())
        	{
        		str_ResultInfo = "非法登陆IP！";
        		iLimitTag = 0;
        		continue;
//        		delete TValidIP;
//        		return false;
        	}
        	else
        	{
        		if(TValidIP->GetString("LOG_TAG")!="1")
        		{
        			str_ResultInfo = "当前登陆IP不允许登陆！";
        			iLimitTag = 0;
        			continue;
//        			delete TValidIP;
//        			return false;
        		}
        		else
        		{
        			/*IP强制判断*/
        			if(TValidIP->GetString("FORCE_TAG")=="1")
        			{
        				str_AreaCode = TValidIP->GetString("AREA_CODE");
        				str_DepartId = TValidIP->GetString("DEPART_ID");
            			TValidIP->Clear();
            			TValidIP->AddSql("SELECT depart_code, depart_name FROM td_m_depart WHERE depart_id =:1");
            			TValidIP->SetParam(":1",str_DepartId);
            			TValidIP->ExecSelect();
        				if(!TValidIP->Fetch())
        				{
        					str_ResultInfo = "用户IP合法，但是强制IP状态下获取地市编码失败！";
        					iLimitTag = 0;
        					continue;
//        					delete TValidIP;
//        					return false;
        				}
        				else
        				{
        					str_DepartCode = TValidIP->GetString("DEPART_CODE");
        					str_DepartName = TValidIP->GetString("DEPART_NAME");
        				}
        			}
        			else
        			{
            			TValidIP->Clear();
            			TValidIP->AddSql("SELECT area_code, depart_code, depart_name FROM td_m_depart WHERE depart_id =:1");
            			TValidIP->SetParam(":1",str_DepartId);
            			TValidIP->ExecSelect();
        				if(!TValidIP->Fetch())
        				{
        					str_ResultInfo = "用户IP合法，但是非强制IP状态下获取地市编码失败！";
        					iLimitTag = 0;
        					continue;
//        					delete TValidIP;
//        					return false;
        				}
        				else
        				{
        					str_AreaCode = TValidIP->GetString("AREA_CODE");
        					str_DepartCode = TValidIP->GetString("DEPART_CODE");
        					str_DepartName = TValidIP->GetString("DEPART_NAME");
        				}
        			}
        		}
        	}
        
        	/*登陆IP受限判断*/
            TValidIP->Clear();
            TValidIP->AddSql("SELECT staff_id FROM TF_M_STAFFTERMIPADDR WHERE staff_id =:1 AND term_ip =:2");
            TValidIP->SetParam(":1",str_staffid);
            TValidIP->SetParam(":2",str_IP);
            TValidIP->ExecSelect();
        	if(TValidIP->Fetch())
        	{
        		str_ResultInfo = "该员工此IP无权登陆系统！";
        		iLimitTag = 0;
        		continue;
//        		delete TValidIP;
//        		return false;
        	}
        	else
        	{
        	    iLimitTag = 1;
        	    if (str_StaffId_Limit == "0")
        	        break;
//        		delete TValidIP;
//        		return true;
        	}
        	
        	/*Add By Zhu_ZhiMin On : 2006-05-24 
        	  Remark : 增加同一个工号在当前时间内只能在一个IP登录功能，避免同工号同时操作业务*/
        	if (str_StaffId_Limit == "1")
            {
                TValidIP->Clear();
                TValidIP->AddSql("SELECT /*+INDEX(a IDX_TL_M_STAFFLOG_INTIME)*/ STAFF_ID, IN_IP FROM tl_m_stafflog a ");
                TValidIP->AddSql(" WHERE staff_id =:1 AND IN_IP !=:2 AND OUT_TIME  IS NULL");
                TValidIP->SetParam(":1",str_staffid);
                TValidIP->SetParam(":2",str_IP);
                TValidIP->ExecSelect();
            	if(TValidIP->Fetch())
            	{
            	    string strOtherIp = TValidIP->GetString("IN_IP");
            	    str_ResultInfo = "该工号已经由[" + strOtherIp + "]终端IP使用，不允许多个机器同时登录，请与管理员联系！";
            		iLimitTag = 0;
            		continue;
            	}
            	else
            	{
            	    iLimitTag = 1;
            	    break;
            	}                                                   //End if(TValidIP->Fetch())..else...
            }                                                       //End if (str_StaffId_Limit == "1")
        }                                                           //End for(int i=0; i<v_IP.size(); i++)
    }                                                               //End try...catch...
    catch(Exception &ex)
    {
        delete TValidIP;
    	THROW_AC(BaseException,ex,VALIDIP_ERR,"获取IP限制信息异常" + ex.toString());
    }
    if(iLimitTag == 0)
    {
        delete TValidIP;
        return false;
    }
    else if(iLimitTag == 1)
    {
        delete TValidIP;
        return true;
    }
    
    return true;
}
/********************************************************************
*函数：		ValidDate()
*功能：		登陆员工登陆时间校验函数
*返回:		bool返回
********************************************************************/
bool ValidStaff::ValidDate()
{
	string str_temstartdate;
	string str_temenddate;
	string str_temstarttime;
	string str_temendtime;
	string str_temsysdate;

	JTable * TValidDate = pcon->CreateJTable();
	try
	{
    	TValidDate->AddSql("SELECT COUNT(*) COUNT_1 FROM TF_M_STAFFDATETIME WHERE STAFF_ID =:1");
    	TValidDate->SetParam(":1",str_staffid);
    	TValidDate->ExecSelect();
    	if(!TValidDate->Fetch())
    	{
    		str_ResultInfo = "获取该员工号登陆时间限制计数无数据！";
    		delete TValidDate;
    		return false;
    	}
    	else
    	{
    
    		/*登陆时间受限判断*/
    		if(TValidDate->GetInt("COUNT_1")==0)
    		{
    			delete TValidDate;
    			return true;
    		}
    		else
    		{
        		TValidDate->Clear();
        		TValidDate->AddSql("SELECT TO_CHAR(SYSDATE,'yyyymmddhh24miss') A,TO_CHAR(MAX(NVL(start_date,TO_DATE('19900101','yyyymmdd'))),'yyyymmddhh24miss') B,TO_CHAR(MIN(NVL(end_date,TO_DATE('20201231','yyyymmdd'))),'yyyymmddhh24miss') C,TO_CHAR(MAX(NVL(start_time,TRUNC(SYSDATE))),'hh24miss') D,TO_CHAR(MIN(NVL(end_time,TO_date('235959','hh24miss'))),'hh24miss') E FROM TF_M_STAFFDATETIME WHERE STAFF_ID =:1 AND SYS_TAG = '0' OR SYS_TAG ='1'");
        		TValidDate->SetParam(":1",str_staffid);
        		TValidDate->ExecSelect();
    			if(!TValidDate->Fetch())
    			{
    				str_ResultInfo = "获取该员工号登陆时间限制无数据！";
    				delete TValidDate;
    				return false;
    			}
    			else
    			{
    
    				str_temsysdate = TValidDate->GetString("A");
    				str_temstartdate = TValidDate->GetString("B");
    				str_temenddate = TValidDate->GetString("C");
    				str_temstarttime = TValidDate->GetString("D");
    				str_temendtime = TValidDate->GetString("E");
    
    				if(str_temsysdate>=str_temstartdate&&
    					 str_temsysdate<=str_temenddate&&
    					 str_temsysdate.substr(8,6)>=str_temstarttime&&
    					 str_temsysdate.substr(8,6)<=str_temendtime)
    				{
    					delete TValidDate;
    					return true;
    				}
    				else
    				{
    					str_ResultInfo = "登陆的时间不对：您应当在"+str_temstartdate.substr(0,4)+"年"+str_temstartdate.substr(4,2)+"月"+str_temstartdate.substr(6,2)+"日";
    					str_ResultInfo = str_ResultInfo+"至"+str_temenddate.substr(0,4)+"年"+str_temenddate.substr(4,2)+"月"+str_temenddate.substr(6,2)+"日的这段时间内";
    					str_ResultInfo = str_ResultInfo+str_temstarttime.substr(0,2)+":"+str_temstarttime.substr(2,2)+":"+str_temstarttime.substr(4,2)+"~~";
    					delete TValidDate;
    					return false;
    				}
    			}
    		}
    	}
    }
    catch(Exception &ex)
    {
        delete TValidDate;
        THROW_AC(BaseException,ex,VALIDDATE_ERR,"获取员工时间限制异常：" + ex.toString());
    }
}
/********************************************************************
*函数：		GetStaffInfo()
*功能：		当前地市校验函数（获取当前地市）
*返回：		bool返回
********************************************************************/
bool ValidStaff::GetStaffInfo()
{
	string str_ThisAreaCode;
	string str_StaffEparchyCode;
	if(str_EparchyCode == "")
	{
		str_ResultInfo = "获取登陆员工相应登陆地市编码失败！";
		return false;
	}
	JTable * TGetStaffInfo = pcon->CreateJTable();
	try
	{
    	TGetStaffInfo->AddSql("SELECT area_code AREA_CODE FROM td_m_area WHERE validflag='0' AND this_tag = '1' AND area_level=20 AND area_code = :1");
    	TGetStaffInfo->SetParam(":1", str_EparchyCode);
    	TGetStaffInfo->ExecSelect();
    	if(!TGetStaffInfo->Fetch())
    	{
 ubstr(4,2)+"鏈