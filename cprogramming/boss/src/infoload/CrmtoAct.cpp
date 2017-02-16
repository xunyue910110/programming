#include "CrmtoAct.h"
#include "DbInterface.h"
#include "appfrm/ErrorHandle.h"

CrmtoAct::CrmtoAct()
{
	m_csrcdbi=NULL;
	m_cobjdes=NULL;
}
CrmtoAct::~CrmtoAct()
{
}

//初始
void CrmtoAct::Cinit(DbInterface *csrcdbi,DbInterface *cobjdes)
{
	m_csrcdbi=csrcdbi;
	m_cobjdes=cobjdes;
}
// 本部分有可能有内存泄漏，但是程序只运行一次，关系不太大的，可以不用考虑改程序
void CrmtoAct::LoadData(const  string& selectsql, const string& updatesql)
{
		try
		{
	  	m_csrcdbi->createStatement();
	  	m_csrcdbi->executeUpdate("ALTER SESSION SET NLS_DATE_FORMAT = 'yyyymmdd HH24:MI:SS'");
	  	m_csrcdbi->setSql((char*)selectsql.c_str());
	  	//printf("select =%s\n",selectsql.c_str());
	  	#ifdef FP_DEBUG
      FP_BEGIN(rinfoload)
      #endif
	  	m_csrcdbi->executeQuery(MAX_RECORD_NUM);
	  	#ifdef FP_DEBUG
      FP_END(rinfoload)
      #endif
	  	//m_cobjdes->createStatement();
	  	//m_cobjdes->setSql((char*)updatesql.c_str());
	  }catch(CException e)
	  {
	  	throw e;
	  }

//求取列类型并分配空间
    int iCount;
		iCount=m_csrcdbi->getColCount();
		vector<DataParam> vBatchDbPars;
		vBatchDbPars.clear();
		int nBatchNum=MAX_RECORD_NUM;
		//m_ind =new short*[iCount];
		
	for(int i=1;i<=iCount;++i)
	{
	  	int coltype,colsize;
	  	coltype=m_csrcdbi->getColumnType(i);
	  	colsize=m_csrcdbi->getColumnSize(i);
	  	DataParam vBatchDbPar;
	    vBatchDbPar.m_dataElemSize = new int[nBatchNum];
	  	vBatchDbPar.m_seq=i;
	  	switch ( coltype )
        {
          case OCCI_SQLT_STR:
          case OCCI_SQLT_CHR:
               vBatchDbPar.m_size = colsize + 1 ;
               vBatchDbPar.m_type = VAR_TYPE_VSTR ;
               vBatchDbPar.m_data = new char[vBatchDbPar.m_size*nBatchNum];
               m_csrcdbi->SetResultDataBuffer(vBatchDbPar.m_seq ,(void*)vBatchDbPar.m_data ,OCCI_SQLT_STR ,vBatchDbPar.m_size,0,reinterpret_cast<sb2*>(vBatchDbPar.m_dataElemSize));
               break;
          case OCCI_SQLT_NUM:
          case OCCI_SQLT_LNG:
               vBatchDbPar.m_size = sizeof(long);
               vBatchDbPar.m_type = VAR_TYPE_LONG ; 
               vBatchDbPar.m_data = new long[nBatchNum];
               m_csrcdbi->SetResultDataBuffer(vBatchDbPar.m_seq ,(void*)vBatchDbPar.m_data ,OCCIINT ,vBatchDbPar.m_size,0,reinterpret_cast<sb2*>(vBatchDbPar.m_dataElemSize));                     
               break;
          case OCCIINT :
               vBatchDbPar.m_size = sizeof(int);
               vBatchDbPar.m_type = VAR_TYPE_INT ; 
               vBatchDbPar.m_data = new int[nBatchNum];
               m_csrcdbi->SetResultDataBuffer(vBatchDbPar.m_seq ,(void*)vBatchDbPar.m_data ,OCCIINT ,vBatchDbPar.m_size,0,reinterpret_cast<sb2*>(vBatchDbPar.m_dataElemSize));                     
               break;  
          case OCCIFLOAT:               
               vBatchDbPar.m_size = sizeof(float);
               vBatchDbPar.m_type = VAR_TYPE_REAL; 
               vBatchDbPar.m_data = new float[nBatchNum];      
               m_csrcdbi->SetResultDataBuffer(vBatchDbPar.m_seq ,(void*)vBatchDbPar.m_data ,OCCIFLOAT ,vBatchDbPar.m_size,0,reinterpret_cast<sb2*>(vBatchDbPar.m_dataElemSize));                                                           
               break;
          default : //默认全部是字符类型
               vBatchDbPar.m_size = MAX_CHAR_ARRAY_SIZE  + 1 ;
               vBatchDbPar.m_type = VAR_TYPE_VSTR ;
               vBatchDbPar.m_data = new char[vBatchDbPar.m_size*nBatchNum];
               m_csrcdbi->SetResultDataBuffer(vBatchDbPar.m_seq ,(void*)vBatchDbPar.m_data ,OCCI_SQLT_STR ,vBatchDbPar.m_size,0,reinterpret_cast<sb2*>(vBatchDbPar.m_dataElemSize));                                          
        }
        vBatchDbPars.push_back(vBatchDbPar);   
	}
	//解决null值转换问题
  for( int i= 0 ; i <vBatchDbPars.size() ;++i )
  memset((char*)vBatchDbPars[i].m_data,0x00,vBatchDbPars[i].m_size*nBatchNum);
	int irow;
	//m_cobjdes->beginTrans();
  //m_cobjdes->createStatement();
      m_cobjdes->beginTrans();
      m_cobjdes->createStatement();
      m_cobjdes->executeUpdate("ALTER SESSION SET NLS_DATE_FORMAT = 'yyyymmdd HH24:MI:SS'");
      m_cobjdes->setSql((char*)updatesql.c_str());
  while(m_csrcdbi->BatchNext(irow,MAX_RECORD_NUM))
	{
    try{
	  //m_cobjdes->setBatchParameter(vBatchDbPars,irow); 
   int iBatchSize = MAX_RECORD_NUM;
   int t_size = 0 ;
      for( int i = 0 ;i < irow ; i+= iBatchSize)
      {
        t_size = iBatchSize;
        if (i + t_size > irow)
           t_size = irow - i;
        //  设置批量缓存
        for( int j = 0 ; j < iCount;++j )
        {
          // 设置类型
          oracle::occi::Type iDataType = oracle::occi::OCCI_SQLT_STR; 
          switch ( vBatchDbPars[j].m_type )
          {
            case VAR_TYPE_INT2:
            case VAR_TYPE_INT:
            case VAR_TYPE_LONG:
            case VAR_TYPE_DATE :
                 iDataType = oracle::occi::OCCIINT;break;
            case VAR_TYPE_REAL:
                 iDataType = OCCIFLOAT;                   
          };
          m_cobjdes->ResultDataBuffer( j + 1,(char*)vBatchDbPars[j].m_data + i* vBatchDbPars[j].m_size ,iDataType, vBatchDbPars[j].m_size,0,reinterpret_cast<sb2*>(vBatchDbPars[j].m_dataElemSize + i ));
        };
        //   执行批量提交
         m_cobjdes->execArrayUpdate( t_size );
         //m_cobjdes->getUpdateNum();
  		 	 m_cobjdes->commitTrans();		        
      }
     
      /*#ifdef FP_DEBUG
      FP_BEGIN(winfoload)
      #endif 
      m_cobjdes->setBatchParameter(vBatchDbPars,irow); 
      #ifdef FP_DEBUG
      FP_END(winfoload)
      #endif*/
		 /* #ifdef FP_DEBUG
      FP_BEGIN(uinfoload)
      #endif 
			m_cobjdes->executeUpdate();
			#ifdef FP_DEBUG
      FP_END(uinfoload)
      #endif 
      #ifdef FP_DEBUG
      FP_BEGIN(OECcommit)
      #endif
			m_cobjdes->commitTrans();
			#ifdef FP_DEBUG
      FP_END(OECcommit)
      #endif */
		}catch(CException e)
		{		  
			throw e;
		}
	}

	//m_cobjdes->commitTrans();
	//释放分配的空间
 for( int i = 0 ;i < vBatchDbPars.size();++i )
 {
     delete[] vBatchDbPars[i].m_data ;
     delete[] vBatchDbPars[i].m_dataElemSize ;     
 }
	/*for(int i=0;i<vBatchDbPars.size();++i)
	{
		switch(vBatchDbPars[i].m_type)
		{
			case PARAM_TYPE_INT:
				delete [] vBatchDbPars[i].m_intValue; break;
			case PARAM_TYPE_LONG:
				delete [] vBatchDbPars[i].m_longValue; break;
			case PARAM_TYPE_FLOAT:
				delete [] vBatchDbPars[i].m_floatValue; break;
			case PARAM_TYPE_STR:
				delete [] vBatchDbPars[i].m_strValue; break;
		} 
	} */
}

