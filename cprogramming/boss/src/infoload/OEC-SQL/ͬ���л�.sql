CREATE OR REPLACE PROCEDURE UOP_ACT1.PG_SAME
(
   v_resultcode        OUT NUMBER ,
   
   v_resultinfo        OUT VARCHAR2 
)
---------------------------------------------------------------------------------------
AUTHID   CURRENT_USER
IS 
v_count  NUMBER;
v_sql    VARCHAR2(200); 
v_infotype NUMBER ;
v_usetable VARCHAR2(200);
cursor c is (select use_table,info_type  from ucr_param.td_b_table_loadcrmrule a where a.trigger_tag='1'and a.channel_no<>'1001'); 
BEGIN 
--v_usetable ucr_param.td_b_table_loadcrmrule.use_table%TYPE;
	v_resultinfo:='start';
	select count(1) into v_count from ucr_param.td_b_table_loadcrmrule a where a.trigger_tag<>'1' and a.channel_no<>'1001';
	if(v_count=0) then
	--同名切换
	v_resultinfo:='start synonms';
  begin
  open c;
  loop
     exit when c%FOUND ;
     fetch c into v_usetable,v_infotype;
	if(substr(v_usetable,-2)<>'_T') then
--	v_sql:='create or replace synonym uop_act1.same_customer for ucr_act1.tf_f_customer_t';
  v_sql:='create or replace synonym uop_act1.same_customer for ucr_act1.'||v_usetable||'_T';
  EXECUTE IMMEDIATE v_sql;
	v_resultinfo:='synonym successful '||v_usetable;
	--v_sql:='update ucr_param.td_b_table_loadcrmrule a set a.use_table=''TF_F_CUSTOMER'||'_T'''; 
  v_sql:='update ucr_param.td_b_table_loadcrmrule a set a.use_table='||''''||v_usetable||'_T'''||' where a.info_type='||v_infotype; 
  EXECUTE IMMEDIATE v_sql;
	v_resultinfo:='update successful'|| v_usetable; 
	else
	--v_sql:='create or replace synonym uop_act1.same_customer for ucr_act1.tf_f_customer'; 
  v_sql:='create or replace synonym uop_act1.same_customer for ucr_act1.'||v_usetable;
  EXECUTE IMMEDIATE v_sql; 
  v_resultinfo:='synonym successful '||v_usetable;
	--v_sql:='update ucr_param.td_b_table_loadcrmrule a set a.use_table=substr(a.use_table,1,length(a.use_table)-2)';  
  v_sql:='update ucr_param.td_b_table_loadcrmrule a set a.use_table=substr('||''''||v_usetable||''''||',1,length('||''''||v_usetable||''''||')-2)'||' where a.info_type='||v_infotype;
  EXECUTE IMMEDIATE v_sql;
  v_resultinfo:='update successful'|| v_usetable; 
	end if; 
  commit;
  end loop;
  close c;
  end;
	else
	  v_resultinfo:='not all trigger is 1';
	  return;
	end if;
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
