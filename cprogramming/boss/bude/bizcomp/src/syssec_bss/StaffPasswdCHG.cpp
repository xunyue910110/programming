#include "StaffPasswdCHG.h"

/********************************************************************
*函数：		StaffPasswdCHG构造函数
*功能：		完成相应变量地初始化
*参数：		staffid：	员工工号
					passwd：	员工修改密码
********************************************************************/
StaffPasswdCHG::StaffPasswdCHG(const string staffid,const string passwd)
{
	pcon = jdb::getConnect();
	str_TradeStaffid = staffid;
	str_TradePasswd = passwd;
	i_ResultCode = -1;
	str_ResultInfo = "员工修改登陆密码：";
}
/********************************************************************
*函数：		StaffPasswdCHG析构函数
*功能：
********************************************************************/
StaffPasswdCHG::~StaffPasswdCHG()
{
	
}
/********************************************************************
*函数：		StaffPasswdChange()
*功能：		修改登陆员工地密码
*返回：		FML域返回修改结果
********************************************************************/
CFmlBuf StaffPasswdCHG::StaffPasswdChange()
{
    JTable * TStaffPasswdCHG = pcon->CreateJTable();
    bool bCheckResult = true;
    string strPasswdChangeCount = "0";
    int iRecNo = 0;
    
	try
	{
		//Add By Zhu_ZhiMin On : 2006-05-12 Remark : 增加员工密码修改日志的查询和相关校验
		TStaffPasswdCHG->AddSql("SELECT NVL(PARA_VALUE9, 5) PARA_VALUE9 FROM td_s_syspara_deflist ");
		TStaffPasswdCHG->AddSql("WHERE EPARCHY_CODE='ZZZZ' AND PARA_ATTR=1000 AND PARA_CODE1='STAFF_PASSWD_COUNT' ");
		TStaffPasswdCHG->AddSql("  AND PARA_CODE2='0' ");
		TStaffPasswdCHG->ExecSelect();
		int iStaffPasswdParaCount = 5;                              //默认五次
        while(TStaffPasswdCHG->Fetch())
		{
		    string strStaffPasswdCount = TStaffPasswdCHG->GetString("PARA_VALUE9");
		    iStaffPasswdParaCount = atol(strStaffPasswdCount.c_str());
        }
		
		
		TStaffPasswdCHG->Clear();
		TStaffPasswdCHG->AddSql("SELECT RIGHT_CODE, OPERED_STAFF_ID, DECODE(TIMES, 0, 1, TIMES) TIMES, ");
		TStaffPasswdCHG->AddSql("NVL(RSRV_STR1, '') RSRV_STR1 FROM tl_m_rightlog ");
		TStaffPasswdCHG->AddSql("WHERE RIGHT_CODE='STAFF_PASSWD_REG' AND RIGHT_ATTR='0' AND OPERED_STAFF_ID=:1 ");
		TStaffPasswdCHG->AddSql(" AND RIGHT_TYPE='6000' ORDER BY RSRV_STR1 DESC ");
		
		TStaffPasswdCHG->SetParam(":1",str_TradeStaffid);
		
		TStaffPasswdCHG->ExecSelect();
		string strStaffPasswd = "";                                 //不存在日志的暂时不校验
		int iPasswdChangeCount = 0;
        while(TStaffPasswdCHG->Fetch() && iPasswdChangeCount <= iStaffPasswdParaCount)
		{
		    iPasswdChangeCount++;
		    strStaffPasswd = TStaffPasswdCHG->GetString("RSRV_STR1");
		    if (iPasswdChangeCount == 1)
		        iRecNo = TStaffPasswdCHG->GetInt("TIMES");
		    
		    if (strStaffPasswd != "")
		    {
		        if (strStaffPasswd == str_TradePasswd)
		        {
		            bCheckResult = false;
		            stringstream stm;
                    stm << iPasswdChangeCount;
                    stm >> strPasswdChangeCount;
		            break;
		        }
		    }
		    
        }
        //END Add By Zhu_ZhiMin On : 2006-05-12
        
        if (bCheckResult == true)
        {
    		TStaffPasswdCHG->Clear();
    		TStaffPasswdCHG->AddSql("UPDATE TF_M_STAFFPASSWD SET STAFF_PASSWD=:2,UPDATE_TIME=SYSDATE,UPDATE_STAFF_ID=:1 WHERE STAFF_ID=:1");
    		TStaffPasswdCHG->SetParam(":1",str_TradeStaffid);
    		TStaffPasswdCHG->SetParam(":2",str_TradePasswd);
    		TStaffPasswdCHG->ExecUpdate();
            
            //Add By Zhu_ZhiMin On : 2006-05-12 Remark : 增加员工密码修改日志的记录
            iRecNo++;
            
            string str_SeqPasswdNumber;
        	GetSeqID * TGetPasswdSeqID = new GetSeqID("LOG_ID","8");
        	str_SeqPasswdNumber = TGetPasswdSeqID->GetDateSequencesId();
        	if(str_SeqPasswdNumber == "GetSeqFail")
        	{
        		delete TGetPasswdSeqID;
        		delete TStaffPasswdCHG;
        		i_ResultCode = -1;
    		    str_ResultInfo = str_ResultInfo + "记录密码修改日志，获取Logid失败！";
    		    B_TempBuf.SetInt("X_RESULTCODE",i_ResultCode);
    		    B_TempBuf.SetString("X_RESULTINFO",str_ResultInfo);
        	}
        	else
        	{
        		delete TGetPasswdSeqID;
        		
        		TStaffPasswdCHG->Clear();
        		TStaffPasswdCHG->AddSql("INSERT INTO tl_m_rightlog(RIGHT_CODE,OPERED_STAFF_ID,RIGHT_ATTR,LOG_ID, ");
        		TStaffPasswdCHG->AddSql("OPER_TYPE,RIGHT_TYPE,ROLE_CODE,AREA_CODE, ");
        		TStaffPasswdCHG->AddSql("OPER_STAFF_ID,OPER_DEPART_ID,OPER_DATE,RIGHT_CLASS, ");
        		TStaffPasswdCHG->AddSql("OPER_SPECIAL,EXTEND_VALUE1,EXTEND_VALUE2, ");
        		TStaffPasswdCHG->AddSql("RSVALUE1,RSVALUE2,USE_TAG,TIMES,START_DATE,END_DATE, ");
        		TStaffPasswdCHG->AddSql("RSRV_STR1,RSRV_STR2,RSRV_STR3,RSRV_STR4,RSRV_STR5,REMARK ) ");
        		
        		TStaffPasswdCHG->AddSql("VALUES('STAFF_PASSWD_REG', :1, '0', :2, ");
        		TStaffPasswdCHG->AddSql(" '0', '6000', '', '', ");
        		TStaffPasswdCHG->AddSql(" :3, '', SYSDATE, '', ");
        		TStaffPasswdCHG->AddSql(" '', '', 0, ");
        		TStaffPasswdCHG->AddSql(" '', '', '0', :4, SYSDATE, SYSDATE+ 5000, ");
        		TStaffPasswdCHG->AddSql(" :5, '', '', '', '', '[员工密码修改]' ) ");
        		
            	TStaffPasswdCHG->SetParam(":1", str_TradeStaffid);
            	TStaffPasswdCHG->SetParam(":2", str_SeqPasswdNumber);
            	TStaffPasswdCHG->SetParam(":3", str_TradeStaffid);
            	TStaffPasswdCHG->SetParam(":4", iRecNo);
            	TStaffPasswdCHG->SetParam(":5", str_TradePasswd);
            	TStaffPasswdCHG->ExecInsert();
        		
        		i_ResultCode = 0;
        		str_ResultInfo = str_ResultInfo + "登陆密码修改成功！";
        		B_TempBuf.SetInt("X_RESULTCODE",i_ResultCode);
        		B_TempBuf.SetString("X_RESULTINFO",str_ResultInfo);
                delete TStaffPasswdCHG;
        	}                                                       //End if(str_SeqPasswdNumber == "GetSeqFail")
    		
        }                                                           //End if (bCheckResult == true)
        else
        {
            i_ResultCode = -1;
    		str_ResultInfo = str_ResultInfo + "本次修改的密码和前第[" + strPasswdChangeCount + "]次设置密码相同，请重新设置！";
    		B_TempBuf.SetInt("X_RESULTCODE",i_ResultCode);
    		B_TempBuf.SetString("X_RESULTINFO",str_ResultInfo);
            delete TStaffPasswdCHG;
        }
		//END Add By Zhu_ZhiMin On : 2006-05-12
	}
	catch(Exception &ex)
	{
	    delete TStaffPasswdCHG;
		THROW_AC(BaseException,ex,CHGPASSWD_ERR,"员工密码修改出错：");
	}
	return B_TempBuf;
}
/********************************************************************
*函数：		StaffPasswdChange(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		提供流程管理系统相应地接口
********************************************************************/
extern "C" void StaffPasswdChange(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	bool b_updateresule = false;
	string staffid = inBuf.GetString("STAFF_ID");
    string newPasswd = inBuf.GetString("PASSWD");
	StaffPasswdCHG * TStaffPasswdCHG = new StaffPasswdCHG(staffid,newPasswd);
	try
	{
		outBuf = TStaffPasswdCHG->StaffPasswdChange();
	}
	catch(Exception &ex)
	{
		outBuf.SetInt("X_RESULTCODE",ex.getCode());
		outBuf.SetString("X_RESULTINFO","密码修改业务处理时资料获取出错:"+ex.toString());
	}
	delete TStaffPasswdCHG;
	return;
}
