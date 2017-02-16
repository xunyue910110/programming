/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set sw=4 ts=8 et tw=80:
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/*
 * JS regular expressions, after Perl.
 */
#include "jsstddef.h"
#include <stdlib.h>
#include <string.h>
#include "jstypes.h"
#include "jsarena.h" /* Added by JSIFY */
#include "jsutil.h" /* Added by JSIFY */
#include "jsapi.h"
#include "jsarray.h"
#include "jsatom.h"
#include "jscntxt.h"
#include "jsconfig.h"
#include "jsfun.h"
#include "jsgc.h"
#include "jsinterp.h"
#include "jslock.h"
#include "jsnum.h"
#include "jsobj.h"
#include "jsopcode.h"
#include "jsregexp.h"
#include "jsscan.h"
#include "jsstr.h"

#if JS_HAS_REGEXPS

/* Note : contiguity of 'simple opcodes' is important for SimpleMatch() */
typedef enum REOp {
    REOP_EMPTY         = 0,  /* match rest of input against rest of r.e. */
    REOP_ALT           = 1,  /* alternative subexpressions in kid and next */
    REOP_SIMPLE_START  = 2,  /* start of 'simple opcodes' */
    REOP_BOL           = 2,  /* beginning of input (or line if multiline) */
    REOP_EOL           = 3,  /* end of input (or line if multiline) */
    REOP_WBDRY         = 4,  /* match "" at word boundary */
    REOP_WNONBDRY      = 5,  /* match "" at word non-boundary */
    REOP_DOT           = 6,  /* stands for any character */
    REOP_DIGIT         = 7,  /* match a digit char: [0-9] */
    REOP_NONDIGIT      = 8,  /* match a non-digit char: [^0-9] */
    REOP_ALNUM         = 9,  /* match an alphanumeric char: [0-9a-z_A-Z] */
    REOP_NONALNUM      = 10, /* match a non-alphanumeric char: [^0-9a-z_A-Z] */
    REOP_SPACE         = 11, /* match a whitespace char */
    REOP_NONSPACE      = 12, /* match a non-whitespace char */
    REOP_BACKREF       = 13, /* back-reference (e.g., \1) to a parenthetical */
    REOP_FLAT          = 14, /* match a flat string */
    REOP_FLAT1         = 15, /* match a single char */
    REOP_FLATi         = 16, /* case-independent REOP_FLAT */
    REOP_FLAT1i        = 17, /* case-independent REOP_FLAT1 */
    REOP_UCFLAT1       = 18, /* single Unicode char */
    REOP_UCFLAT1i      = 19, /* case-independent REOP_UCFLAT1 */
    REOP_UCFLAT        = 20, /* flat Unicode string; len immediate counts chars */
    REOP_UCFLATi       = 21, /* case-independent REOP_UCFLAT */
    REOP_CLASS         = 22, /* character class with index */
    REOP_NCLASS        = 23, /* negated character class with index */
    REOP_SIMPLE_END    = 23, /* end of 'simple opcodes' */
    REOP_QUANT         = 25, /* quantified atom: atom{1,2} */
    REOP_STAR          = 26, /* zero or more occurrences of kid */
    REOP_PLUS          = 27, /* one or more occurrences of kid */
    REOP_OPT           = 28, /* optional subexpression in kid */
    REOP_LPAREN        = 29, /* left paren bytecode: kid is u.num'th sub-regexp */
    REOP_RPAREN        = 30, /* right paren bytecode */
    REOP_JUMP          = 31, /* for deoptimized closure loops */
    REOP_DOTSTAR       = 32, /* optimize .* to use a single opcode */
    REOP_ANCHOR        = 33, /* like .* but skips left context to unanchored r.e. */
    REOP_EOLONLY       = 34, /* $ not preceded by any pattern */
    REOP_BACKREFi      = 37, /* case-independent REOP_BACKREF */
    REOP_LPARENNON     = 41, /* non-capturing version of REOP_LPAREN */
    REOP_ASSERT        = 43, /* zero width positive lookahead assertion */
    REOP_ASSERT_NOT    = 44, /* zero width negative lookahead assertion */
    REOP_ASSERTTEST    = 45, /* sentinel at end of assertion child */
    REOP_ASSERTNOTTEST = 46, /* sentinel at end of !assertion child */
    REOP_MINIMALSTAR   = 47, /* non-greedy version of * */
    REOP_MINIMALPLUS   = 48, /* non-greedy version of + */
    REOP_MINIMALOPT    = 49, /* non-greedy version of ? */
    REOP_MINIMALQUANT  = 50, /* non-greedy version of {} */
    REOP_ENDCHILD      = 51, /* sentinel at end of quantifier child */
    REOP_REPEAT        = 52, /* directs execution of greedy quantifier */
    REOP_MINIMALREPEAT = 53, /* directs execution of non-greedy quantifier */
    REOP_ALTPREREQ     = 54, /* prerequisite for ALT, either of two chars */
    REOP_ALTPREREQ2    = 55, /* prerequisite for ALT, a char or a class */
    REOP_ENDALT        = 56, /* end of final alternate */
    REOP_CONCAT        = 57, /* concatenation of terms (parse time only) */

    REOP_END
} REOp;

