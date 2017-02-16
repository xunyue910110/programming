#include "GprsAttribProc.h"

#include "FilterError.h"
#include "FilterConfig.h"
#include "PMGlobalManager.h"
//#include "base/StringUtil.h"

INSTALL_PROCESSBASE_INSTANCE(GprsAttribProc);


int GprsAttribProc::execute() {
	
	//错单,不进行处理
	if (!isValidCDR())
		return 0;
		
	ProcessBaseHelper::ExecuteProcedure("CdrAttribInit");

	//求A归属地
    if(!getHomeAreaCodeA())
        return false;

	//本地GPRS话单求到访地,出访不求
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_CG)
	{
		//求A到访地
		if(!getGprsVisitAreaCodeA())
		{
			if(m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '1')
				return false; //GCDR求不到到访地，直接判为错单 gaojf 10/17/2006 11:23AM

			//return false;
			//GPRS话单置缺省的省代码
			if (m_Config->m_provinceCode[m_Config->m_provinceCode.length() - 1] == '0')
			{
				m_pcdr[ATTRI_VISIT_CODE_A] = "0" + (m_Config->m_provinceCode).substr(0,m_Config->m_provinceCode.length() - 1);
			}
			else
			{
				m_pcdr[ATTRI_VISIT_CODE_A] = "0" + m_Config->m_provinceCode;
			}
			m_pcdr[ATTRI_ERROR_CODE] = DEFAULT_ERROR;
		}
	}
	//GCDR CNG 话单，根据到访省代码求取
	else if(m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '1')
	{
		if(!getGprsVisitAreaCodeA())
		{
			//GCDR求不到到访地，直接判为错单 gaojf 10/17/2006 11:23AM
			return false;
		}
	}
      
	setRoamType();
	
	if(isValidGprsCdr()==false) return false;
	
	if(isValidApn()==false) return false;
		
	//Added By Sunam 2007-12-7 11:20 GPRS到访国分组
	if (m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_OUT)
	{
		T_ATTRI_VAL groupIdGprs;
		if (m_globalManager->getGroupIdGprsByVisitAreaCode(m_pcdr[ATTRI_VISIT_CODE_A],m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],groupIdGprs))
		{
			m_pcdr[ATTRI_GPRSGROUPID] = groupIdGprs;
		}
		else
		{
			m_pcdr[ATTRI_GPRSGROUPID] = "00";
		}
	}
		
	//Added By Sunam 2007-1-30 15:55 CMWAP APN下的话单有流量但不构造扩展字段 判错
	if ((m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '1') && 
		(m_pcdr[ATTRI_GPRS_APNNIGROUP] == GPRS_APNGROUP_CMWAP) &&
		(m_pcdr[ATTRI_GPRS_RCDEXTS].length() == 0))
	{
		m_pcdr[ATTRI_ERROR_CODE] = GPRS_CONTEXTINFO_ERROR;
		return false;
	}
		

	//设置话单是否有原始费用信息标志
	ProcessBaseHelper::ExecuteProcedure("SetOriginFeeFlag");
	
	//GPRS不在TpProc取基本政策，在GprsPrice中取基本政策
	//m_pcdr[ATTRI_NEED_BASETP] = NO;
	
	return 1;
}

