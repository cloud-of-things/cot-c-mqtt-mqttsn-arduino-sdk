/*
 * TimeoutHelper.cpp
 *
 *  Created on: 04.09.2018
 *      Author: A301620
 */

#include "TimeoutHelper.h"
#include "DTCoTDebugOutput.h"

TimeoutHelper::TimeoutHelper(const char* owner, long &timeoutMs)
: owner(owner), ts(millis()), timeoutMs(timeoutMs), timeoutMsStart(timeoutMs)
{
	DT_DEBUG_PRINTLN_DEBUG("TimeoutHelper::TimeoutHelper(%s) in<%ld>ms...", owner, timeoutMsStart);
}

TimeoutHelper::~TimeoutHelper(void) {
	if (timeoutMs <= 0L) {
		DT_DEBUG_PRINTLN_DEBUG("TimeoutHelper::~TimeoutHelper(%s) in<%ld>ms out<%ld>ms rt<%ld>ms -> TIMEOUT",
				owner, timeoutMsStart, timeoutMs, runtimeMs());
	} else {
		DT_DEBUG_PRINTLN_DEBUG("TimeoutHelper::~TimeoutHelper(%s) in<%ld>ms out<%ld>ms rt<%ld>ms -> OK",
				owner, timeoutMsStart, timeoutMs, runtimeMs());
	}
}

bool TimeoutHelper::checkTimeout(void) {
	unsigned long ats = millis();
	timeoutMs -= (long)(ats - ts);
	ts = ats;
	return timeoutMs <= 0L;
}

long TimeoutHelper::runtimeMs(void) {
	return timeoutMsStart - timeoutMs;
}

