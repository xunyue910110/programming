
#include "LocalBill.h"

#include "base/Directory.h"

#ifdef MSVCPP_CONFIG
#include <io.h>
#define access _access
#define chmod  _chmod
#define link(x,y) false
#define F_OK   0
#define W_OK   2
#define R_OK   4
#define S_IRUSR _S_IREAD
#define S_IWUSR _S_IWRITE
#define S_IRGRP 0
#define S_IWGRP 0
#define S_IROTH 0
#define S_IWOTH 0

#endif

#include <sys/stat.h>
#include <errno.h>
USING_REL_OPS_NAMESPACE


#if defined(TIME_TRACE)
void TRACETIME(const char *msg)
{
  static time_t tb = time(&tb);
  static time_t te = tb;
  time(&te);
  cout << "time: " << difftime(te, tb) << "\t" << msg << endl;
  tb = te;
}
#endif


const string REALTIME_BILLFILE_PREFIX = "R";
const string DAY_BILLFILE_PREFIX = "D";
const string CURRENT_TOTALBILLFILE_PREFIX = "C";
const string TMP_PREFIX = "%";

//Added By Sunam For OCS 2006-6-14 13:08 
const string OCS_REALTIME_BILLFILE_PREFIX = "BTO";
const string OCS_DAY_BILLFILE_PREFIX = "BTOD";

//theErrorMessages->insert(EAPPFRM_ARG, emsg);

void writeErrMsg(const int line, const int errorno, const char * msg)
{
  fstream errout("/tmp/errmsg", ios::out|ios::app) ;
  errout << line << ": " << strerror(errorno) << endl;
  errout << msg << endl;
}


//--------------------------------------------------------------------------//
// CHARGEITEM
int CHARGEITEM::count = 0;

ostream& operator << (ostream& out, const CHARGEITEM& charge)
{
  return out << charge.type << "\t" << charge.value;
}

istream& operator >> (istream& in, CHARGEITEM& charge)
{
  return in >> charge.type >> charge.value;
}

//--------------------------------------------------------------------------//
// Serv
int Serv::count = 0;

ostream& operator << (ostream& out, const Serv& serv)
{
  for (Bill::const_iterator i = serv.m_bill.begin(), e = serv.m_bill.end();
       i != e; ++i)
    out << serv.m_servId << "\t" << *i << endl;
  return out;
}


