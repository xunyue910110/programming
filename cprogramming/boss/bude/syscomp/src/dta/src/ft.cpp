#include "ft.h"

int CFileTransf::conn(const char *host)
{
  return DTA_CONN(&hconn,host);
}

int CFileTransf::disconn()
{
  return DTA_DISC(hconn);
}

int CFileTransf::putfile(const char *sourcefile, const char *targetfile,
                         DTAFTINFO *ftinfo)
{
  return DTA_PUT(hconn,sourcefile,targetfile,ftinfo);
}

int CFileTransf::getfile(const char *sourcefile, const char *targetfile,
                         DTAFTINFO *ftinfo)
{
  return DTA_GET(hconn,sourcefile,targetfile,ftinfo);
}





