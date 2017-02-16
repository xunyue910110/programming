#include "time_span.hpp"
#include "debug.hpp"
#include <stdio.h>

namespace archy {

std::string time_span::format( const char* fmt ) const
{
	ARCHY_ASSERT( fmt != 0 );	
    	const char *p;
	std::string strFmt;
	char buf[8];
    	for(p = fmt; *p != '\0'; p++)  {
            if(*p != '%') {
            	sprintf(buf, "%c", *p );
                strFmt += buf;
                continue;
            }
            switch(*++p) {
            case 'D':
            		sprintf(buf, "%02d", get_days() );
                    break;
            case 'H':
            		sprintf(buf, "%02d", get_hours() );
                    break;
            case 'h':
            		sprintf(buf, "%d", get_hours() );
                    break;
            case 'N':
            		sprintf(buf, "%02d", get_minutes() );
                    break;
            case 'n':
            		sprintf(buf, "%d", get_minutes() );
                    break;
            case 'S':
            		sprintf(buf, "%02d", get_seconds() );
                    break;
            case 's':
            		sprintf(buf, "%d", get_seconds() );
                    break;
            case '%':
            		sprintf(buf, "%c", '%' );
                    break;
            }
            strFmt += buf;
    	}
	return strFmt;
}

} // namespace archy

