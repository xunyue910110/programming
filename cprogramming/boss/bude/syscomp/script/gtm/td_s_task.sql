create table td_s_task 
(
taskid		number(6) not null,
taskname	varchar2(60),
plugtype	number(2) not null,
plugname	varchar2(120) not null,
channelid	number(4) not null,
startdate	DATE not null,
enddate		DATE not null,
usetag		CHAR(1) not null
);

alter table td_s_task add (
station varchar2(6), --REMOTE, LOCAL
isencrypt varchar2(3), --YES(1) NO(0)
username varchar2(20),
passcode varchar(120),
sid varchar2(60)
);

