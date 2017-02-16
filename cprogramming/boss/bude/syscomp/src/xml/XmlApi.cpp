/** 
  @file
    对libxml的api封装.

  @author Shandy
  @author Copyright &copy; Linkage. All Rights Reserved.
  @version 1.0
  @date 2004-01-15
  @sa 请参考: http://www.lianchuang.com
*/

#include "XmlApi.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include <libxml/xpath.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/debugXML.h>
#include <libxml/xmlmemory.h>
#include <libxml/parserInternals.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlerror.h>
#include <libxml/globals.h>
#ifdef __cplusplus
}
#endif
//#include "Debug.h"
//DB_LEVEL(XmlApi0,DEBUG0);

#ifndef NULL
#define NULL 0
#endif
class XmlApi::LibXml
{
public: 
    LibXml();
    ~LibXml();  
    /**
    *去除字符串两边的空格,回车换行,TAB
    *@param str 需要处理的字符串
    *@return 处理后的字符串
    */
    string allTrim(string &str);
    /**
    *将xmlChar* 转化成 string,并去除字符串两边的空格,回车换行,TAB
    *@param xmlchar xmlChar*类型的字符串
    *@param isClear 是否将xmlchar的缓冲清除
    *@return 转化后的字符串
    */
    string getStringByXmlCharPtr(xmlChar* xmlchar,int isClear=0);
    /**
    *dump一个节点到结果集
    *@param nodePtr 当前的节点指针
    *@param resultSet 返回的结果集,参见 XmlApi::getXPathResultSet(multimap<string,string>& resultSet) 方法的说明
    *@return 成功返回true,否则返回false;
    */
    bool   getDumpNode(xmlNodePtr nodePtr,multimap<string,string>&resultSet);
    /**
    *dump一个属性到结果集
    *@param attrPtr 当前的属性指针
    *@param resultSet 返回的结果集,参见XmlApi::getXPathResultSet(multimap<string,string>& resultSet)方法的说明
    *@return 成功返回true,否则返回false;
    */
    bool   getDumpAttribute(xmlAttrPtr attrPtr,multimap<string,string>&resultSet);
public:
    xmlDocPtr doc;
    xmlXPathContextPtr context;
    /*
       结构如下：
            typedef struct _xmlXPathObject xmlXPathObject;
            typedef xmlXPathObject *xmlXPathObjectPtr;
            struct _xmlXPathObject {
                xmlXPathObjectType type;
                xmlNodeSetPtr nodesetval;
                int boolval;
                double floatval;
                xmlChar *stringval;
                void *user;
                int index;
                void *user2;
                int index2;
            };
            typedef struct _xmlNodeSet xmlNodeSet;
            typedef xmlNodeSet *xmlNodeSetPtr;
            struct _xmlNodeSet {
                int nodeNr;                 // number of nodes in the set 
                int nodeMax;                // size of the array as allocated 
                xmlNodePtr *nodeTab;        // array of nodes in no particular order 
                // @@ with_ns to check wether namespace nodes should be looked at @@ 
            };
            typedef struct _xmlNode xmlNode;
            typedef xmlNode *xmlNodePtr;
            struct _xmlNode {
                void           *_private;   // application data 
                xmlElementType   type;      // type number, must be second ! 
                const xmlChar   *name;      // the name of the node, or the entity 
                struct _xmlNode *children;  // parent->childs link 
                struct _xmlNode *last;      // last child link 
                struct _xmlNode *parent;    // child->parent link 
                struct _xmlNode *next;      // next sibling link  
                struct _xmlNode *prev;      // previous sibling link  
                struct _xmlDoc  *doc;       // the containing document 
            
                // End of common part 
                xmlNs           *ns;        // pointer to the associated namespace 
                xmlChar         *content;   // the content 
                struct _xmlAttr *properties;// properties list 
                xmlNs           *nsDef;     // namespace definitions on this node 
            };
    */
    xmlXPathObjectPtr resource;
    bool isOnlyEntityName;
};
XmlApi::LibXml::LibXml()
{
    doc = 0;
    context = 0;
    resource = 0;
}
XmlApi::LibXml::~LibXml()
{
}
/**
*去除字符串两边的空格,回车换行,TAB
*@param str 需要处理的字符串
*@return 处理后的字符串
*/
string XmlApi::LibXml::allTrim(string& str)
{
    string sTmp=str;
    int i;

    if(sTmp.empty()==true)
        return sTmp;

    for(i=0;i<(int)sTmp.length();i++)
    {
        if(sTmp[i]==' ' || sTmp[i]== '\n' || sTmp[i]== '\t' || sTmp[i]== '\r')
            sTmp.erase(i,1);
        else
            break;
    }
    for(i=sTmp.length()-1;i>=0;i--)
    {
        if(sTmp[i]==' ' || sTmp[i]== '\n' || sTmp[i]== '\t' || sTmp[i]== '\r')
            sTmp.erase(i,1);
        else
            break;
    }

    return sTmp;
}
/**
*将xmlChar* 转化成 string,并去除字符串两边的空格,回车换行,TAB
*@param xmlchar xmlChar*类型的字符串
*@return 转化后的字符串
*/
string XmlApi::LibXml::getStringByXmlCharPtr(xmlChar* xmlchar,int isClear)
{
   if(xmlchar == (xmlChar*)NULL)
        return string("");
        
   xmlChar* xmlname;
   string str;
   xmlname = (xmlChar*)xmlStrdup(xmlchar);
   str=(char*)xmlname;
   str=allTrim(str);
   xmlFree(xmlname);
   if(isClear)
   {
        xmlFree(xmlchar);
        xmlchar=NULL;
   }
   return str;
}
/**
*dump一个节点到结果集
*@param nodePtr 当前的节点指针
*@param resultSet 返回的结果集,参见XmlApi::getXPathResultSet(multimap<string,string>& resultSet)方法的说明
*@return 成功返回true,否则返回false;
*/
bool   XmlApi::LibXml::getDumpNode(xmlNodePtr nodePtr,multimap<string,string>&resultSet)
{
    if (nodePtr == NULL) 
        return false;
        
    string name,value;
    switch(nodePtr->type)
    {
        case XML_ELEMENT_NODE:
            name=getStringByXmlCharPtr((xmlChar *)nodePtr->name);
            value=getStringByXmlCharPtr(xmlNodeListGetString(doc, nodePtr->xmlChildrenNode, 1),1);
            resultSet.insert(multimap<string,string>::value_type(name,value));
            break;
        case XML_TEXT_NODE:
    	case XML_CDATA_SECTION_NODE:
    	    name = getStringByXmlCharPtr((xmlChar *)nodePtr->name);
    	    value = "";
            if (nodePtr->content != NULL) 
            {
                if(isOnlyEntityName)
                    value = getStringByXmlCharPtr(nodePtr->content);
                else
                {
                    xmlChar *buffer;
                    buffer = xmlEncodeEntitiesReentrant(doc, nodePtr->content);
                    if (buffer != NULL) 
                    {
                        value=getStringByXmlCharPtr(buffer,1);
                    }
                }
		    }
            resultSet.insert(multimap<string,string>::value_type(name,value));
            break;
    	case XML_PI_NODE:
    	case XML_COMMENT_NODE:
            name = getStringByXmlCharPtr((xmlChar *)nodePtr->name);
            if (nodePtr->content != NULL) 
            {
                value = getStringByXmlCharPtr(nodePtr->content);
		    }
            resultSet.insert(multimap<string,string>::value_type(name,value));
            break;
        case XML_DOCUMENT_NODE:
        case XML_HTML_DOCUMENT_NODE:
            resultSet.insert(multimap<string,string>::value_type(string("document or html_document"),string("/")));
            break;
        case XML_ATTRIBUTE_NODE:
            return getDumpAttribute(reinterpret_cast<xmlAttrPtr>(nodePtr),resultSet);
        case XML_ENTITY_REF_NODE:
            name = getStringByXmlCharPtr((xmlChar *)nodePtr->name);
    	    value = "";
            if (nodePtr->content != NULL) 
            {
                if(isOnlyEntityName)
                {
                    //value = getStringByXmlCharPtr(nodePtr->content);
                    xmlEntityPtr ent = xmlGetDocEntity(doc, nodePtr->name);
                    if (ent != NULL) 
                    {
                        xmlChar *buffer;
    
                        /* an entity content can be any "well balanced chunk",
                         * i.e. the result of the content [43] production:
                         * http://www.w3.org/TR/REC-xml#NT-content.
                         * So it can contain text, CDATA section or nested
                         * entity reference nodes (among others).
                         * -> we recursive  call xmlNodeListGetString()
                         * which handles these types */
                        buffer = xmlNodeListGetString(doc, ent->children, 1);
                        if (buffer != NULL) 
                        {
                            value = getStringByXmlCharPtr(buffer,1);
                        }
                    } 
                    else 
                    {
                        value = getStringByXmlCharPtr(nodePtr->content);
                    }
                }
                else
                {
                    value="&"+getStringByXmlCharPtr((xmlChar*)nodePtr->name)+";";
                }
		    }
            resultSet.insert(multimap<string,string>::value_type(name,value));
            break;
        default:
            ;
    }
    return true;
}
/**
*dump一个属性到结果集
*@param attrPtr 当前的属性指针
*@param resultSet 返回的结果集,参见XmlApi::getXPathResultSet(multimap<string,string>& resultSet)方法的说明
*@return 成功返回true,否则返回false;
*/
bool   XmlApi::LibXml::getDumpAttribute(xmlAttrPtr attrPtr,multimap<string,string>&resultSet)
{
    if(attrPtr == NULL)
        return false;
    string name,value;
    xmlAttrPtr tmpAttrPtr=attrPtr;
    
    if(tmpAttrPtr != NULL)
    {
        name=getStringByXmlCharPtr((xmlChar *)tmpAttrPtr->name);
        if (tmpAttrPtr->children == NULL) 
            value="";
        else
            value=getStringByXmlCharPtr(xmlNodeListGetString(doc, tmpAttrPtr->children, 1),1);
        resultSet.insert(multimap<string,string>::value_type(name,value));
    }
    return true;
}
///构造函数
XmlApi::XmlApi()
{    
    m_xml = new LibXml;
    m_fileName = "";
}
///析构函数
XmlApi::~XmlApi()
{
    close();
    if(m_xml != 0)
    {
        delete m_xml;
        m_xml = 0;
    }
}
/**
*打开并解析xml文档
*@param fileName
*@return true:打开成功;false:打开失败; 
*/
bool XmlApi::open(const string& fileName,int blank)
{
    return open(fileName.c_str(),blank);
}
bool XmlApi::open(const char* fileName,int blank)
{
/*
int xmlAddEncodingAlias(const char *name, const char *alias); 
int xmlDelEncodingAlias(const char *alias); 
const char * xmlGetEncodingAlias(const char *alias); 
void xmlCleanupEncodingAliases(void); 
*/
    if(m_xml->doc != NULL) 
        close();
    xmlLoadExtDtdDefaultValue |= XML_DETECT_IDS;
    xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefaultValue = 1;
    xmlKeepBlanksDefault(blank);
    if(xmlAddEncodingAlias("ISO-8859-1","GBK")<0||
       xmlAddEncodingAlias("ISO-8859-1","GB2312")<0)
        return false;
    m_xml->doc = xmlParseFile(fileName);
    if(m_xml->doc == NULL)
        return false;
    
    m_xml->context = xmlXPathNewContext(m_xml->doc);
    m_xml->context->node = xmlDocGetRootElement(m_xml->doc);
    if(m_xml->context->node == NULL)
    {
        xmlFreeDoc(m_xml->doc);
        return false;
    }
    m_xml->resource=NULL;
    m_fileName=fileName;
    m_xml->isOnlyEntityName=false;
    
    return true;
}
bool XmlApi::openBuffer(const char* buffer,int size,int blank)
{
    int len=strlen(buffer);
    if(size==0||len < size)
        return false;
    if(size == -1)
    {
        size=len;
    }
    ///////////////////////////////////////////////////////  
    if(m_xml->doc != NULL) 
        close();
    xmlLoadExtDtdDefaultValue |= XML_DETECT_IDS;
    xmlLoadExtDtdDefaultValue |= XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefaultValue = 1;
    if(xmlAddEncodingAlias("ISO-8859-2","GBK")<0||
       xmlAddEncodingAlias("ISO-8859-2","GB2312")<0)
        return false;
    xmlKeepBlanksDefault(blank);
    m_xml->doc = xmlParseMemory(buffer,size);
    if(m_xml->doc == NULL)
        return false;
    
    m_xml->context = xmlXPathNewContext(m_xml->doc);
    m_xml->context->node = xmlDocGetRootElement(m_xml->doc);
    if(m_xml->context->node == NULL)
    {
        xmlFreeDoc(m_xml->doc);
        return false;
    }
    m_xml->resource=NULL;
    m_fileName="";
    m_xml->isOnlyEntityName=false;
    
    ///////////////////////////////////////////////////////  
    return true;
}
/**
*关闭并释放xml文档
*/ 
void XmlApi::close()
{
    if(m_xml->resource != NULL)
    {
        xmlXPathFreeObject(m_xml->resource);
        m_xml->resource=NULL;
    }
    if(m_xml->context != NULL)
    {
        xmlXPathFreeContext(m_xml->context);
        m_xml->context = NULL;
    }
    xmlCleanupEncodingAliases();
    if(m_xml->doc != NULL)
        xmlFreeDoc(m_xml->doc);
    xmlCleanupParser();
    m_xml->doc = 0;
}

