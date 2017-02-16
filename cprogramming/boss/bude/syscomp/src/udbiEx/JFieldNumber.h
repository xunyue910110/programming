#ifndef __JFIELDNUMBER_H_
#define __JFIELDNUMBER_H_

#include "JField.h"

namespace UDBI {

using std::string;

    
class JFieldNumber : public JField {
public:
	JFieldNumber(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs);
	virtual ~JFieldNumber();
	virtual void Define(OCIStmt* vpStmt, int fldPos);
    virtual void GetString(string& val, int curRec);
	virtual void GetInt(int& val, int curRec);
	virtual void GetInt64(int64_t& val, int curRec);
	
private:
	char* data;
};

} // UDBI


#endif /* __JFIELDNUMBER_H_ */