//导入前准备
bool CrmtoAct::BeginPro(const string &proname,const string &tablename,const int& infotype,string &err_info)
{
			
	try
	{
	 vector< DbParam> proParams;
	  proParams.clear(); 
	  DbParam proParam;
	//  proParams.push_back(proParam);
	  proParam.m_seq=1;
	  proParam.m_action=PARAM_ACT_IN;
	  proParam.m_type=PARAM_TYPE_STR;
	  strcpy(proParam.m_strValue,tablename.c_str());
	  proParams.push_back(proParam);
	  proParam.m_seq=2;
	  proParam.m_action=PARAM_ACT_IN;
	  proParam.m_type=PARAM_TYPE_INT;
	  proParam.m_intValue=infotype;
	  proParams.push_back(proParam);
	  proParam.m_seq=3;
	  proParam.m_action=PARAM_ACT_OUT;
	  proParam.m_type=PARAM_TYPE_INT;
	  proParam.m_intValue=0;
    proParams.push_back(proParam);
	  proParam.m_seq=4;
	  proParam.m_action=PARAM_ACT_OUT;
	  proParam.m_type=PARAM_TYPE_STR;
	  strcpy(proParam.m_strValue,"0");
	  proParams.push_back(proParam);
	  m_cobjdes->execProcedure(proname,proParams);
      err_info=proParams[3].m_strValue;
      if(proParams[2].m_intValue!=1)
      {
          return false;
      }
	}catch(CException e)
	{
		throw e;
	}
    return true;
}
//导入后准备
bool CrmtoAct::EndPro(const string  &proname,const string &tablename,const int& infotype,string &err_info)
{
	try
	{
   vector< DbParam> proParams;
	  proParams.clear(); 
	  DbParam proParam;
	  proParam.m_seq=1;
	  proParam.m_action=PARAM_ACT_IN;
	  proParam.m_type=PARAM_TYPE_STR;
	  strcpy(proParam.m_strValue,tablename.c_str());
	  proParams.push_back(proParam);
	  proParam.m_seq=2;
	  proParam.m_action=PARAM_ACT_IN;
	  proParam.m_type=PARAM_TYPE_INT;
	  proParam.m_intValue=infotype;
	  proParams.push_back(proParam);
    proParam.m_seq=3;
	  proParam.m_action=PARAM_ACT_OUT;
	  proParam.m_type=PARAM_TYPE_INT;
	  proParam.m_intValue=0;
    proParams.push_back(proParam);
	  proParam.m_seq=4;
	  proParam.m_action=PARAM_ACT_OUT;
	  proParam.m_type=PARAM_TYPE_STR;
	  strcpy(proParam.m_strValue,"0");
	  proParams.push_back(proParam);
 //     printf("end1\n");
	  m_cobjdes->execProcedure(proname,proParams);
      err_info=proParams[3].m_strValue;
      if(proParams[2].m_intValue!=1)
      {
          return false;
      }
    //  printf("end2\n");
	}catch(CException e)
	{
		throw e;
	}
    return true;
}
//同名切换
bool CrmtoAct::SynoNms(const string& proname,const int& infotype,string& err_info)
{
	try
	{
	  vector< DbParam> proParams;
	  proParams.clear(); 
	  DbParam proParam;
      proParam.m_seq=1;
      proParam.m_action=PARAM_ACT_IN;
      proParam.m_type=PARAM_TYPE_INT;
      proParam.m_intValue=infotype;
      proParams.push_back(proParam);
	  proParam.m_seq=2;
	  proParam.m_action=PARAM_ACT_OUT;
	  proParam.m_type=PARAM_TYPE_INT;
	  proParam.m_intValue=0;
    proParams.push_back(proParam);
	  proParam.m_seq=3;
	  proParam.m_action=PARAM_ACT_OUT;
	  proParam.m_type=PARAM_TYPE_STR;
	  strcpy(proParam.m_strValue,"0");
	  proParams.push_back(proParam);
	  m_csrcdbi->execProcedure(proname,proParams);
      err_info=proParams[2].m_strValue;
      if(proParams[1].m_intValue!=1)
        {
            return false;
        }

	}catch(CException e)
	{
		throw e;
	}
    return true;
}





