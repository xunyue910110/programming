#include "InterfaceLog.h"

InterfaceLog::InterfaceLog()
{}

InterfaceLog::~InterfaceLog()
{}

bool InterfaceLog::Init(const PCFmlBuf &inBuf, const PCFmlBuf &outBuf)
{
    try
    {
        if(inBuf->IsFldExist("X_TRANS_CODE"))
            sTransCode = inBuf->GetString("X_TRANS_CODE");
        if(inBuf->IsFldExist("PROVINCE_CODE"))
            sProvinceCode = inBuf->GetString("PROVINCE_CODE");
        if(inBuf->IsFldExist("IN_MODE_CODE"))
            sInModeCode = inBuf->GetString("IN_MODE_CODE");
        if(inBuf->IsFldExist("TRADE_EPARCHY_CODE"))
            sTradeEparchyCode = inBuf->GetString("TRADE_EPARCHY_CODE");
        if(inBuf->IsFldExist("TRADE_CITY_CODE"))
            sTradeCityCode = inBuf->GetString("TRADE_CITY_CODE");
        if(inBuf->IsFldExist("TRADE_DEPART_ID"))
            sTradeDepartId = inBuf->GetString("TRADE_DEPART_ID");
        if(inBuf->IsFldExist("TRADE_STAFF_ID"))
            sTradeStaffId = inBuf->GetString("TRADE_STAFF_ID");
        if(inBuf->IsFldExist("TRADE_DEPART_PASSWD"))
            sTradeDepartPasswd = inBuf->GetString("TRADE_DEPART_PASSWD");
        
        if(outBuf->IsFldExist("X_RESULTCODE"))
            lResultCode = outBuf->GetInt("X_RESULTCODE");
        if(outBuf->IsFldExist("X_RESULTINFO"))
            sResultInfo = outBuf->GetString("X_RESULTINFO");
        
        return true;
    }
    catch( Exception &e )
    {
        return false;
    }
}

bool InterfaceLog::setLogFilePath()
{
    sLogFilePath = getenv("HOME");
    
    if(sLogFilePath.length()==0)
    {
        return false;
    }
    
    sLogFilePath += "/log/ItfLogFilePath";
    
    return true;
}

bool InterfaceLog::setLogFileName()
{
    if(sInModeCode.length()==0)//接入方式为空
    {
        return false;
    }
    
    if(sInModeCode=="1")//客服
    {
        sLogFileName = "CC";
    }
    else if(sInModeCode=="2")//网上客服
    {
        sLogFileName = "WC";
    }
    else if(sInModeCode=="3")//网上营业厅
    {
        sLogFileName = "WB";
    }
    else if(sInModeCode=="4")//银行
    {
        sLogFileName = "BK";
    }
    else if(sInModeCode=="5")//短信平台
    {
        sLogFileName = "SM";
    }
    else if(sInModeCode=="7")//手机支付
    {
        sLogFileName = "SP";
    }
    else if(sInModeCode=="8")//统一帐户服务系统
    {
        sLogFileName = "UA";
    }
    else if(sInModeCode=="9")//短信营销
    {
        //sLogFileName = "CC";
    }
    else if(sInModeCode=="A")//触摸屏
    {
        //sLogFileName = "CC";
    }
    else if(sInModeCode=="B")//自助打印机
    {
        //sLogFileName = "CC";
    }
    else if(sInModeCode=="C")//多媒体
    {
        //sLogFileName = "CC";
    }
    else if(sInModeCode=="D")//自助营业厅
    {
        //sLogFileName = "CC";
    }
    else//其他
    {
        sLogFileName = "OTHER";
    }
    
    string str = sLogFileName; 
    
    sLogFileName = "ITFLOG" + sLogFileName + "." + getCurDay();
    sFMLLogFileName = "ITFFMLLOG" + str + "." + getCurHour();
    
    return true;
}

bool InterfaceLog::writeLog(const PCFmlBuf &inBuf, const PCFmlBuf &outBuf)
{
    pid_t pid = -1;
    pid = getpid();
    
    FILE * logfp;
    FILE * fmllogfp;
    string str = sLogFilePath + "/" + sLogFileName;
    logfp = fopen(str.c_str(),"a+");
    str = sLogFilePath + "/" + sFMLLogFileName;
    fmllogfp = fopen(str.c_str(),"a+");
    
    if ((logfp==NULL) || (fmllogfp==NULL))
    {
        return false;
    }
    
    string tmpstr = "InterfaceLog~" + convert<string>(pid) + "~" + sBeginTime 
                    + "~" + sEndTime + "~" + sTransCode + "~" + sProvinceCode 
                    + "~" + sInModeCode + "~" + sTradeEparchyCode + "~" + sTradeCityCode 
                    + "~" + sTradeDepartId + "~" + sTradeStaffId + "~" + sTradeDepartPasswd
                    + "~" + convert<string>(lResultCode);
    fprintf(logfp,"%s\n",tmpstr.c_str());
    fprintf(fmllogfp,"%s\n",tmpstr.c_str());
    
    tmpstr = "*********输入域:开始*********";
    fprintf(fmllogfp,"%s\n",tmpstr.c_str());
    inBuf->printFmlBuffer(fmllogfp);
    tmpstr = "*********输入域:结束*********";
    fprintf(fmllogfp,"%s\n",tmpstr.c_str());
    
    tmpstr = "*********输出域:开始*********";
    fprintf(fmllogfp,"%s\n",tmpstr.c_str());
    outBuf->printFmlBuffer(fmllogfp);
    tmpstr = "*********输出域:结束*********";
    fprintf(fmllogfp,"%s\n",tmpstr.c_str());
    
    fclose(logfp);   
    fclose(fmllogfp);   
    return true;
}

string InterfaceLog::getCurDay()		/*取当前日期*/
{
    char tmp[9] = "\0";
	struct tm   t;
	time_t      tt;

	tt=time(NULL);
	t=*localtime(&tt);
	sprintf(tmp,"%04d%02d%02d",(t.tm_year+1900),(t.tm_mon+1),t.tm_mday);
	string result = tmp;
	
	return result;
}

string InterfaceLog::getCurHour()		/*取当前小时*/
{
    char tmp[9] = "\0";
	struct tm   t;
	time_t      tt;

	tt=time(NULL);
	t=*localtime(&tt);
	sprintf(tmp,"%04d%02d%02d%02d",(t.tm_year+1900),(t.tm_mon+1),t.tm_mday,t.tm_hour);
	string result = tmp;
	
	return result;
}
