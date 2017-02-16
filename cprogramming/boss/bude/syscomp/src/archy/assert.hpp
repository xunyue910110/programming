//
//  boost/assert.hpp - ARCHY_ASSERT(expr)
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  Note: There are no include guards. This is intentional.
//
//  See http://www.boost.org/libs/utility/assert.html for documentation.
//

#undef ARCHY_ASSERT

#if defined(ARCHY_DISABLE_ASSERTS)

# define ARCHY_ASSERT(expr) ((void)0)

#elif defined(ARCHY_ENABLE_ASSERT_HANDLER)

#include "current_function.hpp"
#include <cstdio>

namespace archy
{

void assertion_failed(char const * expr, char const * function, char const * file, long line)
{
	printf("assertion failed: \"%s\", %s, line %d", pszAssertion, pszFile, nLine);
}

} // namespace archy

#define ARCHY_ASSERT(expr) ((expr)? ((void)0): ::archy::assertion_failed(#expr, ARCHY_CURRENT_FUNCTION, __FILE__, __LINE__))

#else
# include <assert.h>
# define ARCHY_ASSERT(expr) assert(expr)
#endif
