CREATE OR REPLACE PROCEDURE P_INFOLOAD_USERSERVSTATE
(
  v_CHANNELNO         IN      NUMBER ,
  v_filename          IN      VARCHAR2,
  v_resultcode        IN OUT  NUMBER,
  v_resultinfo        IN OUT  VARCHAR2
)
IS
  iv_sql                  VARCHAR2(2000);
  iv_errCnt               NUMBER;
BEGIN
  --(1)获取里面modify_tag='8'的记录,对之前的该用户的所有记录进行清理
  --(1.1)获取modify_tag='8'的记录,并且记录其最大的变更流水信息
   iv_sql := ' DELETE FROM TP_INFOLOAD_USERSERVSTATE A '||
             ' WHERE A.MODIFY_TAG = ''8'' and CHANNELNO ='||v_CHANNELNO||
             ' AND EXISTS (SELECT 1 '||
             ' FROM TP_INFOLOAD_USERSERVSTATE B '||
             ' WHERE B.USER_ID = A.USER_ID '||
             ' AND B.MODIFY_TAG = ''8'' '||
             ' AND B.SYNC_SEQUENCE > A.SYNC_SEQUENCE and CHANNELNO ='||v_CHANNELNO||') ';
   EXECUTE IMMEDIATE iv_sql;
  --(2)将modify_tag='1'和modify_tag='8'的记录在正式表中进行删除
   iv_sql := 'DELETE  FROM TF_F_USER_SERVSTATE a '||
             'WHERE (a.user_id,a.partition_id) IN (SELECT b.user_id,mod(b.user_id,10000)'||
                                           ' from TP_INFOLOAD_USERSERVSTATE b '||
                                           ' WHERE b.MODIFY_TAG = ''8'' '||
                                           ' and b.CHANNELNO ='||v_CHANNELNO||') ';
   EXECUTE  IMMEDIATE iv_sql;
  --(3)插入的记录--处理
  --(3.1)删除重复记录
   iv_sql :=' DELETE FROM TP_INFOLOAD_USERSERVSTATE a '||
            ' WHERE EXISTS (SELECT 1 ' ||
            ' FROM TP_INFOLOAD_USERSERVSTATE b '||
            ' WHERE b.user_id=a.user_id '||
            '      AND b.serv_id=a.serv_id ' ||
            '      AND b.SERV_STATE_CODE = a.SERV_STATE_CODE '||
            '      AND b.start_date=a.start_date '||
            '      AND b.rowid>a.rowid '||
            '      AND b.modify_tag in (''0'',''8'') '||
            '      AND CHANNELNO = '||v_CHANNELNO||' ) '||
            ' AND CHANNELNO = '||v_CHANNELNO||
            ' AND a.MODIFY_TAG IN (''0'',''8'') ' ;
   EXECUTE IMMEDIATE iv_sql;
   COMMIT;
  --(3.2)
  iv_errCnt := 0 ;
  <<lable>>
  BEGIN
    iv_sql := ' INSERT INTO TF_F_USER_SERVSTATE ' ||
              ' (PARTITION_ID,SERV_INS_ID,USER_ID,SERV_ID,MAIN_TAG,SERV_STATE_CODE,START_DATE,END_DATE,'||
              '  UPDATE_TIME,UPDATE_DEPART_ID,UPDATE_STAFF_ID) '||
              ' SELECT mod(a.user_id,10000),nvl(a.SERV_INS_ID,0),a.USER_ID,a.SERV_ID,a.MAIN_TAG,a.SERV_STATE_CODE,a.START_DATE,a.END_DATE, '||
              ' a.UPDATE_TIME,NULL,NULL '||
              ' FROM TP_INFOLOAD_USERSERVSTATE a ' ||
              ' WHERE a.MODIFY_TAG IN (''0'',''8'') '||
              ' AND CHANNELNO = '||v_CHANNELNO ;
    EXECUTE IMMEDIATE iv_sql;
  EXCEPTION
    WHEN OTHERS THEN
       ROLLBACK;
      IF iv_errCnt = 3 THEN
       v_resultcode := -1 ;
       v_resultinfo := 'P_INFOLOAD_USER_SERVSTATE_2执行失败,错误信息:='||SQLERRM;
       return ;
      END IF;
      iv_errCnt := iv_errCnt + 1 ; 
       iv_sql := ' DELETE FROM TP_INFOLOAD_USERSERVSTATE a ' ||
                 ' WHERE a.MODIFY_TAG IN (''0'',''8'') '||
                 ' AND EXISTS (SELECT /*+ index(b,PK_TF_F_USER_SERVSTATE)*/  1 '||
                 '    FROM TF_F_USER_SERVSTATE b '||
                 '    WHERE b.user_id=a.user_id '||
                 '      AND b.serv_id=a.serv_id '||
                 '      AND b.SERV_STATE_CODE = a.SERV_STATE_CODE '||
                 '      AND b.start_date=a.start_date '||
                 '      AND b.partition_id=mod(a.user_id,10000) '||
                 '      AND CHANNELNO = '||v_CHANNELNO||
                 '   ) '||
                 ' AND CHANNELNO = '||v_CHANNELNO;
                 EXECUTE IMMEDIATE iv_sql;
       GOTO lable;
  END;

  --(4)更新记录--处理
  --(4.1)前面处理过的删除、新增、全量记录删除，只保留更新记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERSERVSTATE '||
            ' WHERE MODIFY_TAG != ''2'' AND CHANNELNO = '||v_CHANNELNO ;
  EXECUTE IMMEDIATE iv_sql;

  --(4.2)获取同关键字的最后的一条更新操作记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERSERVSTATE a '||
            ' WHERE EXISTS (SELECT 1 '||
            '    FROM TP_INFOLOAD_USERSERVSTATE b '||
            '    WHERE b.user_id=a.user_id '||
            '      AND b.serv_id=a.serv_id '||
            '      AND b.SERV_STATE_CODE = a.SERV_STATE_CODE '||
            '      AND b.start_date=a.start_date '||
            '      AND b.SYNC_SEQUENCE>a.SYNC_SEQUENCE '||
            '      AND b.CHANNELNO = '||v_CHANNELNO||
            '      ) '||
            ' AND CHANNELNO = '||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  --(4.3)关键字排重
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERSERVSTATE a ' ||
            ' WHERE EXISTS (SELECT 1 '||
            '    FROM TP_INFOLOAD_USERSERVSTATE b '||
            '    WHERE b.user_id=a.user_id '||
            '      AND b.serv_id=a.serv_id '||
            '      AND b.SERV_STATE_CODE = a.SERV_STATE_CODE '||
            '      AND b.start_date=a.start_date '||
            '      AND b.CHANNELNO = '||v_CHANNELNO ||
            '      AND b.rowid>a.rowid) '||
            '  AND a.CHANNELNO = '||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.4)按最后存在的记录对表进行更新操作
    iv_sql:=  ' MERGE /*+index (a,PK_TF_F_USER_SERVSTATE)*/ INTO TF_F_USER_SERVSTATE  a  '||
              ' USING TP_INFOLOAD_USERSERVSTATE b                                        '||
              '  ON (a.user_id = b.user_id AND a.serv_id=b.serv_id                       '||
              '  AND a.serv_state_code = b.serv_state_code AND a.start_date=b.start_date '||
              '  AND b.CHANNELNO = '||v_CHANNELNO ||
              '  AND a.partition_id = mod(b.user_id,10000) )                             '||              
              '  WHEN MATCHED THEN                         '||
              '  UPDATE                                    '||
              '  SET a.SERV_INS_ID = nvl(b.SERV_INS_ID,0), '||
              '  a.MAIN_TAG = b.MAIN_TAG,                  '||
              '  a.END_DATE = b.END_DATE,                  '||
              '  a.UPDATE_TIME = b.UPDATE_TIME              ';
    EXECUTE IMMEDIATE iv_sql;


  --(5)清空临时表
  iv_sql := 'DELETE FROM TP_INFOLOAD_USERSERVSTATE WHERE CHANNELNO = '||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;

  v_resultcode:=0;
  v_resultinfo:='P_INFOLOAD_USERSERVSTATE正确执行!';

EXCEPTION
  WHEN OTHERS THEN
    v_resultcode:=-1;
    v_resultinfo:='P_INFOLOAD_USERSERVSTATE执行失败,错误信息:='||SQLERRM;
    RETURN;
END;
