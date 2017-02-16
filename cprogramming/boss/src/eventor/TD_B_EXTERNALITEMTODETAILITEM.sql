-- Create table
create table TD_B_EXTERNALITEMTODETAILITEM
(
  DETAIL_ITEM_CODE   NUMBER(5) not null,
  ITEM_ID            NUMBER(5),
  EXTERNAL_ITEM_CODE NUMBER(5) not null,
  EXTERNAL_SYS_TYPE  NUMBER(5),
  REMARK             VARCHAR2(100),
  UPDATE_TIME        DATE,
  UPDATE_DEPART_ID   CHAR(5),
  UPDATE_STAFF_ID    CHAR(8)
)
tablespace TBS_ACT_DEF
  pctfree 10
  initrans 1
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  );
-- Add comments to the table 
comment on table TD_B_EXTERNALITEMTODETAILITEM
  is '外部帐目、明细帐目对应关系表:描述外部系统明细帐目和帐务明细帐目之间的对应关系';
-- Add comments to the columns 
comment on column TD_B_EXTERNALITEMTODETAILITEM.DETAIL_ITEM_CODE
  is '明细帐目编码';
comment on column TD_B_EXTERNALITEMTODETAILITEM.ITEM_ID
  is '明细帐目标识';
comment on column TD_B_EXTERNALITEMTODETAILITEM.EXTERNAL_ITEM_CODE
  is '外部帐目编码';
comment on column TD_B_EXTERNALITEMTODETAILITEM.EXTERNAL_SYS_TYPE
  is '外部系统类型：标明帐目来源的外部系统类型
0：固定费计算系统
1：代收费系统
2：其他系统';
comment on column TD_B_EXTERNALITEMTODETAILITEM.REMARK
  is '备注';
comment on column TD_B_EXTERNALITEMTODETAILITEM.UPDATE_TIME
  is '更新时间';
comment on column TD_B_EXTERNALITEMTODETAILITEM.UPDATE_DEPART_ID
  is '更新部门';
comment on column TD_B_EXTERNALITEMTODETAILITEM.UPDATE_STAFF_ID
  is '更新员工';
-- Create/Recreate primary, unique and foreign key constraints 
alter table TD_B_EXTERNALITEMTODETAILITEM
  add constraint PK_TD_B_EXTERNALITEMTODETAILIT primary key (DETAIL_ITEM_CODE)
  using index 
  tablespace TBS_ACT_DEF
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
grant select, insert, update, delete on TD_B_EXTERNALITEMTODETAILITEM to UOP_ACT1;
