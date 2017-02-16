#include "mwci/localservice.h"
#include "mwci/CFmlBuf.h"
#include "mwci/md5.h"
XERCES_CPP_NAMESPACE_USE
using namespace localservice;
using namespace std;

bool Service::writeFmlBuffer(CFmlBuf *infmlbuf,CFmlBuf *outfmlbuf)
{
#ifndef __UNIX
  if (infmlbuf == NULL || outfmlbuf == NULL) return false;

  FLDID iFieldId = FIRSTFLDID;
  FLDOCC occ = 0;
  string strFieldName;

  string strFieldValue;
  string strRefName="";



  while (infmlbuf->GetNext(&iFieldId, &occ, (char *)NULL, (FLDLEN *)NULL) > 0)
  {
    //取得FML字段名称
    char *name = infmlbuf->GetFldName(iFieldId);
    char szFieldId[40]="\0";
    sprintf(szFieldId,"%d",iFieldId);

    if (name == NULL)
    {
      strFieldName = szFieldId;
    }
    else
    {
      strFieldName = name;
    }
    localservice::Field fld;
    if (! strcmp(name,strRefName.c_str())  == 0) //如果是一个新的域
          fld =  this->addInputField(strFieldName.c_str(),szFieldId);



    //取得FML字段类型
    int iFieldType = infmlbuf->GetFldType(iFieldId);
    //取得FML字段值
    switch(iFieldType)
    {
      case  FLD_CHAR	:
        {

          char charValue =  infmlbuf->GetChar(iFieldId,occ);

          strFieldValue[0] = charValue; //字符串保存其字节编码值
        };
        break;
      case  FLD_SHORT	:
        {
          short shortValue  = infmlbuf->GetInt(iFieldId,occ);
          char szTmp[40] = "\0";
          sprintf(szTmp,"%d",shortValue);
          strFieldValue = szTmp;
        };
        break;
      case  FLD_LONG	:
        {
          long longValue  = infmlbuf->GetInt(iFieldId,occ);
          char szTmp[40] = "\0";
          sprintf(szTmp,"%d",longValue);
          strFieldValue = szTmp;
        };
        break;
      case  FLD_FLOAT	:
        {
          float floatValue  = infmlbuf->GetFloat(iFieldId,occ);
             char szTmp[40] = "\0";
          sprintf(szTmp,"%.2f",floatValue);
          strFieldValue = szTmp;
        };
        break;
      case  FLD_DOUBLE:
        {
          double doubleValue  = infmlbuf->GetFloat(iFieldId,occ);
             char szTmp[40] = "\0";
          sprintf(szTmp,"%.2f",doubleValue);
          strFieldValue = szTmp;
        };
        break;
      case  FLD_STRING:
        {
          char* strValue = infmlbuf->GetStr(iFieldId,occ);

          if (strValue != NULL) strFieldValue =  strValue;   //字符串变量的值两边加上双引号
          else return false;
        };
        break;
      case FLD_CARRAY:
        {
        }
        break;
      #ifdef FML32
      case FLD_PTR :
        {
        }
        break;
      case FLD_FML32:
        {
        }
        break;
      case FLD_VIEW32:
        {
        }
        break;
      #endif
    };

    this->addItem(fld,strFieldValue.c_str());
    strRefName = name;
  }

  //outbuf
  iFieldId = FIRSTFLDID;
  occ = 0;
  strFieldName = "";
  strFieldValue = "";
  strRefName = "";
  while (outfmlbuf->GetNext(&iFieldId, &occ, (char *)NULL, (FLDLEN *)NULL) > 0)
  {
    //取得FML字段名称
    char *name = outfmlbuf->GetFldName(iFieldId);
    char szFieldId[40] = "\0";
    sprintf(szFieldId,"%d",iFieldId);

    if (name == NULL)
    {
      strFieldName =szFieldId;
    }
    else
    {
      strFieldName = name;
    }
    localservice::Field fld;
    if (! strcmp(name,strRefName.c_str())  == 0) //如果是一个新的域
           fld =  this->addOutputField(strFieldName.c_str(),szFieldId);


    //取得FML字段类型
    int iFieldType = outfmlbuf->GetFldType(iFieldId);
    //取得FML字段值
    switch(iFieldType)
    {
      case  FLD_CHAR	:
        {

          char charValue =  outfmlbuf->GetChar(iFieldId,occ);

          strFieldValue[0] = charValue; //字符串保存其字节编码值
        };
        break;
      case  FLD_SHORT	:
        {
          short shortValue  = outfmlbuf->GetInt(iFieldId,occ);
              char szTmp[40] = "\0";
          sprintf(szTmp,"%d",shortValue);
          strFieldValue = shortValue;
        };
        break;
      case  FLD_LONG	:
        {
          long longValue  = outfmlbuf->GetInt(iFieldId,occ);
           char szTmp[40] = "\0";
          sprintf(szTmp,"%d",longValue);
          strFieldValue = szTmp;
        };
        break;
      case  FLD_FLOAT	:
        {
          float floatValue  = outfmlbuf->GetFloat(iFieldId,occ);
           char szTmp[40] = "\0";
          sprintf(szTmp,"%.2f",floatValue);
          strFieldValue = floatValue;
        };
        break;
      case  FLD_DOUBLE:
        {
          double doubleValue  = outfmlbuf->GetFloat(iFieldId,occ);
              char szTmp[40] = "\0";
          sprintf(szTmp,"%.2f",doubleValue);
          strFieldValue = szTmp;
        };
        break;
      case  FLD_STRING:
        {
          char* strValue = outfmlbuf->GetStr(iFieldId,occ);

          if (strValue != NULL) strFieldValue = strValue;   //字符串变量的值两边加上双引号
          else return false;
        };
        break;
      case FLD_CARRAY:
        {
        }
        break;
      #ifdef FML32
      case FLD_PTR :
        {
        }
        break;
      case FLD_FML32:
        {
        }
        break;
      case FLD_VIEW32:
        {
        }
        break;
      #endif
    };

    this->addItem(fld,strFieldValue.c_str());
    strRefName = name;
  }
  return true;
 #else
    return false;
 #endif

}


