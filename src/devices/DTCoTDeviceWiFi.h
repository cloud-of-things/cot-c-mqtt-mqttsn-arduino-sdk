/**
 * @file DTCoTDeviceWiFi.h
 * @description header file for base class for management and operations for all Wifi devices 
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_WIFI_HEADER_FILE
#define DT_COT_DEVICE_WIFI_HEADER_FILE

//#define DT_COT_DEVICE_WIFI_WITH_TX_RX_LOGGER

#include "DTCoTPrivate.h"
#include "DTCoTExtensionIface.h"
#include "base-classes/DTCoTDeviceBase.h"
#include "utility/DTCoTDebugOutput.h"
#include "utility/TimeoutHelper.h"

#ifdef DT_COT_DEVICE_WIFI_WITH_TX_RX_LOGGER
	#include "protocols/DTCotClient.h"
#endif

#if defined(ESP8266)
	#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_SAMD) 
	// for Feather M0 and Tuino96
	#include <WiFi101.h>
#else
    // default
	#include <WiFi.h>
#endif



namespace DTCoT {

   /**
    * Config class for generic WiFi device.
    */
class CoTConfigDeviceWiFi: public CoTConfigDevice {
public:
	
	/**
	 * constructor
	 * param WiFiSSID wifi AP
	 * param WiFiPassword wifi password
	 */
    CoTConfigDeviceWiFi(const char* WiFiSSID,
            const char* WiFiPassword) :
        CoTConfigDevice(), _WiFiSSID(WiFiSSID), _WiFiPassword(WiFiPassword) 
    { };

public:
    /** Getter for wifi SSID */
    const char* getWiFiSSID(){ return _WiFiSSID; };
    /** Getter for password */
    const char* getWiFiPassword(){ return _WiFiPassword; };
    /** Setter for wifi SSID */
    void setWiFiSSID(const char* ssid){ _WiFiSSID = ssid; };
    /** Setter for password */
    void setWiFiPassword(const char* pwd){ _WiFiPassword = pwd; };
	
private:
    const char* _WiFiSSID;
    const char* _WiFiPassword;
};

/**
 * Device class for generic WiFi device.
 */
class CoTDeviceWiFi : public CoTDeviceBase {
	
public:
    CoTDeviceWiFi(const CoTConfigDeviceWiFi& wifiConfig) :
        CoTDeviceBase(wifiConfig) 
    {
#ifdef DT_COT_DEVICE_WIFI_WITH_TX_RX_LOGGER
    	_logClient = new DTCotClient("WIFI", &wifiClient);
#endif
    };

	virtual ~CoTDeviceWiFi() {};

    bool init(long &timeoutMs){
		TimeoutHelper toh("CoTDeviceWiFi::init", timeoutMs);
        CoTConfigDeviceWiFi & config = (CoTConfigDeviceWiFi&) _deviceConfig;
        delay(10);
        DT_DEBUG_PRINTLN_INFO("CoTDeviceWiFi::init connecting WiFi<%s>", config.getWiFiSSID());
        WiFi.begin(config.getWiFiSSID(), config.getWiFiPassword());
        while (WiFi.status() != WL_CONNECTED && !toh.checkTimeout()) {
            DT_DEBUG_PRINT(".");
            delay(500);
        }
        if (WiFi.status() == WL_CONNECTED) {
        	DT_DEBUG_PRINTLN("connected with local ip<%s>", WiFi.localIP().toString().c_str());
            return true;
        }
    	DT_DEBUG_PRINTLN_ERROR("WIFI connect failed!");
        return false;
    };

	/** Get the Arduino standard Client interface for this device */
	Client * getClient() const{
#ifdef DT_COT_DEVICE_WIFI_WITH_TX_RX_LOGGER
		return _logClient;
#else
		return (Client*)&wifiClient;
#endif
	};

private:
	WiFiClientSecure wifiClient;
#ifdef DT_COT_DEVICE_WIFI_WITH_TX_RX_LOGGER
	DTCotClient *_logClient;
#endif
};

} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_WIFI_HEADER_FILE */

