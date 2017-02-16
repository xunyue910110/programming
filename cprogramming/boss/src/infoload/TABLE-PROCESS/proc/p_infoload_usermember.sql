CREATE OR REPLACE PROCEDURE P_INFOLOAD_USERMEMBER
(
  v_CHANNELNO         NUMBER(16) not null,
  v_filename          IN      VARCHAR2,
  v_resultcode        IN OUT  NUMBER,
  v_resultinfo        IN OUT  VARCHAR2
)
IS
  iv_sql        VARCHAR2(2000);
  iv_errCnt     NUMBER;
BEGIN
  --(1)获取里面modify_tag='8'的记录,对之前的该用户的所有记录进行清理
  --(1.1)获取modify_tag='8'的记录,并且记录其最大的变更流水信息
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERMEMBER A '||
            ' WHERE A.MODIFY_TAG = ''8'' '||
            ' AND A.CHANNELNO ='||v_CHANNELNO||
            ' AND EXISTS (SELECT 1 '||
            ' FROM TP_INFOLOAD_USERMEMBER B '||
            ' WHERE B.member_role_id = A.member_role_id '||
            ' AND B.MODIFY_TAG = ''8'' '||
            ' AND B.CHANNELNO ='||v_CHANNELNO||
            ' AND B.SYNC_SEQUENCE > A.SYNC_SEQUENCE) ' ;
  EXECUTE IMMEDIATE iv_sql;
  --(2)将modify_tag='1'和modify_tag='8'的记录在正式表中进行删除
   iv_sql := ' DELETE FROM TF_F_USER_MEMBER A '||
             ' WHERE (A.MEMBER_ROLE_ID, A.PARTITION_ID) IN '||
             ' (SELECT C.MEMBER_ROLE_ID, MOD(C.MEMBER_ROLE_ID, 10000) '||
             ' FROM TP_INFOLOAD_USERMEMBER C '||
             ' AND CHANNELNO ='||v_CHANNELNO||  
             ' WHERE C.MODIFY_TAG = ''8'') ' ;
  EXECUTE IMMEDIATE iv_sql;
  --(3)插入的记录--处理
  --(3.1)删除重复记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERMEMBER a '||
            ' WHERE EXISTS (SELECT 1 ' ||
            '    FROM TP_INFOLOAD_USERMEMBER b '||
            '   WHERE b.user_id=a.user_id ' ||
            '      AND b.member_role_code=a.member_role_code ' ||
            '      AND b.member_role_id=a.member_role_id '||
            '      AND b.member_role_number=a.member_role_number '||
            '      AND b.start_date=a.start_date '||
            '      AND b.rowid>a.rowid '||
            '       AND b.CHANNELNO ='||v_CHANNELNO||
            '      AND b.modify_tag in (''0'',''8'')) '||
            '    AND a.MODIFY_TAG IN (''0'',''8'')'
            '    AND a.CHANNELNO ='||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  COMMIT;

  --(3.2)
  iv_errCnt  := 0 ;
  <<lable>>
  BEGIN
    iv_sql := ' INSERT INTO TF_F_USER_MEMBER '||
              ' (PARTITION_ID,USER_ID,MEMBER_ROLE_CODE,MEMBER_ROLE_TYPE,MEMBER_ROLE_ID,MEMBER_ROLE_NUMBER, '||
              '  START_DATE,END_DATE,UPDATE_TIME,UPDATE_DEPART_ID,UPDATE_STAFF_ID,DISCNT_PRIORITY, '||
              ' SERIAL_NUMBER_A,ROLE_CODE_A) '||
              ' SELECT mod(a.MEMBER_ROLE_ID,10000),a.USER_ID,a.MEMBER_ROLE_CODE,a.MEMBER_ROLE_TYPE,a.MEMBER_ROLE_ID,a.MEMBER_ROLE_NUMBER, '||
              ' a.START_DATE,a.END_DATE,a.UPDATE_TIME,NULL,NULL,a.DISCNT_PRIORITY,a.SERIAL_NUMBER_A,a.ROLE_CODE_A '||
              ' FROM TP_INFOLOAD_USERMEMBER a '||
              ' WHERE a.MODIFY_TAG IN (''0'',''8'') ' ||
              ' AND a.CHANNELNO ='||v_CHANNELNO;
     EXECUTE IMMEDIATE iv_sql;
  EXCEPTION
    WHEN OTHERS THEN
       ROLLBACK;
      IF iv_errCnt = 3 THEN
       v_resultcode := -1 ;
       v_resultinfo := 'P_INFOLOAD_USER_MEMBER_2执行失败,错误信息:='||SQLERRM;
       return ;
      END IF;
      iv_errCnt := iv_errCnt + 1 ;    
      iv_sql := ' DELETE FROM TP_INFOLOAD_USERMEMBER a ' ||
                ' WHERE a.MODIFY_TAG IN (''0'',''8'') '||
                ' AND EXISTS (SELECT 1 '||
                '    FROM TF_F_USER_MEMBER b '||
                '    WHERE b.user_id=a.user_id '||
                '      AND b.member_role_code=a.member_role_code '||
                '      AND b.member_role_id=a.member_role_id '||
                '      AND b.member_role_number=a.member_role_number '||
                '      AND b.start_date=a.start_date '||
                '      AND b.CHANNELNO ='||v_CHANNELNO ||
                '   )'||
                ' AND a.CHANNELNO ='||v_CHANNELNO;
       EXECUTE IMMEDIATE iv_sql;                   
      GOTO lable;
  END;

  --(4)更新记录--处理
  --(4.1)前面处理过的删除、新增、全量记录删除，只保留更新记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERMEMBER ' ||
            ' WHERE MODIFY_TAG != ''2''  AND  CHANNELNO ='||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  --(4.2)获取同关键字的最后的一条更新操作记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERMEMBER a '||
            ' WHERE EXISTS (SELECT 1 ' ||
            '    FROM TP_INFOLOAD_USERMEMBER b '||
            '    WHERE b.user_id=a.user_id '||
            '      AND b.member_role_code=a.member_role_code '||
            '      AND b.member_role_id=a.member_role_id '||
            '      AND b.member_role_number=a.member_role_number '||
            '      AND b.start_date=a.start_date '||
            '      AND b.CHANNELNO ='||v_CHANNELNO 
            '      AND b.SYNC_SEQUENCE>a.SYNC_SEQUENCE) '||
            '   AND  a.CHANNELNO ='||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  --(4.3)关键字排重
  iv_sql := ' DELETE FROM TP_INFOLOAD_USERMEMBER a ' ||
            ' WHERE EXISTS (SELECT 1 ' ||
            '    FROM TP_INFOLOAD_USERMEMBER b' ||
            '    WHERE b.user_id=a.user_id ' ||
            '      AND b.member_role_code=a.member_role_code ' ||
            '      AND b.member_role_id=a.member_role_id ' || 
            '      AND b.member_role_number=a.member_role_number ' ||
            '      AND b.start_date=a.start_date ' ||
            '      AND b.CHANNELNO ='||v_CHANNELNO ||
            '      AND b.rowid>a.rowid) ' ||
            '   AND  a.CHANNELNO ='||v_CHANNELNO;
    EXECUTE IMMEDIATE iv_sql;
  --(4.4)按最后存在的记录对表进行更新操作
    iv_sql:=  ' MERGE /*+index (a,PK_TF_F_USER_MEMBER)*/ INTO TF_F_USER_MEMBER  a                    '||
              ' USING TP_INFOLOAD_USERMEMBER b                                                       '||
              '  ON (a.user_id = b.user_id AND a.member_role_code=b.member_role_code                 '||
              '     AND a.member_role_id=b.member_role_id                                            '||
              '     AND a.member_role_number = b.member_role_number AND a.start_date = b.start_date  '||
              '     AND b.CHANNELNO ='||v_CHANNELNO ||
              '     AND a.partition_id = mod(b.member_role_id,10000))                                '||
              '  WHEN MATCHED THEN                              '||
              '  UPDATE                                         '||
              '  SET a.member_role_type = b.member_role_type,   '||
              '      a.END_DATE = b.END_DATE,                   '||
              '      a.UPDATE_TIME = b.UPDATE_TIME,             '||
              '      a.SERIAL_NUMBER_A = b.SERIAL_NUMBER_A,     '||
              '      a.ROLE_CODE_A = b.ROLE_CODE_A              ';
    EXECUTE IMMEDIATE iv_sql;

  --(6)清空临时表
  iv_sql := 'DELETE FROM TP_INFOLOAD_USERMEMBER WHERE CHANNELNO ='||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;

  v_resultcode:=0;
  v_resultinfo:='P_INFOLOAD_USERMEMBER正确执行!';

EXCEPTION
  WHEN OTHERS THEN
    v_resultcode:=-1;
    v_resultinfo:='P_INFOLOAD_USERMEMBER执行失败,错误信息:='||SQLERRM;
    RETURN;
END;
/
