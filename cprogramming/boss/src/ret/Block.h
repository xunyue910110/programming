// ##########################################
// Source file : Block.h
// System      : Mobile Billing System
// Version     : 1.11
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20010821
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#ifndef BLOCK_H_INCLUDED_C4866FC0
#define BLOCK_H_INCLUDED_C4866FC0

#include "config-all.h"

#include <string>
#include <vector>

USING_NAMESPACE(std)

//##ModelId=3B4E92AE020E
//##Documentation
//## 话单记录块处理类
//## 此类用于得到一个记录块，还负责得到记录块里的字段值（在参数里已定义的），并且调用GeneralCdr类得到所有的原始话单记录，存在一个容器里。该容器指针由调用类（PPFileManager）提供。
//##
class Block
{
  public:
	//##ModelId=3B57998703B4
	//##Documentation
	//## 初始化记录块对象的方法。由PPParameterManager对象调用。
	bool initialize(vector <string> aBlockFields,int aBlockHeadSize);

	//##ModelId=3B6216670249
	Block();

	//##ModelId=3B4EAC2D021E
	//##Documentation
	//## 获得块头字段的方法
	//##
	void getBlockHeadField();


  private:
	//##ModelId=3B5658F2010D
	//##Documentation
	//## 记录块头尺寸
	//##
	int m_BlockHeaderSize_;


};



#endif /* BLOCK_H_INCLUDED_C4866FC0 */
