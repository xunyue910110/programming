create or replace procedure P_BPM_CLEAN_LCU_HISTORY(
       v_trade_name        IN      td_bpm_definition.name%TYPE, 
       v_version_all         IN      NUMBER,
       v_resultinfo	 		   OUT     VARCHAR2,
       v_resultcode	 		   OUT     NUMBER
-----------------------------------------------------------------------
-- @param:	v_trade_name 交易编码名 
-- @param:  v_version_all  是否所有版本都删除掉，1，所有，0 保留最新版本
-- @param: 	v_resultinfo  返回操作信息
-- @param: 	v_resultcode 	返回删除条数
-----------------------------------------------------------------------
)
IS      
      iv_def_name  VARCHAR2(50);  
      iv_max_version_id NUMBER;
      TYPE t_cursor IS ref CURSOR;
      iv_def_id    NUMBER(19,0);
      
      iv_cursor 	t_cursor;
begin
  --初始化
  iv_def_name:= TRIM(v_trade_name);
  v_resultinfo:='TradeOk';
  v_resultcode := 0;
  
	--取当前最大版本号
  BEGIN
		SELECT t.id	INTO iv_max_version_id
    from TD_BPM_DEFINITION t , TD_BPM_EXT_ATTRIBUTE ex where ex.name = 'category' and ex.value='lcu'  and ex.element= t.id and t.version_ = 
	   (select max(version_) from TD_BPM_DEFINITION t2 where t.name = iv_def_name and t2.name=t.name); 
	EXCEPTION
    WHEN NO_DATA_FOUND THEN
			 v_resultinfo:='交易编码不存在或没有历史版本!';
			 RETURN;
		WHEN OTHERS	THEN
			 v_resultinfo:='查交易编码异常	－'||sqlerrm;
			 RETURN;
	END;  
  
  if v_version_all > 0 THEN
     iv_max_version_id :=0;   
  END IF;
  
  --循环删除流程
	OPEN iv_cursor FOR   
      select t.ID
      from TD_BPM_DEFINITION t , TD_BPM_EXT_ATTRIBUTE ex 
      where ex.name = 'category' and ex.value='lcu'  
      and ex.element= t.id and t.name = iv_def_name;
	LOOP
		  FETCH iv_cursor INTO iv_def_id;
		  EXIT WHEN iv_cursor%NOTFOUND;
		
		  IF iv_def_id != iv_max_version_id THEN
          BEGIN
              --del action
              delete from Td_Bpm_Action  tt where tt.id in(select a.id from Td_Bpm_Action a ,td_bpm_node n where n.id = a.element and n.definition = iv_def_id);
              --del delegation
              delete from Td_Bpm_Delegation n where n.definition = iv_def_id;              
              --del node ext_attribute
              delete from Td_Bpm_Ext_Attribute tt where tt.id in (select ex.id from Td_Bpm_Ext_Attribute ex ,td_bpm_node n where n.id = ex.element and n.definition = iv_def_id);
              --del definition ext_attribute
              delete from Td_Bpm_Ext_Attribute tt where tt.id in (select ex.id from Td_Bpm_Ext_Attribute ex where ex.element = iv_def_id);
              --del node 
              delete from td_bpm_node n where  n.definition = iv_def_id;
              --del transion
              delete from td_bpm_transition n where  n.definition = iv_def_id;
              --del variable
              delete from td_bpm_variable n  where  n.definition = iv_def_id;
              --del definition
              delete from td_bpm_definition n where  n.id = iv_def_id;              
              v_resultcode:=v_resultcode + 1;
              
        	EXCEPTION
        		WHEN OTHERS	THEN
        			v_resultinfo:='删除交易编码失败:'|| iv_def_name ||SQLERRM;
              ROLLBACK;
        			RETURN;
        	END;         
          
      END IF;      
     
	END LOOP;
	CLOSE iv_cursor;
  v_resultinfo:='成功清理交易编码:'|| iv_def_name ;
  commit;
      
  
end P_BPM_CLEAN_LCU_HISTORY;
/
