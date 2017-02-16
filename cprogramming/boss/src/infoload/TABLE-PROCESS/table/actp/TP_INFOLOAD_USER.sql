-- Create table 用户主表
/*
 单次提交数据结果：
 记录总数：33230 tf_f_user总数17418 用时:4min
     61 2010-04-19 08:56:54.131 PROC [gboss.cbs.billing1.infoload.1] 2462040 o5p90871E2c10000oBGbL4 [BEGIN]
     62 2010-04-19 09:01:02.693 PROC [gboss.cbs.billing1.infoload.1] 2462040 o5p90871E2c10000oBGbL4 [INPUT] fullname=/billingdata/infoloa   d/1/1_20100302060040_385527.CTBII,count=33230,20100419085653
     63 2010-04-19 09:01:02.708 PROC [gboss.cbs.billing1.infoload.1] 2462040 o5p90871E2c10000oBGbL4 [END]
 批量提交 tf_f_user总数17418 用时:50秒
     2010-04-19 12:24:33.349 PROC [gboss.cbs.billing1.infoload.1] 2474338 y5s90871E2c100004MSbL4 [BEGIN]
     2010-04-19 12:25:17.909 PROC [gboss.cbs.billing1.infoload.1] 2474338 y5s90871E2c100004MSbL4 [INPUT] fullname=/billingdata/infoload/1/1_20100302060040_385528.CTBII,count=33216,20100419122433
     2010-04-19 12:25:17.932 PROC [gboss.cbs.billing1.infoload.1] 2474338 y5s90871E2c100004MSbL4 [END]
*/
create table TP_INFOLOAD_USER
(
  CHANNELNO          NUMBER(16) not null,
  SYNC_SEQUENCE      NUMBER(16) not null,
  INFOTYPE           NUMBER(3) not null,
  MODIFY_TAG         CHAR(1) not null,
  PARTITION_ID       NUMBER(4) not null,
  USER_ID            NUMBER(16) not null,
  DUMMY_TAG          CHAR(1) not null,
  NET_TYPE_CODE      VARCHAR2(2) not null,
  SERIAL_NUMBER      VARCHAR2(40) not null,
  EPARCHY_CODE       CHAR(4) not null,
  CITY_CODE          CHAR(4) not null,
  CUST_ID            NUMBER(16) not null,
  USECUST_ID         NUMBER(16) not null,
  BRAND_CODE         CHAR(4) not null,
  PRODUCT_ID         NUMBER(8) not null,
  USER_TYPE_CODE     CHAR(1) not null,
  PREPAY_TAG         CHAR(1) default '0' not null,
  SERVICE_STATE_CODE VARCHAR2(10) not null,
  OPEN_MODE          CHAR(1) default '0' not null,
  ACCT_TAG           CHAR(1) not null,
  REMOVE_TAG         CHAR(1) not null,
  IN_DATE            DATE not null,
  OPEN_DATE          DATE not null,
  PRE_DESTROY_DATE   DATE,
  DESTROY_DATE       DATE,
  FIRST_CALL_DATE    DATE,
  LAST_STOP_DATE     DATE,
  CREDIT_CLASS       NUMBER(8) not null,
  BASE_CREDIT_VALUE  NUMBER(12) not null,
  CREDIT_VALUE       NUMBER(12) not null,
  CREDIT_CONTROL_ID  NUMBER(8),
  CHANGEUSER_DATE    DATE,
  SCORE_VALUE        NUMBER(12) not null,
  UPDATE_TIME        DATE not null,
  USER_PASSWD        VARCHAR2(30)
);
-- Add comments to the table 
comment on table  TP_INFOLOAD_USER
  is 'indb用户表，客户订购含主体服务的产品会生成用户!';
-- Add comments to the columns 
comment on column TP_INFOLOAD_USER.PARTITION_ID
  is '分区标识：USER_ID的后4位';
comment on column TP_INFOLOAD_USER.USER_ID
  is '用户标识';
comment on column TP_INFOLOAD_USER.DUMMY_TAG
  is '0:普通用户 1:虚拟用户(群)';
comment on column TP_INFOLOAD_USER.NET_TYPE_CODE
  is '网别';
comment on column TP_INFOLOAD_USER.SERIAL_NUMBER
  is '服务号码';
comment on column TP_INFOLOAD_USER.EPARCHY_CODE
  is '地市编码';
comment on column TP_INFOLOAD_USER.CITY_CODE
  is '市县编码';
comment on column TP_INFOLOAD_USER.CUST_ID
  is '归属客户标识';
comment on column TP_INFOLOAD_USER.USECUST_ID
  is '使用客户标识';
comment on column TP_INFOLOAD_USER.BRAND_CODE
  is '当前品牌编码';
comment on column TP_INFOLOAD_USER.PRODUCT_ID
  is '当前产品标识';
comment on column TP_INFOLOAD_USER.USER_TYPE_CODE
  is '用户类型：用于将用户分群,如普通、公免、测试卡，详见用户类型参数表，0必须为普通';
comment on column TP_INFOLOAD_USER.PREPAY_TAG
  is '预付费标志：0-后付费，1-预付费。（省内标准）';
comment on column TP_INFOLOAD_USER.SERVICE_STATE_CODE
  is '主服务状态';
comment on column TP_INFOLOAD_USER.OPEN_MODE
  is '开户方式：0-正常，1-预开未返单，2-预开已返单，3-过户新增，4-当日返单并过户';
comment on column TP_INFOLOAD_USER.ACCT_TAG
  is '出帐标志：0-正常处理，1-定时激活，2-待激活用户，Z-不出帐';
comment on column TP_INFOLOAD_USER.REMOVE_TAG
  is '注销标志：0-正常、1-主动预销号、2-主动销号、3-欠费预销号、4-欠费销号、5-开户返销、6-过户注销';
comment on column TP_INFOLOAD_USER.IN_DATE
  is '建档时间';
comment on column TP_INFOLOAD_USER.OPEN_DATE
  is '开户时间';
comment on column TP_INFOLOAD_USER.PRE_DESTROY_DATE
  is '预销号时间';
comment on column TP_INFOLOAD_USER.DESTROY_DATE
  is '注销时间';
comment on column TP_INFOLOAD_USER.FIRST_CALL_DATE
  is '首次通话时间';
comment on column TP_INFOLOAD_USER.LAST_STOP_DATE
  is '最后停机时间';
comment on column TP_INFOLOAD_USER.CREDIT_CLASS
  is '信用等级';
comment on column TP_INFOLOAD_USER.BASE_CREDIT_VALUE
  is '基本信用额度';
comment on column TP_INFOLOAD_USER.CREDIT_VALUE
  is '信用额度';
comment on column TP_INFOLOAD_USER.CREDIT_CONTROL_ID
  is '信用控制规则ID';
comment on column TP_INFOLOAD_USER.CHANGEUSER_DATE
  is '过户时间';
comment on column TP_INFOLOAD_USER.SCORE_VALUE
  is '当前总积分';
comment on column TP_INFOLOAD_USER.UPDATE_TIME
  is '更新时间';
comment on column TP_INFOLOAD_USER.UPDATE_DEPART_ID
  is '更新部门';
comment on column TP_INFOLOAD_USER.UPDATE_STAFF_ID
  is '更新员工';
-- Create/Recreate indexes 
create index  IDX_TP_INFOLOAD_USER_1 on TP_INFOLOAD_USER (SERIAL_NUMBER);
grant insert, update, delete on TP_INFOLOAD_USER to UOP_ACT1;