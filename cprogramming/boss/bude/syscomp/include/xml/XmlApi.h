/*    
    
  @author Shandy
  @author Copyright &copy; Linkage. All Rights Reserved.
  @version 1.1
  @date 2006-07-10
  @sa 请参考: http://www.lianchuang.com
*/

#ifndef XML_API_H
#define XML_API_H

#include "base/config-all.h"

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <stdio.h>

USING_NAMESPACE(std);

#ifndef   XML_CDATA   
#define XML_CDATA   "<![CDATA[]]>\0"
#endif
/**
  
  @class XmlApi XML文档处理类
 
  @breif XML文档处理类
  
  @file
    对libxml2的api封装.
    <PRE><code>
    @htmlonly
    api封装使用了XPATH规范和第三方软件产品libxml2
    
    XPATH简述：
    XPATH结构：/step/step/...
    其中step的结构为：axis::node-test[prediates]
        axis：关键字，有且仅有一个，它表示结果节点与上下文节点的关系，有如下的值：
              ancestor,ancestor-or-self,attribute,child,descendant,descendant-or-self,
              following,following-sibling,namespace,parent,preceding,preceding-sibling,self 
              可以省略，默认值为child
              "attribute::"可以简写为"@"
        node-test：节点测试，有且仅有一个，它表示结果节点的类型或名称：
              当axis=attribute时为属性名称；
              当axis=namespace时为命名空间名称；
              当axis=其他时为元素名称或常规测试：text(),comment()
                        text()     为测试打印其文本节点值
                        comment()  为测试打印其注释节点值
        prediates：谓词，零到多个，它可以是用于限制结果的任意表达式
              Examples:
              @*            
                选择当前节点的所有属性
              para[1] 等效于 para[position()=1]       
                选择当前节点下的第1个节点名称为para的元素
              para[last()] 等效于 para[position()=last()]
                选择当前节点下的最后一个节点名称为para的元素
              /doc/chapter[5]/section[2] 
                选择第5个名称为chapter的节点下的第2个名称为section的节点
              para[@type="warning"] 
                选择当前节点下含有属性type的内容为warning的节点，并且节点名称为para
              para[@type="warning"][5] 
                选择当前节点下含有属性type的内容为warning的节点，并且是第5个名称为para的节点
              chapter[title] 
                选择当前节点下含有title节点且名称为chapter的所有的节点
              employee[@secretary and @assistant] 
                选择当前节点下含有secretary属性和assistant属性且名称为employee的所有节点
    @endhtmlonly
    
    <b>XPATH举例：</b>
    
    下面是举例用到的xml文档：
    =======================p.xml==========================
    \include p.xml
    =======================include.dtd====================
    \include include.dtd
    =======================i_gprs_cfg.xml=================
    \include i_gprs_cfg.xml
    ======================================================
    @htmlonly
    
    <b>说明如下：</b>
    
    /EXAMPLE/chapter/image[1]/attribute::*
    /EXAMPLE/chapter/image[1]/@*
        返回所有image[1]节点中属性:
            {"href","linus.gif"}
            {"width","230"}
            
    /EXAMPLE/chapter/image[1]/attribute::href
    /EXAMPLE/chapter/image[1]/@href
        只返回image[1]节点中属性名称为href的属性内容:
            {"href","linus.gif"}
            
    /child::EXAMPLE/child::chapter[1]/child::title[2]/child::text()
    /EXAMPLE/chapter[1]/title[2]/text()
        返回title[2]的文本节点:
        {"text","The Linux adventure2"}
        
    /child::EXAMPLE/child::chapter[1]/child::title
    /EXAMPLE/chapter[1]/title
        返回/EXAMPLE/chapter下所有名称为title的节点:
        {"title","The Linux adventure1"}
        {"title","The Linux adventure2"}
        
    /EXAMPLE/chapter/image[2]/child::comment()
    /EXAMPLE/chapter/image[2]/comment()
        返回/EXAMPLE/chapter/image[2]下所有的注释节点:
        {"comment","comment1 fffffffffffffffff"}
        {"comment","comment2 asadfdfdfdf"}
        
    /EXAMPLE/indb/dbname
        返回内容:
        {"dbname","gprs"}
        
    /EXAMPLE/indb/dbpassword
        返回内容:
        {"dbpassword","gprs123"}
    @endhtmlonly
    </code></PRE>

 <pre>
 EXAMPLE:
 @code
     string xpath,fname,entName,tmpStr;
     XmlApi xmlApi;
     multimap<string,string> resultSet;
     map<string,string> props;
        
     if(xmlApi.open(fname)==false)
     {
        cerr<<"ERROR:open file:"<<fname<<endl;
        exit(-1);
     }
     if(xmlApi.getEntityValue(entName,props)==false)
        cerr<<"ERROR: Entity "<<entName<<" does not exist!"<<endl;
     if(xmlApi.setXPath(xpath.c_str())==false)
     {
        cerr<<"ERROR:setXPath!"<<endl;
        xmlApi.close();
        exit(0);
     }
     if(xmlApi.getXPathResultSet(resultSet)==false)
        cerr<<"ERROR:can't get!"<<endl;
     xmlApi.close();
  @endcode
  </pre>    
 
*/
 
