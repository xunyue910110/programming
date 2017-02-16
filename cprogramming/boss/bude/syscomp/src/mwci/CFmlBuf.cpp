
#include "mwci/CFmlBuf.h"

static Logger& logger = Logger::getLogger("gboss.scom.mwci.CFmlBuf");

//##ModelId=418A13EA0123
void CFmlBuf::SetValue(const char *strFldNames, void *value, FLDOCC iIdx ,FLDLEN fldlen) throw(CFmlException)
{
	SetValue(GetFldID(strFldNames), (void *)value, iIdx,fldlen);
}

//##ModelId=418A19D20182
int CFmlBuf::GetValue(const char *strFldName, void *value, FLDOCC iIdx ) throw(CFmlException)
{
	if(!this->IsFldExist(strFldName,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[strFldName:%s][iIdx:%d]}",strFldName,iIdx);
	return GetValue(GetFldID(strFldName),(void *)value,iIdx);
}

//##ModelId=418A1A690175
int CFmlBuf::BatSetVal()
{
	//Not support!
	return 0;
}

//##ModelId=418A248A0169
int CFmlBuf::BatGetVal()
{
	//Not support!
	return 0;
}

//##ModelId=418A24B70204
int CFmlBuf::LoadFromFile(const char *strFileName)
{
	//Not support!
	return 0;
}

//##ModelId=418A24D803BA
int CFmlBuf::LoadFromTab(const char *tabName)
{
	//Not support!
	return 0;
}

int CFmlBuf::SetValueEx(FLDID lFldid, void *value, FLDOCC iIdx,FLDLEN fldlen,bool bIfThorwEx) throw(CFmlException)
{
	try
	{
		SetValue(lFldid, value, iIdx,fldlen);
	}
	catch(CFmlException &ex)
	{
		if (bIfThorwEx)
			throw ex;
		else{
			LOG_DEBUG_P1(logger,"SetValueEx Exception:[FldID:%d]",lFldid);
			return -1;
		}
	}
	return 0;
}

int CFmlBuf::SetValueEx(const char *strFldName, void *value, FLDOCC iIdx,FLDLEN fldlen,bool bIfThorwEx) throw(CFmlException)
{
	try
	{
		SetValue(strFldName, value, iIdx,fldlen);
	}
	catch(CFmlException &ex)
	{
		if (bIfThorwEx)
			throw ex;
		else{
			LOG_DEBUG_P1(logger,"SetValueEx Exception:[FldName:%s]",strFldName);
			return -1;
		}
	}
	return 0;
}

//##ModelId=418A36A4020D
void CFmlBuf::SetValue(FLDID lFldid, void *value, FLDOCC iIdx,FLDLEN fldlen) throw(CFmlException)
{
	// Check vaildate of FLDID and FLDOCC
	if (lFldid == 0 || iIdx<0 ) {
		THROW_C_P2(CFmlException,TUX_FML_INVALID_OPER,"[FldID:%d,iIdx:%d]",lFldid,iIdx);
	}

	long iUnuseSize = Funused( this->m_pFmlBuf );

	if( iUnuseSize == -1 )
	{
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	}

	int iLenthOfValue = sizeof( double );
	switch( this->GetFldType( lFldid ) )
	{
		case 5 : iLenthOfValue = strlen( (char *)value );break;
		case 6 : if (fldlen==0)
				THROW_C_P2(CFmlException,TUX_FML_INVALID_LENGTH,"invalidate length,{detail[FldId:%d],[iIdx:%d]}",lFldid,iIdx);
				iLenthOfValue = fldlen;
				break;
	}

	//判断是否有足够的缓冲区
	if( iUnuseSize <= iLenthOfValue + SIZE_4K)
	{
		if(iLenthOfValue > SIZE_4K)
			THROW_C_P1(CFmlException,TUX_FML_SIZEOVER,"[%s]FML Value Length Over SIZE_4k",GetFldName(lFldid));
		
		//FMLBUFFER扩展的话最少扩展DEFAULT_EXTAND_SIZE
		//iLenthOfValue= iLenthOfValue<DEFAULT_EXTAND_SIZE ?DEFAULT_EXTAND_SIZE :iLenthOfValue;
			
		iLenthOfValue = (Fsizeof(this->m_pFmlBuf) + SIZE_4K);
		
		if(iLenthOfValue > SIZE_200M)
			THROW_C(CFmlException,TUX_FML_SIZEOVER,"FML BufSize Length Over SIZE_200M");
		
		if( ( this->m_pFmlBuf = (FBFR*)tprealloc( (char*)m_pFmlBuf, iLenthOfValue) ) == (FBFR*)NULL )
		{
			THROW_C_P2(CFmlException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}
	}

	if( Fchg( this->m_pFmlBuf, lFldid, iIdx, (char *)value, (FLDLEN)0 ) == -1 )
	{
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	}

}

//##ModelId=418A36A4022B
int CFmlBuf::GetValue(FLDID lFldId, void *value, FLDOCC iIdx) throw(CFmlException)
{
	if(!this->IsFldExist(lFldId,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[lFldId:%d][iIdx:%d]}",lFldId,iIdx);

	switch( this->GetFldType( lFldId ) )
	{
		//shot int
		case 1 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&siValue, (FLDLEN*)0 ) == -1 )
					THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
				value=&siValue;
				return 1;
				break;
		//long int
		case 2 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&liValue, (FLDLEN*)0 ) == -1 )
					THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
				value=&siValue;
				return 2;
				break;
		//float
		case 3 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&fValue, (FLDLEN*)0 ) == -1 )
					THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
				return 3;
				break;
		//double
		case 4 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&dValue, (FLDLEN*)0 ) == -1 )
					THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
				return 4;
				break;
		//string (In fact is char * with NULL in end .)
		case 5 :	if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)value, (FLDLEN*)0 ) == -1 )
					THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
				return 5;
				break;
		//char []
		case 6 :	if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)value, (FLDLEN*)0 ) == -1 )
					THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
				return 6;
				break;
		default :
				THROW(CFmlException,"unknow filed type,no support");
	}
	return 	this->GetFldType( lFldId ) ;
}


