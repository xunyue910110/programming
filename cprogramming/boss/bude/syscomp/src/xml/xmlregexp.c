/*
 * regexp.c: generic and extensible Regular Expression engine
 *
 * Basically designed with the purpose of compiling regexps for 
 * the variety of validation/shemas mechanisms now available in
 * XML related specifications these include:
 *    - XML-1.0 DTD validation
 *    - XML Schemas structure part 1
 *    - XML Schemas Datatypes part 2 especially Appendix F
 *    - RELAX-NG/TREX i.e. the counter proposal
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard <veillard@redhat.com>
 */

#define IN_LIBXML
#include "libxml.h"

#ifdef LIBXML_REGEXP_ENABLED

#include <stdio.h>
#include <string.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <libxml/tree.h>
#include <libxml/parserInternals.h>
#include <libxml/xmlregexp.h>
#include <libxml/xmlautomata.h>
#include <libxml/xmlunicode.h>

#ifndef INT_MAX
#define INT_MAX 123456789 /* easy to flag and big enough for our needs */
#endif

/* #define DEBUG_REGEXP_GRAPH */
/* #define DEBUG_REGEXP_EXEC */ 
/* #define DEBUG_PUSH */
/* #define DEBUG_COMPACTION */

#define ERROR(str)							\
    ctxt->error = XML_REGEXP_COMPILE_ERROR;				\
    xmlRegexpErrCompile(ctxt, str);
#define NEXT ctxt->cur++
#define CUR (*(ctxt->cur))
#define NXT(index) (ctxt->cur[index])

#define CUR_SCHAR(s, l) xmlStringCurrentChar(NULL, s, &l)
#define NEXTL(l) ctxt->cur += l;
#define XML_REG_STRING_SEPARATOR '|'

/**
 * TODO:
 *
 * macro to flag unimplemented blocks
 */