//检验是否有效的APN(国内来访检验APNNI，国际来访检验APNOI)
//省内用户校验APNNI(包括国际出访)
bool GprsAttribProc::isValidApn()
{
	char servRange;
	T_GPRS_APNGROUP tmpAnpGroup;
	T_GPRS_APNTYPE  apnniType;
	int  						priority;
	T_GPRS_APNNI    tmpApnni;
	tmpApnni = m_pcdr[ATTRI_GPRS_APNNI].c_str();
	if(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN)
	{//国际来访校验APNOI
		//国际来访APNOI在漫游存储过程中检验，该处不校验 10/20/2006 8:29AM gaojf
		/*
		T_GPRS_APN tmpApnoi;
		tmpApnoi = m_pcdr[ATTRI_GPRS_APNOI].c_str();
		if(m_globalManager->isValidGprsApnoi(tmpApnoi,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME])==false)
		{
			m_pcdr[ATTRI_ERROR_CODE] = INVALID_GPRS_APNOI;
			return false;
		}
		*/
		m_pcdr[ATTRI_GPRS_APNNIGROUP] = "*";//国际来访置缺省APN分组
		m_pcdr[ATTRI_GPRS_APNNITYPE]  = "*";
	return true;
	}else if(m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_OUT)
	{ //国际出访APNNI校验
		//暂定为：校验，但不判为错单
		if(m_globalManager->getGprsApnniInfo(tmpApnni,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			          tmpAnpGroup,servRange)==false)
		{ //无对应的参数，则置缺省
			m_pcdr[ATTRI_GPRS_APNNIGROUP] = "*";
			m_pcdr[ATTRI_GPRS_APNNITYPE]  = "*";
		}else
		{
			m_pcdr[ATTRI_GPRS_APNNIGROUP]= tmpAnpGroup;
			if(m_globalManager->getApnniType(tmpAnpGroup,apnniType,priority)==false)
		  {
		  	m_pcdr[ATTRI_GPRS_APNNITYPE]  = "*";
		  }else
				m_pcdr[ATTRI_GPRS_APNNITYPE] = apnniType;
		}
	}else
	{//校验APNNI
		if(m_globalManager->getGprsApnniInfo(tmpApnni,m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
			          tmpAnpGroup,servRange)==false)
		{
			m_pcdr[ATTRI_ERROR_CODE] = INVALID_GPRS_APNNI;
			return false;
		}
		if(m_pcdr[ATTRI_ROAM_TYPE]==ROAM_TYPE_NATIONAL_IN)
		{
			if(servRange!='0' )
			{ //国内来访，但业务为非全省业务，也判为错单
				m_pcdr[ATTRI_ERROR_CODE] = INVALID_GPRS_APNNI;
				return false;
			}
			m_pcdr[ATTRI_GPRS_APNNIGROUP] = "*"; //国内来访APNNIGROUP置为*
			m_pcdr[ATTRI_GPRS_APNNITYPE]  = "*";
		}else
		{
			m_pcdr[ATTRI_GPRS_APNNIGROUP]= tmpAnpGroup;
			if(m_globalManager->getApnniType(tmpAnpGroup,apnniType,priority)==false)
		  {
		  	m_pcdr[ATTRI_ERROR_CODE] = GPRS_GETAPNNITYPE_ERROR;
		    return false;
		  }
			m_pcdr[ATTRI_GPRS_APNNITYPE] = apnniType;
		}
	}
	return true;
}

void GprsAttribProc::setRoamType(void)
{
    if(m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '0') //SCDR
    {
        setRoamTypeScdr();
    }
    else
    {
        setRoamTypeGcdr();
    }
}

void GprsAttribProc::setRoamTypeScdr(void)
{
	//本地GPRS话单
	if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_CG)
	{
		AttribProc::setRoamType();
	}
	//省际漫游
	else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_ROG)
	{
		m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_OUT;
	}
	//国际漫游
	else if(m_pcdr[ATTRI_SOURCE_TYPE] == SOURCE_TYPE_GPRS_ROIG)
	{
		m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_OUT;
	}
}

bool GprsAttribProc::getGprsVisitAreaCodeA(void)
{
    if(m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '0') //SCDR
    {
        return getScdrVisitAreaCodeA();
    }
    else
    {
        return getGcdrVisitAreaCodeA();
    }
}

bool GprsAttribProc::getScdrVisitAreaCodeA(void)
{
switch(m_Config->m_getGprsAVisitMethod)
	{
		case A_GPRS_VISIT_BY_PROV_CODE:
				if (m_Config->m_provinceCode[m_Config->m_provinceCode.length() - 1] == '0')
				{
					m_pcdr[ATTRI_VISIT_CODE_A] = "0" + (m_Config->m_provinceCode).substr(0,m_Config->m_provinceCode.length() - 1);
				}
				else
				{
					m_pcdr[ATTRI_VISIT_CODE_A] = "0" + m_Config->m_provinceCode;
				}
			break;
		//根据MSC,LAC
		case A_GPRS_VISIT_BY_MSC_LAC:    
			if (!getAVByMscLac())
				return false;
			break;
		//根据MSC,CELLID
		case A_GPRS_VISIT_BY_MSC_CELLID: 
			if (!getAVByMscCellId())
				return false;
			break;
		//根据MSC,LAC,CELLID
		case A_GPRS_VISIT_BY_MSC_LAC_CELLID: 
			if (!getAVByMscLacCellId())
				return false;
			break;
		//根据CELL_ID
		case A_GPRS_VISIT_BY_CELLID: 
			if (!getAVByCellId())
				return false;
			break;
		//根据LAC
		case A_GPRS_VISIT_BY_LAC: 
			if (!getAVByLac())
				return false;
			break;
		//根据 LAC,CELL_ID
		case A_GPRS_VISIT_BY_LAC_CELLID: 
			if  (!getAVByLacCellId())
				return false;
			break;
		//根据交换机msc
		case A_GPRS_VISIT_BY_MSC: 
			//Modified by Sunam 2006-3-27 18:59
			//if (!getAVByMsc())
			if (!getAVByMsc(ALL_MSC))
				return false;
			break;
		default:
			if (!getAVByMscLac())
				return false;
		break;
	}
	return true;
}

