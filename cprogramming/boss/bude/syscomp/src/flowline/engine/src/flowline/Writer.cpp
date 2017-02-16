/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Writer.cpp,v 1.1 2011/04/15 07:39:45 sunam Exp $
* $Log: Writer.cpp,v $
* Revision 1.1  2011/04/15 07:39:45  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:04  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:57:15  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:49  luzl
* *** empty log message ***
*
* Revision 1.4  2005/04/27 14:59:28  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/17 07:37:41  juyf
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:45  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:10  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/
#include "Writer.h"
#include "flowline/DataBuffer.h"
#include "flowline/ResourceRepository.h"
#include "flowline/Reader.h"

namespace flowline {
void Writer::doEnter(Context & ctx, Node * from)  
{
    DataProcessContext& dpc = dynamic_cast<DataProcessContext&>(ctx);
 	  DataBuffer * buffer = dynamic_cast < DataBuffer * > (from);
 	
    PDataSet data = buffer->pull();
    if (data) 
    {
    		ResourceRepository * rr = dynamic_cast < ResourceRepository * > (getSubsequentNode());
        if (!m_result) 
        {
        	m_result = PResource(rr->create(ctx));
        	dpc.result().push_back(m_result);
        } 
        else 
        {            
           if(m_result->getMaxRecordCount() != -1 && m_result->getRecordCount() + data->rowCount() > m_result->getMaxRecordCount()) 
           {
           
            	m_result->close();
            	m_result = PResource(rr->create(ctx));
            	dpc.result().push_back(m_result);
           }
        }
        m_result->write(ctx, data);
    } 
    else 
    {
    	  
        if (dynamic_cast < DataSupportable * > (from)->isResourceFinished()) 
        {
        	    setResourceFinished(true);
	     	    	if ( from->isAllFinished() )
		          	setAllFinished( true );
        }
        if (isResourceFinished()) 
        {
            if (m_result) 
            {
                m_result->close();
                m_result = PResource();
            }
        }
    }
 }
//void Writer::popResult(vector<PResource>& result) {
// 	result.insert(result.end(), m_result.begin(), m_result.end());
// 	m_result.clear();
// }
};