bool Service::saveToLocal(CFmlBuf* inbuf, CFmlBuf* outbuf)
{
#ifndef __UNIX
  if (inbuf == NULL || outbuf == NULL) return false;
  if ( inbuf->IsEmpty() || outbuf->IsEmpty()) return false ;

  //取得系统路径
  char szFullPath[256]="\0";
  GetModuleFileName(NULL,szFullPath,256);
  string strTransCode = szFullPath;

  int j =  strTransCode.find_last_of("\\",strTransCode.length() - 1);

  strTransCode =  strTransCode.substr(0,j);

  strTransCode += "\\servicelocal\\";



   WIN32_FIND_DATA fd;
   HANDLE hFind = FindFirstFile(strTransCode.c_str(), &fd);
   if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
   {
     ;  //存在;
   }
   else
   {    //创建目录
        CreateDirectoryA(strTransCode.c_str(),0);
   }
    FindClose(hFind);







  string sCode;

  try
  {
    sCode = inbuf->GetString("TRANSCODE");
  }
  catch(...)
  {
    return false;
  }


  if (sCode.length() == 0) return false;

   strTransCode +=  sCode + ".txd";

  long lResultCode = 0;
  try
  {
      FLDID iResultCodeFieldId =  outbuf->GetFldID("RESULTCODE");
      lResultCode = outbuf->GetInt(iResultCodeFieldId,0);
  }
  catch(...)
  {
    return false ;
  }

  if (lResultCode != 0) return false;



  FLDOCC iInputFieldCount = inbuf->CountAll();
  FLDOCC iOutputFieldCount = outbuf->CountAll();

  this->RebuildService();

  if ( (this->setInputCount(iInputFieldCount)) &&
       (this->setOutputCount(iOutputFieldCount)) )
  {
      if (writeFmlBuffer(inbuf, outbuf))
      {
            if (this->SaveToXml(strTransCode.c_str()))   //如果本地化成功进行 md5 签名   songzj
            {
                #ifdef MD5DIGEST
                if (!MD5Digest(strTransCode.c_str())) //签名成功
                {

                    return false;
                }
                #endif
            }
            else
            {
                return false;
            }
      }
      else
      {

         return false;
      }
  }
  else
  {

     return false;
  }


  return true;
#else
 return false;
#endif

}



