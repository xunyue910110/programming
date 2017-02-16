#include "config-all.h"

#include "base/SysParam.h"
#include "appfrm/ErrorHandle.h"
#include "OutDb.h"
#include "base/Directory.h"
#include "log4cpp/Logger.h"
#include "OutDbError.h"

#include <assert.h>
#include <unistd.h>
#include <iostream>

USING_NAMESPACE(std)

///必须配置的参数
#define USER            "USER"
#define PASSWD          "PASSWD"
#define SERVER          "SERVER"
#define SQLSTATEMENT    "SQLSTATEMENT"
#define FIELDNUM        "FIELDNUM"
#define TARGETPATH      "TARGETPATH"
///选择配置的参数
#define DATABASE        "DATABASE"
#define SWAPPATH        "SWAPPATH"
#define FIELDLENGTH     "FIELDLENGTH"
#define ARRAYLINE       "ARRAYLINE"
#define OUTPUTFILE      "OUTPUTFILE"
#define PROCEDURE       "PROCEDURE"
#define PROCSTOPCODE    "PROCSTOPCODE"
#define TRUNCATETABLE   "TRUNCATETABLE"
#define FIELDSEP        "FIELDSEP"


OutDb g_application;
Application * const theApp = &g_application;

bool OutDb::initialization()
{
    setoptstr("c:f:");
    if (!Application::initialization())
    {
        return false;
    }

    #ifndef _DEBUG_
        set_asdaemon(true);
        set_onlyone(true);
        set_runinbkg(true);
    #endif

    if ((!GetSysParam()) || (!ConnectDb()))
    {
        return false;
    }

    m_reserved = "";
    m_objectName = "";
    updateStatus();

    return true;
}

bool OutDb::end()
{
    m_dbi.disconnect();
    return Application::end();
}

bool OutDb::loopProcess()
{
    if (GetFileName() && m_fileName != " ")
    {
        WriteStat(m_fileName.c_str(), PROC_RUN);

        theLoggerProcessID = theLogger->pBegin();
        theLogger->pInput(theLoggerProcessID,m_paramMap[SQLSTATEMENT],m_nStatis,"type='indb'");
        if (ProcessFile())
        {
	          theLogger->pOutput(theLoggerProcessID,m_fileName,m_nStatis);
        }
        theLogger->pEnd(theLoggerProcessID);
        writeLog();
    }

    writeAlert();
	  m_status = PROC_IDLE;
	  m_objectName = "";
	  updateStatus();
    sleep(30);

    return Application::loopProcess();
}

bool OutDb::GetFileName()
{
    if (m_paramMap[PROCEDURE] != "")
    {
        m_procParam[0].action   = PARAM_ACT_IN;
        m_procParam[0].type     = PARAM_TYPE_INT;
        m_procParam[0].intValue = m_channelNo;
        m_procParam[1].action   = PARAM_ACT_OUT;
        m_procParam[1].type     = PARAM_TYPE_STR;
        strcpy(m_procParam[1].strValue, "");

        m_nReturn = m_dbi.callProcedure
            (m_paramMap[PROCEDURE].c_str(), 2, m_procParam);

        if (m_nReturn == atoi(m_paramMap[PROCSTOPCODE].c_str()))
        {
            m_stop = 1;
            return false;
        }
        else if (m_nReturn)
        {
            sprintf(m_errorMsg,
                "Procedure(%s) failed. return:%d,errno=%d,errtext=%s",
                m_paramMap[PROCEDURE].c_str(),
                m_nReturn, m_nError, m_procParam[0].strValue);
            m_stop = 1;

            return false;
        }

        m_fileName = m_procParam[1].strValue;
        return true;
    }

    m_stop = 1;
    return true;
}

bool OutDb::ProcessFile()
{
    static string strTemp1, strTemp2;

    if (!RecordOut())
    {
        WriteAlarm(ERROR_OUTDB_PROCFILE, m_errorMsg);
        return false;
    }

    if (m_paramMap[TRUNCATETABLE] != "")
    {
        strTemp1 = "TRUNCATE TABLE ";
        strTemp1 += m_paramMap[TRUNCATETABLE];
        if (m_dbi.executeSql(strTemp1.c_str(),
                m_nTotal, m_nSuccess, m_nError))
        {
            WriteAlarm(ERROR_OUTDB_EXECSQL, strTemp1.c_str());
            return false;
        }
    }

    if (m_paramMap[SWAPPATH] != "")//有交换目录
    {
        strTemp1 = m_paramMap[SWAPPATH] + m_fileName;
        strTemp2 = m_paramMap[TARGETPATH] + m_fileName;
        if (rename(strTemp1.c_str(), strTemp2.c_str()))
        {
            sprintf(m_errorMsg,"Rename(%s,%s) failed",
                   strTemp1.c_str(), strTemp2.c_str());
            WriteAlarm(ERROR_OUTDB_MOVEFILE,m_errorMsg);
            return false;
        }
    }
    return true;
}