/**
*设置XPATH
*@param xpath XPATH的内容
*@return 设置成功返回true,否则返回false;
*/
bool XmlApi::setXPath(const char* xpath)
{
    xmlXPathCompExprPtr comp;

    comp = xmlXPathCompile(BAD_CAST xpath);
    if (comp == NULL) 
        return false;
    
    if(m_xml->resource != NULL)
    {
        xmlXPathFreeObject(m_xml->resource);
        m_xml->resource=NULL;
    }
    
    //XPATH解析树输出
    //xmlXPathDebugDumpCompExpr(stdout, comp, 0);
    m_xml->resource = xmlXPathCompiledEval(comp, m_xml->context);
        
    //XPATH返回结果输出
    //xmlXPathDebugDumpObject(stdout, m_xml->resource, 0);
    xmlXPathFreeCompExpr(comp);
    if(m_xml->resource == NULL)
        return false;
    
    //该XPATH下无节点数则返回失败
    if(m_xml->resource->nodesetval == NULL || m_xml->resource->nodesetval->nodeNr == 0)
        return false;
    
    return true;
}
/**
*返回XPATH所查询的结果集
*@param xpath XPATH的内容
*@param resultSet XPATH所查询的结果集,结构为multimap<string,string>
*                 第一个string为节点名称,属性名称,text,comment中的一种
*                 第二个string为节点名称对应的文本节点内容,属性名对应的属性内容,注释内容中的一种
*@return 成功返回true,否则返回false;
*/
bool XmlApi::getXPathResultSet(const char* xpath,multimap<string,string>& resultSet)
{
    if(setXPath(xpath)==false)
        return false;
    return getXPathResultSet(resultSet);
}
/**
*返回XPATH所查询的结果集
*@param resultSet XPATH所查询的结果集,结构为multimap<string,string>
*                 第一个string为节点名称,属性名称,text,comment中的一种
*                 第二个string为节点名称对应的文本节点内容,属性名对应的属性内容,注释内容中的一种
*@return 成功返回true,否则返回false;
*/
bool XmlApi::getXPathResultSet(multimap<string,string>& resultSet)
{
    
    resultSet.clear();
    //added by shandy 20041208
    if(m_xml->resource == NULL)
        return true;
        
    vector<string> strVector;
    string str,name;
    int i;
    xmlNodeSetPtr tmpPtr=0;
    
    //cout<<"resource->type:"<<m_xml->resource->type<<endl;
    switch(m_xml->resource->type) 
    {
        case XPATH_UNDEFINED://Object is uninitialized
            return false;
            break;
        case XPATH_NODESET://Object is a Node Set
            tmpPtr=m_xml->resource->nodesetval;
            if(tmpPtr == NULL)
                break;
            for(i = 0;i < tmpPtr->nodeNr;i++)
                m_xml->getDumpNode(tmpPtr->nodeTab[i],resultSet);
            break;
        case XPATH_XSLT_TREE://Object is an XSLT value tree
            return false;
            break;
        case XPATH_BOOLEAN://Object is a Boolean
            str=m_xml->getStringByXmlCharPtr(xmlXPathCastBooleanToString(m_xml->resource->boolval),1);
            resultSet.insert(multimap<string,string>::value_type(string("boolean"),str));
            break;
        case XPATH_NUMBER://Object is a number
            str=m_xml->getStringByXmlCharPtr(xmlXPathCastNumberToString(m_xml->resource->floatval),1);
            resultSet.insert(multimap<string,string>::value_type(string("number"),str));
            break;
        case XPATH_STRING://Object is a string
            if(m_xml->resource->stringval != NULL)
            {
                string str=m_xml->getStringByXmlCharPtr(m_xml->resource->stringval);
                resultSet.insert(multimap<string,string>::value_type(string("string"),str));
            }
            break;
        case XPATH_POINT://Object is a point
            //fprintf(output, " : index %d in node", m_xml->resource->index);
            m_xml->getDumpNode((xmlNodePtr) m_xml->resource->user,resultSet);
            break;
        case XPATH_RANGE:
            if ((m_xml->resource->user2 == NULL) ||
                ((m_xml->resource->user2 == m_xml->resource->user) && (m_xml->resource->index == m_xml->resource->index2))) 
            {
                //Object is a collapsed range 
                //if (m_xml->resource->index >= 0)
                //    fprintf(output, "index %d in ", m_xml->resource->index);
                //fprintf(output, "node\n");
                m_xml->getDumpNode((xmlNodePtr) m_xml->resource->user,resultSet);
            } 
            else  
            {
                //Object is a range from 
                //if (m_xml->resource->index >= 0)
                //    fprintf(output, "index %d in ", m_xml->resource->index);
                m_xml->getDumpNode((xmlNodePtr) m_xml->resource->user,resultSet);
                // to 
                //if (m_xml->resource->index2 >= 0)
                //    fprintf(output, "index %d in ", m_xml->resource->index2);
                m_xml->getDumpNode((xmlNodePtr) m_xml->resource->user2,resultSet);
            }
            break;
        case XPATH_LOCATIONSET://Object is a Location Set
            return false;
            break;
        case XPATH_USERS://Object is user defined
            return false;
            break;
    }
    return true;
            
}