bool Service::loadFromLocal(CFmlBuf *inbuf,CFmlBuf * outbuf)
{
#ifndef __UNIX

  if (inbuf == 0 || outbuf == 0) return false;

  if ( (inbuf)->IsEmpty() ||  (outbuf)->IsEmpty()) return false;



  //取得系统路径
  char szFullPath[256]="\0";
  GetModuleFileName(NULL,szFullPath,256);
  string strTransCode = szFullPath;

  int j =  strTransCode.find_last_of("\\",strTransCode.length() - 1);

  strTransCode =  strTransCode.substr(0,j);

  strTransCode += "\\servicelocal\\";

   WIN32_FIND_DATA fd;
   HANDLE hFind = FindFirstFile(strTransCode.c_str(), &fd);
   if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
   {
     ;  //存在;
   }
   else
   {    //创建目录
        CreateDirectoryA(strTransCode.c_str(),0);
   }
    FindClose(hFind);





  string sCode;

  try
  {
    sCode = inbuf->GetString("TRANSCODE");
  }
  catch(...)
  {
    return false;
  }


  if (sCode.length() == 0) return false;

   strTransCode += "\\" + sCode + ".txd";



  //检查文件是否被修改
  char szFileNameTmp[256] = "\0";
  strcpy(szFileNameTmp,strTransCode.c_str());

  #ifdef MD5DIGEST
  if (!MD5DigestCheck(szFileNameTmp))
  {
    return false;
  }
  #endif


  if (!this->LoadFromXml(strTransCode.c_str()))   //装入xml文件失败
  {

    return false;
  }

   CFmlBuf * InBufTmp = new CFmlBuf(inbuf->GetBufSize());

   if (readFmlInputBuffer(InBufTmp))  //本地输入域
   {
     if ((*inbuf) == (*InBufTmp))   //输入域值和本地输入域值相等。
     {
       if (readFmlOutputBuffer(outbuf))
       {
            delete InBufTmp;
            return true;
       }
     }
   }


  delete InBufTmp;
  return false;
#else
  return false;
#endif


}







//songzj 20041206
bool Service::readFmlOutputBuffer(CFmlBuf *fmlbuf)
{
#ifndef __UNIX
 localservice::Field fld = this->getFirstOutputField();

 while (fld != 0)
 {


    FLDID iFieldId;
    try
    {
        iFieldId = fmlbuf->GetFldID(fld.Name.c_str());

    }catch(...)
    {
      fld = this->getNextField(fld);
      continue;
    }


     //取得字段类型
    int iTypeId =  fmlbuf->GetFldType(iFieldId);

    localservice::Item it = this->getFirstItem(fld);



    while (it !=  0)
    {
        int iFieldOcc = fmlbuf->GetCount(iFieldId);

        try
        {
            switch(iTypeId)
            {
              case  FLD_CHAR	:
                {
                  string strValue =   it.Value.c_str();
                  if (strValue.length() > 0)
                  {
                   fmlbuf->SetChar(iFieldId,strValue[0],iFieldOcc);
                  }

                };
                break;
              case  FLD_SHORT	:
                {
                  short shortValue  = atol( it.Value.c_str());
                   fmlbuf->SetInt(iFieldId,shortValue,iFieldOcc);

                };
                break;
              case  FLD_LONG	:
                {
                  long longValue  = atol( it.Value.c_str());
                   fmlbuf->SetInt(iFieldId,longValue,iFieldOcc);
                };
                break;
              case  FLD_FLOAT	:
                {
                  double floatValue  = atof( it.Value.c_str());

                  fmlbuf->SetFloat(iFieldId,floatValue,iFieldOcc);
                };
                break;
              case  FLD_DOUBLE:
                {
                  double doubleValue  = atof( it.Value.c_str());
                    fmlbuf->SetFloat(iFieldId,doubleValue,iFieldOcc);

                };
                break;
              case  FLD_STRING:
                {
                  string strValue =  it.Value.c_str();
                  fmlbuf->SetString(iFieldId,strValue.c_str(),iFieldOcc);
                };
                break;
              case FLD_CARRAY:
                {
                }
                break;
              #ifdef FML32
              case FLD_PTR :
                {
                }
                break;
              case FLD_FML32:
                {
                }
                break;
              case FLD_VIEW32:
                {
                }
                break;
              #endif
            }
          }
          catch(...)
          {
            break;
          }
        it =  this->getNextItem(it);
    }

   fld = this->getNextField(fld);
 }

 return true;
#else
 return false;
#endif

}





