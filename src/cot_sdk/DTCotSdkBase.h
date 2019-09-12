/**
 * @description Header file for central class of SDG for interaction with Telekom Cloud of Things
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_SDK_BASE_H_
#define DT_COT_SDK_BASE_H_

#include "DTCoTPrivate.h"
#include "DTCoTExtensionIface.h"
#include "base-classes/DTCoTDeviceBase.h"
#include "base-classes/DTCotOperation.h"
#include "utility/DTCoTDebugOutput.h"
#include "DTCotSmartRestProcessor.h"

#define DT_COT_SDK_UPDATE_DEFAULT_HW_MODEL            "hwM"
#define DT_COT_SDK_UPDATE_DEFAULT_HW_REVISION         "hwV"
#define DT_COT_SDK_UPDATE_DEFAULT_SW_VERSION          DT_COT_ARDUINO_LIBRARY_VERSION
#define DT_COT_SDK_UPDATE_DEFAULT_IMEI                "ime"
#define DT_COT_SDK_UPDATE_DEFAULT_ICCID               "icc"
#define DT_COT_SDK_UPDATE_DEFAULT_IMSI                "ims"
#define DT_COT_SDK_UPDATE_DEFAULT_MAXLEN              137

namespace DTCoT {

/**
 * High-level object for accessing the service.
 * TODO
 */
class CotSdkBase {
    // == Functions ==
public:

    virtual ~CotSdkBase() {};

    virtual bool init(long &timeoutMs) = 0;

    const char* getSdkVersion() { return DT_COT_SDK_UPDATE_DEFAULT_SW_VERSION; };

    virtual bool updateDevice(const String hwModel =
            DT_COT_SDK_UPDATE_DEFAULT_HW_MODEL, const String hwVersion =
            DT_COT_SDK_UPDATE_DEFAULT_HW_REVISION, const String swVersion =
            DT_COT_SDK_UPDATE_DEFAULT_SW_VERSION, const String imei =
            DT_COT_SDK_UPDATE_DEFAULT_IMEI, const String iccid =
            DT_COT_SDK_UPDATE_DEFAULT_ICCID, const String imsi =
            DT_COT_SDK_UPDATE_DEFAULT_IMSI, int maxlen =
            DT_COT_SDK_UPDATE_DEFAULT_MAXLEN) = 0;

    virtual bool sendMeasurement(const String type, const String model,
            const String measurement, const String unit, const float value,
            const time_t timeStamp) = 0;

    virtual bool sendAlarm(const String text, const String type,
            const AlarmSeverity severity, const AlarmStatus status) = 0;

    virtual bool registerFirmwareUpdate(
            OperationsHandler &operationsHandler) = 0;

    virtual bool registerConfigurationUpdate(
            OperationsHandler &operationsHandler) = 0;

    virtual bool registerOperation(DTCotOperationCallBack callBackFunction,
            int operationId, String cmd, bool ignoreCmdCase) = 0;

    virtual int executeOperations(int timeout = 100, bool trigger = false) = 0;

    virtual String getOrCreateChildDevice(String childDeviceName) = 0;

};

} /* namespace DTCoT */

#endif /* DT_COT_SDK_BASE_H_ */

