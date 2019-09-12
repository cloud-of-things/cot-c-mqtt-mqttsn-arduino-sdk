/**
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#include "DTCotSdkTypes.h"
#include "utility/DTCoTDebugOutput.h"

using namespace DTCoT;

const String AlarmHelper::alarmSeverityToString(AlarmSeverity alarmSeverity) {
    return String(ENUM_to_string(alarmSeverity).c_str());
}

const String AlarmHelper::alarmStatusToString(AlarmStatus alarmStatus) {
    return String(ENUM_to_string(alarmStatus).c_str());
}

const String AlarmHelper::operationStatusToString(OperationStatus operationStatus) {
    return String(ENUM_to_string(operationStatus).c_str());
}

AlarmSeverity AlarmHelper::stringToAlarmSeverity(const String alarmSeverity) {
    int i = 0;
    while(true) {
        AlarmSeverity as = (AlarmSeverity)i++;
        String str = alarmSeverityToString(as);
        if (str.equals(alarmSeverity) || str.equals("UNKNOWN")) {
            return as;
        }
    }
}

AlarmStatus AlarmHelper::stringToAlarmStatus(const String alarmStatus) {
    int i = 0;
    while(true) {
        AlarmStatus as = (AlarmStatus)i++;
        String str = alarmStatusToString(as);
        if (str.equals(alarmStatus) || str.equals("UNKNOWN")) {
            return as;
        }
    }
}

OperationStatus AlarmHelper::stringToOperationStatus(const String operationStatus) {
    int i = 0;
    while(true) {
        OperationStatus os = (OperationStatus)i++;
        String str = operationStatusToString(os);
        if (str.equals(operationStatus) || str.equals("UNKNOWN")) {
            return os;
        }
    }
}
