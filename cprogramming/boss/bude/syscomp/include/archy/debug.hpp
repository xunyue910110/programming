/*
some macros used for DEBUG version or release version,
these macros can be used just like those  defined in Mircrosoft MFC

written by huangjb 2002.02.26 huangjb@lianchuang.com

1、ARCHY_TRACE 
	ARCHY_TRACE( parameters_like_in_printf )
	
	      Provides similar functionality to the printf function 
	by sending a formatted string to a dump device 
	such as a file or debug monitor. Like printf for C 
	programs under MS-DOS, the TRACE macro is a 
	convenient way to track the value of variables 
	as your program executes.
	       In the Debug environment, the TRACE macro 
	output goes to printf. In the Release environment, it does nothing.

2、ARCHY_ASSERT
	ARCHY_ASSERT( booleanExpression )
	
	     Evaluates its argument. If the result is 0, the macro prints 
	a diagnostic message and aborts the program. 
	If the condition is nonzero, it does nothing.
	       In the Release version , ASSERT does 
	not evaluate the expression and thus will not 
	interrupt the program.

3、ARCHY_VERIFY
	ARCHY_VERIFY( booleanExpression )
	
	    In the debug version , the VERIFY macro evaluates 
	its argument. If the result is 0, the macro prints a 
	diagnostic message and halts the program. 
	If the condition is nonzero, it does nothing.
	     In the release version , VERIFY evaluates 
	the expression but does not print or 
	interrupt the program. For example, if the expression 
	is a function call, the call will be made.

4、ARCHY_SHOW
	ARCHY_SHOW( expression )
	
	   In the debug version, the SHOW macro show the  
	expression string on the screen, and then execute 
	the expression. In the release version, SHOW does not
	show the expression string but still execute it!
	   For example:
	      SHOW( getCount() );
	      --In the debug version, it has the same effect as follow
	   statements:
	   	printf("getCount()\n");
	   	getCount();
	     --In the release version, it just equals the follow:
	          getCount();
5、
	          
*/
#ifndef DEBUG_H_FILE
#define DEBUG_H_FILE


#ifdef NDEBUG
	#define ARCHY_CERR if (1) ; else cerr
	#define ARCHY_COUT if (1) ; else cout
	
	#undef ARCHY_TRACE
	#define ARCHY_TRACE
	
	#undef ARCHY_ASSERT
	#define ARCHY_ASSERT(a)	
	
	#undef ARCHY_VERIFY
	#define ARCHY_VERIFY(f)		((void)(f))
	
	#undef ARCHY_SHOW
	#define ARCHY_SHOW(X) 		X
	
#else
	#define ARCHY_CERR if (0) ; else cerr
	#define ARCHY_COUT if (0) ; else cout

	#ifndef ARCHY_TRACE
    #include <stdio.h>
	#define ARCHY_TRACE		 printf
	#endif
	
	#ifndef ARCHY_ASSERT
	#include <assert.h>
	#define ARCHY_ASSERT(a)		assert(a)
	#endif
	
	#ifndef ARCHY_VERIFY
	#define ARCHY_VERIFY(f)		ASSERT(f)
	#endif
	
	#ifndef ARCHY_SHOW
	#define ARCHY_SHOW(X) 		printf(#X"\n");X
	#endif
#endif


#endif //DEBUG_H_FILE

