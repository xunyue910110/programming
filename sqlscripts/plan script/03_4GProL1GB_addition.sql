/*INSERT INTO ptcbill_free_unit VALUES (10434, 421, -1, -1, 7, 1, 0, 7, 907006, 1, 41943040, 0, NULL, 'N', 'N');

DELETE FROM ptcbill_tm_free_unit WHERE tmcode = 537 AND pkg_id = 421 AND free_unit_id = 10405;
INSERT INTO ptcbill_tm_free_unit VALUES (537, 10434, To_Date('20120820','yyyymmdd'), NULL, 0);

UPDATE fuom_all SET inter_fuom = 41943040 WHERE tmcode = 537 AND sncode = 119 AND pkg_sncode = 421;
*/

VAR ref_tmcode NUMBER;
VAR new_tmcode NUMBER;
VAR tmdes VARCHAR2(30);
VAR tmshdes VARCHAR2(5);

BEGIN
SELECT 638, 721, '4G Local 1GB', '4GL1G'
INTO   :ref_tmcode, :new_tmcode, :tmdes, :tmshdes
FROM   DUAL;
END;
/
print

-- ============================================================================
-- ** *** Now release working to production *** ***
-- ============================================================================
-- -- -- -- --
-- release working to production
-- suppose working table mpulktm1/mpulktm2 were configurated correctly
-- -- -- -- --

   -- -- --
   -- get total records before release
   -- -- --
   select 'a: mpulktmb',  count(*) from mpulktmb;

   select 'b: mpulktmm',  count(*) from mpulktmm;

   select 'c: mpulktmt',  count(*) from mpulktmt;

   -- zone
   select 'd: mpulktmz',  count(*) from mpulktmz;

   -- inter free mins
   select 'e: mpufutab',  count(*) from mpufutab;

   -- intra free mins
   select 'f: mpuintab',  count(*) from mpuintab;

   -- mobile data (by circuit switch)
   select 'g: free_mins', count(*) from free_mins;

   -- linkage table, for checking if 2 packages with same nature co-exist
   select 'h: mpulktmc',  count(*) from mpulktmc;

   -- other than telephony and mobil data (e.g.MMS, SMS, etc)
   -- mpulktmm.umcode describe charging unit (per call/per event/per message, etc)
   select 'i: fuom_all',  count(*) from fuom_all;

   select 'j: mpucltab',  count(*) from mpucltab;

   select 'k: ptcbill_free_unit',  count(*) from ptcbill_free_unit;


/*** *** ***
-- modify ..\release_rateplan_Data256WiFi.sql and run
-- (in SQLPLUS)
-- mpulktm1 is the working table for mpulktmb
-- mpulktm2 is the working talbe for mpulktmm
-- after release_rateplan.sql is run, records in mpulktm1 and mpulktm2
--                             will be copied to mpulktmb and mpulktmm respectively
*** *** ***/


   -- -- --
   -- get total records after release
   -- -- --
   select 'a: mpulktmb',  count(*) from mpulktmb;

   select 'b: mpulktmm',  count(*) from mpulktmm;

   select 'c: mpulktmt',  count(*) from mpulktmt;

   select 'd: mpulktmz',  count(*) from mpulktmz;

   select 'e: mpufutab',  count(*) from mpufutab;

   select 'f: mpuintab',  count(*) from mpuintab;

   select 'g: free_mins', count(*) from free_mins;

   select 'h: mpulktmc',  count(*) from mpulktmc;

   select 'i: fuom_all',  count(*) from fuom_all;

   select 'j: mpucltab',  count(*) from mpucltab;

   select 'k: ptcbill_tm_free_unit',  count(*) from ptcbill_tm_free_unit;

-- -- --
-- Check if the count match with other tmcode
-- -- --

SELECT tmcode, count(*) from mpulktmb
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) from mpulktmm
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) from mpulktmt
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) from mpulktmz
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) FROM mpufutab
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) FROM mpuintab
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) FROM free_mins
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) FROM mpulktmc
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) FROM fuom_all
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) FROM mpucltab
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

SELECT tmcode, count(*) FROM ptcbill_tm_free_unit
 where tmcode in (:ref_tmcode, :new_tmcode) group by tmcode;

