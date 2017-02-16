CREATE OR REPLACE PROCEDURE P_INFOLOAD_IMPORTINFO
(
  v_CHANNELNO         NUMBER  ,
  v_filename          IN      VARCHAR2,
  v_resultcode        IN OUT  NUMBER,
  v_resultinfo        IN OUT  VARCHAR2
)
IS
  iv_sql        VARCHAR2(2000);
BEGIN
  --(1)获取里面modify_tag='8'的记录,对之前的该用户的所有记录进行清理
  --(1.1)获取modify_tag='8'的记录,并且记录其最大的变更流水信息
  iv_sql :=' DELETE FROM TP_INFOLOAD_IMPORTINFO A '||
           ' WHERE A.MODIFY_TAG = ''8'' '||
           ' AND A.CHANNELNO ='||v_CHANNELNO||
           ' AND EXISTS (SELECT 1 '||
           ' FROM TP_INFOLOAD_IMPORTINFO B '||
           ' WHERE B.USER_ID = A.USER_ID '||
           '  AND B.MODIFY_TAG = ''8'' '||
           '  AND B.CHANNELNO ='||v_CHANNELNO||
           '  AND B.SYNC_SEQUENCE > A.SYNC_SEQUENCE)';
  EXECUTE IMMEDIATE iv_sql;
  --(2)将modify_tag='1'和modify_tag='8'的记录在正式表中进行删除
  iv_sql :=' DELETE FROM TF_F_USER_IMPORTINFO A '||
           ' WHERE (A.USER_ID, A.PARTITION_ID) IN '||
           ' (SELECT C.USER_ID, MOD(C.USER_ID, 10000) '||
           ' FROM TP_INFOLOAD_IMPORTINFO C '||
           ' WHERE CHANNELNO ='||v_CHANNELNO||
           ' AND C.MODIFY_TAG = ''8'' )';
  EXECUTE IMMEDIATE iv_sql;
  --(3)插入的记录--处理
  --(3.1)删除重复记录
  iv_sql :='   DELETE FROM TP_INFOLOAD_IMPORTINFO a '||
           '   WHERE EXISTS (SELECT 1 '||
           '     FROM TP_INFOLOAD_IMPORTINFO b '||
           '     WHERE b.user_id=a.user_id '||
           '       AND b.start_date=a.start_date '||
           '       AND b.rowid>a.rowid '||
           '       AND b.CHANNELNO ='||v_CHANNELNO||
           '       AND b.modify_tag in (''0'',''8'') '||
           '  ) '||
           '       AND a.CHANNELNO ='||v_CHANNELNO||
           '    AND a.modify_tag in (''0'',''8'')' ;
  EXECUTE IMMEDIATE iv_sql;
  COMMIT;

  --(3.2)
  <<lable>>
  BEGIN
    iv_sql :=' INSERT INTO TF_F_USER_IMPORTINFO ' ||
             ' (PARTITION_ID,USER_ID,NET_TYPE_CODE,PRODUCT_ID,BRAND_CODE,LOGIC_PHONE,PHYICAL_PHONE,'||
             '  START_DATE,END_DATE,UPDATE_TIME,UPDATE_DEPART_ID,UPDATE_STAFF_ID) '||
             '  SELECT mod(a.user_id,10000),a.USER_ID,a.NET_TYPE_CODE,a.PRODUCT_ID,a.BRAND_CODE,a.LOGIC_PHONE,a.PHYICAL_PHONE, '||
             '  a.START_DATE,a.END_DATE,a.UPDATE_TIME,NULL,NULL '||
             '  FROM TP_INFOLOAD_IMPORTINFO a '||
             '  WHERE a.MODIFY_TAG IN (''0'',''8'') ' ||
             '  AND  a.CHANNELNO ='||v_CHANNELNO;
             EXECUTE IMMEDIATE iv_sql;
  EXCEPTION
    WHEN OTHERS THEN
       ROLLBACK;

        iv_sql :=' DELETE FROM TP_INFOLOAD_IMPORTINFO a ' ||
                 ' WHERE a.modify_tag in (''0'',''8'') '||
                 ' AND EXISTS (SELECT 1 ' ||
                 '      FROM TF_F_USER_IMPORTINFO b '||
                 '      WHERE b.user_id=a.user_id '||
                 '      AND a.CHANNELNO ='||v_CHANNELNO||
                 '        AND b.start_date=a.start_date '||
                 '      )' ||
                 ' AND a.CHANNELNO ='||v_CHANNELNO;
       EXECUTE IMMEDIATE iv_sql;
       GOTO lable;
   END;

  --(4)更新记录--处理
  --(4.1)前面处理过的删除、新增、全量记录删除，只保留更新记录
  iv_sql :=' DELETE FROM TP_INFOLOAD_IMPORTINFO ' ||
           ' WHERE MODIFY_TAG != ''2'' '||
           ' AND CHANNELNO ='||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.2)获取同关键字的最后的一条更新操作记录
  iv_sql :=' DELETE FROM TP_INFOLOAD_IMPORTINFO a '||
           ' WHERE EXISTS (SELECT 1 '||
           '     FROM TP_INFOLOAD_IMPORTINFO b '||
           '     WHERE b.user_id=a.user_id '||
           '       AND b.start_date=a.start_date '||
           '       AND b.CHANNELNO ='||v_CHANNELNO||
           '       AND b.SYNC_SEQUENCE>a.SYNC_SEQUENCE) ' ||
           '    AND a.CHANNELNO ='||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.3)关键字排重
  iv_sql :=' DELETE FROM TP_INFOLOAD_IMPORTINFO a ' ||
           ' WHERE EXISTS (SELECT 1 '||
           '     FROM TP_INFOLOAD_IMPORTINFO b '||
           '     WHERE b.user_id=a.user_id '||
           '       AND b.start_date=a.start_date '||
           '       AND b.CHANNELNO ='||v_CHANNELNO||
           '       AND b.rowid>a.rowid) '||
           '    AND a.CHANNELNO ='||v_CHANNELNO;
 EXECUTE IMMEDIATE iv_sql;
  --(4.4)按最后存在的记录对表进行更新操作
 iv_sql:=  ' MERGE /*+index (a,PK_TF_F_USER_IMPORTINFO)*/ INTO TF_F_USER_IMPORTINFO a '||
              ' USING TP_INFOLOAD_IMPORTINFO b                                           '||
              '  ON (a.user_id = b.user_id AND a.start_date=b.start_date                 '||
              '  AND b.CHANNELNO ='||v_CHANNELNO||
              '  AND a.partition_id = MOD(b.user_id,10000) ) '||
              '  WHEN MATCHED THEN                          '||
              '  UPDATE                                    '||
              '  SET a.NET_TYPE_CODE = b.NET_TYPE_CODE,    '||
              '      a.PRODUCT_ID = b.PRODUCT_ID,          '||
              '      a.BRAND_CODE = b.BRAND_CODE,          '||
              '      a.LOGIC_PHONE = b.LOGIC_PHONE,        '||
              '      a.PHYICAL_PHONE = b.PHYICAL_PHONE,    '||
              '      a.END_DATE = b.END_DATE,              '||
              '      a.UPDATE_TIME = b.UPDATE_TIME          ';
    EXECUTE IMMEDIATE iv_sql;

  --(6)清空临时表
  iv_sql := 'DELETE FROM TP_INFOLOAD_IMPORTINFO WHERE CHANNELNO ='||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;

  v_resultcode:=0;
  v_resultinfo:='P_INFOLOAD_IMPORTINFO正确执行!';

EXCEPTION
  WHEN OTHERS THEN
    v_resultcode:=-1;
    v_resultinfo:='P_INFOLOAD_IMPORTINFO执行失败,错误信息:='||SQLERRM;
    RETURN;
END;
