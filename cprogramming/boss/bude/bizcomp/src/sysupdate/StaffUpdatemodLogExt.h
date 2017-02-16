#ifndef __TD_M_STAFF_UPDATEMOD_LOG_EXT_H_
#define __TD_M_STAFF_UPDATEMOD_LOG_EXT_H_

#include "mwci/CFmlBuf.h"
#include "StaffUpdatemodLogDAO.h"

namespace ParaDom {

void GetFromFmlBuf(CFmlBuf &tmpBuf, StaffUpdatemodLog &rStaffUpdatemodLog, int fldOc=0);
void PutToFmlBuf(StaffUpdatemodLog &rStaffUpdatemodLog, CFmlBuf &tmpBuf, int fldOc=0);
void convert(CFmlBuf &tmpBuf, VStaffUpdatemodLog &vrStaffUpdatemodLog);
void convert(VStaffUpdatemodLog &vrStaffUpdatemodLog, CFmlBuf &tmpBuf);

} // namespace ParaDom

#endif