-- ============================================================================
-- *** *** *** Now set free mins (inter) *** *** ***
-- ============================================================================
/*
============================================================================
                                         mpufutab |        mpuintab
                                         fuom     | fuom     on_top  besides
                                         -------- | -------- ------  -------
-- inter/intra independent               ???      | ???      NULL    'A'
-- basic                                 ???      | N/A      N/A     N/A
-- inter remain offset intra overflow    ???      | ???      NULL    'C'
-- used up inter fuom before intra fuom  ???      | ???      'X'     NULL
-- intra remain offset inter overflow    ???      | ???      NULL    'B'
                                                  |
-- no fuom                               0        | 0        NULL    'A'
-- free inter only and charge intra      ???      | 0        NULL    'A'
-- free intra only and charge inter      0        | ???      NULL    'A'

============================================================================
*/

-- -- --
-- MPUFUTAB
-- meaning of umcode is defined in mpsumtab
-- charge unit = per minute ==> mpufutab.umcode = 5
-- mpufutab.fuom = no. of free mins (inter)
-- -- --

VAR inter_fmin NUMBER;
BEGIN
SELECT 0
INTO   :inter_fmin
FROM   DUAL;
END;

print inter_fmin

SELECT tmcode, umcode, fuom
FROM  mpufutab
WHERE tmcode in  (:new_tmcode, :ref_tmcode);

UPDATE mpufutab
SET    fuom = :inter_fmin
WHERE  tmcode = :new_tmcode;

-- -- --
-- MPUINTAB
-- mpuintab.besides = 'A/B/C': inter and intra calls are counted separately. mins left in inter
-- mpuintab.on_top = 'X': all calls are counted as inter call first,
-- -- --

VAR intra_fmin NUMBER;
VAR beside     VARCHAR2(1);
BEGIN
SELECT 0, 'A'
INTO   :intra_fmin, :beside
FROM   DUAL;
END;
/
print intra_fmin
print beside


SELECT *
FROM  mpuintab
WHERE tmcode in ( :new_tmcode, :ref_tmcode);

UPDATE mpuintab
SET    fuom = :intra_fmin, besides = :beside
WHERE  tmcode = :new_tmcode;

-- -- --
-- check mpufutab mpuintab --
-- -- --
SELECT fu.tmcode
     , fu.fuom as Inter
     , ita.fuom as Intra
     , (fu.fuom + nvl(ita.fuom,0)) total_min
     , on_top, besides
  FROM mpufutab fu, mpuintab ita
 WHERE fu.tmcode = ita.tmcode(+)
   AND fu.spcode = ita.spcode(+)
   AND fu.sncode = ita.sncode(+)
   AND fu.tmcode in (:new_tmcode, :ref_tmcode)
 order by tmcode
;

------------------------------------------------
-- check PTCBILL_FREE_UNIT, PTCBILL_TM_FREE_UNIT
------------------------------------------------

-- if no existing free_unit_id match the UR, create new free_unit

/*VAR new_free_unit_id NUMBER;
BEGIN
  SELECT MAX(free_unit_id)+1
  INTO   :new_free_unit_id
  FROM   ptcbill_free_unit;
END;
/
print new_free_unit_id;

INSERT INTO ptcbill_free_unit
SELECT :new_free_unit_id, pkg_id, zone_group, time_group, rtx_type_group, call_type, offset_type
, roam_group, cat_group, uom, 3145728, free_unit_intra
, use_credit_tx, 'N', carry_forward
FROM ptcbill_free_unit
WHERE free_unit_id = 10445
;*/

INSERT INTO ptcbill_tm_free_unit
SELECT :new_tmcode, free_unit_id, To_Date('20160104','yyyymmdd'), NULL, priority
FROM  ptcbill_tm_free_unit
WHERE tmcode = :ref_tmcode
;


-- then update ptcbill_tm_free_unit
/*select * from ptcbill_tm_free_unit
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id = 1);

delete ptcbill_tm_free_unit
where free_unit_id = 10512
and tmcode = :new_tmcode;

delete ptcbill_tm_free_unit
where free_unit_id = 97
and tmcode = :new_tmcode;


insert into ptcbill_tm_free_unit
values
(:new_tmcode,10445,To_Date('20160104','yyyymmdd'), NULL,0);*/

/*DELETE ptcbill_tm_free_unit
WHERE TMCODE IN 605 AND free_unit_id = 10449;*/

/*INSERT INTO ptcbill_tm_free_unit
VALUES
(:new_tmcode,97,To_Date('20140206','yyyymmdd'), NULL,0);*/

/*UPDATE ptcbill_tm_free_unit
SET    free_unit_id = :new_free_unit_id
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id = 481);*/



-----------------
--Video PACKAGE
-----------------

/* basic */
/*********kei
UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10076
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id = 189);
*************/

