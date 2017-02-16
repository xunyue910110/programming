#ifndef __TD_M_BACKUPDATETASK_EXT_H_
#define __TD_M_BACKUPDATETASK_EXT_H_

#include "mwci/CFmlBuf.h"
#include "BackupdatetaskDAO.h"

namespace ParaDom {

void GetFromFmlBuf(CFmlBuf &tmpBuf, Backupdatetask &rBackupdatetask, int fldOc=0);
void PutToFmlBuf(Backupdatetask &rBackupdatetask, CFmlBuf &tmpBuf, int fldOc=0);
void convert(CFmlBuf &tmpBuf, VBackupdatetask &vrBackupdatetask);
void convert(VBackupdatetask &vrBackupdatetask, CFmlBuf &tmpBuf);

} // namespace ParaDom

#endif
