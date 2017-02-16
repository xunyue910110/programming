#ifndef ACCTBILLPROC_H_HEADER_INCLUDED_B59A5243
#define ACCTBILLPROC_H_HEADER_INCLUDED_B59A5243

#include "Procedure.h"
#include "RateGlobalInfo.h"
#include "ComBill.h"
#include "AcctBillMgr.h"

//用户账单->内部明细账单过程
//##ModelId=4A5E9BA8033F
class UserBillToDetailBillProc : public Procedure
{
	public:
	//##ModelId=4A5E9E9D036F
	int	execute();

	void  init();


	private:
	AcctBillMgr m_acctBillMgr;
	ComBill *m_pComBill;

};


//账户账单->内部账户明细账单
//##ModelId=4A5E9BA8033F
class AcctBillToDetailBillProc : public Procedure
{
	public:
	//##ModelId=4A5E9E9D036F
	int	execute();

	void  init();


	private:
	AcctBillMgr m_acctBillMgr;
	ComBill *m_pComBill;



};

//内部明细账单到账户账单
//##ModelId=4A5E9BA8033F
class UpdateAccountBillProc : public Procedure
{
	public:
	//##ModelId=4A5E9E9D036F
	int	execute();

	void  init();


	private:
	AcctBillMgr m_acctBillMgr;
	ComBill *m_pComBill;



};

//##ModelId=4A5E9BA8033F
class updateAccountBill : public Procedure
{
	public:
	//##ModelId=4A5E9E9D036F
	int	execute();

	void  init();


	private:
	AcctBillMgr m_acctBillMgr;
	ComBill *m_pComBill;



};

//##ModelId=4A5E9BA8033F
class UpdateUserBillProc : public Procedure
{
	public:
	//##ModelId=4A5E9E9D036F
	int	execute();

	void  init();


	private:
	AcctBillMgr m_acctBillMgr;
	ComBill *m_pComBill;



};


#endif /* ACCTBILLPROC_H_HEADER_INCLUDED_B59A5243 */
