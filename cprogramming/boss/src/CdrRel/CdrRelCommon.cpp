#include "CdrRelCommon.h"
#include <algorithm>

bool CdrRelConfig::getSysParam(SysParam *r_sysParam,const char * r_appName,const int &r_channelNo)
{
    string appName = r_appName;
    string path,section;
    map<string,string> nodes;
    vector<string> fileTypeNames;
    FileType fileType;
    
    char channelNo[10];
    sprintf(channelNo,"%d",r_channelNo);
    path = SECTDELIM + appName + SECTDELIM + channelNo;
    nodes.clear();
    if (!r_sysParam->getValue(path, nodes)) 
    {
        #ifdef _DEBUG_
            cout<<"get Channel[" << r_channelNo << "] config param false!"<<path<<" "<<__FILE__<<__LINE__<<endl;
        #endif
        return false;
    }
    
    m_dbUser       = nodes["DbUserName"  ];
    m_dbPassWord   = nodes["DbPassWord"  ];
    m_dbServerName = nodes["DbServerName"];
    m_inputPath    = nodes["InputPath"   ];
    m_backupPath   = nodes["BackupPath"  ];
    m_outputPath   = nodes["OutputPath"  ];
    m_errorPath    = nodes["ErrorPath"   ];
    
    string fileRulePath = SECTDELIM + appName + SECTDELIM + "Common" + SECTDELIM + "FileType";
    r_sysParam->setSectionPath(fileRulePath);
    while (r_sysParam->getSubSection(section))
    {
      fileTypeNames.push_back(section);
    }
    for (int i = 0; i < fileTypeNames.size(); ++i)
    {
        fileType.reset();
        path = fileRulePath + SECTDELIM + fileTypeNames[i];
        nodes.clear();
        r_sysParam->getValue(path, nodes);
        
        fileType.m_fileTypeName = fileTypeNames[i];
        StringUtil::split(nodes["PreFix"],",",fileType.m_filePrefix);
        fileType.m_msisdnIndex    =  atoi(nodes["Msisdn"   ].c_str());
        fileType.m_beginDateIndex =  atoi(nodes["BeginDate"].c_str());
        fileType.m_beginTimeIndex =  atoi(nodes["BeginTime"].c_str());
        fileType.m_key1Index      =  atoi(nodes["Key1"     ].c_str());
        fileType.m_key2Index      =  atoi(nodes["Key2"     ].c_str());
        fileType.m_key3Index      =  atoi(nodes["Key3"     ].c_str());
        fileType.m_key4Index      =  atoi(nodes["Key4"     ].c_str());
        fileType.m_key5Index      =  atoi(nodes["Key5"     ].c_str());    
        fileType.m_table          =  nodes["Table"    ];    
        fileType.m_procedure      =  nodes["Procedure"];
        m_fileType.push_back(fileType);
    }
    
    #ifdef _DEBUG_
        cout << *this;
    #endif
    return true;
}

bool CdrRelConfig::getFileType(const string fileName, FileType &fileType)
{
    vector<string>::iterator itr;
    for (int i = 0; i < m_fileType.size(); ++i)
    {
        //itr = ::lower_bound(m_fileType[i].m_filePrefix.begin(),m_fileType[i].m_filePrefix.end(),fileName);
        //if(itr==m_fileType[i].m_filePrefix.end())
        //{
        //    continue;
        //}
        //else if(strncmp((*itr).c_str(),fileName.c_str(),(*itr).length())!=0)
        //{
        //    continue;
        //}
        //fileType = m_fileType[i];
        for (int j = 0; j < m_fileType[i].m_filePrefix.size(); ++j)
        {
            if (strncmp(m_fileType[i].m_filePrefix[j].c_str(),fileName.c_str(),m_fileType[i].m_filePrefix[j].length()) == 0)
            {
                fileType = m_fileType[i];
                return true;
            }
        }
    }
    return false;
}

ostream& operator<<(ostream& os,const CdrRelConfig& po)
{
    os << "Common : " << endl;
    for (int i = 0; i < po.m_fileType.size(); ++i)
    {
        os << "  FileType = " << po.m_fileType[i].m_fileTypeName << endl;
        os << "    Prefix = ";
        for (int j = 0; j < po.m_fileType[i].m_filePrefix.size(); ++j)
        {
            os << po.m_fileType[i].m_filePrefix[j] << " , ";
        }
        os << endl;
        os << "    Msisdn    = " << po.m_fileType[i].m_msisdnIndex    << endl;       
        os << "    BeginDate = " << po.m_fileType[i].m_beginDateIndex << endl;
        os << "    BeginTime = " << po.m_fileType[i].m_beginTimeIndex << endl;
        os << "    Key1      = " << po.m_fileType[i].m_key1Index      << endl;     
        os << "    Key2      = " << po.m_fileType[i].m_key2Index      << endl;     
        os << "    Key3      = " << po.m_fileType[i].m_key3Index      << endl;     
        os << "    Key4      = " << po.m_fileType[i].m_key4Index      << endl;     
        os << "    Key5      = " << po.m_fileType[i].m_key5Index      << endl;     
        os << "    Table     = " << po.m_fileType[i].m_table          << endl;      
        os << "    Procedure = " << po.m_fileType[i].m_procedure      << endl;  
    }
    os << "Channel : DbUser       = " << po.m_dbUser       << endl;
    os << "          DbPassWord   = " << po.m_dbPassWord   << endl;    
    os << "          DbServerName = " << po.m_dbServerName << endl;    
    os << "          InputPath    = " << po.m_inputPath    << endl;
    os << "          BackupPath   = " << po.m_backupPath   << endl;
    os << "          OutputPath   = " << po.m_outputPath   << endl;
    os << "          ErrorPath    = " << po.m_errorPath    << endl;
    return os;
}
