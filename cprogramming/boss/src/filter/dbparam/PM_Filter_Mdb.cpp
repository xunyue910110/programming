//  分拣参数查询接口实现：MDB管理部分.
#include "PM_Filter.h"


bool PM_Filter::setMdbInfo(const MdbInfo &r_mdbInfo)
{
	if(m_mdbManager.initialization(r_mdbInfo)==false)
	{
		return false;
	}
	return true;
}

bool PM_Filter::closeMdbInfo()
{
	m_mdbManager.end();
	return true;
}

bool PM_Filter::MdbNotify(string &r_errMsg)
{
	return m_mdbManager.getMdbNotify(r_errMsg);
}

/*
* 根据逻辑号码(手机号)匹配用户资料
* @param r_sourceType    数据源
* @param r_msisdn       电话号码
* @param r_begintime    开始时间
* @param r_endtime      结束时间
* @param r_userServInfo 用户资料
* @param r_error        错误代码
* @return >0 找到, <=0  未找到
*/
int PM_Filter::getUserServInfoByMsisdn(const T_SOURCE_TYPE &r_sourceType,
									   const T_MSISDN &r_msisdn,
									   const T_DATE_TIME &r_begintime,
									   const T_DATE_TIME &r_endtime,
									   UserServInfo &r_userServInfo,
									   int &r_error)
{
	Mdb_UserImInfobyPhone         t_po;
	vector<Mdb_UserImInfobyPhone> t_rpos;
	int t_iRet=0;
	bool t_flag = false;
	bool t_timesection = false;
	T_FLAG t_timemethod;
	//add by dph
	int t_preoffset, t_postoffset;
	
	//getTimeMethodBySourceType(r_sourceType.c_str(),t_timemethod); //获取绑资料时间模式
	//modify by dph,增加客户资料时间前后偏移
	getTimeMethodOffsetBySourceType(r_sourceType.c_str(),t_timemethod,t_preoffset,t_postoffset); //获取绑资料时间模式
	r_userServInfo.reset();
	//1. 根据MSISDN号在用户_重要信息表tf_f_user_importinfo中查找对应的USERID
	t_po.m_record.m_logic_phone = r_msisdn;
	t_iRet=m_mdbManager.searchMulti<Mdb_UserImInfobyPhone>(t_po,t_rpos);
	if(t_iRet<=0)
	{
		r_error = GET_ERROR_USERID;
		return t_iRet;
	}
	
	//根据开始时间重新排序
	::sort(t_rpos.begin(),t_rpos.end(),UserImInfobyPhoneSort());
	
	if(t_timemethod>=BIND_TIME_SECTION) //先精确匹配资料
	{
		for(vector<Mdb_UserImInfobyPhone>::iterator t_itr=t_rpos.begin();
        t_itr!=t_rpos.end();++t_itr)
		{
			r_userServInfo.m_beginTime = t_itr->m_record.m_beginTime;
			r_userServInfo.m_endTime   = t_itr->m_record.m_endTime;
			//找到一条即返回 以话单开始时间为基准
			if(r_userServInfo.isValid(r_begintime,r_endtime))
			{
				//根据数据源增加网别校验 如果网别校验表中不存在认为匹配上 add by shenglj 2009-04-06 22:33:00
				if(!isCorrectNetBySourceType(r_sourceType.c_str(),t_itr->m_record.m_net_type_code.c_str(),r_begintime))
				{
					continue;
				}
				r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
				r_userServInfo.m_userId     = t_itr->m_record.m_userId;
				r_userServInfo.m_imsi       = t_itr->m_record.m_phyical_phone;
				r_userServInfo.m_msisdn     = r_msisdn;
				r_userServInfo.m_productId  = t_itr->m_record.m_productId;
				r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
				r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
				t_flag = true;
				t_timesection = true;
				break;
			}
		}
	}
	
	for(vector<Mdb_UserImInfobyPhone>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_timesection) //已精确匹配上资料,返回
		{
			break;
		}
		r_userServInfo.m_beginTime = t_itr->m_record.m_beginTime;
		r_userServInfo.m_endTime   = t_itr->m_record.m_endTime;
		//找到一条即返回 以话单开始时间为基准
		//modify by dph,增加客户资料时间偏移判断
		//if(r_userServInfo.isValid(r_begintime,r_endtime,t_timemethod))
		if(r_userServInfo.isValid(r_begintime,r_endtime,t_timemethod,t_preoffset,t_postoffset))
		{
			//根据数据源增加网别校验 如果网别校验表中不存在认为匹配上 add by shenglj 2009-04-06 22:33:00
			if(!isCorrectNetBySourceType(r_sourceType.c_str(),t_itr->m_record.m_net_type_code.c_str(),r_begintime))
			{
				continue;
			}
			r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
			r_userServInfo.m_userId     = t_itr->m_record.m_userId;
			r_userServInfo.m_imsi       = t_itr->m_record.m_phyical_phone;
			r_userServInfo.m_msisdn     = r_msisdn;
			r_userServInfo.m_productId  = t_itr->m_record.m_productId;
			r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
			r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
			t_flag = true;
			break;
		}
	}
	//找不到直接返回
	if(!t_flag)
	{
		r_error = GET_ERROR_USERID;
		return 0;
	}
	
	//2. 根据USERID在主服务实例/用户表tf_f_user中查找用户对应的品牌信息
	t_iRet=getUserServInfo(r_userServInfo.m_userId,r_userServInfo,r_error);
	
	return t_iRet;
}

