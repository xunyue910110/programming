#ifndef __TD_M_UPDATEMOD_EXT_H_
#define __TD_M_UPDATEMOD_EXT_H_

#include "mwci/CFmlBuf.h"
#include "UpdatemodDAO.h"

namespace ParaDom {

void GetFromFmlBuf(CFmlBuf &tmpBuf, Updatemod &rUpdatemod, int fldOc=0);
void PutToFmlBuf(Updatemod &rUpdatemod, CFmlBuf &tmpBuf, int fldOc=0);
void convert(CFmlBuf &tmpBuf, VUpdatemod &vrUpdatemod);
void convert(VUpdatemod &vrUpdatemod, CFmlBuf &tmpBuf);

} // namespace ParaDom

#endif
