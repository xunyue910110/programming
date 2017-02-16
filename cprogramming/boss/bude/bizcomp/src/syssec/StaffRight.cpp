#include "StaffRight.h"

using namespace std;
using namespace UDBI;

/********************************************************************
*函数：		SetTempRightTimes(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		接口函数
*返回：		FML域
********************************************************************/
extern "C" int SetTempRightTimes(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
    StaffRight * TStaffRight = new StaffRight();
	try
	{
		string str_Staffid;
		string str_RightType;
		string str_RightCode;
		str_Staffid = inBuf.GetString("STAFF_ID");
		str_RightType = inBuf.GetString("RIGHT_TYPE");
		str_RightCode = inBuf.GetString("RIGHT_CODE");
		if(str_RightType == "0")
		{
			outBuf = TStaffRight->SetTempRightTimes(str_Staffid,str_RightCode);
		}
		else if(str_RightType == "1")
		{
			string str_DataType;
			str_DataType = inBuf.GetString("DATA_TYPE");
			outBuf = TStaffRight->SetTempRightTimes(str_Staffid,str_RightCode,str_DataType);
		}
	}
	catch(Exception &ex)
	{
		outBuf.SetInt("X_RESULTCODE",ex.getCode());
		outBuf.SetString("X_RESULTINFO","修改权限出错"+ex.toString());
		outBuf.SetInt("X_RECORDNUM",0);
	}
	
	delete TStaffRight;
	
	return 0;
}
/********************************************************************
*函数：		GetStaffRight(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		接口函数
*返回：		FML域
********************************************************************/
extern "C" int GetStaffRight(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	int i_GetMode;
	string str_TradeEparchy;
	string str_TradeStaffid;
	string str_SubSysCode;
	i_GetMode = inBuf.GetInt("X_GETMODE");
	str_TradeStaffid = inBuf.GetString("TRADE_STAFF_ID");
	str_TradeEparchy = inBuf.GetString("TRADE_EPARCHY_CODE");
		
	if(i_GetMode == 0)
	{
		str_SubSysCode = inBuf.GetString("SUBSYS_CODE");
	}
	StaffRight * TStaffRight = new StaffRight();
	try
	{
		outBuf = TStaffRight->GetStaffRight(i_GetMode,str_TradeStaffid,str_SubSysCode,str_TradeEparchy);
	}
	catch(Exception &ex)
	{
		outBuf.SetInt("X_RESULTCODE",ex.getCode());
		outBuf.SetString("X_RESULTINFO","获取权限报错："+ex.toString());
		outBuf.SetInt("X_RECORDNUM",0);
	}
	delete TStaffRight;
	return 0;
}
/********************************************************************
*函数：		StaffRight()
*功能：		构造函数
********************************************************************/
StaffRight::StaffRight()
{
	pcon = jdb::getConnect();
}
/********************************************************************
*函数：		~StaffRight()
*功能：		析构函数
********************************************************************/
StaffRight::~StaffRight()
{
	
}
/********************************************************************
*函数：		GetStaffRight()
*功能:		权限获取控制函数
*参数：		GetMode：								员工权限类型
					TradeStaffid：					员工编码
					SubSysCode：						子系统编码
*返回：		FML域结果
********************************************************************/
CFmlBuf StaffRight::GetStaffRight(const int GetMode,const string TradeStaffid,const string SubSysCode,const string TradeEparchyCode)
{
	string str_temp;
	i_GetMode = GetMode;
	str_TradeStaffid = TradeStaffid;
	str_TradeEparchyCode = TradeEparchyCode;
	str_SubSysCode = SubSysCode;
	JTable * TGetStaffRight = pcon->CreateJTable();
	try
	{	
		switch(i_GetMode)
		{
			case 0:B_TempBuf = StaffGUIMenuRight(TGetStaffRight);
						 break;  	
			case 1:B_TempBuf = StaffCommFuncRight(TGetStaffRight);
						 break;
			case 2:B_TempBuf = StaffSysFuncRight(TGetStaffRight);
						 break;
			case 3:B_TempBuf = StaffFeeDataRight(TGetStaffRight);
						 break;
			case 4:B_TempBuf = StaffSysDataRight(TGetStaffRight);
						 break;
			case 5:B_TempBuf = StaffTempFuncRight(TGetStaffRight);
						 break;
			case 6:B_TempBuf = StaffTempSysDateRight(TGetStaffRight);
						 break;
			case 7:B_TempBuf = SysTagInfo(TGetStaffRight);
						 break;
			case 8:B_TempBuf = SubSysInfo(TGetStaffRight);
						 break;
		    case 9:B_TempBuf = StaffQrgInfo(TGetStaffRight);
		                 break;
			default:
						 B_TempBuf.SetInt("X_RESULTCODE",-1);
						 B_TempBuf.SetString("X_RESULTINFO","GETMODE的范围在（0～9）内！");
						 B_TempBuf.SetInt("X_RECORDNUM",0);
		}	
		delete TGetStaffRight;
	}		
	catch(Exception &ex)
	{
	    delete TGetStaffRight;
		THROW_AC(BaseException,ex,GETSTAFFRIGHT_ERR,"获取权限报错：");
	}
	return B_TempBuf;
}
/********************************************************************
*函数：		StaffGUIMenuRight
*功能：		GUI前台菜单权限
*返回：		权限FML域
********************************************************************/
CFmlBuf StaffRight::StaffGUIMenuRight(JTable * TGetStaffRight)
{
	int i = 0;
	CFmlBuf B_Buf;
	TGetStaffRight->Clear();
	TGetStaffRight->AddSql("SELECT a.menu_id MENUID,NVL(a.parent_menu_id,' ') PARENTMENUID,NVL(a.right_code,' ') RIGHTCODE,");
	TGetStaffRight->AddSql("NVL(a.subsys_code,' ') SUBSYSCODE,NVL(a.menu_title,' ') MENUTITLE,NVL(a.menu_text,' ') MENUTEXT,");
	TGetStaffRight->AddSql("NVL(a.Image_index,-1) IMAGEINDEX,NVL(a.show_order,-1) SHOWORDER,");
	TGetStaffRight->AddSql("NVL(a.toolbar_index,-1) TOOLBARINDEX,NVL(a.short_key,' ') SHORTKEY,NVL(a.class_level,1) CLASSLEVEL ");
	TGetStaffRight->AddSql("FROM td_b_systemguimenu a ");
	TGetStaffRight->AddSql("WHERE a.menu_id NOT IN (SELECT menu_id FROM td_b_hidemenuitem ");
	TGetStaffRight->AddSql("WHERE staff_id =:STAFFID) ");
	TGetStaffRight->AddSql("AND (subsys_code=:SUBSYSCODE OR subsys_code='PUB') ");
	TGetStaffRight->SetParam(":STAFFID",str_TradeStaffid);
	TGetStaffRight->SetParam(":SUBSYSCODE",str_SubSysCode);
	TGetStaffRight->ExecSelect();		
	while(TGetStaffRight->Fetch())
	{
		B_Buf.SetString("MENU_ID",TGetStaffRight->GetString("MENUID"),i);
		B_Buf.SetString("PARENT_MENU_ID",TGetStaffRight->GetString("PARENTMENUID"),i);
		B_Buf.SetString("RIGHT_CODE",TGetStaffRight->GetString("RIGHTCODE"),i);
		B_Buf.SetString("SUBSYS_CODE",TGetStaffRight->GetString("SUBSYSCODE"),i);
		B_Buf.SetString("MENU_TITLE",TGetStaffRight->GetString("MENUTITLE"),i);
		B_Buf.SetString("MENU_TEXT",TGetStaffRight->GetString("MENUTEXT"),i);
		B_Buf.SetInt("IMAGE_INDEX",TGetStaffRight->GetInt("IMAGEINDEX"),i);
		B_Buf.SetInt("SHOW_ORDER",TGetStaffRight->GetInt("SHOWORDER"),i);
		B_Buf.SetInt("TOOLBAR_INDEX",TGetStaffRight->GetInt("TOOLBARINDEX"),i);
		B_Buf.SetString("SHORT_KEY",TGetStaffRight->GetString("SHORTKEY"),i);
		B_Buf.SetInt("CLASS_LEVEL",TGetStaffRight->GetInt("CLASSLEVEL"),i);
		
		i++;
	}
	if(i == 0)
	{
		B_Buf.SetInt("X_RESULTCODE",0);
		B_Buf.SetString("X_RESULTINFO","No StaffGUIMenuRight!");
		B_Buf.SetInt("X_RECORDNUM",0);
		return B_Buf;
	}
	else
	{

		B_Buf.SetInt("X_RESULTCODE",0);
		B_Buf.SetString("X_RESULTINFO","StaffGUIMenuRight OK !");
		B_Buf.SetInt("X_RECORDNUM",i);
		return B_Buf;
	}
}

