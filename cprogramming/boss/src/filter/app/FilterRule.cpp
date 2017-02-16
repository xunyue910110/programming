#include "FilterRule.h"
#include "appfrm/Application.h"
#include "appfrm/ErrorHandle.h"

#include <algorithm>
#include <iostream>
#include "base/StringUtil.h"
#include "base/SysParam.h"
#include "FilterConfig.h"

bool FilterRuleMgr::setCdrFormat(const PString & r_formatName,
                                   const vector<CdrFormat> &r_cdrFormats,
                                   const CdrFormat  *      &r_cdrFmt
                                   )
{
  CdrFormat  t_cdrFormat;
  vector<CdrFormat>::const_iterator t_cdrFmtItr;
  t_cdrFormat.m_formatName = r_formatName;
  t_cdrFmtItr=::lower_bound(r_cdrFormats.begin(),r_cdrFormats.end(),t_cdrFormat);
  if(t_cdrFmtItr==r_cdrFormats.end() || !(*t_cdrFmtItr == t_cdrFormat))
  {
    #ifdef _DEBUG_
      cout<<"cdrformat ："<<r_formatName<<" no defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"cdrformat ：" + r_formatName + " no defined!");
    return false;
  }
  r_cdrFmt = &(*t_cdrFmtItr);
  return true;
}
bool FilterRuleMgr::isPath(const PString &r_pathName)
{
  struct stat r_buf;

  if(lstat(r_pathName.c_str(),&r_buf)<0)
  {
    return false;
  }
  if(S_ISDIR(r_buf.st_mode))
    return true;
  else
    return false;
}

bool InputRuleMgr::initialize(SysParam *r_sysparam,
                              const char* r_appName,
                              const int &r_channelNo,
                              const vector<CdrFormat> &r_cdrFormats,
                              const CdrAttribMacros &r_attribMacros,
                              FilterConfig *r_config)
{
  string t_path,t_subPath,t_subPath1,t_subNodeName,t_value;
  string t_appName,t_channelNo,t_vname;
  vector<string>  t_subNodeList;
  vector<string>::iterator t_nodeItr;
  InputParam       t_inputParam;
  map<string,string> t_nodes,t_nodes1;
  PString            t_pstr;
  char channelNo[4];

  t_appName = r_appName;t_channelNo=StringUtil::toString(r_channelNo);
  //1.读取通道规则总定义信息
  t_path = SECTDELIM + t_appName + SECTDELIM + t_channelNo;

  t_vname = "is_use_mdb";
  if(r_sysparam->getValue(t_path,t_vname,t_value)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysparam->getValue("<<t_path<<":"<<t_vname<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_path + ":" + t_vname + ") false!");
    return false;
  }
   m_isNeeduseMdb = t_value[0];

  t_vname = "is_back_source";
  if(r_sysparam->getValue(t_path,t_vname,t_value))
  {
	   if(t_value[0]=='Y')
	     m_isbackSource = 'Y';
	   else
	   	 m_isbackSource = 'N';
  }
   else
   	 m_isbackSource = 'N';

   m_errorPath = r_config->m_errorPath;
   if(r_config->m_file_sort_method>=0)
   {
      m_sortMode = r_config->m_file_sort_method;
   }

 //2.读取通道子配置
  int chNo=1;
  sprintf(channelNo,"%d",chNo);
  t_path=SECTDELIM + t_appName + SECTDELIM + t_channelNo + SECTDELIM +"channel_" + channelNo;
  t_vname = "source_path";
  while(r_sysparam->getValue(t_path,t_vname,t_value) )
 {
    t_nodes.clear();
    if (r_sysparam->getValue(t_path,t_nodes)==false)
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath<<",t_nodes)"
            <<" false!"<<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath + ",t_nodes) false!");
      return false;
    }
    t_inputParam.m_inputPath  = t_nodes["source_path"  ];

    //如需要备份，获取备份路径
    if(m_isbackSource == 'Y')
    {
    	t_inputParam.m_inputPathBack  = t_nodes["source_bakpath"  ];
    }
    

    if(t_nodes["separate"].length()>0)
    {
        t_inputParam.m_separate = t_nodes["separate"][0];
    }
    else
    {
        t_inputParam.m_separate = r_config->m_default_separate;
    }
    t_inputParam.m_fileType  = t_nodes["file_type"];

    t_subPath1 = SECTDELIM + t_appName + SECTDELIM + "file_type" + SECTDELIM + t_inputParam.m_fileType.c_str();
    t_nodes1.clear();
    if(r_sysparam->getValue(t_subPath1,t_nodes1)==false)
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath1<<") false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath1 + ") false!");
      return false;
    }

    t_inputParam.m_inFormatName  = t_nodes1["input_field_no"];
    if(FilterRuleMgr::setCdrFormat(t_inputParam.m_inFormatName,r_cdrFormats,t_inputParam.m_pFormat)==false)
    {
      #ifdef _DEBUG_
        cout<<"inputcdrformat ："<<t_inputParam.m_inFormatName<<" no defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"inputcdrformat ：" + t_inputParam.m_inFormatName + " no defined!");
      return false;
    }
    //获取话单记录个数
    t_inputParam.m_fieldCount = atoi(t_nodes1["field_count"].c_str());

    t_pstr  = t_nodes1["processor"];
    t_inputParam.m_procName.clear();
    t_pstr.split(';',t_inputParam.m_procName);
   if(t_inputParam.m_procName.size()<=0)
    {
      #ifdef _DEBUG_
        cout<<"processor: "<<t_pstr<<" no defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"processor no defined!");
      return false;
    }

    //获取资料方式
    t_inputParam.m_bindmethod = atoi(t_nodes1["bind_method"].c_str());

   t_inputParam.m_subChanName = "channel_" + StringUtil::toString(chNo);
   m_inputParam.push_back(t_inputParam);

   chNo++;
   sprintf(channelNo,"%d",chNo);
   t_path=SECTDELIM + t_appName + SECTDELIM + t_channelNo + SECTDELIM +"channel_" + channelNo;
   t_vname = "source_path";

  }
  ::sort(m_inputParam.begin(),m_inputParam.end());
  return true;
}

