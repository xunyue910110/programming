/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/DataProcessor.h,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: DataProcessor.h,v $
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
* Revision 1.4  2005/04/27 14:59:28  luzl
* *** empty log message ***
*
* Revision 1.3  2005/03/23 06:15:15  luzl
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
#ifndef _DATA_PROCESSOR_H_
#define _DATA_PROCESSOR_H_
#include "config-all.h"
#include "flowline/DataSupportable.h"
#include "flowline/DataSet.h"
#include "flowline/Node.h"
#include "flowline/Adaptable.h"
#include <vector>
#include <map>
USING_NAMESPACE(std);
namespace flowline{		
//	class ResourceRepository;
    class DataProcessor : public Executor, public Adaptable, public DataSupportable {
    public:
//        void beforeProcessResource(Context & ctx);
        virtual void doEnter(Context & ctx, Node * from) ;
        void addReference(ResourceRepository* repository, const string& condition);
        
        void getReferenceNames(vector<string>& names);
        void locateReference(Context& ctx, DataSet::iterator& it, map<string, DataSet::iterator>& result);
//        void doFinish(Context & ctx, bool isAllFinished, map < string, PData > & result);
	private:
		struct RefInfo {
			ResourceRepository* repository;
			vector<pair<string, dataset::SORT_TYPE> > indexFields;
			vector<string> keyFields;
			string condition;
			bool index;
		};
		vector<RefInfo> m_references;
    };
}
#endif //_DATA_PROCESSOR_H_