/*
* 根据物理号码(IMSI 中继号等)匹配用户资料
* @param r_sourceType    数据源
* @param r_imsi         IMSI号 中继号等物理号码
* @param r_begintime    开始时间
* @param r_endtime      结束时间
* @param r_userServInfo 用户资料
* @param r_error        错误代码
* @return >0 找到, <=0  未找到
*/
int PM_Filter::getUserServInfoByImsi(const T_SOURCE_TYPE &r_sourceType,
									 const T_IMSI &r_imsi,
									 const T_DATE_TIME &r_begintime,
									 const T_DATE_TIME &r_endtime,
									 UserServInfo &r_userServInfo,
									 int &r_error)
{
	Mdb_UserImInfobyImsi         t_po;
	vector<Mdb_UserImInfobyImsi> t_rpos;
	int t_iRet=0;
	bool t_flag = false;
	bool t_timesection = false;
	T_FLAG t_timemethod;
	//add by dph
	int t_preoffset, t_postoffset;
	
	//getTimeMethodBySourceType(r_sourceType.c_str(),t_timemethod); //获取绑资料时间模式
	//modify by dph,增加客户资料时间前后偏移
	getTimeMethodOffsetBySourceType(r_sourceType.c_str(),t_timemethod,t_preoffset,t_postoffset); //获取绑资料时间模式
	
	r_userServInfo.reset();
	//1. 根据IMSI号在用户_重要信息表tf_f_user_importinfo中查找对应的USERID
	t_po.m_record.m_phyical_phone = r_imsi;
	t_iRet=m_mdbManager.searchMulti<Mdb_UserImInfobyImsi>(t_po,t_rpos);
	if(t_iRet<=0)
	{
		r_error = GET_ERROR_USERID;
		return t_iRet;
	}
	
	//根据开始时间重新排序
	::sort(t_rpos.begin(),t_rpos.end(),UserImInfobyImsiSort());
	
	if(t_timemethod>=BIND_TIME_SECTION) //先精确匹配资料
	{
		for(vector<Mdb_UserImInfobyImsi>::iterator t_itr=t_rpos.begin();
        t_itr!=t_rpos.end();++t_itr)
		{
			r_userServInfo.m_beginTime = t_itr->m_record.m_beginTime;
			r_userServInfo.m_endTime   = t_itr->m_record.m_endTime;
			//找到一条即返回 以话单开始时间为基准
			if(r_userServInfo.isValid(r_begintime,r_endtime))
			{
				//根据数据源增加网别校验 如果网别校验表中不存在认为匹配上 add by shenglj 2009-04-06 22:33:00
				if(!isCorrectNetBySourceType(r_sourceType.c_str(),t_itr->m_record.m_net_type_code.c_str(),r_begintime))
				{
					continue;
				}
				
				r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
				r_userServInfo.m_userId  = t_itr->m_record.m_userId;
				r_userServInfo.m_msisdn  = t_itr->m_record.m_logic_phone;
				r_userServInfo.m_imsi    = r_imsi;
				r_userServInfo.m_productId  = t_itr->m_record.m_productId;
				r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
				r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
				t_flag = true;
				t_timesection = true;
				break;
			}
		}
	}
	
	for(vector<Mdb_UserImInfobyImsi>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_timesection) //已精确匹配上资料,返回
		{
			break;
		}
		r_userServInfo.m_beginTime = t_itr->m_record.m_beginTime;
		r_userServInfo.m_endTime   = t_itr->m_record.m_endTime;
		//找到一条即返回 以话单开始时间为基准
		//modify by dph,增加客户资料时间偏移判断
		//if(r_userServInfo.isValid(r_begintime,r_endtime,t_timemethod))
		if(r_userServInfo.isValid(r_begintime,r_endtime,t_timemethod,t_preoffset,t_postoffset))
		{
			//根据数据源增加网别校验 如果网别校验表中不存在认为匹配上 add by shenglj 2009-04-06 22:33:00
			if(!isCorrectNetBySourceType(r_sourceType.c_str(),t_itr->m_record.m_net_type_code.c_str(),r_begintime))
			{
				continue;
			}
			
			r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
			r_userServInfo.m_userId  = t_itr->m_record.m_userId;
			r_userServInfo.m_msisdn  = t_itr->m_record.m_logic_phone;
			r_userServInfo.m_imsi    = r_imsi;
			r_userServInfo.m_productId  = t_itr->m_record.m_productId;
			r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
			r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
			t_flag = true;
			break;
		}
	}
	//找不到直接返回
	if(!t_flag)
	{
		r_error = GET_ERROR_USERID;
		return 0;
	}
	
	//2. 根据USERID在主服务实例/用户表tf_f_user中查找用户对应的品牌信息
	t_iRet=getUserServInfo(r_userServInfo.m_userId,r_userServInfo,r_error);
	
	return t_iRet;
}