//--------------------------------------------------------------------------//
// BILL_OK      = 0;    - OK
// BILL_REVOK   = 1;    - recovery ok
// BILL_LOGERR  = -1;   - no bill log file
// BILL_FILEERR = -2;   - bill file error
int LocalBill::check(int cycle, int channelno)
{
  // is new bill cycle ?
  //  new cycle：init as empty
  //  current cycle：using today DAY-BILL-FILE ＋INCREASE-BILL-FILE
  char tmp[10];
  sprintf(tmp, "%03d", channelno);
  tmp[3] = 0;
  m_channelno = tmp;
  sprintf(tmp, "%06d", cycle);
  tmp[6] = 0;
  m_currentCycle = tmp + m_channelno;

  m_billLogfile = m_billLogFilePath + m_channelno + "BILL.LOG";
  
  // search current TOTAL-BILL-FILE and INCREASE-BILL-FILE
  m_totalBillFileName = m_totalBillFilePath + CURRENT_TOTALBILLFILE_PREFIX
    + m_currentCycle+ "TOTAL.BILL";

  m_dayBillFilenameBak = m_totalBillFilePath + DAY_BILLFILE_PREFIX
    + m_currentCycle;
  m_incrBillFilenameBak = m_totalBillFilePath + REALTIME_BILLFILE_PREFIX
    + m_currentCycle;
  
	//Added By Sunam For OCS 2006-6-14 13:00
	if (m_ocsBillFilePath.size() > 0)
	{
		m_ocsBillLogfile = m_billLogFilePath + m_channelno + "OCSBILL.LOG";
		m_ocsBillFileNameBak = m_totalBillFilePath + OCS_REALTIME_BILLFILE_PREFIX + m_currentCycle;
	}

  /*
   * read info from logfile
   */
  if (access(m_billLogfile.c_str(), F_OK|R_OK))
  {
    string logbak = m_billLogfile + "@";
    if (!access(logbak.c_str(), F_OK|R_OK))
    {
      if (rename(logbak.c_str(), m_billLogfile.c_str()))
        return BILL_LOGERR;
    }
    else
      return BILL_LOGERR;
  }
  
  // open bill log file
  m_fstreamLog.clear();
  m_fstreamLog.open(m_billLogfile.c_str(), ios::in);
  if (m_fstreamLog.rdstate() != ios::goodbit)
    return BILL_LOGERR;
    
  string line;
  bool isNewCycle = true;
  //for port to win32
  if (!(!m_fstreamLog))
  //if ( m_fstreamLog.is_open() )
  {
    m_fstreamLog >> line;

    if (line.size() != m_currentCycle.size())
      return BILL_LOGERR;
    if (line == m_currentCycle)
      isNewCycle = false;
  }
  
	//Added By Sunam For OCS 2006-6-14 13:00
	if (m_ocsBillFilePath.size() > 0)
	{
		if (access(m_ocsBillLogfile.c_str(), F_OK|R_OK))
		{
			return BILL_LOGERR;
		}
		m_ocsFstreamLog.clear();
		m_ocsFstreamLog.open(m_ocsBillLogfile.c_str(), ios::in);
		if (m_ocsFstreamLog.rdstate() != ios::goodbit)
		{
			return BILL_LOGERR;
		}
		if (!(!m_ocsFstreamLog))
		{
			m_ocsFstreamLog >> line;
			if (line.size() != m_currentCycle.size())
			{
				return BILL_LOGERR;
			}
		}
		while(!(!m_ocsFstreamLog))
		{
			m_ocsFstreamLog >> line;
		}
		if (line.size() == m_currentCycle.size())
		{
			m_ocsSequenceNo = 0;
		}
		else if ((line.size() > m_ocsBillFileNameBak.size()) &&
			(!line.compare(0, m_ocsBillFileNameBak.size(), m_ocsBillFileNameBak)))
		{
			if ((line.size() - m_ocsBillFileNameBak.size() - DAY_LEN) != BILLSEQUENCE_LEN)
			{
				return BILL_LOGERR;
			}
			strcpy(tmp, line.c_str() + m_ocsBillFileNameBak.size() + DAY_LEN);
      		m_ocsSequenceNo = atoi(tmp);
      		m_ocsSequenceNo ++;
		}
		m_ocsFstreamLog.close();
	}

  // if exist last total bill, using it
  if (!access(m_totalBillFileName.c_str(), F_OK|R_OK))
  {
    //get sequence number
    //for port to win32
    while(!(!m_fstreamLog))
          m_fstreamLog >> line;

    if ((line.size() > m_incrBillFilenameBak.size())
        && (!line.compare(0, m_incrBillFilenameBak.size(),
                          m_incrBillFilenameBak)))
    {
      if ((line.size() - m_incrBillFilenameBak.size() - DAY_LEN)
          != BILLSEQUENCE_LEN)
        return BILL_LOGERR;

      strcpy(tmp, line.c_str() + m_incrBillFilenameBak.size() + DAY_LEN);
      m_sequenceNo = atoi(tmp);
      m_sequenceNo++;
    }
    m_fstreamLog.close();
    return BILL_OK;
  }

  if (isNewCycle)
  {
    // write log for revovery
    m_fstreamLog.close();
    string logbak = m_billLogfile + "@";
    unlink(logbak.c_str());
    rename(m_billLogfile.c_str(), logbak.c_str());
    //link(m_billLogfile.c_str(), logbak.c_str());

    m_fstreamLog.clear();
    m_fstreamLog.open(m_billLogfile.c_str(), ios::trunc|ios::out|ios::ate);
    if (m_fstreamLog.rdstate() != ios::goodbit)
    {
      unlink(m_billLogfile.c_str());
      m_fstreamLog.close();
      return BILL_LOGERR;
    }
    m_fstreamLog << m_currentCycle << endl;
    m_fstreamLog.flush();
    m_fstreamLog.close();
    m_sequenceNo = 0;
    
	//Added By Sunam For OCS 2006-6-14 13:00
	if (m_ocsBillFilePath.size() > 0)
	{
		m_ocsFstreamLog.clear();
		m_ocsFstreamLog.open(m_ocsBillLogfile.c_str(), ios::trunc|ios::out|ios::ate);
		if (m_ocsFstreamLog.rdstate() != ios::goodbit)
		{
			unlink(m_billLogfile.c_str());
			m_ocsFstreamLog.close();
			return BILL_LOGERR;
		}
		m_ocsFstreamLog << m_currentCycle << endl;
		m_ocsFstreamLog.flush();
		m_ocsFstreamLog.close();
		m_ocsSequenceNo = 0;
	}
    return BILL_OK;
  }
  else
  { // current bill cycle recovery
    //for port to win32
    //if (m_fstreamLog)
    if (!(!m_fstreamLog))
    {
      m_fstreamLog >> line;

      if (line.size() > m_dayBillFilenameBak.size())
      {
        if (!line.compare(0, m_dayBillFilenameBak.size(),
                          m_dayBillFilenameBak))
        {
          if (!initTotalBillFromFile(line.c_str()))
          {
            m_fstreamLog.close();
            return BILL_FILEERR;
          }
        }
        else
        {
          if (!initFromIncreaseFile(line.c_str()))
          {
            m_fstreamLog.close();
            return BILL_FILEERR;
          }
        }
      }
    }

	//for port to win32
    //while(m_fstreamLog)
    while(!(!m_fstreamLog))
    {
      m_fstreamLog >> line;
      if (!initFromIncreaseFile(line.c_str()))
      {
        m_fstreamLog.close();
        return BILL_FILEERR;
      }
    }
  }

  // no last total bill file, new month account or need recovery
  if (!LocalBill::end())
  {
    m_fstreamLog.close();
    return BILL_FILEERR;
  }
  return BILL_REVOK;
}


