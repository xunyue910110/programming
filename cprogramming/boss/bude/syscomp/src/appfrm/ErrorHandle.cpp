

#include "ErrorHandle.h"

#include <string.h>



// ERRORINFO implementation
ERRORINFO::ERRORINFO(const ERRORINFO& right)
{
    no = right.no;
    strcpy(message, right.message);
}

const ERRORINFO& ERRORINFO::operator=(const ERRORINFO& right)
{
    no = right.no;
    strcpy(message, right.message);
    return *this;
}

void ErrorMessages::setApplicationInfo(const string& applicationName,
                                       const int channelNo)
{
    m_applicationName = applicationName;
    m_channelNo = channelNo;
}

void ErrorMessages::getApplicationInfo(string& applicationName, int channelNo)
{
    applicationName = m_applicationName;
    channelNo = m_channelNo;
}

void ErrorMessages::insert(ERRORINFO& errInfo)
{
    m_errInfoQueue.push_back(errInfo);
}

void ErrorMessages::insert(const int errorCode, const string& errorMessage)
{
    ERRORINFO errInfo;
    errInfo.no = errorCode;
    if (errorMessage.length() <= ERRORMSG_LEN)
        strcpy(errInfo.message, errorMessage.c_str());
    else
    {
        strncpy(errInfo.message, errorMessage.c_str(), ERRORMSG_LEN);
        errInfo.message[ERRORMSG_LEN] = 0;
    }
    insert(errInfo);
}

void ErrorMessages::insert(const int errorCode, const char *errorMessage)
{
    ERRORINFO errInfo;
    errInfo.no = errorCode;
  
    if (strlen(errorMessage) <= ERRORMSG_LEN)
        strcpy(errInfo.message, errorMessage);
    else
    {
        strncpy(errInfo.message, errorMessage, ERRORMSG_LEN);
        errInfo.message[ERRORMSG_LEN] = 0;
    }
    insert(errInfo);
}

bool ErrorMessages::get(ERRORINFO& errInfo)
{
    if (m_errInfoQueue.empty())
        return false;
  
    errInfo = m_errInfoQueue.front();
    m_errInfoQueue.pop_front();
    return true;
}

void ErrorMessages::clean()
{
    m_errInfoQueue.clear();
}

bool ErrorMessages::isEmpty()
{
    return m_errInfoQueue.empty();
}

