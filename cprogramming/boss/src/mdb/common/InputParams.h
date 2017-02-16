// ############################################
// Source file : InputParam.h
// Version     : 0.1.1
// Language	   : ANSI C++
// OS Platform : UNIX
// Authors     : chen min
// E_mail      : chenm@lianchuang.com
// Create      : 2008-6-4 16:42:43
// Update      : 2008-6-4 16:42:45
// Copyright(C): chen min, Linkage.
// ############################################

#ifndef INPUTPARAMS1_H_HEADER_INCLUDED_B7B9C467
#define INPUTPARAMS1_H_HEADER_INCLUDED_B7B9C467

#include "MdbConstDef.h"

// 作为MDB的使用者,和MDB API之间的参数传递接口
//##ModelId=48464A2503D8
class InputParams
{
  public:
    //##ModelId=48464CB7008C
    InputParams(int iNum);
    InputParams(){}

    //##ModelId=48464CBE033C
    ~InputParams();
    
    void reset();
    void setValue(const COLUMN_VAR_TYPE type,void * pValue);
    void setValue(const COLUMN_VAR_TYPE type, long l);
  	InputParams & operator = (const InputParams &input);

  public:
    // 参数值
    //##ModelId=48464A43003E
    void ** m_pValues;
    // 参数类型
    //##ModelId=48464A5201C5
    int *m_iValueLenths;
    // 参数的个数
    //##ModelId=48464AC1000F
    int m_iNum;
    // 参数类型
    COLUMN_VAR_TYPE *m_varTypes;

  private:
  	InputParams(const InputParams &input);

  private:
	int m_iSequnce;    
	bool m_isReseted;
};



#endif /* INPUTPARAMS1_H_HEADER_INCLUDED_B7B9C467 */
