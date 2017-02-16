SET PAGESIZE 500 ;
SET LINESIZE 200 ;
set serveroutput on size 1000000;
SET VERIFY OFF;

DECLARE

new_tmcode    number := 721;
new_des       VARCHAR2(30);

CURSOR c1 IS
SELECT tmcode, vscode, vsdate, status, tmind, des,
shdes, plcode, plmnname, tmrc, rec_version,
default_qos, echannel, tier_type
FROM mputmview WHERE tmcode IN (638)
ORDER BY tmcode;



BEGIN
    
    FOR k IN c1 LOOP
          new_des := '4G Local 1GB';
       dbms_output.put_line(new_tmcode||','||new_des);
       INSERT INTO mputmtab
       SELECT new_tmcode,vscode, vsdate, status, tmind, new_des,
              '4GL1G', plcode, plmnname, tmrc, rec_version,
              DEFAULT_QOS, echannel, tier_type
       FROM mputmtab
       WHERE tmcode = k.tmcode;
        
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mputmtab :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO mpulktm1
       SELECT tm.tmcode, ref.vscode, to_date('20160104','YYYYMMDD'), ref.status, spcode, sncode, subscript, accessfee, event
            , echind, amtind, frqind, srvind, proind, advind, susind, taxcode, ltcode, ref.plcode
            , accdmcode, subdmcode, usgdmcode, billfreq, freedays, accglcode, subglcode, usgglcode
            , il01, ir01, im01, ic01, il02, ir02, im02, ic02, il03, ir03, im03, ic03, il04, ir04, im04, ic04
            , il05, ir05, im05, ic05, il06, ir06, im06, ic06, il07, ir07, im07, ic07, il08, ir08, im08, ic08
            , il09, ir09, im09, ic09, il10, ir10, im10, ic10, accjcid, usgjcid, subjcid, 0, creind, creglcode
       FROM mpulktm1 ref, mputmtab tm
       WHERE tm.status = 'W'
       and not exists (select 1 from mpulktm1 where tmcode = tm.tmcode and sncode = 1)
       and ref.tmcode = k.tmcode
       and tm.tmcode = new_tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpulktm1 :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;
       
       update mpulktm1
       set accessfee = 18
       where tmcode = new_tmcode
       and sncode =118;
              
       UPDATE mpulktm1 b
       SET accessfee = 218
         , accglcode = 4007000493
         , subglcode = 4007000493
         , creind = 'Y'
         , creglcode = 4007000493
       WHERE sncode = 1
       AND tmcode = new_tmcode;

       
       INSERT INTO mpulktm2
       SELECT tm.tmcode, ref.vscode, to_date('20160104','YYYYMMDD'), ref.status, spcode, sncode, twcode, ttcode
            , gvcode, zncode, svlcode, splitind, uomind, umcode, usgind, typeind, rndind, rateind
            , minuom, maxuom, hrcode, il01, ir01, im01, ic01, il02, ir02, im02, ic02, il03, ir03, im03, ic03
            , il04, ir04, im04, ic04, il05, ir05, im05, ic05, il06, ir06, im06, ic06, il07, ir07, im07, ic07
            , il08, ir08, im08, ic08, il09, ir09, im09, ic09, il10, ir10, im10, ic10
            , ev_reg, ev_act, ev_dea, ev_int, ev_inv, ev_era, usgglcode, usgjcid, 0
       FROM mpulktm2 ref, mputmtab tm
       WHERE tm.status = 'W'
       and not exists (select 1 from mpulktm2 where tmcode = tm.tmcode and sncode = 1)
       and ref.tmcode = k.tmcode
       and tm.tmcode = new_tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpulktm2 :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       insert into mpulktmb
       SELECT new_tmcode, vscode, to_date('20160104','YYYYMMDD'), 'P', spcode, sncode,
             subscript, accessfee, event,
             echind, amtind, frqind, srvind, proind, advind, susind,
             taxcode, ltcode, plcode, accdmcode, subdmcode, usgdmcode, billfreq,
             freedays, accglcode, subglcode, usgglcode,
             il01, ir01, im01, ic01,
             il02, ir02, im02, ic02,
             il03, ir03, im03, ic03,
             il04, ir04, im04, ic04,
             il05, ir05, im05, ic05,
             il06, ir06, im06, ic06,
             il07, ir07, im07, ic07,
             il08, ir08, im08, ic08,
             il09, ir09, im09, ic09,
             il10, ir10, im10, ic10,
             accjcid, usgjcid, subjcid, 0, creind, creglcode
       FROM mpulktmb
       WHERE tmcode = k.tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpulktmb :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;
       
       update mpulktmb
       set accessfee = 18
       where tmcode = new_tmcode
       and sncode =118;
       
       UPDATE MPULKTMB b
       SET accessfee = 218
         , accglcode = 4007000493
         , subglcode = 4007000493
         , creind = 'Y'
         , creglcode = 4007000493
       WHERE sncode = 1
       AND tmcode = new_tmcode;
       
       insert into mpulktmm
       SELECT new_tmcode, vscode, to_date('20160104','YYYYMMDD'), 'P', spcode, sncode,
             twcode, ttcode, gvcode, zncode, svlcode, splitind,
             uomind, umcode, usgind, typeind, rndind, rateind,
             minuom, maxuom, hrcode,
             il01, ir01, im01, ic01,
             il02, ir02, im02, ic02,
             il03, ir03, im03, ic03,
             il04, ir04, im04, ic04,
             il05, ir05, im05, ic05,
             il06, ir06, im06, ic06,
             il07, ir07, im07, ic07,
             il08, ir08, im08, ic08,
             il09, ir09, im09, ic09,
             il10, ir10, im10, ic10,
             ev_reg, ev_act, ev_dea, ev_int, ev_inv, ev_era,
             usgglcode, usgjcid, 0
       FROM mpulktmm
       WHERE tmcode = k.tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpulktmm :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO mpulktmt
       SELECT new_tmcode, vscode, ttcode, ticode,
              tdcode, tides, lb, rb, tddes, coldate,
              mon, tue, wed, thu, fri, sat, sun, hol, 0
       FROM mpulktmt
       WHERE tmcode = k.tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpulktmt :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO mpulktmz
       SELECT new_tmcode, vscode, zncode, zocode, zpcode, zodes,
             cgi, zpdes, digits, 0
       FROM mpulktmz
       WHERE tmcode = k.tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpulktmz :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO mpufutab
       SELECT new_tmcode, vscode, umcode, to_date('20160104','YYYYMMDD'), NULL
           , spcode, sncode, twcode, ttcode, gvcode
           , zncode, proind, fuom, 0, carry_forward
       FROM mpufutab
       WHERE tmcode = k.tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpufutmb :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO mpuintab
       SELECT new_tmcode, vscode, umcode, to_date('20160104','YYYYMMDD'), NULL
           , spcode, sncode, ttcode, zncode, fuom
           , call_type, on_top, besides
           , 0, prind, carry_forward
       FROM mpuintab
       WHERE tmcode = k.tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpuintmb :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO free_mins
       SELECT n.seq + 1, b.tmcode, b.spcode, b.sncode, ref.free_mins
           , to_date('20160104','YYYYMMDD'), expiry_date, ref.share_telephony
           , sysdate, NULL, 'BATCH', 0
       FROM mpulktmb b
           , (select * from free_mins where tmcode = k.tmcode) ref
           , (select nvl(max(seqno),0) as seq from free_mins) n
       WHERE b.sncode = ref.sncode
       AND b.tmcode = new_tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted free_mins :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO mpulktmc
       SELECT b.tmcode, b.spcode, b.sncode, ref.grp_id
       FROM mpulktmb b
           , (select sncode, grp_id from mpulktmc where tmcode = k.tmcode) ref
       WHERE b.sncode = ref.sncode
       AND b.tmcode = new_tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted mpulktmc :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

       INSERT INTO fuom_all
       SELECT new_tmcode, vscode, umcode, to_date('20160104','YYYYMMDD'), NULL
           , sncode, pkg_sncode, grp_id, call_type
           , inter_fuom, intra_fuom, deduct_mthd, 0, PROIND
       FROM fuom_all
       WHERE tmcode = k.tmcode;
       
       IF SQL%FOUND THEN 
       Dbms_Output.put_line('Inserted fuom_all :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
       else 
       Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
       END IF;

      INSERT INTO mpucltab
      SELECT new_tmcode,vscode, to_date('20160104','YYYYMMDD'), NULL
           , ccl, cdays, kvflag, ccldayly, 0
      FROM mpucltab
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted mpucltab :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO ptcbill_tm_free_unit
      SELECT new_tmcode, free_unit_id, to_date('20160104','YYYYMMDD'), expiry_date, priority
      FROM  ptcbill_tm_free_unit
      WHERE tmcode = k.tmcode
      ;
      
      DELETE ptcbill_tm_free_unit
      WHERE free_unit_id IN (10402)
      AND tmcode = new_tmcode;
      
      INSERT INTO ptcbill_tm_free_unit
      VALUES
      (new_tmcode, 10528, To_Date('20160104','yyyymmdd'), NULL, 0);



      
      /*INSERT INTO ptcbill_free_unit
      SELECT 10547,537,zone_group,time_group,rtx_type_group,CALL_TYPE,OFFSET_TYPE,ROAM_GROUP,CAT_GROUP,UOM,1000,0,USE_CREDIT_TX,PRORATE_IND,CARRY_FORWARD
      FROM ptcbill_free_unit
      WHERE free_unit_id =10481;
      
      DELETE ptcbill_tm_free_unit
      WHERE free_unit_id = 10412
      AND tmcode = 712;

      UPDATE ptcbill_free_unit
      SET free_unit_inter = 999999999
         ,FREE_UNIT_INTRA = 999999999
      WHERE free_unit_id = 10489;

      insert into ptcbill_tm_free_unit 
      VALUES
      (712, 10547, To_Date('20160104','yyyymmdd'), NULL, 0);*/


      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_tm_free_unit :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      insert into mputptab (tpcode,vscode,vsdate,status,des,shdes,rec_version)
      select tpcode + ceil(rownum/2), tp.vscode, to_date('20160104','YYYYMMDD')
      , tp.status, tm.des || ' Pkg', tm.shdes, 0
      from mputptab tp, mputmview tm
      where tpcode = (select max(tpcode) from mputptab)
      and tm.tmcode = new_tmcode 
      order by tm.tmcode, tpcode, tp.vscode
      ;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted mputptab :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;


      insert into mpulktp1 (tpcode, vscode, vsdate, status, seqno, tmcode, spcode, rec_version)
      select tpcode, tp.vscode, tp.vsdate, tp.status, 1 as seqno, tmb.tmcode, tmb.spcode, 0
      from mputptab tp, mputmview tm, mpulktmb tmb
      where tp.des = tm.des || ' Pkg'
      and tm.tmcode = tmb.tmcode
      and tmb.sncode = 1
      and tp.status = 'W'
      and tm.tmcode = new_tmcode
      order by tm.tmcode, tpcode, tp.vscode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted mpulktp1 :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      insert into mpulktpb (tpcode, vscode, vsdate, status, seqno, tmcode, spcode, rec_version)
      select tpcode, tp.vscode, tp.vsdate, tp.status, 1 as seqno, tmb.tmcode, tmb.spcode, 0
      from mputptab tp, mputmview tm, mpulktmb tmb
      where tp.des = tm.des || ' Pkg'
      and tm.tmcode = tmb.tmcode
      and tmb.sncode = 1
      and tp.status = 'P'
      and tm.tmcode = new_tmcode
      order by tm.tmcode, tpcode, tp.vscode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted mpulktpb :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO other_credits
      SELECT new_tmcode, oc_id, vscode, gl_account, amount, amt_changeable, adjust_accessfee,
             accessfee_service, adjust_usagefee, usagefee_service, bill_times, allow_duplicate, carry_forward,
             bill_suspense, valid_from, accessfee_sncode, usagefee_sncode,
             usagefee_grp_id, expiry_date, expiry_bill_times, accessfee_spcode
      FROM other_credits
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted other_credits :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO ptcapp_rateplan
      SELECT rateplan_code, rateplan_name, new_tmcode, rateplan_rank, free_minute
      FROM ptcapp_rateplan
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcapp_rateplan :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO ptcapp_curr_rateplan
      SELECT new_tmcode,(SELECT des FROM mputmtab WHERE tmcode = new_tmcode AND status = 'P'), TYPE
      FROM  ptcapp_curr_rateplan
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcapp_curr_rateplan :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO POS_SERVICE_RULE
      SELECT new_tmcode, spcode, sncode, confirm_dialog, assoc_sncode,
             dflt_service, deact_allowed, act_allowed, customer_type,
             clear_cs_on_cbb, charge_on_cbb, prgcode, ent_date, ent_outlet_user,
             'P3730', mod_date, mod_outlet_user, mod_user,
             (SELECT Max(rule_id) FROM POS_SERVICE_RULE)+rownum, id_prefix_rel_type,
             msg_prompt
      FROM POS_SERVICE_RULE
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted pos_service_rule :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO ptcbill_rateplan_group_lnk
      SELECT tm_group_id, new_tmcode, ent_date, 'P3730', tm_sub_group_id,
             market_type, rateplan_type, sub_account_limit,FREE_SUB_ACCOUNT_LIMIT,null
      FROM ptcbill_rateplan_group_lnk
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_rateplan_group_lnk :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      
      update ptcbill_rateplan_group_lnk
      set SUB_ACCOUNT_LIMIT = 1
      where tmcode = new_tmcode;


      INSERT INTO mbsadm.ptcbpp_cfg_rateplan
      SELECT new_tmcode, new_des, new_des, new_des, other, inter, intra,
             effective_date, 'P3730', expired, free_unit_type_id,
             plan_type, free_unit_cdesc, free_unit_edesc, cbb_effective_date
      FROM mbsadm.ptcbpp_cfg_rateplan
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbpp_cfg_rateplan :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      
      UPDATE ptcbpp_cfg_rateplan
      SET other = '(2000)'
         ,inter = 2000
         ,free_unit_cdesc = '(2000基本分鐘/1GB本地數據)'
         ,free_unit_edesc = '(2000 Basic Min/1GB Local Data)'
      where tmcode = new_tmcode;


      INSERT INTO mbsadm.ptcbill_data_amt_display
      select new_tmcode, sncode, rtx_type, display_data_amt, sncode2, plcode
      FROM mbsadm.ptcbill_data_amt_display
      WHERE tmcode = k.tmcode
      and plcode =1;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_data_amt_display :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO mbsadm.ptcrate_check_ad_tmcode
      VALUES
      (new_tmcode, to_date('20160104','YYYYMMDD'));
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcrate_check_ad_tmcode :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
            
      INSERT INTO PTCRATE_RFH_TM_GRP
      SELECT 4, new_tmcode, to_date('20160104','YYYYMMDD')
      FROM PTCRATE_RFH_TM_GRP
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcrate_rfh_tm_grp :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;


      INSERT INTO ptcbill_tm_daily_max_charge
      SELECT new_tmcode, pkg_sncode, daily_max_charge_id, prorate,
             priority, effective_date, expiry_date
      FROM  ptcbill_tm_daily_max_charge
      WHERE tmcode = 696
      and daily_max_charge_id =7;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_tm_daily_max_charge :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;


      INSERT INTO ptcbill_tm_max_charge
      SELECT new_tmcode, pkg_sncode, max_charge_id, effective_date, expiry_date
      FROM ptcbill_tm_max_charge
      WHERE tmcode = 696;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_max_charge :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED INTO ptcbill_max_charge.');
      END IF;


      INSERT INTO mbsadm.PTCBILL_OCC_MINS_DISPLAY
      SELECT oc_id, new_tmcode, sncode, rtx_type, free_mins
      FROM  mbsadm.PTCBILL_OCC_MINS_DISPLAY
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_occ_mins_display :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;


      INSERT INTO mbsadm.ptcapp_occ_free_mins
      SELECT oc_id, new_tmcode, free_mins, to_date('20160104','YYYYMMDD'), 'P3730'
      FROM mbsadm.ptcapp_occ_free_mins
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcapp_occ_free_mins :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;


      insert into mbsadm.ptcrate_tm_tariff
      (TMCODE,SEQ_NO, TARIFF_ID, EFFECTIVE_DATE, EXPIRY_DATE, ENTER_DATE, ENTER_USER, MOD_DATE, MOD_USER)
      values
      (new_tmcode,1,34,to_date('20160104','YYYYMMDD'),null,to_date('20160104','YYYYMMDD'),'P3730',null,null);

      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcrate_tm_tariff :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;


      INSERT INTO ptcbill_def_tier_commitment
      SELECT new_tmcode, 1048576, max_volume,
             1048576, usg_id, free_usg_id,
             to_date('20160104','YYYYMMDD'), exp_date, 'P3730', to_date('20160104','YYYYMMDD')
      FROM ptcbill_def_tier_commitment
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_def_tier_commitment :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;


      /*INSERT INTO ptcbill_usg_conversion_from
      SELECT conversion_id, new_tmcode, sncode, rtx_type_group,
             roam_group, zone_group, cat_group, convert_volume
      FROM ptcbill_usg_conversion_from
      WHERE tmcode = k.tmcode;*/
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_usg_conversion_from :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      
      INSERT INTO MBSADM.cmcc_simn_tapin_price
      SELECT vplcode, sncode, vscode, call_type, rated_charge_land, 
      rated_charge_land_interval, rated_charge_air, rated_charge_air_interval, digits, 
      last_update_user, last_update_date, pkg_sncode, price_group, new_tmcode
      FROM MBSADM.cmcc_simn_tapin_price
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted cmcc_simn_tapin_price :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      
      
      INSERT INTO ptcmtc_mtc_price
      SELECT vplmn, price_group, version, country, effective_date, 
      mtc_call_price_local, mtc_call_price_idd, mtc_call_interval_local, 
      mtc_call_interval_idd, call_hk_price_local, call_hk_price_idd, call_hk_interval_local, 
      call_hk_interval_idd, call_other_price_local, call_other_price_idd, call_other_interval_local, 
      call_other_interval_idd, pkg_sncode, new_tmcode, mtc_115_price_local, mtc_115_price_idd, 
      mtc_115_interval_local, mtc_115_interval_idd, call_hk_115_price_local, 
      call_hk_115_price_idd, call_hk_115_interval_local, call_hk_115_interval_idd, 
      call_other_115_price_local, call_other_115_price_idd, call_other_115_interval_local, 
      call_other_115_interval_idd, mtc_video_call_price_local, 
      mtc_video_call_price_idd, mtc_video_call_interval_local, mtc_video_call_interval_idd, 
      mtc_call_cost, call_hk_cost, call_other_cost, mtc_video_cost
      FROM ptcmtc_mtc_price
      WHERE tmcode = k.tmcode;

      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcmtc_mtc_price :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      
      INSERT INTO ptcbill_tm_surcharge
      SELECT new_tmcode, surcharge_id, eff_date, exp_date
      FROM ptcbill_tm_surcharge
      WHERE tmcode = k.tmcode;

      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_tm_surcharge :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      

      /*if k.tmind = '2G' then
         update mpulktm1
         set accglcode = '0007602702'
            ,subglcode = '0007602702'
         where tmcode = new_tmcode
         and sncode =118;

         update mpulktmb
         set accglcode = '0007602702'
            ,subglcode = '0007602702'
         where tmcode = new_tmcode
         and sncode =118;

      elsif k.tmind = '3G-M' then
         update mpulktm1
         set accglcode = '3007602702'
            ,subglcode = '3007602702'
         where tmcode = new_tmcode
         and sncode =118;

         update mpulktmb
         set accglcode = '3007602702'
            ,subglcode = '3007602702'
         where tmcode = new_tmcode
         and sncode =118;
      elsif k.tmind = '3G-S' then
         update mpulktm1
         set accglcode = '3207602702'
            ,subglcode = '3207602702'
         where tmcode = new_tmcode
         and sncode =118;

         update mpulktmb
         set accglcode = '3207602702'
            ,subglcode = '3207602702'
         where tmcode = new_tmcode
         and sncode =118;
      elsif k.tmind = '4G' then
         update mpulktm1
         set accglcode = '4007602702'
            ,subglcode = '4007602702'
         where tmcode = new_tmcode
         and sncode =118;

         update mpulktmb
         set accglcode = '4007602702'
            ,subglcode = '4007602702'
         where tmcode = new_tmcode
         and sncode =118;
       elsif k.tmind = '4GLite' then
         update mpulktm1
         set accglcode = '4107602702'
            ,subglcode = '4107602702'
         where tmcode = new_tmcode
         and sncode =118;

         update mpulktmb
         set accglcode = '4107602702'
            ,subglcode = '4107602702'
         where tmcode = new_tmcode
         and sncode =118;
       end if;*/
       
       
       insert into PTCBILL_SPECIAL_PRECISION_CFG
       select new_tmcode, sncode, plcode,decimal_place, sncode2
       from PTCBILL_SPECIAL_PRECISION_CFG
       where tmcode = k.tmcode
       and PLCODE = 1;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted PTCBILL_SPECIAL_PRECISION_CFG :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      
      INSERT INTO ptcbill_2cm_plan_cfg
      SELECT new_tmcode, plan_type, max_volume, free_volume, 
      sysdate, exp_date, 'P3730', SYSDATE
      FROM ptcbill_2cm_plan_cfg
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted ptcbill_2cm_plan_cfg :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;
      

      /*insert into pos_rateplan_rule
      select  rule_id+1, new_tmcode,exist_cust,trunc(sysdate-1),trunc(sysdate-1),sysdate,'P3730',id_prefix_rel_type,msg_prompt,require_contact,acc_type
      from pos_rateplan_rule
      where rule_id = (select max(rule_id) from pos_rateplan_rule);
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted pos_rateplan_rule :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;

      INSERT INTO MPULKNXG
      select sncode, new_tmcode, typeind, prgcode, plcode, lvcode, rec_version, tpcode, module
      from mpulknxg
      WHERE tmcode = k.tmcode;
      
      IF SQL%FOUND THEN 
      Dbms_Output.put_line('Inserted mpulknxg :'||SQL%ROWCOUNT||' ROWS ARE INSERTED.');
      else 
      Dbms_Output.put_line('NOTHING HAS BEEN INSERTED.');
      END IF;*/

    END LOOP;
END;

--commit;
--ROLLBACK;