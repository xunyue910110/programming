CREATE OR REPLACE PROCEDURE P_INFOLOAD_USER
(
  v_CHANNELNO         IN      NUMBER ,
  v_filename          IN      VARCHAR2,
  v_resultcode        IN OUT  NUMBER,
  v_resultinfo        IN OUT  VARCHAR2
)
IS
  iv_sql        VARCHAR2(2000);
BEGIN
  --(1)获取里面modify_tag='8'的记录,对之前的该用户的所有记录进行清理
  --(1.1)获取modify_tag='8'的记录,并且记录其最大的变更流水信息
   iv_sql := ' DELETE FROM TP_INFOLOAD_USER A '||
             ' WHERE A.MODIFY_TAG = ''8'' '||
             ' AND EXISTS (SELECT 1 '||
             ' FROM TP_INFOLOAD_USER B '||
             ' WHERE B.user_id = A.user_id '||
             '  AND B.MODIFY_TAG = ''8'' '||
             '  AND B.CHANNELNO = '||v_CHANNELNO||
             '  AND B.SYNC_SEQUENCE > A.SYNC_SEQUENCE) ' ||
             '  AND A.CHANNELNO = '||v_CHANNELNO  ;
   EXECUTE IMMEDIATE iv_sql;
  --(2)将modify_tag='1'和modify_tag='8'的记录在正式表中进行删除
  iv_sql := ' DELETE FROM TF_F_USER A '||
            ' WHERE (A.USER_ID,A.PARTITION_ID) IN '||
            ' (SELECT C.USER_ID,MOD(C.USER_ID, 10000) '||
            ' FROM TP_INFOLOAD_USER C '||
            ' WHERE C.MODIFY_TAG = ''8'' ' ||
            '  AND C.CHANNELNO = '||v_CHANNELNO||
            ' ) ';
   EXECUTE IMMEDIATE iv_sql;
  --(3)插入的记录--处理
  --(3.1)删除重复记录
    iv_sql := ' DELETE FROM TP_INFOLOAD_USER a '||
              ' WHERE EXISTS (SELECT 1 '||
              ' FROM TP_INFOLOAD_USER b '||
              '  WHERE b.user_id=a.user_id '||
              '    AND b.rowid>a.rowid '||
              '    AND b.CHANNELNO = '||v_CHANNELNO||
              '    AND b.modify_tag in (''0'',''8'') '||
              '    ) '||
              '  AND a.modify_tag in (''0'',''8'') '||
              '  AND CHANNELNO = '||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  COMMIT;

  --(3.2) 应该加个判断，可能死循环
  <<lable>>
  BEGIN
    iv_sql := ' INSERT INTO TF_F_USER  '||
              ' (PARTITION_ID,USER_ID,DUMMY_TAG,NET_TYPE_CODE,SERIAL_NUMBER,EPARCHY_CODE,CITY_CODE,CUST_ID, '||
              '  USECUST_ID,BRAND_CODE,PRODUCT_ID,USER_TYPE_CODE,PREPAY_TAG,SERVICE_STATE_CODE,OPEN_MODE,ACCT_TAG, '||
              '  REMOVE_TAG,IN_DATE,OPEN_DATE,PRE_DESTROY_DATE,DESTROY_DATE,FIRST_CALL_DATE,LAST_STOP_DATE, '||
              '  CREDIT_CLASS,BASE_CREDIT_VALUE,CREDIT_VALUE,CREDIT_CONTROL_ID,CHANGEUSER_DATE ,SCORE_VALUE , '||
              '  UPDATE_TIME ,USER_PASSWD ) '||
              ' SELECT mod(a.USER_ID,10000),a.USER_ID,a.DUMMY_TAG,a.NET_TYPE_CODE,a.SERIAL_NUMBER,a.EPARCHY_CODE,a.CITY_CODE,a.CUST_ID, '||
              ' a.USECUST_ID,a.BRAND_CODE,a.PRODUCT_ID,a.USER_TYPE_CODE,a.PREPAY_TAG,a.SERVICE_STATE_CODE,a.OPEN_MODE,ACCT_TAG, '||
              ' a.REMOVE_TAG,a.IN_DATE,a.OPEN_DATE,a.PRE_DESTROY_DATE,a.DESTROY_DATE,a.FIRST_CALL_DATE,LAST_STOP_DATE, '||
              ' a.CREDIT_CLASS,a.BASE_CREDIT_VALUE,a.CREDIT_VALUE,a.CREDIT_CONTROL_ID,a.CHANGEUSER_DATE,a.SCORE_VALUE, '||
              ' a.UPDATE_TIME,a.USER_PASSWD '||
              ' FROM TP_INFOLOAD_USER a '||
              ' WHERE a.MODIFY_TAG IN (''0'',''8'') '||
              ' AND CHANNELNO = '||v_CHANNELNO;
     EXECUTE IMMEDIATE iv_sql;
  EXCEPTION
    WHEN OTHERS THEN
      ROLLBACK;

      iv_sql := ' DELETE FROM TP_INFOLOAD_USER a '||
                ' WHERE a.modify_tag in (''0'',''8'') '||
                ' AND EXISTS (SELECT 1 '||
                '     FROM TF_F_USER b '||
                '     WHERE b.USER_ID=a.USER_ID '||
                '    ) '||
                ' AND CHANNELNO = '||v_CHANNELNO;
      EXECUTE IMMEDIATE iv_sql;
       GOTO lable;
  END;

  --(4)更新记录--处理
  --(4.1)前面处理过的删除、新增、全量记录删除，只保留更新记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_USER '||
            ' WHERE MODIFY_TAG != ''2'' '||
            ' AND CHANNELNO = '||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  --(4.2)获取同关键字的最后的一条更新操作记录
    iv_sql := ' DELETE FROM TP_INFOLOAD_USER a '||
              ' WHERE EXISTS (SELECT 1 ' ||
              '  FROM TP_INFOLOAD_USER b ' ||
              '  WHERE b.USER_ID=a.USER_ID '||
              ' AND b.CHANNELNO = '||v_CHANNELNO||
              '    AND b.SYNC_SEQUENCE>a.SYNC_SEQUENCE) '||
              ' AND a.CHANNELNO = '||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.3)关键字排重
  iv_sql := ' DELETE FROM TP_INFOLOAD_USER a '||
            ' WHERE EXISTS (SELECT 1 '||
            '    FROM TP_INFOLOAD_USER b '||
            '    WHERE b.USER_ID=a.USER_ID '||
            '          AND b.CHANNELNO = '||v_CHANNELNO||
            '       AND b.rowid>a.rowid)'||
            ' AND a.CHANNELNO = '||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.4)按最后存在的记录对表进行更新操作
    iv_sql:=  ' MERGE /*+index (a,PK_TF_F_USER)*/ INTO TF_F_USER a  '||
              ' USING TP_INFOLOAD_USER b                  '||
              '  ON (a.USER_ID = b.USER_ID   '||
              '  AND b.CHANNELNO = '||v_CHANNELNO ||
              '  AND a.partition_id = mod(b.USER_ID,10000))         '||
              '  WHEN MATCHED THEN                             '||
              '  UPDATE                                        '||
              '  SET             '||
              ' a.DUMMY_TAG         = b.DUMMY_TAG           ,'||
              ' a.NET_TYPE_CODE     = b.NET_TYPE_CODE       ,'||
              ' a.SERIAL_NUMBER     = b.SERIAL_NUMBER       ,'||
              ' a.EPARCHY_CODE      = b.EPARCHY_CODE        ,'||
              ' a.CITY_CODE         = b.CITY_CODE           ,'||
              ' a.CUST_ID           = b.CUST_ID             ,'||
              ' a.USECUST_ID        = b.USECUST_ID          ,'||
              ' a.PRODUCT_ID        = b.PRODUCT_ID          ,'||
              ' a.BRAND_CODE        = b.BRAND_CODE          ,'||
              ' a.USER_TYPE_CODE    = b.USER_TYPE_CODE      ,'||
              ' a.PREPAY_TAG        = b.PREPAY_TAG          ,'||
              ' a.SERVICE_STATE_CODE= b.SERVICE_STATE_CODE  ,'||
              ' a.OPEN_MODE         = b.OPEN_MODE           ,'||
              ' a.ACCT_TAG          = b.ACCT_TAG            ,'||
              ' a.REMOVE_TAG        = b.REMOVE_TAG          ,'||
              ' a.IN_DATE           = b.IN_DATE             ,'||
              ' a.OPEN_DATE         = b.OPEN_DATE           ,'||
              ' a.PRE_DESTROY_DATE  = b.PRE_DESTROY_DATE    ,'||
              ' a.DESTROY_DATE      = b.DESTROY_DATE        ,'||
              ' a.FIRST_CALL_DATE   = b.FIRST_CALL_DATE     ,'||
              ' a.LAST_STOP_DATE    = b.LAST_STOP_DATE      ,'||
              ' a.CREDIT_CLASS      = b.CREDIT_CLASS        ,'||
              ' a.BASE_CREDIT_VALUE = b.BASE_CREDIT_VALUE   ,'||
              ' a.CREDIT_VALUE      = b.CREDIT_VALUE        ,'||
              ' a.CREDIT_CONTROL_ID = b.CREDIT_CONTROL_ID   ,'||
              ' a.CHANGEUSER_DATE   = b.CHANGEUSER_DATE     ,'||
              ' a.SCORE_VALUE       = b.SCORE_VALUE         ,'||
              ' a.UPDATE_TIME       = b.UPDATE_TIME         ,'||
              ' a.USER_PASSWD       = b.USER_PASSWD          ';
    EXECUTE IMMEDIATE iv_sql;

  --(5)清空临时表
  iv_sql := 'DELETE FROM TP_INFOLOAD_USER WHERE CHANNELNO = '||v_CHANNELNO ;
  EXECUTE IMMEDIATE iv_sql;

  v_resultcode:=0;
  v_resultinfo:='P_INFOLOAD_USER正确执行!';

EXCEPTION
  WHEN OTHERS THEN
    v_resultcode:=-1;
    v_resultinfo:='P_INFOLOAD_USER执行失败,错误信息:='||SQLERRM;
    RETURN;
END;
/
