#include "PayRelationSolution.h"

//##ModelId=48BCD4A402DB
PayRelationSolution::PayRelationSolution(): m_userId(INVALID_USERID),
                                            m_cycleId(0)
{
	m_accountParam = NULL;

	memset(m_detailItem, 0, sizeof(m_detailItem));
	memset(m_existDetailItem, 0, sizeof(m_existDetailItem));
    
	m_payRelationVector = NULL;
	m_acctIdVector = NULL;
	m_detailItemFeeVector = NULL;
	m_userAccountBillVector = NULL;
}

//##ModelId=48BCD4A402DC
PayRelationSolution::~PayRelationSolution()
{
	m_detailItemVector.clear();
	memset(m_detailItem, 0, sizeof(m_detailItem));
	memset(m_existDetailItem, 0, sizeof(m_existDetailItem));
}

//##ModelId=48BCD4A402DD
int PayRelationSolution::solutePayRelation(const T_USER_ID& r_userId, 
	                        								ComBill *r_pComBill,  
	                        								vector<DetailItemFee> *r_pDetailItemFeeVector,
	                                        vector<UserAccountBill> *r_pUserAccountBillVector,
	                                        RateGlobalInfo *r_pRateInfo
	                                        )
{
 //初始化成员变量
    m_userId                = r_userId;
    m_pComBill              = r_pComBill;
    m_cycleId               = r_pComBill->m_cycleId;
    m_accountParam          = r_pComBill->m_pAccountParam;
    m_userAccountBillVector = r_pUserAccountBillVector;
    m_payRelationVector     = &r_pComBill->m_payRelationVector;
    m_acctIdVector          = &r_pComBill->m_acctIdVector;
    m_detailItemFeeVector   = r_pDetailItemFeeVector;
    m_pAddBillVec           = r_pComBill->m_pAddBillVector;
    m_pRateInfo							=	r_pRateInfo;
    m_leftPayFee						= 0;
    
    (*m_userAccountBillVector).clear();
    
    dealUserBeforeBill();

    //判断用户付费关系类型
    int ifErrorPayRelation = judgeUserPayRelation();
    
    //普通付费关系
    if (ifErrorPayRelation == 0)
    {
        dealCommonUserPayRelation();
    }
    //复杂付费关系
    else if (ifErrorPayRelation == 1)
    {
        dealSpecialUserPayRelation();
    }
    //用户付费关系错误
    else
    {  
    		DEBUG_PRINT("[%s:%d] 用户[%s]付费关系错误!\n",__FILE__,__LINE__,m_userId.c_str());
       	m_pComBill->setErrorCode(m_userId,E_ACCT_GET_PAYRELA,"0");
    }  

    //附加帐单特殊处理
    vector<UserAccountBill>::iterator       itera;
    vector<DetailItemFee>::iterator     iterd;
    for (itera=(m_addUserAccountBillVector).begin(); 
         itera!=(m_addUserAccountBillVector).end(); itera++)
    {
        for (iterd=(*itera).m_detailItemFeeVector.begin();
             iterd!=(*itera).m_detailItemFeeVector.end(); iterd++)
        {
            addAccountDetailItemFee((*itera).m_acctId, 0, '0', 
                                    *iterd, 1
                                    );
        }
    }
    

/*
    //生成0帐单
    vector<T_ACCT_ID>::iterator            iter;
    vector<UserAccountBill>::iterator   iterS;    
    for (iter=(*m_acctIdVector).begin(); iter!=(*m_acctIdVector).end(); iter++)
    {
        for (iterS=(*m_userAccountBillVector).begin();
             iterS!=(*m_userAccountBillVector).end(); iterS++)
        {
            if (iterS->m_acctId == (*iter))
            {
                break;
            }
        }
        
        //如果该帐户没有帐单，生成0帐单.
        if (iterS == (*m_userAccountBillVector).end())
        {
            addAccountDetailItemFee((*iter), 0, '0',0);
        }
    } 
*/
//由于群优惠不生成用户优惠轨迹，因此不能生成用户级付费关系轨迹 2009-12-3 18:01:22 add by xuf
 if (!(m_pRateInfo->m_bizType == BIZ_TYPE_GRPBILL&&
 	    m_pRateInfo->m_billUserId != m_userId))
 {
	//生成优惠方式为7的0纪录
	m_pComBill->setEffectTrack(m_userId,
														USER_ID_TYPE,           
														m_userId,	     
														0,  
														0, 
														'7',  
														0,    
														9999); 
	}
														
	return ifErrorPayRelation;
}