class XmlApi
{
public:
    ///构造函数
    XmlApi();
    ///析构函数
    virtual ~XmlApi();
    
public:        

    /**
    *打开并解析xml文档
    *@param fileName
    *@param blank 1:保留空格;0:去掉空格
    *@return true:打开成功;false:打开失败; 
    */
    bool open(const string& fileName,int blank=1);
    bool open(const char* fileName,int blank=1);
    /**
    *打开并解析缓冲中的xml文档
    *@param buffer xml格式的缓冲
    *@param size   缓冲的大小
    *@param blank 1:保留空格;0:去掉空格
    *@return true:打开成功;false:打开失败; 
    */
    bool openBuffer(const char* buffer,int size=-1,int blank=1);
    /**
    *关闭并释放xml文档
    */                     
    void close();
    /**
    *设置XPATH
    *@param xpath XPATH的内容
    *@return 设置成功返回true,否则返回false;
    */
    bool setXPath(const char* xpath);
    /**
    *返回当前XPATH所查询的结果集
    *@param resultSet XPATH所查询的结果集,结构为multimap<string,string><br>
    *                 第一个string为节点名称,属性名称,text,comment中的一种<br>
    *                 第二个string为节点名称对应的文本节点内容,属性名对应的属性内容,注释内容中的一种
    *@return 成功返回true,否则返回false;
    *
    */
    bool getXPathResultSet(multimap<string,string>& resultSet);
    /**
    *返回XPATH所查询的结果集
    *@param xpath XPATH的内容
    *@param resultSet XPATH所查询的结果集,结构为multimap<string,string><br>
    *                 第一个string为节点名称,属性名称,text,comment中的一种<br>
    *                 第二个string为节点名称对应的文本节点内容,属性名对应的属性内容,注释内容中的一种
    *@return 成功返回true,否则返回false;
    *
    */
    bool getXPathResultSet(const char* xpath,multimap<string,string>& resultSet);
    /**
    *读取指定的实体相关属性
    *@param entName 要查询的实体名
    *@param props   entName对应的实体属性,包括<br>
    *               ETYPE:  实体的类型<br>
    *               ID:     PUBLIC的外部标识<br>
    *               SYSTEM: PUBLIC,SYSTEM实体的url路径<br>
    *               ORIG:   实体引用没有被替换的内容<br>
    *               CONTENT:没有解析时的内容或数据
    *@return 成功返回true,否则返回false;
    *
    */
    bool getEntityValue(const string& entName,map<string,string>& props);
    
