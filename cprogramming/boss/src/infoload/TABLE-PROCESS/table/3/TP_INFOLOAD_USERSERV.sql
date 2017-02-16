-- Create table
--正式表的主键为：USER_ID, SERV_ID, START_DATE, PARTITION_ID （江苏联通）
drop table TP_INFOLOAD_USERSERV ;
create table TP_INFOLOAD_USERSERV
(
  CHANNELNO        NUMBER(16) not null,
  SYNC_SEQUENCE    NUMBER(16) not null,
  INFOTYPE         NUMBER(3) not null,
  MODIFY_TAG       CHAR(1) not null,
  PARTITION_ID     NUMBER(4) not null,
  SERV_INS_ID      NUMBER(16) not null,
  USER_ID          NUMBER(16) not null,
  REAL_USER_ID     NUMBER(16),---有的省份没有这个字段
  SERV_ID          NUMBER(8) not null,
  PRIOR_ORDER_TIME DATE,
  MAIN_TAG         CHAR(1) not null,
  PRODUCT_ID       NUMBER(8),
  SERV_BUND_ID     NUMBER(8),
  START_DATE       DATE not null,
  END_DATE         DATE not null,
  UPDATE_TIME      DATE not null
)
---分区
partition by RANGE ( CHANNELNO ) 
(
      partition PAR_TP_INFOLOAD_USERSERV_0          values less than ( 1 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR1,  
      partition PAR_TP_INFOLOAD_USERSERV_1          values less than ( 2 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR2,          
      partition PAR_TP_INFOLOAD_USERSERV_2          values less than ( 3 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR3,
      partition PAR_TP_INFOLOAD_USERSERV_3          values less than ( 4 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR4,
      partition PAR_TP_INFOLOAD_USERSERV_4          values less than ( MAXVALUE ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR5
);
-- Add comments to the table 
comment on table TP_INFOLOAD_USERSERV
  is '用户服务资料同步indb临时表';
-- Add comments to the columns 
comment on column TP_INFOLOAD_USERSERV.CHANNELNO
  is '通道号';
comment on column TP_INFOLOAD_USERSERV.SYNC_SEQUENCE
  is '同步流水号';
comment on column TP_INFOLOAD_USERSERV.INFOTYPE
  is '表代号';
comment on column TP_INFOLOAD_USERSERV.MODIFY_TAG
  is '修改类型';
comment on column TP_INFOLOAD_USERSERV.SERV_INS_ID
  is '服务实例标识';
comment on column TP_INFOLOAD_USERSERV.USER_ID
  is '用户标识    ';
comment on column TP_INFOLOAD_USERSERV.REAL_USER_ID
  is '关联群用户标识    ';
comment on column TP_INFOLOAD_USERSERV.SERV_ID
  is '服务标识    ';
comment on column TP_INFOLOAD_USERSERV.PRIOR_ORDER_TIME
  is '首次订购时间';
comment on column TP_INFOLOAD_USERSERV.MAIN_TAG
  is '主服务标志  ';
comment on column TP_INFOLOAD_USERSERV.PRODUCT_ID
  is '产品标识    ';
comment on column TP_INFOLOAD_USERSERV.SERV_BUND_ID
  is '服务包标识  ';
comment on column TP_INFOLOAD_USERSERV.START_DATE
  is '生效时间    ';
comment on column TP_INFOLOAD_USERSERV.END_DATE
  is '失效时间    ';
comment on column TP_INFOLOAD_USERSERV.UPDATE_TIME
  is '更新时间    ';
-- Create/Recreate indexes 
create index IDX_TP_INFOLOAD_USERSERV on TP_INFOLOAD_USERSERV (USER_ID,CHANNELNO)
  pctfree 10
  initrans 10
  maxtrans 255
  tablespace TBS_ACT_IUSR3
local (
   partition P_I_TP_INFOLOAD_USERSERV_0                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,
   partition P_I_TP_INFOLOAD_USERSERV_1                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,
   partition P_I_TP_INFOLOAD_USERSERV_2                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,
   partition P_I_TP_INFOLOAD_USERSERV_3                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,
   partition P_I_TP_INFOLOAD_USERSERV_4                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5
)
nologging;
-- Grant/Revoke object privileges 
grant insert, update, delete on TP_INFOLOAD_USERSERV to UOP_ACT1;
---建立同义词
create or replace synonym  UOP_ACT1.TP_INFOLOAD_USERSERV for UCR_ACT1.TP_INFOLOAD_USERSERV;