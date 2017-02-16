/*
INSERT INTO ptcbill_data_amt_display
VALUES
(537, 119, 'A', 'N', -1, 1)
;

INSERT INTO ptcbill_data_amt_display
VALUES
(537, 119, 'R', 'N', -1, 76)
;

*/
INSERT INTO mbsadm.ptcrate_check_ad_tmcode
VALUES
(721, To_Date('20160104','yyyymmdd'))
;


INSERT INTO PTCRATE_RFH_TM_GRP
VALUES
(2, 721, SYSDATE)
;

/*
INSERT INTO ptcbill_usg_conversion_from values
(2, 537, 119, 'R', 76, -1, -1)
;
*/

INSERT INTO ptcbill_tm_daily_max_charge
(
SELECT 721, pkg_sncode, daily_max_charge_id, prorate, priority, effective_date, expiry_date
FROM ptcbill_tm_daily_max_charge WHERE tmcode = 633 AND daily_max_charge_id = 7
);

INSERT INTO ptcbill_tm_daily_max_charge
(
SELECT 721, pkg_sncode, daily_max_charge_id, prorate, priority, effective_date, expiry_date
FROM ptcbill_tm_daily_max_charge WHERE tmcode = 633 AND daily_max_charge_id = 7
);

INSERT INTO ptcbill_tm_max_charge
(
SELECT 721,PKG_SNCODE,MAX_CHARGE_ID,sysdate,expiry_date From ptcbill_tm_max_charge where tmcode=633
);

INSERT INTO mbsadm.PTCBILL_OCC_MINS_DISPLAY
(
SELECT OC_ID,721,SNCODE,RTX_TYPE,FREE_MINS From mbsadm.PTCBILL_OCC_MINS_DISPLAY where tmcode=638
);

INSERT INTO mbsadm.ptcapp_occ_free_mins 
(
select oc_id, 721, free_mins, sysdate, 'P3730' from mbsadm.ptcapp_occ_free_mins where tmcode = 638
);