//##ModelId=48BCD4A4030D
int PayRelationSolution::judgeUserPayRelation()
{
	   vector<PayRelation>::iterator iter;
   
    //用户付费关系无记录，返回-1,错误
    if ((*m_payRelationVector).size() == 0)
    {
        return -1;
    }
    //用户付费关系只有一条记录
    else if ((*m_payRelationVector).size() == 1)
    { 
       iter = (*m_payRelationVector).begin();
       
       //如果该记录是默认帐户记录，返回0，代表正常
       if ((iter->m_itemCode==DEFAULT_PAYITEM_CODE) && 
           (iter->m_defaultTag=='1'))
       {
            return 0;
       }
       //不是默认帐户记录，返回-1,错误
       else
       {
            return -1;
       }
    }
    //用户有多条付费关系记录
    else
    {
        int i = 0;
       
        for (iter=(*m_payRelationVector).begin();
             iter!=(*m_payRelationVector).end(); iter++)
        {     
                        	//判断付费关系记录有效性
            	if ((iter->m_limitType<'0') || (iter->m_limitType>'2'))
            	{
                return -1;
            	}

            	if ((iter->m_limitType=='0') && 
                ((iter->m_limitValue>100)||(iter->m_limitValue<0)))
            	{
                return -1;
            	}
                
           	 if ((iter->m_limitType=='1') && (iter->m_limitValue<0))
            	{
                return -1;
            	}               

            	if ((iter->m_fillTag!='0') && (iter->m_fillTag!='1')&&
            		  (iter->m_fillTag!='2') && (iter->m_fillTag!='3'))
            	{
                return -1;
            	}

            
            if ((iter->m_bindType!='0') && (iter->m_bindType!='1'))
            {
                return -1;
            }
                                                   
            //如果是默认帐户，但记录不正确，返回-1,错误
            if ((iter->m_defaultTag=='1') && 
                (iter->m_itemCode!=DEFAULT_PAYITEM_CODE))
            {
                return -1;
            }
            
            //如果是默认帐户，累计次数
            if (iter->m_defaultTag == '1')
            {
                i++;
            }
        }
       
        //如果只有一个默认帐户，返回1，特殊的用户
        if (i == 1)
        {
            return 1;
        }
        //如果没有默认帐户或存在多个默认帐户，返回-1,错误
        else
        {
            return -1;
        }
    }
}

//##ModelId=48BCD4A4030E
void PayRelationSolution::dealCommonUserPayRelation()
{
    vector<PayRelation>::iterator       iter;
    vector<DetailItemFee>::iterator iterd;
    
    DetailItemFee                   detailItemFee;
    UserAccountBill                     userAccountBill;
    
    iter = (*m_payRelationVector).begin();
    
    userAccountBill.m_acctId = iter->m_acctId;
    userAccountBill.m_userPriority = iter->m_discntPriority;
    userAccountBill.m_defaultTag = iter->m_defaultTag;
    userAccountBill.m_detailItemFeeVector.clear();
    
    for (iterd=(*m_detailItemFeeVector).begin();
         iterd!=(*m_detailItemFeeVector).end(); iterd++)
    {
    	  detailItemFee = *iterd;        
        *iterd = 0;
        userAccountBill.m_detailItemFeeVector.push_back(detailItemFee); 
    } 
    
    (*m_userAccountBillVector).push_back(userAccountBill);  
}