/* gold */
/*********kei
UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10085
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id = 190);
*************/

/* platinum */
/*********kei
UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10094
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id = 191);
*************/

/* diamond */
/*********kei
UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10103
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id = 192);
*************/

--Create new service package and then assign it to repalce an existing service package.

SELECT *
FROM  mpusptab
WHERE spcode=214;

VAR inter_fmin NUMBER;
VAR inter_number NUMBER;

BEGIN
SELECT 0, 214
INTO   :inter_fmin, :inter_number
FROM   DUAL;
END;
/

print inter_fmin
print inter_number

SELECT tmcode, umcode, fuom
FROM  mpufutab
WHERE tmcode in  (:new_tmcode, :ref_tmcode);

UPDATE mpufutab
SET    fuom = :inter_fmin, spcode = :inter_number
WHERE  tmcode = :new_tmcode;

VAR intra_fmin NUMBER;
VAR beside     VARCHAR2(1);
VAR intra_spcode NUMBER;

BEGIN
SELECT 0, 'A', 214
INTO   :intra_fmin, :beside, :intra_spcode
FROM   DUAL;
END;
/

print intra_fmin
print beside
print intra_spcode

SELECT *
FROM  mpuintab
WHERE tmcode in ( :new_tmcode, :ref_tmcode);

UPDATE mpuintab
SET    fuom = :intra_fmin, besides = :beside, spcode = :intra_spcode
WHERE  tmcode = :new_tmcode;

/*********kei
*************/
/*
INSERT INTO ptcbill_tm_free_unit
SELECT 541, free_unit_id, To_Date('20130122','yyyymmdd'), expiry_date, priority
FROM ptcbill_tm_free_unit
WHERE tmcode = 334
;

-- checking
select a.*, b.* from ptcbill_tm_free_unit a, ptcbill_free_unit b
where a.free_unit_id = b.free_unit_id
and  b.pkg_id in (1,189,190,191,192,237)
and  a.tmcode = :new_tmcode;
*/

-- ============================================================================
-- *** *** Now set MPULKTMC *** ***
-- ============================================================================
-- -- --
-- MPULKTMC is a linkage table, for checking if 2 packages with same nature co-exist
-- -- --

-- -- --
-- get mis-match spcode
-- -- --
SELECT * FROM mpulktmc
 WHERE (tmcode, spcode, sncode) NOT IN (select tmcode, spcode, sncode
                                          FROM mpulktmb)
;

   -- -- --
   -- update mis-match spcode(if any)
   -- -- --
   /*
   UPDATE mpulktmc tmc
      SET spcode = (SELECT spcode
                      FROM mpulktmb
                     WHERE tmcode = tmc.tmcode
                       AND sncode = tmc.sncode)
    WHERE (tmcode, spcode, sncode) NOT IN (select tmcode, spcode, sncode FROM mpulktmb)
   ;
   */

-- -- --
-- find missing dummy package
-- (dummy package: select sncode from mpusntab where psncode > 0;)
-- sncode 201:#SMS Pkg CorpS20400(Inter0.35) for tmcode 241: CSMS0 only
-- -- --
/*
select decode(b.sncode,201,'OK - sn 201 for tm 241 only','MISSING') remark
     , b.tmcode, b.spcode, b.sncode, sp.des as sp_des, sn.des as sn_des
  from mpulktmb b, mpusptab sp, mpusntab sn
 where b.spcode = sp.spcode
   and b.sncode = sn.sncode
   and not exists (select 1 from mpulktmc
                    where sncode = b.sncode
                      and tmcode = b.tmcode)
   and b.sncode in (select sncode from mpusntab where psncode > 0)
   and b.tmcode in (:new_tmcode)
;
*/

   -- -- --
   -- insert for missing(if any, except sncode 201);
   -- -- --
   /*
   insert into mpulktmc
   select tmcode, spcode, sncode, m.grp_id
   from mpulktmb b, sms_group m
   where not exists (select 1 from mpulktmc
                      where grp_id = m.grp_id
                        and sncode = b.sncode
                        and tmcode = b.tmcode)
   and sncode in (select sncode from mpusntab where psncode > 0)
   and grp_id = :new_spcode
   and sncode in (278, 279, 280)
   AND tmcode = :new_tmcode
   ;
   */

