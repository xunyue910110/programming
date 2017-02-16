#include "ReadParam.h"

ReadParam::ReadParam(TypdayVector *vday,TypotherVector *vother)
{
    m_dayVector   = vday;
    m_otherVector = vother;
}
int ReadParam::readParam()
{
    int iReturn;
    
    iReturn = readDayParam();//1
    if (iReturn < 0)
    {
        dbi.disconnect();
        return iReturn;
    }
    
    iReturn = readOtherParam();//2
    return iReturn;
}

//把日报参数 写进全局的 日报参数容器中
int ReadParam::readDayParam()
{
    StringVector outvector;
    StringVector errvector;
    vector<string>::iterator it;
    int i,nTotal,nSuccess,nError;
    int total=0;
    char selectarray[1000];
    DayReportParameter day;//日报参数结构
    int iReturn;
    
    m_dayVector->clear();//日报参数结构的容器
    sprintf(selectarray,"SELECT %s FROM %s",DAY_PARAM_FIELD,DAY_PARAM_TABLE); 
    
//Report.h:#define DAY_PARAM_FIELD		"flag,filenamehead,filenamedate,\
//Report.h:filename3,filenameend,procedurename,reportpath,backuppath,\
//Report.h:processtime,reportno,processlimittime,lastprocessdate"
    
    if(dbi.dataBind(outvector,errvector))
      return -1;
    
    nTotal=DEFAULTARRAYLINE;
    
    while(nTotal==DEFAULTARRAYLINE)
    {
        if(dbi.bulkCopy(selectarray,nTotal,nSuccess,nError))
           return -1;
        if(nTotal>0)
        {
            total += nTotal;
            for(it=outvector.begin(),i=0;it!=outvector.end();it++)
            {
                i = i%12;
                switch (i)
                {
                case 0:
                  memset(&day,0,sizeof(DayReportParameter));
                  day.flag = atoi((*it).c_str());
                  break;
                case 1:
                  strncpy(day.fileNameHead,(*it).c_str(),7);
                  break;
                case 2:
                  strncpy(day.fileNameDate,(*it).c_str(),8);
                  break;
                case 3:
                  strncpy(day.fileName3,(*it).c_str(),7);
                  break;
                case 4:
                  strncpy(day.fileNameEnd,(*it).c_str(),7);
                  break;
                case 5:
                  strncpy(day.procedureName,(*it).c_str(),31);
                  break;
                case 6:
                  strncpy(day.reportPath,(*it).c_str(),79);
                  break;
                case 7:
                  strncpy(day.backupPath,(*it).c_str(),79);
                  break;
                case 8:
                  day.processTime = atol((*it).c_str());
                  break;
                case 9:
                  day.reportNo = atoi((*it).c_str());
                  break;
                case 10:
                  day.processLimitTime = atoi((*it).c_str());
                  break;
                case 11:
                  strncpy(day.lastProcessDate,(*it).c_str(),8);
                  //凑满一条记录后，放入容器
                  m_dayVector->push_back(day);
                  break;
                }
                
                i++;
            }//for
        }
    }//while
    
    return 1;
}