/*
* 根据逻辑号码(手机号)+物理号码(IMSI 中继号等)匹配用户资料
* @param r_sourceType    数据源
* @param r_msisdn       电话号码
* @param r_imsi         IMSI号 中继号等物理号码
* @param r_begintime    开始时间
* @param r_endtime      结束时间
* @param r_userServInfo 用户资料
* @param r_error        错误代码
* @return >0 找到, <=0  未找到
*/
int PM_Filter::getUserServInfoByMsisdnandImsi(const T_SOURCE_TYPE &r_sourceType,
											  const T_MSISDN &r_msisdn,
											  const T_IMSI &r_imsi,
											  const T_DATE_TIME &r_begintime,
											  const T_DATE_TIME &r_endtime,
											  UserServInfo &r_userServInfo,
											  int &r_error)
{
	Mdb_UserImInfobyPhone         t_po;
	vector<Mdb_UserImInfobyPhone> t_rpos;
	
	int t_iRet=0;
	bool t_flag = false;
	bool t_timesection = false;
	T_FLAG t_timemethod;
	
	getTimeMethodBySourceType(r_sourceType.c_str(),t_timemethod); //获取绑资料时间模式
	r_userServInfo.reset();
	//1. 先根据MSISDN号在用户_重要信息表tf_f_user_importinfo中查找对应的USERID
	t_po.m_record.m_logic_phone = r_msisdn;
	t_iRet=m_mdbManager.searchMulti<Mdb_UserImInfobyPhone>(t_po,t_rpos);
	if(t_iRet<=0)
	{
		r_error = GET_ERROR_USERID;
		return t_iRet;
	}
	
	//根据开始时间重新排序
	::sort(t_rpos.begin(),t_rpos.end(),UserImInfobyPhoneSort());
	
	if(t_timemethod>=BIND_TIME_SECTION) //先精确匹配资料
	{
		for(vector<Mdb_UserImInfobyPhone>::iterator t_itr=t_rpos.begin();
        t_itr!=t_rpos.end();++t_itr)
		{
			r_userServInfo.m_beginTime = t_itr->m_record.m_beginTime;
			r_userServInfo.m_endTime   = t_itr->m_record.m_endTime;
			//找到一条即返回（话单时间有效并且中继号相等） 以话单开始时间为基准
			if(strcmp(r_imsi.c_str(),t_itr->m_record.m_phyical_phone.c_str())==0 &&
				r_userServInfo.isValid(r_begintime,r_endtime))
			{
				//根据数据源增加网别校验 如果网别校验表中不存在认为匹配上 add by shenglj 2009-04-06 22:33:00
				if(!isCorrectNetBySourceType(r_sourceType.c_str(),t_itr->m_record.m_net_type_code.c_str(),r_begintime))
				{
					continue;
				}
				
				r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
				r_userServInfo.m_userId  = t_itr->m_record.m_userId;
				r_userServInfo.m_imsi    = r_imsi;
				r_userServInfo.m_msisdn  = r_msisdn;
				r_userServInfo.m_productId  = t_itr->m_record.m_productId;
				r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
				r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
				t_timesection = true;
				t_flag = true;
				break;
			}
		}
	}
	
	for(vector<Mdb_UserImInfobyPhone>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_timesection) //已精确匹配上资料,返回
		{
			break;
		}
		r_userServInfo.m_beginTime = t_itr->m_record.m_beginTime;
		r_userServInfo.m_endTime   = t_itr->m_record.m_endTime;
		//找到一条即返回（话单时间有效并且中继号相等） 以话单开始时间为基准
		if(strcmp(r_imsi.c_str(),t_itr->m_record.m_phyical_phone.c_str())==0 &&
			r_userServInfo.isValid(r_begintime,r_endtime,t_timemethod))
		{
			//根据数据源增加网别校验 如果网别校验表中不存在认为匹配上 add by shenglj 2009-04-06 22:33:00
			if(!isCorrectNetBySourceType(r_sourceType.c_str(),t_itr->m_record.m_net_type_code.c_str(),r_begintime))
			{
				continue;
			}
			
			r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
			r_userServInfo.m_userId  = t_itr->m_record.m_userId;
			r_userServInfo.m_imsi    = r_imsi;
			r_userServInfo.m_msisdn  = r_msisdn;
			r_userServInfo.m_productId  = t_itr->m_record.m_productId;
			r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
			r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
			t_flag = true;
			break;
		}
	}
	//找不到直接返回
	if(!t_flag)
	{
		r_error = GET_ERROR_USERID;
		return 0;
	}
	
	//2. 根据USERID在主服务实例/用户表tf_f_user中查找用户对应的品牌信息
	t_iRet=getUserServInfo(r_userServInfo.m_userId,r_userServInfo,r_error);
	
	return t_iRet;
											  }
											  
											  
											  /*
											  * 根据业务类型+逻辑号码(手机号)+网别 匹配用户资料(江苏联通)
											  * @param r_ServType     服务类型
											  * @param r_msisdn       电话号码
											  * @param r_netType      网别
											  * @param r_begintime    开始时间
											  * @param r_endtime      结束时间
											  * @param r_userServInfo 用户资料
											  * @param r_error        错误代码
											  * @return >0 找到, <=0  未找到
											  */
											  //原系统的5001业务，新系统变更网别为50
											  //加8规则的为老系统的5002业务，新系统变更网别为51
											  //加9规则的为老系统的5003业务，新系统变更网别为82,55,(30)
											  //add by shenglj 2009-06-29 14:53:01 10170固网融合，新增判断网别30
											  //10170开头的不要做任何变更,此类对应老系统的5002,5003,5004,5005,5006对应新系统的51,55,80,82,83,84
											  int PM_Filter::getUserServInfoByMsisdnandNetType(const T_ATTRI_VAL &r_ServType,
												  const T_ATTRI_VAL &r_msisdn,
												  const T_DATE_TIME &r_begintime,
												  const T_DATE_TIME &r_endtime,
												  UserServInfo &r_userServInfo,
												  int &r_error)
											  {
												  Mdb_UserImInfobyPhone         t_po;
												  vector<Mdb_UserImInfobyPhone> t_rpos;
												  
												  int t_iRet=0;
												  bool t_flag = false;
												  
												  r_userServInfo.reset();
												  //1. 先根据MSISDN号在用户_重要信息表tf_f_user_importinfo中查找对应的USERID
												  t_po.m_record.m_logic_phone = r_msisdn;
												  t_iRet=m_mdbManager.searchMulti<Mdb_UserImInfobyPhone>(t_po,t_rpos);
												  if(t_iRet<=0)
												  {
													  r_error = GET_ERROR_USERID;
													  return t_iRet;
												  }
												  
												  for(vector<Mdb_UserImInfobyPhone>::iterator t_itr=t_rpos.begin();
												  t_itr!=t_rpos.end();++t_itr)
												  {
													  r_userServInfo.m_beginTime = t_itr->m_record.m_beginTime;
													  r_userServInfo.m_endTime   = t_itr->m_record.m_endTime;
													  //找到一条即返回（话单时间有效并且各种条件符合） 以话单开始时间为基准
													  if(strncmp(r_msisdn.c_str(), "10170", 5)==0 &&
														  t_itr->m_record.m_net_type_code.in(NET_TYPE_NOR,NET_TYPE_193TRK,NET_TYPE_IP,NET_TYPE_LONGRENT,NET_TYPE_DATATRK,NET_TYPE_DATARENT,NET_TYPE_NETRENT) &&
														  r_userServInfo.isValid(r_begintime,r_endtime))
													  {
														  r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
														  r_userServInfo.m_userId  = t_itr->m_record.m_userId;
														  r_userServInfo.m_imsi    = t_itr->m_record.m_phyical_phone;
														  r_userServInfo.m_msisdn  = r_msisdn;
														  r_userServInfo.m_productId  = t_itr->m_record.m_productId;
														  r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
														  r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
														  t_flag = true;
														  break;
													  }
													  //193专线
													  else if(r_ServType == SERVICE_TYPE_193_TRUNK && t_itr->m_record.m_net_type_code == NET_TYPE_193TRK &&
														  r_userServInfo.isValid(r_begintime,r_endtime))
													  {
														  r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
														  r_userServInfo.m_userId  = t_itr->m_record.m_userId;
														  r_userServInfo.m_imsi    = t_itr->m_record.m_phyical_phone;
														  r_userServInfo.m_msisdn  = r_msisdn;
														  r_userServInfo.m_productId  = t_itr->m_record.m_productId;
														  r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
														  r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
														  t_flag = true;
														  break;
													  }
													  //193散户
													  else if(r_ServType == SERVICE_TYPE_193_SCATTER && t_itr->m_record.m_net_type_code == NET_TYPE_193REG &&
														  r_userServInfo.isValid(r_begintime,r_endtime))
													  {
														  r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
														  r_userServInfo.m_userId  = t_itr->m_record.m_userId;
														  r_userServInfo.m_imsi    = t_itr->m_record.m_phyical_phone;
														  r_userServInfo.m_msisdn  = r_msisdn;
														  r_userServInfo.m_productId  = t_itr->m_record.m_productId;
														  r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
														  r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
														  t_flag = true;
														  break;
													  }
													  //17911专线
													  else if(r_ServType == SERVICE_TYPE_17911_TRUNK &&
														  t_itr->m_record.m_net_type_code.in(NET_TYPE_NOR,NET_TYPE_IP,NET_TYPE_DATATRK) &&
														  r_userServInfo.isValid(r_begintime,r_endtime))
													  {
														  r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
														  r_userServInfo.m_userId  = t_itr->m_record.m_userId;
														  r_userServInfo.m_imsi    = t_itr->m_record.m_phyical_phone;
														  r_userServInfo.m_msisdn  = r_msisdn;
														  r_userServInfo.m_productId  = t_itr->m_record.m_productId;
														  r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
														  r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
														  t_flag = true;
														  break;
													  }
													  //17911散户
													  else if(r_ServType == SERVICE_TYPE_17911_SCATTER &&
														  t_itr->m_record.m_net_type_code.in(NET_TYPE_NOR,NET_TYPE_193REG,NET_TYPE_193TRK,NET_TYPE_IP,NET_TYPE_LONGRENT,NET_TYPE_DATATRK,NET_TYPE_DATARENT,NET_TYPE_NETRENT) &&
														  r_userServInfo.isValid(r_begintime,r_endtime))
													  {
														  r_userServInfo.m_net_type_code =t_itr->m_record.m_net_type_code;
														  r_userServInfo.m_userId  = t_itr->m_record.m_userId;
														  r_userServInfo.m_imsi    = t_itr->m_record.m_phyical_phone;
														  r_userServInfo.m_msisdn  = r_msisdn;
														  r_userServInfo.m_productId  = t_itr->m_record.m_productId;
														  r_userServInfo.m_brand_code = t_itr->m_record.m_brand_code;
														  r_userServInfo.m_inTime     = r_begintime; //建档时间字段暂时存放用户通话开始时间，用于预开卡用户
														  t_flag = true;
														  break;
													  }
												  }
												  //找不到直接返回
												  if(!t_flag)
												  {
													  r_error = GET_ERROR_USERID;
													  return 0;
												  }
												  
												  //2. 根据USERID在主服务实例/用户表tf_f_user中查找用户对应的品牌信息
												  t_iRet=getUserServInfo(r_userServInfo.m_userId,r_userServInfo,r_error);
												  
												  return t_iRet;
}