-- ============================================================================
-- *** *** Now set FUOM_ALL and PTCBILL_FREE_UNIT, PTCBILL_TM_FREE_UNIT *** ***
-- ============================================================================
-- -- --
-- This part target pkg_sncode/sncode (189, 190, 191, 192, 206, 207, 208, 209);
-- -- --
   -- -- --
   --        SNCODE DES
   --    ========== ==============================
   --           189 12-mth free basic video pkg
   --           190 12-mth free gold video pkg
   --           191 12-mth free platinum video pkg
   --           192 12-mth free diamond video pkg
   --           206 MobileEyeFreeVideoMin-Basic
   --           207 MobileEyeFreeVideoMin-Gold
   --           208 MobileEyeFreeVideoMin-Platinum
   --           209 MobileEyeFreeVideoMin-Diamond
   -- -- --


-- -- -- --
-- FUOM_ALL - FUOM setting for service then telephony & GPRS
-- *
-- Loyalty package with differ fuom refer to Loyalty level & rateplan price
-- eg.               VIP     Gold     Platinum   Diamond
--                           (+10%)   (+15%)     (+20%)
-- $0 - $49            0       0        0          0
-- $50 - $67          40      44       46         48
-- $68 - $79          40      44       46         48
-- $80 - $107         60      66       69         72
-- $108 - $137        60      66       69         72
-- $138 - $187        80      88       92         96
-- $188 - $287       120     132      138        144
-- $288 - $387       160     176      184        192
-- $388 - $487       480     528      552        576
-- $488 or above     800     880      920        960
-- -- -- --


-- -- --
-- find rateplan with same montly fee for reference (exact monthly fee or same range)
-- -- --
--select tm.des, b.*
--  from mpulktmb b, mputmview tm
-- where b.tmcode = tm.tmcode
--   and (accessfee, subscript) =
--       (select accessfee, subscript
--          from mpulktmb
--         where sncode = b.sncode
--           and tmcode = :new_tmcode
--        )
--   and sncode = 1
--;
--   and rownum = 1
--;
--select tm.des, b.*
--  from mpulktmb b, mputmview tm
-- where b.tmcode = tm.tmcode
--   and accessfee between 0 and 0
--   and subscript = 0
--   and sncode = 1
--   and tm.tmcode in (select distinct tmcode from fuom_all where pkg_sncode in (1, 189, 190, 191, 192, 206, 207, 208, 209))
--   --and (tm.des like 'BS%' or tm.des like '%BH%')
--;
-- /*==> reference tmcode 334 (DATA98)*/
VAR fuom_ref_tmcode NUMBER;
BEGIN
SELECT 721
INTO   :fuom_ref_tmcode
FROM DUAL;
END;
/
print fuom_ref_tmcode

-- -- --
-- check against with existing(same monthly fee)
-- -- --
select tmcode, a.SNCODE, b.DES, PKG_SNCODE, c.DES, GRP_ID,CALL_TYPE,INTER_FUOM,INTRA_FUOM
  from fuom_all a, mpusntab b, mpusntab c
 where tmcode in (:fuom_ref_tmcode)
 and   a.sncode = b.sncode
 and   a.pkg_sncode = c.sncode
 minus
select tmcode, a.SNCODE, b.DES, PKG_SNCODE, c.DES, GRP_ID,CALL_TYPE,INTER_FUOM,INTRA_FUOM
  from fuom_all a, mpusntab b, mpusntab c
 where tmcode in (:new_tmcode)
 and   a.sncode = b.sncode
 and   a.pkg_sncode = c.sncode
;

select *
from fuom_all
where pkg_sncode in (189, 190, 191, 192, 206, 207, 208, 209)
and tmcode in (:new_tmcode, :fuom_ref_tmcode)
order by pkg_sncode, tmcode
;


-- -- -- -- --
-- according to UR:
-- 189 12-mth free basic video pkg
-- 190 12-mth free gold video pkg
-- 191 12-mth free platinum video pkg
-- 192 12-mth free diamond video pkg
--
-- -- -- -- --
/**********kei
UPDATE fuom_all
SET    inter_fuom = 400
WHERE  pkg_sncode = 189
AND    tmcode = :new_tmcode;

UPDATE fuom_all
SET    inter_fuom = 440
WHERE  pkg_sncode = 190
AND    tmcode = :new_tmcode;

UPDATE fuom_all
SET    inter_fuom = 460
WHERE  pkg_sncode = 191
AND    tmcode = :new_tmcode;

UPDATE fuom_all
SET    inter_fuom = 480
WHERE  pkg_sncode = 192
AND    tmcode = :new_tmcode;
************/
select * from fuom_all where pkg_sncode between 189 and 192 and grp_id = 6002 and tmcode = :new_tmcode;
/*
INSERT INTO fuom_all
VALUES
(:new_tmcode, 1, 1, To_Date('20111201','yyyymmdd'), NULL, 119, 334, 7001, 1, 204800, 0, 1, 0, 'N')
;

DELETE fuom_all
WHERE pkg_sncode IN (SELECT sncode FROM mpulktmb WHERE tmcode = :new_tmcode AND spcode = 198)
AND tmcode = :new_tmcode
;
*/

