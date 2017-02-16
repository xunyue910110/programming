create table td_s_alarm (
workid		NUMBER(4) NOT NULL,
alarmid		NUMBER(4) NOT NULL,
alarmmode	number(2) NOT NULL,
lastalarmtime	DATE,
intval		NUMBER(4),
permitbtime	VARCHAR2(8),
permitetime	VARCHAR2(8),
mphonecode	VARCHAR2(60),
email		VARCHAR2(60),
usetag		CHAR(1) NOT NULL
);
