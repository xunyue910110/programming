CREATE OR REPLACE PROCEDURE P_INFOLOAD_FEEPOLICY
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
   iv_sql := ' DELETE FROM TP_INFOLOAD_FEEPOLICY A '||
             ' WHERE A.MODIFY_TAG = ''8'' '||
             ' AND EXISTS (SELECT 1 '||
             ' FROM TP_INFOLOAD_FEEPOLICY B '||
             ' WHERE B.ID = A.ID '||
             '  AND B.MODIFY_TAG = ''8'' '||
             '  AND B.CHANNELNO = '||v_CHANNELNO||
             '  AND B.SYNC_SEQUENCE > A.SYNC_SEQUENCE) ' ||
             '  AND A.CHANNELNO = '||v_CHANNELNO  ;
   EXECUTE IMMEDIATE iv_sql;
  --(2)将modify_tag='1'和modify_tag='8'的记录在正式表中进行删除
  iv_sql := ' DELETE FROM TF_F_FEEPOLICY A '||
            ' WHERE (A.ID, A.ID_TYPE, A.PARTITION_ID) IN '||
            ' (SELECT C.ID, C.ID_TYPE, MOD(C.ID, 10000) '||
            ' FROM TP_INFOLOAD_FEEPOLICY C '||
            ' WHERE C.MODIFY_TAG = ''8'' ' ||
            '  AND C.CHANNELNO = '||v_CHANNELNO||
            ' ) ';
   EXECUTE IMMEDIATE iv_sql;
  --(3)插入的记录--处理
  --(3.1)删除重复记录
    iv_sql := ' DELETE FROM TP_INFOLOAD_FEEPOLICY a '||
              ' WHERE EXISTS (SELECT 1 '||
              ' FROM TP_INFOLOAD_FEEPOLICY b '||
              '  WHERE b.feepolicy_ins_id=a.feepolicy_ins_id '||
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
    iv_sql := ' INSERT INTO TF_F_FEEPOLICY  '||
              ' (PARTITION_ID,FEEPOLICY_INS_ID,ID_TYPE,ID,FEEPOLICY_ID,RELA_USER_ID,PRODUCT_ID,SERV_BUND_ID, '||
              '  SERV_ID,FEEPOLICY_BUND_ID,START_DATE,END_DATE,UPDATE_TIME,UPDATE_DEPART_ID,UPDATE_STAFF_ID) '||
              ' SELECT mod(a.id,10000),a.FEEPOLICY_INS_ID,a.ID_TYPE,a.ID,a.FEEPOLICY_ID,a.RELA_USER_ID,a.PRODUCT_ID,a.SERV_BUND_ID, '||
              ' a.SERV_ID,a.FEEPOLICY_BUND_ID,a.START_DATE,a.END_DATE,a.UPDATE_TIME,NULL,NULL '||
              ' FROM TP_INFOLOAD_FEEPOLICY a '||
              ' WHERE a.MODIFY_TAG IN (''0'',''8'') '||
              ' AND CHANNELNO = '||v_CHANNELNO;
     EXECUTE IMMEDIATE iv_sql;
  EXCEPTION
    WHEN OTHERS THEN
      ROLLBACK;

      iv_sql := ' DELETE FROM TP_INFOLOAD_FEEPOLICY a '||
                ' WHERE a.modify_tag in (''0'',''8'') '||
                ' AND EXISTS (SELECT 1 '||
                '     FROM TF_F_FEEPOLICY b '||
                '     WHERE b.feepolicy_ins_id=a.feepolicy_ins_id '||
                '    ) '||
                ' AND CHANNELNO = '||v_CHANNELNO;
      EXECUTE IMMEDIATE iv_sql;
       GOTO lable;
  END;

  --(4)更新记录--处理
  --(4.1)前面处理过的删除、新增、全量记录删除，只保留更新记录
  iv_sql := ' DELETE FROM TP_INFOLOAD_FEEPOLICY '||
            ' WHERE MODIFY_TAG != ''2'' '||
            ' AND CHANNELNO = '||v_CHANNELNO;
  EXECUTE IMMEDIATE iv_sql;
  --(4.2)获取同关键字的最后的一条更新操作记录
  iv_sql :=   ' DELETE FROM TP_INFOLOAD_FEEPOLICY a '||
              ' WHERE EXISTS (SELECT 1 ' ||
              '  FROM TP_INFOLOAD_FEEPOLICY b ' ||
              '  WHERE b.feepolicy_ins_id=a.feepolicy_ins_id '||
              ' AND b.CHANNELNO = '||v_CHANNELNO||
              '    AND b.SYNC_SEQUENCE>a.SYNC_SEQUENCE) '||
              ' AND a.CHANNELNO = '||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.3)关键字排重（是否可以与4.2合并）
  iv_sql := ' DELETE FROM TP_INFOLOAD_FEEPOLICY a '||
            ' WHERE EXISTS (SELECT 1 '||
            '    FROM TP_INFOLOAD_FEEPOLICY b '||
            '    WHERE b.feepolicy_ins_id=a.feepolicy_ins_id '||
            '          AND b.CHANNELNO = '||v_CHANNELNO||
            '       AND b.rowid>a.rowid)'||
            ' AND a.CHANNELNO = '||v_CHANNELNO;
   EXECUTE IMMEDIATE iv_sql;
  --(4.4)按最后存在的记录对表进行更新操作
    iv_sql:=  ' MERGE /*+index (a,PK_TF_F_FEEPOLICY)*/ INTO TF_F_FEEPOLICY a  '||
              ' USING TP_INFOLOAD_FEEPOLICY b                  '||
              '  ON (a.feepolicy_ins_id = b.feepolicy_ins_id   '||
              '  AND b.CHANNELNO = '||v_CHANNELNO ||
              '  AND a.partition_id = mod(b.id,10000))         '||
              '  WHEN MATCHED THEN                             '||
              '  UPDATE                                        '||
              '  SET a.ID_TYPE = b.ID_TYPE,                    '||
              '      a.ID = b.ID,                              '||
              '      a.FEEPOLICY_ID = b.FEEPOLICY_ID,          '||
              '      a.RELA_USER_ID = b.RELA_USER_ID,          '||
              '      a.PRODUCT_ID = b.PRODUCT_ID,              '||
              '      a.SERV_BUND_ID = b.SERV_BUND_ID,          '||
              '      a.SERV_ID = b.SERV_ID,                    '||
              '      a.FEEPOLICY_BUND_ID = b.FEEPOLICY_BUND_ID,'||
              '      a.START_DATE = b.START_DATE,              '||
              '      a.END_DATE = b.END_DATE,                  '||
              '      a.UPDATE_TIME = b.UPDATE_TIME              ';
    EXECUTE IMMEDIATE iv_sql;

  --(5)清空临时表
  iv_sql := 'DELETE FROM TP_INFOLOAD_FEEPOLICY WHERE CHANNELNO = '||v_CHANNELNO ;
  EXECUTE IMMEDIATE iv_sql;

  v_resultcode:=0;
  v_resultinfo:='P_INFOLOAD_FEEPOLICY正确执行!';

EXCEPTION
  WHEN OTHERS THEN
    v_resultcode:=-1;
    v_resultinfo:='P_INFOLOAD_FEEPOLICY执行失败,错误信息:='||SQLERRM;
    RETURN;
END;
/
