--rollback;
INSERT INTO pos_rateplan_rule
SELECT  rule_id+1,721,exist_cust,Trunc(SYSDATE-1),Trunc(SYSDATE-1),sysdate,'P3730',id_prefix_rel_type,msg_prompt,require_contact,acc_type
FROM pos_rateplan_rule 
WHERE rule_id = (SELECT Max(rule_id) FROM pos_rateplan_rule);

select * from pos_rateplan_rule where tmcode in 721;

INSERT INTO MPULKNXG
SELECT sncode, 721, typeind, prgcode, plcode, lvcode, rec_version, tpcode, MODULE
FROM MPULKNXG
WHERE tmcode = 638;

INSERT INTO mpulknxg lk
SELECT sncode, lk.tmcode, typeind, prgcode, lk.plcode, lvcode, lk.rec_version, (SELECT tpcode FROM mpulktpb WHERE tmcode = tm.tmcode), MODULE
FROM mpulknxg lk, mputmview tm
WHERE lk.tpcode IN (SELECT tpcode FROM mpulktpb WHERE tmcode = 638)
AND tm.tmcode IN (721);

select * from mpulknxg where tmcode in 721;