int LocalBill::checkex(int cycle, int channelno)
{
  // is new bill cycle ?
  //  new cycle：init as empty
  //  current cycle：using today DAY-BILL-FILE ＋INCREASE-BILL-FILE
  char tmp[10];
  sprintf(tmp, "%03d", channelno);
  tmp[3] = 0;
  m_channelno = tmp;
  sprintf(tmp, "%06d", cycle);
  tmp[6] = 0;
  m_currentCycle = tmp + m_channelno;

  m_billLogfile = m_billLogFilePath + m_currentCycle + "BILL.LOG";

  // search current TOTAL-BILL-FILE and INCREASE-BILL-FILE
  m_totalBillFileName = m_totalBillFilePath + CURRENT_TOTALBILLFILE_PREFIX
    + m_currentCycle+ "TOTAL.BILL";
  m_dayBillFilenameBak = m_totalBillFilePath + DAY_BILLFILE_PREFIX
    + m_currentCycle;
  m_incrBillFilenameBak = m_totalBillFilePath + REALTIME_BILLFILE_PREFIX
    + m_currentCycle;

	//Added By Sunam For OCS 2006-6-14 13:00
	if (m_ocsBillFilePath.size() > 0)
	{
		m_ocsBillLogfile = m_billLogFilePath + m_channelno + "OCSBILL.LOG";
		m_ocsBillFileNameBak = m_totalBillFilePath + OCS_REALTIME_BILLFILE_PREFIX + m_currentCycle;
	}

  // no log file exist 
  if (access(m_billLogfile.c_str(), F_OK|R_OK))
  {
    string logbak = m_billLogfile + "@";
    if (!access(logbak.c_str(), F_OK|R_OK))
    {
      if (rename(logbak.c_str(), m_billLogfile.c_str()))
        return BILL_LOGERR;
    }
    else
      //return BILL_LOGERR;
      //create new bill log file
      m_fstreamLog.close();  
      m_fstreamLog.clear();
      m_fstreamLog.open(m_billLogfile.c_str(), ios::trunc|ios::out|ios::ate);
      if (m_fstreamLog.rdstate() != ios::goodbit)
      {
        m_fstreamLog.close();        
        return BILL_LOGERR;
      }  
      m_fstreamLog << cycle << endl ;
      m_fstreamLog.close();
  }

  // open bill log file
  m_fstreamLog.close();
  m_fstreamLog.clear();
  m_fstreamLog.open(m_billLogfile.c_str(), ios::in);
  if (m_fstreamLog.rdstate() != ios::goodbit)
    return BILL_LOGERR;

  string line;
  bool isNewCycle = true;

  if (!(!m_fstreamLog))
  {
    m_fstreamLog >> line;

    //if (line.size() != m_currentCycle.size())
    //  return BILL_LOGERR;
    if (line == m_currentCycle)
      isNewCycle = false;
  }
  
  	//Added By Sunam For OCS 2006-6-14 13:00
	if (m_ocsBillFilePath.size() > 0)
	{
		if (access(m_ocsBillLogfile.c_str(), F_OK|R_OK))
		{
			return BILL_LOGERR;
		}
		m_ocsFstreamLog.clear();
		m_ocsFstreamLog.open(m_ocsBillLogfile.c_str(), ios::in);
		if (m_ocsFstreamLog.rdstate() != ios::goodbit)
		{
			return BILL_LOGERR;
		}
		if (!(!m_ocsFstreamLog))
		{
			m_ocsFstreamLog >> line;
			if (line.size() != m_currentCycle.size())
			{
				return BILL_LOGERR;
			}
		}
		while(!(!m_ocsFstreamLog))
		{
			m_ocsFstreamLog >> line;
		}
		if (line.size() == m_currentCycle.size())
		{
			m_ocsSequenceNo = 0;
		}
		if ((line.size() > m_ocsBillFileNameBak.size()) &&
			(!line.compare(0, m_ocsBillFileNameBak.size(), m_ocsBillFileNameBak)))
		{
			if ((line.size() - m_ocsBillFileNameBak.size() - DAY_LEN) != BILLSEQUENCE_LEN)
			{
				return BILL_LOGERR;
			}
			strcpy(tmp, line.c_str() + m_ocsBillFileNameBak.size() + DAY_LEN);
      		m_ocsSequenceNo = atoi(tmp);
      		m_ocsSequenceNo ++;
		}
		m_ocsFstreamLog.close();
	}

  // if exist last total bill, using it
  if (!access(m_totalBillFileName.c_str(), F_OK|R_OK))
  {
    //get sequence number
    line ="";
    while(!(!m_fstreamLog))
          m_fstreamLog >> line;

    if ((line.size() > m_incrBillFilenameBak.size())
        && (!line.compare(0, m_incrBillFilenameBak.size(),
                          m_incrBillFilenameBak)))
    {
      if ((line.size() - m_incrBillFilenameBak.size() - DAY_LEN)
          != BILLSEQUENCE_LEN)
        return BILL_LOGERR;

      strcpy(tmp, line.c_str() + m_incrBillFilenameBak.size() + DAY_LEN);
      m_sequenceNo = atoi(tmp);
      m_sequenceNo++;
    }
    if (line == "") m_sequenceNo=0;
    
    m_fstreamLog.close();
    m_localdata.clear();
    return BILL_OK;
  }

  if (isNewCycle)
  {
    // write log for revovery
    m_fstreamLog.close();
    string logbak = m_billLogfile + "@";
    unlink(logbak.c_str());
    rename(m_billLogfile.c_str(), logbak.c_str());

    m_fstreamLog.clear();
    m_fstreamLog.open(m_billLogfile.c_str(), ios::trunc|ios::out|ios::ate);
    if (m_fstreamLog.rdstate() != ios::goodbit)
    {
      unlink(m_billLogfile.c_str());
      m_fstreamLog.close();
      return BILL_LOGERR;
    }
    m_fstreamLog << m_currentCycle << endl;
    m_fstreamLog.flush();
    m_fstreamLog.close();
    m_sequenceNo = 0;
    
    //Added By Sunam For OCS 2006-6-14 13:00
	if (m_ocsBillFilePath.size() > 0)
	{
		m_ocsFstreamLog.clear();
		m_ocsFstreamLog.open(m_ocsBillLogfile.c_str(), ios::trunc|ios::out|ios::ate);
		if (m_ocsFstreamLog.rdstate() != ios::goodbit)
		{
			unlink(m_billLogfile.c_str());
			m_ocsFstreamLog.close();
			return BILL_LOGERR;
		}
		m_ocsFstreamLog << m_currentCycle << endl;
		m_ocsFstreamLog.flush();
		m_ocsFstreamLog.close();
		m_ocsSequenceNo = 0;
	}
    
    return BILL_OK;
  }
  else
  { // current bill cycle recovery
    //for port to win32
    //if (m_fstreamLog)
    if (!(!m_fstreamLog))
    {
      m_fstreamLog >> line;

      if (line.size() > m_dayBillFilenameBak.size())
      {
        if (!line.compare(0, m_dayBillFilenameBak.size(),
                          m_dayBillFilenameBak))
        {
          if (!initTotalBillFromFile(line.c_str()))
          {
            m_fstreamLog.close();
            return BILL_FILEERR;
          }
        }
        else
        {
          if (!initFromIncreaseFile(line.c_str()))
          {
            m_fstreamLog.close();
            return BILL_FILEERR;
          }
        }
      }
    }

	//for port to win32
    //while(m_fstreamLog)
    while(!(!m_fstreamLog))
    {
      m_fstreamLog >> line;
      if (!initFromIncreaseFile(line.c_str()))
      {
        m_fstreamLog.close();
        return BILL_FILEERR;
      }
    }
  }

  // no last total bill file, new month account or need recovery
  if (!LocalBill::end())
  {
    m_fstreamLog.close();
    return BILL_FILEERR;
  }
  return BILL_REVOK;
}


