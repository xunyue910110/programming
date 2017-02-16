#ifndef FT_H
#define FT_H

#include "dta.h"

class CFileTransf
{
 public:
  int conn(const char *host);
  int disconn();
  int putfile(const char *sourcefile, const char *targetfile,
              DTAFTINFO *ftinfo);
  int getfile(const char *sourcefile, const char *targetfile,
              DTAFTINFO *ftinfo);

 protected:
  DTAHCONN hconn;
};

#endif  // FT_H
