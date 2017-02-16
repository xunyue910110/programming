/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/DataSupportable.cpp,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: DataSupportable.cpp,v $
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
* Revision 1.1  2005/03/07 05:58:11  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/
#include "DataSupportable.h"
namespace flowline {
void DataSupportable::beginProcessResource(Context & ctx, PResource source) {
    setResourceFinished(false);
    onBeginProcessResource(ctx, source);
}
void DataSupportable::onBeginProcessResource(Context & ctx, PResource source) {}
bool DataSupportable::isResourceFinished() const {
    return m_resourceFinished;
}
void DataSupportable::setResourceFinished(bool finished) {
    m_resourceFinished = finished;
}
DataSupportable::DataSupportable() : m_resourceFinished(false) {
}
DataSupportable::~DataSupportable() {
}
};