void XmlApi::ltoa(const long& l,string& str)
{
    char buff[10];
    memset(buff,0,sizeof(buff));
    sprintf(buff,"%d\0",l);
    str=buff;
    return ;
}
/**
*读取指定的实体相关属性
*@param entName 要查询的实体名
*@param props   entName对应的实体属性,包括
*               ETYPE:  实体的类型
*               ID:     PUBLIC的外部标识
*               SYSTEM: PUBLIC,SYSTEM实体的url路径
*               ORIG:   实体引用没有被替换的内容
*               CONTENT:没有解析时的内容或数据
*@return 成功返回true,否则返回false;
*/
bool XmlApi::getEntityValue(const string& entName,map<string,string>& props)
{
    xmlEntityPtr ent;
    if(m_xml->doc == 0)
        return false;
    
    string str;
    
    //debug output
    //xmlDebugDumpEntities(stdout,m_xml->doc);
    //end debug
    ent = xmlGetDocEntity(m_xml->doc, (const xmlChar*)entName.c_str());
    if (ent != NULL) 
    {
        props.clear();
        #ifndef _WIN32
        this->ltoa((long)ent->etype,str);
	#else
	char buff[100];
        str=ltoa((long)ent->etype,buff,100);
        #endif
	    props.insert(map<string,string>::value_type(string("ETYPE"),str));
        
        str=m_xml->getStringByXmlCharPtr((xmlChar*)ent->ExternalID); 
	props.insert(map<string,string>::value_type(string("ID"),str));
        
        str=m_xml->getStringByXmlCharPtr((xmlChar*)ent->SystemID); 
	props.insert(map<string,string>::value_type(string("SYSTEM"),str));
        
        str=m_xml->getStringByXmlCharPtr((xmlChar*)ent->orig); 
	props.insert(map<string,string>::value_type(string("ORIG"),str));
        
        str=m_xml->getStringByXmlCharPtr((xmlChar*)ent->content); 
	props.insert(map<string,string>::value_type(string("CONTENT"),str));     
    }
    else 
        return false;
    return true;
}


