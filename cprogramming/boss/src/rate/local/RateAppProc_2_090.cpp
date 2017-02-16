#include "RateApp.h"
#include "Procedure.h"

#include "LocalProc_2_090.h"
#include "AcctDiscntProc.h"
#include "BillProc.h"
#include "GprsPriceProc_Unicom.h"
#include "SpPriceProc_Unicom.h"
#include "PayRelationProc.h"
#include "PriceProc.h"
#include "RentProc.h"
#include "TpProc.h"
#include "UserDiscntProc.h"
#include "SumBillProc.h"
#include "AcctAttribProc.h"
#include "AcctBillProc.h"
#include "AcctTpProc.h"
#include "AccountAttribProc.h"
#include "UserGrpDiscntProc.h"

bool RateApp::installProc()
{
  
  INSTALL_PROCEDURE_INSTANCE(RateCdrAttribInit);
  INSTALL_PROCEDURE_INSTANCE(RateCdrAttribEnd);
  INSTALL_PROCEDURE_INSTANCE(SetOriginFeeFlag);
  INSTALL_PROCEDURE_INSTANCE(SpecialBillProc);
  INSTALL_PROCEDURE_INSTANCE(LocalizeGsmFreeFlagProc);
  INSTALL_PROCEDURE_INSTANCE(IsNoNeedTPSpecialProc);
  INSTALL_PROCEDURE_INSTANCE(AcctDiscntProc);
  INSTALL_PROCEDURE_INSTANCE(GetBillProc);
  INSTALL_PROCEDURE_INSTANCE(UpdateBillProc);
  INSTALL_PROCEDURE_INSTANCE(RollBackBillProc);
  INSTALL_PROCEDURE_INSTANCE(GprsPriceProc_Unicom);
  INSTALL_PROCEDURE_INSTANCE(PayRelationProc);
  INSTALL_PROCEDURE_INSTANCE(PriceProc);
  INSTALL_PROCEDURE_INSTANCE(RentProc);
  INSTALL_PROCEDURE_INSTANCE(TpProc);
  INSTALL_PROCEDURE_INSTANCE(UserDiscntProc);    
  INSTALL_PROCEDURE_INSTANCE(SpPriceProc_Unicom);  
  INSTALL_PROCEDURE_INSTANCE(SumBillProc);
  INSTALL_PROCEDURE_INSTANCE(SumBillProc_Total);
  INSTALL_PROCEDURE_INSTANCE(SumBillProc_OT);
  INSTALL_PROCEDURE_INSTANCE(ErrorProc);
  INSTALL_PROCEDURE_INSTANCE(NullProc);
  
  INSTALL_PROCEDURE_INSTANCE(AcctAttribProc);
  INSTALL_PROCEDURE_INSTANCE(UserBillToDetailBillProc);
  INSTALL_PROCEDURE_INSTANCE(AcctBillToDetailBillProc);
  INSTALL_PROCEDURE_INSTANCE(UpdateAccountBillProc);
  INSTALL_PROCEDURE_INSTANCE(UpdateUserBillProc);
  INSTALL_PROCEDURE_INSTANCE(UserAcctTpProc);
  INSTALL_PROCEDURE_INSTANCE(UserGrpTpProc);
  INSTALL_PROCEDURE_INSTANCE(AccountTpProc);
  INSTALL_PROCEDURE_INSTANCE(UserGrpDiscntProc);
  INSTALL_PROCEDURE_INSTANCE(AccountAttribProc);
  return true;
}

