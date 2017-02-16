#include "CrossRr.h"

ostream& operator<<(ostream& os,const CDR& po)
{
	os<<po.m_key<<" , "<<
	    po.m_sectionCode<<" , "<<
        po.m_otherParty<<" , "<<
        po.m_beginTime<<" , "<<
        po.m_endTime <<" , "<<
        po.m_callDuration<<" , "<<
        po.m_trunkGroupIn<<" , "<<
        po.m_trunkGroupOut<<" , "<<
        po.m_sameFlag;
	return os;
}

int operator == (const CDR &left,const CDR& right)
{
    return (left.m_key == right.m_key);
}     

int operator < (const CDR &left,const CDR& right)
{
    if (left.m_key < right.m_key) return true;
    if (left.m_key > right.m_key) return false;
    if (left.m_beginTime < right.m_beginTime) return true;
    return false;
}
 
CrossRr::CrossRr()
{
}

CrossRr::~CrossRr()
{
}

bool CrossRr::initialization(const DBINFO &paramDbInfo, const DBINFO &srcDbInfo, const DBINFO &desDbInfo, const string &whereClause)
{
    m_paramDbInfo = paramDbInfo;
    m_srcDbInfo   = srcDbInfo;
    m_desDbInfo   = desDbInfo;
    m_whereClause = whereClause;
    
    if(!connect())
    {
        return false;
    }
    return true;
}

bool CrossRr::connect()
{
    if(m_paramDbInterface.connect(m_paramDbInfo.m_userName.c_str(),m_paramDbInfo.m_passWord.c_str(),m_paramDbInfo.m_dbServer.c_str()))
    {
        printf("m_paramDbInterface.connect() failed.\n");
        return false;
    }
    
    if(m_srcDbInterface.connect(m_srcDbInfo.m_userName.c_str(),m_srcDbInfo.m_passWord.c_str(),m_srcDbInfo.m_dbServer.c_str()))
    {
        printf("m_srcDbInterface.connect() failed.\n");
        return false;
    }
    
    if(m_desDbInterface.connect(m_desDbInfo.m_userName.c_str(),m_desDbInfo.m_passWord.c_str(),m_desDbInfo.m_dbServer.c_str()))
    {
        printf("m_desDbInterface.connect() failed.\n");
        return false;
    }
    
    m_connected = true;
    return true;
}

bool CrossRr::setCheckInfo(const int &billCycle, const int &beginPartition, const int &endPartition)
{
    m_billCycle      = billCycle;
    m_beginPartition = beginPartition;
    m_endPartition   = endPartition;
    return true;
}

int CrossRr::getFilterUserList()
{
    StringVector outvector;
    StringVector errvector;
    StringVectorIterator it;
    char msgbuf[500];
    char *sqlStr = "SELECT MSISDN FROM TD_CROSSRR_FILTERUSER";
    int nTotal,nSuccess,nError,total;
    m_filterUserList.clear();
    m_paramDbInterface.dataBind(outvector,errvector);
    do
    {
        if(m_paramDbInterface.executeSql(sqlStr,nTotal,nSuccess,nError))
        {
            cout << "SELECTSQL : " << SELECTSQL << endl;
            sprintf(msgbuf,"dbInterface.executeSql() failed. errormessage:%s",m_paramDbInterface.errorMessage());
            cout << " msgbuf : " << msgbuf << endl;
            return -1;
        }
        if(nTotal>0)
        {
            total+=nTotal;
            for(it=outvector.begin();it!=outvector.end();)
            {
                m_filterUserList.insert((*it).c_str());
                ++it;
            }
        }
    }while(nTotal==DEFAULTARRAYLINE);
    return total;
}