/*
	update fuom_all
	set inter_fuom = 500
	where tmcode = 335
	and pkg_sncode = 189
	and grp_id = 6002;

	update fuom_all
	set inter_fuom = 550
	where tmcode = 335
	and pkg_sncode = 190
	and grp_id = 6002;

	update fuom_all
	set inter_fuom = 575
	where tmcode = 335
	and pkg_sncode = 191
	and grp_id = 6002;

	update fuom_all
	set inter_fuom = 600
	where tmcode = 335
	and pkg_sncode = 192
	and grp_id = 6002;
*/
select * from fuom_all where pkg_sncode between 189 and 192 and grp_id = 6002 and tmcode = :new_tmcode;


/******** check ptcbill_tm_free_unit and ptcbill_free_unit *************/

select a.*, b.* from ptcbill_tm_free_unit a, ptcbill_free_unit b
where a.free_unit_id = b.free_unit_id
and  b.pkg_id in (189, 190, 191, 192)
and  a.tmcode = :new_tmcode;
--==> not match


/*
-- if no existing free_unit_id match the UR, create new free_unit
VAR new_free_unit_id NUMBER;
BEGIN
  SELECT MAX(free_unit_id)+1
  INTO   :new_free_unit_id
  FROM   ptcbill_free_unit;
END;
/
print new_free_unit_id;

INSERT INTO ptcbill_free_unit
(free_unit_id, pkg_id, zone_group, time_group, rtx_type_group, call_type, offset_type
, roam_group, cat_group, uom, free_unit_inter, free_unit_intra
, use_credit_tx, prorate_ind, carry_forward)
VALUES
(:new_free_unit_id, <sncode>, 1, 1, 1, -1, 1
 , 1, -1, 1, 600, 600
 , 'N', 'N', 'N')
;
*/

-- then update ptcbill_tm_free_unit
select * from ptcbill_tm_free_unit
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id in (189, 190, 191, 192));
-- 10077 <- 10075 - 189
-- 10086 <- 10084 - 190
-- 10095 <- 10093 - 191
-- 10104 <- 10102 - 192
/*
UPDATE ptcbill_tm_free_unit
SET    free_unit_id = :new_free_unit_id
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (SELECT free_unit_id FROM ptcbill_free_unit WHERE pkg_id = <sncode>);
*/
/*
UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10077
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (10075);


UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10086
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (10084);

UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10095
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (10093);


UPDATE ptcbill_tm_free_unit
SET    free_unit_id = 10104
WHERE  tmcode = :new_tmcode
AND    free_unit_id IN (10102);
*/

-- checking
select a.*, b.* from ptcbill_tm_free_unit a, ptcbill_free_unit b
where a.free_unit_id = b.free_unit_id
and  b.pkg_id in (189, 190, 191, 192)
and  a.tmcode = :new_tmcode;

-- For mobile eye:
-- 206 MobileEyeFreeVideoMin-Basic
-- 207 MobileEyeFreeVideoMin-Gold
-- 208 MobileEyeFreeVideoMin-Platinum
-- 209 MobileEyeFreeVideoMin-Diamond
--
-- DATA98-WiFi - 120/132/138/144


/*
UPDATE fuom_all
SET    inter_fuom = 120
WHERE  pkg_sncode = 206
AND    tmcode = :new_tmcode;

UPDATE fuom_all
SET    inter_fuom = 132
WHERE  pkg_sncode = 207
AND    tmcode = :new_tmcode;

UPDATE fuom_all
SET    inter_fuom = 138
WHERE  pkg_sncode = 208
AND    tmcode = :new_tmcode;

UPDATE fuom_all
SET    inter_fuom = 144
WHERE  pkg_sncode = 209
AND    tmcode = :new_tmcode;
*/

/******** check ptcbill_tm_free_unit and ptcbill_free_unit *************/
select a.*, b.* from ptcbill_tm_free_unit a, ptcbill_free_unit b
where a.free_unit_id = b.free_unit_id
and  b.pkg_id in (189, 190, 191, 192)
and  a.tmcode = :new_tmcode;

