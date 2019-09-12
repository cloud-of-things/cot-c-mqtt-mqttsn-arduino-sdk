/**
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_CotSDKtypes_h_ 
#define DT_CotSDKtypes_h_ 

#include <Arduino.h>
#include "dt-arduino-iot-agent.h"
#include <string.h> 

/*FIXME ak Put this to more tuino related header file*/
#if defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_1284P_STANDARD)
#include <ArduinoSTL.h>
#endif /*ARDUINO_ARCH_AVR*/

#include <vector>

namespace DTCoT {

#define SUPPRESS_WARNING(a) (void)a

// from 'https://stackoverflow.com/questions/21456262/enum-to-string-in-c11'
#define MAKE_ENUM(name, ...) enum class name { __VA_ARGS__}; \
static std::vector<std::string> Enum_##name##_init(){\
    const std::string content = #__VA_ARGS__; \
    std::vector<std::string> str;\
    size_t len = content.length();\
    std::string temp;\
    for(size_t i = 0; i < len; i ++) {\
          if(isspace(content[i])) continue;\
          else \
            if(content[i] == ',') {\
                str.push_back(temp);\
                temp = std::string();}\
            else \
                temp += content[i];\
    }\
    str.push_back(temp);\
    return str;}\
static const std::vector<std::string> Enum_##name##_str_vec = Enum_##name##_init();\
static inline std::string ENUM_to_string(name val){\
    return Enum_##name##_str_vec[static_cast<size_t>(val)];\
}\

MAKE_ENUM(AlarmSeverity, CRITICAL, MAJOR, MINOR, WARNING, UNKNOWN);

MAKE_ENUM(AlarmStatus, ACTIVE, ACKNOWLEDGED, CLEARED, UNKNOWN);

MAKE_ENUM(OperationStatus, SUCCESSFUL, FAILED, EXECUTING, PENDING, UNKNOWN);

class AlarmHelper {
public:
    static const String alarmSeverityToString(AlarmSeverity alarmSeverity);
    static const String alarmStatusToString(AlarmStatus alarmStatus);
    static const String operationStatusToString(OperationStatus operationStatus);
    static AlarmSeverity stringToAlarmSeverity(const String alarmSeverity);
    static AlarmStatus stringToAlarmStatus(const String alarmStatus);
    static OperationStatus stringToOperationStatus(
            const String operationStatus);
};

class OperationsHandler {

};

}
;
/* namespace DTCoT */

#endif /* DT_CotSDKtypes_h_ */