bool InputRuleMgr::valid() const
{
  for(vector<InputParam>::const_iterator t_in_Itr=m_inputParam.begin();
      t_in_Itr!=m_inputParam.end();++t_in_Itr)
  {
      if(FilterRuleMgr::isPath(t_in_Itr->m_inputPath)==false)
      {
        #ifdef _DEBUG_
        cout<<"m_inputPath:"<<t_in_Itr->m_inputPath<<" is not exists!"
        <<__FILE__<<__LINE__<<endl;
        #endif
        theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_inputPath:" + t_in_Itr->m_inputPath + "is not exists!");
        return false;
      }
      if(m_isbackSource=='Y' && FilterRuleMgr::isPath(t_in_Itr->m_inputPathBack)==false)
      {
        #ifdef _DEBUG_
        cout<<"m_inputPathBack:"<<t_in_Itr->m_inputPathBack<<" is not exists!"
        <<__FILE__<<__LINE__<<endl;
        #endif
        theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_inputPathBack:" + t_in_Itr->m_inputPathBack + "is not exists!");
        return false;
      }
  }
  return true;
}

InputRuleMgr::InputParam *InputRuleMgr::getInputParam(const PString & r_subChanName)
{
  InputParam   t_inputParam;
  vector<InputRuleMgr::InputParam>::iterator r_Itr;
  t_inputParam.m_subChanName=r_subChanName;
  r_Itr=::lower_bound(m_inputParam.begin(),m_inputParam.end(),t_inputParam);
  if(r_Itr==m_inputParam.end()) return NULL;
  if(strncmp(r_subChanName,r_Itr->m_subChanName.c_str(),r_Itr->m_subChanName.length())!=0)
  {
    return NULL;
  }
  return &(*r_Itr);
}

void InputRuleMgr::dump() const
{
  cout<<"............Input Rule Mgr ........"<<endl;
  cout<<"m_isNeeduseMdb            = "<<m_isNeeduseMdb<<endl;
  cout<<"m_isbackSource            = "<<m_isbackSource<<endl;
  cout<<"m_sortMode                = "<<m_sortMode<<endl;
  cout<<"m_errorPath               = "<<m_errorPath<<endl;
  cout<<"-----------------------------------"<<endl;

  for(vector<InputParam>::const_iterator r_inItr=m_inputParam.begin();
      r_inItr!=m_inputParam.end();++r_inItr)
  {
    cout<<"m_subChanName  =  "<<r_inItr->m_subChanName<<"\tbegin........"<<endl;
    cout<<"m_inputPath="<<r_inItr->m_inputPath<<endl;
    cout<<"m_fieldCount="<<r_inItr->m_fieldCount<<endl;
    cout<<"m_separate="<<r_inItr->m_separate<<endl;
    cout<<"m_fileType="<<r_inItr->m_fileType<<endl;
    cout<<"m_inFormatName="<<r_inItr->m_inFormatName<<endl;
    cout<<"m_bindmethod="<<r_inItr->m_bindmethod<<endl;

    cout<<"............Input CdrFormat ........"<<endl;
    cout<<"--------CdrFormat : "<<r_inItr->m_pFormat->m_formatName<<" -----------------"<<endl;
    cout<<"\tm_fieldDeliminater="<<r_inItr->m_pFormat->m_fieldDeliminater<<endl;
    cout<<"\tm_lineDeliminater ="<<r_inItr->m_pFormat->m_lineDeliminater<<endl;
    cout<<"\tm_storeType       ="<<r_inItr->m_pFormat->m_storeType<<endl;

     for(vector<FieldFormat>::const_iterator t_Itr=r_inItr->m_pFormat->m_fields.begin();
         t_Itr!=r_inItr->m_pFormat->m_fields.end();++t_Itr)
     {
       t_Itr->dump();
     }
     for(vector<PString>::const_iterator t_Itr1=(r_inItr->m_procName).begin();
         t_Itr1!=(r_inItr->m_procName).end();++t_Itr1)
     {
       cout<<"m_procName"<<":"<<*t_Itr1<<endl;
     }
       cout<<"m_subChanName  =  "<<r_inItr->m_subChanName<<"\tend........"<<endl;
  }
  cout<<"-----------------------------------\n"<<endl;

}

