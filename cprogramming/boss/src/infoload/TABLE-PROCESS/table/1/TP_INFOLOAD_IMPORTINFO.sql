-- Create table
drop table TP_INFOLOAD_IMPORTINFO;
--正式表的主键为：FEEPOLICY_INS_ID, PARTITION_ID（江苏联通）

create table TP_INFOLOAD_IMPORTINFO
(
  CHANNELNO     NUMBER(16) not null,
  SYNC_SEQUENCE NUMBER(16) not null,
  INFOTYPE      NUMBER(3) not null,
  MODIFY_TAG    CHAR(1) not null,
  PARTITION_ID  NUMBER(4) not null,
  USER_ID       NUMBER(16) not null,
  NET_TYPE_CODE CHAR(2) not null,
  PRODUCT_ID    NUMBER(8) not null,
  BRAND_CODE    CHAR(4) not null,
  LOGIC_PHONE   VARCHAR2(40) not null,
  PHYICAL_PHONE VARCHAR2(40) not null,
  START_DATE    DATE not null,
  END_DATE      DATE not null,
  UPDATE_TIME   DATE not null
)---分区
partition by RANGE ( CHANNELNO ) 
(
      partition PAR_TP_INFOLOAD_USERIMPORT_0          values less than ( 1 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR1,  
      partition PAR_TP_INFOLOAD_USERIMPORT_1          values less than ( 2 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR2,          
      partition PAR_TP_INFOLOAD_USERIMPORT_2          values less than ( 3 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR3,
      partition PAR_TP_INFOLOAD_USERIMPORT_3          values less than ( 4 ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR4,
      partition PAR_TP_INFOLOAD_USERIMPORT_4          values less than ( MAXVALUE ) initrans 20 maxtrans 255 pctfree 25 tablespace TBS_ACT_DUSR5
);
-- Add comments to the table 
comment on table TP_INFOLOAD_IMPORTINFO
  is '用户重要信息资料同步indb临时表';
-- Add comments to the columns 
comment on column TP_INFOLOAD_IMPORTINFO.CHANNELNO
  is '同步通道号';
comment on column TP_INFOLOAD_IMPORTINFO.SYNC_SEQUENCE
  is '同步流水号';
comment on column TP_INFOLOAD_IMPORTINFO.INFOTYPE
  is '表代号';
comment on column TP_INFOLOAD_IMPORTINFO.MODIFY_TAG
  is '修改类型';
comment on column TP_INFOLOAD_IMPORTINFO.USER_ID
  is '用户标识  ';
comment on column TP_INFOLOAD_IMPORTINFO.NET_TYPE_CODE
  is '网别      ';
comment on column TP_INFOLOAD_IMPORTINFO.PRODUCT_ID
  is '主产品标识';
comment on column TP_INFOLOAD_IMPORTINFO.BRAND_CODE
  is '品牌编码  ';
comment on column TP_INFOLOAD_IMPORTINFO.LOGIC_PHONE
  is '逻辑号码  ';
comment on column TP_INFOLOAD_IMPORTINFO.PHYICAL_PHONE
  is '物理号码  ';
comment on column TP_INFOLOAD_IMPORTINFO.START_DATE
  is '生效时间  ';
comment on column TP_INFOLOAD_IMPORTINFO.END_DATE
  is '终止时间  ';
comment on column TP_INFOLOAD_IMPORTINFO.UPDATE_TIME
  is '更新时间  ';
-- Create/Recreate indexes 
create index IDX_TP_INFOLOAD_IMPORTINFO on TP_INFOLOAD_IMPORTINFO (USER_ID, START_DATE,CHANNELNO)
  pctfree 10
  initrans 10
  maxtrans 255
  tablespace TBS_ACT_IUSR1
local (
   partition P_I_TP_INFOLOAD_USERIMPORT_0                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,
   partition P_I_TP_INFOLOAD_USERIMPORT_1                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,
   partition P_I_TP_INFOLOAD_USERIMPORT_2                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,
   partition P_I_TP_INFOLOAD_USERIMPORT_3                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,
   partition P_I_TP_INFOLOAD_USERIMPORT_4                          initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5
)
nologging;
-- Grant/Revoke object privileges 
grant insert, update, delete on TP_INFOLOAD_IMPORTINFO to UOP_ACT1;
---建立同义词
create or replace synonym  UOP_ACT1.TP_INFOLOAD_IMPORTINFO for UCR_ACT1.TP_INFOLOAD_IMPORTINFO;