bool XmlApi::insertNodeByXPath(const char* xpath,const char* nodeName,const char* nodeValue,int length)
{
    if(setXPath(xpath)==false)
        return false;
    return insertNodeByXPath(nodeName,nodeValue,length);
}
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
bool XmlApi::insertNodeByXPath(const char* nodeName,const char* nodeValue,int length)
{
    if(m_xml->resource == NULL)
        return false;
    int len=strlen(nodeValue);
    if(length==0||len < length)
        return false;
    if(length == -1)
    {
        length=len;
    }
    //cout<<"resource->type:"<<m_xml->resource->type<<endl;
    switch(m_xml->resource->type) 
    {
        case XPATH_NODESET://Object is a Node Set
            if(m_xml->resource->nodesetval == NULL)
                return false;
            xmlNodePtr cur,newNode;
            //取得第一个节点
            if((cur=(*(m_xml->resource->nodesetval->nodeTab)))==NULL)
                return false;
            if(strlen(nodeName)==0)
            {
                if(cur->children==NULL)
                {
                    cur->children = xmlNewText((const xmlChar*)nodeValue);
                    cur->last=cur->children;
                    if(cur->last==NULL)
                        return false;
                }
                else
                    if(xmlTextConcat(cur->children,(const xmlChar *)nodeValue,length)<0)
                        return false;
            }
            else 
            {
                if(strcmp(nodeName,XML_CDATA) == 0)
                {
                    if((newNode=xmlNewCDataBlock(m_xml->doc,(const xmlChar*)nodeValue,length)) == NULL)
                        return false;
                }
                else
                {
                    xmlNsPtr* ns=xmlGetNsList(m_xml->doc,cur);
                    if((newNode=xmlNewNode((ns == NULL?NULL:(*ns)),(const xmlChar *)nodeName)) == NULL)
                        return false;
                    xmlNodeSetContent(newNode,(const xmlChar*)nodeValue);
                    
                }
                //从最后的字节点位置进行追加
                if((xmlAddChild(cur,newNode))==NULL)
                    return false;
            }
            break;
        case XPATH_XSLT_TREE://Object is an XSLT value tree
        case XPATH_BOOLEAN://Object is a Boolean
        case XPATH_NUMBER://Object is a number
        case XPATH_STRING://Object is a string
        case XPATH_POINT://Object is a point
        case XPATH_RANGE://是一个范围
        case XPATH_LOCATIONSET://Object is a Location Set
        case XPATH_USERS://Object is user defined
        case XPATH_UNDEFINED://Object is uninitialized
            return false;
            break;
    }
    return true;
}

