create or replace procedure P_BPM_CLEAN_LCU_BY_PATH(
       v_dir_path        IN      VARCHAR2, 
       v_version_all     IN      NUMBER,
       v_resultinfo	 		   OUT     VARCHAR2,
       v_resultcode	 		   OUT     NUMBER
-----------------------------------------------------------------------
-- 统计每个目录下流程的数量：
-- select distinct(value),count(value) from td_bpm_ext_attribute where name = 'path' group by value;
-- @param:	v_dir_name     目录名   例：输入"/LCU测试"
-- @param:  v_version_all  是否所有版本都删除掉，1，所有，0 保留最新版本
-- @param: 	v_resultinfo   返回操作信息
-- @param: 	v_resultcode 	 返回删除条数
-----------------------------------------------------------------------
) is     
      TYPE t_cursor IS ref CURSOR;     
      iv_cursor t_cursor;
      iv_count NUMBER;
      iv_result_count NUMBER;
      iv_def_name  VARCHAR2(50); 
begin
  v_resultcode	:= 0;
  v_resultinfo  := '';
  iv_result_count:=0;
  iv_count :=0;
  
	--取当前最大版本号
  BEGIN
      select count(ex.element) into iv_result_count
      from TD_BPM_EXT_ATTRIBUTE ex 
      where name = 'path' and value = v_dir_path; 
	EXCEPTION
    WHEN NO_DATA_FOUND THEN
			 v_resultinfo:='目录不存在!';
			 RETURN;
		WHEN OTHERS	THEN
			 v_resultinfo:='查询异常	－'||sqlerrm;
			 RETURN;
	END;  


  BEGIN
       OPEN iv_cursor FOR  
            select distinct(t.name) from TD_BPM_DEFINITION t 
            where t.id in (select ex.element from TD_BPM_EXT_ATTRIBUTE ex 
            where name = 'path' and value = v_dir_path );
        LOOP
             
            FETCH iv_cursor INTO iv_def_name;
            EXIT WHEN iv_cursor%NOTFOUND;
                
              begin
              -- Call the procedure
              p_bpm_clean_lcu_history(iv_def_name,
                                      v_version_all,
                                      v_resultinfo,
                                      v_resultcode);   
                                          
              iv_count := iv_count + v_resultcode;
              
             	EXCEPTION
                		WHEN OTHERS	THEN
                			v_resultinfo:='删除交易编码失败:'|| iv_def_name ||SQLERRM;
                      ROLLBACK;
                			RETURN;
              END;  
           
        END LOOP;
        
        CLOSE iv_cursor;
        v_resultinfo:='成功目录清理.目录['|| v_dir_path || ']含交易编码:'|| iv_result_count || '  已经删除旧流程条数:'|| iv_count ;
      
  	END; 
    v_resultcode:= iv_count;
end P_BPM_CLEAN_LCU_BY_PATH;
/
