/**
 * @file DTCoTDeviceFeatherM0_SIM800L.h
 * @description Header file for Class for management and operations for Feather M0 + SIM800L devices.
 * @author T-Systems GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_FEATHER_M0_SIM800L_HEADER_FILE
#define DT_COT_DEVICE_FEATHER_M0_SIM800L_HEADER_FILE

#define DT_COT_DEVICE_FEATHER_M0_SIM800L_SERIAL_SPEED_DEFAULT	9600
// 19200

#include "DTCoTPrivate.h"
#include "DTCoTDeviceGPRS.h"
#include "utility/DTCoTDebugOutput.h"

namespace DTCoT {

/**
 * Config class for Feather M0 device + quad band modem SIM800L.
 */
class CoTConfigDeviceFeatherM0_SIM800L: public CoTConfigDeviceGPRS {
  public:
    CoTConfigDeviceFeatherM0_SIM800L(const char* apn, const char * apnName,
            const char * apnPassword, int rxPin, int txPin, int rstPin, long speed =
            DT_COT_DEVICE_FEATHER_M0_SIM800L_SERIAL_SPEED_DEFAULT) :
        CoTConfigDeviceGPRS(apn, apnName, apnPassword, rxPin, txPin, rstPin, speed) {
        }
};

/**
 * Device class for Feather M0 + quad band modem SIM800L.
 * This class should also work for any other ESP8266 based board.
 */
class CoTDeviceFeatherM0_SIM800L: public CoTDeviceGPRS {
  public:
      CoTDeviceFeatherM0_SIM800L( const CoTConfigDeviceFeatherM0_SIM800L& config) :
          CoTDeviceGPRS(config) 
      {
          DT_DEBUG_PRINT("CoTDeviceFeatherM0_SIM800L::CoTDeviceFeatherM0_SIM800L");
      };

  public:
    bool init(){
        DT_DEBUG_PRINT("CoTDeviceFeatherM0_SIM800L::init...");
        CoTDeviceGPRS::init();
        DT_DEBUG_PRINT("CoTDeviceFeatherM0_SIM800L::init... done.");
        return false;
    };
	void errorHandler(const CoTHandlerParam& error)
    {};
};

} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_FEATHER_M0_SIM800L_HEADER_FILE */