bool OutDb::RecordOut()
{
    static string outputFile;
    FILE *hOutputFile = NULL;

    m_dbi.dataBind(m_outVector, m_errVector);

    if (m_paramMap[SWAPPATH] != "")
    {
        outputFile = m_paramMap[SWAPPATH] + m_fileName;
    }
    else
    {
        outputFile = m_paramMap[TARGETPATH] + m_fileName;
    }

    if ((hOutputFile = fopen(outputFile.c_str(), "wb")) == NULL)
    {
        sprintf(m_errorMsg, "OnputFile %s open failed", outputFile.c_str());
        return false;
    }
    m_fieldNum = atoi(m_paramMap[FIELDNUM].c_str());

    m_nStatis = 0;
    m_nTotal = DEFAULTARRAYLINE;
    while(m_nTotal == DEFAULTARRAYLINE)
    {
        if (m_dbi.executeSql(m_paramMap[SQLSTATEMENT].c_str(),
                m_nTotal, m_nSuccess, m_nError))
        {
            if (hOutputFile)
            {
                fclose(hOutputFile);
            }
            sprintf(m_errorMsg, "SQL error: %s.",
                m_paramMap[SQLSTATEMENT].c_str());
            return false;
        }
        else
        {
            if (m_nTotal > 0)
            {
                for (m_recordIter = m_outVector.begin(), m_fieldIndex = 0;
                     m_recordIter != m_outVector.end(); m_recordIter++)
                {
                    m_fieldIndex++;
                    fprintf(hOutputFile, "%s", (*m_recordIter).c_str());
                    if (m_fieldIndex < m_fieldNum)
                    {
                        fprintf(hOutputFile, m_paramMap[FIELDSEP].c_str());
                    }
                    else if (m_fieldIndex == m_fieldNum)
                    {
                        m_nStatis++;
                        fprintf(hOutputFile, "\n");
                        m_fieldIndex = 0;
                    }
                }
            }
        }
    }

    if (hOutputFile)
    {
        fclose(hOutputFile);
    }
    m_dbi.endTransaction();

    return true;
}


bool OutDb::GetSysParam()
{
    char channelNo[10];
    string sectionPath;

    if (m_channelNo >= 0)
    {
        sprintf(channelNo, "%d", m_channelNo);
        sectionPath = SECTDELIM;
        sectionPath += m_name;
        sectionPath += SECTDELIM;
        sectionPath += channelNo;
    }
    else
    {
        sectionPath = "";
    }

    if (!m_theSysParam->getValue(sectionPath, m_paramMap))
    {
        sprintf(m_errorMsg, "Get general param failed,path is %s.",
                sectionPath.c_str());
        WriteAlarm(ERROR_OUTDB_PARAM, m_errorMsg);
        return false;
    }

    if (    m_paramMap[USER] == ""
        ||  m_paramMap[PASSWD] == ""
        ||  m_paramMap[SERVER] == ""
        ||  m_paramMap[SQLSTATEMENT] == ""
        ||  m_paramMap[FIELDNUM] == ""
        ||  m_paramMap[TARGETPATH] == ""
       )
    {
        sprintf(m_errorMsg, "Param error:some value is null!");
        WriteAlarm(ERROR_OUTDB_PARAM, m_errorMsg);
        return false;
    }

    if ((m_paramMap[PROCEDURE] != "") && (m_paramMap[PROCSTOPCODE] == ""))
    {
        sprintf(m_errorMsg, "Param error:PROCEDURE is not null but PROCSTOPCODE is null!");
        WriteAlarm(ERROR_OUTDB_PARAM, m_errorMsg);
        return false;
    }

    if(  m_paramMap[TARGETPATH].length()
      && m_paramMap[TARGETPATH][m_paramMap[TARGETPATH].length() -1 ] != '/')
    {
        m_paramMap[TARGETPATH] += "/";
    }

    if(  m_paramMap[SWAPPATH].length()
      && m_paramMap[SWAPPATH][m_paramMap[SWAPPATH].length() -1 ] != '/')
    {
        m_paramMap[SWAPPATH] += "/";
    }

    if (m_paramMap[FIELDSEP] == "")
    {
        m_paramMap[FIELDSEP] = ",";
    }
    else if (m_paramMap[FIELDSEP][0] == '\\')
    {
        string strTemp;
        strTemp = atoi(m_paramMap[FIELDSEP].substr(1).c_str());
        m_paramMap[FIELDSEP] = strTemp;
    }

    if (m_paramMap[OUTPUTFILE] != "")
    {
        m_fileName = m_paramMap[OUTPUTFILE];
    }
    else
    {
        m_fileName = "outputfile.dat";
    }

    return true;
}


bool OutDb::ConnectDb()
{
    if (m_dbi.connect(m_paramMap[USER].c_str(),
                     m_paramMap[PASSWD].c_str(),
                     m_paramMap[SERVER].c_str()))
    {
        WriteAlarm(ERROR_OUTDB_DBCONN,"Database connect failed!");
        return false;
    }
    else if (m_paramMap[DATABASE] != "")
    {
        if (m_dbi.useDb(m_paramMap[DATABASE].c_str()))
        {
            WriteAlarm(ERROR_OUTDB_DBCONN,"Use db failed!");
            return false;
        }
    }
    if (m_paramMap[ARRAYLINE] != "")
    {
        m_dbi.setArrayLine(atoi(m_paramMap[ARRAYLINE].c_str()));
    }

    m_dbi.beginTransaction();

    return true;
}

void OutDb::WriteStat(const char *msg,const char stat)
{
    m_objectName = msg;
    m_status = stat;
    updateStatus();
}

void OutDb::WriteAlarm(const int ecode,const char *msg)
{
    m_theErrorMessages->insert(ecode, msg);
    writeAlert();
}