bool Service::readFmlInputBuffer(CFmlBuf *fmlbuf)
{

 #ifndef __UNIX
  //清除输入参数 songzj
 fmlbuf->ClearFmlValue();

 Field fld = this->getFirstInputField();

 while (fld != 0)
 {
    FLDID iFieldId;
    try
    {
        iFieldId = fmlbuf->GetFldID(fld.Name.c_str());
    }
    catch(...)
    {
        fld = this->getNextField(fld);
        continue;
    }

     //取得字段类型
    int iTypeId =  fmlbuf->GetFldType(iFieldId);

    Item it = this->getFirstItem(fld);

    while (it !=  0)
    {
        int iFieldOcc = fmlbuf->GetCount(iFieldId);

        try
        {
            switch(iTypeId)
            {
              case  FLD_CHAR	:
                {
                  string strValue =   it.Value.c_str();
                  if (strValue.length() > 0)
                  {
                   fmlbuf->SetChar(iFieldId,strValue[0],iFieldOcc);
                  }

                };
                break;
              case  FLD_SHORT	:
                {
                  short shortValue  = atol( it.Value.c_str());
                  fmlbuf->SetInt(iFieldId,shortValue,iFieldOcc);

                };
                break;
              case  FLD_LONG	:
                {
                  long longValue  = atol( it.Value.c_str());
                   fmlbuf->SetInt(iFieldId,longValue,iFieldOcc);
                };
                break;
              case  FLD_FLOAT	:
                {
                  float floatValue  = atof( it.Value.c_str());
                  fmlbuf->SetFloat(iFieldId,floatValue,iFieldOcc);
                };
                break;
              case  FLD_DOUBLE:
                {
                  double doubleValue  = atof( it.Value.c_str());
                  fmlbuf->SetFloat(iFieldId,doubleValue,iFieldOcc);
                };
                break;
              case  FLD_STRING:
                {
                  fmlbuf->SetString(iFieldId,it.Value,iFieldOcc);
                };
                break;
              case FLD_CARRAY:
                {
                }
                break;
              #ifdef FML32
              case FLD_PTR :
                {
                }
                break;
              case FLD_FML32:
                {
                }
                break;
              case FLD_VIEW32:
                {
                }
                break;
              #endif
            }
          }
          catch(...)
          {
            break;
          }
        it =  this->getNextItem(it);
    }

   fld = this->getNextField(fld);
 }

 return true;
 #else
   return false;
 #endif
}









Service::Service()
    :parser(0),docRead(0),docWrite(0),impl(0)
{


}

bool Service::LoadFromXml(const char * szXmlFile)
{
      try
      {
        parser = new XercesDOMParser();

        parser->parse(szXmlFile);

        docRead = parser->getDocument();
        docRead->setEncoding(TransStr("UTF-8").c_uni());

      }
      catch(...)
      {
        return false;
      }

      return true;
}
bool Service::setInputCount(int iCount)
{


  DOMNodeList * nlItem  = docWrite->getElementsByTagName(TransStr("INPUTCOUNT").c_uni());
  char szCount[10] = "\0";
  std::sprintf(szCount,"%d",iCount);
  if (nlItem->getLength() < 1)   return false;

  nlItem->item(0)->setTextContent(TransStr(szCount).c_uni());

  return true;
}
bool Service::setOutputCount(int iCount)
{
  DOMNodeList * nlItem  = docWrite->getElementsByTagName(TransStr("OUTPUTCOUNT").c_uni());
  char szCount[10] = "\0";
  std::sprintf(szCount,"%d",iCount);

  if (nlItem->getLength() < 1)   return false;
  nlItem->item(0)->setTextContent(TransStr(szCount).c_uni());

  return true;
}


