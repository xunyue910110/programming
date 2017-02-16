#include "TestApp.h"
#include "CdrFormat.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream> 
#ifdef FP_DEBUG
 #include "base/supdebug.h"
#endif

TestApp   g_testApp;
Application * const theApp = &g_testApp;


TestApp::TestApp()
{
}
TestApp::~TestApp()
{
}

bool TestApp::initialization()
{
  setoptstr("c:");
  // normally, call standard initialization first
  if (!Application::initialization())
  {
    cout<<"Application::initialization false!"
        <<__FILE__<<__LINE__<<endl;
    return false;
  }
  if(m_config.getSysParam(m_theSysParam,"")==false)
  {
    cout<<"m_config.getSysParam false!"
        <<__FILE__<<__LINE__<<endl;
    return false;
  }
  m_config.dump();
  return true;
}

bool TestApp::beforeLoop()
{
	if (!Application::beforeLoop())
	{
	  #ifdef _DEBUG_
	    cout<<"Application::beforeLoop false!"
	        <<__FILE__<<__LINE__<<endl;
	  #endif
		return false;
	}
  return true;  
}
bool TestApp::loopProcess()
{
  if(!testFunc())
  {
    m_stop = true;
  }
  return Application::loopProcess();
}
bool TestApp::end()
{
  return Application::end();
}
bool TestApp::testFunc()
{
  PString t_fileName_in,t_fileName_out;
  ifstream  t_inFile;
  ofstream  t_outFile;
  
  
  t_fileName_in="/billing/src/cdr/data/input/VS13_vpf200803300113.731";
  t_fileName_out="/billing/src/cdr/data/output/VS13_vpf200803300113.731";
  
  t_inFile.open(t_fileName_in.c_str(),ios::in);
  if(!t_inFile)
  {
    cout<<"open file:{"<<t_fileName_in<<"} failed!"
        <<__FILE__<<__LINE__<<endl;
    return false;
  }
  t_outFile.open(t_fileName_out.c_str(),ios::out);
  if(!t_outFile)
  {
    cout<<"open file:{"<<t_fileName_out<<"} failed!"
        <<__FILE__<<__LINE__<<endl;
    return false;
  }

  const CdrFormat *t_cdrFmt_in =m_config.getCdrFormat("GSM_SCP");
  const CdrFormat *t_cdrFmt_out=m_config.getCdrFormat("GSM_OUTPUT");
  
  if(t_cdrFmt_in==NULL || t_cdrFmt_out==NULL)
  {
    cout<<"m_config.getCdrFormat(GSM_SCP||GSM_OUTPUT) false! "
        <<__FILE__<<__LINE__<<endl;
  }
  #ifdef FP_DEBUG
    FP_BEGIN(Inoutput_rate)
  #endif
  while(!t_inFile.eof())
  {
    m_cdr.reset();
    t_inFile.getline(m_cdr.m_orgBuff,m_cdr.m_orgBufLen,'\n');
    if(strlen(m_cdr.m_orgBuff)==0) break;
    m_cdr.inputCdr(t_cdrFmt_in);
    m_cdr.canonicalize();
    //各种处理话单逻辑
    m_cdr.finalize();
    t_outFile<<m_cdr.outputCdr(t_cdrFmt_out);
  };
  t_inFile.close();
  t_inFile.clear();
  t_outFile.close();
  t_outFile.clear();
  #ifdef FP_DEBUG
    FP_END(Inoutput_rate)
  #endif

  m_stop=true;
  return true;
}
