#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <utility>
#include <fstream>
#include <iostream>

#include "xml/XmlApi.h"

extern void dumpTextVector(vector<string>& strVector);
extern void dumpMultiMap(multimap<string,string>& resultMultiMap);
extern void dumpMap(map<string,string>& resultMap);

int main(int argc, char **argv)
{	
    
    bool isPass=false;
    string xpath,fname,entName,tmpStr="",nodeName,nodeValue,propName,propValue;
    if(argc == 3)
    {
        xpath=argv[2];
        fname=argv[1];
        isPass=true;
    }
    else if(argc == 4)
    {
        fname=argv[1];
        tmpStr=argv[2];
        if(tmpStr == "--entity")
        {
            entName=argv[3];
            isPass=true;
        }
        else if(tmpStr == "--erase")
        {
            xpath=argv[3];
            isPass=true;
        }
    }
    else if(argc == 5)
    {
        fname=argv[1];
        tmpStr=argv[2];
        if(tmpStr == "--eraseprop")
        {
            xpath=argv[3];
            propName=argv[4];
            isPass=true;
        }
    }
    else if(argc == 6)
    {
        fname=argv[1];
        tmpStr=argv[2];
        if(tmpStr == "--insert"||tmpStr == "--update")
        {
            xpath=argv[3];
            nodeName=argv[4];
            nodeValue=argv[5];
            isPass=true;
        }
        else if(tmpStr == "--insertprop"||tmpStr == "--updateprop")
        {
            xpath=argv[3];
            propName=argv[4];
            propValue=argv[5];
            isPass=true;
        }
    }
    
    if(isPass == false)
    {
        cout<<"Usage:"<<endl;
        cout<<"      "<<argv[0]<<" filename {xpath} "<<endl;
        cout<<"      "<<argv[0]<<" filename --entity {entityname}"<<endl;
        cout<<"option:"<<endl;
        cout<<"    {xpath}                                    "<<"   "<<"根据XPATH查找xml文档中的内容"<<endl;
        cout<<"    --insert {xpath} {nodename} {nodevalue}    "<<"   "<<"根据XPATH在当前节点下面插入一个子节点或文本节点"<<endl;
        cout<<"    --insertprop {xpath} {propname} {propvalue}"<<"   "<<"根据XPATH插入当前节点的一个属性"<<endl;
        cout<<"    --update {xpath} {newname} {newvalue}      "<<"   "<<"根据XPATH更新当前的节点名称和节点内容"<<endl;
        cout<<"    --updateprop {xpath} {propname} {propvalue}"<<"   "<<"根据XPATH在当前的节点中更新指定属性名称的内容"<<endl;
        cout<<"    --erase {xpath}                            "<<"   "<<"根据XPATH删除当前的节点"<<endl;
        cout<<"    --eraseprop {xpath} {propname}             "<<"   "<<"根据XPATH在当前的节点中删除指定的属性"<<endl;
        cout<<"    --entity {entityname}                      "<<"   "<<"根据entityname查找对应实体的所有相关属性"<<endl;
        cout<<"such as:"<<endl;
        cout<<"         "<<argv[0]<<" p.xml \"/EXAMPLE/chapter/image[2]/attribute::*\""<<endl;
        cout<<"         "<<argv[0]<<" p.xml \"/EXAMPLE/chapter/image[1]/attribute::href\""<<endl;
        cout<<"         "<<argv[0]<<" p.xml \"/child::EXAMPLE/child::chapter/child::title[2]/child::text()\""<<endl;
        cout<<"         "<<argv[0]<<" p.xml \"/child::EXAMPLE/child::chapter/child::title\""<<endl;
        cout<<"         "<<argv[0]<<" p.xml \"/EXAMPLE/chapter/image[2]/child::comment()\""<<endl;
        cout<<"         "<<argv[0]<<" p.xml \"/EXAMPLE/A\""<<endl;
        cout<<"         "<<argv[0]<<" p.xml --entity indb_gprs_config"<<endl;
        cout<<"         "<<argv[0]<<" p.xml --insert \"/EXAMPLE/head\" newnode nodevalue"<<endl;
        cout<<"         "<<argv[0]<<" p.xml --insertprop \"/EXAMPLE/head\" newprop propvalue"<<endl;
        cout<<"         "<<argv[0]<<" p.xml --update \"/EXAMPLE/head\" nodename nodevalue"<<endl;
        cout<<"         "<<argv[0]<<" p.xml --updateprop \"/EXAMPLE/head\" propname propvalue"<<endl;
        cout<<"         "<<argv[0]<<" p.xml --erase \"/EXAMPLE/head\""<<endl;
        cout<<"         "<<argv[0]<<" p.xml --eraseprop \"/EXAMPLE/head\" propname "<<endl;
        return -1;
    }
    
    XmlApi xmlApi;
    multimap<string,string> resultSet;
    map<string,string> props;
    //string xmlBuffer="<?xml version=\"1.0\" encoding=\"GBK\" standalone=\"no\"?> <!DOCTYPE EXAMPLE SYSTEM \"./include.dtd\"> <EXAMPLE prop1=\"gnome is great\" prop2=\"&amp; linux too\"><head><title>Welcome to Gnome</title> </head><chapter id=\"3\" content=\"abc\"> <title>The Linux adventure1</title><title>The Linux adventure2</title><p>bla bla bla ...</p><image href=\"linus.gif\" width=\"230\"/> <image href=\"fad\"><!-- comment1 fffffffffffffffff --> adfccccc<!-- comment2 asadfdfdfdf --></image>     <indb><name>GPRS 入库</name><dbname>gprs</dbname><dbpassword>gprs123</dbpassword><servername>jfboss</servername><procname>indb_gprs_proc</procname></indb>     </chapter>   <indb><name>GPRS 入库</name><dbname>gprs</dbname><dbpassword>gprs123</dbpassword><servername>jfboss</servername><procname>indb_gprs_proc</procname></indb>   <chapter><title>Chapter 2</title><p>this is chapter 2 ...</p></chapter><chapter><title>Chapter 3</title><p>     /EXAMPLE/chapter/image[2]/attribute::*     id('chapter1')/p[1]    </p></chapter></EXAMPLE> ";
    //if(xmlApi.openBuffer(xmlBuffer.c_str(),xmlBuffer.size())==false)
    if(xmlApi.open(fname)==false)
    {
        cout<<"ERROR:open file:"<<fname<<endl;
        exit(-1);
    }
    
    
    if(tmpStr == "--entity")
    {
        //cout<<"{"<<entName<<","<<xmlApi.getEntityValue(entName.c_str())<<"}"<<endl;
        if(xmlApi.getEntityValue(entName,props)==false)
            cout<<"ERROR: Entity "<<entName<<" does not exist!"<<endl;
        dumpMap(props);
    }
    else if(tmpStr == "--insert")
    {
        if((xmlApi.insertNodeByXPath(xpath.c_str(),nodeName.c_str(),nodeValue.c_str()))==false)
            cout<<"ERROR:insertNodeByXPath!"<<endl;
        //if(xmlApi.saveXmlToFile("./q.xml")==false)
        //    cout<<"ERROR:saveXmlToFile!"<<endl;
        if(xmlApi.dump()==false)
            cout<<"ERROR:dump()!"<<endl;
    }
    else if(tmpStr == "--insertprop")
    {
        if((xmlApi.insertPropertyByXPath(xpath.c_str(),propName.c_str(),propValue.c_str()))==false)
            cout<<"ERROR:insertPropertyByXPath!"<<endl;
        //if(xmlApi.saveXmlToFile("./q.xml")==false)
        //    cout<<"ERROR:saveXmlToFile!"<<endl;
        if(xmlApi.dump()==false)
            cout<<"ERROR:dump()!"<<endl;
    }
    else if(tmpStr == "--update")
    {
        if((xmlApi.updateNodeByXPath(xpath.c_str(),nodeName.c_str(),nodeValue.c_str()))==false)
            cout<<"ERROR:updateNodeByXPath!"<<endl;
        //if(xmlApi.saveXmlToFile("./q.xml")==false)
        //    cout<<"ERROR:saveXmlToFile!"<<endl;
        if(xmlApi.dump()==false)
            cout<<"ERROR:dump()!"<<endl;
    }
    else if(tmpStr == "--updateprop")
    {
        if((xmlApi.updatePropertyByXPath(xpath.c_str(),propName.c_str(),propValue.c_str()))==false)
            cout<<"ERROR:updatePropertyByXPath!"<<endl;
        //if(xmlApi.saveXmlToFile("./q.xml")==false)
        //    cout<<"ERROR:saveXmlToFile!"<<endl;
        if(xmlApi.dump()==false)
            cout<<"ERROR:dump()!"<<endl;
    }
    else if(tmpStr == "--erase")
    {
        if((xmlApi.eraseNodeByXPath(xpath.c_str()))==false)
            cout<<"ERROR:eraseNodeByXPath!"<<endl;
        //if(xmlApi.saveXmlToFile("./q.xml")==false)
        //    cout<<"ERROR:saveXmlToFile!"<<endl;
        if(xmlApi.dump()==false)
            cout<<"ERROR:dump()!"<<endl;
    }
    else if(tmpStr == "--eraseprop")
    {
        if((xmlApi.erasePropertyByXPath(xpath.c_str(),propName.c_str()))==false)
            cout<<"ERROR:erasePropertyByXPath!"<<endl;
        //if(xmlApi.saveXmlToFile("./q.xml")==false)
        //    cout<<"ERROR:saveXmlToFile!"<<endl;
        if(xmlApi.dump()==false)
            cout<<"ERROR:dump()!"<<endl;
    }
    else
    {
        if(xmlApi.setXPath(xpath.c_str())==false)
        {
            cout<<"ERROR:setXPath!"<<endl;
            xmlApi.close();
            exit(0);
        }
        if(xmlApi.getXPathResultSet(resultSet))
            dumpMultiMap(resultSet);
    }    
    if(resultSet.size() > 0)
    resultSet.clear();   
    xmlApi.close();
    
    return 1;
}

void dumpTextVector(vector<string>& strVector)
{   
    cout<<"dumpStrVector("<<strVector.size()<<")"<<endl;
    for(int i=0;i<(int)strVector.size();i++)
    {
        cout<<strVector[i].size()<<" bytes: \""<<strVector[i]<<"\""<<endl;
    }
}
void dumpMultiMap(multimap<string,string>& resultMultiMap)
{
    //cout<<"dumpMultiMap("<<resultMultiMap.size()<<")"<<endl;
    multimap<string,string>::iterator iter;
    for(iter=resultMultiMap.begin();iter!=resultMultiMap.end();iter++)
    {
        cout<<"{\""<< iter->first <<"\",\""<< iter->second <<"\"}"<<endl;
    }
}
void dumpMap(map<string,string>& resultMap)
{
    //cout<<"dumpMultiMap("<<resultMultiMap.size()<<")"<<endl;
    map<string,string>::iterator iter;
    for(iter=resultMap.begin();iter!=resultMap.end();iter++)
    {
        cout<<"{\""<< iter->first <<"\",\""<< iter->second <<"\"}"<<endl;
    }
}
