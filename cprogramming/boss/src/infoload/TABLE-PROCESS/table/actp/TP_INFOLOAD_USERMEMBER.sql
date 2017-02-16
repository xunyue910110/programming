-- Create table
create table TP_INFOLOAD_USERMEMBER
(
  CHANNELNO          NUMBER(16) not null,
  SYNC_SEQUENCE      NUMBER(16) not null,
  INFOTYPE           NUMBER(3) not null,
  MODIFY_TAG         CHAR(1) not null,
  USER_ID            NUMBER(16) not null,
  MEMBER_ROLE_CODE   VARCHAR2(5) not null,
  MEMBER_ROLE_TYPE   CHAR(1) not null,
  MEMBER_ROLE_ID     NUMBER(16) not null,
  MEMBER_ROLE_NUMBER VARCHAR2(40) not null,
  START_DATE         DATE not null,
  END_DATE           DATE not null,
  UPDATE_TIME        DATE not null,
  DISCNT_PRIORITY    NUMBER(4) not null,
  SERIAL_NUMBER_A    VARCHAR2(40),
  ROLE_CODE_A        CHAR(1)
)
tablespace TBS_ACT_DACT4
  pctfree 10
  initrans 20
  maxtrans 255
  storage
  (
    initial 64
    minextents 1
    maxextents unlimited
  );
-- Add comments to the table 
comment on table TP_INFOLOAD_USERMEMBER
  is '用户成员关系资料同步indb临时表';
-- Add comments to the columns 
comment on column TP_INFOLOAD_USERMEMBER.CHANNELNO
  is '同步通道号';
comment on column TP_INFOLOAD_USERMEMBER.SYNC_SEQUENCE
  is '同步流水号';
comment on column TP_INFOLOAD_USERMEMBER.INFOTYPE
  is '表代号';
comment on column TP_INFOLOAD_USERMEMBER.MODIFY_TAG
  is '修改类型';
comment on column TP_INFOLOAD_USERMEMBER.USER_ID
  is '用户标识      ';
comment on column TP_INFOLOAD_USERMEMBER.MEMBER_ROLE_CODE
  is '成员角色编码  ';
comment on column TP_INFOLOAD_USERMEMBER.MEMBER_ROLE_TYPE
  is '成员角色类型  ';
comment on column TP_INFOLOAD_USERMEMBER.MEMBER_ROLE_ID
  is '成员角色标识  ';
comment on column TP_INFOLOAD_USERMEMBER.MEMBER_ROLE_NUMBER
  is '成员角色号码  ';
comment on column TP_INFOLOAD_USERMEMBER.START_DATE
  is '生效时间      ';
comment on column TP_INFOLOAD_USERMEMBER.END_DATE
  is '失效时间      ';
comment on column TP_INFOLOAD_USERMEMBER.UPDATE_TIME
  is '更新时间      ';
comment on column TP_INFOLOAD_USERMEMBER.DISCNT_PRIORITY
  is '用户优惠优先级';
-- Create/Recreate indexes 
create index IDX_TP_INFOLOAD_USERMEMBER on TP_INFOLOAD_USERMEMBER (USER_ID, MEMBER_ROLE_CODE, MEMBER_ROLE_ID, MEMBER_ROLE_NUMBER, START_DATE)
  tablespace TBS_ACT_IUSR4
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  );
-- Grant/Revoke object privileges 
grant insert, update, delete on TP_INFOLOAD_USERMEMBER to UOP_ACT4;