//把月报参数 写进全局的 月报参数容器中
int ReadParam::readOtherParam()
{
    StringVector outvector;
    StringVector errvector;
    vector<string>::iterator it;
    int i,nTotal,nSuccess,nError;
    int total=0;
    char selectarray[1000];
    char cTemp[40],cTemp1[40];
    char *ptr;
    OtherReportParameter other;//月报参数结构
    int iReturn;
    string sFileName;

    sprintf(selectarray,"SELECT %s FROM %s",OTHER_PARAM_FIELD,OTHER_PARAM_TABLE);
    
//#define OTHER_PARAM_FIELD	"flag,filename,procedurename,reportpath,\
//backuppath,processtime,reportno,processlimittime,lastprocessdate"
  
    if(dbi.dataBind(outvector,errvector))
      return -1;

    m_otherVector->clear();//月报参数结构的容器

    nTotal=DEFAULTARRAYLINE;
    while(nTotal==DEFAULTARRAYLINE)
    {
        if(dbi.bulkCopy(selectarray,nTotal,nSuccess,nError))
           return -1;
        if(nTotal>0)
        {
            total += nTotal;
            
            for(it=outvector.begin(),i=0;it!=outvector.end();it++)
            {
                i = i%9;
                switch (i)
                {
                case 0:
                  memset(&other,0,sizeof(OtherReportParameter));
                  other.flag = atoi((*it).c_str());
                  break;
                case 1:
                  strncpy(other.fileName,(*it).c_str(),39);
                  sFileName = other.fileName;
                  break;
                case 2:
                  strncpy(other.procedureName,(*it).c_str(),31);
                  break;
                case 3:
                  strncpy(other.reportPath,(*it).c_str(),79);
                  break;
                case 4:
                  strncpy(other.backupPath,(*it).c_str(),79);
                  break;
                case 5:
                  strncpy(other.processTime,(*it).c_str(),12);
                  break;
                case 6:
                  other.reportNo = atoi((*it).c_str());
                  break;
                case 7:
                  other.processLimitTime = atoi((*it).c_str());
                  break;
                case 8:
                  strncpy(other.lastProcessDate,(*it).c_str(),8);
#ifdef _DEBUG_
    cout<<"--td_otherreport中读出的原始参数为:"<<endl;
    cout<<"--other.flag:"             <<other.flag             <<endl;
    cout<<"--other.fileName:"         <<other.fileName         <<endl;
    cout<<"--other.procedureName:"    <<other.procedureName    <<endl;
    cout<<"--other.reportPath:"       <<other.reportPath       <<endl;
    cout<<"--other.backupPath:"       <<other.backupPath       <<endl;
    cout<<"--other.processTime:"      <<other.processTime      <<endl;
    cout<<"--other.reportNo:"         <<other.reportNo         <<endl;
    cout<<"--other.processLimitTime:" <<other.processLimitTime <<endl;
    cout<<"--other.lastProcessDate:"  <<other.lastProcessDate  <<endl;
    cout<<"---------------------"<<endl;
#endif
                  
                  ptr = strstr(other.fileName, "YYYYMM");
                  if ((ptr == NULL)&&(strcmp(other.lastProcessDate,"Y")==0))//只处理一次的特殊报表且已处理过
                    break;
                  if (ptr == NULL)
                  {
#ifdef _DEBUG_
    cout<<"--cy定位临时用"<<endl;
#endif
                    m_otherVector->push_back(other);
                    break;
                  }
                  
                  getcmonth(cTemp);
                  if (strncmp(cTemp,other.lastProcessDate,6) == 0)
                    break;
                  
                  //下面的处理将把 1 other.fileName ,2 other.processTime 写全
                  
                  //modi by cy 2002.10.22 局数据月报文件名类似长途结算
                  //|| added by cy 2003.06
                  /*
                  if (other.processTime[0] == '0'                        ||
                      strstr(other.procedureName,"monstatisnew_")   !=0  ||//新月报   
                      strstr(other.fileName,     "CXAYYYYMMA21001") !=0    //下旬旬报取系统时间的上一月 
                     )  
                    getclastmonth(cTemp);
                  else//长途结算  2003.08.25长途结算报表改过参数后processTime[0] ='0' 也不走此分支了 每月4号上报
                    getcmonth(cTemp);//也包括局数据月报 modi by cy 2002.10.22
                   */
#ifdef _DEBUG_
    cout<<"other.processTime未写全之前为:"<<other.processTime<<endl;
#endif
                  
                  //1.YYYYMMA==>YYMM;2.YYYYMMB==>MM;3.YYYYMM==>YYYYMM;
                  if (sFileName.find("YYYYMMA") != -1)
                    sFileName.replace(sFileName.find("YYYYMMA"),7,cTemp+2);
                  else if (sFileName.find("YYYYMMB") != -1)
                    sFileName.replace(sFileName.find("YYYYMMB"),7,cTemp+4);
                  else
                    sFileName.replace(sFileName.find("YYYYMM"),6,cTemp);
                
                  sprintf(other.fileName,sFileName.c_str());
                  //1 other.fileName 已写全
                  
                  getcmonth(cTemp);
                  sprintf(cTemp1,"%6s%6s",cTemp,other.processTime);
                  sprintf(other.processTime,cTemp1);//2 把other.processTime  年月日时分 写全了
#ifdef _DEBUG_
    cout<<"other.processTime写全之后为:"<<other.processTime<<endl;
#endif
                  //一个月只做一次
                  sprintf(other.lastProcessDate,"M");
                  
                  m_otherVector->push_back(other);
                  break;
                }
                i++;
            }
        }
    }//while
    return 1;
}


int ReadParam::connectDb()
{
    int iReturn = dbi.connect(dbUser,dbPassword,dbServer);
    
    #ifdef _FOR_SYBASE_
      if (iReturn >= 0)
        if (strlen(dbName) != 0)
          iReturn = dbi.useDb(dbName);
    #endif
    
    return iReturn;
}

