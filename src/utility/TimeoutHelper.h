/*
 * TimeoutHelper.h
 *
 *  Created on: 04.09.2018
 *      Author: A301620
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_UTILITY_TIMEOUTHELPER_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_UTILITY_TIMEOUTHELPER_H_

#include <Arduino.h>

class TimeoutHelper {
public:
	TimeoutHelper(const char* owner, long &timeoutMs);
	virtual ~TimeoutHelper(void);
	bool checkTimeout(void);
	long runtimeMs(void);
private:
	const char* owner;
	unsigned long ts;
	long &timeoutMs;
	long timeoutMsStart;
};

#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_UTILITY_TIMEOUTHELPER_H_ */