//##ModelId=48BCD4A40317
void PayRelationSolution::dealSpecialUserPayRelation()
{
    vector<PayRelation>::iterator       iter;
    
    for (iter=(*m_payRelationVector).begin();
         iter!=(*m_payRelationVector).end(); iter++)
    {
    	  DEBUG_PRINT("正在绑定账户[%s]\n",iter->m_acctId.c_str());
        dealPayRelation(*iter);
    }         
}

//##ModelId=48BCD4A40321
void PayRelationSolution::dealPayRelation(const PayRelation& payRelation)
{
    int payItemFee = 0;
    int effectAcctBillFee = 0;
    int effectUserBillFee = 0;
    
    //如果该记录是默认帐户记录
    if (payRelation.m_defaultTag == '1')
    {
        vector<DetailItemFee>::iterator iter;
        
        for (iter=(*m_detailItemFeeVector).begin();
             iter!=(*m_detailItemFeeVector).end(); iter++)
        {
            if (iter->m_ofee != 0||
		 iter->m_nfee != 0||
		 iter->m_afee != 0||
		 iter->m_bfee != 0)//清0表示已经绑定 要考虑原始费用本身为0的情况 add by xuf 20100314
            {
                addAccountDetailItemFee(payRelation.m_acctId,
                                        payRelation.m_discntPriority, 
                                        payRelation.m_defaultTag, 
                                        *iter
                                        );
                *iter = 0;
                
            }                        
        }         
    }
    //非默认帐户记录处理    
    else
    {
        //计算出帐户应该帮用户付的费用,该记录需要处理
        if (computePayFee(payRelation, payItemFee, effectAcctBillFee, 
                          effectUserBillFee))
        {       
            //将用户该部分费用绑定上帐户
            effectPayFee(payRelation, payItemFee, effectAcctBillFee, 
                         effectUserBillFee);    
        }             
    }                 
}

