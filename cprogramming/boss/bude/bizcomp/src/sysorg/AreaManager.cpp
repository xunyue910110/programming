#include "AreaManager.h"

/**********************************************************************************
*函数：		GetAreaInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		地洲资料获取接口函数
**********************************************************************************/
extern "C" int GetAreaInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
	int i_GetModeCode;
	AreaManager * TAreaManager = new AreaManager();
	try
	{
		i_GetModeCode = inBuf.GetInt("X_GETMODE");
		ORG_TdMArea T_Area;
		switch(i_GetModeCode)
		{
			case 0:
			{
				T_Area.ValidFlag = "0";
				break;
			}
			case 1:
			{
				T_Area.ValidFlag = "0";
				T_Area.AreaLevel = inBuf.GetInt("AREA_LEVEL");
				break;
			}
			case 2:
			{
				T_Area.ValidFlag = "0";
				T_Area.AreaCode=inBuf.GetString("AREA_CODE");
				break;
			}
			case 3:
			{
				T_Area.ValidFlag = "0";
				T_Area.AreaCode=inBuf.GetString("AREA_CODE");
				break;
			}
			case 4:
			{
				T_Area.ValidFlag = "0";
				T_Area.UserAreaCode=inBuf.GetString("AREA_CODE");
				break;
			}
			case 5:
			{
				T_Area.ValidFlag = "0";
				T_Area.UserAreaCode=inBuf.GetString("AREA_CODE");
				break;
			}
			default:
			{
				delete TAreaManager;
				THROW_C(BaseException,AREA_COM_0,"获取地洲信息选择参数出错！");
			}
		}
		outBuf = TAreaManager->GetAreaInfo(T_Area,i_GetModeCode);
		outBuf.SetString("X_RESULTINFO","获取地洲资料成功");
		delete TAreaManager;
		return 0;
	}
	catch(Exception &e)
	{
		outBuf.SetInt("X_RESULTCODE",e.getCode());
		outBuf.SetString("X_RESULTINFO","获取地洲资料出错："+e.toString());
		delete TAreaManager;
		return -1;
	}
}
/**********************************************************************************
*函数：		SaveAreaInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
*功能：		地洲资料修改接口函数
**********************************************************************************/
extern "C" int SaveAreaInfo(CFmlBuf &inBuf,CFmlBuf &outBuf)
{
/*	int i_inrecord;
	int i_SaveModeCode;
	int i_result = -1;
	int i_SuccResult = 0;
	
	try
	{
		i_inrecord = inBuf.GetInt("X_RECORDNUM");
		AreaManager * TAreaManager = new AreaManager();
		ORG_TdMArea T_Area;
		
		for(long i = 0;i < i_inrecord;i++)
		{
			T_Area.ORG_init();
			inBuf.GetValueEx("AREA_CODE",(void *)&(T_Area.AreaCode),i,false);
			inBuf.GetValueEx("AREA_NAME",(void *)&(T_Area.AreaName),i,false);
			inBuf.GetValueEx("AREA_FRAME",(void *)&(T_Area.AreaFrame),i,false);
			inBuf.GetValueEx("THIS_TAG",(void *)&(T_Area.ThisTag),i,false);
			inBuf.GetValueEx("USE_TAG",(void *)&(T_Area.UseTag),i,false);
			inBuf.GetValueEx("EXTEND_TAG",(void *)&(T_Area.ExtendTag),i,false);
			inBuf.GetValueEx("ORDER_NO",(void *)&(T_Area.OrderNo),i,false);
			inBuf.GetValueEx("USER_AREA_CODE",(void *)&(T_Area.UserAreaCode),i,false);
			inBuf.GetValueEx("PARENT_AREA_CODE",(void *)&(T_Area.ParentAreaCode),i,false);
			inBuf.GetValueEx("START_DATE",(void *)&(T_Area.StartDate),i,false);
			inBuf.GetValueEx("END_DATE",(void *)&(T_Area.EndDate),i,false);
			inBuf.GetValueEx("AREA_LEVEL",(void *)&(T_Area.AreaLevel),i,false);
			inBuf.GetValueEx("VALID_FLAG",(void *)&(T_Area.ValidFlag),i,false);
			inBuf.GetValueEx("REMARK",(void *)&(T_Area.Remark),i,false);
			inBuf.GetValueEx("RSVALUE1",(void *)&(T_Area.Rsvalue1),i,false);
			inBuf.GetValueEx("RSVALUE2",(void *)&(T_Area.Rsvalue2),i,false);
			inBuf.GetValueEx("UPDATE_TIME",(void *)&(T_Area.UpdateTime),i,false);
			inBuf.GetValueEx("TRADE_STAFF_ID",(void *)&(T_Area.UpdateStaffId),i,false);
			inBuf.GetValueEx("TRADE_DEPART_ID",(void *)&(T_Area.UpdateDepartId),i,false);
			
			i_SaveModeCode = -1;
			i_SaveModeCode = inBuf.GetInt("SAVEMODECODE");
			i_result = TAreaManager->SaveAreaInfo(T_Area,i_SaveModeCode);
			
			if(i_result == 0)
			{
				i_SuccResult++;
			}
		}
		
		OperLog * T_OperLog = new OperLog();
		T_OperLog->InsertOrgmanlog(inBuf);
		
		outBuf.SetInt("RESULTCODE",0);
		outBuf.SetString("RESULTERRINFO","处理地洲编码表成功！");
		outBuf.SetInt("RESULTRECORDS",i_SuccResult);
		delete TAreaManager;
		delete T_OperLog;
		return 0;
	}
	catch(Exception &e)
	{
		outBuf.SetInt("RESULTCODE",e.getCode());
		outBuf.SetString("RESULTERRINFO","处理地洲资料出错："+e.getMessage());
		return -1;
	}*/
	return 0 ;
}