int CrossRr::getData()
{
    StringVector outvector;
    StringVector errvector;
    StringVectorIterator it;
    int nTotal,nSuccess,nError,lastTotal=0,curTotal=0;
    
    time_t now;
    char msgbuf[500];
    char sqlStr[200];
    int index,i;
    time(&now);
    CDR cdr;
    //过滤用户列表
    if (getFilterUserList() < 0)
    {
        return -1;
    }
    m_srcDbInterface.dataBind(outvector,errvector);
    int lastBillCycle,lastPartition;
    //如果当前检查话单为从1号开始，则上一批次话单需要从上一批次话单中取得
    if (m_beginPartition == 1)
    {
        lastBillCycle = ((m_billCycle % 100 - 1) == 0) ? 12 : m_billCycle % 100 - 1;
        lastPartition = 31;
    }
    else
    {
        lastBillCycle = m_billCycle;
        lastPartition = m_beginPartition - 1;
    }
    //当前检查周期前一日最后两个小时跨天数据
    if (m_whereClause.length() > 0)
	{
    	sprintf(sqlStr,"%s TG_CDR%02d_FIX PARTITION(P%02d) WHERE START_DATE!=END_DATE AND SUBSTR(START_TIME,1,2) IN ('22','23') AND %s",SELECTSQL.c_str(),lastBillCycle,lastPartition,m_whereClause.c_str());
	}
	else
	{
		sprintf(sqlStr,"%s TG_CDR%02d_FIX PARTITION(P%02d) WHERE START_DATE!=END_DATE AND SUBSTR(START_TIME,1,2) IN ('22','23') ",SELECTSQL.c_str(),lastBillCycle,lastPartition);
	}
    cout << " sqlStr : " << sqlStr << endl;
    do
    {
        if(m_srcDbInterface.executeSql(sqlStr,nTotal,nSuccess,nError))
        {
            cout << "SELECTSQL : " << sqlStr << endl;
            sprintf(msgbuf,"dbInterface.executeSql() failed. errormessage:%s",m_srcDbInterface.errorMessage());
            cout << " msgbuf : " << msgbuf << endl;
            return -1;
        }
        if(nTotal>0)
        {
            for(it=outvector.begin(),i=0;it!=outvector.end();)
            {
                if (m_filterUserList.find((*it).c_str()) != m_filterUserList.end())
                {
                    it+=8;
                    continue;
                }
                cdr.m_key           = (*it).c_str();       ++it;
                cdr.m_sectionCode   = (*it).c_str();       ++it;
                cdr.m_otherParty    = (*it).c_str();       ++it;
                cdr.m_beginTime     = atol((*it).c_str()); ++it;
                cdr.m_endTime       = atol((*it).c_str()); ++it;
                cdr.m_callDuration  = atol((*it).c_str()); ++it;
                cdr.m_trunkGroupIn  = (*it).c_str();       ++it;
                cdr.m_trunkGroupOut = (*it).c_str();       ++it;
                index = (cdr.m_beginTime % 1000000) / 10000 - 22;
                m_lastData[index].push_back(cdr);
                ++lastTotal;
            }
        }
    }while(nTotal==DEFAULTARRAYLINE);
    
    for (int partition = m_beginPartition; partition <= m_endPartition; ++partition)
    {
    	if (m_whereClause.length() > 0)
		{
			sprintf(sqlStr,"%s TG_CDR%02d_FIX PARTITION(P%02d) WHERE %s ",SELECTSQL.c_str(),m_billCycle,partition,m_whereClause.c_str());
		}
		else
		{
        	sprintf(sqlStr,"%s TG_CDR%02d_FIX PARTITION(P%02d) ",SELECTSQL.c_str(),m_billCycle,partition);
		}
        cout << " sqlStr : " << sqlStr << endl;
        do
        {
            if(m_srcDbInterface.executeSql(sqlStr,nTotal,nSuccess,nError))
            {
                cout << "SELECTSQL : " << sqlStr << endl;
                sprintf(msgbuf,"dbInterface.executeSql() failed. errormessage:%s",m_srcDbInterface.errorMessage());
                cout << " msgbuf : " << msgbuf << endl;
                return -1;
            }
            if(nTotal>0)
            {
                for(it=outvector.begin(),i=0;it!=outvector.end();)
                {
                    if (m_filterUserList.find((*it).c_str()) != m_filterUserList.end())
                    {
                        it+=8;
                        continue;
                    }
                    cdr.m_key           = (*it).c_str();       ++it;
                    cdr.m_sectionCode   = (*it).c_str();       ++it;
                    cdr.m_otherParty    = (*it).c_str();       ++it;
                    cdr.m_beginTime     = atol((*it).c_str()); ++it;
                    cdr.m_endTime       = atol((*it).c_str()); ++it;
                    cdr.m_callDuration  = atol((*it).c_str()); ++it;
                    cdr.m_trunkGroupIn  = (*it).c_str();       ++it;
                    cdr.m_trunkGroupOut = (*it).c_str();       ++it;
                    index = (partition - 1) * 24 +(cdr.m_beginTime % 1000000) / 10000;
                    m_data[index].push_back(cdr);
                    ++curTotal;
                }
            }
        }while(nTotal==DEFAULTARRAYLINE);
    }
    
    //排序
    for (index = 0; index < 1; ++index)
    {
        ::sort(m_lastData[index].begin(),m_lastData[index].end());
    }
    cout << " lastData Size = " << lastTotal << endl;
    for (index = 0; index < SEGMENT_NUM; ++index)
    {
        ::sort(m_data[index].begin(),m_data[index].end());
    }
    cout << " curData Size = " << curTotal << endl;
    return lastTotal + curTotal;
}

