#ifndef __JFIELD_H_
#define __JFIELD_H_

#include "udbi-inter.h"

namespace UDBI {  
using std::string;

class JField {
public:
    JField(OCIParam* vpParam, OCIError* vpErr, ub2 vFieldType, int vRecs);
    virtual ~JField();
    virtual void GetName(string& vFieldName) const;
    virtual int GetType() const;
    virtual void GetTypeName(string& vFieldTypeName) const;
    virtual unsigned GetSize() const;
    virtual int GetReqType() const;
    virtual bool isNull(int rec) const;
    virtual void Define(OCIStmt* vpStmt, int fldPos) = 0;
    virtual void GetString(string &val, int curRec) = 0;

protected:
    virtual void SetTypeName();
    OCIDefine* m_definehp;
    OCIError* m_errhp;
    sb2* m_NullIndicator;
    unsigned m_ColSize;
    ub2 m_ReqType;
    int m_nRecs;
    ub2 m_ColType;
    string m_ColName;
    string m_ColTypeName;
};

} // namespace UDBI


#endif /* __JFIELD_H_ */
