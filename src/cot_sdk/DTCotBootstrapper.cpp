/**
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

/*FIXME Get rid of this workaround!*/
#ifndef ARDUINO_ARCH_AVR

#include "DTCotBootstrapper.h"
#include "utility/DTCoTDebugOutput.h"
#include "utility/TimeoutHelper.h"
#include "DTCoTExtensionIface.h"
#include <Adafruit_MQTT_Client.h>

/**
 * Create 128-Bit password for MQTT-connection to CoT server.
 */
void createTempPassword(char* tempPwd, size_t bufLen) {
    int keyLength = 0;
    // while() is necessary, because of possibly wrong length
    while (keyLength != 16) {
        memset(tempPwd, 0, bufLen);
        randomSeed(micros());
        // Create 128 bit/ 16 byte, random password key.
        strcpy((char*) (&tempPwd[0]), String(random(0xfffffff), HEX).c_str());
        strcpy((char*) (&tempPwd[7]), String(random(0xfffffff), HEX).c_str());
        strcpy((char*) (&tempPwd[14]), String(random(0xff), HEX).c_str());
        keyLength = strlen(tempPwd);
    }
}

using namespace DTCoT;

CotBootstrapper::CotBootstrapper(const CoTDeviceBase& device,
        const String bootstrap_Host, unsigned long bootstrap_Port,
        const String bootstrap_Password, const String bootstrap_User) :
        comm(NULL), subBuffer(""), pubBuffer("") {
	DT_DEBUG_PRINTLN_DEBUG("CotBootstrapper::CotBootstrapper()...");

    CoTConfigDevice& devConfig = (CoTConfigDevice&) device.getDeviceConfig();
    String deviceId = devConfig.getCloudDeviceId();
    subBuffer = String("sr/") + deviceId;
    pubBuffer = String("ss/") + deviceId;

    comm = new CoTCommunicationMQTT(
            device
            , CoTConfigCommunicationMQTT(bootstrap_Host, bootstrap_Port, bootstrap_Password, bootstrap_User)
            , cloudAuthMethod);
    comm->subscribe(subBuffer.c_str(), NULL);

    DT_DEBUG_PRINTLN_DEBUG("CotBootstrapper::CotBootstrapper()... done.");
}

CotBootstrapper::~CotBootstrapper() {
    DT_DEBUG_PRINTLN_DEBUG("CotBootstrapper::~CotBootstrapper()...");
    if (comm) {
        delete (comm);
    }
    DT_DEBUG_PRINTLN_DEBUG("CotBootstrapper::~CotBootstrapper()... done.");
}

bool CotBootstrapper::checkTimeoutMs(long &timeoutMs) {
	static unsigned long lastTs = 0;
	unsigned long ts = millis();
	timeoutMs -= (long)(ts - lastTs);
	lastTs = ts;
	return timeoutMs <= 0L;
}

boolean CotBootstrapper::requestDeviceCredentials(const String iccid, DTCoTPersistence &persistence, long &timeoutMs) {
	TimeoutHelper toh("CotBootstrapper::requestDeviceCredentials()", timeoutMs);
	DT_DEBUG_PRINTLN_INFO("CotBootstrapper::requestDeviceCredentials() iccid<%s>  timeoutMs<%ld>...", iccid.c_str(), timeoutMs);

    if (iccid.equals(persistence.get().getTempUser()) == false || persistence.get().getTempKey().length() <= 0) {
        // Global declaration of Password buffer.
        char tempPwdKey[16];
        // Create 128-Bit password for MQTT-connection to CoT server.
        createTempPassword(tempPwdKey, sizeof(tempPwdKey));
        persistence.get().setTempKey(tempPwdKey);
        persistence.get().setTempUser(iccid.c_str());
        persistence.get().setCotUser("");
        persistence.get().setCotPassword("");
        persistence.get().setMoId("");
        persistence.save();
    }

    comm->publish(pubBuffer.c_str(), persistence.get().getTempKey().c_str());

    DT_DEBUG_PRINTLN("");
	DT_DEBUG_PRINTLN("#################################################################");
    DT_DEBUG_PRINTLN("##");
    DT_DEBUG_PRINTLN("##       Please register device now in the cloud");
    DT_DEBUG_PRINTLN("##       (or save valid credentials for it, if registered)");
    DT_DEBUG_PRINTLN("##");
    DT_DEBUG_PRINTLN("##       %s", iccid.c_str());
    DT_DEBUG_PRINTLN("##");
    DT_DEBUG_PRINTLN("##       and push button to acknowledge.");
    DT_DEBUG_PRINTLN("##");
	DT_DEBUG_PRINTLN("##       Then please be patient and wait (about 1 min).");
    DT_DEBUG_PRINTLN("##");
	DT_DEBUG_PRINTLN("#################################################################");
    DT_DEBUG_PRINTLN("");

    char deviceCredentials[17];
    memset(deviceCredentials, 0, sizeof(deviceCredentials));
    char receivedPwd[11];
    memset(receivedPwd, 0, sizeof(receivedPwd));

    uint8_t reponseBuffer[16];
    int timeout = 5000;
    int len = 0;
    while (len <= 0 && !toh.checkTimeout()) {
        DT_DEBUG_PRINT(".");
        len = comm->readSubscription(timeout, reponseBuffer, 16);
    }

    bool requestOk = false;

    if (len > 0) {
		memcpy(deviceCredentials, reponseBuffer, len);
		EncryptionHelper encryptionHelper;
		bool decryptSuccess = encryptionHelper.decryptPassword(persistence.get().getTempKey().c_str(), deviceCredentials, len, receivedPwd);
		if (!decryptSuccess) {
			DT_DEBUG_PRINTLN_ERROR("CotBootstrapper::requestDeviceCredentials() iccid<%s>... decryption FAILED!", iccid.c_str());
		} else {
			persistence.get().setCotUser(iccid.c_str());
			persistence.get().setCotPassword(receivedPwd);
			persistence.get().setTempUser("");
			persistence.get().setTempKey("");
			persistence.save();
			requestOk = true;
		}
    } else{
    	DT_DEBUG_PRINTLN_ERROR("CotBootstrapper::requestDeviceCredentials() subscription response length is 0!");
    }

	toh.checkTimeout();
	return requestOk;
}

#endif /*ARDUINO_ARCH_AVR*/