#define TODO 								\
    xmlGenericError(xmlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

/************************************************************************
 * 									*
 * 			Datatypes and structures			*
 * 									*
 ************************************************************************/

typedef enum {
    XML_REGEXP_EPSILON = 1,
    XML_REGEXP_CHARVAL,
    XML_REGEXP_RANGES,
    XML_REGEXP_SUBREG,
    XML_REGEXP_STRING,
    XML_REGEXP_ANYCHAR, /* . */
    XML_REGEXP_ANYSPACE, /* \s */
    XML_REGEXP_NOTSPACE, /* \S */
    XML_REGEXP_INITNAME, /* \l */
    XML_REGEXP_NOTINITNAME, /* \l */
    XML_REGEXP_NAMECHAR, /* \c */
    XML_REGEXP_NOTNAMECHAR, /* \C */
    XML_REGEXP_DECIMAL, /* \d */
    XML_REGEXP_NOTDECIMAL, /* \d */
    XML_REGEXP_REALCHAR, /* \w */
    XML_REGEXP_NOTREALCHAR, /* \w */
    XML_REGEXP_LETTER,
    XML_REGEXP_LETTER_UPPERCASE,
    XML_REGEXP_LETTER_LOWERCASE,
    XML_REGEXP_LETTER_TITLECASE,
    XML_REGEXP_LETTER_MODIFIER,
    XML_REGEXP_LETTER_OTHERS,
    XML_REGEXP_MARK,
    XML_REGEXP_MARK_NONSPACING,
    XML_REGEXP_MARK_SPACECOMBINING,
    XML_REGEXP_MARK_ENCLOSING,
    XML_REGEXP_NUMBER,
    XML_REGEXP_NUMBER_DECIMAL,
    XML_REGEXP_NUMBER_LETTER,
    XML_REGEXP_NUMBER_OTHERS,
    XML_REGEXP_PUNCT,
    XML_REGEXP_PUNCT_CONNECTOR,
    XML_REGEXP_PUNCT_DASH,
    XML_REGEXP_PUNCT_OPEN,
    XML_REGEXP_PUNCT_CLOSE,
    XML_REGEXP_PUNCT_INITQUOTE,
    XML_REGEXP_PUNCT_FINQUOTE,
    XML_REGEXP_PUNCT_OTHERS,
    XML_REGEXP_SEPAR,
    XML_REGEXP_SEPAR_SPACE,
    XML_REGEXP_SEPAR_LINE,
    XML_REGEXP_SEPAR_PARA,
    XML_REGEXP_SYMBOL,
    XML_REGEXP_SYMBOL_MATH,
    XML_REGEXP_SYMBOL_CURRENCY,
    XML_REGEXP_SYMBOL_MODIFIER,
    XML_REGEXP_SYMBOL_OTHERS,
    XML_REGEXP_OTHER,
    XML_REGEXP_OTHER_CONTROL,
    XML_REGEXP_OTHER_FORMAT,
    XML_REGEXP_OTHER_PRIVATE,
    XML_REGEXP_OTHER_NA,
    XML_REGEXP_BLOCK_NAME
} xmlRegAtomType;

typedef enum {
    XML_REGEXP_QUANT_EPSILON = 1,
    XML_REGEXP_QUANT_ONCE,
    XML_REGEXP_QUANT_OPT,
    XML_REGEXP_QUANT_MULT,
    XML_REGEXP_QUANT_PLUS,
    XML_REGEXP_QUANT_ONCEONLY,
    XML_REGEXP_QUANT_ALL,
    XML_REGEXP_QUANT_RANGE
} xmlRegQuantType;

typedef enum {
    XML_REGEXP_START_STATE = 1,
    XML_REGEXP_FINAL_STATE,
    XML_REGEXP_TRANS_STATE
} xmlRegStateType;

typedef enum {
    XML_REGEXP_MARK_NORMAL = 0,
    XML_REGEXP_MARK_START,
    XML_REGEXP_MARK_VISITED
} xmlRegMarkedType;

typedef struct _xmlRegRange xmlRegRange;
typedef xmlRegRange *xmlRegRangePtr;

struct _xmlRegRange {
    int neg;		/* 0 normal, 1 not, 2 exclude */
    xmlRegAtomType type;
    int start;
    int end;
    xmlChar *blockName;
};

typedef struct _xmlRegAtom xmlRegAtom;
typedef xmlRegAtom *xmlRegAtomPtr;

typedef struct _xmlAutomataState xmlRegState;
typedef xmlRegState *xmlRegStatePtr;

struct _xmlRegAtom {
    int no;
    xmlRegAtomType type;
    xmlRegQuantType quant;
    int min;
    int max;

    void *valuep;
    void *valuep2;
    int neg;
    int codepoint;
    xmlRegStatePtr start;
    xmlRegStatePtr stop;
    int maxRanges;
    int nbRanges;
    xmlRegRangePtr *ranges;
    void *data;
};

typedef struct _xmlRegCounter xmlRegCounter;
typedef xmlRegCounter *xmlRegCounterPtr;

struct _xmlRegCounter {
    int min;
    int max;
};

typedef struct _xmlRegTrans xmlRegTrans;
typedef xmlRegTrans *xmlRegTransPtr;

struct _xmlRegTrans {
    xmlRegAtomPtr atom;
    int to;
    int counter;
    int count;
};

struct _xmlAutomataState {
    xmlRegStateType type;
    xmlRegMarkedType mark;
    xmlRegMarkedType reached;
    int no;

    int maxTrans;
    int nbTrans;
    xmlRegTrans *trans;
};

typedef struct _xmlAutomata xmlRegParserCtxt;
typedef xmlRegParserCtxt *xmlRegParserCtxtPtr;

struct _xmlAutomata {
    xmlChar *string;
    xmlChar *cur;

    int error;
    int neg;

    xmlRegStatePtr start;
    xmlRegStatePtr end;
    xmlRegStatePtr state;

    xmlRegAtomPtr atom;

    int maxAtoms;
    int nbAtoms;
    xmlRegAtomPtr *atoms;

    int maxStates;
    int nbStates;
    xmlRegStatePtr *states;

    int maxCounters;
    int nbCounters;
    xmlRegCounter *counters;

    int determinist;
};

struct _xmlRegexp {
    xmlChar *string;
    int nbStates;
    xmlRegStatePtr *states;
    int nbAtoms;
    xmlRegAtomPtr *atoms;
    int nbCounters;
    xmlRegCounter *counters;
    int determinist;
    /*
     * That's the compact form for determinists automatas
     */
    int nbstates;
    int *compact;
    void **transdata;
    int nbstrings;
    xmlChar **stringMap;
};

typedef struct _xmlRegExecRollback xmlRegExecRollback;
typedef xmlRegExecRollback *xmlRegExecRollbackPtr;

struct _xmlRegExecRollback {
    xmlRegStatePtr state;/* the current state */
    int index;		/* the index in the input stack */
    int nextbranch;	/* the next transition to explore in that state */
    int *counts;	/* save the automata state if it has some */
};

typedef struct _xmlRegInputToken xmlRegInputToken;
typedef xmlRegInputToken *xmlRegInputTokenPtr;

struct _xmlRegInputToken {
    xmlChar *value;
    void *data;
};

struct _xmlRegExecCtxt {
    int status;		/* execution status != 0 indicate an error */
    int determinist;	/* did we find an indeterministic behaviour */
    xmlRegexpPtr comp;	/* the compiled regexp */
    xmlRegExecCallbacks callback;
    void *data;

    xmlRegStatePtr state;/* the current state */
    int transno;	/* the current transition on that state */
    int transcount;	/* the number of chars in char counted transitions */

    /*
     * A stack of rollback states
     */
    int maxRollbacks;
    int nbRollbacks;
    xmlRegExecRollback *rollbacks;

    /*
     * The state of the automata if any
     */
    int *counts;

    /*
     * The input stack
     */
    int inputStackMax;
    int inputStackNr;
    int index;
    int *charStack;
    const xmlChar *inputString; /* when operating on characters */
    xmlRegInputTokenPtr inputStack;/* when operating on strings */

};

#define REGEXP_ALL_COUNTER	0x123456
#define REGEXP_ALL_LAX_COUNTER	0x123457

static void xmlFAParseRegExp(xmlRegParserCtxtPtr ctxt, int top);
static void xmlRegFreeState(xmlRegStatePtr state);
static void xmlRegFreeAtom(xmlRegAtomPtr atom);

/************************************************************************
 *									*
 * 		Regexp memory error handler				*
 *									*
 ************************************************************************/
/**
 * xmlRegexpErrMemory:
 * @extra:  extra information
 *
 * Handle an out of memory condition
 */
static void
xmlRegexpErrMemory(xmlRegParserCtxtPtr ctxt, const char *extra)
{
    const char *regexp = NULL;
    if (ctxt != NULL) {
        regexp = (const char *) ctxt->string;
	ctxt->error = XML_ERR_NO_MEMORY;
    }
    __xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_REGEXP,
		    XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0, extra,
		    regexp, NULL, 0, 0,
		    "Memory allocation failed : %s\n", extra);
}

