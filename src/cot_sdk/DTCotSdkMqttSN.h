/**
 * @description Header file for central class of SDG for interaction with Telekom Cloud of Things
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_CotSDKmqttSN_h_
#define DT_CotSDKmqttSN_h_

#ifdef ARDUINO_ARCH_AVR
/*TODO Install the library ArduinoSTL if the compiler cannot find it.*/
/*FIXME ak Put this to more tuino related header file*/
#include <ArduinoSTL.h>
// #include "devices/DTCotDeviceGimasi.h"
#else
#include <vector>
#endif /*ARDUINO_ARCH_AVR*/

#include "protocols/DTCoTMqttSn.h"
#include "cot_sdk/DTCotSdkBase.h"
#include "cot_sdk/DTCotSdkTypes.h"
#include "protocols/DTCoTMqttSn-config.h"
#include "devices/DTCoTDeviceGPRS.h"




namespace DTCoT {

/**
 * High-level object for accessing the service.
 */
class CotSdkMqttSN : public CotSdkBase, CotOperationsEngineAdapter {
	/* == Functions == */
public:
	/**
	 * Instantiate the CoT system if it wasn't registered in the Cloud yet.
	 * This will prepare the system using the given device and configuration parameters.
	 */
	CotSdkMqttSN(CoTDeviceGPRS& device, String imsi, const char* password)
	{
		mqttsnConfig = new CoTConfigCommunicationMQTTSN(password);

		mqttsn = new DTCoTMqttSn(
					  mqttsnConfig->getServerIP()
					, mqttsnConfig->getServerPort()
					, imsi.c_str()
					, mqttsnConfig->getPassword()
					, device
				);
	};


	~CotSdkMqttSN()
	{};

	bool init(long &timeoutMs);

	bool updateDevice(const String hwModel =
            DT_COT_SDK_UPDATE_DEFAULT_HW_MODEL, const String hwVersion =
            DT_COT_SDK_UPDATE_DEFAULT_HW_REVISION, const String swVersion =
            DT_COT_SDK_UPDATE_DEFAULT_SW_VERSION, const String imei =
            DT_COT_SDK_UPDATE_DEFAULT_IMEI, const String iccid =
            DT_COT_SDK_UPDATE_DEFAULT_ICCID, const String imsi =
            DT_COT_SDK_UPDATE_DEFAULT_IMSI, int maxlen =
            DT_COT_SDK_UPDATE_DEFAULT_MAXLEN);

	bool sendMeasurement(const String type, const String model,
	            const String measurement, const String unit, const float value,
	            const time_t timeStamp);

	bool sendAlarm(const String text, const String type,
	            const AlarmSeverity severity, const AlarmStatus status);

	/** @brief This feature is not supported in MQTT-SN. The function is empty. */
	bool registerFirmwareUpdate(OperationsHandler &) { return false; }; // &operationsHandler) { return false; };

	/** @brief This feature is not supported in MQTT-SN. The function is empty. */
	bool registerConfigurationUpdate(OperationsHandler &) { return false; }; //& operationsHandler) { return false; };

	bool registerOperation(DTCotOperationCallBack callBackFunction,
            int operationId, String cmd, bool ignoreCmdCase);

	int executeOperations(int timeout = 100, bool trigger = false);

	/** @brief This feature is not supported in MQTT-SN. The function is empty. */
	String getOrCreateChildDevice(String) // childDeviceName)
	{
		String dummy("");
		return dummy;
	};

	/** @brief Returns true if the connection to the NB-IoT network is established. */
	bool isConnectedToNetwork(void);

    void triggerRetrievingOperations(String moId, int timeout, bool trigger);

    std::vector<CotOperationData> getRetrievedOperations();

    void respondOperationsState(std::vector<String> messageParts,
    		OperationStatus status, const String result, const String text);

#ifdef ARDUINO_ARCH_AVR
	/*BC95 related functions*/
	/** @brief Enable/disable power saving mode. BG96 might provide more options.*/
	void setPowerSaveMode(bool enable);
	/** @brief Get IMEI of the used NB-IoT modem.*/
	void getIMEI(String& imei);
	/** @brief Get firmware version information of the used NB-IoT modem.*/
	void getVersion(String& fwVersion);
	/** @brief  */
	void getCSQ(String& csq);
	/** @brief  */
	void getTrafficStats(String& traStats);
	/** @brief  */
	void getUeStats(String& ueStats);
#endif /*ARDUINO_ARCH_AVR*/


protected:
	/** @brief This feature is not supported in MQTT-SN. The function is empty. */
	bool createAndRegisterDevice();

	/** @brief This feature is not supported in MQTT-SN. The function is empty. */
	bool requestDeviceCredentials();

	CotSmartRestProcessor* getSmRProc();

	String getOperationStatusAsString(OperationStatus operationStatus);

private:
	static void OperationDefaultHandler(void* p);

	/*@brief Returns a topic ID. If the given topic is not yet registered, it also performs the topic registration.*/
	int GetTopicId(const String type, const String model, const String measurement);

    /*Treatment of operation functions (data tx --> device).*/
	typedef struct {
		String operationName;
		DTCotOperationCallBack callbackFn;
	} OperationCallbackT;

	/*utitility structures and lists to translate MQTT => MQTT-SN */

	typedef struct {
		String name;
		int id;
	} TopicStructT;

	typedef struct {
		String name;
		MqttSnMeasEnumT measTypeId;
	} TopicNameIdT;

	std::vector<TopicStructT> vActiveTopic;
	std::vector<TopicNameIdT> vMeasType = {
			{"Temperature",		MQTTSN_MEAS_TYPE_TEMPERATURE},
			{"Voltage",			MQTTSN_MEAS_TYPE_VOLTAGE},
			{"Acceleration",	MQTTSN_MEAS_TYPE_ACCELERATION},
			{"Light",			MQTTSN_MEAS_TYPE_LIGHT},
			{"Humidity",		MQTTSN_MEAS_TYPE_HUMIDITY},
			{"Moisture",		MQTTSN_MEAS_TYPE_MOISTURE},
			{"Distance",		MQTTSN_MEAS_TYPE_DISTANCE},
			{"Current",			MQTTSN_MEAS_TYPE_CURRENT},
			{"SignalStrength",	MQTTSN_MEAS_TYPE_SIGNALSTRENGTH},
			{"Pressure",		MQTTSN_MEAS_TYPE_PRESSURE},
			{"Volume",			MQTTSN_MEAS_TYPE_VOLUME},
			{"Weight",			MQTTSN_MEAS_TYPE_WEIGHT},
			{"Frequency",		MQTTSN_MEAS_TYPE_FREQUENCY},
			{"InfoCmd",			MQTTSN_MEAS_TYPE_INFCMD},
	};

// == Members ==
private:
	static std::vector<OperationCallbackT> vOperationCb;
	DTCoTMqttSn	*mqttsn = NULL;
	CoTConfigCommunicationMQTTSN* mqttsnConfig;
	CotOperationsEngine operationsEngine;
	std::vector<CotOperationData> retrievedOperations;

	/// TODO what is this here?
	static const int defaultCommandMessageId = DTCOTOPERATIONID_C8Y_COMMAND;
};

} /* namespace DTCoT */

#endif /* DT_CotSDKmqttSN_h_ */