#define REOP_IS_SIMPLE(op)  ((unsigned)((op) - REOP_SIMPLE_START) <           \
                             (unsigned)REOP_SIMPLE_END)

struct RENode {
    REOp            op;         /* r.e. op bytecode */
    RENode          *next;      /* next in concatenation order */
    void            *kid;       /* first operand */
    union {
        void        *kid2;      /* second operand */
        jsint       num;        /* could be a number */
        size_t      parenIndex; /* or a parenthesis index */
        struct {                /* or a quantifier range */
            uintN  min;
            uintN  max;
            JSPackedBool greedy;
        } range;
        struct {                /* or a character class */
            size_t  startIndex;
            size_t  kidlen;     /* length of string at kid, in jschars */
            size_t  index;      /* index into class list */
            uint16  bmsize;     /* bitmap size, based on max char code */
            JSPackedBool sense;
        } ucclass;
        struct {                /* or a literal sequence */
            jschar  chr;        /* of one character */
            size_t  length;     /* or many (via the kid) */
        } flat;
        struct {
            RENode  *kid2;      /* second operand from ALT */
            jschar  ch1;        /* match char for ALTPREREQ */
            jschar  ch2;        /* ditto, or class index for ALTPREREQ2 */
        } altprereq;
    } u;
};

#define RE_IS_LETTER(c)     (((c >= 'A') && (c <= 'Z')) ||                    \
                             ((c >= 'a') && (c <= 'z')) )
#define RE_IS_LINE_TERM(c)  ((c == '\n') || (c == '\r') ||                    \
                             (c == LINE_SEPARATOR) || (c == PARA_SEPARATOR))

#define CLASS_CACHE_SIZE    4

typedef struct CompilerState {
    JSContext       *context;
    JSTokenStream   *tokenStream; /* For reporting errors */
    const jschar    *cpbegin;
    const jschar    *cpend;
    const jschar    *cp;
    size_t          parenCount;
    size_t          classCount;   /* number of [] encountered */
    size_t          treeDepth;    /* maximum depth of parse tree */
    size_t          progLength;   /* estimated bytecode length */
    RENode          *result;
    size_t          classBitmapsMem; /* memory to hold all class bitmaps */
    struct {
        const jschar *start;        /* small cache of class strings */
        size_t length;              /* since they're often the same */
        size_t index;
    } classCache[CLASS_CACHE_SIZE];
    uint16          flags;
} CompilerState;

