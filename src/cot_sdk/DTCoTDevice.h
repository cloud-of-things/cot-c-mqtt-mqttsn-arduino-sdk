/**
 * Wapper for definition and initialization of the Boards and the Cloud Connection.
 */
/**
 * @file DTCoTDevice.h
 * @description Basic class for all device classes
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 *
 */

#ifndef Device_h_
#define Device_h_

#include "DTCotSdkBase.h"
#include "DTCotSdkTypes.h"
#include "base-classes/DTCotOperation.h"
#include "cot_sdk/DTCotSdkMqtt.h"
#include "utility/DTCoTDebugOutput.h"

namespace DTCoT {

class CoTChildDevice {
public:
	CoTChildDevice(CotSdkMqtt* s = NULL, String moId = "", String childName = "") {
		sdk = s;
		myMoId = moId;
		myChildName = childName;
	};

	virtual ~CoTChildDevice() { };

	void updateDevice(const String hwModel, const String hwVersion
			, const String swVersion, const String imei, const String iccid
			, const String imsi, int maxlen) {
		sdk->getSmRProc()->updateDevice(myMoId, hwModel, hwVersion, swVersion, imei, iccid, imsi, maxlen);
	};

	void sendMeasurement(const String type, const String model,
			const String measurement, const String unit, const float value,
			const time_t timeStamp) {
		sdk->getSmRProc()->sendMeasurement(myMoId, type, model, measurement, unit, value, timeStamp);
	};

	void sendAlarm(const String text, const String type,
			const AlarmSeverity severity, const AlarmStatus status) {
		sdk->getSmRProc()->sendAlarm(myMoId, text, type, severity, status);
	};

	void registerFirmwareUpdate(OperationsHandler &) {
		//empty
	};

	void registerConfigurationUpdate(OperationsHandler &) {
		//empty
	};

	void registerOperation(DTCotOperationCallBack callBackFunction, int operationId, String cmd = "", bool ignoreCmdCase = true) {
		sdk->registerOperation(callBackFunction, operationId, cmd, ignoreCmdCase);
	}

	int executeOperations(int , bool ) { /*empty*/ return 0; };

	bool isValid() {
		return (myMoId.length() > 0);
	}

	String getMoId() {
		return myMoId;
	}

	String getName() {
		return myChildName;
	}

private:
	CotSdkMqtt* sdk;
	String myMoId;
	String myChildName;
};


class CoTDevice {
public:

    /**
     * The default constructor of this device class.
     */
	CoTDevice() { sdk = NULL; };

    /**
     * The default destructor of this device class.
     */
	virtual ~CoTDevice() { delete sdk; };

    /**
     * Setup the current device for the desired SDK.
     * @param s The desired SDK.
     * @return true if the setup was successful.
     */
	bool setup(CotSdkBase* s, long timeoutMs = 600000) {
		sdk = s;
		return sdk->init(timeoutMs);
	};

    /**
     * Update the cloud attributes of the current device.
     * @param hwModel The hardware model.
     * @param hwVersion The hardware version.
     * @param swVersion The software version.
     * @param imei The device IMEI.
     * @param iccid The device ICCID.
     * @param imsi The device IMSI.
     * @param maxlen The maximal length of all attributes, to avoid message buffer overflow.
     *    See /cot_sdk/DTCotSdkBase.h for default values of this method.
     */
	bool updateDevice(const String hwModel, const String hwVersion
			, const String swVersion, const String imei, const String iccid
			, const String imsi, int maxlen) {
		return sdk->updateDevice(hwModel, hwVersion, swVersion, imei, iccid, imsi, maxlen);
	};

    /**
     * Send a measurement for the current device into the cloud.
     * @param type The measurement type, some string, e.g. "ty".
     * @param text The measurement text, some string, e.g. "mod".
     * @param measurement The measurement symbol, e.g. "t".
     * @param unit The measurement unit, e.g. "C".
     * @param value The measurement value.
     * @param timeStamp The measurement timestamp with current time, e.g. now()
     *   See Time.h
     */
	bool sendMeasurement(const String type, const String model,
			const String measurement, const String unit, const float value,
			const time_t timeStamp) {
		return sdk->sendMeasurement(type, model, measurement, unit, value, timeStamp);
	};

    /**
     * Send an alarm for the current device into the cloud.
     * @param text The alarm text, some string.
     * @param type The alarm type, some string.
     * @param severity The alarm severity as enum, e.g. AlarmSeverity::CRITICAL
     * @param status The alarm status as enum, e.g. AlarmStatus::ACTIVE
     */
	bool sendAlarm(const String text, const String type,
			const AlarmSeverity severity, const AlarmStatus status) {
		return sdk->sendAlarm(text, type, severity, status);
	};

    /**
     * Register operation handler for the firmware update.
     * Not implemented yet.
     * @param operationsHandler The operation handler.
     */
	bool registerFirmwareUpdate(OperationsHandler &operationsHandler) {
		return sdk->registerFirmwareUpdate(operationsHandler);
	};

    /**
     * Register operation handler for the configuration update.
     * Not implemented yet.
     * @param operationsHandler The operation handler.
     */
	bool registerConfigurationUpdate(OperationsHandler &operationsHandler) {
		return sdk->registerConfigurationUpdate(operationsHandler);
	};

    /**
     * Register a specific operation.
     * @param Callback function to be executed during this operation.
     * @param operationId The operation ID in the SmartREST template.
     *    One of: #define DTCOTOPERATIONID_C8Y_COMMAND    201
     *       	  #define DTCOTOPERATIONID_C8Y_RESTART    202
     *       	  #define DTCOTOPERATIONID_C8Y_FIRMWARE   203
     *    See in /base-classes/DTCotOperation.h
     * @param cmd The command to be inserted into cloud Shell, for executing
     * 		the corresponding operation callback function, e.g. "led on".
     */
	bool registerOperation(DTCotOperationCallBack callBackFunction, int operationId, String cmd, bool ignoreCmdCase = true) {
		return sdk->registerOperation(callBackFunction, operationId, cmd, ignoreCmdCase);
	}

	/**
     * Execute registered operations.
     * @param timeout Timeout for reading subscription, in millisec.
     * @param trigger True if the operations must be triggered in the cloud, false otherwise.
     * @return Counter how often the operations were executed in this function.
     */
	int executeOperations(int timeout = 100, bool trigger = false) {
		return sdk->executeOperations(timeout, trigger);
	};

    /**
     * Get or create a child device for current master device.
     * First we search in the cloud for kind device with such name for parent device.
     * If found, its ManagedObject ID will be used.
     * If not found, it will be created and assigned to the parent device.
     * @param childDeviceName Name of the possibly available child device.
     * @return Object with an child device. If this object is invalid, managed object ID is void.
     */
	CoTChildDevice getOrCreateChildDevice(String childDeviceName)
	{
		String childMoId = sdk->getOrCreateChildDevice(childDeviceName);

		if(childMoId.length() > 0) {
			return CoTChildDevice((CotSdkMqtt*)sdk, childMoId, childDeviceName);
		}
		else {
			return CoTChildDevice((CotSdkMqtt*)sdk, "", "");
		}
	};

private:
	CotSdkBase* sdk;
};

}

#endif
