/** Copyright (c) 2004, 联创科技
 *  All rights reserved.
 * 
 *  文件名称：jdb.h
 *  文件标识：
 *  摘    要：
 * 
 *  当前版本：1.0
 *  作    者：WangFJ
 *  完成日期：2004年12月06日
 *
 *  取代版本： 
 *  原作者  ：
 *  完成日期：
 */        

#ifndef __JECONNECT_H_
#define __JECONNECT_H_

#include "udbi.h"
#include "JConnect.h"
#include "JCursor.h"

namespace UDBI {
using std::string;

// 数据库连接接口
class JEConnect: public JConnect {
public:

	//! 2005.11.21 WangFJ.
	virtual JCursor* CreateCursor(const int prefetchRecs=UDBI_PREFETCH_RECS)=0;

};

} // namespace UDBI

#endif

