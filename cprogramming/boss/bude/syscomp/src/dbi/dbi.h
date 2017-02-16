//-------------------------------------------------------------------
// Name			: dbi.h
// Version		: 0.2.8
// Create		: 2001.8.1
// Update		: 2001.8.21
// Auther		: qianyx@lianchuang.com
// Copyright	: All Rights Reserved.
// Description	: Should be included in DBI application programs.
//-------------------------------------------------------------------

#ifndef DBI_H
#define DBI_H

# ifdef _FOR_ORACLE_
#   include "oradbi.h"
# else
#    ifdef _FOR_SYBASE_
#      include "sybdbi.h"
#    else
#      include "oradbi.h"
#    endif
# endif


#endif //DBI_H

//------------------------------ THE END ----------------------------