bool OutputRuleMgr::initialize(SysParam *r_sysparam,
                              const char* r_appName,
                              const int &r_channelNo,
                              const vector<CdrFormat> &r_cdrFormats,
                              const CdrAttribMacros &r_attribMacros,
                              FilterConfig *r_config)
{
  string t_path,t_subPath,t_subPath1,t_subPath2,t_subNodeName,t_value;
  string t_appName,t_channelNo,t_vname;
  vector<string>  t_subNodeList;
  vector<string>::iterator t_nodeItr;
  OutputParam       t_outputParam;
  OutChanInfo       t_outChanInfo;
  vector<OutChanInfo>::iterator t_infoItr;
  map<string,string> t_nodes,t_nodes1,t_nodes2;
  map<string,string>::iterator t_nodeMap;
  PString            t_pstr;
  char channelNo[4];

  t_appName = r_appName;t_channelNo=StringUtil::toString(r_channelNo);

 //读取通道子配置
  int chNo=1;
  sprintf(channelNo,"%d",chNo);
  t_path=SECTDELIM + t_appName + SECTDELIM + t_channelNo + SECTDELIM +"channel_" + channelNo;
  t_vname = "output_channel_no";
  while(r_sysparam->getValue(t_path,t_vname,t_value) )
 {
    t_nodes.clear();
    if (r_sysparam->getValue(t_path,t_nodes)==false)
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath<<",t_nodes)"
            <<" false!"<<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath + ",t_nodes) false!");
      return false;
    }
    t_outputParam.m_fileType  = t_nodes["file_type"];
    t_outputParam.m_outputName = t_nodes["output_channel_no"];

    t_subPath1 = SECTDELIM + t_appName + SECTDELIM + "file_type" + SECTDELIM + t_outputParam.m_fileType.c_str();
    t_nodes1.clear();
    if(r_sysparam->getValue(t_subPath1,t_nodes1)==false)
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath1<<") false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath1 + ",t_nodes1) false!");
      return false;
    }
    ///确定默认子进程通道号 通道名
    t_outputParam.m_defaul_outputName  = t_nodes1["output_channel_no"];
    t_subPath2 = SECTDELIM + t_appName + SECTDELIM + "output_channel_no" + SECTDELIM + t_outputParam.m_defaul_outputName.c_str();
    t_nodes2.clear();
    t_outputParam.m_outChanInfo.clear();
    if(t_outputParam.m_defaul_outputName.length()>0)
    {
      if(r_sysparam->getValue(t_subPath2,t_nodes2)==false)
      {
        #ifdef _DEBUG_
          cout<<"r_sysparam->getValue("<<t_subPath2<<") false!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath2 + ",t_nodes2) false!");
        return false;
      }
      for(t_nodeMap=t_nodes2.begin();t_nodeMap!=t_nodes2.end();++t_nodeMap)
      {
          t_outChanInfo.m_channelNo=t_nodeMap->first;
          t_outChanInfo.m_desPath=t_nodeMap->second;
          t_outputParam.m_outChanInfo.push_back(t_outChanInfo);
      }
    }

    ///更新追加子进程通道号 通道名
    if(t_outputParam.m_outputName.length()>0)
    {
	    t_subPath2 = SECTDELIM + t_appName + SECTDELIM + "output_channel_no" + SECTDELIM + t_outputParam.m_outputName.c_str();
	    t_nodes2.clear();
	    if(r_sysparam->getValue(t_subPath2,t_nodes2)==false)
	    {
	      #ifdef _DEBUG_
	        cout<<"r_sysparam->getValue("<<t_subPath2<<") false!"
	            <<__FILE__<<__LINE__<<endl;
	      #endif
        theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath2 + ",t_nodes2) false!");
	      return false;
	    }
	    for(t_nodeMap=t_nodes2.begin();t_nodeMap!=t_nodes2.end();++t_nodeMap)
	    {
	       for(t_infoItr=t_outputParam.m_outChanInfo.begin();
	           t_infoItr!=t_outputParam.m_outChanInfo.end();++t_infoItr)
	       {
	           if(t_infoItr->m_channelNo == t_nodeMap->first)
	           {
	             t_infoItr->m_desPath = t_nodeMap->second;
	             break;
	           }
	           //扫描到最后一位，还没找到，新增
	           if(t_infoItr == t_outputParam.m_outChanInfo.end()-1)
	           {
	              t_outChanInfo.m_channelNo=t_nodeMap->first;
	              t_outChanInfo.m_desPath=t_nodeMap->second;
	              t_outputParam.m_outChanInfo.push_back(t_outChanInfo);
	              break;
	           }
	       }
	    }
    }

    ///追加子进程输出前缀
    t_outputParam.m_prefixName  = t_nodes1["output_prefix"];
    t_subPath2 = SECTDELIM + t_appName + SECTDELIM + "output_prefix" + SECTDELIM + t_outputParam.m_prefixName.c_str();
    t_nodes2.clear();
    if(r_sysparam->getValue(t_subPath2,t_nodes2)==false)
    {
      #ifdef _DEBUG_
        cout<<"r_sysparam->getValue("<<t_subPath2<<") false!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath2 + ",t_nodes2) false!");
      return false;
    }
     for(t_infoItr=t_outputParam.m_outChanInfo.begin();
         t_infoItr!=t_outputParam.m_outChanInfo.end();++t_infoItr)
     {
        if(t_nodes2.count(StringUtil::toString(t_infoItr->m_channelNo)) )
        {
          t_infoItr->m_prefix = t_nodes2[StringUtil::toString(t_infoItr->m_channelNo)] + "_";
        	if(t_infoItr->m_channelNo == "error")
        	{
        		t_infoItr->m_prefix = t_nodes2[StringUtil::toString(t_infoItr->m_channelNo)] + StringUtil::toString(r_channelNo) + "_";
        	}
        }
        else if(t_nodes2.count("provfix") && atoi(t_infoItr->m_channelNo)   )
        {
          t_infoItr->m_prefix = t_nodes2["provfix"] + StringUtil::toString(t_infoItr->m_channelNo) + "_";
        }
        else
        {
          t_infoItr->m_prefix = "";
        }
     }
    ///追加子进程输出表达式
    if(t_nodes1["rule_express"].length()>0)
    {
      t_outputParam.m_expressionName  = t_nodes1["rule_express"];
      t_subPath2 = SECTDELIM + t_appName + SECTDELIM + "rule_express" + SECTDELIM + t_outputParam.m_expressionName.c_str();
      t_nodes2.clear();
      if(r_sysparam->getValue(t_subPath2,t_nodes2)==false)
      {
        #ifdef _DEBUG_
          cout<<"r_sysparam->getValue("<<t_subPath2<<") false!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysparam->getValue(" + t_subPath2 + ",t_nodes2) false!");
        return false;
      }
      for(t_infoItr=t_outputParam.m_outChanInfo.begin();
          t_infoItr!=t_outputParam.m_outChanInfo.end();++t_infoItr)
      {
         if(t_nodes2.count(StringUtil::toString(t_infoItr->m_channelNo)) )
         {
            t_infoItr->m_expression = t_nodes2[t_infoItr->m_channelNo.c_str()];
   	        t_value = t_nodes2[t_infoItr->m_channelNo.c_str()];
			   	  //将话单中的宏定义字段转换成数字
			      if(CdrFormat::parseAttribMacro(t_value,r_attribMacros)==false)
			      {
			        #ifdef _DEBUG_
			          cout<<"rule:"<<t_infoItr->m_expression<<" macros define error!"
			              <<__FILE__<<__LINE__<<endl;
			        #endif
              theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"rule:" + t_infoItr->m_expression + " macros define error!");
			        return false;
			      }

            t_infoItr->m_outRule = r_config->setScriptApplet(t_value);
            if(t_infoItr->m_outRule==NULL)
            {
              #ifdef _DEBUG_
                cout<<"out channel rule defined error!"<<t_infoItr->m_expression<<" "
                    <<__FILE__<<__LINE__<<endl;
              #endif
              theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"out channel rule defined error!" + t_infoItr->m_expression);
              return false;
            }
         }
      }

    }

