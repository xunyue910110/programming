create table td_s_work 
(
	workid		NUMBER(6) not null,
	workname	VARCHAR2(60),
	subsyscode	VARCHAR2(6) not null,
	status		NUMBER(1)	not null,
	exectype	NUMBER(1)	not null,
	lastdate	DATE,
	thisdate	DATE,
	nextdate	DATE,
	intval		VARCHAR2(120),
	alarmtag	CHAR(1)	not null,
	prior		NUMBER(4),
	timeout		NUMBER(4) not null,
	invoketag	CHAR(1) not null,
	startdate	DATE not null,
	enddate		DATE not null,
	usetag		CHAR(1) not null
);