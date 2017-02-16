#include <sys/time.h>
#include <unistd.h>
#include "CrossRr.h"
#include "CrossRrApp.h"
#include "base/SysParam.h"

CrossRrApp::CrossRrApp(const int argc, char **argv)
{
    m_argc      = argc;
    m_argv      = (char **)argv;
    m_channelNo = 0;
    m_beginDate = 0;
    m_endDate   = 0;
    m_month     = 0;
}
CrossRrApp::~CrossRrApp()
{
    delete m_theSysParam;
}

void CrossRrApp::getCfgFileName()
{
    char *tmpCfgFileName;
    if (((tmpCfgFileName = getenv("CROSSRR_CFG")) == 0) || (strcmp(tmpCfgFileName, "") == 0))
    {
        char *workHome;
        if (((workHome = getenv("WORK_HOME")) == 0) || (strcmp(workHome, "") == 0)) 
        {
            workHome = "..";
        }
        m_cfgFileName = workHome;
        m_cfgFileName += "/etc/CrossRr.cfg";
    }
    else
    {
        m_cfgFileName = tmpCfgFileName;
    }
    cout << " cfgFileName : " << m_cfgFileName << endl;
}

bool CrossRrApp::getParamValueByName(const string &name, string &value)
{
    string commonSection;
    string channelSection;
    string userName,passWord,severName;
    string t_value;
    
    commonSection = SECTDELIM;
    commonSection += "CrossRr";
    commonSection += SECTDELIM;
    commonSection += "Common";
    
    channelSection = SECTDELIM;
    channelSection += "CrossRr";
    channelSection += SECTDELIM;
    channelSection += ltoa(m_channelNo);
    
    if(!m_theSysParam->getValue(commonSection,name,t_value))
    {
        if(!m_theSysParam->getValue(channelSection,name,t_value))
        {
            #ifdef _DEBUG_
            	cout<<"get " << name << " config param false!" <<__FILE__<<__LINE__<<endl;
            #endif
            return false;
        }
        else if (t_value.length() == 0)
        {
        	return false;
        }
        else
		{
			value = t_value;
        	return true;
        }
    }
    else if (t_value.length() == 0)
    {
        if (!m_theSysParam->getValue(channelSection,name,t_value))
        {
        	#ifdef _DEBUG_
            	cout<<"get " << name << " config param false!" <<__FILE__<<__LINE__<<endl;
            #endif
            return false;
       	}
       	else if (t_value.length() == 0)
        {
        	return false;
        }
        else
		{
			value = t_value;
        	return true;
        }
    }
    value = t_value;
	return true;
}

bool CrossRrApp::getSysParam()
{
    string name,passwd,dbserver;
    DBINFO r_dbInfo;
    getCfgFileName();
    
    if ((m_theSysParam = new SysParam()) == 0)
    {
        return false;
    }
    m_theSysParam->initialize((char *)m_cfgFileName.c_str());
    
    if (!getParamValueByName("ParamDbUserName",name))
	{
		return false;
	}
    if (!getParamValueByName("ParamDbPassWd",passwd))
    {
		return false;
	}
    if (!getParamValueByName("ParamDbServer",dbserver))
    {
		return false;
	}
    m_paramDbInfo.set(name.c_str(),passwd.c_str(),dbserver.c_str());
    
    if (!getParamValueByName("SourceDbUserName",name))
    {
		return false;
	}
    if (!getParamValueByName("SourceDbPassWd",passwd))
    {
		return false;
	}
    if (!getParamValueByName("SourceDbServer",dbserver))
    {
		return false;
	}
    m_srcDbInfo.set(name.c_str(),passwd.c_str(),dbserver.c_str());
    
    if (!getParamValueByName("DesDbUserName",name))
    {
		return false;
	}
    if (!getParamValueByName("DesDbPassWd",passwd))
    {
		return false;
	}
    if (!getParamValueByName("DesDbServer",dbserver))
    {
		return false;
	}
    m_desDbInfo.set(name.c_str(),passwd.c_str(),dbserver.c_str());
    cout << " m_paramDbInfo : " << m_paramDbInfo << endl;
    cout << " m_srcDbInfo   : " << m_srcDbInfo << endl;
    cout << " m_desDbInfo   : " << m_desDbInfo << endl;
    getParamValueByName("WhereClause",m_whereClause);
    cout << " WhereClause   : " << m_whereClause << endl;
    return true;
}

bool CrossRrApp::parseOpt()
{
    char temp;
    bool flag1=false,flag2=false,flag3=false,flag4=false;
    while((temp=getopt(m_argc,m_argv,"c:m:b:e:"))!=-1)
    {
        switch (temp)
        {
            case 'c': m_channelNo = atoi(optarg); flag1 = true; break;
            case 'm': m_month     = atoi(optarg); flag2 = true; break;
            case 'b': m_beginDate = atoi(optarg); flag3 = true; break;
            case 'e': m_endDate   = atoi(optarg); flag4 = true; break;
        }
    }
    return flag1 && flag2 && flag3 && flag4;
}

int CrossRrApp::run()
{
    if (!parseOpt())
    {
        return -1;
    }
    if (!getSysParam())
    {
        return -1;
    }
    if (m_crossRr.initialization(m_paramDbInfo,m_srcDbInfo,m_desDbInfo,m_whereClause))
    {
        m_crossRr.setCheckInfo(m_month,m_beginDate,m_endDate);
        m_crossRr.crossRrCheck();
        m_crossRr.crossRrIndb();
    }
    return 0;
}