/*
* 根据用户标识(USER_ID)匹配用户资料
* @param r_userId       用户标识
* @param r_userServInfo 用户资料
* @param r_error        错误代码
* @return >0 找到, <=0  未找到
*/
int PM_Filter::getUserServInfo(const long &r_userId,
							   UserServInfo &r_userServInfo,
							   int &r_error)
{
	Mdb_UserInfo_Crm                  t_po;
	Mdb_UserInfo_Crm     				      t_rpo;
	int t_iRet=0;
	T_USER_ID r_userid;
	
	if(r_userServInfo.m_userId<=0)
		return t_iRet;
	
	t_po.m_record.m_userId = r_userServInfo.m_userId;
	t_iRet=m_mdbManager.search<Mdb_UserInfo_Crm>(t_po,t_rpo);
	if(t_iRet<=0)
	{
		r_error = GET_ERROR_USERINFO;
		return t_iRet;
	}
	
	//r_userServInfo.m_net_type_code =t_rpo.m_record.m_net_type_code;
	r_userServInfo.m_custId        =t_rpo.m_record.m_custId;
	r_userServInfo.m_eparchy_code  =t_rpo.m_record.m_eparchy_code;
	r_userServInfo.m_office_code   =t_rpo.m_record.m_office_code;
	
	r_userServInfo.m_acctTag       =t_rpo.m_record.m_acct_tag;
	r_userServInfo.m_prepayTag     =t_rpo.m_record.m_prepay_tag; //add by shenglj 2009-10-06 20:19:01
	r_userServInfo.m_userTypeCode =t_rpo.m_record.m_user_type_code; //add by shenglj 2009-11-18 15:52:01
	//预开卡用户开户时间填用户首次通话时间
	if(t_rpo.m_record.m_acct_tag=="2")
	{
		r_userServInfo.m_openTime  = r_userServInfo.m_inTime; //存放话单开始时间
	}
	else
	{
		r_userServInfo.m_openTime  = t_rpo.m_record.m_openTime;
	}
	
	//获取省内通道号
	r_userid = r_userServInfo.m_userId;
	if(!getChannelNoByUserId(r_userid,r_userServInfo.m_eparchy_code,r_userServInfo.m_channelNo))
	{
		r_error = GET_ERROR_CHANNEL;
		t_iRet = 0;
		assert(r_error>0);
		return t_iRet;
	}		
	
	return t_iRet;
}

