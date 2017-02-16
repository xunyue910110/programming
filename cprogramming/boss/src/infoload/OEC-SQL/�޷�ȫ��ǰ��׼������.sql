CREATE OR REPLACE PROCEDURE PG_TEST
(
   v_tablename         IN VARCHAR2 ,
   v_info              IN NUMBER,
   v_resultcode        OUT NUMBER ,
   v_resultinfo        OUT VARCHAR2
  
)
---------------------------------------------------------------------------------------

IS 
iv_table            VARCHAR2(3000);
iv_info             NUMBER;
iv_sql1             VARCHAR2(3000);
iv_sql2             VARCHAR2(3000);
--iv_sql3             VARCHAR2(3000);
--iv_sql4             VARCHAR2(3000);
BEGIN
	v_resultcode:=-1;
	v_resultinfo:='start';
	iv_table:=v_tablename;
	iv_info:=v_info;
case iv_info
   when '100' then 
   iv_sql1:='TRUNCATE TABLE '||iv_table;
   EXECUTE IMMEDIATE iv_sql1;
   iv_sql2:='ALTER TABLE '||iv_table||' DROP CONSTRAINT PK_'||iv_table;
   EXECUTE IMMEDIATE iv_sql2;
   --iv_sql1:='TRUNCATE TABLE '||iv_table;
   --for i in 0..6
   --loop 
   --iv_sql1:='select * from '||iv_table;
   --EXECUTE IMMEDIATE iv_sql1;
   --dbms_output.put_line('start'||i);
   --end loop; 
   --iv_sql1:='TRUNCATE TABLE '||iv_table;
   --EXECUTE IMMEDIATE iv_sql1;
   --iv_sql2:='ALTER TABLE '||iv_table||' DROP CONSTRAINT PK_'||iv_table;
   --EXECUTE IMMEDIATE iv_sql2;
   --iv_sql3:='DROP INDEX IDX_'||iv_table||'_1';
   --EXECUTE IMMEDIATE iv_sql3;
   /*when '102' then 
   for i in 0..6
   loop 
   iv_sql1:='select * from '||iv_table;
   EXECUTE IMMEDIATE iv_sql1;
   dbms_output.put_line('start'||i);
   end loop;
   --iv_sql2:='ALTER TABLE '||iv_table||' DROP CONSTRAINT PK_'||iv_table;
   --EXECUTE IMMEDIATE iv_sql2; */
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