bool XmlApi::insertPropertyByXPath(const char* xpath,const char* propName,const char* propValue,int length)
{
    if(setXPath(xpath)==false)
        return false;
    return insertPropertyByXPath(propName,propValue,length);
}
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
bool XmlApi::insertPropertyByXPath(const char* propName,const char* propValue,int length)
{
    if(m_xml->resource == NULL)
        return false;
    
    //cout<<"resource->type:"<<m_xml->resource->type<<endl;
    switch(m_xml->resource->type) 
    {
        case XPATH_NODESET://Object is a Node Set
            if(m_xml->resource->nodesetval == NULL)
                return false;
            xmlNodePtr cur;
            //取得第一个节点
            if((cur=(*(m_xml->resource->nodesetval->nodeTab)))==NULL)
                return false;
            if(strlen(propName)==0)//属性名称不能为空
                return false;
            //已经存在的，返回失败
            if(xmlHasProp(cur,(const xmlChar*)propName)!=NULL)
                return false;
            if(xmlSetProp(cur,(const xmlChar *)propName,(const xmlChar *)propValue)==NULL)
                return false;
            break;
        case XPATH_XSLT_TREE://Object is an XSLT value tree
        case XPATH_BOOLEAN://Object is a Boolean
        case XPATH_NUMBER://Object is a number
        case XPATH_STRING://Object is a string
        case XPATH_POINT://Object is a point
        case XPATH_RANGE://是一个范围
        case XPATH_LOCATIONSET://Object is a Location Set
        case XPATH_USERS://Object is user defined
        case XPATH_UNDEFINED://Object is uninitialized
            return false;
            break;
    }
    return true;
}