/**
 * xmlRegexpErrCompile:
 * @extra:  extra information
 *
 * Handle a compilation failure
 */
static void
xmlRegexpErrCompile(xmlRegParserCtxtPtr ctxt, const char *extra)
{
    const char *regexp = NULL;
    int idx = 0;

    if (ctxt != NULL) {
        regexp = (const char *) ctxt->string;
	idx = ctxt->cur - ctxt->string;
	ctxt->error = XML_REGEXP_COMPILE_ERROR;
    }
    __xmlRaiseError(NULL, NULL, NULL, NULL, NULL, XML_FROM_REGEXP,
		    XML_REGEXP_COMPILE_ERROR, XML_ERR_FATAL, NULL, 0, extra,
		    regexp, NULL, idx, 0,
		    "failed to compile: %s\n", extra);
}

/************************************************************************
 * 									*
 * 			Allocation/Deallocation				*
 * 									*
 ************************************************************************/

static int xmlFAComputesDeterminism(xmlRegParserCtxtPtr ctxt);
/**
 * xmlRegEpxFromParse:
 * @ctxt:  the parser context used to build it
 *
 * Allocate a new regexp and fill it with the result from the parser
 *
 * Returns the new regexp or NULL in case of error
 */
static xmlRegexpPtr
xmlRegEpxFromParse(xmlRegParserCtxtPtr ctxt) {
    xmlRegexpPtr ret;

    ret = (xmlRegexpPtr) xmlMalloc(sizeof(xmlRegexp));
    if (ret == NULL) {
	xmlRegexpErrMemory(ctxt, "compiling regexp");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlRegexp));
    ret->string = ctxt->string;
    ret->nbStates = ctxt->nbStates;
    ret->states = ctxt->states;
    ret->nbAtoms = ctxt->nbAtoms;
    ret->atoms = ctxt->atoms;
    ret->nbCounters = ctxt->nbCounters;
    ret->counters = ctxt->counters;
    ret->determinist = ctxt->determinist;

    if ((ret->determinist != 0) &&
	(ret->nbCounters == 0) &&
	(ret->atoms != NULL) &&
	(ret->atoms[0] != NULL) &&
	(ret->atoms[0]->type == XML_REGEXP_STRING)) {
	int i, j, nbstates = 0, nbatoms = 0;
	int *stateRemap;
	int *stringRemap;
	int *transitions;
	void **transdata;
	xmlChar **stringMap;
        xmlChar *value;

	/*
	 * Switch to a compact representation
	 * 1/ counting the effective number of states left
	 * 2/ counting the unique number of atoms, and check that
	 *    they are all of the string type
	 * 3/ build a table state x atom for the transitions
	 */

	stateRemap = (int *)xmlMalloc(ret->nbStates * sizeof(int));
	if (stateRemap == NULL) {
	    xmlRegexpErrMemory(ctxt, "compiling regexp");
	    xmlFree(ret);
	    return(NULL);
	}
	for (i = 0;i < ret->nbStates;i++) {
	    if (ret->states[i] != NULL) {
		stateRemap[i] = nbstates;
		nbstates++;
	    } else {
		stateRemap[i] = -1;
	    }
	}
#ifdef DEBUG_COMPACTION
	printf("Final: %d states\n", nbstates);
#endif
	stringMap = (unsigned char **)xmlMalloc(ret->nbAtoms * sizeof(char *));
	if (stringMap == NULL) {
	    xmlRegexpErrMemory(ctxt, "compiling regexp");
	    xmlFree(stateRemap);
	    xmlFree(ret);
	    return(NULL);
	}
	stringRemap = (int *)xmlMalloc(ret->nbAtoms * sizeof(int));
	if (stringRemap == NULL) {
	    xmlRegexpErrMemory(ctxt, "compiling regexp");
	    xmlFree(stringMap);
	    xmlFree(stateRemap);
	    xmlFree(ret);
	    return(NULL);
	}
	for (i = 0;i < ret->nbAtoms;i++) {
	    if ((ret->atoms[i]->type == XML_REGEXP_STRING) &&
		(ret->atoms[i]->quant == XML_REGEXP_QUANT_ONCE)) {
		value = (unsigned char *)ret->atoms[i]->valuep;
                for (j = 0;j < nbatoms;j++) {
		    if (xmlStrEqual(stringMap[j], value)) {
			stringRemap[i] = j;
			break;
		    }
		}
		if (j >= nbatoms) {
		    stringRemap[i] = nbatoms;
		    stringMap[nbatoms] = xmlStrdup(value);
		    if (stringMap[nbatoms] == NULL) {
			for (i = 0;i < nbatoms;i++)
			    xmlFree(stringMap[i]);
			xmlFree(stringRemap);
			xmlFree(stringMap);
			xmlFree(stateRemap);
			xmlFree(ret);
			return(NULL);
		    }
		    nbatoms++;
		}
	    } else {
		xmlFree(stateRemap);
		xmlFree(stringRemap);
		for (i = 0;i < nbatoms;i++)
		    xmlFree(stringMap[i]);
		xmlFree(stringMap);
		xmlFree(ret);
		return(NULL);
	    }
	}
#ifdef DEBUG_COMPACTION
	printf("Final: %d atoms\n", nbatoms);
#endif
	transitions = (int *) xmlMalloc((nbstates + 1) *
	                                (nbatoms + 1) * sizeof(int));
	if (transitions == NULL) {
	    xmlFree(stateRemap);
	    xmlFree(stringRemap);
	    xmlFree(stringMap);
	    xmlFree(ret);
	    return(NULL);
	}
	memset(transitions, 0, (nbstates + 1) * (nbatoms + 1) * sizeof(int));

	/*
	 * Allocate the transition table. The first entry for each
	 * state corresponds to the state type.
	 */
	transdata = NULL;

	for (i = 0;i < ret->nbStates;i++) {
	    int stateno, atomno, targetno, prev;
	    xmlRegStatePtr state;
	    xmlRegTransPtr trans;

	    stateno = stateRemap[i];
	    if (stateno == -1)
		continue;
	    state = ret->states[i];

	    transitions[stateno * (nbatoms + 1)] = state->type;

	    for (j = 0;j < state->nbTrans;j++) {
		trans = &(state->trans[j]);
		if ((trans->to == -1) || (trans->atom == NULL))
		    continue;
                atomno = stringRemap[trans->atom->no];
		if ((trans->atom->data != NULL) && (transdata == NULL)) {
		    transdata = (void **) xmlMalloc(nbstates * nbatoms *
			                            sizeof(void *));
		    if (transdata != NULL)
			memset(transdata, 0,
			       nbstates * nbatoms * sizeof(void *));
		    else {
			xmlRegexpErrMemory(ctxt, "compiling regexp");
			break;
		    }
		}
		targetno = stateRemap[trans->to];
		/*
		 * if the same atom can generate transitions to 2 different
		 * states then it means the automata is not determinist and
		 * the compact form can't be used !
		 */
		prev = transitions[stateno * (nbatoms + 1) + atomno + 1];
		if (prev != 0) {
		    if (prev != targetno + 1) {
			ret->determinist = 0;
#ifdef DEBUG_COMPACTION
			printf("Indet: state %d trans %d, atom %d to %d : %d to %d\n",
			       i, j, trans->atom->no, trans->to, atomno, targetno);
			printf("       previous to is %d\n", prev);
#endif
			ret->determinist = 0;
			if (transdata != NULL)
			    xmlFree(transdata);
			xmlFree(transitions);
			xmlFree(stateRemap);
			xmlFree(stringRemap);
			for (i = 0;i < nbatoms;i++)
			    xmlFree(stringMap[i]);
			xmlFree(stringMap);
			goto not_determ;
		    }
		} else {
#if 0
		    printf("State %d trans %d: atom %d to %d : %d to %d\n",
			   i, j, trans->atom->no, trans->to, atomno, targetno);
#endif
		    transitions[stateno * (nbatoms + 1) + atomno + 1] =
			targetno + 1; /* to avoid 0 */
		    if (transdata != NULL)
			transdata[stateno * nbatoms + atomno] =
			    trans->atom->data;
		}
	    }
	}
	ret->determinist = 1;
#ifdef DEBUG_COMPACTION
	/*
	 * Debug
	 */
	for (i = 0;i < nbstates;i++) {
	    for (j = 0;j < nbatoms + 1;j++) {
                printf("%02d ", transitions[i * (nbatoms + 1) + j]);
	    }
	    printf("\n");
	}
	printf("\n");
#endif
	/*
	 * Cleanup of the old data
	 */
	if (ret->states != NULL) {
	    for (i = 0;i < ret->nbStates;i++)
		xmlRegFreeState(ret->states[i]);
	    xmlFree(ret->states);
	}
	ret->states = NULL;
	ret->nbStates = 0;
	if (ret->atoms != NULL) {
	    for (i = 0;i < ret->nbAtoms;i++)
		xmlRegFreeAtom(ret->atoms[i]);
	    xmlFree(ret->atoms);
	}
	ret->atoms = NULL;
	ret->nbAtoms = 0;

	ret->compact = transitions;
	ret->transdata = transdata;
	ret->stringMap = stringMap;
	ret->nbstrings = nbatoms;
	ret->nbstates = nbstates;
	xmlFree(stateRemap);
	xmlFree(stringRemap);
    }
