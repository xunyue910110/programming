#ifndef _FLOWLINE_H_
#define _FLOWLINE_H_
#include "config-all.h"
#include "log4cpp/Logger.h"
#include "flowline/FlowLineException.h"
#include <string>

USING_NAMESPACE(std);
using namespace log4cpp;

namespace flowline {
	extern Logger& g_logger;
}

#define FL_TRACE(msg) LOG_TRACE(flowline::g_logger, msg)
#define FL_TRACE_P1(msg, p1) LOG_TRACE_P1(flowline::g_logger, msg, p1)
#define FL_TRACE_P2(msg, p1, p2) LOG_TRACE_P2(flowline::g_logger, msg, p1, p2)
#define FL_TRACE_P3(msg, p1, p2, p3) LOG_TRACE_P3(flowline::g_logger, msg, p1, p2, p3)
#define FL_TRACE_P4(msg, p1, p2, p3, p4) LOG_TRACE_P4(flowline::g_logger, msg, p1, p2, p3, p4)

#define FL_DEBUG(msg) LOG_DEBUG(flowline::g_logger, msg)
#define FL_DEBUG_P1(msg, p1) LOG_DEBUG_P1(flowline::g_logger, msg, p1)
#define FL_DEBUG_P2(msg, p1, p2) LOG_DEBUG_P2(flowline::g_logger, msg, p1, p2)
#define FL_DEBUG_P3(msg, p1, p2, p3) LOG_DEBUG_P3(flowline::g_logger, msg, p1, p2, p3)
#define FL_DEBUG_P4(msg, p1, p2, p3, p4) LOG_DEBUG_P4(flowline::g_logger, msg, p1, p2, p3, p4)
#endif //_FLOWLINE_H_
