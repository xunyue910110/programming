56407060 大陆号码：14714920294
--Ryan 分钟量sql
--查询个人用户的某billcycle期间每个号码（1CMN）语音短信流量使用量,表ptcbill_co_usage_summary
SELECT
Sum(LOCAL_FREE_MINS_INTER),Sum(LOCAL_FREE_MINS_INTRA),Sum(CHINA_FREE_MINS),Sum(INTER_VOICE_USAGE),Sum(INTRA_VOICE_USAGE),Sum(CHINA_USAGE)
-- a.*
FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.3804202')
AND a.invoice_date = To_Date('20161126','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
AND tm.tmcode = lnk.tmcode
AND lnk.TM_GROUP_ID=1
ORDER BY 2
;

--KwLi sql:
-- tm_group_id : 7 Local-China rate plan
-- dpl: network
  1 :HongKong CMHK 45412
76: China Telm 46000
254: China Mobile 46007
115: China Unicom 46001
--Service Code sncode:
  237: 1CMN
  1 : telephony
119: GPRS
227: QPI Mobile Monthly Package
277: WIFI Service
421: 4G-WIFI
491：4G-300 Free China IDD Mins
--service code: 7 for 2/3G , 10 for LTE
--call type:1 mo, 2 mt
SELECT * FROM mpusntab WHERE sncode= 119;
SELECT
tm.tmcode,
tm.des,
lnk.TM_GROUP_ID,
Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 4 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') pool_fu,
a.custcode, a.co_id, a.msisdn, a.invoice_date, a.FREE_GPRS, a.GPRS_USAGE, a.EXTRA_GPRS_VOL, a.FREE_CHINA_GPRS, a.CHINA_GPRS_USAGE, a.EXTRA_CHINA_GPRS_VOL, a.CHINA_LOCAL_GPRS_USAGE, a.EXTRA_CHINA_LOCAL_GPRS_VOL
FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.3875551')
AND a.invoice_date = To_Date('20160806','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
AND tm.tmcode = lnk.tmcode
ORDER BY 2
;
SELECT * FROM  ptcbill_sub_psh_fu_cat WHERE main_customer_id =    3790808;
SELECT * FROM ptcbill_free_Unit_cat WHERE free_unit_cat_id IN (1,4,13,16);
-- For 1CMN service (sncode 237), the CHN MSISDN recorded in the column cs_sparam1

SELECT l.sub_co_id ,dn.dn_num HKG_MSISDN, cs.cs_sparam1 CHN_MSISDN
FROM customer_all ca, ptcbill_main_sub_lnk l, contr_services cs, contr_services cs1, directory_number dn
WHERE custcode = '1.4176596'
AND ca.customer_id = l.main_customer_id
AND l.exp_date IS null
AND l.sub_co_id = cs.co_id
AND cs.sncode = 237
AND SubStr(cs.cs_Stat_chng,-1) IN ('a','s')
AND cs.co_id = cs1.co_id
AND cs.cs_Seqno=cs1.cs_seqno
AND cs1.dn_id = dn.dn_id;

--CREATE SEQUENCE ppcom_roaming_cdr_file_seq START WITH 1 INCREMENT BY 1 NOCACHE NOCYCLE;

--20161129 KwLi NOTE
SELECT * FROM ptcapp_usage_hist WHERE customer_id = 6238481 AND co_id =6432204;
SELECT * FROM ptcapp_usage_hist WHERE customer_id = 5133893 AND co_id =6231559;


SELECT date_billed, Ceil(gprs_usg/60), Ceil(roamgprs_chn_usg/60)
 FROM ptcapp_usage_hist WHERE customer_id = 6227103 AND co_id = 6422401;
SELECT Ceil(unb_p_gprs_usg/60),Ceil(unb_p_roamgprs_usg/60),Ceil(unb_p_chn_roamgprs_usg/60) FROM ptcapp_sub_usage WHERE customer_id = 6227103 AND co_id = 6422401;
SELECT * FROM ptcapp_sub_usage WHERE customer_id = 6227103 AND co_id = 6422401;

SELECT * FROM directory_number WHERE dn_num ='92047383';
SELECT * FROM contr_services WHERE dn_id =163893 ;
SELECT * FROM mputmview WHERE tmcode = 653;
SELECT fu.* FROM mbsadm.ptcbill_tm_free_unit tfu, ptcbill_free_unit fu
WHERE tmcode = 597
AND tfu.expiry_date IS null
AND tfu.free_unit_id = fu.free_unit_id
AND fu.pkg_id = 421;
zone_group: 1 all zones, 2 China
time_group: 1 all times
rtx_type_group: 1 Air, 2 Land, 3 R ( roaming MO ) + r ( roaming MT), 4 R(roaming MO), 5 C (Content),6 A + C, 7 A + R 8 A + R + r
call_type_group: 1 Mobile Originated, 2 Mobile Terminated, 3 Call Forward , 99 All types
offset type: 0 no inter/intra separation, pool setup inter, 1 set up intra
roam_group: 1 all VPLMNS 6 China GPRS, 7 Local + China,  9 WIFI CHN 12 4G China 13 Macau
14 Taiwan 15 Greater China( Local + china + Macau + Taiwan)

SELECT * FROM ptcbill_pkg_group WHERE pkg_id = 421;
SELECT * FROM mpusntab WHERE sncode IN (119,277) ;
SELECT * FROM ptcbill_cat_Group WHERE cat_group = 907002;
SELECT * FROM ptcbill_cat_Group WHERE cat_group = 907006;
SELECT * FROM contr_volume_history WHERE co_id = 6422401 ORDER BY seq_no;
SELECT * FROM contr_volume_history WHERE ent_user NOT LIKE 'md%';
SELECT * FROM ptcbill_rtx_type_Group WHERE rtx_type_group = 7;
SELECT * FROM ptcbill_roam_group WHERE roam_group = 3 ;
SELECT * FROM user_tables WHERE table_name LIKE '%CONTR_%';
SELECT * FROM mbsadm.ROAMING_WIFI_FILE ORDER BY entdate desc;
SELECT * FROM tapin_rtx WHERE f_id = 2210296;
SELECT /*+index(rtx rtx_070301_1)*/ rtx_Type, sncode, call_type, original_Start_d_T,actual_volume, plcode , rated_flat_amount, grp_id
FROM rtx_070301 rtx WHERE sncode = 277 AND rtx_type = 'R' ORDER BY original_start_d_T;

SELECT * FROM tapin_Rtx;
SELECT * FROM mbsadm.outbound_rtx;

SELECT /*+index(rtx rtx_030301_1)*/ tmcode, rtx_Type, sncode, call_type, original_Start_d_T, actual_volume, plcode , rated_flat_amount, grp_id
FROM rtx_030301 rtx WHERE r_p_customer_id = 6115765 AND r_p_contract_id = 6304723 AND rtx_type = 'R' ORDER BY original_start_d_T;
'R' -- Roaming outgoin
'r' -- Roaming incoming
'A' -- Air
'L' -- Land (IDD)
'S' -- sucharge
'C' -- Content

SELECT * FROM mpusntab WHERE sncode = 421;

SELECT * FROM contract_all WHERE co_id = 6422401;
SELECT billcycle, customer_id,custcode FROM customer_all WHERE customer_id = 5608893;
SELECT billcycle, customer_id,custcode FROM customer_all WHERE custcode = '1.5675719';
SELECT billcycle, customer_id,custcode FROM customer_all WHERE custcode = '1.4815717';
SELECT * FROM tapin_rtx WHERE plcode =76 AND service_code = 8;
SELECT * FROM mpdpltab WHERE plcode =76;
SELECT * FROM port WHERE port_num = '454120413220231';
SELECT * FROM contr_devices WHERE port_id = 10059016;
SELECT * FROM contract_all WHERE co_id = 6304723;
SELECT billcycle FROM customer_all WHERE customer_id = 6115765;

##Li Jing SQL:
select * from directory_number where dn_num=92047383; --plcode 1
select * from contr_services where dn_id=163893 and sncode=1 and cs_stat_chng like '%a'; -- co_id=6422401 tmcode=653 spcode=214
select * from contract_all where co_id=6422401;--customer_id=6227103
select * from customer_all where customer_id=6227103;

select * from contract_all where customer_id=6227103;
select * from contr_services where co_id=6422401 and cs_stat_chng like '%a';
select * from mpulktmb where tmcode=653 and spcode=214 order by sncode; --408
select IL01,IC01,IL10,IC10,typeind,zncode from mpulktmm where tmcode=653 and spcode=214 and sncode=1 and typeind in ('A','L') and zncode=2;--1.95
select * from ptcrate_tm_tariff where tmcode=653;
SELECT * FROM ptcrate_contract_tariff WHERE co_id = 6422401;
select * from MBSADM.PTCRATE_TARIFF where tariff_id=32;
SELECT IL01,IC01,IL10,IC10,type_ind FROM ptcrate_tariff_usage WHERE tariff_id = 32 AND vscode = 1 AND sncode = 1 AND digit_id=86;

select * from mpusntab where sncode in (select sncode from contr_services where co_id=6422401 and cs_stat_chng like '%a');
select * from mpuzptab where digits ='+86';
select * from mpulkzop where zncode=231;
select * from mpuzntab where zncode=231;

select * from mpuzntab where zncode in (1,2);

---对于forfeit的处理 以下只针对于monthly access fee occ
select * from MON_DIS_PROCESS_CONTROL;
select * from mon_dis_record;--temp table contra_occ
select * from ptcapp_dis_occ;--对于月费occ,如果需要检查可用性,occ必须配置在此表,非常重要
select * from PTCAPP_DIS_OCC_FORFEIT;--定义是否可以self-forfeit
select * from ptcapp_occ_forfeit_cat;--对cat进行说明
select * from ptcapp_occ_forfeit_cat_oc_lnk; --和其他的occ不能共存
select * from ptcapp_dis_occ_grp;--和同类别的occ不能共存

----forfeit_occ.sql 处理携号转网mnp和手动HS优惠occ的forfeit
select * from ptcreb_user;--一般只有一条记录,并且rebate_program_id不会存在于pos_system_parameter.param_value
select * from ptcreb_occ_lookup;
select * from ptcreb_status_des;
select * from ptcapp_occ_forfeit_wt;

--customers for billing:
/*
Select target customer to process

1) 'S': suspend customer -- customer's last bill run date < bill date and customer's contract deactivated date > bill date - 90 days (configurable billing factor)

2) 'N': new customer -- customer's last bill run date is null and customer's contract activated <= bill date
  -- customer_all, contract_all, contract_history

3) 'D': deactived customer -- customer's last bill run date < bill date, customer's contract deactivated date <= bill date - 90 days and customer has outstanding invoice transaction (records in ORDERHDR_ALL with enter date is in the billing period and opening amount != 0)

*/
Select ca.customer_id,
  'N' customer_type,
  ca.tmcode
  from customer_all ca
 where ca.billcycle = 01
  and ca.lbc_date is NULL --new customer
  and exists
  (select 1
  from contract_all co,
  contract_history ch
  where co.customer_id = ca.customer_id
  and co.co_id = ch.co_id
  and ch.ch_validfrom <= to_date('161201', 'RRMMDD')
  and ch.ch_seqno = (select min(ch1.ch_seqno)
  from contract_history ch1
  where ch1.co_id = ch.co_id
  and ch1.ch_status = 'a')
  )
union
select ca.customer_id,
  'S' customer_type,
  ca.tmcode
  from customer_all ca
 where ca.billcycle = 01
  and ca.lbc_date < to_date('161201', 'RRMMDD')
  and (
  exists
  (select 1
  from contract_all co,
  contract_history ch
  where co.customer_id = ca.customer_id
  and co.co_id = ch.co_id
  and ch.ch_validfrom > to_date('161201', 'RRMMDD') - 90
  and ch.ch_status = 'd'
  and ch.ch_seqno = (select max(ch1.ch_seqno)
  from contract_history ch1
  where ch1.co_id = ch.co_id
  and ch1.ch_validfrom <= to_date('161201', 'RRMMDD'))
  )
  or
  exists
  (select 1
  from contract_all co,
  contract_history ch
  where co.customer_id = ca.customer_id
  and co.co_id = ch.co_id
  and ch.ch_status != 'd'
  and ch.ch_seqno = (select max(ch1.ch_seqno)
  from contract_history ch1
  where ch1.co_id = ch.co_id
  and ch1.ch_validfrom <= to_date('161201', 'RRMMDD'))
  )
  )
union
select oh.customer_id,
  'D' customer_type,
  ca.tmcode
  from orderhdr_all oh,
  customer_all ca
 where oh.ohentdate >= add_months(to_date('161201', 'RRMMDD'), -1)
  and oh.ohentdate < to_date('161201', 'RRMMDD')
  and oh.ohopnamt != 0
  and oh.ohinvtype != 5
  and oh.customer_id = ca.customer_id
  and ca.billcycle = 01
  and ca.csdeactivated <= to_date('161201', 'RRMMDD') - 90
;

-- Question:

Dear Billing team,

Please kindly provide HK, China min and HK, China data usage breakdown with below:

Customer code: 1.5769079

Requested inv. mth: Jun to Nov2016
查询分钟量及流量使用明细（每号码每月一条数据） ， 每月汇总。
SELECT
tm.tmcode,
tm.des,
lnk.TM_GROUP_ID,
Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') pool_fu,
a.custcode, a.co_id, a.msisdn, a.invoice_date, a.inter_voice_usage +a.intra_voice_usage, a.china_usage, Ceil(a.GPRS_USAGE/1024),Ceil(a.CHINA_GPRS_USAGE/1024)
FROM ptcbill_co_usage_summary a, contract_all co, customer_all ca, mputmview tm, ptcbill_rateplan_group_lnk lnk
WHERE a.custcode IN ('1.5070640')
AND a.invoice_date = To_Date('20161126','yyyymmdd')
AND a.co_id = co.co_id
AND co.customer_id = ca.customer_id
AND ca.tmcode = tm.tmcode
--AND Nvl((SELECT 'Y' FROM ptcbill_sub_psh_fu_cat WHERE sub_customer_id = ca.customer_id AND free_unit_cat_id = 16 AND EFF_BILL_DATE <= a.invoice_date AND (EXP_BILL_DATE IS NULL OR EXP_BILL_DATE > a.invoice_date)), 'N') = 'Y'
AND tm.tmcode = lnk.tmcode
ORDER BY msisdn
;
--Load OCC info
 select
 a.oc_id,
  b.description,
  a.gl_account,
  a.amount,
  a.amt_changeable,
  a.adjust_accessfee,
  a.accessfee_service,
  a.adjust_usagefee,
  a.usagefee_service,
  a.bill_times,
  a.allow_duplicate,
  a.carry_forward,
  a.bill_suspense,
  a.tmcode,
  a.vscode,
  to_char(a.valid_from,'yyyymmddhh24miss') valid_from,
  nvl(to_char(a.expiry_date,'yyyymmddhh24miss'),'99999999999999') expiry_date,
  a.accessfee_sncode,
  a.usagefee_sncode,
  nvl(a.usagefee_grp_id,-1) usagefee_grp_id,
  nvl(c.sumcode,-1) sumcode,
  nvl(b.proind,'N') proind,
  d.airland
  FROM other_credits a, master_occ b, usage_sum_group c, usage_sum_def d
WHERE c.sumcode = d.sumcode
AND a.oc_Id = b.oc_id
AND a.usagefee_grp_id = c.group_id(+)
;

--single customer
SELECT Ceil(unb_p_gprs_usg/60),Ceil(unb_p_roamgprs_usg/60),Ceil(unb_p_chn_roamgprs_usg/60)
FROM ptcapp_sub_usage psu, customer_all ca, contract_all co
WHERE psu.customer_id = ca.customer_id
AND co.customer_id = ca.customer_id
AND psu.co_id = co.co_id
AND ca.custcode = '2.11.52.64.100109';

--multi sub customers
SELECT ca.custcode, cs.tmcode, tm.des, dn.dn_num MSISDN, Nvl(Ceil(unb_p_gprs_usg/60),0) HK_DATA,
Nvl(Ceil(unb_p_roamgprs_usg/60),0)  ROAM_DATA,
Nvl(Ceil(unb_p_chn_roamgprs_usg/60),0) CHINA_DATA
FROM ptcapp_sub_usage psu, customer_all ca, ptcbill_main_sub_lnk l,contr_services cs, directory_number dn, mputmtab tm
WHERE psu.customer_id(+) = l.sub_customer_id
AND psu.co_id(+) = l.sub_co_id
AND ca.custcode = '1.5070640'
AND ca.customer_id = l.main_customer_id
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_id
AND SubStr(cs.cs_stat_chng , -1) IN ( 'a', 's')
AND l.eff_date < To_Date('20161201', 'yyyymmdd')
AND ( l.exp_date IS NULL OR l.exp_date > To_Date( '20161201', 'yyyymmdd'))
AND cs.tmcode = tm.tmcode
ORDER BY dn.dn_num
;

SELECT ca.custcode, psu.date_billed, cs.tmcode, tm.des, dn.dn_num MSISDN, Nvl(Ceil(gprs_usg/60),0) HK_DATA,
Nvl(Ceil(roamgprs_chn_usg/60),0) CHINA_DATA
FROM ptcapp_usage_hist psu, customer_all ca, ptcbill_main_sub_lnk l,contr_services cs, directory_number dn, mputmview tm
WHERE psu.customer_id(+) = l.sub_customer_id
AND psu.co_id(+) = l.sub_co_id
AND ca.custcode = '1.5070640'
AND ca.customer_id = l.main_customer_id
AND cs.co_id = l.sub_co_id
AND cs.dn_id = dn.dn_id
AND SubStr(cs.cs_stat_chng , -1) IN ( 'a', 's')
--AND l.eff_date < To_Date('20161201', 'yyyymmdd')
--AND ( l.exp_date IS NULL OR l.exp_date > To_Date( '20161201', 'yyyymmdd'))
AND cs.tmcode = tm.tmcode
AND psu.date_billed IN (To_Date('20160726', 'yyyymmdd'),
 To_Date('20160826', 'yyyymmdd'),
  To_Date('20160926', 'yyyymmdd'),
   To_Date('20161026', 'yyyymmdd'),
    To_Date('20161126', 'yyyymmdd'))
ORDER BY psu.date_billed, dn.dn_num
;