//##ModelId=48BCD4A4032B
bool PayRelationSolution::computePayFee(const PayRelation& payRelation, int& payItemFee, int& effectAcctBillFee, int& effectUserBillFee)
{
    bool    isNeedDeal = true;
    
    payItemFee = 0;
    effectAcctBillFee = 0;
    effectUserBillFee = 0;
    
    //获取用户付费帐目费用，没有获取到
    if (!getPayItemFee(payRelation.m_itemCode,payItemFee))
    {
        isNeedDeal = false;
        payItemFee = 0;
    }    
    
    //无限定
    if (payRelation.m_limitType == '0')
    {
			 effectAcctBillFee = payItemFee ;
       effectUserBillFee = payItemFee ;
    } 
    //按金额限定
    else if (payRelation.m_limitType == '1')	 
    {
			//不累计
			if (payRelation.m_addupMethod=='0')
			{
         //用户付费帐目大于等于限定值
         if (payItemFee >= payRelation.m_limitValue)
        {
           effectAcctBillFee = payRelation.m_limitValue;
           effectUserBillFee = payRelation.m_limitValue; 
        }
        //用户付费帐目费用小于限定值
        else
        {
            //正常，不补足
            if (payRelation.m_fillTag == '0')
            {
                effectAcctBillFee = payItemFee;
                effectUserBillFee = payItemFee;                 
            }
            //转入用户对应默认账户
            else if (payRelation.m_fillTag == '1')
            { 	
            	//
            }
            //强制补足
            else if(payRelation.m_fillTag == '2')
            {
                effectAcctBillFee = payRelation.m_limitValue;
                effectUserBillFee = payItemFee; 
                
                isNeedDeal = true;                
            }

        	}  
      } 
      //多月限额，累计
			else if (payRelation.m_addupMethod=='1') 
			{
				long t_usedValue=0;
				long t_limitValue=payRelation.m_limitValue;
				//起始帐期<当前帐期
        if (payRelation.m_startCycId<m_cycleId)
        {
        	//取已经使用的累计值
        	if(!getAddupInfo(payRelation.m_payRelationId, payRelation.m_startCycId, payRelation.m_itemCode, t_usedValue))
        	{
#ifdef _DEBUG_
			cout<<"["<<__FILE__<<":"<<__LINE__<<"] 取付费关系累计值错误!"<<endl;
#endif        		
        		m_pComBill->setErrorCode(m_userId,E_ACCT_GET_SUMBILL,"0");
        	}	 
        	
        	 t_limitValue -= t_usedValue; 	
        	 if (t_limitValue<0) t_limitValue=0;
        }//if (payRelation.m_startCycId<m_cycleId)
        
        //用户付费帐目大于等于限定值
				if (payItemFee >= t_limitValue)
				{
					effectAcctBillFee = t_limitValue;
					effectUserBillFee = t_limitValue; 
				}
				//用户付费帐目费用小于限定值
				else
				{
					effectAcctBillFee = payItemFee;
					effectUserBillFee = payItemFee;                 
				}         		
				
				//累计
				t_usedValue += effectAcctBillFee; 
				if (t_usedValue!=0)
					updateAddupInfo(payRelation.m_payRelationId,payRelation.m_itemCode,payRelation.m_startCycId,payRelation.m_endCycId,t_usedValue);
				
			} //if (payRelation.m_addupMethod=='1')
			//2:普通累计，累计值传给下一条付费关系  
			else if (payRelation.m_addupMethod=='2')
			{
				  int t_leftPayFee=0; 
				  //用户付费帐目大于等于限定值
         	if (payItemFee >= payRelation.m_limitValue)
        	{
           	effectAcctBillFee = payRelation.m_limitValue;
           	effectUserBillFee = payRelation.m_limitValue;   
           	t_leftPayFee = 0;
        	}
        	//用户付费帐目费用小于限定值
        	else
        	{
						effectAcctBillFee = payItemFee;
						effectUserBillFee = payItemFee;
					  t_leftPayFee = payRelation.m_limitValue - effectAcctBillFee ; 	             
        	} 
        	
        	m_leftPayFee += t_leftPayFee;  
        	 
			}//if (payRelation.m_addupMethod=='2')
			//3：接收上一条的累计值，需要把月限制额+上一条的累计值作为本条限额。 
			else if (payRelation.m_addupMethod=='3')
			{
				int t_limitValue = payRelation.m_limitValue + m_leftPayFee; 
								
				//用户付费帐目大于等于限定值
        if (payItemFee >= t_limitValue)
        {
         	effectAcctBillFee = t_limitValue;
         	effectUserBillFee = t_limitValue; 
        }
        //用户付费帐目费用小于限定值
        else
        {
          //正常，不补足
          if (payRelation.m_fillTag == '0')
          {
              effectAcctBillFee = payItemFee;
              effectUserBillFee = payItemFee;                 
          }
          //转入用户对应默认账户
          else if (payRelation.m_fillTag == '1')
          { 	
          	//
          }
          //强制补足
          else if(payRelation.m_fillTag == '2')
          {
              effectAcctBillFee = t_limitValue;
              effectUserBillFee = payItemFee; 
              
              isNeedDeal = true;                
          }

        }  
				
			}//if (payRelation.m_addupMethod=='3') 
			//4：跨月累计，并接受上一条付费关系的剩余值（本月可使用的=本条总限额-上期已经使用累计值+上一条使用剩余值）。 
			else if (payRelation.m_addupMethod=='4')
			{
				long t_usedValue=0;
								//起始帐期<当前帐期
        if (payRelation.m_startCycId<m_cycleId)
        {
        	//取已经使用的累计值
        	if(!getAddupInfo(payRelation.m_payRelationId, payRelation.m_startCycId, payRelation.m_itemCode, t_usedValue))
        	{
#ifdef _DEBUG_
			cout<<"["<<__FILE__<<":"<<__LINE__<<"] 取付费关系累计值错误!"<<endl;
#endif        		
        		m_pComBill->setErrorCode(m_userId,E_ACCT_GET_SUMBILL,"0");
        	}	 
        	
        }//if (payRelation.m_startCycId<m_cycleId)
				
				int t_limitValue = payRelation.m_limitValue - t_usedValue + m_leftPayFee; 
				
				//用户付费帐目大于等于限定值
        if (payItemFee >= t_limitValue)
        {
         	effectAcctBillFee = t_limitValue;
         	effectUserBillFee = t_limitValue; 

        }
        //用户付费帐目费用小于限定值
        else
        {
						effectAcctBillFee = payItemFee;
            effectUserBillFee = payItemFee;                   
        }  
				     
				//累计(上条付费关系剩余值不参与累计）
				int t_thisLimitValue= (payRelation.m_limitValue - t_usedValue)>0?(payRelation.m_limitValue - t_usedValue):0;
				t_usedValue += (effectAcctBillFee<t_thisLimitValue?effectAcctBillFee:t_thisLimitValue); 

				if (t_usedValue!=0)
					updateAddupInfo(payRelation.m_payRelationId,payRelation.m_itemCode,payRelation.m_startCycId,payRelation.m_endCycId,t_usedValue);
			}//if (payRelation.m_addupMethod=='4') 
			else//默认不累计
			{
         	//用户付费帐目大于等于限定值
         	if (payItemFee >= payRelation.m_limitValue)
        	{
           	effectAcctBillFee = payRelation.m_limitValue;
           	effectUserBillFee = payRelation.m_limitValue; 
        	}
        	//用户付费帐目费用小于限定值
        	else
        	{
            //正常，不补足
            if (payRelation.m_fillTag == '0')
            {
                effectAcctBillFee = payItemFee;
                effectUserBillFee = payItemFee;                 
            }
            //转入用户对应默认账户
            else if (payRelation.m_fillTag == '1')
            {
            	//	
            }     
            //强制补足
            else if(payRelation.m_fillTag == '2')
            {
                effectAcctBillFee = payRelation.m_limitValue;
                effectUserBillFee = payItemFee; 
                
                isNeedDeal = true;                
            }

        	}  
        }  
            
    }
    //按对应付费帐目消费比例限定
    else if (payRelation.m_limitType == '2')
    {   
    	effectAcctBillFee = (1.0 * payItemFee * payRelation.m_limitValue) / 100;
    	effectUserBillFee = (1.0 * payItemFee * payRelation.m_limitValue) / 100;
    } 
    else//默认不限定
    {
				effectAcctBillFee = payItemFee ;
				effectUserBillFee = payItemFee ;
    } 
      
    return isNeedDeal;
}