//--------------------------------------------------------------------------//
bool LocalBill::initialization(int cycle, int channelno)
{
  char tmp[10];
  sprintf(tmp, "%03d", channelno);
  tmp[3] = 0;
  m_channelno = tmp;
  sprintf(tmp, "%06d", cycle);
  tmp[6] = 0;
  m_currentCycle = tmp + m_channelno;

  struct tm * tmNow;
  time_t theDate;
  time(&theDate);
  tmNow = localtime(&theDate);

  sprintf(m_dealtime, "%02d%02d%02d%02d%02d%02d", tmNow->tm_year - 100,
          tmNow->tm_mon + 1,
          tmNow->tm_mday, tmNow->tm_hour,
          tmNow->tm_min, tmNow->tm_sec);
  m_dealtime[DATETIMELEN] = 0;

  // search current total bill file and increase bill file
  m_totalBillFileName = m_totalBillFilePath + CURRENT_TOTALBILLFILE_PREFIX
    + m_currentCycle+ "TOTAL.BILL";
  string tmpTotalBillFilename = m_totalBillFileName + ".OLD";

  m_dayBillFilename = m_increaseBillFilePath + DAY_BILLFILE_PREFIX
    + m_currentCycle;
  m_dayBillFilenameBak = m_totalBillFilePath + DAY_BILLFILE_PREFIX
    + m_currentCycle;
  m_incrBillFilename = m_increaseBillFilePath + REALTIME_BILLFILE_PREFIX
    + m_currentCycle;
  m_incrBillFilenameBak = m_totalBillFilePath + REALTIME_BILLFILE_PREFIX
    + m_currentCycle;
    
  //Added by Sunam 2005-11-28 FOR DR
  if (m_drBillFilePath.length() > 0)
  {
  	m_drBillFileName = m_drBillFilePath + REALTIME_BILLFILE_PREFIX + m_currentCycle;
  	m_dayBillFilenameDr = m_drBillFilePath + DAY_BILLFILE_PREFIX + m_currentCycle;
  }

  m_servVector.reserve(m_servReserve);
  Bill::reserve(m_headReserve, m_dataReserve);

  if (!access(m_totalBillFileName.c_str(), F_OK|R_OK))
  { // if exist last total bill, using it
    if (!initTotalBillFromFile(m_totalBillFileName.c_str()))
      return false;

    if (!access(tmpTotalBillFilename.c_str(), F_OK|R_OK))
    {
      if (unlink(tmpTotalBillFilename.c_str()))
        return false;
    }
    if (rename(m_totalBillFileName.c_str(), tmpTotalBillFilename.c_str()))
      return false;
  }
  else
  { // no last total bill file, new month account
    // new bill cycle startup
    m_sequenceNo = 0;
  }

  // open bill log file
  m_fstreamLog.clear();
  m_fstreamLog.open(m_billLogfile.c_str(), ios::out|ios::app);
  if (m_fstreamLog.rdstate() != ios::goodbit)
    return false;
    
	//Added By Sunam For OCS 2006-6-14 11:56
	if (m_ocsBillFilePath.size() > 0 )
	{
		m_ocsBillFileName = m_ocsBillFilePath + OCS_REALTIME_BILLFILE_PREFIX + m_currentCycle;
		m_ocsBillFileNameBak = m_totalBillFilePath + OCS_REALTIME_BILLFILE_PREFIX + m_currentCycle;
		m_dayBillFileNameOcs = m_ocsBillFilePath + OCS_DAY_BILLFILE_PREFIX + m_currentCycle; 
		m_ocsFstreamLog.clear();
		m_ocsFstreamLog.open(m_ocsBillLogfile.c_str(), ios::out|ios::app);
		if (m_ocsFstreamLog.rdstate() != ios::goodbit)
		{
			return false;
		}
	}
	return true;
}


