// Program code written by Steven De Toni ACBC 11

#include "anyobj.hpp"


// Set to virtual so that objects can call there own destructors
// without becoming confused.
namespace archy {
	any_object::~any_object () {};
}
