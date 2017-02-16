SELECT * FROM directory_number   WHERE dn_num = '56407060';
SELECT ca.customer_id,ca.custcode,ca.billcycle,  coa.co_id, cs.tmcode,cs.spcode,cs.sncode, cs.spcode,cs.sncode,cs.dn_id,
dn.dn_num, cs.cs_sparam1,csactivated, csdeactivated,cs.cs_status,cs.cs_stat_chng,cs.cs_on_cbb
FROM directory_number dn, contr_services cs,contract_all coa,customer_all ca
WHERE
dn.dn_num = '56407060'
--ca.custcode = '407060'
--ca.customer_id = 6187521
--coa.co_id = 5979591
AND dn.dn_id = cs.dn_id
AND cs.co_id = coa.co_id
AND coa.customer_id = ca.customer_id     --ORDER BY csactivated
AND substr(cs.cs_stat_chng, -1) IN ('a', 's');

SELECT * FROM customer_all ca WHERE custcode  = '1.5607861';
SELECT * FROM ptcbill_main_sub_lnk WHERE sub_customer_id  =   6350025;

SELECT *FROM equipment WHERE customer_id = 5747798;
SELECT * FROM rtx_050301 WHERE r_p_customer_id = 5803836 AND r_p_contract_id  =5979591  ;
SELECT plcode, Sum(rounded_volume/60) FROM rtx_040401 WHERE r_p_customer_id = 6350025 AND r_p_contract_id  =6548123  AND rated_flat_amount <>0 GROUP BY plcode ;
SELECT 11224882 - 1024*1024*6 FROM dual;
SELECT * FROM rtx_070101 WHERE r_p_customer_id = 6238481 AND r_p_contract_id  =6432204  AND sncode = ;


--查找所有子账号的imei
SELECT l.sub_co_id ,dn.dn_num HKG_MSISDN, mv.imei, cs.cs_sparam1 CHN_MSISDN, l.sub_customer_id
FROM customer_all ca, ptcbill_main_sub_lnk l, contr_services cs,  directory_number dn, DW_CONTRACT_IMEI_MVIEW mv, DW_HANDSET_MODEL
hd
WHERE custcode = '1.5483165'
AND ca.customer_id = l.main_customer_id
AND l.exp_date IS null
AND l.sub_co_id = cs.co_id
AND SubStr(cs.cs_Stat_chng,-1) IN ('a','s')
AND cs.sncode = 1
AND cs.dn_id = dn.dn_id
AND mv.co_id = l.sub_co_id
AND mv.create_date = (SELECT Max(create_date) FROM  DW_CONTRACT_IMEI_MVIEW WHERE co_id= l.sub_co_id)
AND mv.model_id = hd.model_id
ORDER BY hkg_msisdn
;

SELECT * FROM contr_services WHERE co_id = 5979591;

--查找大陆副号
SELECT l.sub_co_id ,dn.dn_num HKG_MSISDN, cs.cs_sparam1 CHN_MSISDN, l.sub_customer_id
FROM customer_all ca, ptcbill_main_sub_lnk l, contr_services cs, contr_services cs1, directory_number dn
WHERE custcode = '1.5483165'
AND ca.customer_id = l.main_customer_id
AND l.exp_date IS null
AND l.sub_co_id = cs.co_id
AND cs.sncode = 237
AND SubStr(cs.cs_Stat_chng,-1) IN ('a','s')
AND cs.co_id = cs1.co_id
AND cs.cs_Seqno=cs1.cs_seqno
AND cs1.dn_id = dn.dn_id     ;