typedef struct EmitStateStackEntry {
    jsbytecode      *altHead;       /* start of REOP_ALT* opcode */
    jsbytecode      *nextAltFixup;  /* fixup pointer to next-alt offset */
    jsbytecode      *nextTermFixup; /* fixup ptr. to REOP_JUMP offset */
    jsbytecode      *endTermFixup;  /* fixup ptr. to REOPT_ALTPREREQ* offset */
    RENode          *continueNode;  /* original REOP_ALT* node being stacked */
    jsbytecode      continueOp;     /* REOP_JUMP or REOP_ENDALT continuation */
    JSPackedBool    jumpToJumpFlag; /* true if we've patched jump-to-jump to
                                       avoid 16-bit unsigned offset overflow */
} EmitStateStackEntry;

/*
 * Immediate operand sizes and getter/setters.  Unlike the ones in jsopcode.h,
 * the getters and setters take the pc of the offset, not of the opcode before
 * the offset.
 */
#define ARG_LEN             2
#define GET_ARG(pc)         ((uint16)(((pc)[0] << 8) | (pc)[1]))
#define SET_ARG(pc, arg)    ((pc)[0] = (jsbytecode) ((arg) >> 8),       \
                             (pc)[1] = (jsbytecode) (arg))

#define OFFSET_LEN          ARG_LEN
#define OFFSET_MAX          (JS_BIT(ARG_LEN * 8) - 1)
#define GET_OFFSET(pc)      GET_ARG(pc)

/*
 * Maximum supported tree depth is maximum size of EmitStateStackEntry stack.
 * For sanity, we limit it to 2^24 bytes.
 */
#define TREE_DEPTH_MAX  (JS_BIT(24) / sizeof(EmitStateStackEntry))

/*
 * The maximum memory that can be allocated for class bitmaps.
 * For sanity, we limit it to 2^24 bytes.
 */
#define CLASS_BITMAPS_MEM_LIMIT JS_BIT(24)

/*
 * Functions to get size and write/read bytecode that represent small indexes
 * compactly.
 * Each byte in the code represent 7-bit chunk of the index. 8th bit when set
 * indicates that the following byte brings more bits to the index. Otherwise
 * this is the last byte in the index bytecode representing highest index bits.
 */
static size_t
GetCompactIndexWidth(size_t index)
{
    size_t width;

    for (width = 1; (index >>= 7) != 0; ++width) { }
    return width;
}

static jsbytecode *
WriteCompactIndex(jsbytecode *pc, size_t index)
{
    size_t next;

    while ((next = index >> 7) != 0) {
        *pc++ = (jsbytecode)(index | 0x80);
        index = next;
    }
    *pc++ = (jsbytecode)index;
    return pc;
}

static jsbytecode *
ReadCompactIndex(jsbytecode *pc, size_t *result)
{
    size_t nextByte;

    nextByte = *pc++;
    if ((nextByte & 0x80) == 0) {
        /*
         * Short-circuit the most common case when compact index <= 127.
         */
        *result = nextByte;
    } else {
        size_t shift = 7;
        *result = 0x7F & nextByte;
        do {
            nextByte = *pc++;
            *result |= (nextByte & 0x7F) << shift;
            shift += 7;
        } while ((nextByte & 0x80) != 0);
    }
    return pc;
}

typedef struct RECapture {
    ptrdiff_t index;           /* start of contents, -1 for empty  */
    size_t length;             /* length of capture */
} RECapture;

typedef struct REMatchState {
    const jschar *cp;
    RECapture parens[1];      /* first of 're->parenCount' captures,
                                 allocated at end of this struct */
} REMatchState;

struct REBackTrackData;

typedef struct REProgState {
    jsbytecode *continue_pc;        /* current continuation data */
    jsbytecode continue_op;
    ptrdiff_t index;                /* progress in text */
    size_t parenSoFar;              /* highest indexed paren started */
    union {
        struct {
            uintN min;             /* current quantifier limits */
            uintN max;
        } quantifier;
        struct {
            size_t top;             /* backtrack stack state */
            size_t sz;
        } assertion;
    } u;
} REProgState;

