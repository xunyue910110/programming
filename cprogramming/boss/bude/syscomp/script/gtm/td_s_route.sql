create table td_s_route (
	workid				number(4) not null,
	startTaskid			number(4) not null,			
	endTaskid			number(4) not null,				
	
	status				number(2) not null,					
	pidTask				number(8),				
	exectime			Date,				
	
	timeoutFlag			CHAR(1),			
	timeoutIntval			NUMBER(4),			
	timeoutFaultIntval		NUMBER(4),		
	timeoutActionCode		NUMBER(2),		
	
	faultActionCode			NUMBER(2),		
	faultRedoCtrlNumber		NUMBER(4),	
	faultNowRedoCtrlNumber		NUMBER(4),	
	faultLastRedoDate		DATE,		
	faultRedoIntval			NUMBER(4)
);		
