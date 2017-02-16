CREATE OR REPLACE PROCEDURE PG_TEST2
(
   v_tablename         IN VARCHAR2 ,
   v_info              IN NUMBER,
   v_resultcode       IN OUT NUMBER ,
   v_resultinfo       IN OUT VARCHAR2
)
---------------------------------------------------------------------------------------

IS 
iv_table           VARCHAR2(3000);
iv_info            NUMBER;
iv_sql1             VARCHAR2(3000);
iv_sql2             VARCHAR2(3000);
iv_sql3             VARCHAR2(3000);
--iv_sql4             VARCHAR2(3000);
BEGIN
	v_resultcode:=-1;
	v_resultinfo:='start';
	iv_table:=v_tablename;
	iv_info:=v_info;
case iv_info
   when '100' then 
  -- iv_sql2:='UPDATE ||iv_table|| SET CUST_NAME=NVL(REPLACE(CUST_NAME,'','',''''),''无名'') WHERE CUST_NAME like ''%,%''';
  -- EXECUTE IMMEDIATE iv_sql2; 
      --iv_sql3:='alter table '||iv_table||' add constraint PK_'||iv_table||' primary key  (CUST_ID, PARTITION_ID)'||'using index'||' pctfree 10'||' initrans 10'||' maxtrans 255'||' tablespace TBS_ACT_IUSR1'||' local ('||' partition P_I_'||iv_table||'_0   initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_1  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_2  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_3  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_4  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5,'||' partition P_I_'||iv_table||'_5  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_6  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_7  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_8  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_9  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5'||')nologging';
   --EXECUTE IMMEDIATE iv_sql3;
   --iv_sql1:='alter table '||iv_table||' add constraint PK_'||iv_table||' primary key  (USER_ID, PARTITION_ID)'||'using index'||' pctfree 10'||' initrans 10'||' maxtrans 255'||' tablespace TBS_ACT_IUSR1'||' local ('||' partition P_I_'||iv_table||'_0   initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_1  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_2  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_3  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_4  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5,'||' partition P_I_'||iv_table||'_5  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_6  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_7  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_8  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_9  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5'||')nologging';
   --EXECUTE IMMEDIATE iv_sql1;
   --iv_sql2:='create index IDX_'||iv_table||'_1 on '||iv_table||' (SERIAL_NUMBER ASC)'||' pctfree 10'||' initrans 10'||' maxtrans 255'||' tablespace TBS_ACT_IUSR1'||' local ('||' partition P_I_'||iv_table||'_0   initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_1  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_2  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_3  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_4  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5,'||' partition P_I_'||iv_table||'_5  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_6  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_7  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_8  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_9  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5'||')||parallel'||' nologging';
  -- EXECUTE IMMEDIATE iv_sql2;
   --iv_sql3:='alter index IDX_'||iv_table||'_1  noparallel';
   --EXECUTE IMMEDIATE iv_sql3; 
   iv_sql1:='UPDATE '||iv_table||' SET CUST_NAME = replace(replace(CUST_NAME,chr(13),''''),CHR(10),''''),PSPT_ID = replace(replace(PSPT_ID,chr(13),''''),CHR(10),'''') WHERE INSTR(CUST_NAME,CHR(13))>0 OR INSTR(CUST_NAME,CHR(10))>0 OR INSTR(PSPT_ID,CHR(13))>0 OR INSTR(PSPT_ID,CHR(10))>0';
   EXECUTE IMMEDIATE iv_sql1;
   iv_sql2:='UPDATE '||iv_table||' SET CUST_NAME=NVL(REPLACE(CUST_NAME,'','',''''),''无名'') WHERE CUST_NAME like ''%,%''';
   EXECUTE IMMEDIATE iv_sql2;
   iv_sql3:='alter table '||iv_table||' add constraint PK_'||iv_table||' primary key  (CUST_ID, PARTITION_ID)'||'using index'||' pctfree 10'||' initrans 10'||' maxtrans 255'||' tablespace TBS_ACT_IUSR1'||' local ('||' partition P_I_'||iv_table||'_0   initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_1  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_2  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_3  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_4  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5,'||' partition P_I_'||iv_table||'_5  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR1,'||' partition P_I_'||iv_table||'_6  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR2,'||' partition P_I_'||iv_table||'_7  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR3,'||' partition P_I_'||iv_table||'_8  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR4,'||' partition P_I_'||iv_table||'_9  initrans 10 maxtrans 255 pctfree 10 tablespace TBS_ACT_IUSR5'||')nologging';
   EXECUTE IMMEDIATE iv_sql3;
   else   
      dbms_output.put_line('no find'||iv_table); 
end case;
commit;
  v_resultcode:=1;
	v_resultinfo:='end';
	return;
EXCEPTION
   WHEN OTHERS THEN
      v_resultinfo := 'err@end:'||SUBSTR(SQLERRM,1,250) ;
      ROLLBACK ;
      RETURN ;
END;