typedef struct REBackTrackData {
    size_t sz;                      /* size of previous stack entry */
    jsbytecode *backtrack_pc;       /* where to backtrack to */
    jsbytecode backtrack_op;
    const jschar *cp;               /* index in text of match at backtrack */
    size_t parenIndex;              /* start index of saved paren contents */
    size_t parenCount;              /* # of saved paren contents */
    size_t saveStateStackTop;       /* number of parent states */
    /* saved parent states follow */
    /* saved paren contents follow */
} REBackTrackData;

#define INITIAL_STATESTACK  100
#define INITIAL_BACKTRACK   8000

typedef struct REGlobalData {
    JSContext *cx;
    JSRegExp *regexp;               /* the RE in execution */
    JSBool ok;                      /* runtime error (out_of_memory only?) */
    size_t start;                   /* offset to start at */
    ptrdiff_t skipped;              /* chars skipped anchoring this r.e. */
    const jschar    *cpbegin;       /* text base address */
    const jschar    *cpend;         /* text limit address */

    REProgState *stateStack;        /* stack of state of current parents */
    size_t stateStackTop;
    size_t stateStackLimit;

    REBackTrackData *backTrackStack;/* stack of matched-so-far positions */
    REBackTrackData *backTrackSP;
    size_t backTrackStackSize;
    size_t cursz;                   /* size of current stack entry */

    JSArenaPool     pool;           /* It's faster to use one malloc'd pool
                                       than to malloc/free the three items
                                       that are allocated from this pool */
} REGlobalData;

/*
 * 1. If IgnoreCase is false, return ch.
 * 2. Let u be ch converted to upper case as if by calling
 *    String.prototype.toUpperCase on the one-character string ch.
 * 3. If u does not consist of a single character, return ch.
 * 4. Let cu be u's character.
 * 5. If ch's code point value is greater than or equal to decimal 128 and cu's
 *    code point value is less than decimal 128, then return ch.
 * 6. Return cu.
 */
static jschar
upcase(jschar ch)
{
    jschar cu = JS_TOUPPER(ch);
    if (ch >= 128 && cu < 128)
        return ch;
    return cu;
}

static jschar
downcase(jschar ch)
{
    jschar cl = JS_TOLOWER(ch);
    if (cl >= 128 && ch < 128)
        return ch;
    return cl;
}

/* Construct and initialize an RENode, returning NULL for out-of-memory */
static RENode *
NewRENode(CompilerState *state, REOp op)
{
    JSContext *cx;
    RENode *ren;

    cx = state->context;
    JS_ARENA_ALLOCATE_CAST(ren, RENode *, &cx->tempPool, sizeof *ren);
    if (!ren) {
        JS_ReportOutOfMemory(cx);
        return NULL;
    }
    ren->op = op;
    ren->next = NULL;
    ren->kid = NULL;
    return ren;
}

/*
 * Validates and converts hex ascii value.
 */
static JSBool
isASCIIHexDigit(jschar c, uintN *digit)
{
    uintN cv = c;

    if (cv < '0')
        return JS_FALSE;
    if (cv <= '9') {
        *digit = cv - '0';
        return JS_TRUE;
    }
    cv |= 0x20;
    if (cv >= 'a' && cv <= 'f') {
        *digit = cv - 'a' + 10;
        return JS_TRUE;
    }
    return JS_FALSE;
}


typedef struct {
    REOp op;
    const jschar *errPos;
    size_t parenIndex;
} REOpData;


/*
 * Process the op against the two top operands, reducing them to a single
 * operand in the penultimate slot. Update progLength and treeDepth.
 */