    /**
    *在当前XPATH下面插入一个子节点.
    *若该XPATH下有多个相同的节点名称,并且没有指定位置,那么将默认插入第一个位置的下面,并且子节点在所有子节点的最后面,否则就插入指定的位置下面
    *如：当前的XPATH是"/EXAMPLE/head"，insertNodeByXPath("subhead","hello");
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head><subhead>hello</subhead></head></EXAMPLE>
    *若该节点名称为空则插入一个文本节点(即nodeValue),若已经存在文本节点,则会追加到第一个文本节点上,然后返回true;
    *若该节点名称为"<![CDATA[]]>"则插入一个CDATA段的字符节点(即nodeValue),若已经存在CDATA段,则会追加到第一个文本节点上,然后返回true;
    *@param nodeName 节点名称.
    *@param nodeValue 节点的值
    *@param length    nodeValue的长度
    *@return 若该节点名称非空则插入一个节点(即nodeName),若已经存在该节点,则返回false,否则返回true;
    *
    */
    bool insertNodeByXPath(const char* nodeName,const char* nodeValue,int length=-1);
    /**
    *在指定的XPATH下面插入一个子节点.
    *若该XPATH下有多个相同的节点名称,并且没有指定位置,那么将默认插入第一个位置的下面,并且子节点在所有子节点的最后面,否则就插入指定的位置下面
    *如：insertNodeByXPath("/EXAMPLE/head","subhead","hello");
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head><subhead>hello</subhead></head></EXAMPLE>
    *若该节点名称为空则插入一个文本节点(即nodeValue),若已经存在文本节点,则会追加到第一个文本节点上,然后返回true;
    *若该节点名称为"<![CDATA[]]>"则插入一个CDATA段的字符节点(即nodeValue),若已经存在CDATA段,则会追加到第一个文本节点上,然后返回true;
    *@param xpath XPATH的内容
    *@param nodeName 节点名称
    *@param nodeValue 节点的值
    *@param length    nodeValue的长度
    *@return 若该节点名称非空则插入一个节点(即nodeName),若已经存在该节点,则返回false,否则返回true;
    *
    */
    bool insertNodeByXPath(const char* xpath,const char* nodeName,const char* nodeValue,int length=-1);
    
    /**
    *在当前XPATH下面插入一个节点的属性.
    *如：当前的XPATH是"/EXAMPLE/head",insertPropertyByXPath("subhead","hello");
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head subhead="hello"></head></EXAMPLE>
    *@param propName 属性名称
    *@param propValue 属性的值
    *@param length    propValue的长度
    *@return 若该属性名称已经存在,则返回false,否则处理成功返回true;
    *
    */
    bool insertPropertyByXPath(const char* propName,const char* propValue,int length=-1);
    /**
    *在指定的XPATH下面插入一个节点的属性.
    *如：insertPropertyByXPath("/EXAMPLE/head","subhead","hello");
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head subhead="hello"></head></EXAMPLE>
    *@param xpath XPATH的内容
    *@param propName 属性名称
    *@param propValue 属性的值
    *@param length    propValue的长度
    *@return 若该属性名称已经存在,则返回false,否则处理成功返回true;
    *
    */
    bool insertPropertyByXPath(const char* xpath,const char* propName,const char* propValue,int length=-1);
    
    /**
    *在当前XPATH下面更新一个节点.
    *若该XPATH下有多个相同的节点名称,并且没有指定位置,那么将默认更新第一个位置的节点,否则就插入指定的节点
    *如：当前的XPATH是"/EXAMPLE/head"，updateNodeByXPath("head","hello");
    *<?xml version="1.0" ><EXAMPLE><head>hhhhh</head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head>hello</head></EXAMPLE>
    *@param nodeName 节点名称.为空时保留原来的节点名称,否则使用nodeName覆盖旧名称
    *@param nodeValue 节点的值
    *@param length    nodeValue的长度
    *@return 更新节点成功返回true,否则返回false;
    *        若该节点名称不存在,则返回false;
    *
    */
    bool updateNodeByXPath(const char* nodeName,const char* nodeValue,int length=-1);
    /**
    *在指定的XPATH下面更新一个节点.
    *若该XPATH下有多个相同的节点名称,并且没有指定位置,那么将默认更新第一个位置的节点,否则就插入指定的节点
    *如：updateNodeByXPath("/EXAMPLE/head","head","hello");
    *<?xml version="1.0" ><EXAMPLE><head>hhhhh</head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head>hello</head></EXAMPLE>
    *@param xpath XPATH的内容.
    *@param nodeName 节点名称.为空时保留原来的节点名称,否则使用nodeName覆盖旧名称
    *@param nodeValue 节点的值.
    *@param length    nodeValue的长度
    *@return 更新节点成功返回true,否则返回false;
    *        若该节点名称不存在,则返回false;
    *
    */
    bool updateNodeByXPath(const char* xpath,const char* nodeName,const char* nodeValue,int length=-1);
    
