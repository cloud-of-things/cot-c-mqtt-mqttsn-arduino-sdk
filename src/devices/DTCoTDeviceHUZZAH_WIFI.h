/**
 * @file DTCoTDeviceHUZZAH.h
 * @description header file for Class for management and operations for all HUZZAH devices
 * @author T-Systems GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_HUZZAH_WIFI_HEADER_FILE
#define DT_COT_DEVICE_HUZZAH_WIFI_HEADER_FILE

#define DT_COT_DEVICE_HUZZAH_WIFI_CLOUD_DEVICE_ID_PREFIX_DEFAULT "HUZ."

#include "DTCoTPrivate.h"
#include "DTCoTDeviceWiFi.h"
#include <EEPROM.h>
#include "utility/DTCoTDebugOutput.h"

namespace DTCoT {

static String getHuzzahWifiDeviceId(const char* devicePrefix) {
    return String(devicePrefix) + String(ESP.getChipId(), 10);
}

/**
 * CotEEPROM implementation of ESP8266.
 */
class CotEepromHuzzah: public CotEEPROM {
    uint8_t read(int address) { return EEPROM.read(address); };
    bool readBulk(int const address, size_t size, uint8_t* target) {
        for (int i = 0; (size_t) i < size; i++) {
            *(target + i) = read(address + i);
        };
        return true;
    };
    void write(int address, uint8_t value) { EEPROM.write(address, value); };
    size_t writeBulk(int const address, size_t size, const uint8_t* source) {
        for (int i = 0; (size_t) i < size; i++) {
            write(address + i, *(source + i));
        };
        return size;
    };
    void update(int, uint8_t) { /*no equivalent*/ };

    bool isValid() { /*no equivalent*/ return true; };
    size_t length() { return EEPROM.length(); };
    void begin(size_t size) { EEPROM.begin(size); };
    void commit() { EEPROM.commit(); };
    void end() { EEPROM.end(); };
};

/**
 * Config class for HUZZAH ESP8266 device.
 */
class CoTConfigDeviceHuzzahWifi: public CoTConfigDeviceWiFi {
public:
    CoTConfigDeviceHuzzahWifi(const char* WiFiSSID, const char* WiFiPassword,
            const char* devicePrefix = DT_COT_DEVICE_HUZZAH_WIFI_CLOUD_DEVICE_ID_PREFIX_DEFAULT) :
            CoTConfigDeviceWiFi(WiFiSSID, WiFiPassword) {
        cloudDeviceId = getHuzzahWifiDeviceId(devicePrefix);
    }
    void setDevicePrefix(const char* devicePrefix) { cloudDeviceId = getHuzzahWifiDeviceId(devicePrefix); };
    void setDeviceId(const char* deviceId) { cloudDeviceId = deviceId; };
};

/**
 * Device class for HUZZAH ESP8266 with using wifi connection.
 * This class should also work for any other ESP8266 based board running wifi.
 */
class CoTDeviceHuzzahWifi: public CoTDeviceWiFi {
public:
    CoTDeviceHuzzahWifi(const CoTConfigDeviceHuzzahWifi& config) :
            CoTDeviceWiFi(config), eeprom() {
        ESP.wdtEnable(WDTO_8S);
    };
    virtual ~CoTDeviceHuzzahWifi() {};
public:
    bool init(long &timeoutMs) {
        DT_DEBUG_PRINTLN_INFO("CoTDeviceHuzzahWifi::init()...");
        // #ESt, WDT Reset because of "WDT Soft Reset" Error. These lines MUST BE HERE !
        ESP.wdtDisable();
        ESP.wdtEnable(0);
       // delay(1);	- Does not help effectively

        bool initSuccess = CoTDeviceWiFi::init(timeoutMs);
        DT_DEBUG_PRINTLN_INFO(
                "CoTDeviceHuzzahWifi::init()... done. initSuccess<%d>",
                initSuccess);
        return initSuccess;
    };
    void errorHandler(const CoTHandlerParam&) {};
    CotEEPROM& getEEPROM() { return (CotEEPROM&) eeprom; };
private:
    CotEepromHuzzah eeprom;
};

} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_HUZZAH_WIFI_HEADER_FILE */

