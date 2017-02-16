CREATE OR REPLACE PROCEDURE P_INFOLOAD_USER_SP
(
  v_CHANNELNO         IN      NUMBER ,
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
   iv_sql := ' DELETE FROM TP_INFOLOAD_USER_SP A '||
             ' WHERE A.MODIFY_TAG = ''8'' '||
             ' AND EXISTS (SELECT 1 '||
             ' FROM TP_INFOLOAD_USER_SP B '||
             ' WHERE B.user_id = A.user_id '||
             '  AND B.MODIFY_TAG = ''8'' '||
             '  AND B.CHANNELNO = '||v_CHANNELNO||
             '  AND B.SYNC_SEQUENCE > A.SYNC_SEQUENCE) ' ||
             '  AND A.CHANNELNO = '||v_CHANNELNO  ;
   EXECUTE IMMEDIATE iv_sql;
  --(2)将modify_tag='1'和modify_tag='8'的记录在正式表中进行删除
  iv_sql := ' DELETE FROM TF_F_USER_SP A '||
            ' WHERE (A.USER_ID,A.PARTITION_ID) IN '||
            ' (SELECT C.USER_ID,MOD(C.USER_ID, 10000) '||
            ' FROM TP_INFOLOAD_USER_SP C '||
            ' WHERE C.MODIFY_TAG = ''8'' ' ||
            '  AND C.CHANNELNO = '||v_CHANNELNO||
            ' ) ';
   EXECUTE IMMEDIATE iv_sql;
  --(3)插入的记录--处理
  --(3.1)删除重复记录
    iv_sql := ' DELETE FROM TP_INFOLOAD_USER_SP a '||
              ' WHERE EXISTS (SELECT 1 '||
              ' FROM TP_INFOLOAD_USER_SP b '||
              '  WHERE  B.user_id=a.user_id '||
              '     AND B.PARTY_ID=A.PARTY_ID '||
              '     AND B.SP_SERVICE_ID=A.SP_SERVICE_ID '||
              '     AND B.SP_PRODUCT_ID=A.SP_PRODUCT_ID '||
              '     AND B.START_DATE = A.START_DATE '||
              '     AND B.PARTITION_ID = A.PARTITION_ID '||
              '     AND b.rowid>a.rowid '||
              '     AND b.CHANNELNO = '||v_CHANNELNO||
              '     AND b.modify_tag in (''0'',''8'') '||
              '    ) '||
              '  AND a.modify_tag in (''0'',''8'') '||
              '  AND CHANNELNO = '||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  COMMIT;
  --(3.2) 应该加个判断，放置死循环
  iv_errCnt := 0;
  <<lable>>
  BEGIN
    iv_sql := ' INSERT INTO TF_F_USER_SP  '||
              ' (PARTITION_ID,USER_ID,SERIAL_NUMBER,PAY_USER_ID,PAY_SERIAL_NUMBER,PARTY_ID,SP_SERVICE_ID,SP_PRODUCT_ID, '||
              '  START_DATE,END_DATE,PRIOR_ORDER_TIME,OPR_SOURCE,PRODUCT_ID,SERV_BUND_ID,PRE_VALUE_S1,PRE_VALUE_S2, '||
              '  PRE_VALUE_S3,UPDATE_TIME) '||
              ' SELECT mod(a.USER_ID,10000),a.USER_ID,a.SERIAL_NUMBER,a.PAY_USER_ID,a.PAY_SERIAL_NUMBER,a.PARTY_ID,a.SP_SERVICE_ID,a.SP_PRODUCT_ID, '||
              ' a.START_DATE,a.END_DATE,a.PRIOR_ORDER_TIME,a.OPR_SOURCE,a.PRODUCT_ID,a.SERV_BUND_ID,a.PRE_VALUE_S1,PRE_VALUE_S2, '||
              ' a.PRE_VALUE_S3,a.UPDATE_TIME '||
              ' FROM TP_INFOLOAD_USER_SP a '||
              ' WHERE a.MODIFY_TAG IN (''0'',''8'') '||
              ' AND CHANNELNO = '||v_CHANNELNO;
     EXECUTE IMMEDIATE iv_sql;
  EXCEPTION
    WHEN OTHERS THEN
      ROLLBACK;
      IF iv_errCnt = 3 THEN
             v_resultcode := -1 ;
             v_resultinfo := 'P_INFOLOAD_USER_sp_2执行失败,错误信息:='||SQLERRM;
             return ;
      END IF;
      iv_errCnt := iv_errCnt + 1 ;
      iv_sql := ' DELETE FROM TP_INFOLOAD_USER_SP a '||
                ' WHERE a.modify_tag in (''0'',''8'') '||
                ' AND EXISTS (SELECT 1 '||
                '     FROM TF_F_USER_SP b '||
                '     WHERE b.USER_ID=a.USER_ID '||
                '     AND   B.PARTY_ID=A.PARTY_ID '||
                '     AND   B.SP_SERVICE_ID=A.SP_SERVICE_ID '||
                '     AND   B.SP_PRODUCT_ID=A.SP_PRODUCT_ID '||
                '     AND   B.START_DATE = A.START_DATE'||
                '     AND   B.PARTITION_ID =A.PARTITION_ID'||
                '    ) '||
                ' AND CHANNELNO = '||v_CHANNELNO;
      EXECUTE IMMEDIATE iv_sql;
       GOTO lable;
  END;

  --(4)更新记录--处理
  --(4.1)前面处理过的删除、新增、全量记录删除，只保留更新记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_USER_SP '||
            ' WHERE MODIFY_TAG != ''2'' '||
            ' AND CHANNELNO = '||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  --(4.2)获取同关键字的最后的一条更新操作记录
    iv_sql := ' DELETE FROM TP_INFOLOAD_USER_SP a '||
              ' WHERE EXISTS (SELECT 1 ' ||
              '  FROM TP_INFOLOAD_USER_SP b ' ||
              '  WHERE b.USER_ID=a.USER_ID '||
              '     AND   B.PARTY_ID=A.PARTY_ID '||
              '     AND   B.SP_SERVICE_ID=A.SP_SERVICE_ID '||
              '     AND   B.SP_PRODUCT_ID=A.SP_PRODUCT_ID '||
              '     AND   B.START_DATE = A.START_DATE'||
              '     AND   B.PARTITION_ID =A.PARTITION_ID'||
              '     AND   b.CHANNELNO = '||v_CHANNELNO||
              '     AND b.SYNC_SEQUENCE>a.SYNC_SEQUENCE) '||
              '  AND a.CHANNELNO = '||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.3)关键字排重
  iv_sql := ' DELETE FROM TP_INFOLOAD_USER_SP a '||
            ' WHERE EXISTS (SELECT 1 '||
            '    FROM TP_INFOLOAD_USER_SP b  '||
            '    WHERE b.USER_ID = a.USER_ID '||
            '     AND   B.PARTY_ID=A.PARTY_ID '||
            '     AND   B.SP_SERVICE_ID=A.SP_SERVICE_ID '||
            '     AND   B.SP_PRODUCT_ID=A.SP_PRODUCT_ID '||
            '     AND   B.START_DATE = A.START_DATE'||
            '     AND   B.PARTITION_ID =A.PARTITION_ID'||
            '          AND b.CHANNELNO = '||v_CHANNELNO||
            '       AND b.rowid>a.rowid)'||
            ' AND a.CHANNELNO = '||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.4)按最后存在的记录对表进行更新操作
    iv_sql:=  ' MERGE /*+index (a,PK_TF_F_USER_SP)*/ INTO TF_F_USER_SP a  '||
              ' USING TP_INFOLOAD_USER_SP b                  '||
              '  ON (a.USER_ID = b.USER_ID   '||
              '  AND b.CHANNELNO = '||v_CHANNELNO ||
              '  AND a.partition_id = mod(b.USER_ID,10000)         '||
              '  AND a.PARTY_ID = b.PARTY_ID '||
              '  AND A.SP_SERVICE_ID = B.SP_SERVICE_ID '||
              '  AND A.SP_PRODUCT_ID = B.SP_PRODUCT_ID '||
              '  AND A.START_DATE = B.SP_PRODUCT_ID )'||
              '  WHEN MATCHED THEN                             '||
              '  UPDATE                                        '||
              '  SET             '||
              ' a.SERIAL_NUMBER         = b.SERIAL_NUMBER     ,'||
              ' a.PAY_USER_ID           = b.PAY_USER_ID       ,'||
              ' a.PAY_SERIAL_NUMBER     = b.PAY_SERIAL_NUMBER ,'||
              ' a.END_DATE              = b.END_DATE          ,'||
              ' a.PRIOR_ORDER_TIME      = b.PRIOR_ORDER_TIME  ,'||
              ' a.OPR_SOURCE            = b.OPR_SOURCE        ,'||
              ' a.PRODUCT_ID            = b.PRODUCT_ID        ,'||
              ' a.SERV_BUND_ID          = b.SERV_BUND_ID      ,'||
              ' a.PRE_VALUE_S1          = b.PRE_VALUE_S1      ,'||
              ' a.PRE_VALUE_S2          = b.PRE_VALUE_S2      ,'||
              ' a.PRE_VALUE_S3          = b.PRE_VALUE_S3      ,'||
              ' a.UPDATE_TIME           = b.UPDATE_TIME       ';
    EXECUTE IMMEDIATE iv_sql;

  --(5)清空临时表
  iv_sql := 'DELETE FROM TP_INFOLOAD_USER_SP WHERE CHANNELNO = '||v_CHANNELNO ;
  EXECUTE IMMEDIATE iv_sql;

  v_resultcode:=0;
  v_resultinfo:='P_INFOLOAD_USER_sp正确执行!';

EXCEPTION
  WHEN OTHERS THEN
    v_resultcode:=-1;
    v_resultinfo:='P_INFOLOAD_USER_sp执行失败,错误信息:='||SQLERRM;
    RETURN;
END;
