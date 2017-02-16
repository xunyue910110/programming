/*
* Copyright (c) 2005, 联创科技
* All rights reserved.
* 
* $Header: /cvsroot/BSS4.2/BSS4.2/billing/bude/syscomp/src/flowline/engine/src/flowline/Definition.h,v 1.1 2011/04/15 07:39:40 sunam Exp $
* $Log: Definition.h,v $
* Revision 1.1  2011/04/15 07:39:40  sunam
* *** empty log message ***
*
* Revision 1.1  2011/03/24 12:01:02  hanyu
* *** empty log message ***
*
* Revision 1.1  2009/09/08 15:56:36  shenglj
* *** empty log message ***
*
* Revision 1.1  2006/01/19 06:37:26  kongdb
* new flowline from yuaneg
*
* Revision 1.1  2005/05/08 11:57:48  luzl
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
* Revision 1.1  2005/03/04 10:24:25  luzl
* init
* 
*/
#ifndef _DEFINITION_H_
#define _DEFINITION_H_
#include "config-all.h"
#include "flowline/Node.h"
#include "flowline/Cycle.h"
#include "flowline/Checker.h"

#include <vector>
USING_NAMESPACE(std);
namespace flowline{
	class FlowLineDao;
    class Definition : public Element {
        
    public:
        Definition();

        virtual ~Definition();
		void init(FlowLineDao & dao);
		
		//0 批处理完毕, 1 交付完毕, -1 交付条件不满足, -2 交付条件超时
        int execute(Context & ctx);

        void reset();
        /**
         * 接管node的生命周期
         */ 
        void addNode(Node* node);
        
        Node* getNodeByName(const string& name);
        
        Node* getNodeById(int id);
        
        size_t getNodeCount() const;
        
        Node* getNode(size_t index);
        
        //接管生命周期
        void addDeliveryChecker(Checker* checker);
        
		const Cycle& getDeliveryCycle() const;
		void setDeliveryCycle(const Cycle& cycle);
		
		void setValidFrom(time_t validFrom);
		void setValidTo(time_t validTo);
		void setBatchCycle(int batchCycle);
		void setConditionTimeOut(int conditionTimeOut);
		void setBlockSize(size_t blockSize); 
		void setDeliveryDelay(int delay);
		
		int getDeliveryDelay() const; 
		time_t getValidFrom() const;
		time_t getValidTo() const;
		int getBatchCycle() const;
		int getConditionTimeOut() const;
		size_t getBlockSize() const;
    private:
                //20060116change -z 
		//size_t Definition::getBusyExecutors(bool isDelivery);
		size_t getBusyExecutors(bool isDelivery);

		//void Definition::execute(Context& ctx, bool isDelivery);
		void execute(Context& ctx, bool isDelivery);

		//bool Definition::checkDeliveryCondition(Context& ctx);
		bool checkDeliveryCondition(Context& ctx);
    
        //所有节点
        vector < Node* > m_nodes;
        //启动节点
        vector < Node* > m_bootNodes;
        //交付阶段的启动节点,
        vector < Node* > m_deliveryBootNodes;
        
        //准许发布数据的的检查器列表
        vector <Checker* > m_deliveryCheckers;
        
        int m_deliveryDelay;
        
        Cycle m_deliveryCycle;
		//生效时间
		time_t m_validFrom;
		//失效时间
		time_t m_validTo;
		//批处理周期(s)
		int    m_batchCycle;
		//交付条件超时时间(s)
		int    m_conditionTimeOut;
		
		size_t m_blockSize;
    };
}


#endif //_DEFINITION_H_
