//表结构不变，发布流程的工具取最新的版本(2005-04-30)
//旧表名 -> 新表名
EBPM4J_DEFINITION     ->     TD_BPM_DEFINITION 
EBPM4J_EXT_ATTRIBUTE  ->     TD_BPM_EXT_ATTRIBUTE
EBPM4J_NODE           ->     TD_BPM_NODE
EBPM4J_ACTION         ->     TD_BPM_ACTION
EBPM4J_DELEGATION     ->     TD_BPM_DELEGATION      
EBPM4J_TRANSITION     ->     TD_BPM_TRANSITION  
EBPM4J_PROCESSLOG     ->     TL_BPM_LCU_UNDO_LOG  
    
//以下的与LCU无关与 BPM流程定义工具有关                                   
EBPM4J_TYPE             -> TD_BPM_TYPE                        
EBPM4J_VARIABLE         -> TD_BPM_VARIABLE                  
EBPM4J_PROCESSINSTANCE  -> TF_BPM_PROCESS_INSTANCE            
EBPM4J_SEQUENCEBLOCK    -> TF_BPM_SEQUENCE_BLOCK             
EBPM4J_TOKEN            -> TF_BPM_TOKEN                      
EBPM4J_VARIABLEINSTANCE -> TF_BPM_VARIABLE_INSTANCE                    
EBPM4J_EXELOG           -> TL_BPM_EXE_LOG              
EBPM4J_INVOKELOG        -> TL_BPM_INVOKE_LOG         
EBPM4J_FILE             -> TD_BPM_FILE  
EBPM4J_SWIMLANE         -> TD_BPM_SWIMLANE    
  
//--------------------   

   
drop public synonym       TD_BPM_DEFINITION        ;       
drop public synonym       TD_BPM_EXT_ATTRIBUTE     ;       
drop public synonym       TD_BPM_NODE              ;       
drop public synonym       TD_BPM_ACTION            ;       
drop public synonym       TD_BPM_DELEGATION        ;       
drop public synonym       TD_BPM_TRANSITION        ;       
drop public synonym       TL_BPM_LCU_UNDO_LOG      ;       
drop public synonym       TD_BPM_TYPE              ;       
drop public synonym       TD_BPM_VARIABLE          ;       
drop public synonym       TF_BPM_PROCESS_INSTANCE  ;       
drop public synonym       TF_BPM_SEQUENCE_BLOCK    ;       
drop public synonym       TF_BPM_TOKEN             ;       
drop public synonym       TF_BPM_VARIABLE_INSTANCE ;       
drop public synonym       TL_BPM_EXE_LOG           ;       
drop public synonym       TL_BPM_INVOKE_LOG        ;       
drop public synonym       TD_BPM_FILE              ;       
drop public synonym       TD_BPM_SWIMLANE          ;       


create public synonym    TD_BPM_DEFINITION             for   TD_BPM_DEFINITION          ;
create public synonym    TD_BPM_EXT_ATTRIBUTE          for   TD_BPM_EXT_ATTRIBUTE       ;
create public synonym    TD_BPM_NODE                   for   TD_BPM_NODE                ;
create public synonym    TD_BPM_ACTION                 for   TD_BPM_ACTION              ;
create public synonym    TD_BPM_DELEGATION             for   TD_BPM_DELEGATION          ;
create public synonym    TD_BPM_TRANSITION             for   TD_BPM_TRANSITION          ;
create public synonym    TL_BPM_LCU_UNDO_LOG           for   TL_BPM_LCU_UNDO_LOG        ;
create public synonym    TD_BPM_TYPE                   for   TD_BPM_TYPE                ;
create public synonym    TD_BPM_VARIABLE               for   TD_BPM_VARIABLE            ;
create public synonym    TF_BPM_PROCESS_INSTANCE       for   TF_BPM_PROCESS_INSTANCE    ;
create public synonym    TF_BPM_SEQUENCE_BLOCK         for   TF_BPM_SEQUENCE_BLOCK      ;
create public synonym    TF_BPM_TOKEN                  for   TF_BPM_TOKEN               ;
create public synonym    TF_BPM_VARIABLE_INSTANCE      for   TF_BPM_VARIABLE_INSTANCE   ;
create public synonym    TL_BPM_EXE_LOG                for   TL_BPM_EXE_LOG             ;
create public synonym    TL_BPM_INVOKE_LOG             for   TL_BPM_INVOKE_LOG          ;
create public synonym    TD_BPM_FILE                   for   TD_BPM_FILE                ;
create public synonym    TD_BPM_SWIMLANE               for   TD_BPM_SWIMLANE            ;
 
                                                                      
grant select,update,delete,insert on TD_BPM_DEFINITION         to crm1   ; 
grant select,update,delete,insert on TD_BPM_EXT_ATTRIBUTE      to crm1   ; 
grant select,update,delete,insert on TD_BPM_NODE               to crm1   ; 
grant select,update,delete,insert on TD_BPM_ACTION             to crm1   ; 
grant select,update,delete,insert on TD_BPM_DELEGATION         to crm1   ; 
grant select,update,delete,insert on TD_BPM_TRANSITION         to crm1   ; 
grant select,update,delete,insert on TL_BPM_LCU_UNDO_LOG       to crm1   ; 
grant select,update,delete,insert on TD_BPM_TYPE               to crm1   ; 
grant select,update,delete,insert on TD_BPM_VARIABLE           to crm1   ; 
grant select,update,delete,insert on TF_BPM_PROCESS_INSTANCE   to crm1   ; 
grant select,update,delete,insert on TF_BPM_SEQUENCE_BLOCK     to crm1   ; 
grant select,update,delete,insert on TF_BPM_TOKEN              to crm1   ; 
grant select,update,delete,insert on TF_BPM_VARIABLE_INSTANCE  to crm1   ; 
grant select,update,delete,insert on TL_BPM_EXE_LOG            to crm1   ; 
grant select,update,delete,insert on TL_BPM_INVOKE_LOG         to crm1   ; 
grant select,update,delete,insert on TD_BPM_FILE               to crm1   ; 
grant select,update,delete,insert on TD_BPM_SWIMLANE           to crm1   ; 
             
   
//--------------------------   
//以前的表结构     
 
