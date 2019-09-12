/**
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

/*FIXME Get rid of this workaround!*/
#ifndef ARDUINO_ARCH_AVR

#include <cot_sdk/DTCotSdkMqtt.h>
#include <cot_sdk/DTCotSmartRestProcessor.h>
#include "utility/DTCoTDebugOutput.h"
#include "utility/TimeoutHelper.h"

using namespace DTCoT;

CotSdkMqtt::CotSdkMqtt(CoTDeviceBase& device, const String templateId,
		const String bootstrap_Host, unsigned long bootstrap_Port,
		const String bootstrap_User, const String bootstrap_Password,
		const String cloudTenant_Host, unsigned long cloudTenant_Port)
	: selectedDevice(device)
	, persistence(selectedDevice.getEEPROM())
	, smartRestTemplateId(templateId)
	, bootstrap_Host(bootstrap_Host), bootstrap_Port(bootstrap_Port)
	, bootstrap_Password(bootstrap_Password), bootstrap_User(bootstrap_User)
	, cloudTenant_Host(cloudTenant_Host), cloudTenant_Port(cloudTenant_Port)
	, smartRestProcessor(NULL)
	, isInitialized(false)
	, myMoId("")
	, operationsEngine()
{
}

CotSdkMqtt::~CotSdkMqtt() {
	if (smartRestProcessor) {
		delete smartRestProcessor;
	}
}

void CotSdkMqtt::setTemplateId(const char* value) {
	smartRestTemplateId = value;
}

void CotSdkMqtt::setBootstrapHost(const char* value) {
	bootstrap_Host = value;
}

void CotSdkMqtt::setBootstrapPort(unsigned long value) {
	bootstrap_Port = value;
}

void CotSdkMqtt::setBootstrapUser(const char* value) {
	bootstrap_User = value;
}

void CotSdkMqtt::setBootstrapPwd(const char* value) {
	bootstrap_Password = value;
}

void CotSdkMqtt::setCloudTenantHost(const char* value) {
	cloudTenant_Host = value;
}

void CotSdkMqtt::setCloudTenantPort(unsigned long value) {
	cloudTenant_Port = value;
}



CotSmartRestProcessor* CotSdkMqtt::getSmRProc() {
	if (!smartRestProcessor) {
		smartRestProcessor = new CotSmartRestProcessor(selectedDevice,
				cloudTenant_Host, cloudTenant_Port, smartRestTemplateId,
				persistence);
	}
	return smartRestProcessor;
}

bool CotSdkMqtt::init(long &timeoutMs) {
	if (!isInitialized) {
		TimeoutHelper toh("CotSdkMqtt::init", timeoutMs);
		DT_DEBUG_PRINTLN_INFO("CotSdkMqtt::init()...");
		if (selectedDevice.init(timeoutMs)) {
			CoTConfigDevice& devConfig = (CoTConfigDevice&) selectedDevice.getDeviceConfig();
			persistence.load();
			if (persistence.setActualItemByUser(devConfig.getCloudDeviceId()) == false) {
				persistence.setActualItemCleared();
			}
			if(requestDeviceCredentials(timeoutMs)) {
				isInitialized = createAndRegisterDevice(timeoutMs);
				if (isInitialized) {
					DT_DEBUG_PRINTLN_INFO("CotSdkMqtt initializing for device<%s> with cloud moId<%s>...", persistence.get().getCotUser().c_str(), persistence.get().getMoId().c_str());
					isInitialized = updateDevice();
					if (isInitialized) {
						DT_DEBUG_PRINTLN("Cloud time<%s>", smartRestProcessor->getCloudTimestringNow().c_str());
						DT_DEBUG_PRINTLN("CotSdkMqtt initializing for device<%s> with cloud moId<%s>... done.", persistence.get().getCotUser().c_str(), persistence.get().getMoId().c_str());
						DT_DEBUG_PRINTLN_INFO("CotSdkMqtt::init()... done. moId<%s>", persistence.get().getMoId().c_str());
					} else {
						DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::init()... updateDevice failed!");
						DT_DEBUG_PRINTLN("CotSdkMqtt initializing for device<%s> with cloud moId<%s>... FAILED!", persistence.get().getCotUser().c_str(), persistence.get().getMoId().c_str());
					}
				} else {
					DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::init()... createAndRegisterDevice failed!");
				}
			} else {
				DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::init()... requestDeviceCredentials failed!");
			}
		} else {
			DT_DEBUG_PRINTLN_CRITICAL("CotSdkMqtt::init()... device initializing failed!");
		}
		toh.checkTimeout();

		operationsEngine.init( getSmRProc() );
	}
	return isInitialized;
}

