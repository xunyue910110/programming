-- ============================================================================--
-- create new tmcode
-- ============================================================================
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

INSERT INTO mputmtab
(tmcode, vscode, vsdate, status, tmind, des, shdes, tmrc, rec_version, default_qos, echannel,tier_type)
VALUES
(:new_tmcode, 0, to_date('20160104','YYYYMMDD'), 'W', '4G', :tmdes, :tmshdes, 0, 0, '4G-5GB-LC', 'sms',1);

-- -- -- --
-- check mputmtab
-- should be only 1 rec with vscode=0 at this moment
-- -- -- --

select * from mputmtab
 where tmcode in (:new_tmcode, :ref_tmcode)
 order by tmcode desc, vscode;

-- ============================================================================
-- *** *** Now setup MPULKTM1 (working table of mpulktmb) (Accessfee) *** ***
-- ============================================================================
-- -- -- --
-- copy from the best matched rateplan (tmcode 334)
-- -- -- --
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
   and ref.tmcode = :ref_tmcode
   and tm.tmcode in (:new_tmcode);

-- -- --
-- check MPULKTM1
-- should have same no. of records as reference  tmcode
-- -- --
SELECT tmcode, vscode, count(*) FROM mpulktm1
 WHERE tmcode IN (:ref_tmcode, :new_tmcode)
 GROUP BY tmcode, vscode;

   -- check sncode unmatch with mpulkpxn;
/*
   select spcode, sncode
     from mpulkpxn pxn
    where not exists (select 'x' from mpulktm1 b
                       where b.spcode = pxn.spcode
                         and b.sncode = pxn.sncode
                         and tmcode =  :new_tmcode)
      and spcode = :new_spcode
   ;
*/

select tm1.tmcode, tm.des, tm1.spcode, sp.des as spdes, tm1.sncode, sn.des as sndes
      , tm1.accessfee, tm1.subscript, tm1.proind, tm1.advind, tm1.susind
      , tm1.accglcode, tm1.subglcode, tm1.usgglcode, tm1.creind, tm1.creglcode
from mpulktm1 tm1, mpusptab sp, mpusntab sn , mputmtab tm
where tm1.tmcode in (:new_tmcode)
and tm1.tmcode = tm.tmcode
and tm.status = 'W'
and tm1.spcode=sp.spcode
and tm1.sncode=sn.sncode
order by tm1.tmcode, tm1.sncode;

-- ============================================================================
-- *** *** *** Now setup MPULKTM2 (USG) *** *** ***
-- ============================================================================
-- -- -- --
-- copy from the best matched rateplan (tmcode 334)
-- -- -- --

INSERT INTO mpulktm2
SELECT tm.tmcode, ref.vscode, To_Date('20160104','yyyymmdd'), ref.status, spcode, sncode, twcode, ttcode
     , gvcode, zncode, svlcode, splitind, uomind, umcode, usgind, typeind, rndind, rateind
     , minuom, maxuom, hrcode, il01, ir01, im01, ic01, il02, ir02, im02, ic02, il03, ir03, im03, ic03
     , il04, ir04, im04, ic04, il05, ir05, im05, ic05, il06, ir06, im06, ic06, il07, ir07, im07, ic07
     , il08, ir08, im08, ic08, il09, ir09, im09, ic09, il10, ir10, im10, ic10
     , ev_reg, ev_act, ev_dea, ev_int, ev_inv, ev_era, usgglcode, usgjcid, 0
  FROM mpulktm2 ref, mputmtab tm
WHERE tm.status = 'W'
   and not exists (select 1 from mpulktm2 where tmcode = tm.tmcode and sncode = 1)
   and ref.tmcode = :ref_tmcode
   and tm.tmcode in (:new_tmcode);

-- -- --
-- check MPULKTM2
-- should have same no. of records as reference  tmcode
-- -- --

SELECT tmcode, spcode, vscode, count(*) FROM mpulktm2
 WHERE tmcode IN (:ref_tmcode, :new_tmcode)
 GROUP BY tmcode, spcode, vscode
 ORDER BY spcode, vscode, tmcode
;
UPDATE mpulktm1
SET accessfee = 18
WHERE tmcode = :new_tmcode
AND sncode = 118

/*
UPDATE mpulktm1
SET spcode = 386
WHERE tmcode = :new_tmcode
AND sncode = 118
;

update mpulktm1 
set spcode = 387
where tmcode = :new_tmcode
and sncode = 85;*/

/*UPDATE mpulktm2
SET spcode = 214
WHERE tmcode = :new_tmcode
AND sncode = 29
;

UPDATE mpulktm1
SET ACCGLCODE = '4007601014', SUBGLCODE = '4007601014', USGGLCODE = '4007601014', CREGLCODE = '4007100206'
WHERE tmcode = :new_tmcode
AND sncode = 421
;

UPDATE mpulktm1
SET accglcode = '4007000493', subglcode = '4007000493', usgglcode = '4007000493', creglcode = '4007000493'
WHERE tmcode = :new_tmcode
AND sncode = 395
;

UPDATE mpulktm1
SET accglcode = '4007603620', subglcode = '4007603620', usgglcode = '4007603620', creglcode = '4007603620'
WHERE tmcode  = :new_tmcode
AND sncode = 399
;
*/

-- ============================================================================
-- *** *** Now UPDATE service fee MPULKTM1(ACC) *** *** ***
-- ============================================================================
-- -- --
-- browse service charge
-- -- --
   select
          tmb.tmcode, tm.des as tmdes
        , tmb.spcode, sp.des as spdes
        , tmb.sncode, sn.des as sndes
        , tmb.accessfee, tmb.subscript, tmb.event
 --       , ord.des as ur_sn_des
        , '$'||(tmb.accessfee + tmb.subscript) as mth_fee
        , tmb.accglcode, tmb.subglcode, tmb.usgglcode
        , tmb.creind, tmb.creglcode
  --      , ord.seq
     from mpulktm1 tmb, mpulkpxn a
        , mputmtab tm, mpusptab sp, mpusntab sn