drop public synonym  ebpm4j_processlog
create public synonym ebpm4j_processlog       for ebpm4j_processlog
grant select,update,delete,insert on ebpm4j_processlog           to scott
 
drop public synonym  ebpm4j_action
drop public synonym  ebpm4j_definition
drop public synonym  ebpm4j_delegation
drop public synonym  ebpm4j_exelog
drop public synonym  ebpm4j_ext_attribute
drop public synonym  ebpm4j_file
drop public synonym  ebpm4j_invokelog
drop public synonym  ebpm4j_job
drop public synonym  ebpm4j_node
drop public synonym  ebpm4j_processinstance
drop public synonym  ebpm4j_sequenceblock
drop public synonym  ebpm4j_swimlane
drop public synonym  ebpm4j_token
drop public synonym  ebpm4j_transition
drop public synonym  ebpm4j_type
drop public synonym  ebpm4j_variable
drop public synonym  ebpm4j_variableinstance
 
           
create public synonym ebpm4j_action           for ebpm4j_action             
create public synonym ebpm4j_definition       for ebpm4j_definition         
create public synonym ebpm4j_delegation       for ebpm4j_delegation         
create public synonym ebpm4j_exelog           for ebpm4j_exelog             
create public synonym ebpm4j_ext_attribute    for ebpm4j_ext_attribute      
create public synonym ebpm4j_file             for ebpm4j_file               
create public synonym ebpm4j_invokelog        for ebpm4j_invokelog          
create public synonym ebpm4j_job              for ebpm4j_job                
create public synonym ebpm4j_node             for ebpm4j_node               
create public synonym ebpm4j_processinstance  for ebpm4j_processinstance    
create public synonym ebpm4j_sequenceblock    for ebpm4j_sequenceblock      
create public synonym ebpm4j_swimlane         for ebpm4j_swimlane           
create public synonym ebpm4j_token            for ebpm4j_token              
create public synonym ebpm4j_transition       for ebpm4j_transition         
create public synonym ebpm4j_type             for ebpm4j_type               
create public synonym ebpm4j_variable         for ebpm4j_variable           
create public synonym ebpm4j_variableinstance for ebpm4j_variableinstance   
                                                               
grant select,update,delete,insert on ebpm4j_action           to scott                                                               
grant select,update,delete,insert on ebpm4j_definition       to scott                                                               
grant select,update,delete,insert on ebpm4j_delegation       to scott                                                               
grant select,update,delete,insert on ebpm4j_exelog           to scott                                                               
grant select,update,delete,insert on ebpm4j_ext_attribute    to scott 
grant select,update,delete,insert on ebpm4j_file             to scott 
grant select,update,delete,insert on ebpm4j_invokelog        to scott 
grant select,update,delete,insert on ebpm4j_job              to scott                                       
grant select,update,delete,insert on ebpm4j_node             to scott                                       
grant select,update,delete,insert on ebpm4j_processinstance  to scott 
grant select,update,delete,insert on ebpm4j_sequenceblock    to scott
grant select,update,delete,insert on ebpm4j_swimlane         to scott
grant select,update,delete,insert on ebpm4j_token            to scott
grant select,update,delete,insert on ebpm4j_transition       to scott
grant select,update,delete,insert on ebpm4j_type             to scott
grant select,update,delete,insert on ebpm4j_variable         to scott
grant select,update,delete,insert on ebpm4j_variableinstance to scott



drop  table  EBPM4J_PROCESSLOG  
-- Create table
create table EBPM4J_PROCESSLOG
(
  FLOWID         NUMBER(16) not null,
  SERVIDX        NUMBER(4) not null,
  PROCESSID      NUMBER(4) not null,
  DEFINITIONID   NUMBER(19) not null,
  DEFINITIONNAME VARCHAR2(255) not null,
  NODETRACK      VARCHAR2(4000),
  EXTAINFO       VARCHAR2(255),
  DATE_          VARCHAR2(20),
  ISUNDOABLE     NUMBER(1)
)
tablespace TBS_OSS_DEF
  pctfree 10
  initrans 1
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  );
-- Create/Recreate primary, unique and foreign key constraints 
alter table EBPM4J_PROCESSLOG
  add constraint EBPM4J_PROCESSLOG_PRIMARY primary key (FLOWID,SERVIDX,PROCESSID,DEFINITIONID)
  using index 
  tablespace TBS_OSS_DEF
  pctfree 10
  initrans 2
  maxtrans 255
  storage
  (
    initial 64K
    minextents 1
    maxextents unlimited
  );

drop public synonym  ebpm4j_processlog
create public synonym ebpm4j_processlog  for ebpm4j_processlog
grant select,update,delete,insert on ebpm4j_processlog  to scott