/*
* 根据用户标识(USER_ID)获取用户状态
* @param r_userId       用户标识
* @param r_finishtime   开始时间
* @param r_userstate    用户状态
* @param r_stategroup   用户状态分组
* @param r_lastmonthstop 0:本月停机 1:上月停机 2:未确定
* @return >0 找到, <=0  未找到
*/
int PM_Filter::getUserStateByUserId(const T_USER_ID &r_userId,
									const T_DATE_TIME &r_finishtime,
									T_ATTRI_VAL &r_userstate,
									T_ATTRI_VAL &r_stategroup,
									T_ATTRI_VAL &r_lastmonthstop,
									int  &r_error)
{
	Mdb_UserServState         t_po;
	vector<Mdb_UserServState> t_rpos;
	int t_iRet=0;
	long r_lastmonth,r_begin_monthbystate,r_end_monthbystate;
	r_error=0;
	
	r_userstate = "+";                //默认状态为+，找不到用户状态
	r_stategroup = USER_UNDEFINED;    //默认分组为3 未确定
	r_lastmonthstop = "0";            //默认本月停机
	t_po.m_record.m_userId = r_userId.value();
	t_iRet=m_mdbManager.searchMulti<Mdb_UserServState>(t_po,t_rpos);
	if(t_iRet<=0)
		return t_iRet;
	
	
	for(vector<Mdb_UserServState>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_itr->m_record.m_main_tag == "1")
		{
			if(!getStateGroup(t_itr->m_record.m_state,t_itr->m_record.m_stategroup))
			{
				r_error = GET_ERROR_STATEGROUP;
				return false;
			}
		}
	}
	
	for(vector<Mdb_UserServState>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_itr->m_record.m_beginTime <= r_finishtime && t_itr->m_record.m_endTime >= r_finishtime &&
			t_itr->m_record.m_main_tag == "1") //取1标识位
		{
			r_userstate  = t_itr->m_record.m_state;
			r_stategroup = t_itr->m_record.m_stategroup;
			r_lastmonth = r_finishtime.substr(0,6).value();
			addMonth(-1,r_lastmonth); //话单的上个月时间
			r_begin_monthbystate = t_itr->m_record.m_beginTime.substr(0,6).value();
			break;
			/*  	 	 
			if(r_lastmonth>=r_begin_monthbystate)
			{
			r_islastmonth = '1'; //状态持续到上个月，直接返回
			return t_iRet;
			}
			else
			{
			r_islastmonth = '0';
			break;  //资料中会存在时间连续，但用户状态不变的情况，重新判
			}
			*/
		}
	}
	
	//用户正常、销户直接返回
	if(r_stategroup == USER_NORMAL || r_stategroup == USER_DESTROY)
	{
		return t_iRet;
	}
	//用户当前停机状态延续到上周
	if(r_stategroup == USER_STOP && r_lastmonth>=r_begin_monthbystate)
	{
		r_lastmonthstop = "1"; //上月停机，返回
		return t_iRet;
	}
	
	if(r_stategroup == USER_STOP)
	{
		::sort(t_rpos.begin(),t_rpos.end());
		for(vector<Mdb_UserServState>::iterator t_itr=t_rpos.begin();
		t_itr!=t_rpos.end();++t_itr)
		{
			//从获取状态处倒序开始算
			if((t_itr->m_record.m_beginTime <= r_finishtime && t_itr->m_record.m_endTime <= r_finishtime) &&
				t_itr->m_record.m_main_tag == "1") //取1标识位
			{
				if(t_itr->m_record.m_stategroup == USER_NORMAL || 
					t_itr->m_record.m_stategroup == USER_DESTROY) //状态发生变化 找到正常状态 销户状态
				{
					r_begin_monthbystate = t_itr->m_record.m_beginTime.substr(0,6).value(); //停机状态取开始时间
					r_end_monthbystate   = t_itr->m_record.m_endTime.substr(0,6).value();   //非停机状态取结束时间
					if(r_lastmonth>=r_end_monthbystate)
					{
						r_lastmonthstop = "1"; //上月停机，返回
						return t_iRet;
					}
					else
					{
						r_lastmonthstop = "0"; //本月停机，返回
						return t_iRet;
					}
				}
			}
		}
		//停机状态
		if(r_lastmonth>=r_begin_monthbystate)
		{
			r_lastmonthstop = "1"; //上月停机，返回
			return t_iRet;
		}
		else
		{
			r_lastmonthstop = "0"; //本月停机，返回
			return t_iRet;
		}
	}
	
	t_iRet = 0;
	return t_iRet;
}

