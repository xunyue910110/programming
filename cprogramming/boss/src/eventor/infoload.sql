---------------------------------------------------------
prompt --(26)营业费用表TF_F_USER_TRADEFEE
---------------------------------------------------------
prompt --(26.0)清空数据
TRUNCATE TABLE TF_F_USER_TRADEFEE;

prompt --(16.1)删除索引
ALTER TABLE TF_F_USER_TRADEFEE DROP CONSTRAINT PK_TF_F_USER_TRADEFEE;
DROP INDEX IDX_TF_F_USER_TRADEFEE_U;

prompt --(16.2)导入数据
--INSERT /*+ append */ INTO TF_F_USER_TRADEFEE
--	(TRADE_ID,USER_ID,TRADE_ITEM_CODE,TRADE_FEE,IN_TIME,ACT_TAG,UPDATE_TIME,UPDATE_DEPART_ID,UPDATE_STAFF_ID)
--SELECT TRADE_ID,USER_ID,FEE_TYPE_CODE,MONEY,to_date(DEFER_ACYC_ID,'YYYYMM'),decode--(CANCEL_TAG,0,1,0),sysdate,'crm','infoload'
--FROM ucr_crm1.tf_b_tradefee_defer@dblnk_ngcrm1 
--WHERE CANCEL_TAG='0';


INSERT /*+ append */ INTO TF_F_USER_TRADEFEE
	(TRADE_ID,USER_ID,TRADE_ITEM_CODE,TRADE_FEE,IN_TIME,ACT_TAG,UPDATE_TIME,UPDATE_DEPART_ID,UPDATE_STAFF_ID)
SELECT TRADE_ID,USER_ID,FEE_TYPE_CODE,MONEY,to_date(DEFER_ACYC_ID,'YYYYMM'),decode(CANCEL_TAG,0,1,0),sysdate,'crm','infoload'
FROM ucr_crm1.ti_b_tradefee_defer@dblnk_ngcrm1 
WHERE CANCEL_TAG='0';

COMMIT;


prompt --(16.3)创建索引
alter table TF_F_USER_TRADEFEE
  add constraint PK_TF_F_USER_TRADEFEE primary key  (
TRADE_ID, USER_ID, TRADE_ITEM_CODE
)
  using index 
  tablespace TBS_ACT_IUSR2
  pctfree 10
  initrans 20
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  )  
nologging;

create index IDX_TF_F_USER_TRADEFEE_U on TF_F_USER_TRADEFEE (
   USER_ID      ASC  
)
 tablespace TBS_ACT_IUSR2
  pctfree 10
  initrans 20
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  )
parallel
nologging;

alter index IDX_TF_F_USER_TRADEFEE_U noparallel;