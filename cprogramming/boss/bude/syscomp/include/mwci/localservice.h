#ifndef __LOCALSERVICE_H
#define __LOCALSERVICE_H
 /*********************************************************************************
 * 描述： localservice.h
 * 说明： 本地化保存
 * 环境： 基于标准C++
 * 日期： ZHIJUN SONG 创建 于 2004-12-20
 * 备注： Version 0.10  Buildno

 **********************************************************************************/

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>


#include <string>

#include <vector>


#ifdef __NOSTD__
#define STD
#else
#define STD  std::
#endif


#define MD5DIGEST     //是否需要数字签名
#undef  MD5DIGEST

 //tuxedo 缓存类
class CFmlBuf;

namespace localservice
{

//负责字符的转换
class TransStr
{
public :
     TransStr(const XMLCh* const toTranscode)
        :szStr_(0),szUni_(0)
     {
         szStr_ =  xercesc::XMLString::transcode(toTranscode);
     }

     TransStr(const char* const toTranscode)
        :szStr_(0),szUni_(0)
     {
         szUni_ =  xercesc::XMLString::transcode(toTranscode);
     }


     ~TransStr()
     {

        xercesc::XMLString::release(&szStr_);
        xercesc::XMLString::release(&szUni_);
     }

    const char* c_str() const
    {
        return szStr_;
    }

    const XMLCh* c_uni() const
    {
        return szUni_;
    }

private :
    char*   szStr_;
    XMLCh*  szUni_;

};




class Service;


class Field;
class Item;
class Service 
{
public:
    Service();
    bool LoadFromXml(const char * szXmlFile); //从文件中构建

    bool RebuildService();        //重新构建一个xml

    bool SaveToXml(const char * szXmlFile); //保存到Xml 

    static void  Initialize()
    {
         xercesc::XMLPlatformUtils::Initialize();

    }

    static void Terminate()
    {
         xercesc::XMLPlatformUtils::Terminate();
    }

    ~Service();

    int getInputCount() const ;
    int getOutputCount()const ;

    bool setInputCount(int iCount);   //设置输入参数
    bool setOutputCount(int iCount);  //设置输出参数

    //增加输入域
    Field addInputField(const char * szName,const char * szFieldId);
    //增加输出域
    Field addOutputField(const char * szName,const char * szFieldId);

    //增加输入输出域
    Field addField(Field & fld,const char *szName,const char * szFieldId);


    //在对应的Field 下增加Item
    Item  addItem(Field &fld,const char * szValue);


    Field  getFirstInputField() const ; //取得第一个输入字段
    Field  getFirstOutputField()const ; //取得第一个输出字段

    Field  getNextField(Field &fld)const ; //取得下一个field ,如果没有就返回空值


    Item  getFirstItem( Field &fld)const;   //取得对应field字段下的第一个item
    Item  getNextItem( Item & item)const ; //取得下一个item，如果没有就返回空


    /////////////////////////////////////////

    //比较对应的输入缓存    输入的缓存的格式为serialnumber,13951633027
    bool compareInputBuffer(STD vector<STD string> &vc);


    //本地化导入
    bool loadFromLocal(CFmlBuf *inbuf,CFmlBuf * outbuf);
    bool readFmlInputBuffer(CFmlBuf *fmlbuf);
    bool readFmlOutputBuffer(CFmlBuf *fmlbuf);

    bool saveToLocal(CFmlBuf* inbuf, CFmlBuf* outbuf);
    bool writeFmlBuffer(CFmlBuf *infmlbuf,CFmlBuf *outfmlbuf) ;

protected:



private:
       Service(const Service & svc)    //不支持拷贝构造
       {

       }

       Service & operator = (const Service & svc)   //不支持赋值操作
       {
         return *this;
       }

        xercesc::XercesDOMParser *parser;

        xercesc::DOMDocument *docRead;
        xercesc::DOMDocument *docWrite;


        xercesc::DOMImplementation* impl;

};


class Field  
{
public:
    friend class Service;
    Field()
    {
    }
    Field(xercesc::DOMNode * node);
    STD string Name;
    STD string FieldId;

    getNodeName();

