/* REM  Insert new rate plan to production

   Date     By       Modification
   20050309 Amalia   - combine Step06-release_rateplan.sql and Step07-insert_misc.sql
                     - change free_sms to fuom_all

   20050613 Amalia   - modify with respect to fuom_all.proind
*/
SET LINES 500
SET SERVEROUTPUT ON SIZE 100000

DECLARE

   v_date           DATE   := TO_DATE('04-1ды-2016','DD-MON-YYYY');
   n_main_sp        number := NULL;

   n_ref_tm_dlf     number := 721;
   n_ref_tmcode     number := NULL;
   n_ref_tm_des     varchar2(40) := NULL;

   -- -- added by Theresa
   v_new_tmcode    NUMBER := 721;
   v_ref_tmcode    NUMBER := 638;


   CURSOR c_all_tmcode IS
      SELECT tm.tmcode, tm.vscode + 1 as vscode
        FROM mputmtab tm
       WHERE vscode = 0
         -- -- added by Theresa
         -- -- because at this moment, tmcode 300 and 301 are not released yet
         -- -- this cursor will select both 300 and 301 to process
         -- -- AND tm.tmcode = 300
         AND tm.tmcode = v_new_tmcode
         -- -- end added by Theresa
         AND not exists (select 1 from mputmtab where vscode != tm.vscode and tmcode = tm.tmcode)
      ;


   cnt_tmcode                   number := 0;
   inserted_mpulktmb_records    number := 0;
   inserted_mpulktmm_records    number := 0;
   inserted_mpulktmt_records    number := 0;
   inserted_mpulktmz_records    number := 0;
   inserted_mpufutab_records    number := 0;
   inserted_mpuintab_records    number := 0;
   inserted_free_mins_records   number := 0;
   inserted_mpulktmc_records    number := 0;
   inserted_fuom_all_records    number := 0;
   inserted_mpucltab_records    number := 0;

   t_cnt_tmcode                 number := 0;
   t_inserted_mpulktmb_records  number := 0;
   t_inserted_mpulktmm_records  number := 0;
   t_inserted_mpulktmt_records  number := 0;
   t_inserted_mpulktmz_records  number := 0;
   t_inserted_mpufutab_records  number := 0;
   t_inserted_mpuintab_records  number := 0;
   t_inserted_free_mins_records number := 0;
   t_inserted_mpulktmc_records  number := 0;
   t_inserted_fuom_all_records  number := 0;
   t_inserted_mpucltab_records  number := 0;

BEGIN


   FOR c_tm IN c_all_tmcode LOOP

      select spcode
        into n_main_sp
        from mpulktm1
       where tmcode = c_tm.tmcode
         and sncode = 1;


      /* default copy from tmcode 64
       where tmcode = decode(c_tm.tmcode
       ---------------------------------------
                add this -> ,<new_tm>,<ref_tm>
                      eg.   ,270, 258
       ---------------------------------------
                            ,c_tm.tmcode,n_ref_tm_dlf
                            ,n_ref_tm_dlf);
      */
      select tmcode, des
        into n_ref_tmcode, n_ref_tm_des
        from mputmview
       where tmcode = decode(c_tm.tmcode
                            ,v_new_tmcode, v_ref_tmcode
                            ,c_tm.tmcode,n_ref_tm_dlf
                            ,n_ref_tm_dlf);


      DBMS_OUTPUT.PUT_LINE('TMCODE = '||TO_CHAR(c_tm.tmcode)||
                           ', MAX VSCODE = '||TO_CHAR(c_tm.vscode)||
                           ', EFF DATE = '||TO_CHAR(v_date,'DD-MON-YYYY')||
                           ', MAIN SPCODE = '||TO_CHAR(n_main_sp)||chr(10)||
                           ', COPY FROM = ' ||TO_CHAR(n_ref_tmcode) || ':' || TO_CHAR(n_ref_tm_des)
                           );

      cnt_tmcode                 := 0;
      inserted_mpulktmb_records  := 0;
      inserted_mpulktmm_records  := 0;
      inserted_mpulktmt_records  := 0;
      inserted_mpulktmz_records  := 0;
      inserted_mpufutab_records  := 0;
      inserted_mpuintab_records  := 0;
      inserted_free_mins_records := 0;
      inserted_mpulktmc_records  := 0;
      inserted_fuom_all_records  := 0;
      inserted_mpucltab_records  := 0;