/*

-- -- --
-- if the setting copied from reference rateplan does not suit this rateplan
-- then update inter_fuom accordingly
-- -- --
   update fuom_all
      set inter_fuom = 99999999/0/??
        , intra_fuom = 0
    where pkg_sncode = 164
      and tmcode in (:new_tmcode);
*/

-- -- --
-- check fuom_all
-- -- --
select c.tmcode, tmb.sncode, sn.des, tmb.accessfee
     , fa.grp_id, sg.des as sg_des
     , fa.inter_fuom, fa.intra_fuom, fa.umcode, fa.call_type, psn.des as psn
  from mpusntab sn, mpulktmb tmb, mpulktmc c, sms_group sg, fuom_all fa, mpusntab psn
 where sn.sncode = tmb.sncode
   and tmb.tmcode = c.tmcode
   and tmb.spcode = c.spcode
   and tmb.sncode = c.sncode
   and c.grp_id = sg.grp_id
   and c.tmcode = fa.tmcode
   and c.sncode = decode(fa.pkg_sncode,0,fa.sncode,fa.pkg_sncode)
   and c.grp_id = fa.grp_id
   and fa.sncode = psn.sncode
   and tmb.tmcode in (:ref_tmcode, :new_tmcode)
 order by c.sncode, c.tmcode, c.grp_id
;

-- ============================================================================
-- *** *** Now define Tariff package for old POS *** ***
-- ============================================================================

-- -- --
-- As told, the following tables are for old POS use.
-- 1. MPUTPTAB; 2. MPULKTP1; 3. MPULKTPB
-- Record(s) need to be inserted into these table so that the new created rateplan could be displayed in old POS
-- -- --

-- -- --
-- 1. MPUTPTAB
-- -- --
   select * from mputmtab
    where tmcode in (:new_tmcode)
    order by tmcode;

   select * from mputptab
    where tpcode = (select max(tpcode) from mputptab)
    order by tpcode desc;

insert into mputptab (tpcode,vscode,vsdate,status,des,shdes,rec_version)
select tpcode + ceil(rownum/2), tp.vscode, to_date('20140206','yyyymmdd')
     , tp.status, tm.des || ' Pkg', tm.shdes, 0
  from mputptab tp, mputmview tm
 where tpcode = (select max(tpcode) from mputptab)
   and tm.tmcode in (:new_tmcode)
 order by tm.tmcode, tpcode, tp.vscode
;

-- -- --
-- 2. MPULKTP1
-- -- --
   select * from mpulktp1
    where tpcode = (select max(tpcode) from mpulktp1)
    order by tpcode desc;

insert into mpulktp1 (tpcode, vscode, vsdate, status, seqno, tmcode, spcode, rec_version)
select tpcode, tp.vscode, tp.vsdate, tp.status, 1 as seqno, tmb.tmcode, tmb.spcode, 0
/*     , tp.des as tp_des, tm.des as tm_des */
  from mputptab tp, mputmview tm, mpulktmb tmb
 where tp.des = tm.des || ' Pkg'
   and tm.tmcode = tmb.tmcode
   and tmb.sncode = 1
   and tp.status = 'W'
   and tm.tmcode in (:new_tmcode)
 order by tm.tmcode, tpcode, tp.vscode
;

-- -- --
-- 3. MPULKTPB
-- -- --
   select * from mpulktpb
    where tpcode = (select max(tpcode) from mpulktpb)
    order by tpcode desc;

insert into mpulktpb (tpcode, vscode, vsdate, status, seqno, tmcode, spcode, rec_version)
select tpcode, tp.vscode, tp.vsdate, tp.status, 1 as seqno, tmb.tmcode, tmb.spcode, 0
/*     , tp.des as tp_des, tm.des as tm_des */
  from mputptab tp, mputmview tm, mpulktmb tmb
 where tp.des = tm.des || ' Pkg'
   and tm.tmcode = tmb.tmcode
   and tmb.sncode = 1
   and tp.status = 'P'
   and tm.tmcode in (:new_tmcode)
 order by tm.tmcode, tpcode, tp.vscode
;

-- -- --
-- check mputptab mpulktp1 mpulktpb
-- -- --

select lk.*, tp.*
  from mputptab tp, mpulktp1 lk
 where tp.tpcode = lk.tpcode
   and tp.vscode = lk.vscode
   and lk.tmcode in (:ref_tmcode, :new_tmcode);

