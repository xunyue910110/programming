CREATE OR REPLACE PROCEDURE p_asp_otp_refreshinfo
(
    v_eparchyCode      in   VARCHAR2,
    v_resultCode      OUT   NUMBER,
    v_resultErrInfo   OUT   VARCHAR2
)
-----------------------------------------------------------------------
--    Proceduce:   p_asp_otp_refreshinfo
--    Description: 用户一次性费用生成资料刷新存储过程
--    Author:
--    Date:
--    Editor:  UltraEdit-32
-----------------------------------------------------------------------
IS
    --iv_computeState       NUMBER;
    --iv_computeMode        NUMBER;
    --iv_recomputeAcycId    NUMBER;
    iv_computeAcycId      NUMBER;
    iv_sql                VARCHAR2(2000);
    --iv_computeDate        DATE;
    --iv_discnt_code        Number;
    iv_acycstarttime      td_b_cycle.cyc_start_time%TYPE;
    iv_acycendtime        td_b_cycle.cyc_end_time%TYPE;
    --v_eparchyCode         char(4);

BEGIN

    v_resultCode:=0;
    --v_eparchyCode:='0022';
    v_resultErrInfo:='p_asp_otp_refreshinfo 存储过程执行成功';

    --iv_computeState:=0;
    --iv_computeMode:=0;
    --iv_computeAcycId:=0;
    --iv_recomputeAcycId:=0;
    iv_sql:='';

    --获取最小实时帐期
    --Select min(cycle_id) Into iv_computeAcycId FROM td_b_cycle Where use_tag='0';
    Select min(cycle_id) Into iv_computeAcycId FROM td_b_cycle_eparchy Where use_tag='0' and (eparchy_code=v_eparchyCode  or v_eparchyCode is null or v_eparchyCode ='0');

    --获取帐期开始时间结束时间
    BEGIN
        Select cyc_start_time,cyc_end_time
        INTO iv_acycstarttime,iv_acycendtime
        FROM td_b_cycle_eparchy
        Where cycle_id=iv_computeAcycId and (eparchy_code=v_eparchyCode or   v_eparchyCode is null or v_eparchyCode = '0') 
        and rownum < 2;
    EXCEPTION
        WHEN OTHERS THEN
            v_resultCode:=-1;
            v_resultErrInfo:='error_010:获取计算时间所在帐期错误！'||SQLERRM;
            RETURN;
    END;

    --插入表
    Begin
        --插入有tradefee的用户
--        Insert INTO TM_A_SMSDISCOUNT  (user_id,pre_value_n1,pre_value_n2,Pre_Value_N3)
--        Select user_id, trade_item_code,sum(trade_fee*10) ,0
--        From tf_f_user_tradefee
--        Where in_time>=iv_acycstarttime AND
--              in_time<=iv_acycendtime   AND--防止错误，每次只生成最小实时帐期的费用
--              act_tag = 0
--        GROUP BY user_id,trade_item_code
--        Order By user_id;
        --插入有tradefee的用户
        iv_sql:= ' Insert Into TM_A_SMSDISCOUNT '||
                 ' (user_id,pre_value_n1,pre_value_n2,Pre_Value_N3)'||
                 ' Select a.user_id, b.item_id,a.trade_fee*1000 ,0'||
                 ' From tf_f_user_tradefee a,td_b_externalitemtodetailitem b'||
                 ' Where a.in_time>=:iv_acycstarttime And '||
                 '       a.in_time<=:iv_acycendtime AND'||
                 '       a.trade_fee<>0 AND'||
                 '       a.trade_item_code=b.external_item_id AND'||
                 '       a.act_tag = ''0'' ' ||
                 ' Order By user_id';
        EXECUTE IMMEDIATE iv_sql USING iv_acycstarttime,iv_acycendtime;


    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            v_resultcode := -1;
            v_resultErrInfo := 'error_010:'||SQLERRM;
            RETURN;
        WHEN OTHERS THEN
            v_resultcode := -1;
            v_resultErrInfo := 'error_020:'||SQLERRM;
            RETURN;
    END;


    --去除欠停一月以上的用户，不参与相应计算
 /*   Begin
        Insert INTO TM_A_SMSDISCOUNT (user_id,pre_value_n3)
        Select  a.user_id ,1 From tf_f_user_servstate a
        Where NOT Exists
        (Select 1 From tf_f_user_servstate b
         Where b.serv_state_code='0' And b.main_tag='1' And b.start_date<(iv_acycstarttime-40) And b.end_date>iv_acycendtime  --'5'表示欠停，40天以前欠停
         And a.user_id=b.user_id
        );

    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            v_resultcode := -1;
            v_resultErrInfo := 'error_030:'||SQLERRM;
            RETURN;
        WHEN OTHERS THEN
            v_resultcode := -1;
            v_resultErrInfo := 'error_040:'||SQLERRM;
            RETURN;
    END;


    --过滤掉Pre_Value_N3=3的用户(表示用户是长时间欠停时间用户)
    Begin
        DELETE TM_A_SMSDISCOUNT a
        Where a.pre_value_n3=0
        And Exists
        (
        Select 1 From TM_A_SMSDISCOUNT b Where b.pre_value_n3=1 And a.user_id=b.user_id
        );

        DELETE TM_A_SMSDISCOUNT a Where a.pre_value_n3=1;

    EXCEPTION
        WHEN NO_DATA_FOUND THEN
            v_resultcode := -1;
            v_resultErrInfo := 'error_050:'||SQLERRM;
            RETURN;
        WHEN OTHERS THEN
            v_resultcode := -1;
            v_resultErrInfo := 'error_060:'||SQLERRM;
            RETURN;
    END;
    */

    --输出到日志表
    /*
        Insert Into tl_b_abnormalid Select user_id, pre_value_n3 From TM_A_SMSDISCOUNT where pre_value_n3=1;
    */


    /*-- 完成资料刷新候需要更新 td_update_flag 以通知固定费用计算主程序更新系统参数
    BEGIN
        UPDATE td_update_flag
           SET time_stamp = TO_CHAR(TO_NUMBER(time_stamp)+1);
    EXCEPTION
        WHEN OTHERS THEN
            ROLLBACK;
            v_resultCode:=-1;
            v_resultErrInfo:='error_070:更新 td_update_flag 错误！'||SQLERRM;
            RETURN;
    END;*/
    --COMMIT;

EXCEPTION
    WHEN OTHERS THEN
        ROLLBACK;
        v_resultCode:=-1;
        v_resultErrInfo:='error_200:@END'||SQLERRM;

        COMMIT;

        RETURN;
END;
/