--        , ama_sn_order ord
    where tmb.spcode = a.spcode
      and tmb.sncode = a.sncode
      and tmb.tmcode = tm.tmcode
      and tmb.vscode = tm.vscode
      and tmb.spcode = sp.spcode
      and tmb.sncode(+) = sn.sncode
      --and sn.sncode = ord.sncode(+)
      and tmb.tmcode in (:new_tmcode)
    order by tmb.tmcode, tmb.spcode, tmb.sncode
   ;

/*
-- -- --
-- get glcode
-- -- --
   select * from glaccount_all
    where gladesc like '%'||:tmdes||'%';

   select to_number(replace(des,'BS','')) as accessfee, gl.*
     from glaccount_all gl, mputmtab tm
    where instr(gladesc,tm.des) > 0
      and tm.tmcode in (:new_tmcode)
   ;
*/

-- -- -- --
-- set service fee
-- update according to UR
-- -- -- --

SELECT tmcode, accessfee
     , subscript
     , accglcode
     , subglcode
     , creind
     , creglcode
FROM  mpulktm1
 WHERE sncode = 1
   AND tmcode in (:new_tmcode, :ref_tmcode);

VAR accessfee NUMBER;
VAR subscript NUMBER;
VAR glacode VARCHAR2(30);
VAR creglcode VARCHAR2(30);
BEGIN
SELECT 218, 0, '4007000493', '4007000493'
INTO   :accessfee, :subscript, :glacode, :creglcode
FROM DUAL;
END;
/
print accessfee
print subscript
print glacode
print creglcode

UPDATE mpulktm1 b
   SET accessfee = :accessfee
     , subscript = :subscript
     , accglcode = :glacode
     , subglcode = :glacode
     , creind = 'Y'
     , creglcode = :creglcode
 WHERE sncode = 1
   AND tmcode = :new_tmcode;

UPDATE mpulktm1 SET accessfee = 0 WHERE tmcode = :new_tmcode AND sncode = 399;

SELECT tmcode, accessfee
     , subscript
     , accglcode
     , subglcode
     , creind
     , creglcode
FROM  mpulktm1
 WHERE sncode = 1
   AND tmcode in (:new_tmcode, :ref_tmcode);

-- ============================================================================
-- *** *** Now UPDATE usage fee MPULKTM2(USG) *** ***
-- ============================================================================
-- -- --
-- browse usage charge
-- typeind = 'A' means charge as airtime
-- tmm.sncode in (1, 61, 80, 81, 84, 119) ==> these are services that charge as airtime
-- 1 - Telephony; 61 - International call forward; 80 - Mobile Fax Service
-- 84 - Dual Numbering Service; 119 - GPRS
-- If some new services created later that would be charged as airtime, then need to be included
-- -- --
   select distinct tmm.tmcode, tm.des as tmdes
        , tmm.sncode, sn.des as sndes
        , ic01*decode(typeind, 'A', pu.ppuair, 'L', pu.ppuic, -1) as HK$
        , decode(tmm.zncode, 42,'ptc toll-free', 81,'#7007',287,'1083 call'
                           , 71,'Data  - GPRS', 265,'Color - GPRS', 266, 'MMS - GPRS'
                           , '') as zn
        , tmm.spcode
        , typeind, il01, ic01, il10, ic10
     from mpulktm2 tmm, mputmtab tm, mpusntab sn, mpuputab pu
    where tmm.tmcode = tm.tmcode
      and tmm.vscode = tm.vscode
      and tmm.sncode(+) = sn.sncode
      and tmm.sncode in (1, 61, 80, 81, 84, 119)
      and typeind in ('A')
      and pu.vsdate = (select max(vsdate) from mpuputab)
      and tmm.tmcode in (:ref_tmcode, :new_tmcode)
    order by tmm.sncode, 6, tmm.tmcode, il01
   ;

-- -- --
-- set usage fee
-- update according to UR
-- clicks, A:100, L:10000 ==> If typeind = 'A' then $1 = 100
--                        ==> If typeind = 'L' then $1 = 10000
-- as told by Amalia, only need to set sncode (1, 80, 84)
-- UR: Free 600 inter, 600 intra, thereafter $0.8/min
-- -- --

VAR thereafter_chg NUMBER;
BEGIN
SELECT 0.25*100
INTO   :thereafter_chg
FROM DUAL;
END;
/
print thereafter_chg

SELECT tmcode, typeind, sncode, ic01, ic10
FROM   mpulktm2
WHERE sncode in (1, 80, 84)
   and ic01 <> 0
   and typeind = 'A'
   and tmcode in (:new_tmcode);

update mpulktm2
   SET ic01 = :thereafter_chg
     , ic10 = :thereafter_chg
 where sncode in (1, 80, 84)
   and ic01 <> 0
   and typeind = 'A'
   and tmcode in (:new_tmcode);

-- -- --
-- check airtime charge mpulktm2
-- -- --
select distinct tmcode, sncode
     , decode(zncode, 42,'ptc toll-free', 81,'#7007',287,'1083 call'
                    , 71,'Data  - GPRS', 265,'Color - GPRS', 266, 'MMS - GPRS'
                    , '') as zn
     , il01,ic01,il10,ic10
  from mpulktm2
 where sncode in (1, 80, 84)
   and typeind = 'A'
   and tmcode in (:ref_tmcode, :new_tmcode)
order by sncode, 3, tmcode,  ic01
;


