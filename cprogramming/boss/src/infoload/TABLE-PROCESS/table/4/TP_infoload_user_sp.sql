--2010-05-24 09:27:25.179  开始
--2010-05-24 09:37:38.910  结束  总数 407155
--------调用存储过程
---2010-05-24 11:09:02.791 开始 PROC 
---2010-05-24 11:11:41.873 结束       407155


--------调用存储过程
---2010-05-24 14:30:03.537 PROC   记录数 ： 678814
---2010-05-24 2010-05-24 14:34:10.277 PROC

create table TP_INFOLOAD_USER_SP
(
  CHANNELNO          NUMBER(16) not null,
  SYNC_SEQUENCE      NUMBER(16) not null,
  INFOTYPE           NUMBER(3) not null,
  MODIFY_TAG         CHAR(1) not null,
  PARTITION_ID       NUMBER(4) not null,
  USER_ID            NUMBER(16) not null,
  SERIAL_NUMBER      VARCHAR2(40) not null,
  PAY_USER_ID        NUMBER(16) not null,
  PAY_SERIAL_NUMBER  VARCHAR2(40) not null,
  PARTY_ID           VARCHAR2(20) not null,
  SP_SERVICE_ID      VARCHAR2(20) not null,
  SP_PRODUCT_ID      VARCHAR2(40) not null,
  START_DATE         DATE not null,
  END_DATE           DATE not null,
  PRIOR_ORDER_TIME   DATE not null,
  OPR_SOURCE         VARCHAR2(5),
  PRODUCT_ID         NUMBER(8),
  SERV_BUND_ID       NUMBER(8),
  PRE_VALUE_S1       VARCHAR2(50),
  PRE_VALUE_S2       VARCHAR2(50),
  PRE_VALUE_S3       VARCHAR2(50),
  UPDATE_TIME       DATE not null,
  UPDATE_DEPART_ID  CHAR(5),
  UPDATE_STAFF_ID   CHAR(8)    
);
-- Add comments to the table 
comment on table  TP_INFOLOAD_USER_SP
  is 'indb用户表，用户SP表';
-- Create/Recreate indexes 
grant insert, update, delete on TP_INFOLOAD_USER_SP to UOP_ACT1;
create index IDX_TP_INFOLOAD_USER_SP on TP_INFOLOAD_USER_SP (USER_ID, PARTY_ID, SP_SERVICE_ID, SP_PRODUCT_ID, START_DATE, PARTITION_ID);
---建立同义词
create or replace synonym  UOP_ACT1.TP_INFOLOAD_USER_SP for UCR_ACT1.TP_INFOLOAD_USER_SP;