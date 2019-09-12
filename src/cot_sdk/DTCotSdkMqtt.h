/**
 * @description Header file for central class of SDG for interaction with Telekom Cloud of Things
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_SDK_MQTT_H_
#define DT_COT_SDK_MQTT_H_


#include <cot_sdk/DTCotSdkBase.h>
#include <vector>
#include "DTCotSdkTypes.h"
#include "DTCotBootstrapper.h"
#include "DTCoTPersistence.h"
#include "DTCotSdkTypes.h"
#include "DTCotOperationsEngine.h"


#define DT_COT_MQTT_BOOTSTRAP_HOST_DEFAULT      "nb-iot.ram.m2m.telekom.com"
#define DT_COT_MQTT_BOOTSTRAP_PORT_DEFAULT      8883
#define DT_COT_MQTT_BOOTSTRAP_USER_DEFAULT      "devicebootstrap"
#define DT_COT_MQTT_BOOTSTRAP_PASSWORD_DEFAULT  "Fhdt1bb1f"
#define DT_COT_MQTT_TENNANT_HOST_DEFAULT        "nb-iot.ram.m2m.telekom.com"
#define DT_COT_MQTT_TENNANT_PORT_DEFAULT        8883
#define DT_COT_MQTT_SMARTREST_XID_DEFAULT       "CoT-MQTT-Arduino_Lib"


namespace DTCoT {

/**
 * High-level object for accessing the service.
 * TODO
 */
class CotSdkMqtt : public CotSdkBase {
    // == Functions ==
public:
    /**
     * Instantiate the CoT system if it wasn't registered in the Cloud yet.
     * This will prepare the system using the given device and configuration parameters.
     */
	CotSdkMqtt(
	        CoTDeviceBase& device,
	        const String templateId = DT_COT_MQTT_SMARTREST_XID_DEFAULT,
	        const String bootstrap_Host = DT_COT_MQTT_BOOTSTRAP_HOST_DEFAULT,
	        unsigned long bootstrap_Port = DT_COT_MQTT_BOOTSTRAP_PORT_DEFAULT,
	        const String bootstrap_User = DT_COT_MQTT_BOOTSTRAP_USER_DEFAULT,
	        const String bootstrap_Password = DT_COT_MQTT_BOOTSTRAP_PASSWORD_DEFAULT,
	        const String cloudTenant_Host = DT_COT_MQTT_TENNANT_HOST_DEFAULT,
	        unsigned long cloudTenant_Port = DT_COT_MQTT_TENNANT_PORT_DEFAULT);

	~CotSdkMqtt();

	void setTemplateId(const char* value);
	void setBootstrapHost(const char* value);
	void setBootstrapPort(unsigned long value);
	void setBootstrapUser(const char* value);
	void setBootstrapPwd(const char* value);
	void setCloudTenantHost(const char* value);
	void setCloudTenantPort(unsigned long value);

    bool init(long &timeoutMs);

    /**
     * Update of device attributes in the CoT cloud.
     *
     * @param maxlen Maximal total length of update strings, device name and template name.
     *    It is necessary to avoid buffer overflow at PUB / SUB MQTT messages in Adafruit_MQTT.
     *    This value 137 applies to #define MAXBUFFERSIZE (150)  in Adafruit_MQTT.h.
     *    If MAXBUFFERSIZE will be greater, then this default value may be corresponding increased.
     */
    bool updateDevice(
    		const String hwModel = 		DT_COT_SDK_UPDATE_DEFAULT_HW_MODEL,
			const String hwVersion = 	DT_COT_SDK_UPDATE_DEFAULT_HW_REVISION,
			const String swVersion = 	DT_COT_SDK_UPDATE_DEFAULT_SW_VERSION,
			const String imei = 		DT_COT_SDK_UPDATE_DEFAULT_IMEI,
			const String iccid = 		DT_COT_SDK_UPDATE_DEFAULT_ICCID,
			const String imsi = 		DT_COT_SDK_UPDATE_DEFAULT_IMSI,
			int maxlen =				DT_COT_SDK_UPDATE_DEFAULT_MAXLEN);

    bool sendMeasurement(const String type, const String model,
            const String measurement, const String unit, const float value,
            const time_t timeStamp);

    bool sendAlarm(const String text, const String type,
            const AlarmSeverity severity, const AlarmStatus status);

    bool registerFirmwareUpdate(OperationsHandler &operationsHandler);

    bool registerConfigurationUpdate(OperationsHandler &operationsHandler);

    bool registerOperation(DTCotOperationCallBack callBackFunction, int operationId,
    						String cmd, bool ignoreCmdCase);

    int executeOperations(int timeout = 100, bool trigger = false);

    String getOrCreateChildDevice(String childDeviceName);

    /// for internal use only
    CotSmartRestProcessor* getSmRProc();

    /**
     * Set the CoT's device credentials.
     * This is necessary while developing a sketch, as at some boards the persistant
     * memory of the device get's erased together with the sketch while uploading.
     * If the device was already registered in the cloud, the it would try to re-register
     * every time after a sketch upload.
     *
     * @param deviceID the device's cloud id or name
     * @param password the device's cloud password given at the registration
     * @param deviceMoId the device's cloud "managed object id"
     * 			(a cloud internal ID for the device)
     * @param tempUser only for development; ignore
     * @param tempKey only for development; ignore
     */
    void setCotCredentials(const String deviceID, const String password,
    		const String deviceMoId, const String tempUser = "", const String tempKey = "");

protected:
    bool createAndRegisterDevice(long &timeoutMs);
    bool requestDeviceCredentials(long &timeoutMs);

    // == Members ==
private:
    CoTDeviceBase& selectedDevice;
    DTCoTPersistence persistence;
    String smartRestTemplateId;
    String bootstrap_Host;
    unsigned long bootstrap_Port;
    String bootstrap_Password;
    String bootstrap_User;
    String cloudTenant_Host;
    unsigned long cloudTenant_Port;
    CotSmartRestProcessor *smartRestProcessor;
    bool isInitialized;
    String myMoId;
    CotOperationsEngine operationsEngine;
};

} /* namespace DTCoT */

#endif /* DT_COT_SDK_MQTT_H_ */

