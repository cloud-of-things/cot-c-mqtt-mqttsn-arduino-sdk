/**
 * @file DTCotBootstrapper.h
 * @description header file for an Interface for Bootstrap a Device at the CoT
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef Bootstrapper_h_
#define Bootstrapper_h_

#include <cot_sdk/DTCotSdkMqtt.h>
#include "base-classes/DTCoTDeviceBase.h"
#include "base-classes/DTCoTCommunicationBase.h"
#include "communication/DTCotCommunicationMQTT.h"
#include "utility/DTCoTDebugOutput.h"
#include "DTEncryptionHelper.h"
#include "DTCoTPersistence.h"


namespace DTCoT {
	
/**
 * Calls the bootstrap credentials request and stores the received device name
 * and passwort in persistant memory.
 */
class CotBootstrapper 
{ 
  // == Methods ==
  public:

    /** 
     * C-Tor
     * @param device the device to do the communication over
     */
    CotBootstrapper(const CoTDeviceBase& device
        , const String  bootstrap_Host
		, unsigned long bootstrap_Port 
		, const String  bootstrap_Password
		, const String  bootstrap_User
    );

    ~CotBootstrapper();

    /**
     * Register device on DT MQTT registration broker.
     * Publishes a value to the remote broker. You should see the value change immediately.
     * @param iccid The ICCID of desired device (IMEI/MAC).
     *   Used for creation of key/topic to publish to. For example "123456789012".
     */
    boolean requestDeviceCredentials(const String iccid, DTCoTPersistence &persistence, long &timeoutMs);

    bool credentialsDecrypt(const char* pwdKey, const char* deviceCredentials, size_t credLen, char* pwdBuffer);

  private:

    void StringCrypt(char *inout, int len, bool encrypt);
    bool checkTimeoutMs(long &timeoutMs);

  //  == Members ==
  private:
    static const int BLOCK_SIZE = 8;
    CoTCommunicationMQTT *comm;
    String subBuffer;
    String pubBuffer;
    CoTAuthTLS cloudAuthMethod;
};

}

#endif // Bootstrapper_h_