    /**
    *在当前XPATH下面更新一个节点的属性.
    *如：当前的XPATH是"/EXAMPLE/head",updatePropertyByXPath("subhead","hello");
    *<?xml version="1.0" ><EXAMPLE><head subhead="hhhhh"></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head subhead="hello"></head></EXAMPLE>
    *@param propName 属性名称
    *@param propValue 属性的值
    *@param length    propValue的长度
    *@return 更新成功返回true;否则返回false;若该属性名称不存在,则返回false;
    *
    */
    bool updatePropertyByXPath(const char* propName,const char* propValue,int length=-1);
    /**
    *在指定的XPATH下面更新一个节点的属性.
    *updatePropertyByXPath("/EXAMPLE/head","subhead","hello");
    *<?xml version="1.0" ><EXAMPLE><head subhead="hhhhh"></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head subhead="hello"></head></EXAMPLE>
    *@param xpath XPATH的内容
    *@param propName 属性名称
    *@param propValue 属性的值
    *@param length    propValue的长度
    *@return 更新成功返回true;否则返回false;若该属性名称不存在,则返回false;
    *
    */
    bool updatePropertyByXPath(const char* xpath,const char* propName,const char* propValue,int length=-1);
    
    /**
    *在当前XPATH下面删除最后一个节点树.
    *若该XPATH下有多个相同的节点名称,并且没有指定位置,那么将默认删除第一个位置的节点树,否则就插入指定的节点树
    *如：当前的XPATH是"/EXAMPLE/head"，eraseNodeByXPath();
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE></EXAMPLE>
    *@return 删除成功返回true,否则返回false;若该节点树不存在,返回false!
    *
    */
    bool eraseNodeByXPath();
    /**
    *在指定的XPATH下面删除最后一个节点树.
    *若该XPATH下有多个相同的节点名称,并且没有指定位置,那么将默认删除第一个位置的节点树,否则就插入指定的节点树
    *如：eraseNodeByXPath("/EXAMPLE/head");
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE></EXAMPLE>
    *@param xpath XPATH的内容
    *@return 删除成功返回true,否则返回false;若该节点树不存在,返回false!
    *
    */
    bool eraseNodeByXPath(const char* xpath);
    
    /**
    *在当前XPATH下面删除一个节点的属性.
    *如：当前的XPATH是"/EXAMPLE/head"，erasePropertyByXPath("subhead");
    *<?xml version="1.0" ><EXAMPLE><head subhead="head"></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *@param propName 属性名称
    *@return 删除成功返回true,否则返回false;若该属性不存在,返回false!
    *
    */
    bool erasePropertyByXPath(const char* propName);
    /**
    *在指定的XPATH下面删除一个节点的属性.
    *如：erasePropertyByXPath("/EXAMPLE/head","subhead");
    *<?xml version="1.0" ><EXAMPLE><head subhead="head"></head></EXAMPLE>
    *结果就是:
    *<?xml version="1.0" ><EXAMPLE><head></head></EXAMPLE>
    *@param xpath XPATH的内容
    *@param propName 属性名称
    *@return 删除成功返回true,否则返回false;若该属性不存在,返回false!
    *
    */
    bool erasePropertyByXPath(const char* xpath,const char* propName);
    
    /**
    * saveXmlToFile:
    * @param fname 文件名或url,为空时将保存到原来的文件中.
    * @param encoding 使用的字符编码,默认为空
    * @param format 0:不进行格式输出;1:格式输出,默认为1
    *
    * Dump an XML document to a file or an URL.
    *
    * @return 成功返回true,否则false;
    *
    */
    bool saveXmlToFile(const char* fname="",const char* encoding="",int format=1);
    /**
    * saveXmlToBuffer:
    * @param buffStr 输出的缓冲.
    * @param encoding 使用的字符编码,默认为空
    * @param format 0:不进行格式输出;1:格式输出,默认为1
    *
    * Dump an XML document to a buffer.
    *
    * @return 成功返回true,否则false;
    *
    */
    bool saveXmlToBuffer(string& buffStr,const char* encoding="",int format=1);

    /**
    * 将xml文件导出到指定的文件流
    *@param f 文件指针,可以是stdin,stdout,stderr
    *@return 成功返回true,否则false;
    */
    bool dump(FILE* f=stdout);
    /**
    * 设置在读取数据时仅仅读取实体名字的标记.
    *@param b true:仅仅读取实体名字;false:读取实体扩展后的内容
    *@return 成功返回true,否则false;
    */
    //void setOnlyGetEntityNameFlag(bool b);
    
protected:
    class LibXml;
    LibXml* m_xml;
private:
    void ltoa(const long& l,string& str);
    string m_fileName;
    
};

#endif //XML_API_H
