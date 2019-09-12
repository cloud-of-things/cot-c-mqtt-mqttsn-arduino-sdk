/**
 * @file DTCoTDeviceFeatherM0-WIFI.h
 * @description header file for Class for management and operations for Feather M0 WiFi device
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_FEATHER_M0_WIFI_HEADER_FILE
#define DT_COT_DEVICE_FEATHER_M0_WIFI_HEADER_FILE


#include "DTCoTPrivate.h"
#include "DTCoTDeviceWiFi.h"
#include "utility/DTCoTDebugOutput.h"

namespace DTCoT {

/**
 * Config class for Feather M0 WiFi setup
 */
class CoTConfigDeviceFeatherM0_WiFi: public CoTConfigDeviceWiFi {
  public:
    /**
     * Constructor
     * @param WiFiSSID wifi AP name
     * @param WiFiPassword password for the wifi network
     */
    CoTConfigDeviceFeatherM0_WiFi( 
            const char* WiFiSSID
            , const char* WiFiPassword )
        : CoTConfigDeviceWiFi( WiFiSSID, WiFiPassword) 
    { };
};


/**
 * Feather M0 board running WiFi connection
 */
class CoTDeviceFeatherM0_WiFi : public CoTDeviceWiFi {
  public:
    /**
     * constructor
     * @param cfg configuration class to setup the device.
     */
    CoTDeviceFeatherM0_WiFi(  const CoTConfigDeviceFeatherM0_WiFi& cfg )
        : CoTDeviceWiFi( cfg)
    { }

public:
    bool init() {
        CoTDeviceWiFi::init();
        DT_DEBUG_PRINTLN_INFO("CoTDeviceFeatherM0_WiFi::CoTDeviceFeatherM0_WiFi");
        return false;
    }

	void errorHandler( const CoTHandlerParam& error ) 
    { };
};

} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_FEATHER_M0_HEADER_WIFI_FILE */

