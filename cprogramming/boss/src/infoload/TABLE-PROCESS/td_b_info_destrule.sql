-- Add/modify columns 
alter table TD_B_INFO_DESTRULE add OBJ_DB_DEALTAG CHAR(1) default 0;
alter table TD_B_INFO_DESTRULE add OBJ_DB_IN_TABLENAMES VARCHAR2(100);
alter table TD_B_INFO_DESTRULE add OBJ_DB_IN_PROCS VARCHAR2(100);
alter table TD_B_INFO_DESTRULE add OBJ_PROVDB_IN_TABLENAMES VARCHAR2(100);
alter table TD_B_INFO_DESTRULE add OBJ_PROVDB_IN_PROCS VARCHAR2(100);
alter table TD_B_INFO_DESTRULE add hostname VARCHAR2(100);
-- Add comments to the columns 
comment on column TD_B_INFO_DESTRULE.OBJ_DB_DEALTAG
  is '0 : 正常入库模式(默认)
1 ：批量入库模式
【如有省中心库的话，直接通过存储过程完成该部分的入库】
';
comment on column TD_B_INFO_DESTRULE.OBJ_DB_IN_TABLENAMES
  is '入物理库临时表名称，一定要和OBJ_DB_NAMES保持一致
【如有省中心库的话，直接通过存储过程完成该部分的入库】
';
comment on column TD_B_INFO_DESTRULE.OBJ_DB_IN_PROCS
  is '入物理库存储过程名称，一定要和OBJ_DB_NAMES保持一致
【如有省中心库的话，直接通过存储过程完成该部分的入库
';
comment on column TD_B_INFO_DESTRULE.OBJ_PROVDB_IN_TABLENAMES
  is '入省中心库的临时表';
comment on column TD_B_INFO_DESTRULE.OBJ_PROVDB_IN_PROCS
  is '入省中心库的存储过程';
comment on column TD_B_INFO_DESTRULE.hostname
  is '主机名称';
  
  -- Create table
create table TD_B_INFO_DESTRULE
(
  INFO_TYPE                NUMBER(4) not null,
  EPARCHY_CODE             VARCHAR2(4) not null,
  OBJ_DB_NAMES             VARCHAR2(100),
  OBJ_MM_NAMES             VARCHAR2(100),
  REMARK                   VARCHAR2(100),
  OBJ_DB_DEALTAG           CHAR(1) default 0,
  OBJ_DB_IN_TABLENAMES     VARCHAR2(100),
  OBJ_DB_IN_PROCS          VARCHAR2(100),
  OBJ_PROVDB_IN_TABLENAMES VARCHAR2(100),
  OBJ_PROVDB_IN_PROCS      VARCHAR2(100),
  HOSTNAME                 VARCHAR2(100)  
  UPDATE_TIME              DATE,
  UPDATE_DEPART_ID         CHAR(5),
  UPDATE_STAFF_ID          CHAR(8),
);
-- Add comments to the table 
comment on table TD_B_INFO_DESTRULE
  is '资料目标规则';
-- Add comments to the columns 
comment on column TD_B_INFO_DESTRULE.INFO_TYPE
  is '100：客户
101: 个人客户
102：集团客户
103：大客户
200：帐户
203：帐户托收银行
204：帐户间费用互转信息
300：用户
301：用户_成员
302：用户_重要
303：用户_资源
304：服务实例
305：服务实例_参数
306：服务实例_状态
307：用户_其他
308：用户_挂帐
309：SP订购
310：MAS/ADC/行业网关主信息
311：MAS/ADC/行业网关子信息
312：用户费用互转信息
313：帐务(付费)关系
315：周期费用重算
316: 用户_购机
317：用户_押金
318：邮寄信息
401：资费实例
402：资费实例_参数
';
comment on column TD_B_INFO_DESTRULE.EPARCHY_CODE
  is '来源地区编码';
comment on column TD_B_INFO_DESTRULE.OBJ_DB_NAMES
  is '物理数据库集中间用分号”;”分隔开，比如DB1;DB2;DB3;如果该字段为NULL，代表该信息不需要入物理库!';
comment on column TD_B_INFO_DESTRULE.OBJ_MM_NAMES
  is '内存数据库集中间用分号”;”分隔开，比如MM1;MM2;MM3;如果该字段为NULL，代表该信息不需要入内存库!';
comment on column TD_B_INFO_DESTRULE.REMARK
  is '备注';
comment on column TD_B_INFO_DESTRULE.UPDATE_TIME
  is '更新时间';
comment on column TD_B_INFO_DESTRULE.UPDATE_DEPART_ID
  is '更新部门';
comment on column TD_B_INFO_DESTRULE.UPDATE_STAFF_ID
  is '更新员工';
comment on column TD_B_INFO_DESTRULE.OBJ_DB_DEALTAG
  is '0 : 正常入库模式(默认)
1 ：批量入库模式
【如有省中心库的话，直接通过存储过程完成该部分的入库】
';
comment on column TD_B_INFO_DESTRULE.OBJ_DB_IN_TABLENAMES
  is '入物理库临时表名称，一定要和OBJ_DB_NAMES保持一致
【如有省中心库的话，直接通过存储过程完成该部分的入库】
';
comment on column TD_B_INFO_DESTRULE.OBJ_DB_IN_PROCS
  is '入物理库存储过程名称，一定要和OBJ_DB_NAMES保持一致
【如有省中心库的话，直接通过存储过程完成该部分的入库
';
comment on column TD_B_INFO_DESTRULE.OBJ_PROVDB_IN_TABLENAMES
  is '入省中心库的临时表';
comment on column TD_B_INFO_DESTRULE.OBJ_PROVDB_IN_PROCS
  is '入省中心库的存储过程';
comment on column TD_B_INFO_DESTRULE.HOSTNAME
  is '主机名称';
-- Create/Recreate primary, unique and foreign key constraints 
alter table TD_B_INFO_DESTRULE
  add constraint PK_TD_B_INFO_DESTRULE primary key (INFO_TYPE, EPARCHY_CODE)
  using index 
  tablespace TBS_ACT_IPARAM
  pctfree 10
  initrans 10
  maxtrans 255
  storage
  (
    initial 64K
    next 256K
    minextents 1
    maxextents unlimited
    pctincrease 0
  );