-------------------
-- SET UP OCC
------------------

INSERT INTO other_credits
SELECT 719, oc_id, vscode, gl_account, amount, amt_changeable, adjust_accessfee, accessfee_service, adjust_usagefee, usagefee_service, bill_times, allow_duplicate, carry_forward, bill_suspense, valid_from, accessfee_sncode, usagefee_sncode,
usagefee_grp_id, expiry_date, expiry_bill_times, accessfee_spcode
FROM other_credits
WHERE tmcode = 605;

--DELETE FROM other_credits WHERE tmcode = 541 AND amount <= 0 AND accessfee_service IN ('A', 'T');

/*
UPDATE other_credits
SET amount = 0
WHERE (adjust_accessfee = 'Y' OR
       (adjust_usagefee = 'Y' AND usagefee_service != 'T' AND usagefee_service != 'I') OR
       (adjust_usagefee = 'Y' AND usagefee_service = 'I' AND usagefee_sncode != 1)
      )
AND tmcode = 506
;
*/


/***************************
  Miscellanous table
***************************/
-- ============================================================================
-- ptcbpp_cfg_rateplan - rateplan bill description;
-- ============================================================================
SELECT * FROM ptcbpp_cfg_rateplan
WHERE tmcode in (select max(tmcode)-1 from mputmtab)
;
SELECT * FROM ptcbpp_cfg_rateplan
WHERE tmcode in (:ref_tmcode, :new_tmcode)
;

-- -- -- --
-- need to update rateplan bill description
-- -- -- --
SELECT * FROM ptcbpp_cfg_Rateplan WHERE tmcode in (:new_tmcode, :ref_tmcode);

-- -- -- --
-- end update rateplan bill description
-- -- -- --

--check bill desc ptcbpp_cfg_rateplan--;
SELECT *
  FROM ptcbpp_cfg_rateplan
 WHERE source <> edesc
   AND source <> cdesc
   AND tmcode in (:new_tmcode)
;

SELECT *
  FROM ptcbpp_cfg_rateplan
   where tmcode in (:new_tmcode, :ref_tmcode)
;

/***************************
  POS table
***************************/
-- ============================================================================
-- ptcapp_rateplan
-- ============================================================================
SELECT * FROM ptcapp_rateplan
 where tmcode in (:new_tmcode)
;

   -- -- --
   -- delete record if tmcode reuse
   -- -- --
   /* not applicable to this rateplan *
   delete from ptcapp_rateplan
    where tmcode in (&new_tm_270);
   * end not applicable to this rateplan */

select * from ptcapp_rateplan
 where tmcode = (select max(tmcode) from ptcapp_rateplan);

INSERT INTO ptcapp_rateplan(rateplan_code,rateplan_name,tmcode,rateplan_rank,free_minute)
SELECT pr.rateplan_code + 1
     , tm.des
     , tm.tmcode
     , rk.rateplan_rank
     , nvl(fu.fuom,0) + /*nvl(ita.fuom,0)*/0 ttl_free_min
  FROM mpufutab fu, mpuintab ita, mputmtab tm, ptcapp_rateplan pr, ptcapp_rateplan rk
 WHERE fu.tmcode = ita.tmcode(+)
   AND fu.tmcode = tm.tmcode
/*
   AND rk.rateplan_rank = (select max(rateplan_rank) FROM ptcapp_rateplan where free_minute = (nvl(fu.fuom,0) + nvl(ita.fuom,0)))
*/
   AND pr.rateplan_code = (select max(rateplan_code) from ptcapp_rateplan)
   AND tm.status = 'P'
   AND fu.tmcode = :new_tmcode
   AND rownum = 1
;
-- must insert 1 record!!!


-- check ptcapp_rateplan --;
SELECT * FROM ptcapp_rateplan
 where free_minute = (select free_minute from ptcapp_rateplan
                       where tmcode in (:new_tmcode))
order by rateplan_code
;

-- ============================================================================
-- ptcapp_curr_rateplan;
-- ============================================================================
SELECT * FROM ptcapp_curr_rateplan
 where tmcode in (:new_tmcode)
;

   -- -- --
   -- delete record if tmcode reuse
   -- -- --
   /* not applicable to this rateplan *
   delete from ptcapp_curr_rateplan
    where tmcode in (:new_tmcode);
   * end not applicable to this rateplan */

select * from ptcapp_curr_rateplan
 where tmcode = (select max(tmcode) from ptcapp_curr_rateplan);