/********************************************************************
*函数：		StaffCommFuncRight
*功能：		获取员工普通权限
*返回：		权限FML域
********************************************************************/
CFmlBuf StaffRight::StaffCommFuncRight(JTable * TGetStaffRight)
{
	int i = 0;
	CFmlBuf B_Buf;
	if(str_TradeStaffid=="SUPERUSR")
	{
		TGetStaffRight->Clear();
		//TGetStaffRight->AddSql("SELECT NVL(a.right_code,' ') RIGHTCODE,m.mod_name MODNAME,m.mod_type MODTYPE ");
		
		//Modified By Zhu_ZhiMin On : 2006-05-21 Remark : 增加权限名称信息返回，以便相关业务变换菜单提示信息
		TGetStaffRight->AddSql("SELECT NVL(a.right_code,' ') RIGHTCODE,RIGHT_NAME, m.mod_name MODNAME,m.mod_type MODTYPE ");
		
		TGetStaffRight->AddSql("FROM td_m_funcright a,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE a.right_attr='0' ");
		TGetStaffRight->AddSql("AND a.mod_code=m.mod_code ");
		TGetStaffRight->AddSql("ORDER BY RIGHTCODE ");
		TGetStaffRight->ExecSelect();			
		while(TGetStaffRight->Fetch())
		{
			B_Buf.SetString("RIGHT_CODE",TGetStaffRight->GetString("RIGHTCODE"),i);
			
			B_Buf.SetString("RIGHT_NAME",TGetStaffRight->GetString("RIGHT_NAME"),i);
			
			B_Buf.SetString("MOD_NAME",TGetStaffRight->GetString("MODNAME"),i);
			B_Buf.SetString("MOD_TYPE",TGetStaffRight->GetString("MODTYPE"),i);
			i++;
		}
		if(i == 0)
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","No StaffCommFuncRight Sysoper!");
			B_Buf.SetInt("X_RECORDNUM",0);
			return B_Buf;
		}
		else
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","StaffCommFuncRight Sysoper OK !");
			B_Buf.SetInt("X_RECORDNUM",i);
			return B_Buf;
		}
	}
	else
	{
		TGetStaffRight->Clear();
		
		//Modified By Zhu_ZhiMin On : 2006-05-21 Remark : 增加权限名称信息返回
		//TGetStaffRight->AddSql("SELECT f.right_code RIGHTCODE,m.mod_name MODNAME,m.mod_type MODTYPE ");
		TGetStaffRight->AddSql("SELECT /*+INDEX(s PK_TF_M_STAFFFUNCRIGHT)*/ f.right_code RIGHTCODE, ");
	 	TGetStaffRight->AddSql("f.right_name RIGHT_NAME,m.mod_name MODNAME,m.mod_type MODTYPE ");
	 	
	 	TGetStaffRight->AddSql("FROM tf_m_stafffuncright s,td_m_funcright f,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE s.right_attr='0' ");
		TGetStaffRight->AddSql("AND s.right_tag='1' ");
		TGetStaffRight->AddSql("AND f.right_attr='0' ");
		TGetStaffRight->AddSql("AND m.mod_code=f.mod_code ");
		TGetStaffRight->AddSql("AND s.right_code=f.right_code ");
		TGetStaffRight->AddSql("AND (s.rsvalue1 IS NULL OR s.rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND s.staff_id=:1 ");
		
		TGetStaffRight->AddSql("UNION ");
		
		//Modified By Zhu_ZhiMin On : 2006-05-21 Remark : 增加权限名称信息返回
		//TGetStaffRight->AddSql("SELECT 	NVL(f.right_code,' ') RIGHTCODE,m.mod_name MODNAME,m.mod_type MODTYPE ");
		TGetStaffRight->AddSql("SELECT 	/*+INDEX(s PK_TF_M_STAFFFUNCRIGHT)*/ NVL(f.right_code,' ') RIGHTCODE, ");
		TGetStaffRight->AddSql("f.right_name RIGHT_NAME,m.mod_name MODNAME,m.mod_type MODTYPE ");
		
		TGetStaffRight->AddSql("FROM tf_m_stafffuncright s,td_m_funcright f,tf_m_rolefuncright r,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE s.right_attr='1' ");
		TGetStaffRight->AddSql("AND s.right_tag='1' ");
		TGetStaffRight->AddSql("AND s.right_code=r.role_code ");
		TGetStaffRight->AddSql("AND r.right_code=f.right_code ");
		TGetStaffRight->AddSql("AND m.mod_code=f.mod_code ");
		TGetStaffRight->AddSql("AND (r.rsvalue1 IS NULL OR r.rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND (s.rsvalue1 IS NULL OR s.rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND NOT EXISTS(SELECT 1 ");
		TGetStaffRight->AddSql("FROM tf_m_stafffuncright ");
		TGetStaffRight->AddSql("WHERE staff_id=s.staff_id ");
		TGetStaffRight->AddSql("AND right_code=r.right_code ");
		//Modified By Zhu_ZhiMin On : 2006-05-26 Remark : 注释下面条件(只要存在失效都不获取)
		//TGetStaffRight->AddSql("AND (rsvalue1 IS NULL OR rsvalue1 != '1') ");
		//TGetStaffRight->AddSql("AND right_tag='0') ");
		
		TGetStaffRight->AddSql("AND (right_tag='0' OR rsvalue1 = '1' )) ");
		TGetStaffRight->AddSql("AND s.staff_id=:2 ");
		
		//Add By Zhu_ZhiMin On : 2006-05-26 Remark : 增加临时菜单权限信息的获取
		TGetStaffRight->AddSql("UNION ");
		TGetStaffRight->AddSql("SELECT NVL(s.right_code,' ') RIGHTCODE, ");
    	TGetStaffRight->AddSql("f.right_name RIGHTNAME, ");
    	TGetStaffRight->AddSql("NVL(m.mod_name,' ') MODNAME,NVL(m.mod_type,' ') MODTYPE ");
    	TGetStaffRight->AddSql("FROM tf_m_stafftempfuncright s,td_m_funcright f,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE s.staff_id=:3 ");
		TGetStaffRight->AddSql(" AND s.right_code=f.right_code ");
		TGetStaffRight->AddSql(" AND f.mod_code=m.mod_code ");
		TGetStaffRight->AddSql(" AND ((s.use_tag='1' AND SYSDATE>=s.start_date AND SYSDATE<=s.end_date) ");
		TGetStaffRight->AddSql("      OR (s.use_tag='0' AND s.used_times<s.times)) ");
		TGetStaffRight->AddSql(" AND (s.rsvalue1 IS NULL OR s.rsvalue1 != '1') ");		
		
		TGetStaffRight->AddSql("UNION ");
		
		//TGetStaffRight->AddSql("SELECT f.right_code RIGHTCODE,m.mod_name MODNAME,m.mod_type MODTYPE ");
		TGetStaffRight->AddSql("SELECT f.right_code RIGHTCODE,f.right_name RIGHT_NAME,m.mod_name MODNAME,m.mod_type MODTYPE ");
		
		TGetStaffRight->AddSql("FROM td_m_funcright f,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE f.right_attr='0' ");
		TGetStaffRight->AddSql("AND f.class_code='PUB' ");  
		TGetStaffRight->AddSql("AND f.mod_code=m.mod_code ");
		TGetStaffRight->SetParam(":1",str_TradeStaffid);
		TGetStaffRight->SetParam(":2",str_TradeStaffid);
		TGetStaffRight->SetParam(":3",str_TradeStaffid);
		TGetStaffRight->ExecSelect();
		while(TGetStaffRight->Fetch())
		{
			B_Buf.SetString("RIGHT_CODE",TGetStaffRight->GetString("RIGHTCODE"),i);
			
			B_Buf.SetString("RIGHT_NAME",TGetStaffRight->GetString("RIGHT_NAME"),i);
			
			B_Buf.SetString("MOD_NAME",TGetStaffRight->GetString("MODNAME"),i);
			B_Buf.SetString("MOD_TYPE",TGetStaffRight->GetString("MODTYPE"),i);
			i++;
		}
		if(i == 0)
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","No StaffCommFuncRight Sysoper!");
			B_Buf.SetInt("X_RECORDNUM",0);
			return B_Buf;
		}
		else
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","StaffCommFuncRight Sysoper OK !");
			B_Buf.SetInt("X_RECORDNUM",i);
			return B_Buf;
		}
	}
}
/********************************************************************
*函数：		StaffSysFuncRight
*功能：		获取系统函数权限
*返回：		权限FML域
********************************************************************/
CFmlBuf StaffRight::StaffSysFuncRight(JTable * TGetStaffRight)
{
	int i = 0;
	CFmlBuf B_Buf;
	if(str_TradeStaffid=="SUPERUSR")
	{
		TGetStaffRight->Clear();
		TGetStaffRight->AddSql("SELECT 'SUPERUSR' STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id','SUPERUSR','',''),' ') STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(f.right_code,' ') RIGHTCODE,NVL(f.right_name,' ') RIGHTNAME, ");
		TGetStaffRight->AddSql("NVL(f.right_attr,' ') RIGHTATTR, ");
		TGetStaffRight->AddSql("DECODE(f.right_attr,'0','菜单权限','1','界面控件权限','2','系统特权',' ') B_RIGHTATTR, ");
		TGetStaffRight->AddSql("'1' RIGHTTAG, ");
		TGetStaffRight->AddSql("'有效' B_RIGHTTAG, ");
		TGetStaffRight->AddSql("NVL(s.extend_value1,' ') EXTENDVALUE1, ");
		TGetStaffRight->AddSql("NVL(s.extend_value2,0) EXTENDVALUE2, ");
		TGetStaffRight->AddSql("NVL(s.rsvalue1,' ') RSVALUE1, ");
		TGetStaffRight->AddSql("NVL(s.rsvalue2,' ') RSVALUE2, ");
		TGetStaffRight->AddSql("' ' ACCREDITTIME, ");
		TGetStaffRight->AddSql("' ' ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("' ' REMARK, ");
		TGetStaffRight->AddSql("NVL(m.mod_name,' ') MODNAME,NVL(m.mod_type,' ') MODTYPE,NVL(f.class_code,' ') CLASSCODE ");
		TGetStaffRight->AddSql("FROM tf_m_stafffuncright s,td_m_funcright f,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE (f.right_attr='1' OR f.right_attr='2' ) ");
		TGetStaffRight->AddSql("AND f.mod_code=m.mod_code(+) ");
		TGetStaffRight->AddSql("AND f.right_code=s.right_code ");
		TGetStaffRight->AddSql("AND s.staff_id='SUPERUSR' ");
		TGetStaffRight->AddSql("UNION ");
		TGetStaffRight->AddSql("SELECT 'SUPERUSR' STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id','SUPERUSR','',''),' ') STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(f.right_code,' ') RIGHTCODE,NVL(f.right_name,' ') RIGHTNAME, ");
		TGetStaffRight->AddSql("NVL(f.right_attr,' ') RIGHTATTR, ");
		TGetStaffRight->AddSql("DECODE(f.right_attr,'0','菜单权限','1','界面控件权限','2','系统特权',' ') B_RIGHTATTR, ");
		TGetStaffRight->AddSql("'1' RIGHTTAG, ");
		TGetStaffRight->AddSql("'有效' B_RIGHTTAG, ");
		TGetStaffRight->AddSql("' ' EXTENDVALUE1, ");
		TGetStaffRight->AddSql("0 EXTENDVALUE2, ");
		TGetStaffRight->AddSql("' ' RSVALUE1, ");
		TGetStaffRight->AddSql("' ' RSVALUE2, ");
		TGetStaffRight->AddSql("' ' ACCREDITTIME, ");
		TGetStaffRight->AddSql("' ' ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("' ' REMARK, ");
		TGetStaffRight->AddSql("NVL(m.mod_name,' ') MODNAME,NVL(m.mod_type,' ') MODTYPE,NVL(f.class_code,' ') CLASSCODE ");
		TGetStaffRight->AddSql("FROM td_m_funcright f,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE (f.right_attr='1' OR f.right_attr='2' ) ");
		TGetStaffRight->AddSql("AND f.mod_code=m.mod_code(+) ");
		TGetStaffRight->AddSql("AND NOT EXISTS(SELECT 1 FROM tf_m_stafffuncright ");
		TGetStaffRight->AddSql("WHERE right_code=f.right_code ");
		TGetStaffRight->AddSql("AND staff_id='SUPERUSR') ");
		TGetStaffRight->AddSql("ORDER BY RIGHTCODE ");
		TGetStaffRight->ExecSelect();			
		while(TGetStaffRight->Fetch())
		{
			B_Buf.SetString("STAFF_ID",TGetStaffRight->GetString("STAFFID"),i);
			B_Buf.SetString("STAFF_NAME",TGetStaffRight->GetString("STAFFNAME"),i);
			B_Buf.SetString("RIGHT_CODE",TGetStaffRight->GetString("RIGHTCODE"),i);
			B_Buf.SetString("RIGHT_NAME",TGetStaffRight->GetString("RIGHTNAME"),i);
			B_Buf.SetString("RIGHT_ATTR",TGetStaffRight->GetString("RIGHTATTR"),i);
			B_Buf.SetString("X_RIGHTATTR",TGetStaffRight->GetString("B_RIGHTATTR"),i);
			B_Buf.SetString("RIGHT_TAG",TGetStaffRight->GetString("RIGHTTAG"),i);
			B_Buf.SetString("X_RIGHTTAG",TGetStaffRight->GetString("B_RIGHTTAG"),i);
			B_Buf.SetString("EXTEND_VALUE1",TGetStaffRight->GetString("EXTENDVALUE1"),i);
			B_Buf.SetInt("EXTEND_VALUE2",TGetStaffRight->GetInt("EXTENDVALUE2"),i);
			B_Buf.SetString("RSVALUE1",TGetStaffRight->GetString("RSVALUE1"),i);
			B_Buf.SetString("RSVALUE2",TGetStaffRight->GetString("RSVALUE2"),i);
			B_Buf.SetString("ACCREDIT_TIME",TGetStaffRight->GetString("ACCREDITTIME"),i);
			B_Buf.SetString("X_ACCREDIT_STAFF_ID",TGetStaffRight->GetString("ACCREDITSTAFFNAME"),i);
			B_Buf.SetString("REMARK",TGetStaffRight->GetString("REMARK"),i);
			B_Buf.SetString("MOD_NAME",TGetStaffRight->GetString("MODNAME"),i);
			B_Buf.SetString("MOD_TYPE",TGetStaffRight->GetString("MODTYPE"),i);
			B_Buf.SetString("CLASS_CODE",TGetStaffRight->GetString("CLASSCODE"),i);
			
			i++;
		}
		if(i == 0)
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","No StaffSysFuncRight Sysoper!");
			B_Buf.SetInt("X_RECORDNUM",0);
			return B_Buf;
		}
		else
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","StaffSysFuncRight Sysoper OK !");
			B_Buf.SetInt("X_RECORDNUM",i);
			return B_Buf;
		}
	}
	else
	{
		TGetStaffRight->Clear();
		TGetStaffRight->AddSql("SELECT /*+INDEX(s PK_TF_M_STAFFFUNCRIGHT)*/ s.staff_id STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',RTRIM(s.staff_id),'',''),' ') STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(s.right_code,' ') RIGHTCODE ,NVL(f.right_name,' ') RIGHTNAME, ");
		TGetStaffRight->AddSql("NVL(f.right_attr,' ') RIGHTATTR, ");
		TGetStaffRight->AddSql("DECODE(f.right_attr,'0','菜单权限','1','界面控件权限','2','系统特权',' ') B_RIGHTATTR, ");
		TGetStaffRight->AddSql("NVL(s.right_tag,' ') RIGHTTAG, ");
		TGetStaffRight->AddSql("DECODE(s.right_tag,'0','无效','1','有效',' ') B_RIGHTTAG, ");
		TGetStaffRight->AddSql("NVL(s.extend_value1,' ') EXTENDVALUE1, ");
		TGetStaffRight->AddSql("NVL(s.extend_value2,0) EXTENDVALUE2, ");
		TGetStaffRight->AddSql("NVL(s.rsvalue1,' ') RSVALUE1,NVL(s.rsvalue2,' ') RSVALUE2, ");
		TGetStaffRight->AddSql("NVL(TO_CHAR(s.accredit_time,'yyyy/mm/dd hh24:mi:ss'),' ') ACCREDITTIME, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',RTRIM(s.accredit_staff_id),'',''),' ') ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("NVL(s.remark,' ') REMARK,NVL(m.mod_name,' ') MODNAME,NVL(m.mod_type,' ') MODTYPE,NVL(f.class_code,' ') CLASSCODE ");
		TGetStaffRight->AddSql("FROM tf_m_stafffuncright s,td_m_funcright f,td_s_modfile m ");
		TGetStaffRight->AddSql("WHERE s.right_attr='0' ");
		TGetStaffRight->AddSql("AND s.right_tag='1' ");
		TGetStaffRight->AddSql("AND f.mod_code=m.mod_code(+) ");
		TGetStaffRight->AddSql("AND s.right_code=f.right_code ");
		TGetStaffRight->AddSql("AND (f.right_attr='1' OR f.right_attr='2') ");
		TGetStaffRight->AddSql("AND (s.rsvalue1 IS NULL OR s.rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND s.staff_id=:1 ");
		TGetStaffRight->AddSql("UNION ");
		TGetStaffRight->AddSql("SELECT /*+INDEX(s PK_TF_M_STAFFFUNCRIGHT)*/ s.staff_id STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',RTRIM(s.staff_id),'',''),' ') STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(r.right_code,' ') RIGHTCODE ,NVL(f.right_name,' ') RIGHTNAME, ");
		TGetStaffRight->AddSql("NVL(f.right_attr,' ') RIGHTATTR, ");
		TGetStaffRight->AddSql("DECODE(f.right_attr,'0','菜单权限','1','界面控件权限','2','系统特权',' ') RIGHTATTR, ");
		TGetStaffRight->AddSql("NVL(s.right_tag,' ') RIGHTTAG, ");
		TGetStaffRight->AddSql("DECODE(s.right_tag,'0','无效','1','有效',' ') B_RIGHTTAG, ");
		TGetStaffRight->AddSql("NVL(r.extend_value1,' ') EXTENDVALUE1, ");
		TGetStaffRight->AddSql("NVL(r.extend_value2,0) EXTENDVALUE2, ");
		TGetStaffRight->AddSql("NVL(r.rsvalue1,' ') RSVALUE1, NVL(r.rsvalue2,' ') RSVALUE2, ");
		TGetStaffRight->AddSql("NVL(TO_CHAR(s.accredit_time,'yyyy/mm/dd hh24:mi:ss'),' ') ACCREDITTIME, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',rtrim(s.accredit_staff_id),'',''),' ') ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("NVL(s.remark,' ') REMARK,NVL(m.mod_name,' ') MODNAME,NVL(m.mod_type,' ') MODTYPE,NVL(f.class_code,' ') CLASSCODE ");
		TGetStaffRight->AddSql("FROM tf_m_stafffuncright s,tf_m_rolefuncright r,td_s_modfile m,td_m_funcright f ");
		TGetStaffRight->AddSql("WHERE s.right_attr='1' ");
		TGetStaffRight->AddSql("AND s.right_tag='1' ");
		TGetStaffRight->AddSql("AND s.right_code=r.role_code ");
		TGetStaffRight->AddSql("AND r.right_code=f.right_code ");
		TGetStaffRight->AddSql("AND f.mod_code=m.mod_code(+) ");
		TGetStaffRight->AddSql("AND (f.right_attr='1' OR f.right_attr='2') ");
		TGetStaffRight->AddSql("AND (r.rsvalue1 IS NULL OR r.rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND (s.rsvalue1 IS NULL OR s.rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND NOT EXISTS(SELECT 1 FROM tf_m_stafffuncright ");
		TGetStaffRight->AddSql("WHERE staff_id=s.staff_id ");
		//Modified By Zhu_ZhiMin On : 2006-05-26 Remark : 注释下面条件，单个权限优先级高，先获取了
		//TGetStaffRight->AddSql("AND (rsvalue1 IS NULL OR rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND right_code=r.right_code) ");
		TGetStaffRight->AddSql("AND s.staff_id=:2 ");
		TGetStaffRight->AddSql("ORDER BY RIGHTCODE ");
		TGetStaffRight->SetParam(":1",str_TradeStaffid);
		TGetStaffRight->SetParam(":2",str_TradeStaffid);
		TGetStaffRight->ExecSelect();
		while(TGetStaffRight->Fetch())
		{
			B_Buf.SetString("STAFF_ID",TGetStaffRight->GetString("STAFFID"),i);
			B_Buf.SetString("STAFF_NAME",TGetStaffRight->GetString("STAFFNAME"),i);
			B_Buf.SetString("RIGHT_CODE",TGetStaffRight->GetString("RIGHTCODE"),i);
			B_Buf.SetString("RIGHT_NAME",TGetStaffRight->GetString("RIGHTNAME"),i);
			B_Buf.SetString("RIGHT_ATTR",TGetStaffRight->GetString("RIGHTATTR"),i);
			B_Buf.SetString("X_RIGHTATTR",TGetStaffRight->GetString("B_RIGHTATTR"),i);
			B_Buf.SetString("RIGHT_TAG",TGetStaffRight->GetString("RIGHTTAG"),i);
			B_Buf.SetString("X_RIGHTTAG",TGetStaffRight->GetString("B_RIGHTTAG"),i);
			B_Buf.SetString("EXTEND_VALUE1",TGetStaffRight->GetString("EXTENDVALUE1"),i);
			B_Buf.SetInt("EXTEND_VALUE2",TGetStaffRight->GetInt("EXTENDVALUE2"),i);
			B_Buf.SetString("RSVALUE1",TGetStaffRight->GetString("RSVALUE1"),i);
			B_Buf.SetString("RSVALUE2",TGetStaffRight->GetString("RSVALUE2"),i);
			B_Buf.SetString("ACCREDIT_TIME",TGetStaffRight->GetString("ACCREDITTIME"),i);
			B_Buf.SetString("X_ACCREDIT_STAFF_ID",TGetStaffRight->GetString("ACCREDITSTAFFNAME"),i);
			B_Buf.SetString("REMARK",TGetStaffRight->GetString("REMARK"),i);
			B_Buf.SetString("MOD_NAME",TGetStaffRight->GetString("MODNAME"),i);
			B_Buf.SetString("MOD_TYPE",TGetStaffRight->GetString("MODTYPE"),i);
			B_Buf.SetString("CLASS_CODE",TGetStaffRight->GetString("CLASSCODE"),i);
			i++;
		}
		if(i == 0)
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","No StaffSysFuncRight Sysoper!");
			B_Buf.SetInt("X_RECORDNUM",0);
			return B_Buf;
		}
		else
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","StaffSysFuncRight Sysoper OK !");
			B_Buf.SetInt("X_RECORDNUM",i);
			return B_Buf;
		}
	}
}
/********************************************************************
*函数：		StaffFeeDataRight
*功能：		获取员工费用权限
*返回：		权限FML域
********************************************************************/
CFmlBuf StaffRight::StaffFeeDataRight(JTable * TGetStaffRight)
{
	int i = 0;
	CFmlBuf B_Buf;
	return B_Buf;
}
/********************************************************************
*函数：		StaffSysDataRight
*功能：		获取员工系统数据权限
*返回：		权限FML域
********************************************************************/
CFmlBuf StaffRight::StaffSysDataRight(JTable * TGetStaffRight)
{
	int i = 0;
	CFmlBuf B_Buf;
	if(str_TradeStaffid=="SUPERUSR")
	{
		TGetStaffRight->Clear();
		TGetStaffRight->AddSql("SELECT 'SUPERUSR' STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id','SUPERUSR','',''),' ') STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(d.data_code,' ') DATACODE,NVL(d.data_name,' ') DATANAME, ");
		TGetStaffRight->AddSql("NVL(d.data_type,' ') DATATYPE, ");
		//TGetStaffRight->AddSql("DECODE(d.data_type,'0','资源权限','1','数据特权',' ') B_DATATYPE, ");
		
		//Modified By Zhu_ZhiMin On : 2006-11-02 Remark :  增加网别权限
		TGetStaffRight->AddSql("DECODE(d.data_type,'0','资源权限','1','数据特权','9','网别权限',' ') B_DATATYPE, ");
		TGetStaffRight->AddSql("'1' RIGHTTAG,'有效' B_RIGHTTAG, ");
		TGetStaffRight->AddSql("NVL(s.right_class,'5') RIGHTCLASS, ");
		TGetStaffRight->AddSql("s.oper_special OPERSPECIAL, ");
		TGetStaffRight->AddSql("s.rsvalue1 RSVALUE1, s.rsvalue2 RSVALUE2, ");
		TGetStaffRight->AddSql("' ' ACCREDITTIME, ");
		TGetStaffRight->AddSql("' ' ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("' ' REMARK,d.class_code CLASSCODE ");
		TGetStaffRight->AddSql("FROM td_m_dataright d,tf_m_staffdataright s ");
		//TGetStaffRight->AddSql("WHERE (d.data_type='0' OR d.data_type='1') ");
		TGetStaffRight->AddSql("WHERE (d.data_type='0' OR d.data_type='1' OR d.data_type='9') ");
		TGetStaffRight->AddSql("AND d.data_code=s.data_code ");
		TGetStaffRight->AddSql("AND d.data_type=s.data_type ");
		TGetStaffRight->AddSql("AND s.staff_id='SUPERUSR' ");
		TGetStaffRight->AddSql("union ");
		TGetStaffRight->AddSql("SELECT 'SUPERUSR' STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id','SUPERUSR','',''),' ') STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(d.data_code,' ') DATACODE,NVL(d.data_name,' ') DATANAME, ");
		TGetStaffRight->AddSql("NVL(d.data_type,' ') DATATYPE, ");
		TGetStaffRight->AddSql("DECODE(d.data_type,'0','资源权限','1','数据特权','9','网别权限',' ') B_DATATYPE, ");
		TGetStaffRight->AddSql("'1' RIGHTTAG,'有效' B_RIGHTTAG, ");
		TGetStaffRight->AddSql("'5' RIGHTCLASS, ");
		TGetStaffRight->AddSql("'2' OPERSPECIAL, ");
		TGetStaffRight->AddSql("' ' RSVALUE1, ' ' RSVALUE2, ");
		TGetStaffRight->AddSql("' ' ACCREDITTIME, ");
		TGetStaffRight->AddSql("' ' ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("' ' REMARK,d.class_code CLASSCODE ");
		TGetStaffRight->AddSql("FROM td_m_dataright d ");
		TGetStaffRight->AddSql("WHERE d.data_type='0' OR d.data_type='1' OR d.data_type='9' ");
		TGetStaffRight->AddSql("AND NOT EXISTS (SELECT 1 FROM tf_m_staffdataright ");
		TGetStaffRight->AddSql("WHERE data_code=d.data_code ");
		TGetStaffRight->AddSql("AND data_type=d.data_type ");
		TGetStaffRight->AddSql("AND staff_id='SUPERUSR') ");
		TGetStaffRight->AddSql("ORDER BY DATACODE ");
		TGetStaffRight->ExecSelect();			
		while(TGetStaffRight->Fetch())
		{
			B_Buf.SetString("STAFF_ID",TGetStaffRight->GetString("STAFFID"),i);
			B_Buf.SetString("STAFF_NAME",TGetStaffRight->GetString("STAFFNAME"),i);
			B_Buf.SetString("DATA_CODE",TGetStaffRight->GetString("DATACODE"),i);
			B_Buf.SetString("DATA_NAME",TGetStaffRight->GetString("DATANAME"),i);
			B_Buf.SetString("DATA_TYPE",TGetStaffRight->GetString("DATATYPE"),i);
			B_Buf.SetString("X_DATATYPE",TGetStaffRight->GetString("B_DATATYPE"),i);
			B_Buf.SetString("RIGHT_TAG",TGetStaffRight->GetString("RIGHTTAG"),i);
			B_Buf.SetString("X_RIGHTTAG",TGetStaffRight->GetString("B_RIGHTTAG"),i);
			B_Buf.SetString("RIGHT_CLASS",TGetStaffRight->GetString("RIGHTCLASS"),i);
			B_Buf.SetString("OPER_SPECIAL",TGetStaffRight->GetString("OPERSPECIAL"),i);
			B_Buf.SetString("RSVALUE1",TGetStaffRight->GetString("RSVALUE1"),i);
			B_Buf.SetString("RSVALUE2",TGetStaffRight->GetString("RSVALUE2"),i);
			B_Buf.SetString("ACCREDIT_TIME",TGetStaffRight->GetString("ACCREDITTIME"),i);
			B_Buf.SetString("X_ACCREDIT_STAFF_ID",TGetStaffRight->GetString("ACCREDITSTAFFNAME"),i);
			B_Buf.SetString("REMARK",TGetStaffRight->GetString("REMARK"),i);
			B_Buf.SetString("CLASS_CODE",TGetStaffRight->GetString("CLASSCODE"),i);
			i++;
		}
		if(i == 0)
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","No StaffSysDataRight Sysoper!");
			B_Buf.SetInt("X_RECORDNUM",0);
			return B_Buf;
		}
		else
		{
			B_Buf.SetInt("X_RESULTCODE",0);
			B_Buf.SetString("X_RESULTINFO","StaffSysDataRight Sysoper OK !");
			B_Buf.SetInt("X_RECORDNUM",i);
			return B_Buf;
		}
	}
	else
	{
		TGetStaffRight->Clear();
		TGetStaffRight->AddSql("SELECT s.staff_id STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',RTRIM(s.staff_id),'',''),' ')STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(s.data_code,' ') DATACODE,NVL(d.data_name,' ') DATANAME, ");
		TGetStaffRight->AddSql("NVL(s.data_type,' ') DATATYPE, ");
		//TGetStaffRight->AddSql("DECODE(s.data_type,'0','资源权限','1','数据特权','2','产品权限','3','资费权限','4','服务权限',' ') B_DATATYPE, ");
		TGetStaffRight->AddSql("DECODE(s.data_type,'0','资源权限','1','数据特权','2','产品权限','3','资费权限','4','服务权限','9','网别权限',' ') B_DATATYPE, ");
		TGetStaffRight->AddSql("NVL(s.right_tag,' ') RIGHTTAG, ");
		TGetStaffRight->AddSql("DECODE(s.right_tag,'0','无效','1','有效',' ') B_RIGHTTAG, ");
		TGetStaffRight->AddSql("NVL(s.right_class,' ') RIGHTCLASS, ");
		TGetStaffRight->AddSql("NVL(s.oper_special,' ') OPERSPECIAL, ");
		TGetStaffRight->AddSql("NVL(s.rsvalue1,' ') RSVALUE1, NVL(s.rsvalue2,' ') RSVALUE2, ");
		TGetStaffRight->AddSql("NVL(TO_CHAR(s.accredit_time,'yyyy/mm/dd hh24:mi:ss'),' ') ACCREDITTIME, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',rtrim(s.accredit_staff_id),'',''),' ') ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("NVL(s.remark,' ') REMARK,d.class_code CLASSCODE ");
		TGetStaffRight->AddSql("FROM tf_m_staffdataright s,td_m_dataright d ");
		TGetStaffRight->AddSql("WHERE s.right_attr='0' ");
		TGetStaffRight->AddSql("AND s.right_tag='1' ");
		TGetStaffRight->AddSql("AND s.data_code=d.data_code ");
		TGetStaffRight->AddSql("AND (s.data_type='0' OR s.data_type='1' OR s.data_type='9') ");
		TGetStaffRight->AddSql("AND (s.rsvalue1 IS NULL OR s.rsvalue1 != '1') ");
		TGetStaffRight->AddSql("AND s.data_type=d.data_type ");
		TGetStaffRight->AddSql("AND s.staff_id=:1 ");
		TGetStaffRight->AddSql("UNION ");
		TGetStaffRight->AddSql("SELECT s.staff_id STAFFID, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',RTRIM(s.staff_id),'',''),' ') STAFFNAME, ");
		TGetStaffRight->AddSql("NVL(r.data_code,' ') DATACODE,NVL(d.data_name,' ') DATANAME, ");
		TGetStaffRight->AddSql("NVL(s.data_type,' ') DATATYPE, ");
		TGetStaffRight->AddSql("DECODE(s.data_type,'0','资源权限','1','数据特权','2','产品权限','3','资费权限','4','服务权限','9','网别权限',' ') B_DATATYPE, ");
		TGetStaffRight->AddSql("NVL(s.right_tag,' ') RIGHTTAG, ");
		TGetStaffRight->AddSql("DECODE(s.right_tag,'0','无效','1','有效',' ') B_RIGHTTAG, ");
		TGetStaffRight->AddSql("NVL(r.right_class,' ') RIGHTCLASS, ");
		TGetStaffRight->AddSql("NVL(r.oper_special,' ') OPERSPECIAL, ");
		TGetStaffRight->AddSql("NVL(r.rsvalue1,' ') RSVALUE1,NVL(r.rsvalue2,' ') RSVALUE2, ");
		TGetStaffRight->AddSql("NVL(TO_CHAR(s.accredit_time,'yyyy/mm/dd hh24:mi:ss'),' ') ACCREDITTIME, ");
		TGetStaffRight->AddSql("NVL(f_sys_getcodename('staff_id',RTRIM(s.accredit_staff_id),'',''),' ') ACCREDITSTAFFNAME, ");
		TGetStaffRight->AddSql("NVL(s.remark,' ') REMARK,d.class_code CLASSCODE ");
		TGetStaffRight->AddSql("FROM tf_m_staffdataright s,td_m_dataright d,tf_m_roledataright r ");
		TGetStaffRight->AddSql("WHERE s.right_attr='1' ");
		TGetStaffRight->AddSql("AND s.right_tag='1' ");
		TGetStaffRight->AddSql("AND s.data_code=r.role_c