/*
* 根据用户标识(USER_ID)和开始时间时间判上个月用户是否正常
* @param r_userId       用户标识
* @param r_begintime    开始时间
* @return true 找到, false 未找到
*/
bool PM_Filter::StateisValidByLastMonth(const T_USER_ID &r_userId,const T_DATE_TIME &r_begintime)
{
	Mdb_UserServState         t_po;
	vector<Mdb_UserServState> t_rpos;
	int t_iRet=0;
	long r_lastmonth,r_begin_month,r_end_month;
	
	t_po.m_record.m_userId = r_userId.value();
	t_iRet=m_mdbManager.searchMulti<Mdb_UserServState>(t_po,t_rpos);
	if(t_iRet<=0)
		return true; //找不到状态算正常
	
	r_lastmonth = r_begintime.substr(0,6).value();
	addMonth(-1,r_lastmonth);
	
	for(vector<Mdb_UserServState>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		r_begin_month = t_itr->m_record.m_beginTime.substr(0,6).value();
		r_end_month = t_itr->m_record.m_endTime.substr(0,6).value();
		if(r_begin_month <= r_lastmonth && r_end_month >= r_lastmonth)
		{
			if(t_itr->m_record.m_main_tag[0] != '1') //暂定用户状态不为1即判断为上月停机状态
				return false;
		}
	}
	
	return true;
}