/*
    ///输入字段
    t_outputParam.m_inFormatName  = t_nodes1["input_field_no"];
    if(FilterRuleMgr::setCdrFormat(t_outputParam.m_inFormatName,r_cdrFormats,t_outputParam.m_pFormat)==false)
    {
      #ifdef _DEBUG_
        cout<<"inputcdrformat ："<<t_outputParam.m_inFormatName<<" no defined!"
            <<__FILE__<<__LINE__<<endl;
      #endif
      return false;
    }
*/
    ///输出字段
     t_outputParam.m_outputFormatName  = t_nodes1["output_field_no"];
     if(FilterRuleMgr::setCdrFormat(t_outputParam.m_outputFormatName,r_cdrFormats,t_outputParam.m_bFormat)==false)
     {
       #ifdef _DEBUG_
         cout<<"outputcdrformat ："<<t_outputParam.m_outputFormatName<<" no defined!"
             <<__FILE__<<__LINE__<<endl;
       #endif
       theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"outputcdrformat ：" + t_outputParam.m_outputFormatName + " no defined!");
       return false;
     }


   t_outputParam.m_subChanName = "channel_" + StringUtil::toString(chNo);
   m_outputParam.push_back(t_outputParam);

   chNo++;
   sprintf(channelNo,"%d",chNo);
   t_path=SECTDELIM + t_appName + SECTDELIM + t_channelNo + SECTDELIM +"channel_" + channelNo;
   t_vname = "output_channel_no";
  }
  ::sort(m_outputParam.begin(),m_outputParam.end());
  return true;
}