//##ModelId=41D4F7A8009C
int CFmlBuf::GetValueEx(FLDID lFldId, void *value, FLDOCC iIdx,bool bIfThrowEx) throw (CFmlException)
{
	if(!this->IsFldExist(lFldId,iIdx))
		if(bIfThrowEx)
			THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[lFldId:%d][iIdx:%d]}",lFldId,iIdx);
		else
			return -3;

	switch( this->GetFldType( lFldId ) )
	{
		//shot int
		case 1 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&siValue, (FLDLEN*)0 ) == -1 )
					if(bIfThrowEx)
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					else
						return -1;
				value=&siValue; return 1;
				break;
		//long int
		case 2 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&liValue, (FLDLEN*)0 ) == -1 )
					if(bIfThrowEx)
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					else
						return -1;
				value=&siValue;
				return 2;
				break;
		//float
		case 3 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&fValue, (FLDLEN*)0 ) == -1 )
					if(bIfThrowEx)
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					else
						return -1;
				value=&siValue; return 3;
				break;
		//double
		case 4 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&dValue, (FLDLEN*)0 ) == -1 )
					if(bIfThrowEx)
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					else
						return -1;
				value=&siValue;
				return 4;
				break;
		//string (In fact is char * with NULL in end .)
		case 5 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)value, (FLDLEN*)0 ) == -1 )
					if(bIfThrowEx)
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					else
						return -1;
				return 5;
				break;
		//char []
		case 6 : if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)value, (FLDLEN*)0 ) == -1 )
					if(bIfThrowEx)
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					else
						return -1;
				return 6;
				break;
		default :
				return -2;
	}
	return -2 ;
}

//##ModelId=418A36A4022B
int CFmlBuf::GetValueEx(const char *strFldName, void *value, FLDOCC iIdx,bool bIfThrowEx) throw(CFmlException)
{
	if(!this->IsFldExist(strFldName,iIdx))
		if(bIfThrowEx)
			THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[strFldName:%s][iIdx:%d]}",strFldName,iIdx);
	return GetValueEx(GetFldID(strFldName), (void *)value, iIdx);
}

//##ModelId=418A36A40253
bool CFmlBuf::IsFldExist(const char *strFldName, FLDOCC iIdx) throw(CFmlException)
{
	return IsFldExist(GetFldID(strFldName),iIdx);
}

//##ModelId=418B4E1201CB
bool CFmlBuf::IsFldExist(FLDID lFldId, FLDOCC iIdx) throw(CFmlException)
{
	if( Ffind( this->m_pFmlBuf, lFldId, iIdx, (FLDLEN*)0 ) != NULL )
		return true;
	else
		return false;
}

//##ModelId=418B4E710196
int CFmlBuf::GetFldType(const char * strFldName) throw(CFmlException)
{
	FLDID  id=0;
	id = GetFldID(strFldName) ;
	return GetFldType(  id );
}

//##ModelId=418B4EB201F4
int CFmlBuf::GetFldType(FLDID fldid) throw(CFmlException)
{
	return Fldtype( fldid );
}

//##ModelId=418B4F1600AD
int CFmlBuf::GetCount(const char *strFldName) throw(CFmlException)
{
	return GetCount(GetFldID(strFldName));
}

//##ModelId=418B4F4F0394
int CFmlBuf::GetCount(FLDID fldid) throw(CFmlException)
{
	return Foccur( this->m_pFmlBuf, fldid);
}

//##ModelId=418B5EFF0349
int CFmlBuf::DelFld(const char *strFldName, FLDOCC iIdx) throw(CFmlException)
{
	return DelFld(GetFldID(strFldName), iIdx);
}