/* MPUTMTAB

TMCODE, VSCODE, VSDATE, STATUS, TMIND,
DES, SHDES, PLCODE, PLMNNAME, TMRC, REC_VERSION
*/
      INSERT INTO mputmtab
      SELECT tmcode, c_tm.vscode, v_date, 'P', tmind,
             des, shdes, plcode, 'all HPLMNs', tmrc, 0, default_qos, echannel,tier_type
        FROM mputmtab
       WHERE tmcode = c_tm.tmcode
         AND vscode = 0;

      cnt_tmcode := SQL%ROWCOUNT;

/* MPULKTMB

TMCODE, VSCODE, VSDATE, STATUS, SPCODE, SNCODE,
SUBSCRIPT, ACCESSFEE, EVENT,
ECHIND, AMTIND, FRQIND, SRVIND, PROIND, ADVIND, SUSIND,
TAXCODE, LTCODE, PLCODE, ACCDMCODE, SUBDMCODE, USGDMCODE, BILLFREQ,
FREEDAYS, ACCGLCODE, SUBGLCODE, USGGLCODE,
IL01, IR01, IM01, IC01,
..
IL10, IR10, IM10, IC10,
ACCJCID, USGJCID, SUBJCID, REC_VERSION, CREIND, CREGLCODE
*/

      INSERT INTO mpulktmb
      SELECT tmcode, c_tm.vscode, v_date, 'P', spcode, sncode,
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
        FROM mpulktm1
       WHERE tmcode = c_tm.tmcode;

      inserted_mpulktmb_records := SQL%ROWCOUNT;

/* MPULKTMM

TMCODE, VSCODE, VSDATE, STATUS, SPCODE, SNCODE,
TWCODE, TTCODE, GVCODE, ZNCODE, SVLCODE, SPLITIND,
UOMIND, UMCODE, USGIND, TYPEIND, RNDIND, RATEIND
MINUOM, MAXUOM, HRCODE
IL01, IR01, IM01, IC01,
...
IL10, IR10, IM10, IC10,
EV_REG, EV_ACT, EV_DEA, EV_INT, EV_INV, EV_ERA,
USGGLCODE, USGJCID, REC_VERSION
*/
      INSERT INTO mpulktmm
      SELECT tmcode, c_tm.vscode, v_date, 'P', spcode, sncode,
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
        FROM mpulktm2
       WHERE tmcode = c_tm.tmcode;

      inserted_mpulktmm_records := SQL%ROWCOUNT;


/* MPULKTMT

TMCODE, VSCODE, TTCODE, TICODE,
TDCODE, TIDES, LB, RB, TDDES, COLDATE,
MON, TUE, WED, THU, FRI, SAT, SUN, HOL, REC_VERSION
*/

      INSERT INTO mpulktmt
      SELECT c_tm.tmcode, decode(vscode,0,vscode,c_tm.vscode), ttcode, ticode,
             tdcode, tides, lb, rb, tddes, coldate,
             mon, tue, wed, thu, fri, sat, sun, hol, 0
        FROM mpulktmt tm1
       WHERE tmcode = n_ref_tmcode
         AND (vscode = 0 or vscode = (select max(vscode)
                                        from mpulktmt
                                       where tmcode = tm1.tmcode)
              )
      ;

      inserted_mpulktmt_records := SQL%ROWCOUNT;


/* MPULKTMZ

TMCODE, VSCODE, ZNCODE, ZOCODE, ZPCODE, ZODES,
CGI, ZPDES, DIGITS, REC_VERSION
*/

      INSERT INTO mpulktmz
      SELECT c_tm.tmcode, decode(vscode,0,vscode,c_tm.vscode), zncode, zocode, zpcode, zodes,
             cgi, zpdes, digits, 0
        FROM mpulktmz tm1
       WHERE tmcode = n_ref_tmcode
         AND (vscode = 0 or vscode = (select max(vscode)
                                        from mpulktmz
                                       where tmcode = tm1.tmcode)
              )
      ;

      inserted_mpulktmz_records := SQL%ROWCOUNT;


