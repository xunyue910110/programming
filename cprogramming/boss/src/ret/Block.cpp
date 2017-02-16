// ##########################################
// Source file : Block.cpp
// System      : Mobile Billing System
// Version     : 1.01
// Language	   : ANSI C++
// OS Platform : HPUX
// Authors     : Yang Zhang
// E_mail      : zhangy@lianchuang.com
// Create      : 20010816
// Update      : 20010821
// Copyright(C): 2001 by Yang Zhang, Linkage.
// ##########################################

#include "Block.h"

//##ModelId=3B57998703B4
bool Block::initialize(vector <string> aBlockFields,int aBlockHeadSize)
{
	m_BlockHeaderSize_=aBlockHeadSize;
	return true;
}

//##ModelId=3B6216670249
Block::Block()
{
}

//##ModelId=3B4EAC2D021E
void Block::getBlockHeadField()
{
}