not_determ:
    ctxt->string = NULL;
    ctxt->nbStates = 0;
    ctxt->states = NULL;
    ctxt->nbAtoms = 0;
    ctxt->atoms = NULL;
    ctxt->nbCounters = 0;
    ctxt->counters = NULL;
    return(ret);
}

/**
 * xmlRegNewParserCtxt:
 * @string:  the string to parse
 *
 * Allocate a new regexp parser context
 *
 * Returns the new context or NULL in case of error
 */
static xmlRegParserCtxtPtr
xmlRegNewParserCtxt(const xmlChar *string) {
    xmlRegParserCtxtPtr ret;

    ret = (xmlRegParserCtxtPtr) xmlMalloc(sizeof(xmlRegParserCtxt));
    if (ret == NULL)
	return(NULL);
    memset(ret, 0, sizeof(xmlRegParserCtxt));
    if (string != NULL)
	ret->string = xmlStrdup(string);
    ret->cur = ret->string;
    ret->neg = 0;
    ret->error = 0;
    ret->determinist = -1;
    return(ret);
}

/**
 * xmlRegNewRange:
 * @ctxt:  the regexp parser context
 * @neg:  is that negative
 * @type:  the type of range
 * @start:  the start codepoint
 * @end:  the end codepoint
 *
 * Allocate a new regexp range
 *
 * Returns the new range or NULL in case of error
 */
