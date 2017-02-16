#include "Checker.h"
#include "flowline/adapter/CheckerAdapter.h"
#include "flowline/Context.h"
namespace flowline
{

Checker::Checker()
{
}

Checker::~Checker()
{
}

bool Checker::check(Context& ctx) {
	//	检查过一遍, 就不检查第二次
	if(ctx.getFlowLineDao().hasPassed(ctx.getCycleLogId(), getId()))
		return true;
	bool passed = dynamic_cast<CheckerAdapter*>(getAdapter())->check(ctx);
	if(passed)
		ctx.getFlowLineDao().logPassed(ctx.getCycleLogId(), getId());
	return passed;
}

};