void GprsAttribProc::setRoamTypeGcdr(void)
{
    if(m_pcdr[ATTRI_USER_HOME_TYPE_A] < AREA_TYPE_NATIONAL)
    { //省内用户
        if (m_sgsnHomeType == AREA_TYPE_UNKNOWN ||
            m_sgsnHomeType == AREA_TYPE_PROVINCE||
            m_sgsnHomeType == AREA_TYPE_LOCAL)
        { //省内漫游 1
            m_pcdr[ATTRI_USER_HOME_TYPE_A] = AREA_TYPE_LOCAL;
            m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NONE; //非漫游 - 0
        }
        else if(m_sgsnHomeType == AREA_TYPE_NATIONAL)
        {//国内出访 4
            m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_OUT;
        }
        else 
        {//国际出访 6
            m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_OUT;
        }	
    }
    else if (m_pcdr[ATTRI_USER_HOME_TYPE_A] == AREA_TYPE_NATIONAL)
    { //国内来访
        m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_NATIONAL_IN;  //国内来访　- 3 
    }
    else //国际来访
    {
        m_pcdr[ATTRI_ROAM_TYPE] = ROAM_TYPE_INTERNATIONAL_IN; //国际来访　-5
    }
}

bool GprsAttribProc::getGcdrVisitAreaCodeA(void)
{
	//根据SGSN地址，求取到访地信息：包括到访地区号、到访地归属类型
	T_GPRS_SGSN tmpSgsn;
	tmpSgsn = m_pcdr[ATTRI_GPRS_SGSN].c_str();
	if(m_globalManager->getSgsnAddrInfo(m_pcdr[ATTRI_BEGIN_DATE] + m_pcdr[ATTRI_BEGIN_TIME],
					tmpSgsn,m_visitProvCode,m_sgsnHomeType)==false)
	{
		#ifdef _DEBUG_
			cout<<"getGgsnAddrInfo Error sgsn = "<< tmpSgsn<<" "<<__FILE__<<__LINE__<<endl;
		#endif
		m_pcdr[ATTRI_ERROR_CODE] = AV_GPRS_SGSN_ERROR;
		return false;
	}
	//直接取区号(联通需求)
	if(m_visitProvCode.length()>0)
	{
		 m_pcdr[ATTRI_VISIT_CODE_A] = m_visitProvCode;
	}
	m_pcdr[ATTRI_GPRS_VISIT_HOMETYPE] = m_sgsnHomeType;
/*
	string tmpProvCode;
	tmpProvCode = m_visitProvCode.c_str();
	if(tmpProvCode.length()>0)
	{
		if (tmpProvCode[tmpProvCode.length() - 1] == '0')
		{
			m_pcdr[ATTRI_VISIT_CODE_A] = "0"+tmpProvCode.substr(0,tmpProvCode.length() - 1);
		}
		else
		{
			m_pcdr[ATTRI_VISIT_CODE_A] = "0"+ tmpProvCode;
		}
	}
*/
	return true;
}

bool GprsAttribProc::isValidGprsCdr()
{
	//1. 国际来出访GCDR话单判为无效话单
	if((m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_OUT ||
		  m_pcdr[ATTRI_ROAM_TYPE] == ROAM_TYPE_INTERNATIONAL_IN)&&
		 m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '1')
	{
		m_pcdr[ATTRI_ERROR_CODE] = INVALID_ERROR;
		return false;
	} 
	//2. 改造完毕后，只保留国内来访国际来出访SCDR话单
	/*if(m_pcdr[ATTRI_ROAM_TYPE] < ROAM_TYPE_INTERNATIONAL_IN &&
		 m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '0')
	*/
	if ((m_pcdr[ATTRI_GPRS_RECORDTYPE][0] == '0') &&
		(m_pcdr[ATTRI_ROAM_TYPE] != ROAM_TYPE_NATIONAL_IN) &&
		(m_pcdr[ATTRI_ROAM_TYPE] != ROAM_TYPE_INTERNATIONAL_IN) &&
		(m_pcdr[ATTRI_ROAM_TYPE] != ROAM_TYPE_INTERNATIONAL_OUT))
	{
		m_pcdr[ATTRI_ERROR_CODE] = INVALID_ERROR;
		return false;
	} 
	return true;
}