bool OutputRuleMgr::valid() const
{
   for(vector<OutputParam>::const_iterator t_out_Itr=m_outputParam.begin();
       t_out_Itr!=m_outputParam.end();++t_out_Itr)
      {
         for(vector<OutChanInfo>::const_iterator t_Itr=t_out_Itr->m_outChanInfo.begin();
             t_Itr!=t_out_Itr->m_outChanInfo.end();++t_Itr)
         {
           if(FilterRuleMgr::isPath(t_Itr->m_desPath)==false)
           {
             #ifdef _DEBUG_
	             cout<<"m_desPath:"<<t_Itr->m_desPath<<" is not exists!"
	                 <<__FILE__<<__LINE__<<endl;
             #endif
             theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"m_desPath:" + t_Itr->m_desPath + "is not exists!");
             return false;
           }
         }
       }
  return true;
}

OutputRuleMgr::OutputParam *OutputRuleMgr::getOutputParam(const PString & r_subChanName)
{
  OutputParam   t_outputParam;
  vector<OutputRuleMgr::OutputParam>::iterator r_Itr;
  t_outputParam.m_subChanName=r_subChanName;
  r_Itr=::lower_bound(m_outputParam.begin(),m_outputParam.end(),t_outputParam);
  if(r_Itr==m_outputParam.end()) return NULL;
  if(strncmp(r_subChanName,r_Itr->m_subChanName.c_str(),r_Itr->m_subChanName.length())!=0)
  {
    return NULL;
  }
  return &(*r_Itr);
}

int OutputRuleMgr::getallOutChannelNumber()
{
  int i=0;
  for(vector<OutputParam>::const_iterator r_outItr=m_outputParam.begin();
      r_outItr!=m_outputParam.end();++r_outItr)
  {
     if(r_outItr->m_outChanInfo.size()<=0)
     {
       i=0;
       break;
     }
     i+=r_outItr->m_outChanInfo.size();
  }
  return i;
}