Field Service::addInputField(const char *szName,const char * szFieldId)
{

  DOMNodeList * nl  = docWrite->getElementsByTagName(TransStr("INPUTBUFFER").c_uni());
  for (unsigned int j = 0; j < nl->getLength(); ++j)
  {
    DOMNode * nd = nl->item(j);

    if ( (nd->getNodeType() == DOMNode::ELEMENT_NODE) &&
         strcmp("INPUTBUFFER",TransStr(nd->getNodeName()).c_str()) == 0)
    {
       DOMElement * Elem = docWrite->createElement(TransStr("FIELD").c_uni());
       Elem->setAttribute(TransStr("NAME").c_uni(),TransStr(szName).c_uni());
       Elem->setAttribute(TransStr("ID").c_uni(),TransStr(szFieldId).c_uni());
       nd->appendChild(Elem);
       return Field(Elem);
    }
  }
  return 0;
}

Field Service::addField( Field & fld,const char *szName,const char *szFieldId)
{
       if (fld == 0) return 0;

       DOMElement * Elem = docWrite->createElement(XMLString::transcode("FIELD"));


       Elem->setAttribute(TransStr("NAME").c_uni(),TransStr(szName).c_uni());
       Elem->setAttribute(TransStr("ID").c_uni(),TransStr(szFieldId).c_uni());

       //取得父节点
       return Field(fld.getNode()->getParentNode()->appendChild(Elem));

}

//在对应的Field 下增加Item
Item  Service::addItem(Field &fld,const char *sValue)
{
      if (fld == 0) return 0;

      DOMElement * Elem = docWrite->createElement(TransStr("I").c_uni());

      Elem->setTextContent(TransStr(sValue).c_uni());

      return fld.getNode()->appendChild(Elem);

}



Field Service::addOutputField(const char * szName,const char *szFieldId)
{
  DOMNodeList * nl  = docWrite->getElementsByTagName(TransStr("OUTPUTBUFFER").c_uni());
  for (unsigned int j = 0; j < nl->getLength(); ++j)
  {
    DOMNode * nd = nl->item(j);
    if ( (nd->getNodeType() == DOMNode::ELEMENT_NODE) &&
         strcmp("OUTPUTBUFFER",TransStr(nd->getNodeName()).c_str()) == 0)
    {
       DOMElement * Elem = docWrite->createElement(TransStr("FIELD").c_uni());
       Elem->setAttribute(TransStr("NAME").c_uni(),TransStr(szName).c_uni());
       Elem->setAttribute(TransStr("ID").c_uni(),TransStr(szFieldId).c_uni());
       nd->appendChild(Elem);
       return Field(Elem);
    }
  }
  return 0;
}




bool Service::RebuildService()      //重新构建一个xml
{

       static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };

       impl = DOMImplementationRegistry::getDOMImplementation(gLS);

       if (impl != NULL)
       {
           try
           {
               if (docWrite != 0)
               {
                    docWrite->release();
                    docWrite = 0;
               }

               docWrite = impl->createDocument(
                           0,                    // root element namespace URI.
                           TransStr("LOCALSERVICE").c_uni(),         // root element name
                           0);                   // document type object (DTD).

               DOMElement* rootElem = docWrite->getDocumentElement();

               DOMElement*  elem = docWrite->createElement(TransStr("SERVICE").c_uni());

               DOMNode * ndService  = rootElem->appendChild(elem);

               elem = docWrite->createElement(TransStr("PARAMCOUNT").c_uni());
               DOMNode * ndParamCount = ndService->appendChild(elem);

               elem = docWrite->createElement(TransStr("INPUTCOUNT").c_uni());
               ndParamCount->appendChild(elem);

               elem = docWrite->createElement(TransStr("OUTPUTCOUNT").c_uni());
               ndParamCount->appendChild(elem);

               elem = docWrite->createElement(TransStr("INPUTBUFFER").c_uni());
               ndService->appendChild(elem);

               elem = docWrite->createElement(TransStr("OUTPUTBUFFER").c_uni());
               ndService->appendChild(elem);

           }
           catch(...)
           {
               return false;
           }
      }
      else
      {
        return false;
      }

      return true;

}

bool Service::SaveToXml(const char * szXmlFile)
{
         DOMWriter         *theSerializer;
         try
         {
            theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

            theSerializer->setEncoding(TransStr("UTF-8").c_uni());

            LocalFileFormatTarget myFormTarget(szXmlFile);
			docWrite->setEncoding(TransStr("UTF-8").c_uni());
            theSerializer->writeNode(&myFormTarget, *docWrite);
            delete theSerializer;
            docWrite->release();
            docWrite = 0;
            return true;

         }
         catch (...)
         {
            delete theSerializer;
            docWrite->release();
            docWrite = 0;
            return false;

         }
}


  //比较对应的输入缓存    输入的缓存的格式为fieldnamevalue ---serialnumber13951733026