/*
* 根据用户标识(USER_ID)、VALUE1和VALUE2之间的范围判是否条件成立
* @param r_infoType     信息类型
* @param r_userId       用户标识
* @param r_begintime    开始时间
* @param r_value        数值
* @return true 找到, false 未找到
*/
bool PM_Filter::getOtherInfoRange(const T_ATTRI_VAL &r_infoType,
								  const T_USER_ID &r_userId,
								  const T_DATE_TIME &r_begintime,
								  const T_ATTRI_VAL &r_value)
{
	Mdb_UserOtherInfo         t_po;
	vector<Mdb_UserOtherInfo> t_rpos;
	int t_iRet=0;
	
	t_po.m_record.m_userId = r_userId.value();
	t_iRet=m_mdbManager.searchMulti<Mdb_UserOtherInfo>(t_po,t_rpos);
	if(t_iRet<=0)
		return false;
	
	for(vector<Mdb_UserOtherInfo>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_itr->m_record.m_otherinfo_type == r_infoType || t_itr->m_record.m_otherinfo_type == "*")
		{
			if(t_itr->m_record.m_attr_value1 <= r_value &&
				t_itr->m_record.m_attr_value2 >= r_value &&
				t_itr->m_record.m_beginTime <= r_begintime && t_itr->m_record.m_endTime >= r_begintime)
			{
				return true;
			}
		}
	}
	
	return false;
}

/*
* 根据用户标识(USER_ID)、VALUE1、VALUE2、VALUE3判是否条件成立
* @param r_infoType     信息类型
* @param r_userId       用户标识
* @param r_begintime    开始时间
* @param r_value1       数值1
* @param r_value2       数值2
* @param r_value3       数值3
* @return true 找到, false 未找到
*/
bool PM_Filter::getOtherInfoBy3(const T_ATTRI_VAL &r_infoType,
								const T_USER_ID &r_userId,
								const T_DATE_TIME &r_begintime,
								const T_ATTRI_VAL &r_value1,
								const T_ATTRI_VAL &r_value2,
								const T_ATTRI_VAL &r_value3)
{
	Mdb_UserOtherInfo         t_po;
	vector<Mdb_UserOtherInfo> t_rpos;
	int t_iRet=0;
	
	t_po.m_record.m_userId = r_userId.value();
	t_iRet=m_mdbManager.searchMulti<Mdb_UserOtherInfo>(t_po,t_rpos);
	if(t_iRet<=0)
		return false;
	
	for(vector<Mdb_UserOtherInfo>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_itr->m_record.m_otherinfo_type == r_infoType || t_itr->m_record.m_otherinfo_type == "*")
		{
			if(strcmp(r_value1.c_str(),t_itr->m_record.m_attr_value1.c_str())==0 &&
				strcmp(r_value2.c_str(),t_itr->m_record.m_attr_value2.c_str())==0 &&
				strcmp(r_value2.c_str(),t_itr->m_record.m_attr_value3.c_str())==0 &&
				t_itr->m_record.m_beginTime <= r_begintime && t_itr->m_record.m_endTime >= r_begintime)
			{
				return true;
			}
		}
	}
	
	return false;
}