static xmlRegRangePtr
xmlRegNewRange(xmlRegParserCtxtPtr ctxt,
	       int neg, xmlRegAtomType type, int start, int end) {
    xmlRegRangePtr ret;

    ret = (xmlRegRangePtr) xmlMalloc(sizeof(xmlRegRange));
    if (ret == NULL) {
	xmlRegexpErrMemory(ctxt, "allocating range");
	return(NULL);
    }
    ret->neg = neg;
    ret->type = type;
    ret->start = start;
    ret->end = end;
    return(ret);
}

/**
 * xmlRegFreeRange:
 * @range:  the regexp range
 *
 * Free a regexp range
 */
static void
xmlRegFreeRange(xmlRegRangePtr range) {
    if (range == NULL)
	return;

    if (range->blockName != NULL)
	xmlFree(range->blockName);
    xmlFree(range);
}

/**
 * xmlRegNewAtom:
 * @ctxt:  the regexp parser context
 * @type:  the type of atom
 *
 * Allocate a new regexp range
 *
 * Returns the new atom or NULL in case of error
 */
static xmlRegAtomPtr
xmlRegNewAtom(xmlRegParserCtxtPtr ctxt, xmlRegAtomType type) {
    xmlRegAtomPtr ret;

    ret = (xmlRegAtomPtr) xmlMalloc(sizeof(xmlRegAtom));
    if (ret == NULL) {
	xmlRegexpErrMemory(ctxt, "allocating atom");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlRegAtom));
    ret->type = type;
    ret->quant = XML_REGEXP_QUANT_ONCE;
    ret->min = 0;
    ret->max = 0;
    return(ret);
}

/**
 * xmlRegFreeAtom:
 * @atom:  the regexp atom
 *
 * Free a regexp atom
 */
static void
xmlRegFreeAtom(xmlRegAtomPtr atom) {
    int i;

    if (atom == NULL)
	return;

    for (i = 0;i < atom->nbRanges;i++)
	xmlRegFreeRange(atom->ranges[i]);
    if (atom->ranges != NULL)
	xmlFree(atom->ranges);
    if (atom->type == XML_REGEXP_STRING)
	xmlFree(atom->valuep);
    xmlFree(atom);
}

static xmlRegStatePtr
xmlRegNewState(xmlRegParserCtxtPtr ctxt) {
    xmlRegStatePtr ret;

    ret = (xmlRegStatePtr) xmlMalloc(sizeof(xmlRegState));
    if (ret == NULL) {
	xmlRegexpErrMemory(ctxt, "allocating state");
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlRegState));
    ret->type = XML_REGEXP_TRANS_STATE;
    ret->mark = XML_REGEXP_MARK_NORMAL;
    return(ret);
}

/**
 * xmlRegFreeState:
 * @state:  the regexp state
 *
 * Free a regexp state
 */
static void
xmlRegFreeState(xmlRegStatePtr state) {
    if (state == NULL)
	return;

    if (state->trans != NULL)
	xmlFree(state->trans);
    xmlFree(state);
}

/**
 * xmlRegFreeParserCtxt:
 * @ctxt:  the regexp parser context
 *
 * Free a regexp parser context
 */
static void
xmlRegFreeParserCtxt(xmlRegParserCtxtPtr ctxt) {
    int i;
    if (ctxt == NULL)
	return;

    if (ctxt->string != NULL)
	xmlFree(ctxt->string);
    if (ctxt->states != NULL) {
	for (i = 0;i < ctxt->nbStates;i++)
	    xmlRegFreeState(ctxt->states[i]);
	xmlFree(ctxt->states);
    }
    if (ctxt->atoms != NULL) {
	for (i = 0;i < ctxt->nbAtoms;i++)
	    xmlRegFreeAtom(ctxt->atoms[i]);
	xmlFree(ctxt->atoms);
    }
    if (ctxt->counters != NULL)
	xmlFree(ctxt->counters);
    xmlFree(ctxt);
}

/************************************************************************
 * 									*
 * 			Display of Data structures			*
 * 									*
 ************************************************************************/

