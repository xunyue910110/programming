#include "SysWeb.h"

SysWeb g_SysWeb;

/*插入WEB信息错误*/
const int ADDWEBINFO_ERR = 320001;

/*查询WEB信息错误*/
const int GETWEBINFO_ERR = 320002;

extern "C" void AddWebInfo(CFmlBuf &InBuf, CFmlBuf &OutBuf)
{
    // 输入参数
    SysWebAddInPara sSysWebAddInPara;

	// 接受消息
    sSysWebAddInPara.strRecvEparchyCode =  InBuf.GetString("RECV_EPARCHY_CODE");
    sSysWebAddInPara.strRecvCityCode = InBuf.GetString("RECV_CITY_CODE");
    sSysWebAddInPara.strRecvDepartId = InBuf.GetString("RECV_DEPART_ID");
    sSysWebAddInPara.strRecvStaffId = InBuf.GetString("RECV_STAFF_ID");

	//  发送消息
    sSysWebAddInPara.strTradeEparchyCode =  InBuf.GetString("TRADE_EPARCHY_CODE");
    sSysWebAddInPara.strTradeCityCode = InBuf.GetString("TRADE_CITY_CODE");
    sSysWebAddInPara.strTradeDepartId = InBuf.GetString("TRADE_DEPART_ID");
    sSysWebAddInPara.strTradeStaffId = InBuf.GetString("TRADE_STAFF_ID");

    // 容器类型
    sSysWebAddInPara.iInfoContainerType = InBuf.GetInt("INFO_CONTAINER_TYPE") ;

    // 信息类型
    sSysWebAddInPara.iInfoType = InBuf.GetInt("INFO_TYPE") ;

    // 信息主题
    sSysWebAddInPara.strInfoTopic = InBuf.GetString("INFO_TOPIC") ;

    // 信息内容
    sSysWebAddInPara.strInfoContent = InBuf.GetString("INFO_CONTENT") ;

    // 相关文档
    sSysWebAddInPara.strInfoAttach = InBuf.GetString("INFO_ATTACH") ;

    // 输出参数
    SysWebAddOutPara sSysWebAddOutPara;

    // 执行
    int iCount = g_SysWeb.AddWebInfo(sSysWebAddInPara, sSysWebAddOutPara);

    OutBuf.SetInt("X_RECORDNUM", iCount);

    return;
}

extern "C" void GetWebInfo(CFmlBuf &InBuf, CFmlBuf &OutBuf)
{
    // 输入参数
    SysWebGetInPara sSysWebGetInPara;

    sSysWebGetInPara.strTradeEparchyCode =  InBuf.GetString("TRADE_EPARCHY_CODE");
    sSysWebGetInPara.strTradeCityCode = InBuf.GetString("TRADE_CITY_CODE");
    sSysWebGetInPara.strTradeDepartId = InBuf.GetString("TRADE_DEPART_ID");
    sSysWebGetInPara.strTradeStaffId = InBuf.GetString("TRADE_STAFF_ID");

    // 信息类型
    sSysWebGetInPara.iGetMode = InBuf.GetInt("X_GETMODE");

    // 输出参数
    SysWebGetOutPara sSysWebGetOutPara;
    sSysWebGetOutPara.iInfoCount = 0;
    sSysWebGetOutPara.strInfo = " ";

    // 执行
    g_SysWeb.GetWebInfo(sSysWebGetInPara, sSysWebGetOutPara);

    // 返回
    OutBuf.SetInt("X_RECORDNUM", sSysWebGetOutPara.iInfoCount);
    OutBuf.SetString("X_RESULTINFO", sSysWebGetOutPara.strInfo);

    return;
}