bool LocalBill::end()
{
  // update total bill file
  if (!writeTotalFileBill(m_totalBillFileName.c_str()))
    return false;
  
  // CHG_20021110: update by WGW, 2002/11/10, clean memory contents
  m_servVector.resize(0);    
  m_servIndex.reset(m_servVector.begin(), m_servVector.end());
  if (!(!m_fstreamLog))
    m_fstreamLog.close();
  m_isFirstOnce = true;  
  // CHG_20021110 end
  
  Bill::reset();
  return true;
}


bool LocalBill::fileInit(const char * dealdate)   //dealdate: YYYYMMDDHHMMSS
{
  m_localdata.clear();
  m_isNewServAdd = false;

  if (m_isFirstOnce)
  {
    strncpy(m_day, dealdate + 2, DAY_LEN);
    m_day[DAY_LEN] = 0;
    string daybillfile = m_dayBillFilename + m_day;
    string daybillfileBak = m_dayBillFilenameBak + m_day;

    // if day bill file bak exist
    if (!access(daybillfileBak.c_str(), F_OK|R_OK))
      m_isFirstOnce = false;
    //added by zhanglong 20060706
    //第一次启动的时候就用传进的参数来给m_dealtime赋值
    strncpy(m_dealtime, dealdate + 2, DATETIMELEN);
    m_dealtime[DATETIMELEN] = 0;
  }

  if (strncmp(m_dealtime, dealdate + 2, DAY_LEN) || m_isFirstOnce)
  {// change day, write daily total bill file
    m_sequenceNo = 0;
    //Added by Sunam 2006-6-15 9:11
    m_ocsSequenceNo = 0;

    strncpy(m_day, dealdate + 2, DAY_LEN);
    m_day[DAY_LEN] = 0;
    string daybillfile = m_dayBillFilename + m_day;
    string daybillfileBak = m_dayBillFilenameBak + m_day;
	string dayBillFileDr;
	//Added by Sunam 2005-11-28 FOR DR
	if (m_drBillFilePath.length() > 0)
	{
    	dayBillFileDr = m_dayBillFilenameDr + m_day;
    }
    //Added by Sunam 2005-11-28 FOR DR

    if (!writeTotalFileBill(daybillfileBak.c_str()))
      return false;

    if (daybillfileBak != daybillfile)
    {
      if (link(daybillfileBak.c_str(), daybillfile.c_str()))
      {
        unlink(daybillfileBak.c_str());
        unlink(daybillfile.c_str());
        return false;
      }
      if (chmod(daybillfile.c_str(),
                S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))
      {
        unlink(daybillfileBak.c_str());
        unlink(daybillfile.c_str());
        return false;
      }
      //Added by Sunam 2005-11-28 FOR DR
	  if (m_drBillFilePath.length() > 0)
	  {
	    if (link(daybillfileBak.c_str(), dayBillFileDr.c_str()))
	    {
	      unlink(daybillfileBak.c_str());
	      unlink(dayBillFileDr.c_str());
	      unlink(daybillfile.c_str());
	      return false;
	    }
	    if (chmod(dayBillFileDr.c_str(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))
    	{          
	        unlink(daybillfileBak.c_str());
	        unlink(dayBillFileDr.c_str());
	        unlink(daybillfile.c_str());
	        return false;
	    }     
	  }
	  
		//Added by Sunam 2006-7-4 14:32 For OCS
		if (m_ocsBillFilePath.length() > 0)
		{
			string dayBillFileOcs = m_dayBillFileNameOcs + m_day;
			if (link(daybillfileBak.c_str(), dayBillFileOcs.c_str()))
			{
				unlink(daybillfileBak.c_str());
				unlink(dayBillFileDr.c_str());
				unlink(dayBillFileOcs.c_str());
				unlink(daybillfile.c_str());
				return false;
			}
			if (chmod(dayBillFileOcs.c_str(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))
			{          
				unlink(daybillfileBak.c_str());
				unlink(dayBillFileDr.c_str());
				unlink(dayBillFileOcs.c_str());
				unlink(daybillfile.c_str());
				return false;
			}
		}
    }


    // write log for revovery
    m_fstreamLog.close();
    string logbak = m_billLogfile + "@";
    unlink(logbak.c_str());
    rename(m_billLogfile.c_str(), logbak.c_str());

    m_fstreamLog.clear();
    m_fstreamLog.open(m_billLogfile.c_str(), ios::trunc|ios::out|ios::ate);
    if (m_fstreamLog.rdstate() != ios::goodbit)
    {
      unlink(m_billLogfile.c_str());
      return false;
    }
    m_fstreamLog << m_currentCycle << endl;
    m_fstreamLog << daybillfileBak << endl;
    m_fstreamLog.flush();
    
	//Added By Sunam For OCS 2006-6-14 13:00
	if (m_ocsBillFilePath.size() > 0)
	{
		m_ocsFstreamLog.close();
		m_ocsFstreamLog.clear();
		m_ocsFstreamLog.open(m_ocsBillLogfile.c_str(), ios::trunc|ios::out|ios::ate);
		m_ocsFstreamLog << m_currentCycle << endl;
		m_ocsFstreamLog.flush();
	}
  }

  strncpy(m_dealtime, dealdate + 2, DATETIMELEN);
  m_dealtime[DATETIMELEN] = 0;

  return true;
}


bool LocalBill::fileCommit(const char *filename)
{
	//Modified by Sunam 2005-11-28 FOR DR
	if (m_drBillFilePath.length() == 0)
	{
		if (m_localdata.empty())
		{
			return true;
		}
	}

  char tmp[10];
  // BILLSEQUENCE_LEN = 8
  sprintf(tmp, "%08d", m_sequenceNo);
  tmp[BILLSEQUENCE_LEN] = 0;

  /*
  //REALTIME BILL FILE NAME: RYYYYMMddhhmmssfilename
  string tmpFileName1 = m_incrBillFilenameBak
  + (m_dealtime + 2) + filename;
  // link realtime bill file for account system
  string tmpFileName2 = m_incrBillFilename + (m_dealtime + 2) + filename;
  */

  //INCREASE-BILL-FILE name: RYYYYMMXXXYYMMDDSSSSSSSS
  string tmpFileName1 = m_incrBillFilenameBak + m_day + tmp;
  // link increase bill file for account system
  string tmpFileName2 = m_incrBillFilename + m_day + tmp;
  //Added by Sunam 2005-11-28 FOR DR
  string tmpFileName3;
  if (m_drBillFilePath.length() > 0)
  {
  	tmpFileName3 = m_drBillFileName + m_day + tmp;
  }

  // write increase bill file
  if (!writeIncreaseFileBill(tmpFileName1.c_str()))
  {
    writeErrMsg(__LINE__, errno, tmpFileName1.c_str());
    return false;
  }
  if (tmpFileName1 != tmpFileName2)
  {
    if (link(tmpFileName1.c_str(), tmpFileName2.c_str()))
    {
      writeErrMsg(__LINE__, errno, tmpFileName2.c_str());
      unlink(tmpFileName1.c_str());
      unlink(tmpFileName2.c_str());
      return false;
    }
    if (chmod(tmpFileName2.c_str(),
              S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))
    {
      unlink(tmpFileName1.c_str());
      unlink(tmpFileName2.c_str());
      return false;
    }
    //Added by Sunam 2005-11-28 FOR DR
    if (m_drBillFilePath.length() > 0)
    {
    	if (link(tmpFileName1.c_str(), tmpFileName3.c_str()))
		{
		  writeErrMsg(__LINE__, errno, tmpFileName3.c_str());
		  unlink(tmpFileName1.c_str());
		  unlink(tmpFileName2.c_str());
		  unlink(tmpFileName3.c_str());
		  return false;
		}
    	if (chmod(tmpFileName3.c_str(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))
  		{          
    	  unlink(tmpFileName1.c_str());
    	  unlink(tmpFileName2.c_str());
    	  unlink(tmpFileName3.c_str());
    	  return false;
    	}
    }   
    //Added by Sunam 2005-11-28 FOR DR
  }

  // update Total Bill
  if (!updateTotalBill())
  {
    writeErrMsg(__LINE__, errno, "write total bill error!");
    unlink(tmpFileName1.c_str());
    unlink(tmpFileName2.c_str());
    return false;
  }
  
	//Added By Sunam For OCS 2006-6-14 11:56
	if (m_ocsBillFilePath.size() > 0 )
	{
		if (!m_localdataOcs.empty())
		{
			string tmpFileName4,tmpFileName5;
			sprintf(tmp, "%08d", m_ocsSequenceNo);
			tmp[BILLSEQUENCE_LEN] = 0;
			tmpFileName4 = m_ocsBillFileNameBak + m_day + tmp;
			tmpFileName5 = m_ocsBillFileName + m_day + tmp;
			
			if (!writeOcsIncreaseFileBill(tmpFileName4.c_str()))
			{
				writeErrMsg(__LINE__, errno, tmpFileName1.c_str());
				return false;
			}
			if (tmpFileName4 != tmpFileName5)
			{
				if (link(tmpFileName4.c_str(), tmpFileName5.c_str()))
				{
					writeErrMsg(__LINE__, errno, tmpFileName5.c_str());
					unlink(tmpFileName4.c_str());
					unlink(tmpFileName5.c_str());
					return false;
				}
				if (chmod(tmpFileName5.c_str(), S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH))
				{
					unlink(tmpFileName4.c_str());
					unlink(tmpFileName5.c_str());
					return false;
				}
			}
			m_ocsFstreamLog << tmpFileName4 << endl;
  			m_ocsFstreamLog.flush();
			m_ocsSequenceNo ++;
			m_localdataOcs.clear();
		}
	}

  // write bill log
  m_fstreamLog << tmpFileName1 << endl;
  m_fstreamLog.flush();

  // is need to rebuild server index
  if (m_isNewServAdd)
    m_servIndex.reset(m_servVector.begin(), m_servVector.end());
  //m_servIndex.append(bitr, m_servVector.end());

  m_localdata.clear();
  m_sequenceNo++;
  return true;
}


bool LocalBill::fileCommit()
{
  if (m_localdata.empty())
    return true;

  char tmp[10];
  sprintf(tmp, "%08d", m_sequenceNo);
  tmp[BILLSEQUENCE_LEN] = 0;

  // update Total Bill
  if (!updateTotalBill())
  {
    writeErrMsg(__LINE__, errno, "write total bill error!");
    return false;
  }

  // is need to rebuild server index
  if (m_isNewServAdd)
    m_servIndex.reset(m_servVector.begin(), m_servVector.end());

  m_localdata.clear();
  m_sequenceNo++;
  return true;
}

bool LocalBill::getBill(const SERVID& servid, Integer *chargeVals)
{
  // get Bill From Local
  BILLDATAMIter it;
  pair<BILLDATAMIter, BILLDATAMIter> mrange;
  mrange = m_localdata.equal_range(servid);
  if (mrange.first != mrange.second)
  {
    for (it = mrange.first; it != mrange.second; ++it)
    {
      assert(((*it).second.charge.type <= MAX_ITEMID)
             && ((*it).second.charge.type >= MIN_ITEMID));
      *(chargeVals + (*it).second.charge.type) = (*it).second.charge.value;
    }
    return true;
  }

  // get Bill From Total
  Serv s;
  s.m_servId = servid;
  ServIndex::iterator sit = m_servIndex.locate(s);
  if (sit != m_servIndex.end())
  {
    Bill::const_iterator bit = (*sit).m_bill.begin();
    Bill::const_iterator eit = (*sit).m_bill.end();
    BILLITEM billitem;
    billitem.sit = sit;
    for (; bit != eit; ++bit)
    {
      assert(((*bit).type <= MAX_ITEMID) && ((*bit).type >= MIN_ITEMID));
      *(chargeVals + (*bit).type) = (*bit).value;
      billitem.charge.type = (*bit).type;
      billitem.charge.value = (*bit).value;
      m_localdata.insert(BILLDATAMAP::value_type(servid, billitem));
    }
  }
  return true;
}



#ifndef RELOPS_NAMESPACE_DISABLE
#if defined(SUNCPP_CONFIG)
using rel_ops::operator !=;
#else
//USING_NAMESPACE(rel_ops)
#endif
#endif

bool LocalBill::updateBill(const SERVID& servid, const int groupid,
                           const Integer *chargeVals)
{
  // update the servid's bill
  // updateLocalBill
  //bit_vector updateflags(MAX_CHARGEITEM);
  unsigned char updateflags[MAX_CHARGEITEM];
  memset(updateflags, 0, sizeof(updateflags));

  BILLDATAMIter it;
  pair<BILLDATAMIter, BILLDATAMIter> mrange;
  mrange = m_localdata.equal_range(servid);

  // update exist items
  for (it = mrange.first; it != mrange.second; ++it)
  {
    assert(((*it).second.charge.type <= MAX_ITEMID)
           && ((*it).second.charge.type >= MIN_ITEMID));
    (*it).second.charge.value = *(chargeVals + (*it).second.charge.type);
    (*it).second.groupid = groupid;
    updateflags[(*it).second.charge.type] = 1;
  }

  BILLITEM billitem;
  billitem.groupid = groupid;
  if (mrange.first == mrange.second)
    //*(void**)&billitem.sit = 0;//ServIndex::iterator;
    billitem.sit = m_servIndex.nit();
  else
    billitem.sit = (*mrange.first).second.sit;

  // insert new items and/or add new serv
  for (int i = MIN_ITEMID; i < MAX_ITEMID + 1; i++)
  {
    if ((*(chargeVals + i) != 0) && (updateflags[i] == 0))
    {
      billitem.charge.type = i;
      billitem.charge.value = *(chargeVals + i);
      m_localdata.insert(BILLDATAMAP::value_type(servid, billitem));
    }
  }
  return true;
}


bool LocalBill::writeIncreaseFileBill(const char * filename)
{
  BILLDATAMIter bit = m_localdata.begin(), eit = m_localdata.end();

  string tmpFilename = m_totalBillFilePath + TMP_PREFIX + "IBILL";
  m_fstreamIncrease.clear();
  m_fstreamIncrease.open(tmpFilename.c_str(), ios::out);
  if (m_fstreamIncrease.rdstate() != ios::goodbit)
  {
    writeErrMsg(__LINE__, errno, tmpFilename.c_str());
    unlink(tmpFilename.c_str());
    return false;
  }
  for (; bit != eit; ++bit)
  {
    // update by WGW, 2002/06/20, delete group_id
    //m_fstreamIncrease << (*bit).first << "\t" << (*bit).second.charge
    //                << "\t" << (*bit).second.groupid << endl;
    m_fstreamIncrease << (*bit).first << "\t" << (*bit).second.charge
                      << endl;
  }
  m_fstreamIncrease.close();

  if (rename(tmpFilename.c_str(), filename))
  {
    string errmsg = "rename ";
    errmsg = errmsg + tmpFilename + filename;
    writeErrMsg(__LINE__, errno, errmsg.c_str());
    unlink(tmpFilename.c_str());
    return false;
  }
  return true;
}


bool LocalBill::writeTotalFileBill(const char * filename)
{
  string tmpFilename = m_totalBillFilePath + TMP_PREFIX + "TBILL";
  m_fstreamTotal.clear();
  m_fstreamTotal.open(tmpFilename.c_str(), ios::out);
  if (m_fstreamTotal.rdstate() != ios::goodbit)
  {
    m_fstreamTotal.close();
    unlink(tmpFilename.c_str());
    return false;
  }

  for (ServVector::iterator bit = m_servVector.begin(),
         eit = m_servVector.end();
       bit != eit; bit++)
  {
    for (Bill::const_iterator bb = bit->m_bill.begin(), eb = bit->m_bill.end();
         bb != eb; bb++)
      m_fstreamTotal << (*bit).m_servId << "\t" << *bb << endl;
      //m_fstreamTotal << (*bit).m_servId << "," << *bb << endl;

  }
  m_fstreamTotal.close();

  if (rename(tmpFilename.c_str(), filename))
  {
    unlink(tmpFilename.c_str());
    return false;
  }
  return true;
}


bool LocalBill::updateTotalBill()
{
  Serv serv;
  serv.m_servId = INVALID_SERVID;
  ServIndex::iterator errit = m_servIndex.nit();

  BILLDATAMIter bit = m_localdata.begin(), eit = m_localdata.end();

  bool diffServid = true;
  ServVectorItr svitr;
  for (bit = m_localdata.begin(); bit != eit; ++bit)
  {
    if (serv.m_servId != (*bit).first)
    {
      serv.m_servId = (*bit).first;
      diffServid = true;
    }
    else
      diffServid = false;

    if ((*bit).second.sit == errit)
    { // new serv process
      // add new serv and item
      if (diffServid)
      {
        m_isNewServAdd = true;
        serv.m_bill.clear();
        serv.m_bill.push_back((*bit).second.charge);
        m_servVector.push_back(serv);
        svitr = m_servVector.end();
        --svitr;
      }
      else // only add new item
        (*svitr).m_bill.push_back((*bit).second.charge);
    }
    else // (*bit).second.sit != errit
    {
      // update exist charge item
      BillItr bbitr = (*(*bit).second.sit).m_bill.begin();
      BillItr ebitr = (*(*bit).second.sit).m_bill.end();

      for (; bbitr != ebitr; )
      {
        if ((*bbitr).type == (*bit).second.charge.type)
        {
          (*bbitr).value = (*bit).second.charge.value;
          break;
        }
        ++bbitr;
      }
      // insert new charge item
      if (bbitr == ebitr)
        (*(*bit).second.sit).m_bill.push_back((*bit).second.charge);
    }
  }
  return true;
}


bool LocalBill::initTotalBillFromFile(const char * filename)
{
  Serv serv;
  serv.m_bill.clear();
  CHARGEITEM chargeitem;
  serv.m_servId = INVALID_SERVID;
  SERVID servid = 0;

  ifstream totalStream;
  totalStream.open(filename, ios::in);
  if (totalStream.rdstate() != ios::goodbit)
  {
    totalStream.close();
    return false;
  }

  string strServid;
  while (totalStream >> strServid >> chargeitem)
  {
    servid = strServid;

    if (serv.m_servId == servid) // same serv
      serv.m_bill.push_back(chargeitem);
    else
    { // serv change, put old serv
      if (serv.m_servId != INVALID_SERVID)
        m_servVector.push_back(serv);
      serv.m_bill.clear();
      serv.m_servId = servid;
      serv.m_bill.push_back(chargeitem);
    }
  }
  if (serv.m_servId != INVALID_SERVID)
    m_servVector.push_back(serv);

  totalStream.close();

  // rebuild server index
  m_servIndex.reset(m_servVector.begin(), m_servVector.end());

  return true;
}

bool LocalBill::initFromIncreaseFile(const char *filename)
{
  ifstream increaseStream;
  increaseStream.open(filename, ios::in);
  if (increaseStream.rdstate() != ios::goodbit)
  {
    increaseStream.close();
    return false;
  }

  /*
    Serv serv;
    serv.m_bill.clear();
    serv.m_servId = INVALID_SERVID;
  */
  CHARGEITEM chargeitem;
  SERVID servid = INVALID_SERVID;
  SERVID servidOld = INVALID_SERVID;
  string strServid;
  string strGroupid;

  Integer  chargeValues[MAX_CHARGEITEM];
  memset(chargeValues, 0, sizeof(chargeValues));

  m_localdata.clear();
  m_isNewServAdd = false;
  // update by WGW