bool CotSdkMqtt::requestDeviceCredentials(long &timeoutMs) {
	if (!persistence.get().isCleared() && !persistence.get().getTempUser().length()) {
		DT_DEBUG_PRINTLN_INFO( "CotSdkMqtt::requestDeviceCredentials() already found.");
		return true;
	}
	CoTConfigDevice& devConfig = (CoTConfigDevice&) selectedDevice.getDeviceConfig();
	CotBootstrapper bootstrapper = CotBootstrapper(selectedDevice, bootstrap_Host, bootstrap_Port, bootstrap_Password, bootstrap_User);
	boolean success = bootstrapper.requestDeviceCredentials(devConfig.getCloudDeviceId(), persistence, timeoutMs);
	DT_DEBUG_PRINTLN_INFO("CotSdkMqtt::requestDeviceCredentials() cloudDeviceId<%s>... done. success<%d> user<%s> pwd<%s>",
			devConfig.getCloudDeviceId().c_str(), success,
			persistence.get().getCotUser().c_str(),
			persistence.get().getCotPassword().c_str());
	selectedDevice.getClient()->stop();
	return success;
}

bool CotSdkMqtt::createAndRegisterDevice(long &timeoutMs) {
	TimeoutHelper toh("CotSdkMqtt::createAndRegisterDevice", timeoutMs);
	String moId = persistence.get().getMoId();
	if (moId.length() > 0) {
		myMoId = moId;
		DT_DEBUG_PRINTLN_INFO( "CotSdkMqtt::createAndRegisterDevice() already registered as moId<%s>.", moId.c_str());
		return true;
	}
	DT_DEBUG_PRINTLN_INFO("CotSdkMqtt::createAndRegisterDevice()...");

	DT_DEBUG_PRINTLN("");
	DT_DEBUG_PRINTLN("#################################################################");
	DT_DEBUG_PRINTLN("##");
	DT_DEBUG_PRINTLN("##       Now creating and registering the device in the cloud.");
	DT_DEBUG_PRINTLN("##");
	DT_DEBUG_PRINTLN("##       Be patient please.");
	DT_DEBUG_PRINTLN("##");
	DT_DEBUG_PRINTLN("#################################################################");


	bool registerOk = getSmRProc()->createAndRegisterDevice();
	while (!registerOk && !toh.checkTimeout()) {
		DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::createAndRegisterDevice()... FAILED! try again in 5 seconds...");
		delay(5000);
		registerOk = getSmRProc()->createAndRegisterDevice();
	}

	if(registerOk) {
		DT_DEBUG_PRINTLN("");
		DT_DEBUG_PRINTLN("#################################################################");
		DT_DEBUG_PRINTLN("##");
		DT_DEBUG_PRINTLN("##       Device is registered in the cloud.");
		DT_DEBUG_PRINTLN("##       Credentials:");
		DT_DEBUG_PRINTLN("##         moId     %s", persistence.get().getMoId().c_str());
		DT_DEBUG_PRINTLN("##         name     %s", persistence.get().getCotUser().c_str());
		DT_DEBUG_PRINTLN("##         password %s", persistence.get().getCotPassword().c_str());
		DT_DEBUG_PRINTLN("##");
		DT_DEBUG_PRINTLN("##       Attention!");
		DT_DEBUG_PRINTLN("##");
		DT_DEBUG_PRINTLN("##       Credentials will not be displayed again!");
		DT_DEBUG_PRINTLN("##");
		DT_DEBUG_PRINTLN("##       Please note carefully!");
		DT_DEBUG_PRINTLN("##");
		DT_DEBUG_PRINTLN("#################################################################");
		DT_DEBUG_PRINTLN("");

		moId = persistence.get().getMoId();
		DT_DEBUG_PRINTLN_INFO("CotSdkMqtt::createAndRegisterDevice()... done. moId<%s> registerOk<%d>", moId.c_str(), registerOk);
		myMoId = moId;

	} else {
		DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::createAndRegisterDevice()... FAILED!");
	}

	toh.checkTimeout();
	return registerOk;
}

