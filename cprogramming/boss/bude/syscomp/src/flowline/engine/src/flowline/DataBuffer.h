/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/DataBuffer.h,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: DataBuffer.h,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:01  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:56:36  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:49  luzl
* *** empty log message ***
*
* Revision 1.4  2005/04/27 14:59:30  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:15:16  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:27  luzl
* init
* 
*/
#ifndef _DATA_BUFFER_H_
#define _DATA_BUFFER_H_
#include "flowline/DataSupportable.h"
#include "flowline/Node.h"
#include "flowline/DataSet.h"

namespace flowline {
class DataBuffer : public Storage, public DataSupportable {
public:
    //20060116change -z
    //virtual void DataBuffer::doEnter(Context & ctx, Node * from);
    virtual void doEnter(Context & ctx, Node * from);
    PDataSet pull();
    void push(PDataSet data);
    virtual const Metadata* getMetadata();
private:
	//20060116
	//const Metadata* DataBuffer::backwardRef();
	const Metadata* backwardRef();
	//const Metadata* DataBuffer::forwardRef();
	const Metadata* forwardRef();
    PDataSet m_data;
};
}
#endif //_DATA_BUFFER_H_
