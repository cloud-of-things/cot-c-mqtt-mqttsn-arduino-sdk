/**
 * @file DTCoTDeviceHUZZAH.h
 * @description header file for Class for management and operations for all HUZZAH devices
 * @author T-Systems GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_HUZZAH_SIM800L_HEADER_FILE
#define DT_COT_DEVICE_HUZZAH_SIM800L_HEADER_FILE

#define DT_COT_DEVICE_HUZZAH_SIM800L_SERIAL_SPEED_DEFAULT	19200
#define DT_COT_DEVICE_HUZZAH_SIM800L_CLOUD_DEVICE_ID_PREFIX_DEFAULT "S800."

#include "DTCoTPrivate.h"
#include "DTCoTDeviceGPRS.h"
#include "utility/DTCoTDebugOutput.h"

namespace DTCoT {


static String getHuzzah_SIM800L_DeviceId(const char* devicePrefix) {
    return String(devicePrefix) + String(ESP.getChipId(), 10);
}

/**
 * Config class for HUZZAH ESP8266 device + quad band modem SIM800L.
 */
class CoTConfigDeviceHUZZAH_SIM800L: public CoTConfigDeviceGPRS {
  public:
      CoTConfigDeviceHUZZAH_SIM800L(const char* apn, const char * apnName,
              const char * apnPassword, int rxPin, int txPin, int rstPin, 
              const char* devicePrefix = DT_COT_DEVICE_HUZZAH_SIM800L_CLOUD_DEVICE_ID_PREFIX_DEFAULT,
              long speed = DT_COT_DEVICE_HUZZAH_SIM800L_SERIAL_SPEED_DEFAULT) :
          CoTConfigDeviceGPRS(apn, apnName, apnPassword, rxPin, txPin, rstPin, speed) {
              cloudDeviceId = getHuzzah_SIM800L_DeviceId(devicePrefix);
          };
      void setDevicePrefix(const char* devicePrefix) { cloudDeviceId = getHuzzah_SIM800L_DeviceId(devicePrefix); };
      void setDeviceId(const char* deviceId) { cloudDeviceId = deviceId; };
};

/**
 * Device class for HUZZAH ESP8266 + quad band modem SIM800L.
 * This class should also work for any other ESP8266 based board.
 */
class CoTDeviceHUZZAH_SIM800L: public CoTDeviceGPRS {
  public:
    CoTDeviceHUZZAH_SIM800L(
            const CoTConfigDeviceHUZZAH_SIM800L& config) :
        CoTDeviceGPRS(config) {
            DT_DEBUG_PRINT("CoTDeviceHUZZAH_SIM800L::CoTDeviceHUZZAH_SIM800L");
        };
  public:
    bool init() {
        DT_DEBUG_PRINT("CoTDeviceHUZZAH_SIM800L::init...");
        CoTDeviceGPRS::init();
        DT_DEBUG_PRINT("CoTDeviceHUZZAH_SIM800L::init... done.");
        return false;
    };

	void errorHandler(const CoTHandlerParam& error)
    {};
};

} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_HUZZAH_SIM800L_HEADER_FILE */