static void
xmlRegPrintAtomType(FILE *output, xmlRegAtomType type) {
    switch (type) {
        case XML_REGEXP_EPSILON:
	    fprintf(output, "epsilon "); break;
        case XML_REGEXP_CHARVAL:
	    fprintf(output, "charval "); break;
        case XML_REGEXP_RANGES:
	    fprintf(output, "ranges "); break;
        case XML_REGEXP_SUBREG:
	    fprintf(output, "subexpr "); break;
        case XML_REGEXP_STRING:
	    fprintf(output, "string "); break;
        case XML_REGEXP_ANYCHAR:
	    fprintf(output, "anychar "); break;
        case XML_REGEXP_ANYSPACE:
	    fprintf(output, "anyspace "); break;
        case XML_REGEXP_NOTSPACE:
	    fprintf(output, "notspace "); break;
        case XML_REGEXP_INITNAME:
	    fprintf(output, "initname "); break;
        case XML_REGEXP_NOTINITNAME:
	    fprintf(output, "notinitname "); break;
        case XML_REGEXP_NAMECHAR:
	    fprintf(output, "namechar "); break;
        case XML_REGEXP_NOTNAMECHAR:
	    fprintf(output, "notnamechar "); break;
        case XML_REGEXP_DECIMAL:
	    fprintf(output, "decimal "); break;
        case XML_REGEXP_NOTDECIMAL:
	    fprintf(output, "notdecimal "); break;
        case XML_REGEXP_REALCHAR:
	    fprintf(output, "realchar "); break;
        case XML_REGEXP_NOTREALCHAR:
	    fprintf(output, "notrealchar "); break;
        case XML_REGEXP_LETTER:
            fprintf(output, "LETTER "); break;
        case XML_REGEXP_LETTER_UPPERCASE:
            fprintf(output, "LETTER_UPPERCASE "); break;
        case XML_REGEXP_LETTER_LOWERCASE:
            fprintf(output, "LETTER_LOWERCASE "); break;
        case XML_REGEXP_LETTER_TITLECASE:
            fprintf(output, "LETTER_TITLECASE "); break;
        case XML_REGEXP_LETTER_MODIFIER:
            fprintf(output, "LETTER_MODIFIER "); break;
        case XML_REGEXP_LETTER_OTHERS:
            fprintf(output, "LETTER_OTHERS "); break;
        case XML_REGEXP_MARK:
            fprintf(output, "MARK "); break;
        case XML_REGEXP_MARK_NONSPACING:
            fprintf(output, "MARK_NONSPACING "); break;
        case XML_REGEXP_MARK_SPACECOMBINING:
            fprintf(output, "MARK_SPACECOMBINING "); break;
        case XML_REGEXP_MARK_ENCLOSING:
            fprintf(output, "MARK_ENCLOSING "); break;
        case XML_REGEXP_NUMBER:
            fprintf(output, "NUMBER "); break;
        case XML_REGEXP_NUMBER_DECIMAL:
            fprintf(output, "NUMBER_DECIMAL "); break;
        case XML_REGEXP_NUMBER_LETTER:
            fprintf(output, "NUMBER_LETTER "); break;
        case XML_REGEXP_NUMBER_OTHERS:
            fprintf(output, "NUMBER_OTHERS "); break;
        case XML_REGEXP_PUNCT:
            fprintf(output, "PUNCT "); break;
        case XML_REGEXP_PUNCT_CONNECTOR:
            fprintf(output, "PUNCT_CONNECTOR "); break;
        case XML_REGEXP_PUNCT_DASH:
            fprintf(output, "PUNCT_DASH "); break;
        case XML_REGEXP_PUNCT_OPEN:
            fprintf(output, "PUNCT_OPEN "); break;
        case XML_REGEXP_PUNCT_CLOSE:
            fprintf(output, "PUNCT_CLOSE "); break;
        case XML_REGEXP_PUNCT_INITQUOTE:
            fprintf(output, "PUNCT_INITQUOTE "); break;
        case XML_REGEXP_PUNCT_FINQUOTE:
            fprintf(output, "PUNCT_FINQUOTE "); break;
        case XML_REGEXP_PUNCT_OTHERS:
            fprintf(output, "PUNCT_OTHERS "); break;
        case XML_REGEXP_SEPAR:
            fprintf(output, "SEPAR "); break;
        case XML_REGEXP_SEPAR_SPACE:
            fprintf(output, "SEPAR_SPACE "); break;
        case XML_REGEXP_SEPAR_LINE:
            fprintf(output, "SEPAR_LINE "); break;
        case XML_REGEXP_SEPAR_PARA:
            fprintf(output, "SEPAR_PARA "); break;
        case XML_REGEXP_SYMBOL:
            fprintf(output, "SYMBOL "); break;
        case XML_REGEXP_SYMBOL_MATH:
            fprintf(output, "SYMBOL_MATH "); break;
        case XML_REGEXP_SYMBOL_CURRENCY:
            fprintf(output, "SYMBOL_CURRENCY "); break;
        case XML_REGEXP_SYMBOL_MODIFIER:
            fprintf(output, "SYMBOL_MODIFIER "); break;
        case XML_REGEXP_SYMBOL_OTHERS:
            fprintf(output, "SYMBOL_OTHERS "); break;
        case XML_REGEXP_OTHER:
            fprintf(output, "OTHER "); break;
        case XML_REGEXP_OTHER_CONTROL:
            fprintf(output, "OTHER_CONTROL "); break;
        case XML_REGEXP_OTHER_FORMAT:
            fprintf(output, "OTHER_FORMAT "); break;
        case XML_REGEXP_OTHER_PRIVATE:
            fprintf(output, "OTHER_PRIVATE "); break;
        case XML_REGEXP_OTHER_NA:
            fprintf(output, "OTHER_NA "); break;
        case XML_REGEXP_BLOCK_NAME:
	    fprintf(output, "BLOCK "); break;
    }
}

