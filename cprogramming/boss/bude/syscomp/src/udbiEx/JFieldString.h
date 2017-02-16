#ifndef __JFIELDSTRING_H_
#define __JFIELDSTRING_H_

#include "JField.h"

namespace UDBI {

using std::string;
      
class JFieldString : public JField {
public:
	JFieldString(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs);
	virtual ~JFieldString();
	virtual void GetString(string& val, int curRec);
	virtual void Define(OCIStmt* vpStmt, int fldPos);

private:
	char* data;
};


} // UDBI


#endif /* __JFIELDSTRING_H_ */