//##ModelId=418B60D602E0
int CFmlBuf::DelFld(FLDID lFldID, FLDOCC iIdx) throw(CFmlException)
{
	if(Fdel( this->m_pFmlBuf, lFldID, iIdx )==-1)
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	return 0;
}

//##ModelId=418B60FE037E
void CFmlBuf::Print()
{
	if( this->m_pFmlBuf != NULL )
		Fprint(this->m_pFmlBuf);
}

//##ModelId=419B514C01BF
long CFmlBuf::CalBufSize() const
{
	if (this->m_pFmlBuf)
		return Fsizeof(this->m_pFmlBuf);
	else
		return 0L;
}

//##ModelId=418F2D1C02CE
CFmlBuf::CFmlBuf(const CFmlBuf& right) throw(CFmlException)
{
	this->m_fldid=right.m_fldid;
	this->m_fldidx=right.m_fldidx;
//	this->m_iState=right.m_iState;
//	this->m_lBufSize=right.m_lBufSize;
//	this->ClearFmlValue();


	const long iLenthOfBuf =
		right.GetBufSize()<DEFAULT_SIZE?DEFAULT_SIZE:right.GetBufSize();
	if( ( this->m_pFmlBuf = (FBFR*)tpalloc( (char*)FMLTYPE, NULL, iLenthOfBuf ) ) == (FBFR*)NULL )
	{
		THROW_C_P2(CFmlException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}


	if( Fcpy( this->m_pFmlBuf, right.m_pFmlBuf ) == -1 )
	{
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	}
}

//##ModelId=418F2D1C035B
CFmlBuf::~CFmlBuf()
{
	if( this->m_pFmlBuf != NULL )
		tpfree( (char*)(this->m_pFmlBuf) );
}

//##ModelId=418F2D1C03A1
CFmlBuf& CFmlBuf::operator=(const CFmlBuf& right) throw(CFmlException)
{
	this->m_fldid=right.m_fldid;
	this->m_fldidx=right.m_fldidx;
//	this->m_iState=right.m_iState;
//	this->m_lBufSize=right.m_lBufSize;
	this->ClearFmlValue();

	if( right.GetBufSize() > this->GetBufSize()  )
	{

		long iLenthOfBuf;
		iLenthOfBuf = right.GetBufSize()<SIZE_32K?SIZE_32K:right.GetBufSize()+SIZE_1K;
		if( ( this->m_pFmlBuf = (FBFR*)tprealloc( (char*)m_pFmlBuf, iLenthOfBuf ) ) == (FBFR*)NULL )
		{
			THROW_C_P2(CFmlException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}

	}

	if( Fcpy( this->m_pFmlBuf, right.m_pFmlBuf ) == -1 )
	{
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	}
	return *this;
}

//##ModelId=418F5F520097
void CFmlBuf::ClearFmlValue()
{
	if( Finit( this->m_pFmlBuf, (FLDLEN)Fsizeof(this->m_pFmlBuf) ) == -1 )
	{
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	}
}

//##ModelId=41902B550003
void CFmlBuf::AppFldValue(FLDID fldid, void *value,FLDLEN fldlen) throw(CFmlException)
{
	long iUnuseSize = Funused( this->m_pFmlBuf );
	if( iUnuseSize == -1 )
	{
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	}

	int iLenthOfValue = sizeof( double );
	if( this->GetFldType( fldid ) == 5 )	//如果数据类型是STRING
	{
		iLenthOfValue = strlen( (char *)value );
	}
	if( this->GetFldType( fldid ) == 6 )	//如果数据类型是C arrary
	{
		if (fldlen==0)
			THROW_C(CFmlException,TUX_FML_INVALID_LENGTH,"invaild length");
		iLenthOfValue = fldlen;
	}

	if( iUnuseSize < SIZE_4K )
		iUnuseSize = 0L;

	//判断是否有足够的缓冲区
	if( iUnuseSize < iLenthOfValue + SIZE_1K)
	{
		//FMLBUFFER扩展的话最少扩展DEFAULT_EXTAND_SIZE
		iLenthOfValue= iLenthOfValue<DEFAULT_EXTAND_SIZE ?DEFAULT_EXTAND_SIZE :iLenthOfValue;
		if( ( this->m_pFmlBuf = (FBFR*)tprealloc( (char*)m_pFmlBuf, Fsizeof(this->m_pFmlBuf) + iLenthOfValue ) ) == (FBFR*)NULL )
		{
			THROW_C_P2(CFmlException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
		}
	}

	if(Fadd( this->m_pFmlBuf, fldid, (char*)value, (FLDLEN)0 ) == -1)
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	if (Findex( this->m_pFmlBuf, (FLDOCC)0 ) == -1)
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));

}

//##ModelId=41902B8801F1
void CFmlBuf::AppFldValue(const char *strFldName, void *value,FLDLEN fldlen) throw(CFmlException)
{
	AppFldValue(GetFldID(strFldName),value,fldlen);
}

//##ModelId=41902C0603DD
long CFmlBuf::GetInt(FLDID lFldId, FLDOCC iIdx) throw(CFmlException)
{
	long iRet;

	if(!this->IsFldExist(lFldId,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[lFldId:%d][iIdx:%d]}",lFldId,iIdx);

	int iCase = this->GetFldType( lFldId );
	switch( iCase )
	{
	case 0:
		{
			short iTemp;
			if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&iTemp, (FLDLEN*)0 ) == -1 )
			{
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
			}
			iRet = iTemp;
			break;
		}
	case 1:
		{
			if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&iRet, (FLDLEN*)0 ) == -1 )
			{
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
			}
			break;
		}
	default:
		{
			//try convert to long
			if( CFget( this->m_pFmlBuf, lFldId, iIdx, (char *)&iRet, (FLDLEN*)0, 1) == -1 )
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
		}
	}

	return(iRet);

}