int SysWeb::AddWebInfo(const SysWebAddInPara& sSysWebAddInPara, SysWebAddOutPara& sSysWebAddOutPara)
{
    int iCount = 0;

    try
	{
		JConnect * pcon = jdb::getConnect();
		JTable * jTable = pcon->getJTable();
		jTable->Clear();

		jTable->AddSql("BEGIN ");
		jTable->AddSql("     p_web_addwebinfo(:v_EparchyCode, :v_CityCode, :v_DepartId, :v_StaffId, :v_InfoContainerType, :v_InfoType, :v_InfoTopic, :v_InfoContent, :v_InfoAttach, :v_OperEparchyCode, :v_OperCityCode, :v_OperDepartId, :v_OperStaffId, :v_RetCode, :v_RetErr);");
		jTable->AddSql("END; ");

        // 输入参数
        jTable->SetParam(":v_EparchyCode", sSysWebAddInPara.strRecvEparchyCode);
        jTable->SetParam(":v_CityCode", sSysWebAddInPara.strRecvCityCode);
        jTable->SetParam(":v_DepartId", sSysWebAddInPara.strRecvDepartId);
        jTable->SetParam(":v_StaffId", sSysWebAddInPara.strRecvStaffId);

        jTable->SetParam(":v_InfoContainerType", sSysWebAddInPara.iInfoContainerType);
        jTable->SetParam(":v_InfoType", sSysWebAddInPara.iInfoType);
        jTable->SetParam(":v_InfoTopic", sSysWebAddInPara.strInfoTopic);
        jTable->SetParam(":v_InfoContent", sSysWebAddInPara.strInfoContent);
        jTable->SetParam(":v_InfoAttach", sSysWebAddInPara.strInfoAttach);

        jTable->SetParam(":v_OperEparchyCode", sSysWebAddInPara.strTradeEparchyCode);
        jTable->SetParam(":v_OperCityCode", sSysWebAddInPara.strTradeCityCode);
        jTable->SetParam(":v_OperDepartId", sSysWebAddInPara.strTradeDepartId);
        jTable->SetParam(":v_OperStaffId", sSysWebAddInPara.strTradeStaffId);

        // 输出参数 错误代码
        int iRetCode = 0;
        jTable->BindParam(":v_RetCode", iRetCode);

        // 输出参数 错误信息
        char cRetErr[1024] = "";
        jTable->BindParam(":v_RetErr", cRetErr, 1024);

        jTable->ExecExt(1);

        if (iRetCode != 0)
        {
            THROW_C(BaseException, ADDWEBINFO_ERR, cRetErr);
        }

        // 返回数据
	}
	catch(Exception &e)
	{
	    string strEsg = "插入WEB信息 出错 " + e.toString();
	    THROW_C(BaseException, ADDWEBINFO_ERR, strEsg);
	}

    return iCount;
}

int SysWeb::GetWebInfo(const SysWebGetInPara& sSysWebGetInPara, SysWebGetOutPara& sSysWebGetOutPara)
{
    int iCount = 0;

    try
	{
		JConnect * pcon = jdb::getConnect();
		JTable * jTable = pcon->getJTable();
		jTable->Clear();

		jTable->AddSql("BEGIN ");
		jTable->AddSql("     p_web_getwebinfo(:v_EparchyCode, :v_CityCode, :v_DepartId, :v_StaffId, :v_GetMod, :v_InfoCount, :v_InfoStr, :v_RetCode, :v_RetErr);");
		jTable->AddSql("END; ");

        // 输入参数
        jTable->SetParam(":v_EparchyCode", sSysWebGetInPara.strTradeEparchyCode);
        jTable->SetParam(":v_CityCode", sSysWebGetInPara.strTradeCityCode);
        jTable->SetParam(":v_DepartId", sSysWebGetInPara.strTradeDepartId);
        jTable->SetParam(":v_StaffId", sSysWebGetInPara.strTradeStaffId);
        jTable->SetParam(":v_GetMod", sSysWebGetInPara.iGetMode);

        // 输出参数 信息记录数
        jTable->BindParam(":v_InfoCount", iCount);

        // 输出参数 信息串
        char cInfoStr[1024] = "";
        jTable->BindParam(":v_InfoStr", cInfoStr, 1024);

        // 输出参数 错误代码
        int iRetCode = 0;
        jTable->BindParam(":v_RetCode", iRetCode);

        // 输出参数 错误信息
        char cRetErr[1024] = "";
        jTable->BindParam(":v_RetErr", cRetErr, 1024);

        jTable->ExecExt(1);

        if (iRetCode != 0)
        {
            THROW_C(BaseException, GETWEBINFO_ERR, cRetErr);
        }

        // 返回数据
        sSysWebGetOutPara.iInfoCount = iCount;
        sSysWebGetOutPara.strInfo = cInfoStr;
	}
	catch(Exception &e)
	{
	    string strEsg = "查询WEB信息 出错 " + e.toString();
	    THROW_C(BaseException, GETWEBINFO_ERR, strEsg);
	}

    return iCount;
}
