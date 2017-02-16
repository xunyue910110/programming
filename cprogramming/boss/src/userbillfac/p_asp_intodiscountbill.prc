CREATE OR REPLACE PROCEDURE p_asp_intodiscountbill
(
    v_dealfile      IN      char,
    v_resultcode    IN OUT  NUMBER,
    v_resultinfo    IN OUT  VARCHAR2
)
-----------------------------------------------------------------------
--  v_dealfile:         处理文件名称
--  v_resultcode:       返回结果编码
--  v_resultinfo:       返回结果信息
-----------------------------------------------------------------------
IS
    iv_procedurename    VARCHAR2(50);
    iv_acycid           NUMBER(6);
    iv_month            CHAR(2);
    iv_partchannel      NUMBER(4);
--    iv_i                NUMBER(2);
    v_billchannel_no    NUMBER(4);
--    iv_channelno        CHAR(4);
--    iv_RatechannelNo    CHAR(2);
    iv_sql              VARCHAR2(2000);
    iv_detailbillname   VARCHAR2(50);
    iv_currdate         DATE;

-----------------------------------------------------------------------
--  iv_procedurename:   存储过程名称
--  iv_acycid:          帐务帐期标识
--  iv_channelno:       通道号
--  iv_cursor:          游标
--  iv_userid:          用户标识
--  iv_sql:             SQL语句
-----------------------------------------------------------------------
BEGIN

   Begin
        iv_procedurename:='p_asp_intodiscountbill';

        v_resultcode:=0;
        v_resultinfo:=iv_procedurename||' 存储过程正确执行!';


        iv_month:=SUBSTR(v_dealfile,6,2);
        iv_currdate:=SYSDATE;
        --获取帐务帐期标识
       iv_acycId:=to_number(substr(v_dealFile,2,6));

       --获取当前通道号
      v_billchannel_no:=to_number(SUBSTR(v_dealfile,9,2));

      iv_detailbillname:='TS_B_BDETAILINFO_'||TO_CHAR(iv_month);
   Exception
       When Others Then
            v_resultCode:=-1;
            v_resultinfo:=SUBSTR('error_001:'||SQLERRM,1,250);
            Return;
   End;

    ----删除用户明细帐单表TS_B_BDETAILINFO_XX
    --FOR iv_i IN 1..10 LOOP
    --BEGIN
    --iv_partchannel:=v_billchannel_no*10+iv_i;
    --iv_sql:='DELETE /*+ index(a ind_'||iv_detailbillname||'_id) */ FROM '||iv_detailbillname||' PARTITION (PAR_TS_B_BDETAILINFO_'||iv_partchannel||') a'||
    --        --'DELETE /*+ ordered use_nl(a b) */ FROM '||iv_detailbillname||' PARTITION (PAR_TS_B_BDETAILINFO_'||iv_partchannel||') a'||
    --        --' WHERE user_id IN'||
    --        --' (SELECT user_id'||
    --        --' FROM tm_a_smsdiscount b)';
    --        ' WHERE (id,id_type) IN '||
    --        ' (SELECT id,id_type    '||
    --        ' FROM TP_B_BDETAILINFO b)';
    --
    --
    --EXECUTE IMMEDIATE iv_sql;
    --
    --EXCEPTION
    --  WHEN OTHERS THEN
    --    ROLLBACK;
    --    v_resultCode:=-1;
    --    v_resultinfo:=SUBSTR('error_003:'||SQLERRM,1,250);
    --    RETURN;
    --END;
    --END LOOP;
    
    --修改了账单分拣程序，文件中的通道号和表的分区号对应，不需要删除10次
    BEGIN
      iv_partchannel:=v_billchannel_no+1;
      iv_sql:='DELETE /*+ index(a ind_'||iv_detailbillname||'_id) */ FROM '||iv_detailbillname||' PARTITION (PAR_TS_B_BDETAILINFO_'||iv_partchannel||') a'||            
              ' WHERE (id,id_type) IN '||
              ' (SELECT distinct id,id_type    '||
              ' FROM TP_B_BDETAILINFO b)';    
    
      EXECUTE IMMEDIATE iv_sql;
    
    EXCEPTION
      WHEN OTHERS THEN
        ROLLBACK;
        v_resultCode:=-1;
        v_resultinfo:=SUBSTR('error_003:'||SQLERRM,1,250);
        RETURN;
    END;

    BEGIN
        iv_sql:='DELETE FROM TP_B_BDETAILINFO WHERE  bill_type=''7'' AND detail_item_code=0 AND fee=0';
        EXECUTE IMMEDIATE iv_sql;
    EXCEPTION
        WHEN OTHERS THEN
            v_resultcode := -1;
            v_resultinfo := SUBSTR(iv_procedurename||'删除临时帐单错误:'||SQLERRM,1,250);
            RETURN;
    END;

    --插入用户明细帐单信息round(fee/10)
    Begin
        iv_sql:='INSERT INTO '||iv_detailbillname||
                ' (id,id_type,partition_id,USER_ID,cyc_id,'||
                'detail_item_code,fee,DISCNT_INS_ID,bill_type,ORDER_NO,ADJUST_INS_ID,update_time,FEEDISCNT_ID)'||
                --' SELECT id,ID_TYPE,mod(user_id,10000),user_id,:iv_acycid,'||
                ' SELECT id,ID_TYPE,mod(id,10000),user_id,:iv_acycid,'||
                ' detail_item_code,fee,discnt_id,bill_type,ORDER_NO,ADJUST_INS_ID,:iv_currdate,FEEDISCNT_ID'||
                ' FROM TP_B_BDETAILINFO';

        EXECUTE IMMEDIATE iv_sql USING iv_acycid,iv_currdate;
    EXCEPTION
        WHEN OTHERS THEN
            ROLLBACK;
            v_resultcode := -1;
            v_resultinfo := SUBSTR(iv_procedurename||'插入用户明细帐单错误:'||SQLERRM,1,250);
            RETURN;
    END;

    COMMIT;

EXCEPTION
    WHEN OTHERS THEN
        ROLLBACK;
        v_resultcode:=-1;
        v_resultinfo:=SUBSTR(iv_procedurename||'@END:'||SQLERRM,1,250);
        ROLLBACK;
        RETURN;
END;
/