INSERT INTO ptcapp_curr_rateplan(tmcode, des, type)
select distinct tmcode, des, NULL
  from mputmtab tm
 where not exists (select 'x' from ptcapp_curr_rateplan ctm where ctm.tmcode = tm.tmcode and ctm.des = tm.des)
   and tmcode in (:new_tmcode);

-- check ptcapp_curr_rateplan --;

SELECT ctm.*, tm.tmcode, tm.des
  FROM ptcapp_curr_rateplan ctm, mputmview tm
 where ctm.tmcode(+) = tm.tmcode
   and tm.tmcode in (:new_tmcode)
 order by tm.tmcode desc;

-- ============================================================================
-- Insert for Service rule for New POS
-- ============================================================================
select * from pos_service_rule
 where tmcode = (select max(tmcode) from pos_service_rule)
 order by tmcode desc
;
-------------------------;
-- Add for Main Package
-------------------------;
select * from pos_service_rule
where tmcode in (:ref_tmcode, :new_tmcode);

INSERT INTO POS_SERVICE_RULE (tmcode,spcode,sncode,confirm_dialog
                             , assoc_sncode,dflt_service,deact_allowed
                             , act_allowed, customer_type, clear_cs_on_cbb, rule_id)
select b.tmcode, decode(b.sncode,1,b.spcode,null) as spcode, r.sncode
     , confirm_dialog, assoc_sncode, dflt_service, deact_allowed
     , act_allowed, customer_type, clear_cs_on_cbb, (SELECT Max(rule_id) FROM POS_SERVICE_RULE)+rownum
  from mpulktmb b, pos_service_rule r
 where b.sncode = nvl(r.sncode,1)
   and not exists (select 1 from pos_service_rule where tmcode = b.tmcode and nvl(sncode,1) = b.sncode)
   and b.sncode != 21
   and r.tmcode = :ref_tmcode
   and b.tmcode in (:new_tmcode)
;
-- must have 1(or more) record inserted
-- if no rows selected ==> ERROR!!

   ----------------------------------;
   -- Add for Voice Mail (Optional)
   ----------------------------------;

   -- insert if Voice Mail Services included in the VAS main package for free;
   -- purpose to set the sncode default as deactiveated;
INSERT INTO POS_SERVICE_RULE (tmcode,spcode,sncode,confirm_dialog
                            , assoc_sncode,dflt_service,deact_allowed
                            , act_allowed, customer_type, clear_cs_on_cbb,rule_id)
select b.tmcode, decode(b.sncode,1,b.spcode,null) as spcode, r.sncode
    , confirm_dialog, assoc_sncode, dflt_service, deact_allowed
    , act_allowed, customer_type, clear_cs_on_cbb,(SELECT Max(rule_id) FROM POS_SERVICE_RULE)+rownum
  from mpulktmb b, pos_service_rule r
where b.sncode = nvl(r.sncode,1)
  and not exists (select 1 from pos_service_rule where tmcode = b.tmcode and nvl(sncode,1) = b.sncode)
  and b.sncode = 21
  and b.accessfee = 0
  and r.tmcode = (select max(tmcode) from POS_SERVICE_RULE where sncode = 21)
  and b.tmcode in (:new_tmcode)
;



-- check pos_service_rule --;

select * from pos_service_rule
where tmcode in (:ref_tmcode, :new_tmcode)
order by tmcode, spcode, sncode
;

/*
INSERT INTO ptcbill_rateplan_group
VALUES
(6, '3G Data Plan', SYSDATE, 'P2168')
;*/


/*INSERT INTO ptcbill_rerate_price VALUES
(119, 1, 1, 30, 62914560, Trunc(SYSDATE), NULL, 4, -1,0,0,NULL);*/


INSERT INTO ptcbill_rateplan_group_lnk
VALUES
(7, :new_tmcode, SYSDATE, 'P3730', 6 , 'A', NULL, 1,null,null)
;

INSERT INTO ptcbpp_cfg_rateplan
VALUES
(721, '4G Local 1GB', '4G Local 1GB', '4G Local 1GB', '(2000)', 2000, 0, To_Date('20160104','yyyymmdd'),'P3730', NULL, 10, 6, '(2000基本分鐘/1GB本地數據)', '(2000 Basic Min/1GB Local Data)', To_Date('20160104','yyyymmdd'))
;

/*
INSERT INTO ptcbill_special_precision_cfg
values
(547, 119, 1, 4, -1)
;

INSERT INTO ptcbill_special_precision_cfg
values
(547, 119, 76, 4, -1)
;
*/