int CrossRr::crossRrCheck()
{
    int beginIndex,lastBeginIndex=0;
    int endIndex,lastEndIndex=0;
    vector<CDR *> result;
    vector<CDR>::iterator itr;
    vector<CDR> *serachContainer;
    int index;
    //导入清单数据到内存中
    int total = getData();
    if (total < 0 )
    {
        return -1;
    }
    
    PString lastKey="0";
    for (index = 0; index < SEGMENT_NUM; ++index)
    {
        for (int i=0; i < m_data[index].size(); ++i)
        {
            //当前处理记录
            CDR &curCdr = m_data[index][i];
            //从当前记录开始时间前2个小时开始查找
            beginIndex = index >= 2 ? index -2 : 0;
            //到当前记录结束时间
            endIndex   = (((curCdr.m_endTime / 1000000) % 100) - 1) * 24 +(curCdr.m_beginTime % 1000000) / 10000;
            //如果查找关键字发生变化或者查找区间发生变化再重新查找，否则直接在上一次的结果集中遍历即可
            if ((lastKey != curCdr.m_key) || (beginIndex != lastBeginIndex) || (endIndex != lastEndIndex))
            {
                lastKey = curCdr.m_key;
                lastBeginIndex = beginIndex;
                lastEndIndex = endIndex;
                result.clear();
                for (int j = beginIndex; j <= endIndex ; ++j)
                {
                    switch (j)
                    {
                        case -2 : serachContainer = &m_lastData[0]; break;
                        case -1 : serachContainer = &m_lastData[1]; break;
                        default : serachContainer = &m_data[j];     break;
                    }
                    itr = ::lower_bound(serachContainer->begin(),serachContainer->end(),curCdr);
                    for (;itr != serachContainer->end(); ++itr)
                    {
                        if(!(*itr==curCdr))
                        {
                            break;
                        }
                        result.push_back(&(*itr));
                    }
                }
            }
            //在结果集中遍历
            for (int k=0; k < result.size(); ++k)
            {
                if (curCdr.m_beginTime >= result[k]->m_beginTime &&
                    curCdr.m_beginTime <  result[k]->m_endTime ||
                    curCdr.m_beginTime <= result[k]->m_beginTime &&
                    curCdr.m_endTime   >  result[k]->m_beginTime )
                {
                    //不是同一条记录
                    if (&curCdr != result[k])
                    {
                        //对端号码相同
                        if (curCdr.m_otherParty == result[k]->m_otherParty)
                        {
                            curCdr.m_sameFlag = '1';
                        }
                        //对端号码不同
                        else
                        {
                            curCdr.m_sameFlag = '0';
                        }
                        //保存结果
                        m_result.push_back(&curCdr);
                        break;
                    }
                }
             }
        }
    }
    cout << " Total = " << total << endl;
    return m_result.size();
}

int CrossRr::crossRrIndb()
{
    StringVector invector;
    StringVector errvector;
    StringVectorIterator it;
    int nTotal,nSuccess,nError,total,recordIndex=0;
    m_desDbInterface.dataBind(invector,errvector);
    string tmp,beginDate;
    cout << "has CrossRr Cdr " << m_result.size() << " Rows." << endl;
    cout << "indb Begin" << endl;
    for (int i=0; i < m_result.size(); ++i)
    {
        invector.push_back(m_result[i]->m_key.c_str());
        invector.push_back(m_result[i]->m_sectionCode.c_str());
        invector.push_back(m_result[i]->m_otherParty.c_str());
        beginDate = ltoa(m_result[i]->m_beginTime);
        invector.push_back(beginDate);
        invector.push_back(ltoa(m_result[i]->m_endTime));
        invector.push_back(ltoa(m_result[i]->m_callDuration));
        invector.push_back(m_result[i]->m_trunkGroupIn.c_str());
        invector.push_back(m_result[i]->m_trunkGroupOut.c_str());
        tmp = m_result[i]->m_sameFlag;
        invector.push_back(tmp);
        invector.push_back(beginDate.substr(4,2));
        ++recordIndex;
        if(recordIndex==DEFAULTARRAYLINE)
        {
            m_desDbInterface.bulkCopy(INSERTSQL.c_str(),nTotal,nSuccess,nError);
            recordIndex = 0;
            invector.clear();
        }
    }
    if (recordIndex > 0)
    {
        m_desDbInterface.bulkCopy(INSERTSQL.c_str(),nTotal,nSuccess,nError);
    }
    cout << "indb End..." << endl;
    return 0;
}