bool XmlApi::updateNodeByXPath(const char* xpath,const char* nodeName,const char* nodeValue,int length)
{
    if(setXPath(xpath)==false)
        return false;
    return updateNodeByXPath(nodeName,nodeValue,length);
}
/**
*在当前XPATH下面更新一个节点.
*若该XPATH下有多个相同的节点名称,并且没有指定位置,那么将默认更新第一个位置的节点,否则就插入指定的节点
*如：当前的XPATH是"/EXAMPLE/head"，updateNodeByXPath("head","hello");
*<?xml version="1.0" ><EXAMPLE><head>hhhhh</head></EXAMPLE>
*结果就是:
*<?xml version="1.0" ><EXAMPLE><head>hello</head></EXAMPLE>
*@param nodeName 节点名称.为空时保留原来的节点名称,否则覆盖
*@param nodeValue 节点的值
*@param length    nodeValue的长度
*@return 更新一个节点(即nodeName)成功返回true,否则返回false;
*        若该节点名称不存在,则返回false;
*
*/
bool XmlApi::updateNodeByXPath(const char* nodeName,const char* nodeValue,int length)
{
    if(m_xml->resource == NULL)
        return false;
    
    //cout<<"resource->type:"<<m_xml->resource->type<<endl;
    switch(m_xml->resource->type) 
    {
        case XPATH_NODESET://Object is a Node Set
            if(m_xml->resource->nodesetval == NULL)
                return false;
            xmlNodePtr cur;
            //取得第一个节点
            if((cur=(*(m_xml->resource->nodesetval->nodeTab)))==NULL)
                return false;
            //更新节点的内容
            //第一个节点为NULL,或非文本节点,则添加一个文本节点
            if(cur->type==XML_DOCUMENT_FRAG_NODE||
               cur->type==XML_ELEMENT_NODE)
            {
                //更新节点名称
                if(strlen(nodeName)>0)
                {
                    xmlNodeSetName(cur,(const xmlChar*)nodeName);
                }
                if(cur->children==NULL)//为空时插入一个节点
                {
                    insertNodeByXPath("",nodeValue);
                }
                else
                {
                    xmlFree(cur->children->content);
                    cur->children->content=xmlStrdup((const xmlChar*)nodeValue);
                }
            }
            //第一个节点为文本节点,则覆盖内容
            else if(cur->type==XML_TEXT_NODE||
                    cur->type==XML_CDATA_SECTION_NODE||
                    cur->type==XML_ENTITY_REF_NODE||
                    cur->type==XML_ENTITY_NODE||
                    cur->type==XML_PI_NODE||
                    cur->type==XML_COMMENT_NODE||
                    cur->type==XML_NOTATION_NODE)
            {
                xmlFree(cur->content);
                cur->content=xmlStrdup((const xmlChar*)nodeValue);
            }
            else
                return false;
            break;
        case XPATH_XSLT_TREE://Object is an XSLT value tree
        case XPATH_BOOLEAN://Object is a Boolean
        case XPATH_NUMBER://Object is a number
        case XPATH_STRING://Object is a string
        case XPATH_POINT://Object is a point
        case XPATH_RANGE://是一个范围
        case XPATH_LOCATIONSET://Object is a Location Set
        case XPATH_USERS://Object is user defined
        case XPATH_UNDEFINED://Object is uninitialized
            return false;
            break;
    }
    return true;
}


