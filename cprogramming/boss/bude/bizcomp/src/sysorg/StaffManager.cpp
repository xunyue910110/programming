#include "StaffManager.h"

/**********************************************************************************
*函数：		GetStaffInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		员工资料管理查询接口函数
**********************************************************************************/
extern "C" int GetStaffInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	int i_GetModeCode;
	StaffManager * TStaffManager = new StaffManager();
	try
	{
		i_GetModeCode = inBuf.GetInt("X_GETMODE");
		ORG_TdMStaff T_Staff;
		switch(i_GetModeCode)
		{
			case 0:
			{
				T_Staff.DimissionTag = "0";
				break;
			}
			case 1:
			{
				T_Staff.StaffId = inBuf.GetString("STAFF_ID");
				break;
			}
			case 2:
			{
				T_Staff.DimissionTag = "0";
				T_Staff.AreaCode=inBuf.GetString("AREA_CODE");
				break;
			}
			case 3:
			{
				T_Staff.DimissionTag = "0";
				T_Staff.DepartId=inBuf.GetString("DEPART_ID");
				break;
			}
			case 4:
			{
				T_Staff.DimissionTag = "0";
				T_Staff.StartStaffId = inBuf.GetString("X_START_STAFF_ID");
				T_Staff.EndStaffId = inBuf.GetString("X_END_STAFF_ID");
				break;
			}
			case 5:
			{
				T_Staff.DimissionTag = "0";
				T_Staff.DepartId = inBuf.GetString("DEPART_ID");
				T_Staff.ValidFlag = "0";
				break;
			}
			case 6:
			{
				T_Staff.DimissionTag = "0";
				T_Staff.Rsvalue2 = inBuf.GetString("EPARCHY_CODE");
				T_Staff.ValidFlag = "0";
				break;
			}
			case 7:
			{
				T_Staff.DimissionTag = "0";
				T_Staff.StartStaffId = inBuf.GetString("X_START_STAFF_ID");
				T_Staff.EndStaffId = inBuf.GetString("X_END_STAFF_ID");
				T_Staff.AreaCode = inBuf.GetString("AREA_CODE");
				T_Staff.ValidFlag = "0";
				break;
			}
			default:
			{
				THROW_C(BaseException,STAFF_COM_0,"获取员工信息选择参数出错！");
			}
		}
		outBuf = TStaffManager->GetStaffInfo(T_Staff,i_GetModeCode);
		outBuf.SetString("X_RESULTINFO","获取员工资料成功");
		delete TStaffManager;
		return 0;
	}
	catch(Exception &e)
	{
		outBuf.SetInt("X_RESULTCODE",e.getCode());
		outBuf.SetString("X_RESULTINFO","获取员工资料出错："+e.toString());
		delete TStaffManager;
		return -1;
	}
}
/**********************************************************************************
*函数：		SaveStaffInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		员工资料管理修改接口函数
**********************************************************************************/
extern "C" int SaveStaffInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
{	
/*	int i_inrecord;
	int i_SaveModeCode;
	int i_result = -1;
	int i_SuccResult = 0;
	
	try
	{
		i_inrecord = inBuf.GetInt("INRECORDS");
		StaffManager * TStaffManager = new StaffManager();
		ORG_TdMStaff T_Staff;
		TfFCustomer T_Customer;
		TfFCustomerDAO * D_TfFCustomerDAO = new TfFCustomerDAO();
			
		for(int i = 0;i < i_inrecord;i++)
		{
			T_Staff.ORG_init();
			inBuf.GetValueEx("STAFFID",(void *)&(T_Staff.StaffId),i,true);
			inBuf.GetValueEx("DEPARTID",(void *)&(T_Staff.DepartId),i,false); 
			inBuf.GetValueEx("STAFFNAME",(void *)&(T_Staff.StaffName),i,false); 
			inBuf.GetValueEx("JOBINFO",(void *)&(T_Staff.JobInfo),i,false); 
			inBuf.GetValueEx("MANAGERINFO",(void *)&(T_Staff.ManagerInfo),i,false); 
			inBuf.GetValueEx("SEX",(void *)&(T_Staff.Sex),i,false); 
			inBuf.GetValueEx("EMAIL",(void *)&(T_Staff.Email),i,false); 
			inBuf.GetValueEx("USERPID",(void *)&(T_Staff.UserPid),i,false); 
			inBuf.GetValueEx("SERIALNUMBER",(void *)&(T_Staff.SerialNumber),i,false); 
			inBuf.GetValueEx("CUSTID",(void *)&(T_Staff.CustId),i,false); 
			inBuf.GetValueEx("DIMISSIONTAG",(void *)&(T_Staff.DimissionTag),i,false);
			inBuf.GetValueEx("BIRTHDAY",(void *)&(T_Staff.Birthday),i,false); 
			inBuf.GetValueEx("STAFFGROUPID",(void *)&(T_Staff.StaffGroupId),i,false); 
			inBuf.GetValueEx("CUSTHOBYY",(void *)&(T_Staff.CustHobyy),i,false); 
			inBuf.GetValueEx("REMARK",(void *)&(T_Staff.Remark),i,false); 
			inBuf.GetValueEx("RSVALUE1",(void *)&(T_Staff.Rsvalue1),i,false); 
			inBuf.GetValueEx("RSVALUE2",(void *)&(T_Staff.Rsvalue2),i,false); 
			inBuf.GetValueEx("UPDATETIME",(void *)&(T_Staff.UpdateTime),i,false); 
			inBuf.GetValueEx("TRADESTAFFID",(void *)&(T_Staff.UpdateStaffId),i,false); 
			inBuf.GetValueEx("TRADEDEPARTID",(void *)&(T_Staff.UpdateDepartId),i,false); 
 
			inBuf.GetValueEx("DEPARTNAME",(void *)&(T_Staff.b_DepartId),i,false); 
			inBuf.GetValueEx("B_SEX",(void *)&(T_Staff.b_Sex),i,false); 
			inBuf.GetValueEx("B_DIMISSIONTAG",(void *)&(T_Staff.b_DimissionTag),i,false); 

			i_SaveModeCode = -1;
			i_SaveModeCode = inBuf.GetInt("SAVEMODECODE",i);
			i_result = TStaffManager->SaveStaffInfo(T_Staff,i_SaveModeCode);
			
			if(i_SaveModeCode == 2)
			{
				
				T_Customer.init();
				inBuf.GetValueEx("CUSTNAME",(void *)&(T_Customer.CustName),i,false);
				inBuf.GetValueEx("PSPTID",(void *)&(T_Customer.PsptId),i,false);
				inBuf.GetValueEx("TRADEEPARCHYCODE",(void *)&(T_Customer.EparchyCode),i,false);
				inBuf.GetValueEx("TRADECITYCODE",(void *)&(T_Customer.CityCode),i,false);
				inBuf.GetValueEx("TRADEDEPARTID",(void *)&(T_Customer.IndepartId),i,false);
				inBuf.GetValueEx("TRADESTAFFID",(void *)&(T_Customer.InStaffId),i,false);
				GetSeqID * T_GetSeqID = new GetSeqID("CUSTID","8");
				T_Customer.CustId = T_GetSeqID->GetDateSequencesId();
				delete T_GetSeqID;
				try
				{
					D_TfFCustomerDAO->jinsby(INS_STAFF_0,T_Customer);
				}
				catch(DAOException &e)
				{
					THROW_AC(DAOException,e,15223,"插入员工信息出错！");
				}
				if(i_result == 0)
				{
					i_SuccResult++;
				}
			}
		}	
		OperLog * T_OperLog = new OperLog();
		T_OperLog->InsertOrgmanlog(inBuf);
		outBuf.SetInt("RESULTCODE",0);
		outBuf.SetString("RESULTERRINFO","处理员工编码表成功！");
		outBuf.SetInt("RESULTRECORDS",i_SuccResult);
		delete TStaffManager;
		delete T_OperLog;
		delete D_TfFCustomerDAO;
		return 0;
	}
	catch(Exception &e)
	{
		outBuf.SetInt("RESULTCODE",e.getCode());
		outBuf.SetString("RESULTERRINFO","修改员工资料出错："+e.toString());
		outBuf.SetInt("RESULTRECORDS",0);
		return -1;
	}*/
	return 0;
}
/**********************************************************************************
*函数：		StaffManager()
*功能：		员工资料管理构造函数
**********************************************************************************/
StaffManager::StaffManager()
{
	pcon = jdb::getConnect();
	D_ORG_TdMStaffDAO = new ORG_TdMStaffDAO();
}
/**********************************************************************************
*函数：		~StaffManager()
*功能：		员工资料管理析构函数
**********************************************************************************/
StaffManager::~StaffManager()
{
	delete D_ORG_TdMStaffDAO;
}
/**********************************************************************************
*函数：		GetStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff,const int GetModeCode)
*功能：		员工资料查询控制函数
*参数：		T_ORG_TdMStaff								员工资料管理条件参数
					GetModeCode										员工资料查询类型编码
*返回：		查询结果FML
**********************************************************************************/
CFmlBuf StaffManager::GetStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff,const int GetModeCode)
{
	CFmlBuf B_Buf;
	int i = 0;
	VPORG_TdMStaff vpRetORG_TdMStaff;                                     
	                                                              
	switch (GetModeCode)                                          
	{                                                             
		case 0:                                                     
		{                                                           
			/*获取全部有效员工*/
			try                                                       
			{                                                         
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselby(SEL_STAFF_0,T_ORG_TdMStaff);
			}                                                         
			catch(Exception &e)                                    
			{                                                         
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取全部有效员工出错！");
			}
			break;
		}
		case 1:
		{
			/*获取指定员工编码的员工信息*/
			try
			{
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselby(SEL_STAFF_1,T_ORG_TdMStaff);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取指定员工编码的员工信息出错！");
			}
			break;
		}
		case 2:
		{
			/*获取指定地域编码的员工信息*/
			try
			{
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselby(SEL_STAFF_2,T_ORG_TdMStaff);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取指定地域编码的员工信息出错！");
			}
			break;
		}
		case 3:
		{
			/*获取指定部门/渠道编码下的员工信息*/
			try
			{
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselby(SEL_STAFF_3,T_ORG_TdMStaff);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取指定部门/渠道编码下的员工信息出错！");
			}
			break;
		}
		case 4:
		{
			/*获取指定编码范围的员工信息*/
			try
			{
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselect(SEL_STAFF_4,T_ORG_TdMStaff);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取指定编码范围的员工信息出错！");
			}
			break;
		}
		case 5:
		{
			/*获取指定部门/渠道的所有下级部门员工信息*/
			try
			{
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselby(SEL_STAFF_5,T_ORG_TdMStaff);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取指定部门/渠道的所有下级部门员工信息出错！");
			}
			break;
		}
		case 6:
		{
			/*获取指定地域(地州)所有员工信息*/
			try
			{
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselby(SEL_STAFF_6,T_ORG_TdMStaff);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取指定地域(地州)所有员工信息出错！");
			}
			break;
		}
		case 7:
		{
			/*获取指定编码范围的指定地域员工信息*/
			try
			{
				vpRetORG_TdMStaff = D_ORG_TdMStaffDAO->ORG_jselect(SEL_STAFF_7,T_ORG_TdMStaff);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,STAFF_DAO_S,"获取指定编码范围的指定地域员工信息出错！");
			}
			break;
		}
		default:
		{
			THROW_C(BaseException,STAFF_COM_0,"获取员工信息填入参数出错！");
		}
	}
	/*获取相应结果*/
	try
	{
		vector<ORG_TdMStaff>::const_iterator it = vpRetORG_TdMStaff.begin();
		for( ;it != vpRetORG_TdMStaff.end();it++)
		{
			B_Buf.SetString("STAFF_ID",(*it).StaffId,i); 
			B_Buf.SetString("DEPART_ID",(*it).DepartId,i); 
			B_Buf.SetString("STAFF_NAME",(*it).StaffName,i); 
			B_Buf.SetString("JOB_INFO",(*it).JobInfo,i); 
			B_Buf.SetString("MANAGER_INFO",(*it).ManagerInfo,i); 
			B_Buf.SetString("SEX",(*it).Sex,i); 
			B_Buf.SetString("EMAIL",(*it).Email,i); 
			B_Buf.SetString("USER_PID",(*it).UserPid,i); 
			B_Buf.SetString("SERIAL_NUMBER",(*it).SerialNumber,i); 
			B_Buf.SetString("CUST_ID",(*it).CustId,i); 
			B_Buf.SetString("DIMISSION_TAG",(*it).DimissionTag,i); 
			B_Buf.SetString("BIRTHDAY",(*it).Birthday,i); 
			B_Buf.SetInt("STAFF_GROUP_ID",(*it).StaffGroupId,i); 
			B_Buf.SetString("CUST_HOBYY",(*it).CustHobyy,i); 
			B_Buf.SetString("REMARK",(*it).Remark,i); 
			B_Buf.SetString("RSVALUE1",(*it).Rsvalue1,i); 
			B_Buf.SetString("RSVALUE2",(*it).Rsvalue2,i); 
			B_Buf.SetString("UPDATE_TIME",(*it).UpdateTime,i); 
			B_Buf.SetString("UPDATE_STAFF_ID",(*it).UpdateStaffId,i); 
			B_Buf.SetString("UPDATE_DEPART_ID",(*it).UpdateDepartId,i);
			
			B_Buf.SetString("DEPART_NAME",(*it).b_DepartId,i); 
			B_Buf.SetString("X_SEX",(*it).b_Sex,i); 
			B_Buf.SetString("X_DIMISSIONTAG",(*it).b_DimissionTag,i); 
			B_Buf.SetString("X_UPDATESTAFFID",(*it).b_UpdateStaffId,i); 
			B_Buf.SetString("X_UPDATEDEPARTID",(*it).b_UpdateDepartId,i); 
			
			i++;
		}	
		B_Buf.SetInt("X_RESULTCODE",0);
		B_Buf.SetInt("X_RECORDNUM",i);
		return B_Buf;
	}
	catch(DAOException &e)
	{
		THROW_AC(BaseException,e,STAFF_COM_1,GetModeCode+"方式获取指定员工信息赋值出错！");
	}
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int StaffManager::SaveStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff,const int SaveModeCode)
{
	int i_Result = -1;
	{
		switch(SaveModeCode)
		{
			case 1:
				{
					i_Result = UpdateStaffInfo(T_ORG_TdMStaff);
					break;
				}
			case 2:
				{
					i_Result = InsertStaffInfo(T_ORG_TdMStaff);
					break;
				}
			case 3:
				{
					T_ORG_TdMStaff.DimissionTag = "1";
					i_Result = DeleteStaffInfo(T_ORG_TdMStaff);
					break;
				}
			default:
				THROW_C(BaseException,STAFF_COM_0,SaveModeCode+"方式修改指定地洲信息赋值出错！");
		}
	}
	return i_Result;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int StaffManager::InsertStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff)
{
	try
	{
		D_ORG_TdMStaffDAO->ORG_jinsert(T_ORG_TdMStaff);
	}
	catch(Exception &e)
	{
		THROW_AC(BaseException,e,STAFF_DAO_I,"插入员工信息出错！");
	}
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int StaffManager::UpdateStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff)
{
	try
	{
		D_ORG_TdMStaffDAO->ORG_jupdate(UPD_UPD_STAFF_0,T_ORG_TdMStaff);
	}
	catch(Exception &e)
	{
		THROW_AC(BaseException,e,STAFF_DAO_U,"修改员工信息出错！");
	}
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int StaffManager::DeleteStaffInfo(ORG_TdMStaff &T_ORG_TdMStaff)
{
	try
	{
		D_ORG_TdMStaffDAO->ORG_jupdate(UPD_DEL_STAFF_0,T_ORG_TdMStaff);
	}
	catch(Exception &e)
	{
		THROW_AC(BaseException,e,STAFF_DAO_D,"删除员工信息出错！");
	}
	return 0;
}