//##ModelId=48BCD4A40341
void PayRelationSolution::effectPayFee(const PayRelation& payRelation, const int& payItemFee, const int& effectAcctBillFee, const int& effectUserBillFee)
{
    vector<DetailItemFee>::iterator iter;
    int   nfee = 0;
    
    //如果作用在用户帐单上费用和作用在帐户帐单上费用相同
    if (effectUserBillFee == effectAcctBillFee)
    {
        //如果作用在用户帐目上费用等同于付费帐目费用
        if (effectUserBillFee == payItemFee)
        {
            for (iter=(*m_detailItemFeeVector).begin();
                 iter!=(*m_detailItemFeeVector).end(); iter++)
            {
                //该费用项是付费帐目的一个费用项，且存在
                if (m_detailItem[iter->m_detailItemCode] == 1 &&
                (iter->m_ofee != 0||
		 iter->m_nfee != 0||
		 iter->m_afee != 0||
		 iter->m_bfee != 0)) //要考虑原始费用本身为零的情况 add by xuf 20110314
                {
                    //对帐户帐单作用
                    addAccountDetailItemFee(payRelation.m_acctId,
                                            payRelation.m_discntPriority, 
                                            payRelation.m_defaultTag, 
                                            *iter);
                    //对用户帐单作用
                    *iter=0;
                    
                }    
            }         
        }
        //如果作用在用户帐目上费用不等同于付费帐目费用
        else
        {
            int nfeeRemain = effectUserBillFee;
            
            //按比例绑定
            if (payRelation.m_bindType == '0')
            {
                //按帐目金额平摊处理
                for (iter=(*m_detailItemFeeVector).begin();
                     iter!=(*m_detailItemFeeVector).end(); iter++)
                {
                    //如果剩余作用金额为0，退出
                    if (nfeeRemain == 0)
                    {
                        break;
                    }  
                    
                    //帐目和费用均存在，作用
                    if (m_detailItem[iter->m_detailItemCode] == 1 &&
                        iter->m_ofee != 0)
                    {
                        nfee = (1.0 * iter->m_nfee * effectUserBillFee) / 
                                                                    payItemFee;                         
                        nfeeRemain = nfeeRemain - nfee;       
                        
                        if (iter->m_nfee == nfee)
                        {
                            addAccountDetailItemFee(payRelation.m_acctId,
                                                    payRelation.m_discntPriority,
                                                    payRelation.m_defaultTag,  
                                                    *iter);
                            *iter=0;
                                                          
                        }
                        else
                        {   
                        	  DetailItemFee l_dItemFee;
                        	  l_dItemFee.m_detailItemCode=iter->m_detailItemCode; 
                        	  l_dItemFee.m_nfee=nfee;
                            addAccountDetailItemFee(payRelation.m_acctId,
                                                    payRelation.m_discntPriority,
                                                    payRelation.m_defaultTag, 
                                                    l_dItemFee);
                            iter->m_nfee -= nfee;
                        }    
                    }    
                }//!按帐目金额平摊处理－完成
                
                //补差处理
                if (nfeeRemain != 0)
                {
                    for (iter=(*m_detailItemFeeVector).begin();
                         iter!=(*m_detailItemFeeVector).end(); iter++)
                    {
                        if (m_detailItem[iter->m_detailItemCode] == 1 &&
                                (iter->m_ofee != 0||
		 		                 iter->m_nfee != 0||
		                         iter->m_afee != 0||
		                         iter->m_bfee != 0)) //要考虑原始费用本身为零的情况 add by xuf 20110314
                        {
                            if ((iter->m_nfee - nfeeRemain) > 0)                         
                            {       
                            		DetailItemFee l_dItemFee;
                        	  		l_dItemFee.m_detailItemCode=iter->m_detailItemCode; 
                        	  		l_dItemFee.m_nfee=nfeeRemain;               
                                addAccountDetailItemFee(payRelation.m_acctId,
                                                    payRelation.m_discntPriority,
                                                    payRelation.m_defaultTag, 
                                                    l_dItemFee);
                                iter->m_nfee -= nfeeRemain;
                                
                                break;
                            }
                            else if ((iter->m_nfee - nfeeRemain) == 0)
                            {
                                addAccountDetailItemFee(payRelation.m_acctId,
                                                    payRelation.m_discntPriority,
                                                    payRelation.m_defaultTag, 
                                                    *iter);
                                *iter = 0;

                                break;
                            }       
                        }        
                    }                    
                } //!补差处理－完成                
            }//!按比例绑定－完成
            
            //按优先级绑定
            else
            {
                vector<DetailItem>::iterator    iterS;
                int                             position = 0;
              
                for (iterS=m_detailItemVector.begin(); 
                     iterS!=m_detailItemVector.end(); iterS++)
                {
                    //如果剩余作用金额为0，退出
                    if (nfeeRemain == 0)
                    {
                        break;
                    }
                    
                    position = m_existDetailItem[iterS->m_id];
                    
                    //帐目和费用均存在，作用
                    if (position > 0)
                    {
                        iter = (*m_detailItemFeeVector).begin() + 
                               (position - 1);
                        
                        if (nfeeRemain > iter->m_nfee)
                        {
                            nfee = iter->m_nfee;                         
                        }
                        else
                        {
                            nfee = nfeeRemain;
                        }    
                        
                        nfeeRemain = nfeeRemain - nfee;       
                        
                        if (nfee == iter->m_nfee)
                        {
                            addAccountDetailItemFee(payRelation.m_acctId,
                                                    payRelation.m_discntPriority,
                                                    payRelation.m_defaultTag, 
                                                    *iter);
                            *iter = 0;
                     
                        }
                        else
                        {
                        	  DetailItemFee l_dItemFee;
                        	  l_dItemFee.m_detailItemCode=iter->m_detailItemCode;
                        	  l_dItemFee.m_nfee = nfee; 
                        	  
                            addAccountDetailItemFee(payRelation.m_acctId,
                                                    payRelation.m_discntPriority,
                                                    payRelation.m_defaultTag, 
                                                    l_dItemFee);
                            iter->m_nfee -= nfee;                                                    
                        }    
                    }    
                }    
            }//!按优先级绑定－完成                          
        }//!作用在用户帐目上费用不等同于付费帐目费用－完成
    }//!作用在用户帐单上费用和作用在帐户帐单上费用相同－完成
            
    //作用在用户帐单上费用和作用在帐户帐单上费用不相同，存在补足问题
    else
    {
        //正常作用
        for (iter=(*m_detailItemFeeVector).begin();
             iter!=(*m_detailItemFeeVector).end(); iter++)
        {
            if (m_detailItem[iter->m_detailItemCode] == 1 &&
                iter->m_ofee != 0)
            {
                addAccountDetailItemFee(payRelation.m_acctId,
                                        payRelation.m_discntPriority,
                                        payRelation.m_defaultTag, 
                                        *iter);
                *iter = 0;

            }    
        }         
        
        //对费用进行补足处理
        fillPayFee(payRelation, effectUserBillFee, effectAcctBillFee);                     
    }                 

}

