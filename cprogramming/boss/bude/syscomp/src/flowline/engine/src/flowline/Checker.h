#ifndef _CHECKER_H_
#define _CHECKER_H_

#include "flowline/Element.h"
#include "flowline/Adaptable.h"

namespace flowline
{
class Context;
class Checker : public flowline::Adaptable
{
public:
	Checker();
	bool check(Context& ctx);
	virtual ~Checker();
};

};

#endif //_CHECKER_H_
