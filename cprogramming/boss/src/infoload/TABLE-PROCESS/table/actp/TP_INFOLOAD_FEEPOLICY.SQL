-- Create table
drop table TP_INFOLOAD_FEEPOLICY;
--正式表的主键为：FEEPOLICY_INS_ID, PARTITION_ID（江苏联通）
create table TP_INFOLOAD_FEEPOLICY
(
  CHANNELNO         NUMBER(16) not null,
  SYNC_SEQUENCE     NUMBER(16) not null,
  INFOTYPE          NUMBER(3)  not null,
  MODIFY_TAG        CHAR(1)    not null,
  PARTITION_ID      NUMBER(4)  not null,
  FEEPOLICY_INS_ID  NUMBER(16) not null,
  ID_TYPE           CHAR(1)    not null,
  ID                NUMBER(16) not null,
  FEEPOLICY_ID      NUMBER(8)  not null,
  RELA_USER_ID      NUMBER(16),
  PRODUCT_ID        NUMBER(8),
  SERV_BUND_ID      NUMBER(8),
  SERV_ID           NUMBER(8),
  FEEPOLICY_BUND_ID NUMBER(8),
  START_DATE        DATE not null,
  END_DATE          DATE not null,
  UPDATE_TIME       DATE not null
)
---分区
partition by RANGE ( CHANNELNO ) 
(
      partition PAR_TP_INFOLOAD_USERFEEPLCY_0          values less than ( 1 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR1,  
      partition PAR_TP_INFOLOAD_USERFEEPLCY_1          values less than ( 2 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR2,          
      partition PAR_TP_INFOLOAD_USERFEEPLCY_2          values less than ( 3 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR3,
      partition PAR_TP_INFOLOAD_USERFEEPLCY_3          values less than ( 4 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR4,
      partition PAR_TP_INFOLOAD_USERFEEPLCY_4          values less than ( MAXVALUE ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR5
);
-- Add comments to the table 
comment on table TP_INFOLOAD_FEEPOLICY
  is '资费资料同步indb临时表';
-- Add comments to the columns 
comment on column TP_INFOLOAD_FEEPOLICY.CHANNELNO
  is '通道号';
comment on column TP_INFOLOAD_FEEPOLICY.SYNC_SEQUENCE
  is '同步流水号';
comment on column TP_INFOLOAD_FEEPOLICY.INFOTYPE
  is '表代号';
comment on column TP_INFOLOAD_FEEPOLICY.MODIFY_TAG
  is '修改类型';
comment on column TP_INFOLOAD_FEEPOLICY.FEEPOLICY_INS_ID
  is '资费实例标识  ';
comment on column TP_INFOLOAD_FEEPOLICY.ID_TYPE
  is '三户类型      ';
comment on column TP_INFOLOAD_FEEPOLICY.ID
  is '三户标识      ';
comment on column TP_INFOLOAD_FEEPOLICY.FEEPOLICY_ID
  is '资费标识      ';
comment on column TP_INFOLOAD_FEEPOLICY.RELA_USER_ID
  is '关联群用户标识';
comment on column TP_INFOLOAD_FEEPOLICY.PRODUCT_ID
  is '产品标识      ';
comment on column TP_INFOLOAD_FEEPOLICY.SERV_BUND_ID
  is '服务包标识    ';
comment on column TP_INFOLOAD_FEEPOLICY.SERV_ID
  is '服务标识      ';
comment on column TP_INFOLOAD_FEEPOLICY.FEEPOLICY_BUND_ID
  is '资费包标识    ';
comment on column TP_INFOLOAD_FEEPOLICY.START_DATE
  is '生效时间      ';
comment on column TP_INFOLOAD_FEEPOLICY.END_DATE
  is '失效时间      ';
comment on column TP_INFOLOAD_FEEPOLICY.UPDATE_TIME
  is '更新时间      ';
-- Create/Recreate indexes 
create index IDX_TP_INFOLOAD_FEEPOLICY on TP_INFOLOAD_FEEPOLICY (FEEPOLICY_INS_ID,CHANNELNO)
  pctfree 10
  initrans 10
  maxtrans 255
  tablespace TBS_ACT_IUSR5
local (
   partition P_I_TP_INFOLOAD_USERFEEPLCY_0                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_1                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_2                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_3                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_4                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5
)
nologging;
create index IDX_TP_INFOLOAD_FEEPOLICY_ID on TP_INFOLOAD_FEEPOLICY (ID, ID_TYPE,CHANNELNO)
  pctfree 10
  initrans 10
  maxtrans 255
  tablespace TBS_ACT_IUSR1
local (
   partition P_I_TP_INFOLOAD_USERFEEPLCY_0                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_1                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_2                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_3                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,
   partition P_I_TP_INFOLOAD_USERFEEPLCY_4                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5
)
nologging;
-- Grant/Revoke object privileges 
grant insert, update, delete on TP_INFOLOAD_FEEPOLICY to UOP_ACTP;

---建立同义词
create or replace synonym  UOP_ACTP.TP_INFOLOAD_FEEPOLICY for UCR_ACTP.TP_INFOLOAD_FEEPOLICY;