static JSBool
ProcessOp(CompilerState *state, REOpData *opData, RENode **operandStack,
          intN operandSP)
{
    RENode *result;

    switch (opData->op) {
    case REOP_ALT:
        result = NewRENode(state, REOP_ALT);
        if (!result)
            return JS_FALSE;
        result->kid = operandStack[operandSP - 2];
        result->u.kid2 = operandStack[operandSP - 1];
        operandStack[operandSP - 2] = result;

        if (state->treeDepth == TREE_DEPTH_MAX) {
            js_ReportCompileErrorNumber(state->context, state->tokenStream,
                                        JSREPORT_TS | JSREPORT_ERROR,
                                        JSMSG_REGEXP_TOO_COMPLEX);
            return JS_FALSE;
        }
        ++state->treeDepth;

        /*
         * Look at both alternates to see if there's a FLAT or a CLASS at
         * the start of each. If so, use a prerequisite match.
         */
        if (((RENode *) result->kid)->op == REOP_FLAT &&
            ((RENode *) result->u.kid2)->op == REOP_FLAT &&
            (state->flags & JSREG_FOLD) == 0) {
            result->op = REOP_ALTPREREQ;
            result->u.altprereq.ch1 = ((RENode *) result->kid)->u.flat.chr;
            result->u.altprereq.ch2 = ((RENode *) result->u.kid2)->u.flat.chr;
            /* ALTPREREQ, <end>, uch1, uch2, <next>, ...,
                                            JUMP, <end> ... ENDALT */
            state->progLength += 13;
        }
        else
        if (((RENode *) result->kid)->op == REOP_CLASS &&
            ((RENode *) result->kid)->u.ucclass.index < 256 &&
            ((RENode *) result->u.kid2)->op == REOP_FLAT &&
            (state->flags & JSREG_FOLD) == 0) {
            result->op = REOP_ALTPREREQ2;
            result->u.altprereq.ch1 = ((RENode *) result->u.kid2)->u.flat.chr;
            result->u.altprereq.ch2 = ((RENode *) result->kid)->u.ucclass.index;
            /* ALTPREREQ2, <end>, uch1, uch2, <next>, ...,
                                            JUMP, <end> ... ENDALT */
            state->progLength += 13;
        }
        else
        if (((RENode *) result->kid)->op == REOP_FLAT &&
            ((RENode *) result->u.kid2)->op == REOP_CLASS &&
            ((RENode *) result->u.kid2)->u.ucclass.index < 256 &&
            (state->flags & JSREG_FOLD) == 0) {
            result->op = REOP_ALTPREREQ2;
            result->u.altprereq.ch1 = ((RENode *) result->kid)->u.flat.chr;
            result->u.altprereq.ch2 =
                ((RENode *) result->u.kid2)->u.ucclass.index;
            /* ALTPREREQ2, <end>, uch1, uch2, <next>, ...,
                                          JUMP, <end> ... ENDALT */
            state->progLength += 13;
        }
        else {
            /* ALT, <next>, ..., JUMP, <end> ... ENDALT */
            state->progLength += 7;
        }
        break;

    case REOP_CONCAT:
        result = operandStack[operandSP - 2];
        while (result->next)
            result = result->next;
        result->next = operandStack[operandSP - 1];
        break;

    case REOP_ASSERT:
    case REOP_ASSERT_NOT:
    case REOP_LPARENNON:
    case REOP_LPAREN:
        /* These should have been processed by a close paren. */
        js_ReportCompileErrorNumberUC(state->context, state->tokenStream,
                                      JSREPORT_TS | JSREPORT_ERROR,
                                      JSMSG_MISSING_PAREN, opData->errPos);
        return JS_FALSE;

    default:;
    }
    return JS_TRUE;
}

/*
 * Parser forward declarations.
 */
static JSBool ParseTerm(CompilerState *state);
static JSBool ParseQuantifier(CompilerState *state);
static intN ParseMinMaxQuantifier(CompilerState *state, JSBool ignoreValues);

