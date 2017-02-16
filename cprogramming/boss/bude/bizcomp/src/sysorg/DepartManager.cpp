#include "DepartManager.h"

/**********************************************************************************
*函数：		GetDepartInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		部门资料管理接口函数
**********************************************************************************/
extern "C" int GetDepartInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	int i_GetModeCode;
	DepartManager * TDepartManager = new DepartManager();
	try
	{
		i_GetModeCode = inBuf.GetInt("X_GETMODE");
		ORG_TdMDepart T_Depart;
		switch(i_GetModeCode)
		{
			case 0:
			{
				T_Depart.ValidFlag = "0";
				T_Depart.DepartKindCode = "0";
				break;
			}
			case 1:
			{
				T_Depart.DepartLevel = inBuf.GetInt("DEPART_LEVEL");
				T_Depart.AreaCode = inBuf.GetString("AREA_CODE");
				T_Depart.ValidFlag = "0";
				break;
			}
			case 2:
			{
				T_Depart.DepartId=inBuf.GetString("DEPART_ID");
				break;
			}
			case 3:
			{
				T_Depart.DepartId=inBuf.GetString("DEPART_ID");
				T_Depart.ValidFlag = "0";
				break;
			}
			case 4:
			{
				T_Depart.AreaCode=inBuf.GetString("AREA_CODE");
				T_Depart.ValidFlag = "0";
				break;
			}
			case 5:
			{
				T_Depart.UserDepartCode=inBuf.GetString("USER_DEPART_CODE");
				break;
			}
			case 6:
			{
				T_Depart.AreaCode=inBuf.GetString("AREA_CODE");
				T_Depart.ValidFlag = "0";
				T_Depart.DepartId = inBuf.GetString("DEPART_ID");
				break;
			}
			case 7:
			{
				T_Depart.AreaCode=inBuf.GetString("AREA_CODE");
				T_Depart.ValidFlag = "0";
				T_Depart.DepartId = inBuf.GetString("DEPART_ID");
				break;
			}
			case 8:
			{
				T_Depart.ValidFlag = "0";
				T_Depart.DepartId = inBuf.GetString("DEPART_ID");
				break;
			}
			case 9:
			{
				T_Depart.ValidFlag = "0";
				T_Depart.DepartKindCode = "0";
				break;
			}
			case 10:
			{
				T_Depart.ValidFlag = "0";
				T_Depart.Rsvalue2 = inBuf.GetString("TRADE_EPARCHY_CODE");
				break;
			}
			case 11:
			{
				T_Depart.DepartCode = inBuf.GetString("DEPART_CODE");
				break;
			}
			default:
			{
				THROW_C(BaseException,DEPART_COM_0,"获取部门信息选择参数出错！");
			}
		}
		outBuf = TDepartManager->GetDepartInfo(T_Depart,i_GetModeCode);
		outBuf.SetString("X_RESULTINFO","获取部门资料成功");
		delete TDepartManager;
		return 0;
	}
	catch(Exception &e)
	{
		outBuf.SetInt("X_RESULTCODE",e.getCode());
		outBuf.SetString("X_RESULTINFO","获取部门资料出错："+e.toString());
		delete TDepartManager;
		return -1;
	}
}
/**********************************************************************************
*函数：		SaveDepartInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		修改部门资料
**********************************************************************************/
extern "C" int SaveDepartInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
{	
/*	int i_inrecord;
	int i_SaveModeCode;
	int i_result = -1;
	int i_SuccResult = 0;
	try
	{
		i_inrecord = inBuf.GetInt("X_GETMODE");
		DepartManager * TDepartManager = new DepartManager();
		ORG_TdMDepart T_Depart;
		for(int i = 0;i < i_inrecord;i++)
		{
			inBuf.GetValueEx("DEPART_ID",(void *)&(T_Depart.DepartId),i,false);
			inBuf.GetValueEx("DEPART_CODE",(void *)&(T_Depart.DepartCode),i,false); 
			inBuf.GetValueEx("DEPART_NAME",(void *)&(T_Depart.DepartName),i,false); 
			inBuf.GetValueEx("DEPART_KIND_CODE",(void *)&(T_Depart.DepartKindCode),i,false); 
			inBuf.GetValueEx("X_DEPARTKINDCODE",(void *)&(T_Depart.b_DepartKindCode),i,false); 
			inBuf.GetValueEx("DEPARTFRAME",(void *)&(T_Depart.DepartFrame),i,false); 
			inBuf.GetValueEx("VALIDFLAG",(void *)&(T_Depart.ValidFlag),i,false); 
			inBuf.GetValueEx("X_VALIDFLAG",(void *)&(T_Depart.b_ValidFlag),i,false); 
			inBuf.GetValueEx("AREACODE",(void *)&(T_Depart.AreaCode),i,false); 
			inBuf.GetValueEx("X_AREACODE",(void *)&(T_Depart.b_AreaCode),i,false); 
			inBuf.GetValueEx("PARENTDEPARTID",(void *)&(T_Depart.ParentDepartId),i,false); 
			inBuf.GetValueEx("X_PARENTDEPARTID",(void *)&(T_Depart.b_ParentDepartId),i,false); 
			inBuf.GetValueEx("MANAGERID",(void *)&(T_Depart.ManagerId),i,false); 
			inBuf.GetValueEx("X_MANAGERID",(void *)&(T_Depart.b_ManagerId),i,false); 
			inBuf.GetValueEx("ORDERNO",(void *)&(T_Depart.OrderNo),i,false); 
			inBuf.GetValueEx("USERDEPARTCODE",(void *)&(T_Depart.UserDepartCode),i,false); 
			inBuf.GetValueEx("STARTDATE",(void *)&(T_Depart.StartDate),i,false); 
			inBuf.GetValueEx("ENDDATE",(void *)&(T_Depart.EndDate),i,false); 
			inBuf.GetValueEx("DEPARTLEVEL",(void *)&(T_Depart.DepartLevel),i,false); 
			inBuf.GetValueEx("REMARK",(void *)&(T_Depart.Remark),i,false); 
			inBuf.GetValueEx("RSVALUE1",(void *)&(T_Depart.Rsvalue1),i,false); 
			inBuf.GetValueEx("RSVALUE2",(void *)&(T_Depart.Rsvalue2),i,false); 
			inBuf.GetValueEx("RSVALUE3",(void *)&(T_Depart.Rsvalue3),i,false); 
			inBuf.GetValueEx("RSVALUE4",(void *)&(T_Depart.Rsvalue4),i,false); 
			inBuf.GetValueEx("UPDATETIME",(void *)&(T_Depart.UpdateTime),i,false); 
			inBuf.GetValueEx("TRADESTAFFID",(void *)&(T_Depart.UpdateStaffId),i,false); 
			inBuf.GetValueEx("TRADEDEPARTID",(void *)&(T_Depart.UpdateDepartId),i,false); 
			
			i_SaveModeCode = -1;
			i_SaveModeCode = inBuf.GetInt("SAVEMODECODE");
			i_result = TDepartManager->SaveDepartInfo(T_Depart,i_SaveModeCode);
			
			if(i_result == 0)
			{
				i_SuccResult++;
			}
		}
		
		OperLog * T_OperLog = new OperLog();
		T_OperLog->InsertOrgmanlog(inBuf);
	
		outBuf.SetInt("RESULTCODE",0);
		outBuf.SetString("RESULTERRINFO","处理部门编码表成功！");
		outBuf.SetInt("RESULTRECORDS",i_SuccResult);
		delete TDepartManager;
		delete T_OperLog;
		return 0;
	}
	catch(Exception &e)
	{
		outBuf.SetInt("RESULTCODE",e.getCode());
		outBuf.SetString("RESULTREEINFO","修改部门资料出错："+e.getMessage());
		return -1;
	}*/
	return 0;
}
/**********************************************************************************
*函数：		DepartManager()
*功能：		部门资料管理构造函数
**********************************************************************************/
DepartManager::DepartManager()
{
	pcon = jdb::getConnect();
	D_ORG_TdMDepartDAO = new ORG_TdMDepartDAO();
}
/**********************************************************************************
*函数：		~DepartManager()
*功能：		部门资料管理析构函数
**********************************************************************************/
DepartManager::~DepartManager()
{
	delete D_ORG_TdMDepartDAO;
}
/**********************************************************************************
*函数：		GetDepartInfo
*功能：		部门资料获取控制函数
*参数：		T_ORG_TdMDepart：							获取条件参数
					GetModeCode										获取资料类型编码
*返回：		资料FML域
**********************************************************************************/
CFmlBuf DepartManager::GetDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart,const int GetModeCode)
{
	CFmlBuf B_Buf;
	int i = 0;
	
	VPORG_TdMDepart vpRetORG_TdMDepart;
	
	switch (GetModeCode)
	{
		case 0:
		{
			/*获取全部有效部门*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselect(SEL_DEPART_0,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取全部有效部门出错！");
			}
			break;
		}
		case 1:
		{
			/*获取指定级别有效部门*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselect(SEL_DEPART_1,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定级别有效部门出错！");
			}
			break;
		}
		case 2:
		{
			/*获取指定部门/渠道编码的信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselby(SEL_DEPART_2,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定部门/渠道编码的信息出错！");
			}
			break;
		}
		case 3:
		{
			/*获取指定部门/渠道编码的所有下级信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselect(SEL_DEPART_3,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定部门/渠道编码的所有下级信息出错！");
			}
			break;
		}
		case 4:
		{
			/*获取指定地域的部门/渠道信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselect(SEL_DEPART_4,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定地域的部门/渠道信息出错！");
			}
			break;
		}
		case 5:
		{
			/*获取指定用户标识的部门/渠道信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselect(SEL_DEPART_5,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定用户标识的部门/渠道信息出错！");
			}
			break;
		}
		case 6:
		{
			/*获取指定地域下非指定代理商的部门信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselect(SEL_DEPART_6,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定地域下非指定代理商的部门信息出错！");
			}
			break;
		}
		case 7:
		{
			/*获取指定地域下指定部门的同级信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselect(SEL_DEPART_7,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S," 获取指定地域下指定部门的同级信息出错！");
			}
			break;
		}
		case 8:
		{
			/*获取指定部门的上级信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselby(SEL_DEPART_8,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定部门的上级信息出错！");
			}
			break;
		}
		case 9:
		{
			/*获取所有有效渠道信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselby(SEL_DEPART_9,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取所有有效渠道信息出错！");
			}
			break;
		}
		case 10:
		{
			/*获取指定地域(地州)下所有部门/渠道信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselby(SEL_DEPART_10,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定地域(地州)下所有部门/渠道信息出错！");
			}
			break;
		}
		case 11:
		{
			/*获取指定部门编码的部门/渠道信息*/
			try
			{
				vpRetORG_TdMDepart = D_ORG_TdMDepartDAO->ORG_jselby(SEL_DEPART_11,T_ORG_TdMDepart);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,DEPART_DAO_S,"获取指定部门编码的部门/渠道信息出错！");
			}
			break;
		}
		default:
		{
			THROW_C(BaseException,DEPART_COM_0,"获取部门信息填入参数出错！");
		}
	}
	/*获取相应地结果*/
	try
	{
		vector<ORG_TdMDepart>::const_iterator it = vpRetORG_TdMDepart.begin();
		for( ;it != vpRetORG_TdMDepart.end();it++)
		{
			B_Buf.SetString("DEPART_ID",(*it).DepartId,i); 
			B_Buf.SetString("DEPART_CODE",(*it).DepartCode,i); 
			B_Buf.SetString("DEPART_NAME",(*it).DepartName,i); 
			B_Buf.SetString("DEPART_KIND_CODE",(*it).DepartKindCode,i); 
			B_Buf.SetString("DEPART_FRAME",(*it).DepartFrame,i); 
			B_Buf.SetString("VALID_FLAG",(*it).ValidFlag,i); 
			B_Buf.SetString("AREA_CODE",(*it).AreaCode,i); 
			B_Buf.SetString("PARENT_DEPART_ID",(*it).ParentDepartId,i); 
 			B_Buf.SetString("MANAGER_ID",(*it).ManagerId,i); 
			B_Buf.SetInt("ORDER_NO",(*it).OrderNo,i); 
			B_Buf.SetString("USER_DEPART_CODE",(*it).UserDepartCode,i); 
			B_Buf.SetString("START_DATE",(*it).StartDate,i); 
			B_Buf.SetString("END_DATE",(*it).EndDate,i); 
			B_Buf.SetInt("DEPART_LEVEL",(*it).DepartLevel,i); 
			B_Buf.SetString("REMARK",(*it).Remark,i); 
			B_Buf.SetString("RSVALUE1",(*it).Rsvalue1,i); 
			B_Buf.SetString("RSVALUE2",(*it).Rsvalue2,i); 
			B_Buf.SetString("RSVALUE3",(*it).Rsvalue3,i); 
			B_Buf.SetString("RSVALUE4",(*it).Rsvalue4,i); 
			B_Buf.SetString("UPDATE_TIME",(*it).UpdateTime,i); 
			B_Buf.SetString("UPDATE_STAFF_ID",(*it).UpdateStaffId,i); 
			B_Buf.SetString("UPDATE_DEPART_ID",(*it).UpdateDepartId,i);
			
			B_Buf.SetString("X_DEPARTKINDCODE",(*it).b_DepartKindCode,i); 			
			B_Buf.SetString("X_AREACODE",(*it).b_AreaCode,i); 
			B_Buf.SetString("X_VALIDFLAG",(*it).b_ValidFlag,i);
			B_Buf.SetString("X_PARENTDEPARTID",(*it).b_ParentDepartId,i);
			B_Buf.SetString("X_MANAGERID",(*it).b_ManagerId,i); 
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
		THROW_C(BaseException,DEPART_COM_1,GetModeCode+"方式获取指定部门信息赋值出错！");
	}
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int DepartManager::SaveDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart,const int SaveModeCode)
{/*
	int i_Result;
	{
		switch(SaveModeCode)
		{
			case 1:
				{
					i_Result = UpdateDepartInfo(T_ORG_TdMDepart);
					break;
				}
			case 2:
				{
					string str_DepartNumber;
					GetSeqID * TGetSeqID = new GetSeqID("DEPARTID","5");
					str_DepartNumber = TGetSeqID->GetSequencesId();
					if(str_DepartNumber == "GetSeqFail")
					{
						delete TGetSeqID;
						return -1;
					}
					else
					{
						delete TGetSeqID;
					}
					T_ORG_TdMDepart.DepartId = str_DepartNumber;
					T_ORG_TdMDepart.DepartFrame = T_ORG_TdMDepart.DepartFrame + str_DepartNumber;
					i_Result = InsertDepartInfo(T_ORG_TdMDepart);
					break;
				}
			case 3:
				{
					i_Result = DeleteDepartInfo(T_ORG_TdMDepart);
					break;
				}
			default:
				THROW_C(BaseException,DEPART_COM_0,SaveModeCode+"方式修改指定地洲信息赋值出错！");
		}
	}
	return i_Result;*/
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int DepartManager::InsertDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart)
{
	try
	{
		D_ORG_TdMDepartDAO->ORG_jinsert(T_ORG_TdMDepart);
	}
	catch(DAOException &e)
	{
		THROW_AC(BaseException,e,DEPART_DAO_I,"插入部门信息出错！");
	}
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int DepartManager::UpdateDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart)
{
	try
	{
		D_ORG_TdMDepartDAO->ORG_jupdate(UPD_UPD_DEPART_0,T_ORG_TdMDepart);
	}
	catch(DAOException &e)
	{
		THROW_AC(BaseException,e,DEPART_DAO_U,"修改部门信息出错！");
	}
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int DepartManager::DeleteDepartInfo(ORG_TdMDepart &T_ORG_TdMDepart)
{
	try
	{
		D_ORG_TdMDepartDAO->ORG_jupdate(UPD_DEL_DEPART_0,T_ORG_TdMDepart);
	}
	catch(DAOException &e)
	{
		THROW_AC(BaseException,e,DEPART_DAO_D,"删除部门信息出错！");
	}
	return 0;
}