static void
xmlRegPrintQuantType(FILE *output, xmlRegQuantType type) {
    switch (type) {
        case XML_REGEXP_QUANT_EPSILON:
	    fprintf(output, "epsilon "); break;
        case XML_REGEXP_QUANT_ONCE:
	    fprintf(output, "once "); break;
        case XML_REGEXP_QUANT_OPT:
	    fprintf(output, "? "); break;
        case XML_REGEXP_QUANT_MULT:
	    fprintf(output, "* "); break;
        case XML_REGEXP_QUANT_PLUS:
	    fprintf(output, "+ "); break;
	case XML_REGEXP_QUANT_RANGE:
	    fprintf(output, "range "); break;
	case XML_REGEXP_QUANT_ONCEONLY:
	    fprintf(output, "onceonly "); break;
	case XML_REGEXP_QUANT_ALL:
	    fprintf(output, "all "); break;
    }
}
static void
xmlRegPrintRange(FILE *output, xmlRegRangePtr range) {
    fprintf(output, "  range: ");
    if (range->neg)
	fprintf(output, "negative ");
    xmlRegPrintAtomType(output, range->type);
    fprintf(output, "%c - %c\n", range->start, range->end);
}

static void
xmlRegPrintAtom(FILE *output, xmlRegAtomPtr atom) {
    fprintf(output, " atom: ");
    if (atom == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    xmlRegPrintAtomType(output, atom->type);
    xmlRegPrintQuantType(output, atom->quant);
    if (atom->quant == XML_REGEXP_QUANT_RANGE)
	fprintf(output, "%d-%d ", atom->min, atom->max);
    if (atom->type == XML_REGEXP_STRING)
	fprintf(output, "'%s' ", (char *) atom->valuep);
    if (atom->type == XML_REGEXP_CHARVAL)
	fprintf(output, "char %c\n", atom->codepoint);
    else if (atom->type == XML_REGEXP_RANGES) {
	int i;
	fprintf(output, "%d entries\n", atom->nbRanges);
	for (i = 0; i < atom->nbRanges;i++)
	    xmlRegPrintRange(output, atom->ranges[i]);
    } else if (atom->type == XML_REGEXP_SUBREG) {
	fprintf(output, "start %d end %d\n", atom->start->no, atom->stop->no);
    } else {
	fprintf(output, "\n");
    }
}

static void
xmlRegPrintTrans(FILE *output, xmlRegTransPtr trans) {
    fprintf(output, "  trans: ");
    if (trans == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    if (trans->to < 0) {
	fprintf(output, "removed\n");
	return;
    }
    if (trans->counter >= 0) {
	fprintf(output, "counted %d, ", trans->counter);
    }
    if (trans->count == REGEXP_ALL_COUNTER) {
	fprintf(output, "all transition, ");
    } else if (trans->count >= 0) {
	fprintf(output, "count based %d, ", trans->count);
    }
    if (trans->atom == NULL) {
	fprintf(output, "epsilon to %d\n", trans->to);
	return;
    }
    if (trans->atom->type == XML_REGEXP_CHARVAL)
	fprintf(output, "char %c ", trans->atom->codepoint);
    fprintf(output, "atom %d, to %d\n", trans->atom->no, trans->to);
}
    
static void
xmlRegPrintState(FILE *output, xmlRegStatePtr state) {
    int i;

    fprintf(output, " state: ");
    if (state == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    if (state->type == XML_REGEXP_START_STATE)
	fprintf(output, "START ");
    if (state->type == XML_REGEXP_FINAL_STATE)
	fprintf(output, "FINAL ");
    
    fprintf(output, "%d, %d transitions:\n", state->no, state->nbTrans);
    for (i = 0;i < state->nbTrans; i++) {
	xmlRegPrintTrans(output, &(state->trans[i]));
    }
}

#ifdef DEBUG_REGEXP_GRAPH
static void
xmlRegPrintCtxt(FILE *output, xmlRegParserCtxtPtr ctxt) {
    int i;

    fprintf(output, " ctxt: ");
    if (ctxt == NULL) {
	fprintf(output, "NULL\n");
	return;
    }
    fprintf(output, "'%s' ", ctxt->string);
    if (ctxt->error)
	fprintf(output, "error ");
    if (ctxt->neg)
	fprintf(output, "neg ");
    fprintf(output, "\n");
    fprintf(output, "%d atoms:\n", ctxt->nbAtoms);
    for (i = 0;i < ctxt->nbAtoms; i++) {
	fprintf(output, " %02d ", i);
	xmlRegPrintAtom(output, ctxt->atoms[i]);
    }
    if (ctxt->atom != NULL) {
	fprintf(output, "current atom:\n");
	xmlRegPrintAtom(output, ctxt->atom);
    }
    fprintf(output, "%d states:", ctxt->nbStates);
    if (ctxt->start != NULL)
	fprintf(output, " start: %d", ctxt->start->no);
    if (ctxt->end != NULL)
	fprintf(output, " end: %d", ctxt->end->no);
    fprintf(output, "\n");
    for (i = 0;i < ctxt->nbStates; i++) {
	xmlRegPrintState(output, ctxt->states[i]);
    }
    fprintf(output, "%d counters:\n", ctxt->nbCounters);
    for (i = 0;i < ctxt->nbCounters; i++) {
	fprintf(output, " %d: min %d max %d\n", i, ctxt->counters[i].min,
		                                ctxt->counters[i].max);
    }
}
#endif

/************************************************************************
 * 									*
 *		 Finite Automata structures manipulations		*
 * 									*
 ************************************************************************/

static void 
xmlRegAtomAddRange(xmlRegParserCtxtPtr ctxt, xmlRegAtomPtr atom,
	           int neg, xmlRegAtomType type, int start, int end,
		   xmlChar *blockName) {
    xmlRegRangePtr range;

    if (atom == NULL) {
	ERROR("add range: atom is NULL");
	return;
    }
    if (atom->type != XML_REGEXP_RANGES) {
	ERROR("add range: atom is not ranges");
	return;
    }
    if (atom->maxRanges == 0) {
	atom->maxRanges = 4;
	atom->ranges = (xmlRegRangePtr *) xmlMalloc(atom->maxRanges *
		                             sizeof(xmlRegRangePtr));
	if (atom->ranges == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding ranges");
	    atom->maxRanges = 0;
	    return;
	}
    } else if (atom->nbRanges >= atom->maxRanges) {
	xmlRegRangePtr *tmp;
	atom->maxRanges *= 2;
	tmp = (xmlRegRangePtr *) xmlRealloc(atom->ranges, atom->maxRanges *
		                             sizeof(xmlRegRangePtr));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "adding ranges");
	    atom->maxRanges /= 2;
	    return;
	}
	atom->ranges = tmp;
    }
    range = xmlRegNewRange(ctxt, neg, type, start, end);
    if (range == NULL)
	return;
    range->blockName = blockName;
    atom->ranges[atom->nbRanges++] = range;
    
}

static int
xmlRegGetCounter(xmlRegParserCtxtPtr ctxt) {
    if (ctxt->maxCounters == 0) {
	ctxt->maxCounters = 4;
	ctxt->counters = (xmlRegCounter *) xmlMalloc(ctxt->maxCounters *
		                             sizeof(xmlRegCounter));
	if (ctxt->counters == NULL) {
	    xmlRegexpErrMemory(ctxt, "allocating counter");
	    ctxt->maxCounters = 0;
	    return(-1);
	}
    } else if (ctxt->nbCounters >= ctxt->maxCounters) {
	xmlRegCounter *tmp;
	ctxt->maxCounters *= 2;
	tmp = (xmlRegCounter *) xmlRealloc(ctxt->counters, ctxt->maxCounters *
		                           sizeof(xmlRegCounter));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "allocating counter");
	    ctxt->maxCounters /= 2;
	    return(-1);
	}
	ctxt->counters = tmp;
    }
    ctxt->counters[ctxt->nbCounters].min = -1;
    ctxt->counters[ctxt->nbCounters].max = -1;
    return(ctxt->nbCounters++);
}