/*
 * Top-down regular expression grammar, based closely on Perl4.
 *
 *  regexp:     altern                  A regular expression is one or more
 *              altern '|' regexp       alternatives separated by vertical bar.
 */
#define INITIAL_STACK_SIZE  128

static JSBool
ParseRegExp(CompilerState *state)
{
    size_t parenIndex;
    RENode *operand;
    REOpData *operatorStack;
    RENode **operandStack;
    REOp op;
    intN i;
    JSBool result = JS_FALSE;

    intN operatorSP = 0, operatorStackSize = INITIAL_STACK_SIZE;
    intN operandSP = 0, operandStackSize = INITIAL_STACK_SIZE;

    /* Watch out for empty regexp */
    if (state->cp == state->cpend) {
        state->result = NewRENode(state, REOP_EMPTY);
        return (state->result != NULL);
    }

    operatorStack = (REOpData *)
        JS_malloc(state->context, sizeof(REOpData) * operatorStackSize);
    if (!operatorStack)
        return JS_FALSE;

    operandStack = (RENode **)
        JS_malloc(state->context, sizeof(RENode *) * operandStackSize);
    if (!operandStack)
        goto out;

    for (;;) {
        parenIndex = state->parenCount;
        if (state->cp == state->cpend) {
            /*
             * If we are at the end of the regexp and we're short one or more
             * operands, the regexp must have the form /x|/ or some such, with
             * left parentheses making us short more than one operand.
             */
            if (operatorSP >= operandSP) {
                operand = NewRENode(state, REOP_EMPTY);
                if (!operand)
                    goto out;
                goto pushOperand;
            }
        } else {
            switch (*state->cp) {
            case '(':
                ++state->cp;
                if (state->cp + 1 < state->cpend &&
                    *state->cp == '?' &&
                    (state->cp[1] == '=' ||
                     state->cp[1] == '!' ||
                     state->cp[1] == ':')) {
                    switch (state->cp[1]) {
                    case '=':
                        op = REOP_ASSERT;
                        /* ASSERT, <next>, ... ASSERTTEST */
                        state->progLength += 4;
                        break;
                    case '!':
                        op = REOP_ASSERT_NOT;
                        /* ASSERTNOT, <next>, ... ASSERTNOTTEST */
                        state->progLength += 4;
                        break;
                    default:
                        op = REOP_LPARENNON;
                        break;
                    }
                    state->cp += 2;
                } else {
                    op = REOP_LPAREN;
                    /* LPAREN, <index>, ... RPAREN, <index> */
                    state->progLength
                        += 2 * (1 + GetCompactIndexWidth(parenIndex));
                    state->parenCount++;
                    if (state->parenCount == 65535) {
                        js_ReportCompileErrorNumber(state->context,
                                                    state->tokenStream,
                                                    JSREPORT_TS |
                                                    JSREPORT_ERROR,
                                                    JSMSG_TOO_MANY_PARENS);
                        goto out;
                    }
                }
                goto pushOperator;

            case ')':
                /*
                 * If there's no stacked open parenthesis, throw syntax error.
                 */
                for (i = operatorSP - 1; ; i--) {
                    if (i < 0) {
                        js_ReportCompileErrorNumber(state->context,
                                                    state->tokenStream,
                                                    JSREPORT_TS |
                                                    JSREPORT_ERROR,
                                                    JSMSG_UNMATCHED_RIGHT_PAREN);
                        goto out;
                    }
                    if (operatorStack[i].op == REOP_ASSERT ||
                        operatorStack[i].op == REOP_ASSERT_NOT ||
                        operatorStack[i].op == REOP_LPARENNON ||
                        operatorStack[i].op == REOP_LPAREN) {
                        break;
                    }
                }
                /* FALL THROUGH */

            case '|':
                /* Expected an operand before these, so make an empty one */
                operand = NewRENode(state, REOP_EMPTY);
                if (!operand)
                    goto out;
                goto pushOperand;

            default:
                if (!ParseTerm(state))
                    goto out;
                operand = state->result;
pushOperand:
                if (operandSP == operandStackSize) {
                    operandStackSize += operandStackSize;
                    operandStack = (RENode **)
                        JS_realloc(state->context, operandStack,
                                   sizeof(RENode *) * operandStackSize);
                    if (!operandStack)
                        goto out;
                }
                operandStack[operandSP++] = operand;
                break;
            }
        }

        /* At the end; process remaining operators. */
restartOperator:
        if (state->cp == state->cpend) {
            while (operatorSP) {
                --operatorSP;
                if (!ProcessOp(state, &operatorStack[operatorSP],
                               operandStack, operandSP))
                    goto out;
                --operandSP;
            }
            JS_ASSERT(operandSP == 1);
            state->result = operandStack[0];
            result = JS_TRUE;
            goto out;
        }

        switch (*state->cp) {
        case '|':
            /* Process any stacked 'concat' operators */
            ++state->cp;
            while (operatorSP &&
                   operatorStack[operatorSP - 1].op == REOP_CONCAT) {
                --operatorSP;
                if (!ProcessOp(state, &operatorStack[operatorSP],
                               operandStack, operandSP)) {
                    goto out;
                }
                --operandSP;
            }
            op = REOP_ALT;
            goto pushOperator;

        case ')':
            /*
             * If there's no stacked open parenthesis, throw syntax error.
             */
            for (i = operatorSP - 1; ; i--) {
                if (i < 0) {
                    js_ReportCompileErrorNumber(state->context,
                                                state->tokenStream,
                                                JSREPORT_TS | JSREPORT_ERROR,
                                                JSMSG_UNMATCHED_RIGHT_PAREN);
                    goto out;
                }
                if (operatorStack[i].op == REOP_ASSERT ||
                    operatorStack[i].op == REOP_ASSERT_NOT ||
                    operatorStack[i].op == REOP_LPARENNON ||
                    operatorStack[i].op == REOP_LPAREN) {
                    break;
                }
            }
            ++state->cp;

            /* Process everything on the stack until the open parenthesis. */
            for (;;) {
                JS_ASSERT(operatorSP);
                --operatorSP;
                switch (operatorStack[operatorSP].op) {
                case REOP_ASSERT:
                case REOP_ASSERT_NOT:
                case REOP_LPAREN:
                    operand = NewRENode(state, operatorStack[operatorSP].op);
                    if (!operand)
                        goto out;
                    operand->u.parenIndex =
                        operatorStack[operatorSP].parenIndex;
                    JS_ASSERT(operandSP);
                    operand->kid = operandStack[operandSP - 1];
                    operandStack[operandSP - 1] = operand;
                    if (state->treeDepth == TREE_DEPTH_MAX) {
                        js_ReportCompileErrorNumber(state->context,
                                                    state->tokenStream,
                                                    JSREPORT_TS |
                                                    JSREPORT_ERROR,
                                                    JSMSG_REGEXP_TOO_COMPLEX);
                        goto out;
                    }
                    ++state->treeDepth;
                    /* FALL THROUGH */

                case REOP_LPARENNON:
                    state->result = operandStack[operandSP - 1];
                    if (!ParseQuantifier(state))
                        goto out;
                    operandStack[operandSP - 1] = state->result;
                    goto restartOperator;
                default:
                    if (!ProcessOp(state, &operatorStack[operatorSP],
                                   operandStack, operandSP))
                        goto out;
                    --operandSP;
                    break;
                }
            }
            break;

        case '{':
        {
            const jschar *errp = state->cp;

            if (ParseMinMaxQuantifier(state, JS_TRUE) < 0) {
                /*
                 * This didn't even scan correctly as a quantifier, so we should
                 * treat it as flat.
                 */
                op = REOP_CONCAT;
                goto pushOperator;
            }

            state->cp = errp;
            /* FALL THROUGH */
        }

        case '+':
        case '*':
        case '?':
            js_ReportCompileErrorNumberUC(state->context, state->tokenStream,
                                          JSREPORT_TS | JSREPORT_ERROR,
                                          JSMSG_BAD_QUANTIFIER, state->cp);
            result = JS_FALSE;
            goto out;

        default:
            /* Anything else is the start of the next term. */
            op = REOP_CONCAT;
pushOperator:
            if (operatorSP == operatorStackSize) {
                operatorStackSize += operatorStackSize;
                operatorStack = (REOpData *)
                    JS_realloc(state->context, operatorStack,
                               sizeof(REOpData) * operatorStackSize);
                if (!operatorStack)
                    goto out;
            }
            operatorStack[operatorSP].op = op;
            operatorStack[operatorSP].errPos = state->cp;
            operatorStack[operatorSP++].parenIndex = parenIndex;
            break;
        }
    }
out:
    if (operatorStack)
        JS_free(state->context, operatorStack);
    if (operandStack)
        JS_free(state->context, operandStack);
    return result;
}