    xercesc::DOMNode * getNode()
    {
       return node_;
    }
    bool operator == (const Field &fld)
    {
        return (this->node_ == fld.node_);
    }

    bool operator != (const Field &fld)
    {

        return !(this->node_ == fld.node_);

    }


private:
    xercesc::DOMNodeList * getChildNodes()
    {
        return node_->getChildNodes();
    }

    xercesc::DOMNode * node_;
};

class Item 
{
public:
    Item(xercesc::DOMNode * node);

    STD string Value;

    xercesc::DOMNode * getNode()
    {
        return node_;
    }

    bool operator == (const Item &it)
    {
        return (this->node_ == it.node_);
    }

    bool operator != (const Item &it)
    {
        return !(this->node_ == it.node_);
    }

private:
    xercesc::DOMNode * node_;
};


//////////////////////////////以下为名字服务
class LocalContextInfo
{
public:
        LocalContextInfo(LocalContextInfo * ctx){}
        LocalContextInfo(){}
        ~LocalContextInfo(){}

       std::string  ContextId;
       std::string  EparchyCode;
       std::string  CityCode;
       std::string  OriginCode;
       std::string  NamingUptTime;
       std::string  NeedLocalService;

};

class RemoteNamingSrv
{
public:
     RemoteNamingSrv(RemoteNamingSrv * srv){}
     RemoteNamingSrv(){}
     std::string Ip;
     std::string Port;
     std::string MachineName;
};




//名字
class Naming
{
public:
      Naming(xercesc::DOMNode * node);
      ~Naming(){};

      
    STD string MachineName;
    STD string Ip;
    STD string Port;
    STD string PhysicalName;
    STD string TypeCode;

    xercesc::DOMNode * getNode() const
    {
        return node_;
    }

    bool operator != (const Naming &name)
    {

        return !(this->node_ == name.node_);

    }
    bool operator == (const Naming &name)
    {
        return (this->node_ == name.node_);
    }


private:
      xercesc::DOMNode * node_;
};

class NameService
{
public:
     NameService(xercesc::DOMNode * node);
     STD string LogicalName;
     xercesc::DOMNode * getNode()  const 
     {
        return node_;
     }

    bool operator != (const NameService &ns)
    {
    
        return !(this->node_ == ns.node_);

    }
    bool operator == (const NameService &ns)
    {
        return (this->node_ == ns.node_);
    }


private:
     xercesc::DOMNode * node_;
};


class LocalNameService
{
public:
    LocalNameService();
    bool LoadFromXml(const char * szXmlFile); //从文件中构建

    //取得对应nameservice的第一个naming
    Naming  getFirstNaming(NameService & ns) ;
    Naming  getNextNaming( Naming & nm);

    //增加NameService
    NameService addNameService(const char *szLogicName);

    //删除一个NameService
    bool deleteNameService(const char * szLogicName);

    //删除一个Naming.
    bool deleteNaming(NameService &ns,char * MachineName);

    //取得一个NameService
    NameService getNameService(const char *szLogicName);


    //增加Naming
    Naming addNaming(const NameService &ns,
                      char * MachineName,
                      char * Ip,
                       char * Port,
                       char * PhysicalName,
                       char * TypeCode);
   //设置Naming
   Naming setNaming(const Naming &name,
                      char * MachineName,
                      char * Ip,
                       char * Port,
                       char * PhysicalName,
                       char * TypeCode);


    bool SaveToXml(); //保存到Xml



    LocalContextInfo getLocalContextInfo();

    RemoteNamingSrv getRemoteNamingSrv();




    static void  Initialize()
    {
         xercesc::XMLPlatformUtils::Initialize();

    }

    static void Terminate()
    {
         xercesc::XMLPlatformUtils::Terminate();
    }

    ~LocalNameService();


protected:



private:
       LocalNameService(const NameService & svc)    //不支持拷贝构造
       {

       }

       LocalNameService & operator = (const NameService & svc)   //不支持赋值操作
       {
         return *this;
       }

        xercesc::XercesDOMParser *parser;

        xercesc::DOMDocument *docRead;
        xercesc::DOMDocument *docWrite;


        xercesc::DOMImplementation* impl;

};







}

#endif
 
