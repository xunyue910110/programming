25事件中的ID为虚拟用户ID，
26事件中的ID为账户ID。

etc/rate.cfg 中事件必须分开配置

    #出帐事件
           <9>
              rule               = (cdr[ATTRI_BIZ_TYPE] == '21')
              #话单处理过程
              procedure          = GetBillProc;AcctAttribProc;TpProc;RentProc;UserDiscntProc;PayRelationProc;AcctDiscntProc;UpdateBillProc
              #基本政策输入字段
              basetp_factors     = ATTRI_SOURCE_TYPE
              #费用帐单项输入字段
              rate_feeid_factors = ATTRI_SOURCE_TYPE
              acct_feeid_factors = ATTRI_SOURCE_TYPE
           </9>
              #群出帐事件
           <25>
              rule               = (cdr[ATTRI_BIZ_TYPE] == '25' )
              #话单处理过程
              procedure          = UserGrpDiscntProc;PayRelationProc;UpdateBillProc
              #基本政策输入字段
              basetp_factors     = ATTRI_SOURCE_TYPE
              #费用帐单项输入字段
              rate_feeid_factors = ATTRI_SOURCE_TYPE
              acct_feeid_factors = ATTRI_SOURCE_TYPE
           </25>
            #账户出帐事件
           <26>
              rule               = (cdr[ATTRI_BIZ_TYPE] == '26' )
              #话单处理过程
              procedure          = AccountAttribProc;AccountTpProc;AcctDiscntProc;UpdateBillProc
              #基本政策输入字段
              basetp_factors     = ATTRI_SOURCE_TYPE
              #费用帐单项输入字段
              rate_feeid_factors = ATTRI_SOURCE_TYPE
              acct_feeid_factors = ATTRI_SOURCE_TYPE
           </26>          

老的生成群或账户下的代表用户的eventor，在eventor_old下。
etc/rate.cfg 中事件配置如下：
   #出帐事件
           <9>
              rule               = (cdr[ATTRI_BIZ_TYPE] == '21'||cdr[ATTRI_BIZ_TYPE] == '25'||cdr[ATTRI_BIZ_TYPE] == '26')
              #话单处理过程
              procedure          = GetBillProc;AcctAttribProc;TpProc;RentProc;UserDiscntProc;PayRelationProc;AcctDiscntProc;UpdateBillProc
              #基本政策输入字段
              basetp_factors     = ATTRI_SOURCE_TYPE
              #费用帐单项输入字段
              rate_feeid_factors = ATTRI_SOURCE_TYPE
              acct_feeid_factors = ATTRI_SOURCE_TYPE
           </9>


营业挂账说明（一次费用收取）

挂账是从CRM录入费用，并同步到BILLING，在账单表中体现。
流程为
1）CRM增量表（TI_B_USER_DEFER），或全量表（tf_b_tradefee_defer）
同步到BILLING侧tf_f_user_tradefee
2）BILLING启动eventor 中配置eventtype = 2的通道，
  eventor 会自动调用帐务库的存储过程p_asp_otp_refreshinfo
  生产文件OTP为前缀的文件
3）拷贝OTP为前缀的文件到批价的输入，批价读取这些文件合到BILL_USER_X
 若OTP对应事件10未配置付费关系绑定，使用相关U文件触发出帐。
 
 
注意事项：
1）要配置挂账的帐目项，CRM对于表为：
td_b_feeitem，BILLING为td_b_externalitemtodetailitem
并配置TD_B_ITEM和TD_B_DETAILITEM

insert into td_b_externalitemtodetailitem 
value 
(
 select distinct feeitem_code, feeitem_code ,1,feeitem_name,sysdate, 'bill','acct',1   from 
ucr_cen1.td_b_feeitem@dblnk_ngcrmdev  where defer_tag = 1
);

insert into td_b_item 
select item_id ,'(固网营业费)'||remark ,  1,0,200501,205012,null ,'ZZZZ' 
from ucr_param.td_b_externalitemtodetailitem;


insert into td_b_detailitem
select item_id, '(固网营业费)'||remark , 0, 0,1,1,1,null,null,null,null,null ,0 from ucr_param.td_b_externalitemtodetailitem


2）精度问题，目前默认CRM单位为元，在存储过程中*1000，转换成厘，
如有变动，修改同步脚本或存储过程。

3）tf_f_user_tradefee.ACT_TAG 对应 tf_b_tradefee_defer.CANCEL_TAG
目前默认ACT_TAG =1 ，CANCEL_TAG =0 为有效，其他值无效。
要和CRM确认，并修改同步脚本。
详见：



配置文件中增加不生成21事件的网别配置，

netTypeCodes = ('13','14','15','16','WV','60','30','52','66','XN','YZ','44','61','63','45','65','32','37','64','31','CP')

其中：    netTypeCodes 配置了不生成21 事件U文件的用户网别，
若不需过滤网别，可配置为空或 0 。
如：     netTypeCodes =
         netTypeCodes = 0
         netTypeCodes =  ‘ ‘
         netTypeCodes =  （‘ ‘）
默认G网的配置为：netTypeCodes = （'15','16'）


=======================================
更新时间：20091214
--1--
备份支持
--1.1--
增加备份标志isBackupValidFile
增加获取函数
--1.2--
CEventOutputFile::linkUnlink
ugp/acctLinkUnlink
函数部分增加拷贝copy函数
--2--
CEventCreatorApp.cpp
实时处理部分更新
判断是否为月结，是则吐文件