/**********************************************************************************
*函数：		AreaManager()
*功能：		地洲资料管理构造函数
**********************************************************************************/
AreaManager::AreaManager()
{
	pcon = jdb::getConnect();
	D_ORG_TdMAreaDAO = new ORG_TdMAreaDAO();
}

/**********************************************************************************
*函数：		~AreaManager()
*功能：		地洲资料管理析构函数
*
**********************************************************************************/
AreaManager::~AreaManager()
{
	delete D_ORG_TdMAreaDAO;
}

/**********************************************************************************
*函数：		GetAreaInfo(ORG_TdMArea &T_ORG_TdMArea,const int GetModeCode)
*功能：		获取地洲资料信息
*参数：		T_ORG_TdMArea：							获取地洲资料条件
					GetModeCode：								获取地洲资料类型编码
*返回：
**********************************************************************************/
CFmlBuf AreaManager::GetAreaInfo(ORG_TdMArea &T_ORG_TdMArea,const int GetModeCode)
{
	CFmlBuf B_Buf;
	int i = 0;
	
	VPORG_TdMArea vpRetORG_TdMArea;
	
	switch (GetModeCode)
	{
		case 0:
		{
			/*获取全部有效地洲信息*/
			try
			{
				vpRetORG_TdMArea = D_ORG_TdMAreaDAO->ORG_jselby(SEL_AREA_0,T_ORG_TdMArea);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,AREA_DAO_S,"获取全部有效地洲出错！");
			}
			break;
		}
		case 1:
		{
			/*获取指定级别有效地洲*/
			try
			{
				vpRetORG_TdMArea = D_ORG_TdMAreaDAO->ORG_jselby(SEL_AREA_1,T_ORG_TdMArea);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,AREA_DAO_S,"获取指定级别有效地洲出错！");
			}
			break;
		}
		case 2:
		{
			/*获取指定地域编码的地域信息*/
			try
			{
				vpRetORG_TdMArea = D_ORG_TdMAreaDAO->ORG_jselby(SEL_AREA_2,T_ORG_TdMArea);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,AREA_DAO_S,"获取指定地域编码的地域信息出错！");
			}
			break;
		}
		case 3:
		{
			/*获取指定地域编码的所有下级地域信息*/
			try
			{
				vpRetORG_TdMArea = D_ORG_TdMAreaDAO->ORG_jselby(SEL_AREA_3,T_ORG_TdMArea);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,AREA_DAO_S,"获取指定地域编码的所有下级地域信息出错！");
			}
			break;
		}
		case 4:
		{
			/*获取指定用户编码的地域信息*/
			try
			{
				vpRetORG_TdMArea = D_ORG_TdMAreaDAO->ORG_jselby(SEL_AREA_4,T_ORG_TdMArea);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,AREA_DAO_S,"获取指定用户编码的地域信息出错！");
			}
			break;
		}
		case 5:
		{
			/*获取指定用户编码的所有下级地域信息*/
			try
			{
				vpRetORG_TdMArea = D_ORG_TdMAreaDAO->ORG_jselby(SEL_AREA_5,T_ORG_TdMArea);
			}
			catch(Exception &e)
			{
				THROW_AC(BaseException,e,AREA_DAO_S,"获取指定用户编码的所有下级地域信息出错！");
			}
			break;
		}
		default:
		{
			/*其它处理*/
			THROW_C(BaseException,AREA_COM_0,"获取地洲信息填入参数出错！");
		}
	}
	/*获取相应地结果*/
	try
	{
		vector<ORG_TdMArea>::const_iterator it = vpRetORG_TdMArea.begin();
		for( ;it != vpRetORG_TdMArea.end();it++)
		{
			B_Buf.SetString("AREA_CODE",(*it).AreaCode,i);
			B_Buf.SetString("AREA_NAME",(*it).AreaName,i);
			B_Buf.SetString("AREA_FRAME",(*it).AreaFrame,i);
			B_Buf.SetString("THIS_TAG",(*it).ThisTag,i);
			B_Buf.SetString("USE_TAG",(*it).UseTag,i);
			B_Buf.SetString("EXTEND_TAG",(*it).ExtendTag,i);
			B_Buf.SetInt("ORDER_NO",(*it).OrderNo,i);
			B_Buf.SetString("USER_AREA_CODE",(*it).UserAreaCode,i);
			B_Buf.SetString("PARENT_AREA_CODE",(*it).ParentAreaCode,i);
			B_Buf.SetString("START_DATE",(*it).StartDate,i);
			B_Buf.SetString("END_DATE",(*it).EndDate,i);
			B_Buf.SetInt("AREA_LEVEL",(*it).AreaLevel,i);
			B_Buf.SetString("VALID_FLAG",(*it).ValidFlag,i);
			B_Buf.SetString("REMARK",(*it).Remark,i);
			B_Buf.SetString("RSVALUE1",(*it).Rsvalue1,i);
			B_Buf.SetString("RSVALUE2",(*it).Rsvalue2,i);
			B_Buf.SetString("UPDATE_TIME",(*it).UpdateTime,i);
			B_Buf.SetString("UPDATE_STAFF_ID",(*it).UpdateStaffId,i);
			B_Buf.SetString("UPDATE_DEPART_ID",(*it).UpdateDepartId,i);
			
			B_Buf.SetString("X_THISTAG",(*it).b_ThisTag,i);
			B_Buf.SetString("X_USETAG",(*it).b_UseTag,i);
			B_Buf.SetString("X_EXTENDTAG",(*it).b_ExtendTag,i);
			B_Buf.SetString("X_VALIDFLAG",(*it).b_ValidFlag,i);
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
		THROW_AC(BaseException,e,AREA_COM_1,GetModeCode+"方式获取指定地洲信息赋值出错！");
	}
}