bool Service::compareInputBuffer(STD vector<STD string> &vc)
{
     if (vc.size() == 0) return false;

     //稳定排序
     stable_sort(vc.begin(), vc.end());

     STD vector<STD string> vcInput;

     Field fld = getFirstInputField();

     while (fld != 0)
     {
         Item it(this->getFirstItem(fld));
         while ( it != 0)
         {
             vcInput.push_back(fld.Name+ it.Value);

             it = this->getNextItem(it);
         }
         fld =  this->getNextField(fld);
     }

     if (vc.size() != vcInput.size()) return false;

     stable_sort(vcInput.begin(), vcInput.end());

     for (unsigned int i = 0; i < vc.size(); ++i)
     {
        if (vc[i] != vcInput[i]) return false;

     }

     return true;

}



Service::~Service()
{
         delete docWrite;
         delete parser ;

}


///////////////////////////////////////////

int Service::getInputCount() const
{
          DOMNodeList * nl = docRead->getElementsByTagName(TransStr("INPUTCOUNT").c_uni());

          if (nl->getLength() < 1)
            return -1;
          else
            return atol(TransStr(nl->item(0)->getTextContent()).c_str());

}


int Service::getOutputCount() const
{
          DOMNodeList * nl = docRead->getElementsByTagName(TransStr("OUTPUTCOUNT").c_uni());
          DOMNode * nd = nl->item(0)->getFirstChild();

          return atol(TransStr(nd->getNodeValue()).c_str());


}



Field  Service::getFirstInputField() const
{
            DOMNodeList * nl = docRead->getElementsByTagName(TransStr("INPUTBUFFER").c_uni());
            DOMNodeList * nlField = nl->item(0)->getChildNodes();
            for (unsigned int i = 0; i < nlField->getLength(); ++i)
            {
                DOMNode * nd =  nlField->item(i);
                if (strcmp(TransStr(nd->getNodeName()).c_str(),"FIELD") == 0)
                {
                    return Field(nd);
                }

            }
            return 0;


}

Field  Service::getFirstOutputField() const
{
            DOMNodeList * nl = docRead->getElementsByTagName(TransStr("OUTPUTBUFFER").c_uni());
            if (0 == nl) return 0;

            DOMNodeList * nlField = nl->item(0)->getChildNodes();
            for (unsigned int i = 0; i < nlField->getLength(); ++i)
            {
                DOMNode * nd =  nlField->item(i);
                if (strcmp(TransStr(nd->getNodeName()).c_str(),"FIELD") == 0)
                {
                    return Field(nd);
                }

            }
            return 0;




}


Field  Service::getNextField(Field &fld) const
{

    DOMNode * node = fld.getNode()->getNextSibling();

    while ( node!= 0 && (node->getNodeType() != DOMNode::ELEMENT_NODE))
    {
        node = node->getNextSibling();

    }

    if (node != 0)
    {
      return Field(node);
    }

    return Field(0);


}

Item  Service::getFirstItem(Field &fld) const   //取得第一个item
{
       DOMNodeList * nlItem = fld.getNode()->getChildNodes();
       for (unsigned int m = 0; m < nlItem->getLength(); ++m)
       {
             DOMNode * ndItem = nlItem->item(m); //item
             if (ndItem->getNodeType() == DOMNode::ELEMENT_NODE)
             {
                 return Item(ndItem);
             }
       }
       return 0;












}

Item  Service::getNextItem( Item & item)const //取得下一个item，如果没有就返回空
{


    DOMNode * node = item.getNode()->getNextSibling();

    while ( node!= 0 && (node->getNodeType() != DOMNode::ELEMENT_NODE))
    {
        node = node->getNextSibling();

    }

    if (node != 0)
    {

      return Item(node);
    }

    return 0;






}





