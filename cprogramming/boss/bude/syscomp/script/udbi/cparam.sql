insert into code_code(dao_name, tab_name, sql_ref, sql_stmt)
values('CPARAM', 'TD_S_CPARAM', 'INIT_LOAD', 'select key, value1, value2, vresult from td_s_cparam');
 
insert into code_code(dao_name, tab_name, sql_ref, sql_stmt)
values('CPARAM', 'TD_S_CPARAM', 'TAB200.STATE', 'select key, value1, value2, vresult from td_s_cparam where key=:vkey');

 
CREATE TABLE TD_S_CPARAM(
 KEY	VARCHAR2(30)	NOT NULL,
 VALUE1	VARCHAR2(30)	,
 VALUE2 VARCHAR2(30)	,
 VRESULT	VARCHAR2(60)
);