/**********************************************************************************
*函数：		SaveAreaInfo
*功能：		
*返回
**********************************************************************************/
int AreaManager::SaveAreaInfo(ORG_TdMArea &D_ORG_TdMArea,const int SaveModeCode)
{
	int i_Result;
	{
		switch(SaveModeCode)
		{
			case 1:
				{
					i_Result = UpdateAreaInfo(D_ORG_TdMArea);
					break;
				}
			case 2:
				{
					i_Result = InsertAreaInfo(D_ORG_TdMArea);
					break;
				}
			case 3:
				{
					i_Result = DeleteAreaInfo(D_ORG_TdMArea);
					break;
				}
			default:
				THROW_C(BaseException,AREA_COM_0,SaveModeCode+"方式修改指定地洲信息赋值出错！");
		}
	}
	return 0;
}

/**********************************************************************************
*
*
*
**********************************************************************************/
int AreaManager::InsertAreaInfo(ORG_TdMArea &T_ORG_TdMArea)
{
	try
	{
		D_ORG_TdMAreaDAO->ORG_jinsert(T_ORG_TdMArea);
	}
	catch(DAOException &e)
	{
		THROW_AC(BaseException,e,AREA_DAO_I,"插入地洲信息出错！");
	}
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int AreaManager::UpdateAreaInfo(ORG_TdMArea &T_ORG_TdMArea)
{
	try
	{
		D_ORG_TdMAreaDAO->ORG_jupdate(UPD_UPD_AREA_0,T_ORG_TdMArea);
	}
	catch(DAOException &e)
	{
		THROW_AC(BaseException,e,AREA_DAO_U,"修改地洲信息出错！");
	}
	return 0;
}
/**********************************************************************************
*
*
*
**********************************************************************************/
int AreaManager::DeleteAreaInfo(ORG_TdMArea &T_ORG_TdMArea)
{
	try
	{
		D_ORG_TdMAreaDAO->ORG_jupdate(UPD_DEL_AREA_0,T_ORG_TdMArea);
	}
	catch(DAOException &e)
	{
		THROW_AC(BaseException,e,AREA_DAO_D,"删除地洲信息出错！");
	}
	return 0;
}