/* MPUFUTAB (inter free minutes)

TMCODE, VSCODE, UMCODE, VSSTART, VSEND,
SPCODE, SNCODE, TWCODE, TTCODE, GVCODE,
ZNCODE, PROIND, FUOM, REC_VERSION, CARRY_FORWARD
*/

      INSERT INTO mpufutab
      SELECT c_tm.tmcode, c_tm.vscode, umcode, v_date, NULL
           , spcode, sncode, twcode, ttcode, gvcode
           , zncode, proind, fuom, 0, carry_forward
        FROM mpufutab fu
       WHERE tmcode = n_ref_tmcode
         AND vscode = (SELECT MAX(vscode)
                         FROM mpufutab
                        WHERE tmcode = fu.tmcode)
      ;

      inserted_mpufutab_records := SQL%ROWCOUNT;

/* MPUINTAB (intra free minutes)

TMCODE, VSCODE, UMCODE, VSSTART, VSEND,
SPCODE, SNCODE, TTCODE, ZNCODE, FUOM,
CALL_TYPE, ON_TOP, BESIDES,
REC_VERSION, PRIND, CARRY_FORWARD
*/

      INSERT INTO mpuintab
      SELECT c_tm.tmcode, c_tm.vscode, umcode, v_date, NULL
           , spcode, sncode, ttcode, zncode, fuom
           , call_type, on_top, besides
           , 0, prind, carry_forward
        FROM mpuintab fu
       WHERE tmcode = n_ref_tmcode
         AND vscode = (SELECT MAX(vscode)
                         FROM mpuintab
                        WHERE tmcode = fu.tmcode)
      ;

      inserted_mpuintab_records := SQL%ROWCOUNT;

/* FREE_MINS (Mobile data free minutes)

SEQNO, TMCODE, SPCODE, SNCODE, FREE_MINS,
EFF_DATE, EXPIRY_DATE, SHARE_TELEPHONY,
ENTRY_DATE, LASTMOD_DATE, LASTMODBY, REC_VERSION
*/

      INSERT INTO free_mins
      SELECT n.seq + 1, b.tmcode, b.spcode, b.sncode, ref.free_mins
           , v_date, expiry_date, ref.share_telephony
           , sysdate, NULL, 'BATCH', 0
        FROM mpulktmb b
           , (select * from free_mins where tmcode = n_ref_tmcode) ref
           , (select nvl(max(seqno),0) as seq from free_mins) n
       WHERE b.sncode = ref.sncode
         AND b.tmcode = c_tm.tmcode
      ;

      inserted_free_mins_records := SQL%ROWCOUNT;


/* MPULKTMC (service <-> usage grouping, overlapping checking by CPH)

TMCODE, SPCODE, SNCODE, GRP_ID
*/

      INSERT INTO mpulktmc
      SELECT b.tmcode, b.spcode, b.sncode, ref.grp_id
        FROM mpulktmb b
           , (select sncode, grp_id from mpulktmc where tmcode = n_ref_tmcode) ref
       WHERE b.sncode = ref.sncode
         AND b.tmcode = c_tm.tmcode
      ;

      inserted_mpulktmc_records := SQL%ROWCOUNT;


/* FUOM_ALL (FUOM on services other than Telephony(mpufutab/mpuintab) or Mobile data(free_mins))

TMCODE, VSCODE, UMCODE, VSSTART, VSEND,
SNCODE, PKG_SNCODE, GRP_ID, CALL_TYPE,
INTER_FUOM, INTRA_FUOM, DEDUCT_MTHD, REC_VERSION, PROIND
*/

      INSERT INTO fuom_all
      SELECT c_tm.tmcode, c_tm.vscode, umcode, v_date, NULL
           , sncode, pkg_sncode, grp_id, call_type
           , inter_fuom, intra_fuom, deduct_mthd, 0, PROIND
        FROM fuom_all fa
       WHERE tmcode = n_ref_tmcode
         AND vscode = (SELECT MAX(vscode)
                         FROM fuom_all
                        WHERE tmcode = fa.tmcode)
      ;

      inserted_fuom_all_records := SQL%ROWCOUNT;