/*
 * Hack two bits in CompilerState.flags, for use within FindParenCount to flag
 * its being on the stack, and to propagate errors to its callers.
 */
#define JSREG_FIND_PAREN_COUNT  0x8000
#define JSREG_FIND_PAREN_ERROR  0x4000

/*
 * Magic return value from FindParenCount and GetDecimalValue, to indicate
 * overflow beyond GetDecimalValue's max parameter, or a computed maximum if
 * its findMax parameter is non-null.
 */
#define OVERFLOW_VALUE          ((uintN)-1)

static uintN
FindParenCount(CompilerState *state)
{
    CompilerState temp;
    int i;

    if (state->flags & JSREG_FIND_PAREN_COUNT)
        return OVERFLOW_VALUE;

    /*
     * Copy state into temp, flag it so we never report an invalid backref,
     * and reset its members to parse the entire regexp.  This is obviously
     * suboptimal, but GetDecimalValue calls us only if a backref appears to
     * refer to a forward parenthetical, which is rare.
     */
    temp = *state;
    temp.flags |= JSREG_FIND_PAREN_COUNT;
    temp.cp = temp.cpbegin;
    temp.parenCount = 0;
    temp.classCount = 0;
    temp.progLength = 0;
    temp.treeDepth = 0;
    temp.classBitmapsMem = 0;
    for (i = 0; i < CLASS_CACHE_SIZE; i++)
        temp.classCache[i].start = NULL;

    if (!ParseRegExp(&temp)) {
        state->flags |= JSREG_FIND_PAREN_ERROR;
        return OVERFLOW_VALUE;
    }
    return temp.parenCount;
}

/*
 * Extract and return a decimal value at state->cp.  The initial character c
 * has already been read.  Return OVERFLOW_VALUE if the result exceeds max.
 * Callers who pass a non-null findMax should test JSREG_FIND_PAREN_ERROR in
 * state->flags to discover whether an error occurred under findMax.
 */
static uintN
GetDecimalValue(jschar c, uintN max, uintN (*findMax)(CompilerState *state),
                CompilerState *state)
{
    uintN value = JS7_UNDEC(c);
    JSBool overflow = (value > max && (!findMax || value > findMax(state)));

    /* The following restriction allows simpler overflow checks. */
    JS_ASSERT(max <= ((uintN)-1 - 9) / 10);
    while (state->cp < state->cpend) {
        c = *state->cp;
        if (!JS7_ISDEC(c))
            break;
        value = 10 * value + JS7_UNDEC(c);
        if (!overflow && value > max && (!findMax || value > findMax(state)))
            overflow = JS_TRUE;
        ++state->cp;
    }
    return overflow ? OVERFLOW_VALUE : value;
}

/*
 * Calculate the total size of the bitmap required for a clas