int CotSdkMqtt::executeOperations(int timeout, bool trigger) {
	if (!isInitialized) {
		return -1;
	}
	return operationsEngine.executeOperations(myMoId, timeout, trigger);
}

bool CotSdkMqtt::updateDevice(const String hwModel, const String hwVersion,
		const String swVersion, const String imei, const String iccid,
		const String imsi, int maxlen) {
	if (!isInitialized) {
		DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::updateDevice()  Not initialized!");
		return false;
	}
	return getSmRProc()->updateDevice(myMoId, hwModel, hwVersion, swVersion, imei,
			iccid, imsi, maxlen);
}

bool CotSdkMqtt::sendMeasurement(const String type, const String model,
		const String measurement, const String unit, const float value,
		const time_t timeStamp) {
	if (!isInitialized) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSdkMqtt::sendMeasurement() Not initialized!");
		return false;
	}
	return getSmRProc()->sendMeasurement(myMoId, type, model, measurement, unit, value,
			timeStamp);
}

bool CotSdkMqtt::sendAlarm(const String text, const String type,
		const AlarmSeverity severity, const AlarmStatus status) {
	if (!isInitialized) {
		DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::sendAlarm() Not initialized!");
		return false;
	}
	return getSmRProc()->sendAlarm(myMoId, text, type, severity, status);
}

bool CotSdkMqtt::registerFirmwareUpdate(OperationsHandler &) {
	DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::registerFirmwareUpdate() Not yet implemented!");
	return false;
}

bool CotSdkMqtt::registerConfigurationUpdate(OperationsHandler &) {
	DT_DEBUG_PRINTLN_ERROR("CotSdkMqtt::registerConfigurationUpdate() Not yet implemented!");
	return false;
}

bool CotSdkMqtt::registerOperation(DTCotOperationCallBack callBackFunction,	int operationId, String cmd, bool ignoreCmdCase) {
	return operationsEngine.registerOperation(callBackFunction, operationId, cmd, ignoreCmdCase);
}

String CotSdkMqtt::getOrCreateChildDevice(String childDeviceName) {
	DT_DEBUG_PRINTLN_INFO("CotSdkMqtt::getOrCreateChildDevice(), Begin");// FIXME: Remove it!
	if (!isInitialized) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSdkMqtt::getOrCreateChildDevice() Not initialized!");
		return "";
	}
	return getSmRProc()->getOrCreateChildDevice(myMoId, childDeviceName);
}

void CotSdkMqtt::setCotCredentials(const String user, const String password, const String deviceMoId,
					const String tempUser, const String tempKey) {
	persistence.load();
	if (persistence.setActualItemByUser(user) == false) {
		persistence.setActualItemCleared();
	}
	persistence.get().setCotUser(user.c_str());
	persistence.get().setCotPassword(password.c_str());
	persistence.get().setMoId(deviceMoId.c_str());
	persistence.get().setTempUser(tempUser.c_str());
	persistence.get().setTempKey(tempKey.c_str());
	persistence.save();
}

#endif /*ARDUINO_ARCH_AVR*/