//##ModelId=41902C150168
const string CFmlBuf::GetString(FLDID lFldId, FLDOCC iIdx) throw(CFmlException)
{
	FLDLEN len;
	string strData;
	int iCase = this->GetFldType( lFldId );
	char *chValue = NULL;
	char  preDefinechValue[DEFAULT_SIZE];

	if(!this->IsFldExist(lFldId,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[lFldId:%d][iIdx:%d]}",lFldId,iIdx);

	switch( iCase )
	{
	case 5:
		{	//计算字符串长度
			if((len=Flen(this->m_pFmlBuf,lFldId,iIdx))==-1)
					THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
			if (DEFAULT_SIZE>len)
				{
					if( Fget( this->m_pFmlBuf, lFldId, iIdx, preDefinechValue,(FLDLEN*)0 ) == -1 )
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					strData = preDefinechValue;
				}
			else
				{
					chValue = new char[len+SIZE_1K];
					if (chValue == NULL)
						THROW_C(CFmlException,FMALLOC,"malloc memory fail");
					if ( Fget( this->m_pFmlBuf, lFldId, iIdx, chValue,(FLDLEN*)0 ) == -1 )
					{
						delete [] chValue;
						THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
					}
					strData = chValue;
					delete [] chValue;
				}
			break;
		}
	default:
		{
			// try convert to char *
			if ( CFget( this->m_pFmlBuf, lFldId, iIdx, preDefinechValue,(FLDLEN*)0,5 ) == -1 )
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
			else
				strData = preDefinechValue;
		}
	}

	return strData;
}

//##ModelId=41902C1C0258
char CFmlBuf::GetChar(FLDID lFldId, FLDOCC iIdx) throw(CFmlException)
{
	char ch;

	if(!this->IsFldExist(lFldId,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[lFldId:%d][iIdx:%d]}",lFldId,iIdx);

	int iCase = this->GetFldType( lFldId );
	switch( iCase )
	{
		case 2:
		{
			if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&ch, (FLDLEN*)0 ) == -1 )
			{
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
			}
			break;
		}
		default:
		{
			THROW_P1(CFmlException,"GetChar,FiledType[%d] Not Support",iCase);
		}
	}

	return ch;

}

//##ModelId=41902CCB0228
double CFmlBuf::GetFloat(FLDID lFldId, FLDOCC iIdx) throw(CFmlException)
{
	double fRet;
	if(!this->IsFldExist(lFldId,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[lFldId:%d][iIdx:%d]}",lFldId,iIdx);


	int iCase = this->GetFldType( lFldId );

	switch( iCase )
	{
	case 3:
		{
			float fTemp;
			if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&fTemp, (FLDLEN*)0 ) == -1 )
			{
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
			}
			fRet = fTemp;
			break;
		}
	case 4:
		{
			if( Fget( this->m_pFmlBuf, lFldId, iIdx, (char *)&fRet, (FLDLEN*)0 ) == -1 )
			{
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
			}
			break;
		}
	default:
		{
			//try convert to double
			if( CFget( this->m_pFmlBuf, lFldId, iIdx, (char *)&fRet, (FLDLEN*)0, 4) == -1 )
				THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
		}
	}

	return fRet;

}