-------------------------------
-------------------------------
【附】Eventor配置参考：（以阿盟地市为例）配置文件如下：
-------------------------------------
##红色与橘红色为更新或新增部分##
-------------------------------------
  <1083>
    #ACT数据库(重算表)
    userName = import_nmg
    passWord = nmg123
    servName = ngact
    #参数表数据库
    paramUserName = ucr_param
    paramPassWord = ucrparamabc
    paramServName = ngact
    #altibase/timesten/mdb 内存数据库
    mdbType     =  0            #{0,MDB;1,ODBCMDB;2,REMOTEMDB}
    mdbServer   =  billing      #ODBC时填空(自主研发的mdb时候需要)
    mdbHostName =         #填空即为IPC本地通讯 不为空为跨主机SOCKET通讯，mdb_port端口才有意义
    mdbPort     =         #ODBC和自主远程时候需要
    mdbUserName =
    mdbPassWord =
    sleepTime   =  10
    dbNo        =  1
    eparchyCode =0483


    isBackupValidFile = 1       #是否备份合法输出事件，0:不备份，其他为备份
    backupFilePath =  /billdata1/eventor/common/backup   #合法事件备份目录
    workFilePath =  /billdata1/eventor/1/work
    outputEventFilePath =  /billdata1/eventor/1/output
    outputRateFilePath = /billdata1/rr  #批价输入路径前半部分，这个例子中批价输入路径是/billing/data/rr/XX/uniq/
    #0：用户增量模式
    #1：用户（用户群、账户）全量模式
    #2：一次性费用计算
    #3：用户（用户群、账户）实时模式
    #4：用户群增量模式
    #5：账户增量模式
    eventorType = 1             #0:从接口表读取重出帐事件，1:从内存数据库主表读取重出帐事件，2:一次性费用计算
    netTypeCodes = ('15','16')     #不出帐网别过滤: 内蒙 G网 ('14','15') / 固网 ('15','16')
    fixNetTypeCodes = ('63','60','71','40','61','CP','32','45','20','30','42','51','70','34','31')
    autoOutputFileUserNum =10000
  </1083>
-------------------------------------

修改时间2009年12月26日
应融合需求，G网往月停机不生成U文件，eventor程序加入固网网别判断。
相应配置文件在原有基础上增加参数：fixNetTypeCodes 用于配置固网生成21事件用户网别。


另外，融合版本若要分开G网、固网出账，可以加通道实现，同时配置上做下述更改：
1）单独出G网：
netTypeCodes 配置为G/固网不出账网别+固网出账网别
fixNetTypeCodes 可配置为空或保留为固网出账网别

2）单独出固网：
netTypeCodes 配置为G/固网不出账网别+G网出账网别
fixNetTypeCodes 配置为固网出账网别

3）出G网/固网：

netTypeCodes 配置为G/固网不出账网别
fixNetTypeCodes 配置为固网出账网别

修改时间20091230
1）增加定时自动生成模式的时间偏移问题
配置为：
runTime = 0030  #HHMM

小时分钟，HH ：00-23 MM 00-59

参考配置如下：
#act1库全量重算事件生成(用户，群，账户)--定时自动模式
  <2101>
  
    #ACT数据库(重算表)
    userName = uop_act1
    passWord = A_125C
    servName = NGACT1_TAF
    
    #参数表数据库
    paramUserName = uop_param
    paramPassWord = D_UBA5
    paramServName = NGACT1_TAF
    
    #altibase/timesten/mdb 内存数据库            
    mdbType     =  0            #{0,MDB;1,ODBCMDB;2,REMOTEMDB}
    mdbServer   =  billing      #ODBC时填空(自主研发的mdb时候需要)
    mdbHostName =               #填空即为IPC本地通讯 不为空为跨主机SOCKET通讯，mdb_port端口才有意义
    mdbPort     =               #ODBC和自主远程时候需要
    mdbUserName =  
    mdbPassWord =  
    sleepTime   =  10		
    dbNo        =  1
    
    workFilePath = /billdata2/eventor/eventor1/1/work
    outputEventFilePath = /billdata2/eventor/eventor1/1/output
    isBackupValidFile = 0       #是否备份合法输出事件，0:不备份，其他为备份
    backupFilePath =  /billdata2/eventor/common/backup   #合法事件备份目录
    outputRateFilePath =  /billdata3/rr
    eventorType = 3             #0:从接口表读取重出帐事件，1:从内存数据库主表读取重出帐事件，2:一次性费用计算 3：用户（群）定时自动模式 4：用户群增量模式 5：账户增量模式 3：用户（群）定时自动模式 4：用户群增量模式 5：账户增量模式
    eparchyCode = 0
    netTypeCodes = (' ')
    fixNetTypeCodes =  0
    runTime = 0030
    autoOutputFileUserNum = 20000
  </2101>
  
------------------------------------------------

修改时间2010年1月16日
增加控制固网后付费用户是否生产U文件功能。
固网后付费为：TF_F_USER.prepay_tag = 0 的 网别为固网的用户（在eventor.cfg 中 fixNetTypeCodes 中配置 ）
若：TD_B_CYCLE_EPARCHY.FILE_CREATE_TAG <> 3 则 固网后付费用户不生成文件。

Makefile 中增加编译宏 -D_POSTPAIDSP_





           