/* MPUCLTAB (credit limit)

TMCODE, VSCODE, VSSTART, VSEND, CCL, CDAYS, KVFLAG, CCLDAYLY, REC_VERSION
*/

      INSERT INTO mpucltab
      SELECT c_tm.tmcode, c_tm.vscode, v_date, NULL
           , ccl, cdays, kvflag, ccldayly, 0
        FROM mpucltab cl
       WHERE tmcode = n_ref_tmcode
         AND vscode = (SELECT MAX(vscode)
                         FROM mpucltab
                        WHERE tmcode = cl.tmcode)
      ;

      inserted_mpucltab_records := SQL%ROWCOUNT;



      DBMS_OUTPUT.PUT_LINE('Inserted mputmtab :'||TO_CHAR(cnt_tmcode)                );
      DBMS_OUTPUT.PUT_LINE('Inserted mpulktmb :'||TO_CHAR(inserted_mpulktmb_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted mpulktmm :'||TO_CHAR(inserted_mpulktmm_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted mpulktmt :'||TO_CHAR(inserted_mpulktmt_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted mpulktmz :'||TO_CHAR(inserted_mpulktmz_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted mpufutab :'||TO_CHAR(inserted_mpufutab_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted mpuintab :'||TO_CHAR(inserted_mpuintab_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted free_mins:'||TO_CHAR(inserted_free_mins_records));
      DBMS_OUTPUT.PUT_LINE('Inserted mpulktmc :'||TO_CHAR(inserted_mpulktmc_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted fuom_all :'||TO_CHAR(inserted_fuom_all_records) );
      DBMS_OUTPUT.PUT_LINE('Inserted mpucltab :'||TO_CHAR(inserted_mpucltab_records) );

      t_cnt_tmcode := t_cnt_tmcode + cnt_tmcode;
      t_inserted_mpulktmb_records := t_inserted_mpulktmb_records + inserted_mpulktmb_records;
      t_inserted_mpulktmm_records := t_inserted_mpulktmm_records + inserted_mpulktmm_records;
      t_inserted_mpulktmt_records := t_inserted_mpulktmt_records + inserted_mpulktmt_records;
      t_inserted_mpulktmz_records := t_inserted_mpulktmz_records + inserted_mpulktmz_records;
      t_inserted_mpufutab_records := t_inserted_mpufutab_records + inserted_mpufutab_records;
      t_inserted_mpuintab_records := t_inserted_mpuintab_records + inserted_mpuintab_records;
      t_inserted_free_mins_records := t_inserted_free_mins_records + inserted_free_mins_records;
      t_inserted_mpulktmc_records := t_inserted_mpulktmc_records + inserted_mpulktmc_records;
      t_inserted_fuom_all_records := t_inserted_fuom_all_records + inserted_fuom_all_records;
      t_inserted_mpucltab_records := t_inserted_mpucltab_records + inserted_mpucltab_records;

   END LOOP; /* each unreleased plan */

   DBMS_OUTPUT.PUT_LINE(chr(10));

   DBMS_OUTPUT.PUT_LINE('Total selected Rate plan:'||TO_CHAR(t_cnt_tmcode));
   DBMS_OUTPUT.PUT_LINE('Total inserted mpulktmb :'||TO_CHAR(t_inserted_mpulktmb_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted mpulktmm :'||TO_CHAR(t_inserted_mpulktmm_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted mpulktmt :'||TO_CHAR(t_inserted_mpulktmt_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted mpulktmz :'||TO_CHAR(t_inserted_mpulktmz_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted mpufutab :'||TO_CHAR(t_inserted_mpufutab_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted mpuintab :'||TO_CHAR(t_inserted_mpuintab_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted free_mins:'||TO_CHAR(t_inserted_free_mins_records));
   DBMS_OUTPUT.PUT_LINE('Total inserted mpulktmc :'||TO_CHAR(t_inserted_mpulktmc_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted fuom_all :'||TO_CHAR(t_inserted_fuom_all_records) );
   DBMS_OUTPUT.PUT_LINE('Total inserted mpucltab :'||TO_CHAR(t_inserted_mpucltab_records) );

END;
/