//##ModelId=48BCD4A4034D
bool PayRelationSolution::getPayItemFee(const int& payItemCode, int& nfee)
{
    nfee = 0;
    getDetailItemFromPayItem(payItemCode);

    bool                                ifExist = false;
    int                                 i = 0;
    vector<DetailItemFee>::iterator iter;
    memset(m_existDetailItem, 0, sizeof(m_existDetailItem));
    
    for (iter=(*m_detailItemFeeVector).begin();
         iter!=(*m_detailItemFeeVector).end(); iter++)
    {
        i++;
        if ((m_detailItem[iter->m_detailItemCode]==1) && (iter->m_ofee!=0))
        {
            m_existDetailItem[iter->m_detailItemCode] = i;
            
            nfee += iter->m_nfee;
            ifExist = true;
        }    
    }
    
    return ifExist;

}

//##ModelId=48BCD4A40355
void PayRelationSolution::getDetailItemFromPayItem(const int& payItemCode)
{
    if(!(*m_accountParam).getDetailItemFromPayItem(payItemCode, m_detailItemVector))
    {
      DEBUG_PRINT("根据付费帐目获取明细帐目错误!\n");
    	m_pComBill->setErrorCode(m_userId,E_ACCT_PAY_DETAIL,"0");
    }
    
    vector<DetailItem>::iterator iter;
    memset(m_detailItem, 0, sizeof(m_detailItem));    
    
    //帐目本身为明细帐目
    if (m_detailItemVector.empty()) 
    {
    	m_detailItem[payItemCode] = 1;
    	return;
    }
    
    for (iter=m_detailItemVector.begin(); 
         iter!=m_detailItemVector.end(); iter++)
    {
        m_detailItem[iter->m_id] = 1;
    }
}

//##ModelId=48BCD4A40357
void PayRelationSolution::addAccountDetailItemFee(const T_ACCT_ID& r_acctId, const int& r_userPriority, const char& r_defaultTag, DetailItemFee r_detailItemFee, int r_flag)
{
    vector<UserAccountBill>::iterator   iter;
    vector<DetailItemFee>::iterator