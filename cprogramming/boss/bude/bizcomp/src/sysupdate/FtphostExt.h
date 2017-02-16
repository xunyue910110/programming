#ifndef __TD_S_FTPHOST_EXT_H_
#define __TD_S_FTPHOST_EXT_H_

#include "mwci/CFmlBuf.h"
#include "FtphostDAO.h"

namespace ParaDom {

void GetFromFmlBuf(CFmlBuf &tmpBuf, Ftphost &rFtphost, int fldOc=0);
void PutToFmlBuf(Ftphost &rFtphost, CFmlBuf &tmpBuf, int fldOc=0);
void convert(CFmlBuf &tmpBuf, VFtphost &vrFtphost);
void convert(VFtphost &vrFtphost, CFmlBuf &tmpBuf);

} // namespace ParaDom

#endif
