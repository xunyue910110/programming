#include <occi.h>
#include <iostream>
using namespace std;
using namespace oracle::occi;

Environment *ptrEnv = 0;
Connection  *ptrCon = 0;

typedef unsigned int uint_t;
Date addTime(const Date& d, uint_t  h = 0, uint_t m = 0, uint_t s = 0);
Date getOraDate(time_t);
time_t getUTCTime(const Date&);

int main()
{
	ptrEnv = Environment::createEnvironment (Environment::OBJECT);
	ptrCon = ptrEnv->createConnection ("guest", "guest", "eboss");

	if (!ptrCon) {
		cout << "Create Connection fault." << endl;
		exit(1);
	}


	Date d ;
	d = Date::getSystemDate(ptrEnv);
	cout << "d=" << d.toText("yyyy-mm-dd hh24:mi:ss") << endl;

	cout << "lastdate=" << d.lastDay().toText("yyyy-mm-dd hh24:mi:ss") << endl;
	int year = 0;
	unsigned int month = 0;
	unsigned int day= 0;
	unsigned int hour = 0;
	unsigned int min = 0;
	unsigned int sec = 0;

	d.getDate(year, month, day, hour, min, sec);

	cout << "year=" << year <<endl;
	cout << "month=" << month << endl;
	cout << "day=" << day << endl;
	cout << "hour=" << hour << endl;
	cout << "min=" << min << endl;
	cout << "sec=" << sec << endl;

	Date d1(ptrEnv, 2004,11,8,8,30,30);
	Date d2(ptrEnv, 2005,12,9,11,30,40);

	cout << "d1=" << d1.toText("yyyy-mm-dd hh24:mi:ss") << endl;
	cout << "d2=" << d2.toText("yyyy-mm-dd hh24:mi:ss") << endl;

	cout << "addTime= " << addTime(d1).toText("yyyy-mm-dd hh24:mi:ss") << endl;
	cout << "addTime= " << addTime(d1, 20).toText("yyyy-mm-dd hh24:mi:ss") << endl;
	cout << "addTime= " << addTime(d1, 0, 20).toText("yyyy-mm-dd hh24:mi:ss") << endl;
	cout << "addTime= " << addTime(d1, 0, 0, 20).toText("yyyy-mm-dd hh24:mi:ss") << endl;
	cout << "addTime= " << addTime(d1, 20, 20, 20).toText("yyyy-mm-dd hh24:mi:ss") << endl;
	cout << "addTime= " << addTime(d2).toText("yyyy-mm-dd hh24:mi:ss") << endl;

	time_t t0;
	time(&t0);

	struct tm *ptrTm = 0;
	ptrTm = localtime(&t0);
	cout << "wday=" << ptrTm->tm_wday << endl;

	Statement *ptrStmt = ptrCon->createStatement();
	ptrStmt->setSQL("select name from tst");
	
	ResultSet *ptrRs = ptrStmt->executeQuery();
	while (ptrRs->next()) {
		string s;
		s = ptrRs->getString(1);
		cout << "s=" << s << endl;
	}

	ptrStmt->closeResultSet(ptrRs);

	ptrStmt->setSQL("update tst set name = :1");
	ptrStmt->setString(1, "中国人");
	ptrStmt->executeUpdate();
	ptrCon->commit();
	ptrCon->terminateStatement(ptrStmt);

	/*
	Number num;

	if (num.isNull()) { cout << "num is null" << endl; }

	num = 918.9925;
	cout << num << endl;
	cout << num.toText(ptrEnv, "9999999.999") << endl;

	double i;
	i = (double)num;

	fprintf(stderr, "i=%f", i);

	cout << "i=" << i << endl;
	*/
	Date da;
	cout << "da is:" << (da.isNull() ? "Null" : "Not Null") << endl;
	//cout << "da=[" << da.toText("yyyy-mm-dd hh24:mi:ss") << "]" << endl;

	Date da0 = Date::getSystemDate(ptrEnv);
	cout << "da0=[" << da0.toText("yyyy-mm-dd hh24:mi:ss") << "]" << endl;
	if (da == da0) {
		cout << "da == da0 is true" << endl;
	} else {
		cout << "da == da0 is false" << endl;
	}

	Number n(100);

	if (n == Number(100)) {
		cout << "that's all right" << endl;
	}

	ptrEnv->terminateConnection(ptrCon);
	Environment::terminateEnvironment(ptrEnv);
	exit(0);
}

time_t getUTCTime(const Date& d)
{
	typedef struct tm tm_t;
	tm_t tm;

	int year = 0;
        unsigned int month = 0;
        unsigned int day= 0;
        unsigned int hour = 0;
        unsigned int min = 0;
        unsigned int sec = 0;

	d.getDate(year, month, day, hour, min, sec);
	cout << "getUTCTime:" << endl;

	cout << "year=" << year << endl;
	cout << "month=" << month << endl;
	cout << "day=" << day << endl;
	cout << "hour=" << hour << endl;
	cout << "min=" << min << endl;
	cout << "sec=" << sec << endl;

	tm.tm_sec = sec;	/* [0, 61] */
	tm.tm_min = min;	/* [0, 59] */
	tm.tm_hour = hour;	/* [0, 23] */
	tm.tm_mday = day;	/* [1, 31] */
	tm.tm_mon =  month - 1; /* [0, 11] */
	tm.tm_year = year - 1900; /* years since 1900 */
	tm.tm_isdst = 0;
	
	return mktime(&tm);
}

Date getOraDate(time_t time)
{
	typedef struct tm tm_t;
	tm_t *ptrTm = 0;

	//Date d;
	ptrTm = gmtime(&time);

	cout << "In getOraDate ..." << endl;

	if (!ptrTm) cout << "gmtime fault" << endl;

	cout << "year=" << ptrTm->tm_year+1900 << endl;
	cout << "mon=" << ptrTm->tm_mon + 1 << endl;
	cout << "day=" << ptrTm->tm_mday << endl;
	cout << "hour=" << ptrTm->tm_hour << endl;
	cout << "min=" << ptrTm->tm_min << endl;
	cout << "sec=" << (ptrTm->tm_sec > 59 ? 59 : ptrTm->tm_sec) << endl;
	cout << "tm_isdst=" << ptrTm->tm_isdst << endl;
	
	int year =  ptrTm->tm_year+1900;
	unsigned int mon = ptrTm->tm_mon + 1;
	unsigned int day = ptrTm->tm_mday;
	unsigned int hour= ptrTm->tm_hour;
	unsigned int min = ptrTm->tm_min;
	unsigned int sec = ptrTm->tm_sec > 59 ? 59 : ptrTm->tm_sec;

	Date d(ptrEnv, year, mon, day, hour, min, sec);
	//d.setDate(year, mon, day, hour, min, sec);
	cout << "d=" << d.toText("yyyy-mm-dd hh24:mi:ss") << endl;
	return d;
}

Date addTime(const Date& d, uint_t  h, uint_t m, uint_t s)
{
	int year;
	uint_t month, day, hour, min, sec, mday;

	d.getDate(year, month, day, hour, min, sec);
	hour = hour + h; //total hour;
	min  = min  + m; //total min;
	sec  = sec  + s; //total sec;

	min  =  min + sec/60;
	sec  =  sec%60;

	hour =  hour + min/60;
	min  =  min%60;

	mday =  hour/24; /* added day */
	hour =  hour%24;


	Date d0(ptrEnv, year, month, day, hour, min, sec);
	return d0.addDays(mday);		
}