void OutputRuleMgr::dump() const
{
  cout<<"............Out Rule Mgr ........"<<endl;
  cout<<"-----------------------------------"<<endl;

  for(vector<OutputParam>::const_iterator r_outItr=m_outputParam.begin();
      r_outItr!=m_outputParam.end();++r_outItr)
  {
    cout<<"m_subChanName  =  "<<r_outItr->m_subChanName<<"\tbegin........"<<endl;
    cout<<"m_outputName="<<r_outItr->m_outputName<<endl;
    cout<<"m_fileType="<<r_outItr->m_fileType<<endl;
    cout<<"m_defaul_outputName="<<r_outItr->m_defaul_outputName<<endl;
    cout<<"m_prefixName="<<r_outItr->m_prefixName<<endl;
    cout<<"m_expressionName="<<r_outItr->m_expressionName<<endl;
    cout<<"m_outputFormatName="<<r_outItr->m_outputFormatName<<endl;

    cout<<"............Output CdrFormat ........"<<endl;
    cout<<"--------CdrFormat : "<<r_outItr->m_bFormat->m_formatName<<" -----------------"<<endl;
    //cout<<"\tm_fieldDeliminater="<<r_outItr->m_bFormat->m_fieldDeliminater<<endl;
    //cout<<"\tm_lineDeliminater ="<<r_outItr->m_bFormat->m_lineDeliminater<<endl;
    //cout<<"\tm_storeType       ="<<r_outItr->m_bFormat->m_storeType<<endl;
    for(vector<FieldFormat>::const_iterator t_Itr=r_outItr->m_bFormat->m_fields.begin();
        t_Itr!=r_outItr->m_bFormat->m_fields.end();++t_Itr)
    {
      t_Itr->dump();
    }

     for(vector<OutChanInfo>::const_iterator t_Itr1=(r_outItr->m_outChanInfo).begin();
         t_Itr1!=(r_outItr->m_outChanInfo).end();++t_Itr1)
     {
       cout<<"channelNo: "<<t_Itr1->m_channelNo<<"\t"
       <<"prefix: "<<t_Itr1->m_prefix<<"\t"
       <<"m_desPath: "<<t_Itr1->m_desPath<<"\t"
       <<"m_expression: "<<t_Itr1->m_expression<<endl;
     }
     cout<<"m_subChanName  =  "<<r_outItr->m_subChanName<<"\tend........"<<endl;
  }
  cout<<"-----------------------------------\n"<<endl;

}


bool DividRule::VectorDividParam::initialize(const CdrFormat * &r_cdrFormats)
{
  char buffer[1024];
  if(m_fidName<=0) return false;
  memset(buffer,0,sizeof(buffer));
  strcpy(buffer,m_fidName.c_str());

  m_DividParamList.clear();
  for(char *s = strtok(buffer, ")"); s != NULL; s = strtok(NULL, ")"))
  {
      int fieldIndex, offset, length;
      char fixstr[20];
      DividParam       t_DividParam;
      
      if(sscanf(s, s == buffer? "%d%*[ (]%d%*[ ,]%d" : "%*[, ]%d%*[ (]%d%*[ ,]%d",
          &fieldIndex, &offset, &length) != 3)
      {
      	   if(sscanf(s, s == buffer? "%d%*[ (]%s" : "%*[, ]%d%*[ (]%s",&fieldIndex, fixstr) == 2)
           {
           	 if(fieldIndex!=0) return false;
             t_DividParam.m_fieldId = fieldIndex;
             t_DividParam.m_fixstr  = fixstr;
             m_DividParamList.push_back(t_DividParam);
             continue;
           }
           else
           {
           	 return false;
           }
      }
      t_DividParam.m_fieldId = fieldIndex;
      t_DividParam.m_pos     = offset;
      t_DividParam.m_len     = length;

     for(vector<FieldFormat>::const_iterator r_Itr=r_cdrFormats->m_fields.begin();r_Itr!=r_cdrFormats->m_fields.end();r_Itr++)
     {
         if(t_DividParam.m_fieldId == r_Itr->m_position)
         {
            t_DividParam.m_cdrid = r_Itr->m_id;
            break;
         }
     }
     //未找到
     if(0 == t_DividParam.m_cdrid)
     {
		     #ifdef _DEBUG_
		       cout<<"排重配置输入话单格式中第 "<<t_DividParam.m_fieldId<<" 个字段输入话单中 no defined!"
		           <<__FILE__<<__LINE__<<endl;
		     #endif
		     theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"排重配置输入话单格式中第 " + StringUtil::toString(t_DividParam.m_fieldId) + " 个字段输入话单中 no defined!");
         return false;
     }

     m_DividParamList.push_back(t_DividParam);
  }
  //::sort(m_DividParamList.begin(),m_DividParamList.end());
  return true;
}