/////////////////////////////////////
Field::Field(DOMNode * node)
{
    node_ = node;

    if (node == 0)
    {
       Name.clear();
       FieldId.clear();
    }
    else
    {
       DOMNamedNodeMap *mp =   node_->getAttributes();

       for (unsigned int i = 0; i < mp->getLength(); ++i)
       {
           if (strcmp(TransStr(mp->item(i)->getNodeName()).c_str(),"ID") == 0)
           {
                FieldId = TransStr(mp->item(i)->getNodeValue()).c_str();
           }
           else if (strcmp(TransStr(mp->item(i)->getNodeName()).c_str(),"NAME") == 0)
           {
                 Name = TransStr(mp->item(i)->getNodeValue()).c_str();
           }
       }

   }
}

////////////////////////////////////////

Item::Item(xercesc::DOMNode * node)
{
      node_ = node;

      if (node == 0)
      {
        Value.clear();

      }
      else
      {
         Value = TransStr(node_->getTextContent()).c_str();
		//Value = TransStr(node_->getNodeValue()).c_str();
        //cout << "Item Value = " << (char*)&(node_->getTextContent()) <<endl;
      }
}


/////////////////////////一下为名字服务的

NameService::NameService(xercesc::DOMNode * node)
{
        node_ = node;
        if (node_ != 0)
        {
            DOMNamedNodeMap * mp = node_->getAttributes();
            LogicalName =  TransStr(mp->item(0)->getNodeValue()).c_str();
        }
}

/////////////////////

LocalNameService::LocalNameService()
  :parser(0),
   docRead(0),
   impl(0)
{


}
bool LocalNameService::LoadFromXml(const char * szXmlFile)
{
      try
      {
        parser = new XercesDOMParser();
        parser->parse(szXmlFile);
        docRead = parser->getDocument();

      }
      catch(...)
      {
        return false;
      }

      return true;
}


Naming LocalNameService::getFirstNaming(NameService & ns)
{
            DOMNodeList * nl = ns.getNode()->getChildNodes();
            for (unsigned int i = 0;i < nl->getLength(); ++i)
            {
               DOMNode * ndName = nl->item(i);
               if (ndName->getNodeType() == DOMNode::ELEMENT_NODE)
               {

                   return Naming(ndName);

               }
            }

           return 0;
}

Naming LocalNameService::getNextNaming( Naming & nm)
{

    DOMNode * node = nm.getNode()->getNextSibling();

    while ( node!= 0 && (node->getNodeType() != DOMNode::ELEMENT_NODE))
    {
        node = node->getNextSibling();

    }

    if (node != 0)
    {

      return Naming(node);
    }

    return 0;
}

//增加NameService
NameService LocalNameService::addNameService(const char *szLogicName)
{
     DOMNodeList * nl = docRead->getElementsByTagName(TransStr("LOCALNAMESERVICE").c_uni());

     DOMNode * nd = nl->item(0);
     DOMElement * Elem = docRead->createElement(TransStr("NAMESERVICE").c_uni());
     Elem->setAttribute(TransStr("LOGICNAME").c_uni(),TransStr(szLogicName).c_uni());
     nd->appendChild(Elem);
     return NameService(Elem);
}

bool LocalNameService::SaveToXml()
{

        DOMWriter         *theSerializer;
         try
         {
            static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };

            impl = DOMImplementationRegistry::getDOMImplementation(gLS);

            theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

            LocalFileFormatTarget myFormTarget("LocalName.xml");
            theSerializer->writeNode(&myFormTarget, *docRead);
            delete theSerializer;
            return true;

         }
         catch (...)
         {
            delete theSerializer;
            return false;

         }

}
 //删除节点
bool LocalNameService::deleteNameService(const char * szLogicName)
{

  NameService ns = getNameService(szLogicName);
  if (ns != 0)
  {
    ns.getNode()->getParentNode()->removeChild(ns.getNode());
    return true;
  }
  return false;

}


bool LocalNameService::deleteNaming(NameService &ns,char * MachineName)
{
    DOMNodeList * nl = ns.getNode()->getChildNodes();

    for (unsigned int i = 0; i < nl->getLength(); ++i)
    {
       DOMNode * ndName = nl->item(i);
       if (ndName->getNodeType() == DOMNode::ELEMENT_NODE)
       {
           DOMNodeList * nl = ndName->getChildNodes();
           for (unsigned j = 0; j < nl->getLength(); ++j)
           {
                DOMNode * item = nl->item(i);
                if (ndName->getNodeType() == DOMNode::ELEMENT_NODE)
                {
                   //找到对应的逻辑名
                    if (strcmp(TransStr(item->getNodeName()).c_str(),"MACHINENAME") == 0)
                       if ( strcmp(TransStr(item->getTextContent()).c_str(),MachineName) == 0)
                       {
                          ndName->getParentNode()->removeChild(ndName);
                          return true;
                       }
                }

           }
       }
    }

    return false;

}