bool XmlApi::updatePropertyByXPath(const char* xpath,const char* propName,const char* propValue,int length)
{
    if(setXPath(xpath)==false)
        return false;
    return updatePropertyByXPath(propName,propValue,length);
}
bool XmlApi::updatePropertyByXPath(const char* propName,const char* propValue,int length)
{
    if(m_xml->resource == NULL)
        return false;
    if(strlen(propName)==0)
        return false;
    //cout<<"resource->type:"<<m_xml->resource->type<<endl;
    switch(m_xml->resource->type) 
    {
        case XPATH_NODESET://Object is a Node Set
            if(m_xml->resource->nodesetval == NULL)
                return false;
            xmlNodePtr cur;
            //取得第一个节点
            if((cur=(*(m_xml->resource->nodesetval->nodeTab)))==NULL)
                return false;
            //更新节点的属性内容
            if(xmlHasProp(cur,(const xmlChar*)propName)==NULL)
                return false;
            if(xmlSetProp(cur,(const xmlChar*)propName,(const xmlChar*)propValue)==NULL)
                return false;
            break;
        case XPATH_XSLT_TREE://Object is an XSLT value tree
        case XPATH_BOOLEAN://Object is a Boolean
        case XPATH_NUMBER://Object is a number
        case XPATH_STRING://Object is a string
        case XPATH_POINT://Object is a point
        case XPATH_RANGE://是一个范围
        case XPATH_LOCATIONSET://Object is a Location Set
        case XPATH_USERS://Object is user defined
        case XPATH_UNDEFINED://Object is uninitialized
            return false;
            break;
    }
    return true;
}