bool DividRule::initialize(SysParam *r_sysparam,
                              const char* r_appName,
                              const string & r_subPath,
                              const vector<CdrFormat> &rt_cdrFormats,
                              const CdrAttribMacros &rt_attribMacros,
                              FilterConfig *rs_config)
{
  string t_path,t_subPath,t_value;
  string t_appName,t_vname,t_subNodeName;
  vector<string>  t_subNodeList;
  vector<string>::iterator t_nodeItr;
  VectorDividParam t_vectorDividParam;
  map<string,string> t_nodes;
  PString            t_pstr;

  t_appName = r_appName;
  t_path = r_subPath;

  //获取数据源
  t_vname = "source";
  if(r_sysparam->getValue(t_path,t_vname,t_value)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysParam->getValue("<<t_path<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysParam->getValue(" + t_path + ") false!");
    return false;
  }
  m_sourceType.clear();
  m_SourceName = t_value;
  m_SourceName.split(',',m_sourceType);

  if(m_sourceType.size()<=0)
  {
     #ifdef _DEBUG_
       cout<<"unique ："<<"t_vectorDividParam.m_sourceType"<<" no defined!"
           <<__FILE__<<__LINE__<<endl;
     #endif
     theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"unique ："+ m_SourceName + " no defined!");
     return false;
  }

  //获取最大排重字段长度
  t_vname = "max_fid_length";
  if(r_sysparam->getValue(t_path,t_vname,t_value)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysParam->getValue("<<t_path<<" "<<t_vname<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"unique ：" + m_SourceName + " no defined!");
    return false;
  }
  m_maxLength= atoi(t_value.c_str());
  if(m_maxLength<=0)
  {
    #ifdef _DEBUG_
      cout<<"r_sysParam->getValue("<<t_path<<" "<<t_vname<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysParam->getValue(" + t_path + " "+t_vname+") false!");
    return false;
  }

  //对应的输入字段
  t_vname = "input_field_no";
  if(r_sysparam->getValue(t_path,t_vname,t_value)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysParam->getValue("<<t_path<<" "<<t_vname<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysParam->getValue(" + t_path + " " + t_vname + ") false!");
    return false;
  }
  m_rrFormatName  = t_value;
  if(FilterRuleMgr::setCdrFormat(m_rrFormatName,rt_cdrFormats,m_rrFormat)==false)
  {
    #ifdef _DEBUG_
      cout<<"inputcdrformat ："<<m_rrFormatName<<" no defined!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"inputcdrformat ：" + m_rrFormatName + " no defined!");
    return false;
  }

  if(r_sysparam->setSectionPath(t_path)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysParam->setSectionPath("<<t_path<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysParam->setSectionPath(" + t_path + ") false!");
    return false;
  }
  t_subNodeList.clear();
  while (r_sysparam->getSubSection(t_subNodeName) == true)
  {
    t_subNodeList.push_back(t_subNodeName);
  }

  m_vectorDividParamList.clear();
  for(vector<string>::iterator t_nodeItr=t_subNodeList.begin();
      t_nodeItr!=t_subNodeList.end();++t_nodeItr)
  {
    t_subPath=t_path+SECTDELIM+*t_nodeItr;

    t_nodes.clear();
    if (!r_sysparam->getValue(t_subPath, t_nodes)) 
    {
      #ifdef _DEBUG_
        cout<<"get common config param false!"<<t_path
            <<" "<<__FILE__<<__LINE__<<endl;
      #endif
      theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"get common config param false!" + t_path);
      return false;
    }

    if(t_nodes["rewrite"].length()<=0)
    {
      t_vectorDividParam.m_reWrite = "Y";
    }
    else
    {
      t_vectorDividParam.m_reWrite = t_nodes["rewrite"];
    }

    if(t_nodes["rule"].length()<=0) 
    {
       return false;
    }
    else
   {
     t_vectorDividParam.m_ruleName = t_nodes["rule"];
   	 t_value = t_nodes["rule"];
   	 //将话单中的宏定义字段转换成数字
     if(CdrFormat::parseAttribMacro(t_value,rt_attribMacros)==false)
     {
       #ifdef _DEBUG_
         cout<<"rule:"<<t_vectorDividParam.m_ruleName<<" macros define error!"
             <<__FILE__<<__LINE__<<endl;
       #endif
       theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"rule:"+t_vectorDividParam.m_ruleName+" macros define error!");
       return false;
     }
     t_vectorDividParam.m_DividRule = rs_config->setScriptApplet(t_value);

      if(t_vectorDividParam.m_DividRule==NULL)
      {
        #ifdef _DEBUG_
          cout<<"Divid rule defined error!"<<t_vectorDividParam.m_ruleName<<" "
              <<__FILE__<<__LINE__<<endl;
        #endif
        theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"Divid rule defined error!"+t_vectorDividParam.m_ruleName);
        return false;
      }

    }

    if(t_nodes["fid"].length()<=0)
    {
    	//不配置表示文件排重
      t_vectorDividParam.m_fidName = "file type rr";
    }
    else
    {
      t_vectorDividParam.m_fidName = t_nodes["fid"];
      if(t_vectorDividParam.initialize(m_rrFormat) == false)
      {
		     #ifdef _DEBUG_
		       cout<<"出错输入话单格式名： "<<m_rrFormatName<<" "
		           <<__FILE__<<__LINE__<<endl;
		     #endif
		     theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"出错输入话单格式名： " + m_rrFormatName);
         return false;
      }
    }
    m_vectorDividParamList.push_back(t_vectorDividParam);

  }

  return true;
}

