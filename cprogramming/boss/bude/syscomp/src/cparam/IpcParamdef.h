#ifndef __IPCPARAM_DEF_H_
#define __IPCPARAM_DEF_H_

///
#define TIME_LEN_MAX	20

#define KEY_LEN_MAX		100
#define CODE_LEN_MAX	62
#define NAME_LEN_MAX	100

/**
 * test parameter data struct
 */
struct TSegHead {
	long shmbase;
	long shmsize;
	long indbase;
	long parcnt;
	long recbase1;
	long recbase2;
	long recsize;
	long extbase;
	long extsize;
	long trace;			//跟踪日志开关
	long resvn1;
	long resvn2;
	char resvc1[60];
};


struct TParIndex {
	char skey[KEY_LEN_MAX+1];
	long sequence;
	long lockid;
	long recflag;				/*<! flag to record-segment use */
	long reccnt;				/*<! record cnt */
	long recoff;				/*<! records off */
	long extcnt;				/*<! ext record cnt */
	long extoff;				/*<! ext records off */
	long capacity;
	long rinterval;				/*<! refresh interval */
	long lrtime;				/*<! last refresh time */
	char sutime[TIME_LEN_MAX];	/*<! last update time */
	long resvn1;
	char resvc1[20];	
};

struct TParRecord {
	char value1[CODE_LEN_MAX+1];
	char value2[CODE_LEN_MAX+1];
	char value3[CODE_LEN_MAX+1];
	char sresult[NAME_LEN_MAX+1];
};

#endif