bool XmlApi::eraseNodeByXPath(const char* xpath)
{
    if(setXPath(xpath)==false)
        return false;
    return eraseNodeByXPath();
}
bool XmlApi::eraseNodeByXPath()
{
    if(m_xml->resource == NULL)
        return false;
    
    //cout<<"resource->type:"<<m_xml->resource->type<<endl;
    switch(m_xml->resource->type) 
    {
        case XPATH_NODESET://Object is a Node Set
            if(m_xml->resource->nodesetval == NULL)
                return false;
            xmlNodePtr cur;
            //取得第一个节点
            if((cur=(*(m_xml->resource->nodesetval->nodeTab)))==NULL)
                return true;
            xmlUnlinkNode(cur);
            xmlFreeNode(cur);
            cur=NULL;
            break;
        case XPATH_XSLT_TREE://Object is an XSLT value tree
        case XPATH_BOOLEAN://Object is a Boolean
        case XPATH_NUMBER://Object is a number
        case XPATH_STRING://Object is a string
        case XPATH_POINT://Object is a point
        case XPATH_RANGE://是一个范围
        case XPATH_LOCATIONSET://Object is a Location Set
        case XPATH_USERS://Object is user defined
        case XPATH_UNDEFINED://Object is uninitialized
            return false;
            break;
    }
    return true;
}


bool XmlApi::erasePropertyByXPath(const char* xpath,const char* propName)
{
    if(setXPath(xpath)==false)
        return false;
    return erasePropertyByXPath(propName);
}
bool XmlApi::erasePropertyByXPath(const char* propName)
{
    if(m_xml->resource == NULL)
        return false;
    if(strlen(propName)==0)
        return true;
    //cout<<"resource->type:"<<m_xml->resource->type<<endl;
    switch(m_xml->resource->type) 
    {
        case XPATH_NODESET://Object is a Node Set
            if(m_xml->resource->nodesetval == NULL)
                return false;
            xmlNodePtr cur;
            //取得第一个节点
            if((cur=(*(m_xml->resource->nodesetval->nodeTab)))=BOOLEAN://Object is a Boolean
        case XPATH_NUMBER://Object is a number
        case XPATH_STRING://Object is a string
        case XPATH_POINT://Object is a point
        case XPATH_RANGE://鏄