//##ModelId=41902D400244
long CFmlBuf::GetInt(const char *strFldName, FLDOCC iIdx) throw(CFmlException)
{
	if(!this->IsFldExist(strFldName,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[strFldName:%s][iIdx:%d]}",strFldName,iIdx);

	return 	GetInt(GetFldID(strFldName),iIdx);
}

//##ModelId=41AC2D6F01EC
char * CFmlBuf::GetStr(const char *strFldName, FLDOCC iIdx) throw(CFmlException)
{
	if(!this->IsFldExist(strFldName,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[strFldName:%s][iIdx:%d]}",strFldName,iIdx);
	return GetStr(GetFldID(strFldName),iIdx);
}

//##ModelId=41902D400276
char CFmlBuf::GetChar(const char *strFldName, FLDOCC iIdx) throw(CFmlException)
{
	if(!this->IsFldExist(strFldName,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[strFldName:%s][iIdx:%d]}",strFldName,iIdx);
	return GetChar(GetFldID(strFldName),iIdx);
}

//##ModelId=41902D400294
double CFmlBuf::GetFloat(const char *strFldName, FLDOCC iIdx) throw(CFmlException)
{
	if(!this->IsFldExist(strFldName,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[strFldName:%s][iIdx:%d]}",strFldName,iIdx);
	return GetFloat(GetFldID(strFldName),iIdx);
}

//##ModelId=41C78B37009E
CFmlBuf::CFmlBuf(long lSize)
{

	m_fldid=0;
	m_fldidx=-1;
//	m_iState=0;
//	m_lBufSize=0L;
	m_pFmlBuf=NULL;

	//最要需要开辟4K的缓冲区
	if (lSize<DEFAULT_SIZE)
		m_pFmlBuf = (FBFR *)tpalloc( (char*)FMLTYPE, NULL, DEFAULT_SIZE );
	else
		m_pFmlBuf = (FBFR *)tpalloc( (char*)FMLTYPE, NULL, lSize );
	//判断申请操作是否成功
	if (m_pFmlBuf==NULL)
	{
		THROW_C_P2(CFmlException,tperrno,"[detail:[%d][%s]]",tperrno,tpstrerror(tperrno));
	}
}


//##ModelId=41921273008A
CFmlBuf::CFmlBuf(FBFR * right) throw(CFmlException)
{

	m_fldid=0;
	m_fldidx=-1;
	m_pFmlBuf=NULL;

	if (right==NULL)
	{
		THROW(CFmlException,"Invalidate Address ");
	}

	m_pFmlBuf=right;
}


//##ModelId=41931A8C01D3
int CFmlBuf::DelAllFld(const char *strFldName) throw(CFmlException)
{
	return DelAllFld(GetFldID(strFldName));
}

//##ModelId=41931A8C01DD
int CFmlBuf::DelAllFld(FLDID lFldID) throw(CFmlException)
{
	if ( Fdelall( this->m_pFmlBuf, lFldID ) == -1 )
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	return 0;
}

//##ModelId=41931BE7002D
CFmlBuf& CFmlBuf::operator()(const char *strFldName, FLDOCC iIdx) throw(CFmlException)
{
	return this->operator()(GetFldID(strFldName),iIdx);
}

//##ModelId=41931C77007B
CFmlBuf& CFmlBuf::operator=(const long value) throw(CFmlException)
{
	SetValue(m_fldid, (void *)&value, m_fldidx,0);
	return *this;
}

//##ModelId=419327EB02B7
CFmlBuf& CFmlBuf::operator()(FLDID lFldId, FLDOCC iIdx) throw(CFmlException)
{
	m_fldid=lFldId;
	m_fldidx=iIdx;
	return *this;
}

//##ModelId=4193282002EF
CFmlBuf& CFmlBuf::operator=(const int value) throw(CFmlException)
{
	SetValue(m_fldid, (void *)&value, m_fldidx);
	return *this;
}

//##ModelId=419328400115
CFmlBuf& CFmlBuf::operator=(const char *value) throw(CFmlException)
{
	SetValue(m_fldid, (void *)value, m_fldidx);
	return *this;
}

//##ModelId=419328AE02C2
CFmlBuf& CFmlBuf::operator=(const char value) throw(CFmlException)
{
	SetValue(m_fldid, (void *)&value, m_fldidx);
	return *this;
}

//##ModelId=4194B700038E
void CFmlBuf::SetInt(FLDID lFldId, long iValue, FLDOCC iIdx) throw(CFmlException)
{
	SetValue(lFldId, &iValue, iIdx);
}

//##ModelId=4194B7010096
void CFmlBuf::SetString(FLDID lFldId, string strValue, FLDOCC iIdx) throw(CFmlException)
{
	SetValue(lFldId,(void *)strValue.c_str(),iIdx);
}


//##ModelId=419AF8070268
void CFmlBuf::SetString(const char *  strFldName, string strValue, FLDOCC iIdx) throw(CFmlException)
{
	SetValue(GetFldID(strFldName),(void *)strValue.c_str(),iIdx);
}

//##ModelId=4194B7010190
void CFmlBuf::SetChar(FLDID lFldId, char cValue, FLDOCC iIdx) throw(CFmlException)
{
	SetValue( lFldId, &cValue, iIdx);
}

//##ModelId=4194B7010281
void CFmlBuf::SetFloat(FLDID lFldId, double fvalue, FLDOCC iIdx) throw(CFmlException)
{
	SetValue( lFldId, &fvalue, iIdx);
}

//##ModelId=4194B701037B
void CFmlBuf::SetInt(const char *strFldName, long iValue, FLDOCC iIdx) throw(CFmlException)
{
	SetValue(GetFldID(strFldName), &iValue, iIdx);
}

//##ModelId=4194B702019C
void CFmlBuf::SetChar(const char *strFldName, char cValue, FLDOCC iIdx) throw(CFmlException)
{
	SetChar(GetFldID(strFldName),cValue,iIdx);
}

//##ModelId=4194B70202A0
void CFmlBuf::SetFloat(const char *strFldName, double fvalue, FLDOCC iIdx) throw(CFmlException)
{
	SetValue(GetFldID(strFldName), &fvalue, iIdx);
}

//##ModelId=4199D8440078
FLDID CFmlBuf::GetFldID(const char *strFLDName) throw(CFmlException)
{
	FLDID id=0;
	id=Fldid((char *)strFLDName);
	if (id==BADFLDID)
		THROW_C_P4(CFmlException,Ferror,"[strFLDName:%s][FldID:%d]域不存在或者域名错误{detail:[%d],[%s]}",strFLDName,id,Ferror,Fstrerror(Ferror));
	return id;
}

//##ModelId=419B3E0B0366
char *CFmlBuf::GetFldName(FLDID lFldId)
{
	return Fname(lFldId);
}

//##ModelId=419B530400DB
long CFmlBuf::Append(CFmlBuf& right)
{
	long iUnuseSize = Funused( this->m_pFmlBuf );
	long iUsedRightSize = right.GetUsed();

	if (iUnuseSize < iUsedRightSize )
	{
		//this->ClearFmlValue();??
		if (!this->Resize( iUsedRightSize - iUnuseSize + this->GetBufSize() + SIZE_4K))
			THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	}

	if(Fconcat((Fbfr *)m_pFmlBuf,(Fbfr *)right.GetData()) == -1 )
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));

	return CalBufSize();
}

void * CFmlBuf::GetData()
{
	return (void *)m_pFmlBuf;
}

int CFmlBuf::GetBufType() const
{
	return TUX_BUF_TYPE_FML;
}

//##ModelId=419C766B03A2
long CFmlBuf::GetBufSize() const
{
	return CalBufSize();
}

//##ModelId=41A599A20353
void *  CFmlBuf::GetBufRef()
{
	return (void *)&m_pFmlBuf;
}

//##ModelId=41AB1374024A
bool CFmlBuf::Resize(long newSize)
{

	if( ( this->m_pFmlBuf = (FBFR*)tprealloc( (char*)m_pFmlBuf, newSize ) ) == (FBFR*)NULL )
		return false;
	else
		return true;
}

//##ModelId=41AC109F011E
long CFmlBuf::GetUsed()
{
	return Fused(this->m_pFmlBuf);
}

//##ModelId=41AC10DC00FE
long CFmlBuf::GetUnused()
{
	return Funused(this->m_pFmlBuf);
}

//##ModelId=41AC112E0124
long CFmlBuf::GetIdxUsed()
{
	return Fidxused(this->m_pFmlBuf);
}

//##ModelId=41AC128E013E
int CFmlBuf::GetNext(FLDID *lFldId, FLDOCC *fldidx, void *value, FLDLEN *len) throw(CFmlException)
{
	int iRet;
	iRet= Fnext(this->m_pFmlBuf, lFldId, fldidx, (char *)value, len);
	if (iRet == -1 )
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	return iRet;
}

//##ModelId=41902D400258
char *CFmlBuf::GetStr(FLDID lFldId, FLDOCC iIdx) throw(CFmlException)
{
	char * ptr;
	if(!this->IsFldExist(lFldId,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[lFldId:%d][iIdx:%d]}",lFldId,iIdx);

	ptr= Fvals(this->m_pFmlBuf, lFldId, iIdx) ;
	if (!ptr)
		THROW_C_P2(CFmlException,Ferror,"[detail:[%d][%s]]",Ferror,Fstrerror(Ferror));
	return ptr;
}


//##ModelId=41AC2D71011D
const string CFmlBuf::GetString(const char * strFldName, FLDOCC iIdx) throw(CFmlException)
{
	if(!this->IsFldExist(strFldName,iIdx))
		THROW_C_P2(CFmlException,LIMW_FLD_NOT_EXIST,"Value Not Found!{detail:[strFldName:%s][iIdx:%d]}",strFldName,iIdx);
	return GetString(GetFldID(strFldName),iIdx);
}

//##ModelId=41AC5EFC0088
FLDOCC CFmlBuf::CountAll()
{

	return 	Fnum(this->m_pFmlBuf);
}

//##ModelId=41AC60740238
bool CFmlBuf::IsEmpty()
{
	return !Fielded(this->m_pFmlBuf);
}

int CFmlBuf::printFmlBufLogger(Logger& logfile)
{
  //输出表格头
  LOG_TRACE_P3(logfile, "\t@! %-20s %-10s %6s", "FLDNAME","FLDID", "OCC");
  LOG_TRACE_P2(logfile, " %-8s %s\n", "TYPE", "VALUE");
  LOG_TRACE(logfile, "\t@! ==================== ========== ====== ======== ======\n");
  FLDID iFieldId = FIRSTFLDID;
  FLDOCC occ = 0;
  while (GetNext(&iFieldId, &occ, NULL, NULL)>0)
  {
    //输出前缀空格
    LOG_TRACE(logfile, "\t@! ");
    //输出字段名称和ID
   // string strFldName = string(Fname(iFieldId));
    LOG_TRACE_P2(logfile,"%-20s %-10u ", Fname(iFieldId), iFieldId);
    //输出发生次数
    LOG_TRACE_P1(logfile,"%6u ", occ);
    //输出字段类型
    int iTypeId = GetFldType(iFieldId);
    string strType ;
    switch(iTypeId)
    {
      case  FLD_CHAR	:   strType = "char";   break;
      case  FLD_SHORT	:   strType = "short";  break;
      case  FLD_LONG	:   strType = "long";   break;
      case  FLD_FLOAT	:   strType = "float";  break;
      case  FLD_DOUBLE:   strType = "double"; break;
      case  FLD_STRING:   strType = "string"; break;
      case  FLD_CARRAY:   strType = "carray"; break;
      #ifdef FML32
      case  FLD_PTR:      strType = "ptr";    break;
      case  FLD_FML32:    strType = "fml32";  break;
      case  FLD_VIEW32:   strType = "view32"; break;
      #endif
    };
    LOG_TRACE_P1(logfile,"%-8s ",strType.c_str());
    //输出字段值
    switch(iTypeId)
    {
      case  FLD_CHAR	:
        {
           char charValue ;
           charValue = GetChar(iFieldId,occ);
          LOG_TRACE_P1(logfile,"'%c'",charValue);
        };
        break;
      case  FLD_SHORT	:
        {
          short shortValue = GetInt(iFieldId,occ);
          LOG_TRACE_P1(logfile,"%d",shortValue);
        };
        break;
      case  FLD_LONG	:
        {
          long longValue = GetInt(iFieldId,occ);
          LOG_TRACE_P1(logfile,"%d",longValue);
        };
        break;
      case  FLD_FLOAT	:
        {
          float floatValue = GetFloat(iFieldId,occ);
          LOG_TRACE_P1(logfile,"%f",floatValue);
        };
        break;
      case  FLD_DOUBLE:
        {
           float doubleValue = GetFloat(iFieldId,occ);
          LOG_TRACE_P1(logfile,"%f",doubleValue);
        };
        break;
      case  FLD_STRING:
        {
           string strValue = GetStr(iFieldId,occ);
           LOG_TRACE_P1(logfile,"\"%s\"",strValue.c_str());
        };
        break;
      //TODO: parse carray,prt,fml32,view32 data from fml structure
      case FLD_CARRAY:
        {
        }
        break;
      #ifdef FML32
      case FLD_PTR :
        {
        }
        break;
      case FLD_FML32:
        {
        }
        break;
      case FLD_VIEW32:
        {
        }
        break;
      #endif
    };
    //输出换行
    LOG_TRACE(logfile,"\n");
  }
  return 0;
}

//##ModelId=41D163700162
int CFmlBuf::printFmlBuffer(FILE *out)
{
  if (out == NULL) return -1;
  //输出表格头
  fprintf(out, "\t@! %-20s %-10s %6s %-8s %s\n", "FLDNAME","FLDID", "OCC","TYPE", "VALUE");
  fprintf(out, "\t@! ==================== ========== ====== ======== ======\n");
  FLDID iFieldId = FIRSTFLDID;
  FLDOCC occ = 0;
  while (GetNext(&iFieldId, &occ, NULL, NULL)>0)
  {
    //输出前缀空格
    fprintf(out, "\t@! ");
    //输出字段名称和ID
   // string strFldName = string(Fname(iFieldId));
    fprintf(out,"%-20s %-10u ", Fname(iFieldId), iFieldId);
    //输出发生次数
    fprintf(out,"%6u ", occ);
    //输出字段类型
    int iTypeId = GetFldType(iFieldId);
    string strType ;
    switch(iTypeId)
    {
      case  FLD_CHAR	:   strType = "char";   break;
      case  FLD_SHORT	:   strType = "short";  break;
      case  FLD_LONG	:   strType = "long";   break;
      case  FLD_FLOAT	:   strType = "float";  break;
      case  FLD_DOUBLE:   strType = "double"; break;
      case  FLD_STRING:   strType = "string"; break;
      case  FLD_CARRAY:   strType = "carray"; break;
      #ifdef FML32
      case  FLD_PTR:      strType = "ptr";    break;
      case  FLD_FML32:    strType = "fml32";  break;
      case  FLD_VIEW32:   strType = "view32"; break;
      #endif
    };
    fprintf(out,"%-8s ",strType.c_str());
    //输出字段值
    switch(iTypeId)
    {
      case  FLD_CHAR	:
        {
           char charValue ;
           charValue = GetChar(iFieldId,occ);
          fprintf(out,"'%c'",charValue);
        };
        break;
      case  FLD_SHORT	:
        {
          short shortValue = GetInt(iFieldId,occ);
          fprintf(out,"%d",shortValue);
        };
        break;
      case  FLD_LONG	:
        {
          long longValue = GetInt(iFieldId,occ);
          fprintf(out,"%d",longValue);
        };
        break;
      case  FLD_FLOAT	:
        {
          float floatValue = GetFloat(iFieldId,occ);
          fprintf(out,"%f",floatValue);
        };
        break;
      case  FLD_DOUBLE:
        {
           float doubleValue = GetFloat(iFieldId,occ);
          fprintf(out,"%f",doubleValue);
        };
        break;
      case  FLD_STRING:
        {
           string strValue = GetStr(iFieldId,occ);
           fprintf(out,"\"%s\"",strValue.c_str());
        };
        break;
      //TODO: parse carray,prt,fml32,view32 data from fml structure
      case FLD_CARRAY:
        {
        }
        break;
      #ifdef FML32
      case FLD_PTR :
        {
        }
        break;
      case FLD_FML32:
        {
        }
        break;
      case FLD_VIEW32:
        {
        }
        break;
      #endif
    };
    //输出换行
    fprintf(out,"\n");
  }
  return 0;
}

//##ModelId=41D163700162
const string CFmlBuf::ToString()
{
	ostringstream oss;

  //输出表格头
  oss << "\t@! FLDNAME              FLDID         OCC TYPE     VALUE " << endl;
  oss << "\t@! ==================== ========== ====== ======== ======" << endl;
  FLDID iFieldId = FIRSTFLDID;
  FLDOCC occ = 0;
  int i = 0;
  while (GetNext(&iFieldId, &occ, NULL, NULL)>0)
  {
    //输出前缀空格,输出字段名称和ID,输出发生次数
    oss << "\t@! ";
    oss << setw(21) << setfill(' ') << fixed << left << Fname(iFieldId)==NULL?"?":Fname(iFieldId);
    oss << setw(11) << setfill(' ') << fixed << left << iFieldId ;
    oss << setw(6) << setfill(' ') << fixed << right << occ << ' ';
    int iTypeId = GetFldType(iFieldId);
    string strType ;
    switch(iTypeId)
    {
      case  FLD_CHAR	:   strType = "char";   break;
      case  FLD_SHORT	:   strType = "short";  break;
      case  FLD_LONG	:   strType = "long";   break;
      case  FLD_FLOAT	:   strType = "float";  break;
      case  FLD_DOUBLE:   strType = "double"; break;
      case  FLD_STRING:   strType = "string"; break;
      case  FLD_CARRAY:   strType = "carray"; break;
      #ifdef FML32
      case  FLD_PTR:      strType = "ptr";    break;
      case  FLD_FML32:    strType = "fml32";  break;
      case  FLD_VIEW32:   strType = "view32"; break;
      #endif
    };
    //输出字段类型
    oss << setw(9) << setfill(' ') << fixed << left << strType.c_str() ;
    //输出字段值
    switch(iTypeId)
    {
      case  FLD_CHAR	:
        {
           char charValue = GetChar(iFieldId,occ);
			     oss << charValue ;
        };
        break;
      case  FLD_SHORT	:
        {
          short shortValue = GetInt(iFieldId,occ);
  		    oss << shortValue ;
        };
        break;
      case  FLD_LONG	:
        {
          long longValue = GetInt(iFieldId,occ);
  		    oss << longValue ;
        };
        break;
      case  FLD_FLOAT	:
        {
          float floatValue = GetFloat(iFieldId,occ);
  		    oss << floatValue ;
        };
        break;
      case  FLD_DOUBLE:
        {
           float doubleValue = GetFloat(iFieldId,occ);
  		    oss << doubleValue ;
        };
        break;
      case  FLD_STRING:
        {
           string strValue = GetStr(iFieldId,occ);
  		    oss << strValue.c_str() ;
        };
        break;
      //TODO: parse carray,prt,fml32,view32 data from fml structure
      case FLD_CARRAY:
        {
        }
        break;
      #ifdef FML32
      case FLD_PTR :
        {
        }
        break;
      case FLD_FML32:
        {
        }
        break;
      case FLD_VIEW32:
        {
        }
        break;
      #endif
    };
    //输出换行
  	oss << endl << flush;
	i++;
	if(10000 < i) break;
  	if(oss.str().length() > SIZE_4M*25) break;
  }
  return oss.str();
}

//##ModelId=41D163700162
const string CFmlBuf::ToString1()
{
	int nBufSize = this->CalBufSize()*4;
	nBufSize = nBufSize>100000?nBufSize:100000;
	char *szFml = (char *)malloc(nBufSize);

  //输出表格头
  sprintf(szFml, "\t@! %-20s %-10s %6s %-8 CFmlBuf::ToString1()
{
	int nBufSize = this->CalBufSize()*4;
	nBufSize = nBufSize>100000?nBufSize:100000;
	char *szFml = (char *)malloc(nBufSize);

  //杈撳嚭琛ㄦ牸澶