/*
* 根据用户标识(USER_ID)、VALUE1、VALUE2判是否条件成立
* @param r_infoType     信息类型
* @param r_userId       用户标识
* @param r_begintime    开始时间
* @param r_value1       数值1
* @param r_value2       数值2
* @return true 找到, false 未找到
*/
bool PM_Filter::getOtherInfoBy2(const T_ATTRI_VAL &r_infoType,
								const T_USER_ID &r_userId,
								const T_DATE_TIME &r_begintime,
								const T_ATTRI_VAL &r_value1,
								const T_ATTRI_VAL &r_value2)
{
	Mdb_UserOtherInfo         t_po;
	vector<Mdb_UserOtherInfo> t_rpos;
	int t_iRet=0;
	
	t_po.m_record.m_userId = r_userId.value();
	t_iRet=m_mdbManager.searchMulti<Mdb_UserOtherInfo>(t_po,t_rpos);
	if(t_iRet<=0)
		return false;
	
	for(vector<Mdb_UserOtherInfo>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_itr->m_record.m_otherinfo_type == r_infoType || t_itr->m_record.m_otherinfo_type == "*")
		{
			if(strcmp(r_value1.c_str(),t_itr->m_record.m_attr_value1.c_str())==0 &&
				strcmp(r_value2.c_str(),t_itr->m_record.m_attr_value2.c_str())==0 &&
				t_itr->m_record.m_beginTime <= r_begintime && t_itr->m_record.m_endTime >= r_begintime)
			{
				return true;
			}
		}
	}
	
	return false;
}

/*
* 根据用户标识(USER_ID)、VALUE1、VALUE2判是否条件成立
* @param r_infoType     信息类型
* @param r_userId       用户标识
* @param r_begintime    开始时间
* @param r_value1       数值1
* @param r_value2       数值2
* @return true 找到, false 未找到
*/
bool PM_Filter::getOtherInfoBy2(const T_ATTRI_VAL &r_infoType,
								const T_USER_ID &r_userId,
								const T_DATE_TIME &r_begintime,
								const T_ATTRI_VAL &r_value1,
								const T_ATTRI_VAL &r_value2,
								T_ATTRI_VAL &r_getvalue)
{
	Mdb_UserOtherInfo         t_po;
	vector<Mdb_UserOtherInfo> t_rpos;
	int t_iRet=0;
	
	t_po.m_record.m_userId = r_userId.value();
	t_iRet=m_mdbManager.searchMulti<Mdb_UserOtherInfo>(t_po,t_rpos);
	if(t_iRet<=0)
		return false;
	
	for(vector<Mdb_UserOtherInfo>::iterator t_itr=t_rpos.begin();
	t_itr!=t_rpos.end();++t_itr)
	{
		if(t_itr->m_record.m_otherinfo_type == r_infoType || t_itr->m_record.m_otherinfo_type == "*")
		{
			if(strcmp(r_value1.c_str(),t_itr->m_record.m_attr_value1.c_str())==0 &&
				strcmp(r_value2.c_str(),t_itr->m_record.m_attr_value2.c_str())==0 &&
				t_itr->m_record.m_beginTime <= r_begintime && t_itr->m_record.m_endTime >= r_begintime)
			{
				r_getvalue = t_itr->m_record.m_attr_value3;
				return true;
			}
		}
	}
	
	return false;
}

/*
* 根据用户标识(USER_ID)、VALUE1判是否条件成立
* @param r_infoType     信息类型
* @param r_userId       用户标识
* @param r_begintime    开始时间
* @param r_value1       数值1
* @return true 找到, false 未找到
*/
bool PM_Filter::getOtherInfoBy1(const T_ATTRI_VAL &r_infoType,
								const T_USER_ID &r_userId,
								const T_DATE_TIME &r_begintime,
								const T_ATTRI_VAL &r_value1)
{
	Mdb_UserOtherInfo         t_po;
	vector<Mdb_UserOtherInfo> t_rpos;
	int t_iRet=0;
	
	t_po.m_record.m_userId = r_userId.value();
	t_iRe瘑(USER_ID)銆乂ALUE1銆乂ALUE2鍒ゆ槸鍚︽潯浠舵垚绔