NameService LocalNameService::getNameService(const char *szLogicName)
{

    DOMNodeList * nl = docRead->getElementsByTagName(TransStr("NAMESERVICE").c_uni());

    for (unsigned int i = 0; i < nl->getLength(); ++i)
    {
      DOMNode * ndNameService = nl->item(i);
     if (ndNameService->getNodeType() == DOMNode::ELEMENT_NODE)
     {
          DOMNode * ndAttr = ndNameService->getAttributes()->getNamedItem(TransStr("LOGICALNAME").c_uni());

         //找到对应的逻辑名
         if (strcmp(TransStr(ndAttr->getNodeValue()).c_str(),szLogicName) == 0)
         {
           return NameService(ndNameService);
         }
     }
    }

    return 0;

}


LocalContextInfo LocalNameService::getLocalContextInfo()
{
   DOMNodeList * nlInfo = docRead->getElementsByTagName(TransStr("LOCALCONTEXTINFO").c_uni());
   if (nlInfo->getLength() > 0)
   {
        DOMNode * nd = nlInfo->item(0);
        DOMNodeList * nl = nd->getChildNodes();

        LocalContextInfo ctx;
        for (unsigned int i = 0; i < nl->getLength(); ++i)
        {
              if (nl->item(i)->getNodeType() == DOMNode::ELEMENT_NODE)
              {
                 if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"CONTEXTID") == 0)
                 {
                        ctx.ContextId = TransStr(nl->item(i)->getTextContent()).c_str();
                 }
                 else if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"EPARCHYCODE") == 0)
                 {
                       ctx.EparchyCode = TransStr(nl->item(i)->getTextContent()).c_str();
                 }
                 else if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"CITYCODE") == 0)
                 {
                       ctx.CityCode  =  TransStr(nl->item(i)->getTextContent()).c_str();
                 }
                 else if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"ORIGINCODE") == 0)
                 {        ctx.OriginCode =  TransStr(nl->item(i)->getTextContent()).c_str();
                 }
                 else if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"NAMINGUPTTIME") == 0)
                 {       ctx.NamingUptTime = TransStr(nl->item(i)->getTextContent()).c_str();

                 }
                 else if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"NEEDLOCALSERVICE") == 0)
                 {       ctx.NeedLocalService = TransStr(nl->item(i)->getTextContent()).c_str();
                 }
              }
        }
        return ctx;
   }
   return  0;

}
RemoteNamingSrv LocalNameService::getRemoteNamingSrv()
{
   DOMNodeList * nlInfo = docRead->getElementsByTagName(TransStr("REMOTENAMINGSRV").c_uni());
   if (nlInfo->getLength() > 0)
   {
        DOMNode * nd = nlInfo->item(0);
        DOMNodeList * nl = nd->getChildNodes();

        RemoteNamingSrv srv;
        for (unsigned int i = 0; i < nl->getLength(); ++i)
        {
              if (nl->item(i)->getNodeType() == DOMNode::ELEMENT_NODE)
              {
                 if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"IP") == 0)
                 {
                       srv.Ip = TransStr(nl->item(i)->getTextContent()).c_str();
                 }
                 else if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"PORT") == 0)
                 {
                       srv.Port = TransStr(nl->item(i)->getTextContent()).c_str();
                 }
                 else if (strcmp(TransStr(nl->item(i)->getNodeName()).c_str(),"MACHINENAME") == 0)
                 {
                       srv.MachineName  =  TransStr(nl->item(i)->getTextContent()).c_str();
                 }

              }
        }
        return srv;
   }
   return  0;


}




LocalNameService::~LocalNameService()
{

         delete parser ;

}

Naming LocalNameService::setNaming(const Naming &name,
                      char * MachineName,
                      char * Ip,
                       char * Port,
                       char * PhysicalName,
                       char * TypeCode)
{

  DO