--查询分钟量及流量使用明细（每号码每月一条数据） ， 每月汇总。
SELECT
tm.tmcode,
tm.des,
lnk.TM_GROUP_ID,
Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') pool_fu,
a.custcode, a.co_id, a.msisdn, a.invoice_date, a.inter_voice_usage +a.intra_voice_usage, a.china_usage, Ceil(a.GPRS_USAGE/1024),Ceil(a.CHINA_GPRS_USAGE/1024)
FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.6375924')
AND a.invoice_date = To_Date('20170101','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
AND tm.tmcode = lnk.tmcode
ORDER BY msisdn
;
SELECT * FROM contr_services WHERE co_id =   6379001;
SELECT *FROM contract_history WHERE co_id =   6379001;
SELECT *FROM USER_tab_columns WHERE column_name = 'MODEL_ID';
SELECT * FROM PTCBILL_FREE_UNIT WHERE pkg_id = 421 AND free_unit_id = 10598;


SELECT * FROM master_occ;
SELECT *FROM other_credits WHERE tmcode =728 ;
SELECT *FROM ptcbill_sub_psh_fu_cat  WHERE  main_customer_id =4308519 ;
SELECT *FROM customer_all WHERE custcode = '1.4392172';  --6333805
SELECT * FROM mputmtab WHERE tmcode = 739;
SELECT * FROM customer_all WHERE custcode = '1.6051560';
--查询主账号下所有子账号的流量限制
SELECT dn.dn_num, volume/1024 "volume(GB)", l.sub_customer_id,  cvh.co_id, seq_no, cvh.ent_date, cs.tmcode, cs.spcode, cs.sncode FROM contr_volume_history cvh,
ptcbill_main_sub_lnk l ,contr_services cs, directory_number dn
WHERE l.sub_co_id = cvh.co_id
AND l.main_customer_id = 5987560
AND seq_no = (SELECT Max(seq_no) FROM contr_volume_history hh WHERE hh.co_id = cvh.co_id)
AND  cs.co_id = l.sub_co_id
AND substr(cs.cs_stat_chng, -1) IN ('a', 's')
--AND cs.dn_id IS NOT NULL
and cs.dn_id = dn.dn_id
ORDER BY dn_num;

SELECT *FROM rtx_040101 WHERE r_p_customer_id =  6293607 AND r_p_contract_id =  6488825 AND sncode = 119 AND ;

SELECT  rtx.r_p_customer_id, Sum(rounded_volume)/60/1024/1024 FROM  rtx_010401 rtx, ptcbill_main_sub_lnk l
WHERE rtx.r_p_customer_id = l.sub_customer_id
AND rtx.r_p_contract_id = l.sub_co_id
AND l.main_customer_id =  3843789
AND rtx.rtx_type IN ('A')
AND rtx.sncode = 119
GROUP BY rtx.r_p_customer_id;
--66931872
SELECT Sum(unb_p_gprs_usg)/60 FROM ptcapp_sub_usage psu, ptcbill_main_sub_lnk l
WHERE psu.co_id = l.sub_co_id
AND l.main_customer_id = 3843789;
--66931872

SELECT *FROM   contr_volume_history cvh,  ptcbill_main_sub_lnk l ,contr_services cs
WHERE l.sub_co_id = cvh.co_id
AND l.main_customer_id = 3843789
AND  cs.co_id = l.sub_co_id
AND substr(cs.cs_stat_chng, -1) IN ('a', 's')
AND cs.dn_id IS NOT null
ORDER BY cs.co_id, cvh.seq_no desc;


SELECT *FROM customer_all WHERE custcode = '1.4392172';
SELECT *FROM contract_all WHERE customer_id = 3843789;
SELECT *FROM
SELECT *FROM contr_volume_history WHERE co_id = 6422401;
SELECT * FROM ptcbill_main_sub_lnk WHERE sub_co_id = 6379001;
SELECT sub_co_id FROM ptcbill_main_sub_lnk WHERE main_customer_id = 5559330 ORDER BY sub_co_id
minus
SELECT co_id FROM (
SELECT cvh.co_id, seq_no, volume, cvh.ent_date FROM contr_volume_history cvh,   ptcbill_main_sub_lnk l
WHERE l.sub_co_id = cvh.co_id
AND l.main_customer_id = 5559330
AND seq_no = (SELECT Max(seq_no) FROM contr_volume_history hh WHERE hh.co_id = cvh.co_id) ORDER BY co_id )
;
SELECT * FROM ptcbill_main_sub_lnk WHERE main_customer_id = 3843789;
SELECT * FROM contract_history WHERE co_id = 5048296;
SELECT * FROM ptcbill_main_sub_lnk WHERE main_customer_id = 3843789;
SELECT * FROM contract_history WHERE co_id = 5048296;
SELECT * FROM mputmview WHERE tmcode IN (674, 726, 728);
SELECT tmcode, Count(*) cnt FROM ptcbill_co_usage_summary
WHERE custcode LIKE '2.11%'
GROUP BY tmcode
ORDER BY cnt desc;
SELECT * FROM ptcbill_rateplan_group_lnk WHERE tm_group_id = 1 AND market_type  = 'M';
SELECT tmcode FROM customer_all ca WHERE customer_id IN (
SELECT sub_customer_id  FROM ptcbill_main_sub_lnk WHERE main_customer_id = 6042274);

SELECT *  FROM contract_all  WHERE co_id IN (
SELECT sub_co_id FROM ptcbill_main_sub_lnk WHERE main_customer_id = 3790808);

SELECT *  FROM contr_services cs WHERE co_id IN (
SELECT sub_co_id FROM ptcbill_main_sub_lnk WHERE main_customer_id = 3790808)
AND  ;

SELECT sp.des, sn.des,cs.cs_stat_chng, cs.* FROM contr_services cs, mpusptab sp, mpusntab sn
 WHERE co_id = 6379001
 AND cs.spcode = sp.spcode
 AND cs.sncode = sn.sncode
 ORDER BY sp.des, sn.des, cs.cs_stat_chng;
SELECT *FROM customer_all WHERE custcode = '1.5626549';
SELECT * FROM contract_history  WHERE co_id = 3940002;
SELECT * FROM contract_all  WHERE co_id = 3940002;SELECT * FROM customer_all WHERE customer_id = 3969801;
SELECT * FROM customer_all WHERE custcode = '1.3875551';SELECT
--Sum(LOCAL_FREE_MINS_INTER),Sum(LOCAL_FREE_MINS_INTRA),Sum(CHINA_FREE_MINS),Sum(INTER_VOICE_USAGE),Sum(INTRA_VOICE_USAGE),Sum(CHINA_USAGE)
a.*
FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm
--, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.6195420')
AND a.invoice_date = To_Date('20160806','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
--AND tm.tmcode = lnk.tmcode
--AND lnk.TM_GROUP_ID=1
ORDER BY 2
;
SELECT  *
 FROM ptcbill_co_usage_summary a WHERE custcode = '1.4815717' AND invoice_date =To_Date('20161116','yyyymmdd');

SELECT
tm.tmcode,
tm.des,
lnk.TM_GROUP_ID,
Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') pool_fu,
a.custcode, a.co_id, a.msisdn, a.invoice_date, a.FREE_GPRS, a.GPRS_USAGE,  a.EXTRA_GPRS_VOL,  a.FREE_CHINA_GPRS,  a.CHINA_GPRS_USAGE,  a.EXTRA_CHINA_GPRS_VOL, a.CHINA_LOCAL_GPRS_USAGE, a.EXTRA_CHINA_LOCAL_GPRS_VOL
FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.5607861')
AND a.invoice_date = To_Date('20170106','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
AND tm.tmcode = lnk.tmcode
ORDER BY 4,3
;


SELECT * FROM customer_all WHERE custcode LIKE '2.11%';
SELECT * FROM customer_all WHERE custcode = '1.4815717';
SELECT * FROM mpusntab WHERE sncode IN (1, 441, 399);
SELECT * FROM mpusptab WHERE spcode IN(361);
SELECT * FROM MPUTMview WHERE tmcode IN(754);
SELECT * FROM mputmview WHERE tmcode IN    (736);
SELECT * FROM tapin_Rtx WHERE
--tapin_output_file = 'CDJPNDOHK0PP02534'
 imsi = '454120410751713'
 AND tapin_filename = 'CDJPNDOHKGPP02534'
AND msisdn = '85259335232'
;
--1907421
SELECT * FROM TAPIN_FILE WHERE f_filename = 'CDJPNDOHKGPP02534';
SELECT Max(f_id)  FROM "TAPIN_RTX_DMP" WHERE ROWNUM<=1;
SELECT * FROM tapin_cdr WHERE ROWNUM <= 1;
SELECT * FROM s2t_roaming_output_file  WHERE filename = 'GGSN_S2T_2016090508' ;
SELECT *FROM user_tab_columns WHERE column_name   = 'F_ID';
SELECT *FROM mpdpltab WHERE cc = 81;
SELECT * FROM user_tables WHERE table_name LIKE 'TAPIN%';
ca.customer_id,ca.custcode,ca.billcycle,  coa.co_id, cs.tmcode, cs.spcode,
cs.sncode,cs.dn_id,dn.dn_num
FROM directory_number dn, contr_services cs,contract_all coa,customer_all ca
WHERE
-- dn.dn_num = '92047383'
 dn.dn_id = cs.dn_id
AND cs.co_id = coa.co_id
AND coa.customer_id = ca.customer_id
AND substr(cs.cs_stat_chng, -1) IN ('a', 's')
AND ca.billcycle = '07';

SELECT * FROM s2t_cdr;
SELECT * FROM contract_all WHERE customer_id = 6227103;
--57420
SELECT * FROM GPRS_CDR_DETAIL_ROAMING_MVNO WHERE  file_name = 'GGSN_S2T_2016090508' ;


SELECT * FROM ptcapp_sub_usage WHERE customer_id = 6238481 AND co_id = 6432204;
SELECT Ceil(unb_p_gprs_usg/60),Ceil(unb_p_roamgprs_usg/60),Ceil(unb_p_chn_roamgprs_usg/60)
FROM ptcapp_sub_usage WHERE customer_id = 6227103 AND co_id = 6422401;
SELECT sn.des, fu.* FROM mbsadm.ptcbill_tm_free_unit tfu, ptcbill_free_unit fu,
mpusntab sn
WHERE tmcode = 727
AND sn.sncode = fu.pkg_id
AND   tfu.expiry_date IS null
AND   tfu.free_unit_id = fu.free_unit_id
--and free_unit_inter = 100
AND   fu.pkg_id = 421
;

SELECT sn.des, fu.* FROM mbsadm.ptcbill_tm_free_unit tfu, ptcbill_free_unit fu,
mpusntab sn
WHERE tmcode = 739
AND sn.sncode = fu.pkg_id
AND   tfu.expiry_date IS null
AND   tfu.free_unit_id = fu.free_unit_id
--and free_unit_inter = 100
AND   fu.pkg_id IN (
SELECT sn.sncode FROM contr_services cs, mpusptab sp, mpusntab sn
 WHERE co_id = 6379001
 AND cs.spcode = sp.spcode
 AND cs.sncode = sn.sncode
 AND SubStr(cs.cs_stat_chng, -1) IN ('a', 's')
 )
;

SELECT sn.des, fu.* FROM mbsadm.ptcbill_tm_free_unit tfu, ptcbill_free_unit fu,
mpusntab sn
WHERE tmcode = 751
AND sn.sncode = fu.pkg_id
AND   tfu.expiry_date IS null
AND   tfu.free_unit_id = fu.free_unit_id
AND fu.pkg_id IN (
SELECT sncode FROM contr_services cs
WHERE cs.co_id = 6488825
AND SubStr(cs.cs_stat_chng, -1) IN ('a', 's')
AND cs.cs_seqno = (SELECT Max(cs2.cs_seqno) FROM contr_services cs2
WHERE cs2.co_id = cs.co_id
AND cs2.tmcode = cs.tmcode
AND cs2.spcode = cs.spcode
AND cs2.sncode = cs.sncode)
);

SELECT * FROM mputmtab ;
SELECT * FROM ptcbill_free_unit;
SELECT * FROM ptcbill_pkg_group WHERE pkg_id = 421;
SELECT * FROM mpusntab WHERE sncode IN (421,1) ;
SELECT * FROM mpusptab WHERE spcode IN (208);
SELECT * FROM ptcbill_cat_Group WHERE cat_group = 907002;
SELECT * FROM sms_group  WHERE GRp_id IN (0, 8200);
SELECT * FROM ptcbill_cat_Group WHERE cat_group = 907006;
SELECT * FROM contr_volume_history WHERE co_id = 6422401 ORDER BY seq_no;
SELECT * FROM contr_volume_history WHERE ent_user NOT LIKE 'md%';
SELECT * FROM ptcbill_rtx_type_Group ;
SELECT * FROM ptcbill_roam_group WHERE roam_group = 15;
SELECT * FROM mpdpltab WHERE plcode IN (SELECT plcode FROM ptcbill_roam_group WHERE roam_group = 15) ;
SELECT *FROM directory_number;
SELECT * FROM user_tab_columns  WHERE COLUMN_name LIKE 'IMSI';
SELECT * FROM contr_devices;
SELECT *FROM port WHERE  port_num = 454120054983764;
SELECT *FROM EQUIPMENT;
SELECT * FROM storage_medium sm, contr_devices cd
WHERE co_id = 4816974
AND cd.cd_sm_num = sm.sm_serialnum;

SELECT * FROM mputmview WHERE des LIKE '4G HK%CH 3GB%';
SELECT * FROM customer_all WHERE tmcode = 727;

SELECT pcd.cdesc, ids.*FROM ptcbill_invoice_detail_cosum ids,
ptcbpp_cfg_description pcd
WHERE
ids.des = pcd.source and custcode = '1.6204460'
AND msisdn = 56467561 ORDER BY seq;
SELECT * FROM ptcbill_text_config ;
SELECT * FROM ptcbpp_cfg_description WHERE ROWNUM <= 1;
select * from v$parameter WHERE name LIKE 'nls%';
SELECT * FROM v$database;
SELECT * FROM v$session ;
SELECT * FROM v$instance;
SELECT * FROM dba_tables WHERE table_name LIKE '%NLS';
SELECT * FROM all_tab_columns WHERE column_name = Upper('nls_language');
SELECT * FROM v$session;
SELECT * FROM mputmview WHERE DES LIKE '%21Mbps%';
SELECT * FROM mputmview WHERE tmcode = 653;
SELECT *FROM mpusntab WHERE sncode  = 525;
SELECT * FROM mpuzptab WHERE zpcode = 7364;
SELECT * FROM mpdpltab WHERE plcode = 133;
SELECT * FROM ptcbill_invoice_detail_cdr WHERE custcode = '1.6204460' and tag IN ('ITB1I','ITB1A') AND msisdn = 53031755 ORDER BY seq;
SELECT * FROM ptcbill_invoice_detail_cdr WHERE custcode = '1.6204460' and tag IN ('ITB1R','ITB1r') AND msisdn = 53031755 ORDER BY plcode, seq;
SELECT * FROM ptcbill_invoice_detail_cdr WHERE custcode = '1.6204460' AND msisdn = 51084508;

SELECT * FROM ptcbill_invoice_header WHERE custcode =    '1.6204460';
SELECT *FROM ptcbill_invoice_detail_sum WHERE custcode = '1.6204460' ORDER BY seq;
SELECT *FROM ptcbill_text_config ;
SELECT * FROM 	ptcbpp_cfg_rateplan WHERE tmcode = 653;
SELECT * FROM ptcapp_bill_country pbc, MPDPLTAB dpl
WHERE pbc.source = dpl.source ;
SELECT * FROM MPDPLTAB ;
SELECT * FROM mpulktmz WHERE zpcode = 231;
SELECT * FROM ptcbill_idd_roa_display WHERE sncode = 4;
SELECT *FROM user_tab_columns WHERE column_name LIKE '%QOS%';
SELECT *FROM mputmview WHERE des LIKE '4G Local 6GB%';
SELECT * FROM ptcbill_co_usage_summary WHERE msisdn = 92047223;
SELECT tm.des, ca.customer_id,ca.custcode,ca.billcycle,  coa.co_id, cs.tmcode, cs.spcode,cs.sncode,cs.dn_id,
dn.dn_num, cs.cs_sparam1,csactivated, csdeactivated,cs.cs_stat_chng
FROM directory_number dn, contr_services cs,contract_all coa,customer_all ca, mputmview tm
WHERE
--dn.dn_num = '92047223'
--ca.custcode = '1.6404104'
 dn.dn_id = cs.dn_id
AND cs.co_id = coa.co_id
AND coa.customer_id = ca.customer_id
AND substr(cs.cs_stat_chng, -1) IN ('a', 's')
AND tm.tmcode = ca.tmcode
AND ca.custcode LIKE '2.11%'
ORDER BY tm.des, dn.dn_num;

SELECT custcode, msisdn, invoice_date,s.*  FROM ptcbill_co_usage_summary s
 WHERE tmcode = 509 AND  invoice_date = To_Date('20161120', 'yyyymmdd');
SELECT *  FROM ptcapp_usage_hist s
 WHERE air_entitle_tmcode = 509 AND  date_billed = To_Date('20161120', 'yyyymmdd');
SELECT *FROM rtx_060201 where r_p_customer_id = 6049325 AND r_p_contract_id = 6236008 AND sncode = 119 ORDER BY start_d_t ;
SELECT *FROM rtx_060201 ;
SELECT sncode, call_type, rtx.* FROM rtx_070401 rtx where r_p_customer_id = 6238481 AND r_p_contract_id = 6432204 AND sncode  IN (4) AND rated_flat_amount <>0 ORDER BY start_d_t ;

SELECT trunc(original_start_d_t) ,rtx.r_p_customer_Id,
rtx.r_p_contract_id, dn.dn_num,
Sum(CASE  rtx_type  when 'A' THEN  rounded_volume/60 ELSE 0 END)
as HK_DATA, Sum(CASE   rtx_type when 'R' THEN  rounded_volume/60 ELSE 0 END) as CHINA_DATA, Sum(rounded_volume/60)
FROM rtx_060201 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.6110132'
AND rtx.r_p_customer_id = l.sub_customer_id
AND rtx.r_p_contract_id = l.sub_co_id
AND rtx.sncode = 119
AND ( rtx_type = 'A' OR (rtx_type = 'R' AND rtx.plcode IN (76, 115, 254, 417)))
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
GROUP BY trunc(original_start_d_t) ,rtx.r_p_customer_Id, rtx.r_p_contract_id,  dn.dn_num;

SELECT trunc(original_start_d_t) ,rtx.r_p_customer_Id,
rtx.r_p_contract_id, dn.dn_num,
Sum(CASE  rtx_type  when 'A' THEN  rounded_volume/60 ELSE 0 END)
as HK_DATA, Sum(CASE   rtx_type when 'R' THEN  rounded_volume/60 ELSE 0 END) as CHINA_DATA, Sum(rounded_volume/60)
FROM rtx_060201 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.6110132'
AND rtx.r_p_customer_id = l.sub_customer_id
AND rtx.r_p_contract_id = l.sub_co_id
AND rtx.sncode = 119
AND rtx.rated_flat_amount <> 0
AND ( rtx_type = 'A' OR (rtx_type = 'R' AND rtx.plcode IN (76, 115, 254, 417)))
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
GROUP BY trunc(original_start_d_t) ,rtx.r_p_customer_Id, rtx.r_p_contract_id,  dn.dn_num;

--香港
SELECT l.sub_customer_id,
l.sub_co_id, dn.dn_num,
Sum(CASE  rtx_type  when 'A' THEN  rounded_volume/60 ELSE 0 END)
as HK_DATA, Sum(CASE   rtx_type when 'R' THEN  rounded_volume/60 ELSE 0 END) as CHINA_DATA, Sum(rounded_volume/60)
FROM rtx_060201 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.6110132'
AND rtx.r_p_customer_id(+) = l.sub_customer_id
AND rtx.r_p_contract_id(+) = l.sub_co_id
AND rtx.sncode(+) = 119
AND rtx.rated_flat_amount(+) <> 0
--AND ( rtx.rtx_type = 'A' OR (rtx.rtx_type= 'R' AND rtx.plcode IN (76, 115, 254, 417)))
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
AND Trunc(rtx.original_start_d_t) = To_Date('20161026', 'yyyymmdd')
GROUP BY l.sub_customer_id, l.sub_co_id,  dn.dn_num;
--大陆
SELECT dn.dn_num, Sum(CASE  rtx_type  when 'A' THEN  Nvl(rounded_volume,0)/60 ELSE 0 END)
as HK_DATA, Sum(CASE   rtx_type when 'R' THEN  Nvl(rounded_volume,0)/60 ELSE 0 END) as CHINA_DATA, Sum(Nvl(rounded_volume,0)/60)
FROM rtx_060201 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
 WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.6110132'
AND rtx.r_p_customer_id(+) = l.sub_customer_id
AND rtx.r_p_contract_id(+) = l.sub_co_id
AND rtx.sncode(+) = 119
AND rtx.rated_flat_amount(+) <> 0
AND ( rtx.rtx_type(+) = 'A' )

AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
AND  original_start_d_t(+) >= To_Date('20161029', 'yyyymmdd')
AND original_start_d_t(+) <  To_Date('20161030', 'yyyymmdd')
GROUP BY dn.dn_num
ORDER BY dn.dn_num;

SELECT dn.dn_num Mobile, Sum(CASE  rtx_type  when 'A' THEN  Nvl(rounded_volume,0)/60 ELSE 0 END) AS "HK data(KB)"
, Sum(CASE   rtx_type when 'R' THEN  Nvl(rounded_volume,0)/60 ELSE 0 END) "China data(KB)" , Sum(Nvl(rounded_volume,0)/60) "Total(KB)"
FROM rtx_060201 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
 WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.6110132'
AND rtx.r_p_customer_id(+) = l.sub_customer_id
AND rtx.r_p_contract_id(+) = l.sub_co_id
AND rtx.sncode(+) = 119
AND rtx.rated_flat_amount(+) <> 0
--AND ( rtx.rtx_type(+) = 'A' )

AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
--AND Trunc(original_start_d_t) = To_Date('20161026', 'yyyymmdd')
AND  original_start_d_t(+) >= To_Date('20161106', 'yyyymmdd')
AND original_start_d_t(+) <  To_Date('20161107', 'yyyymmdd')
GROUP BY dn.dn_num
ORDER BY dn.dn_num;


SELECT dn.dn_num, Sum(CASE  rtx_type  when 'A' THEN  Nvl(rounded_volume,0)/60 ELSE 0 END)
as HK_DATA, Sum(CASE   rtx_type when 'R' THEN  Nvl(rounded_volume,0)/60 ELSE 0 END) as CHINA_DATA, Sum(Nvl(rounded_volume,0)/60)
FROM rtx_060201 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
 WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.6110132'
AND rtx.r_p_customer_id(+) = l.sub_customer_id
AND rtx.r_p_contract_id(+) = l.sub_co_id
AND rtx.sncode(+) = 119
AND rtx.rated_flat_amount(+) <> 0
AND ( rtx.rtx_type(+) = 'R' AND rtx.plcode(+) = 76 )

AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
AND  original_start_d_t(+) >= To_Date('20161029', 'yyyymmdd')
AND original_start_d_t(+) <  To_Date('20161030', 'yyyymmdd')
GROUP BY dn.dn_num
ORDER BY dn.dn_num;




SELECT *
FROM rtx_060201 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.6110132'
AND rtx.r_p_customer_id = l.sub_customer_id
AND rtx.r_p_contract_id = l.sub_co_id
AND rtx.sncode = 119
AND rtx.rounded_volume = 68640
AND ( rtx_type = 'A' OR (rtx_type = 'R' AND rtx.plcode IN (76, 115, 254, 417)))
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id ;


SELECT rtx.r_p_customer_Id,
rtx.r_p_contract_id, dn.dn_num,
Sum(CASE  rtx_type  when 'A' THEN  rounded_volume/60 ELSE 0 END)
as HK_DATA, Sum(CASE   rtx_type when 'R' THEN  rounded_volume/60 ELSE 0 END) as CHINA_DATA, Sum(rounded_volume/60)
FROM rtx_010101 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.4392172'
AND rtx.r_p_customer_id = l.sub_customer_id
AND rtx.r_p_contract_id = l.sub_co_id
AND rtx.sncode = 119
AND ( rtx_type = 'A' OR (rtx_type = 'R' AND rtx.plcode IN (76, 115, 254, 417)))
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
AND rtx.rated_flat_amount <> 0
GROUP BY rtx.r_p_customer_Id, rtx.r_p_contract_id,  dn.dn_num
ORDER BY dn.dn_num;

SELECT  dn.dn_num, original_start_d_t,  Decode (rtx_type, 'A', 'HK', 'R', 'China') , rounded_volume/60 FROM rtx_010101 rtx ,
 ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, directory_number dn
WHERE l.main_customer_id = ca.customer_id
AND ca.custcode = '1.4392172'
AND rtx.r_p_customer_id = l.sub_customer_id
AND rtx.r_p_contract_id = l.sub_co_id
AND rtx.sncode = 119
AND dn.dn_num = 53988328
AND ( rtx_type = 'A' OR (rtx_type = 'R' AND rtx.plcode IN (76, 115, 254, 417)))
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_Id
AND rtx.rated_flat_amount <> 0
ORDER BY original_start_d_t;

SELECT * FROM rtx_060201 ;
SELECT *FROM PTCBILL_MASTER_CONTROL;
SELECT *FROM mpusntab;
SELECT *FROM ;

SELECT *FROM ccontact ;
SELECT *FROM customer_all;
SELECT * FROM user_tab_columns WHERE column_name LIKE '%ADDR%';
SELECT *FROM MIAP_SUBSCRIPTION
 WHERE email = 'zhaojun1912@yeah.net';
 SELECT *FROM PTCBILL_CHECK_PARTY;

 SELECT
Sum(LOCAL_FREE_MINS_INTER),Sum(LOCAL_FREE_MINS_INTRA),Sum(CHINA_FREE_MINS),Sum(INTER_VOICE_USAGE),Sum(INTRA_VOICE_USAGE),Sum(CHINA_USAGE)
-- a.*
FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.5607861')
AND a.invoice_date = To_Date('20161106','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
AND tm.tmcode = lnk.tmcode
AND lnk.TM_GROUP_ID=1
ORDER BY 2
;

SELECT
tm.tmcode,
tm.des,
--lnk.TM_GROUP_ID,
--Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') pool_fu,
a.custcode, a.co_id, a.msisdn,  a.GPRS_USAGE,a.CHINA_GPRS_USAGE, a.GPRS_USAGE+ a.CHINA_GPRS_USAGE total FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.5607861')
AND a.invoice_date = To_Date('20170106','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
AND tm.tmcode = lnk.tmcode
ORDER BY msisdn
;
SELECT * FROM ptcbill_co_usage_summary WHERE invoice_date = To_Date('20170116', 'yyyymmdd') AND msisdn = 67572883;
SELECT * FROM ptcbill_co_usage_summary WHERE invoice_date = To_Date('20160120', 'yyyymmdd') AND custcode = '2.11.52.64.100109';


SELECT * FROM mpusntab WHERE des LIKE '%Bar%' ORDER BY sncode;
SELECT * FROM  mpusntab WHERE sncode = 17;
SELECT * FROM customer_all WHERE custcode = '1.5988245';
SELECT * FROM  ptcbill_main_sub_lnk WHERE main_customer_id = 5923768;
SELECT dn.dn_num, sn.des, cs.cs_stat_chng  FROM ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, contract_all co,directory_number dn, mpusntab sn
 WHERE main_customer_id = ca.customer_id
 AND ca.custcode = '1.5988245'
 AND cs.co_id = co.co_id
 AND SubStr(cs.cs_Stat_chng,-1) IN ('a','s')
 AND cs.sncode IN ( 423,85, 68,69,284,71)
 --AND cs.dn_id = dn.dn_id
 AND l.sub_co_id = cs.co_id
 AND cs.sncode = sn.sncode
;

WITH t AS
( SELECT dn.dn_num, co.co_id  FROM ptcbill_main_sub_lnk l, customer_all ca, contr_services cs, contract_all co,directory_number dn, mpusntab sn
 WHERE main_customer_id = ca.customer_id
 AND ca.custcode = '1.5988245'
 AND cs.co_id = co.co_id
 AND SubStr(cs.cs_Stat_chng,-1) IN ('a','s')
 --AND cs.sncode = 1
-- AND cs.sncode IN ( 423,85, 68,69,284,71)
 AND cs.dn_id = dn.dn_id
 AND l.sub_co_id = cs.co_id
 AND cs.sncode = sn.sncode
 )

 SELECT t.dn_num, t.co_id, sn.des, cs.cs_Stat_chng FROM t,  contr_services cs, mpusntab sn
 WHERE
 --cs.sncode IN ( 24,31,175,423,85, 68,69,284,71)
 --cs.sncode IN ( 85, 68,69,284,71)
 cs.sncode IN ( 24,31,175,423)
 AND t.co_id = cs.co_id
 AND cs.sncode = sn.sncode
 AND  SubStr(cs.cs_Stat_chng,-1) IN ('a','s')
;

SELECT * FROM contr_services WHERE co_id = 6432204 AND sncode IN ( 24,31,175,423);
SELECT *FROM mpusntab;