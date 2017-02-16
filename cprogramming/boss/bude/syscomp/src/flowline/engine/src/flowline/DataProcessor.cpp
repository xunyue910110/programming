/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/DataProcessor.cpp,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: DataProcessor.cpp,v $
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
* Revision 1.3  2005/04/27 14:59:30  luzl
* *** empty log message ***
*
* Revision 1.2  2005/03/14 12:55:46  luzl
* *** empty log message ***
*
* Revision 1.1  2005/03/07 05:58:12  luzl
* 改名dpp为flowline
*
* Revision 1.1  2005/03/04 10:24:26  luzl
* init
* 
*/

#include "DataProcessor.h"
#include "flowline/DataBuffer.h"
#include "flowline/adapter/DataProcessorAdapter.h"
#include "flowline/adapter/ResourceRepositoryAdapter.h"
#include "base/StringUtil.h"
#include "flowline/ResourceRepository.h"

namespace flowline {
using flowline::adapter::DataProcessorAdapter;

void DataProcessor::doEnter( Context & ctx, Node * from ) {
    DataBuffer * buffer = dynamic_cast < DataBuffer * > ( from );
    PDataSet data = buffer->pull();
    DataProcessorAdapter *adapter = dynamic_cast < DataProcessorAdapter * > ( getAdapter() );
    map < string, PDataSet > result;    
    if ( data ) {
        pair< string, PDataSet > input(buffer->getName(), data);
        adapter->process( ctx, input, result );
    } else if(dynamic_cast < DataSupportable * > ( from ) -> isResourceFinished()) {
     	 setResourceFinished( true );   
     	 if ( from->isAllFinished() )
	        setAllFinished( true );
         adapter->finish( ctx, from->isAllFinished(), result );
    }

    map < string, PDataSet >::iterator rit = result.begin();
    for ( ; rit != result.end(); rit++ ) {
        vector<Node * >::iterator lit = m_subsequentNodes.begin();
        for ( ; lit != m_subsequentNodes.end(); lit++ ) {
            DataBuffer * to = dynamic_cast < DataBuffer * > ( *lit );
            //如果匹配, 或者只有一个输出
            if ( to->getName() == rit->first || m_subsequentNodes.size() == 1) {
                to->push( rit->second );
                to->enter( ctx, this );
                break;
            }
        }
    }
    
    //通知的结束了
    if ( this->isResourceFinished() ) {
        vector<Node * >::iterator lit = m_subsequentNodes.begin();
        for ( ; lit != m_subsequentNodes.end(); lit++ ) {
            DataBuffer * to = dynamic_cast < DataBuffer * > ( *lit );
            to->enter( ctx, this );
        }
    }
}


void DataProcessor::addReference(ResourceRepository* repository, const string& condition) {	
	RefInfo ref;
	ref.index = false;
	ref.condition = condition;
	vector<string> exprs;
	StringUtil::split(condition, " and ", exprs, true);
	for(int i=0; i<exprs.size(); i++) {
		vector<string> params;
		string& expr = exprs[i];
		
		//前面是参考库的字段名, 后面是处理数据的字段名
		StringUtil::split(expr, "=", params, true);
		if(expr.size() != 2) {
			THROW_P1(DefinitionException, "关联条件配置错误: %s", condition.c_str());
		} 
		
		size_t pos = params[0].find(".");
		if(pos != string::npos)
			//去除节点名
			ref.indexFields.push_back(pair<string, dataset::SORT_TYPE>(params[0].substr(pos, params[0].size() - pos), dataset::ASC));
		else
			ref.indexFields.push_back(pair<string, dataset::SORT_TYPE>(params[0], dataset::ASC));
			
		pos = params[1].find(".");
		if(pos != string::npos) 
			ref.keyFields.push_back(params[1].substr(pos, params[1].size() - pos));
		else
			ref.keyFields.push_back(params[1]);	
	}
		
	m_references.push_back(ref);
}

void DataProcessor::getReferenceNames(vector<string>& names) {
	for(int i=0; i<m_references.size(); i++) {
		names.push_back(m_references[i].repository->getName());
	}
}        

void DataProcessor::locateReference(Context& ctx, DataSet::iterator& it, map<string, DataSet::iterator>& result) {
	vector<PResource> resources;
	for(int i=0; i<m_references.size(); i++) {
		RefInfo& ref = m_references[i];
		resources.clear();
		dynamic_cast<ResourceRepositoryAdapter*>(ref.repository->getAdapter())->list(ctx, resources);
		if(resources.size() ==0) {
			THROW(ExecutionException, "关联的库无效: 没有可以资源");
		}
		PDataSet ds = resources[0]->read(ctx);
		if(!ds) {
			THROW(ExecutionException, "关联的库无效: 无法读取资源库");
		}
		if(!ref.index) {
			ds->buildIndex(ref.condition.c_str(), ref.indexFields);		
		}
		
		vector<dataset::Data> datas;
		for(int f=0; f<ref.keyFields.size(); f++) {
			int idx = (*it).getDataSet()->getFieldIndex(ref.keyFields[f].c_str());
			if(idx == -1)
				THROW_P2(ExecutionException, "在%s中有无效的字段%s", ref.condition.c_str(), ref.keyFields[f].c_str());
			datas.push_back((*it)[idx]);
		}
		DataSet::index_iterator it1, it2;
		ds->locate(it1, it2, ref.condition.c_str(), datas);
		if(it1 != it2) {
			//如果找到的话
			result[ref.repository->getName()] = it1;
		} else {
			//没有找到的话
			result[ref.repository->getName()] = ds->end();
		}	
	}
}

}
		


