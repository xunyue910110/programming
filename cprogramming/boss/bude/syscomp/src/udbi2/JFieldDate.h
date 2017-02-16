#ifndef __JFIELDDATE_H_
#define __JFIELDDATE_H_

#include "JField.h"

namespace UDBI {
using std::string;

class JFieldDate : public JField {
private:
	char* date;
public:
	JFieldDate(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs);
	virtual ~JFieldDate();
	virtual void Define(OCIStmt* vpStmt, int fldPos);
    virtual void GetString(string& val, int curRec);
};

} // UDBI


#endif /* __JFIELDDATE_H_ */
