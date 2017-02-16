/*==============================================================*/
/* Database name:  代码生成器                                        */
/* DBMS name:      ORACLE Version 9i                            */
/* Created on:     2004-12-22 10:36:18                          */
/*==============================================================*/

drop table UDBI_TEST cascade constraints
/

drop table UDBI_TEST_DTY cascade constraints
/

drop table UDBI_TEST_ALIAS cascade constraints
/

drop table UDBI_TEST_CUST cascade constraints
/

drop table CODE_CODE cascade constraints
/

/*==============================================================*/
/* Table: UDBI_TEST                                             */
/*==============================================================*/

create table UDBI_TEST  (
   ID                   NUMBER(10),
   NAME                 VARCHAR2(30),
   TYPE                 VARCHAR2(3),
   DTIME                DATE
)
/

/*==============================================================*/
/* Table: UDBI_TEST_DTY                                         */
/*==============================================================*/

create table UDBI_TEST_DTY  (
   ID                   NUMBER(10),
   KIND_ID              NUMBER(3),
   VIP_NBR              NUMBER(12),
   SOC_NBR              NUMBER(11),
   SEX                  CHAR(1),
   PASSWORD             CHAR(6),
   NAME                 VARCHAR2(30),
   REMARK               VARCHAR2(200),
   STATE                VARCHAR2(3),
   STATE_DATE           DATE
)
/

/*==============================================================*/
/* Table: UDBI_TEST_ALIAS                                       */
/*==============================================================*/

create table UDBI_TEST_ALIAS  (
   KIND_ID              NUMBER(3),
   KIND_NAME            VARCHAR2(30)
)
/

/*==============================================================*/
/* Table: UDBI_TEST_CUST                                        */
/*==============================================================*/

create table UDBI_TEST_CUST  (
   CUST_ID              NUMBER(10)                       not null,
   CUST_SEQ_NBR         NUMBER(3)                        not null,
   SOCIAL_TYPE_ID       NUMBER(2),
   SOCIAL_ID            VARCHAR2(30),
   CUST_TYPE_ID         NUMBER(2),
   CUST_NAME            VARCHAR2(60),
   VIP_FLAG             NUMBER(3),
   VIP_NBR              VARCHAR2(16),
   SEX                  CHAR(1),
   STATE                VARCHAR2(3),
   STATE_DATE           DATE,
   "Column_12"          CHAR(10),
   constraint PK_UDBI_TEST_CUST primary key (CUST_ID, CUST_SEQ_NBR)
)
/

/*==============================================================*/
/* Table: CODE_CODE                                             */
/*==============================================================*/

create table CODE_CODE  (
   TAB_NAME             VARCHAR2(30),
   SQL_REF              VARCHAR2(30),
   SQL_STMT             VARCHAR2(4000)
)
/

-- ◆◇◇＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
create or replace procedure proc_test(vID In Number,
                                      vName In Varchar2,
                                      vType OUT varchar2,
                                      vRet	OUT number) is
begin
  select type into vtype from udbi_test where id = vID;
  
  Update udbi_test Set Name = vName, type=vType, DTime=Sysdate 
    Where id = vID;
    
  vRet := 99;
end proc_test;