static int 
xmlRegAtomPush(xmlRegParserCtxtPtr ctxt, xmlRegAtomPtr atom) {
    if (atom == NULL) {
	ERROR("atom push: atom is NULL");
	return(-1);
    }
    if (ctxt->maxAtoms == 0) {
	ctxt->maxAtoms = 4;
	ctxt->atoms = (xmlRegAtomPtr *) xmlMalloc(ctxt->maxAtoms *
		                             sizeof(xmlRegAtomPtr));
	if (ctxt->atoms == NULL) {
	    xmlRegexpErrMemory(ctxt, "pushing atom");
	    ctxt->maxAtoms = 0;
	    return(-1);
	}
    } else if (ctxt->nbAtoms >= ctxt->maxAtoms) {
	xmlRegAtomPtr *tmp;
	ctxt->maxAtoms *= 2;
	tmp = (xmlRegAtomPtr *) xmlRealloc(ctxt->atoms, ctxt->maxAtoms *
		                             sizeof(xmlRegAtomPtr));
	if (tmp == NULL) {
	    xmlRegexpErrMemory(ctxt, "allocating counter");
	    ctxt->maxAtoms /= 2;
	    return(-1);
	}
	ctxt->atoms = tmp;
    }
    atom->no = ctxt->nbAtoms;
    ctxt->atoms[ctxt->nbAtoms++] = atom;
    return(0);
}

static void 
xmlRegStateAddTrans(xmlRegParserCtxtPtr ctxt, xmlRegStatePtr state,
	            xmlRegAtomPtr atom, xmlRegStatePtr target,
		    int counter, int count) {

    int nrtrans;

    if (state == NULL) {
	ERROR("add state: state is NULL");
	return;
    }
    if (target == NULL) {
	ERROR("add state: target is NULL");
	return;
    }
    /*
     * Other routines follow the philosophy 'When in doubt, add a transition'
     * so we check here whether such a transition is already present and, if
     * so, silently ignore this request.
     */

    for (nrtrans=0; nrtrans<state->nbTrans; nrtrans++) {
        if ((state->trans[nrtrans].atom == atom) &&
            (state->trans[nrtrans].to == target->no) &&
            (state->trans[nrtrans].counter == counter) &&
            (state->t