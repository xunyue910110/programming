// ##########################################
// Source file : Block.h
// System      : Mobile Billing System
// Version     : NG1.0
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Gao xile
// E_mail      : gaoxl@lianchuang.com
// Create      : 20080619
// Update      : 20080619
// Copyright(C): 2008 by Gao xile, Linkage.
// ##########################################

#ifndef BLOCK_H_HEADER_INCLUDED_B7A61E70
#define BLOCK_H_HEADER_INCLUDED_B7A61E70

#include "config-all.h"
#include "DataFormat.h"
#include "AlertCode.h"
#include "appfrm/ErrorHandle.h"
#include "FieldInfo.h"
#include <string>
#include <vector>

extern ErrorMessages * const theErrorMessages;

//##ModelId=4855BB8C032C
//##Documentation
//## 话单记录块处理类
//## 此类用于得到一个记录块,还负责得到记录块里的字段值(在参数里已定义的)
//## 并且调用Generalfile类得到所有的原始话单记录,存在一个容器里,该容器指针由调用类（PPFileManager）提供。
class Block
{
  public:
    //##ModelId=4855BBA20157
    Block(){}

    //##ModelId=4855BC140148
    //##Documentation
    //## 初始化记录块对象的方法。由PPParameterManager对象调用
    bool initialize(vector <string> aBlockFields, int aBlockHeadSize, const string theBlockParameter, int &BlockSize);

    //##ModelId=4855BC2102CE
    //##Documentation
    //## 获得块头字段的方法
    void getBlockHeadField();

    //##ModelId=4855BC2D02EE
    //##Documentation
    //## 获得块大小的方法
    int getBlockSize(const unsigned char *srcCdrBuffer);

  private:
    //##ModelId=4855BC4102DE
    //##Documentation
    //## 记录块头尺寸
    int m_BlockHeaderSize_;

    //##ModelId=4855BC4B00FA
    int m_StartByte;

    //##ModelId=4855BC5201D4
    int m_Length;

    //##ModelId=4855BC680242
    string m_dataType;

    //##ModelId=4855BC76035B
    char m_padding;

    //##ModelId=4855BC80035B
    int m_order;

    //##ModelId=4855BC8B000F
    DataFormat m_DataFormat;
    
    FieldInfo m_fielInfo;

};



#endif /* BLOCK_H_HEADER_INCLUDED_B7A61E70 */
