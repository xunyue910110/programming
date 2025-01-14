#include <stdio.h>
#include <xa.h>
#include <atmi.h>

#if defined(__cplusplus)
extern "C" {
#endif
extern int _tmrunserver _((int));
extern void MWSVC _((TPSVCINFO *));
#if defined(__cplusplus)
}
#endif

static struct tmdsptchtbl_t _tmdsptchtbl[] = {
	{ (char*)"MWSVC", (char*)"MWSVC", (void (*) _((TPSVCINFO *))) MWSVC, 0, 0 },
	{ NULL, NULL, NULL, 0, 0 }
};

#ifndef _TMDLLIMPORT
#define _TMDLLIMPORT
#endif

#if defined(__cplusplus)
extern "C" {
#endif
_TMDLLIMPORT extern struct xa_switch_t tmnull_switch;
#if defined(__cplusplus)
}
#endif

typedef void (*tmp_void_cast)();
typedef void (*tmp_voidvoid_cast)(void);
typedef int (*tmp_intchar_cast)(int, char **); 
typedef int (*tmp_int_cast)(int);
static struct tmsvrargs_t tmsvrargs = {
	NULL,
	&_tmdsptchtbl[0],
	0,
	(tmp_intchar_cast)tpsvrinit,
	(tmp_voidvoid_cast)tpsvrdone,
	(tmp_int_cast)_tmrunserver,	/* PRIVATE  */
	NULL,			/* RESERVED */
	NULL,			/* RESERVED */
	NULL,			/* RESERVED */
	NULL,			/* RESERVED */
	(tmp_intchar_cast)tpsvrthrinit,
	(tmp_voidvoid_cast)tpsvrthrdone
};

struct tmsvrargs_t *
#ifdef _TMPROTOTYPES
_tmgetsvrargs(void)
#else
_tmgetsvrargs()
#endif
{
	tmsvrargs.reserved1 = NULL;
	tmsvrargs.reserved2 = NULL;
	tmsvrargs.xa_switch = &tmnull_switch;
	return(&tmsvrargs);
}

int
#ifdef _TMPROTOTYPES
main(int argc, char **argv)
#else
main(argc,argv)
int argc;
char **argv;
#endif
{
#ifdef TMMAINEXIT
#include "mainexit.h"
#endif

	return( _tmstartserver( argc, argv, _tmgetsvrargs()));
}