DividRule::VectorDividParam *DividRule::getVectorDividParamRule(FilterCdr *r_cdr)
{
  for(vector<DividRule::VectorDividParam>::iterator r_ItrParam = m_vectorDividParamList.begin();r_ItrParam != m_vectorDividParamList.end();r_ItrParam++)
  {
     if(r_ItrParam->m_ruleName.length()>0)
     {
        if((r_ItrParam->m_DividRule)->matches())
        {
          return &(*r_ItrParam);
        }
     }
  }
  return NULL;
}

void DividRule::dump() const
{
  cout<<"............Divid Rule Mgr ........"<<endl;

  cout<<"source<\t";
  for(vector<PString>::const_iterator r_Itr=m_sourceType.begin();
      r_Itr!=m_sourceType.end();++r_Itr)
  {
      cout<<*r_Itr<<"\t";
  }
    cout<<">source"<<endl;
    cout<<"m_maxLength    = "<<m_maxLength<<endl;
    cout<<"m_rrFormatName = "<<m_rrFormatName<<endl;
  
    for(vector<VectorDividParam>::const_iterator r_ListItr=m_vectorDividParamList.begin();
        r_ListItr!=m_vectorDividParamList.end();++r_ListItr)
    {
        cout<<"m_ruleName  = "<<r_ListItr->m_ruleName<<endl;
        //cout<<"m_fidName   = "<<r_ListItr->m_fidName<<endl;
        for(vector<DividParam>::const_iterator r_DividItr=r_ListItr->m_DividParamList.begin();
            r_DividItr!=r_ListItr->m_DividParamList.end();++r_DividItr)
        {
            cout<<"field sequence   = "<<r_DividItr->m_fieldId<<"  "<<r_DividItr->m_pos<<
           "  "<<r_DividItr->m_len<<"  "<<r_DividItr->m_fixstr<<endl;
        }
    }
  cout<<"-----------------------------------\n"<<endl;

}

bool DividRuleMgr::initialize(SysParam *r_sysparam,
                             const char* r_appName,
                             const vector<CdrFormat> &r_cdrFormats,
                             const CdrAttribMacros &r_attribMacros,
                             FilterConfig *r_config)
{
  string t_path,t_subPath,t_subNodeName;
  string t_appName;
  vector<string>  t_subNodeList;
  vector<string>::iterator t_nodeItr;
  DividRule   t_dividRule;

  t_appName = r_appName;
  t_path= SECTDELIM + t_appName + SECTDELIM + "unique";
  if(r_sysparam->setSectionPath(t_path)==false)
  {
    #ifdef _DEBUG_
      cout<<"r_sysParam->setSectionPath("<<t_path<<") false!"
          <<__FILE__<<__LINE__<<endl;
    #endif
    theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"r_sysParam->setSectionPath(" + t_path + ") false!");
    return false;
  }
  t_subNodeList.clear();

  while (r_sysparam->getSubSection(t_subNodeName) == true)
  {
    t_subNodeList.push_back(t_subNodeName);
  }
  for(vector<string>::iterator t_nodeItr=t_subNodeList.begin();
      t_nodeItr!=t_subNodeList.end();++t_nodeItr)
  {
    t_subPath=t_path+SECTDELIM+*t_nodeItr;
    if(t_dividRule.initialize(r_sysparam,r_appName,t_subPath,r_cdrFormats,r_attribMacros,r_config)==false)
    {
        #ifdef _DEBUG_
          cout<<"t_dividRule.initialize(r_sysParam,"<<r_appName<<","
              <<t_path<<") false!"
              <<__FILE__<<__LINE__<<endl;
        #endif
        theErrorMessages->insert(FILTER_ERROR_INITIALIZE,"t_dividRule.initialize(r_sysParam,"+t_appName+","+t_path+") false!");
        return false;
    }
    m_dividRuleList.push_back(t_dividRule);
  }
  return true;
}

DividRule *DividRuleMgr::getDividRule(const PString & r_sourceType)
{
  for(vector<DividRule>::iterator r_ItrRule = m_dividRuleList.begin();r_ItrRule != m_dividRuleList.end();r_ItrRule++)
  {
     for(vector<PString>::iterator r_Itr = r_ItrRule->m_sourceType.begin();
          r_Itr != r_ItrRule->m_sourceType.end();r_Itr++