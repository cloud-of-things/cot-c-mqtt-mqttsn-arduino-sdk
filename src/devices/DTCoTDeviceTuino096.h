/**
 * @file DTCoTDeviceFeatherM0_SIM800L.h
 * @description Header file for Class for management and operations for Feather M0 + SIM800L devices.
 * @author T-Systems GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_TUINO096_HEADER_FILE
#define DT_COT_DEVICE_TUINO096_HEADER_FILE

#define DT_COT_DEVICE_TUINO096_CLOUD_DEVICE_PREFIX	"TU96."

#define DT_COT_DEVICE_TUINO096_GSM_APN_DEFAULT          "internet.m2mportal.de"
#define DT_COT_DEVICE_TUINO096_GSM_APN_USER_DEFAULT		"m2m"
#define DT_COT_DEVICE_TUINO096_GSM_APN_PWD_DEFAULT		"sim"
#define DT_COT_DEVICE_TUINO096_NBIOT_APN_DEFAULT		"internet.nbiot.telekom.de"
// #define DT_COT_DEVICE_TUINO096_NBIOT_APN_DEFAULT		"nb-cloud.ic.m2mportal.de"

#define DT_COT_DEVICE_TUINO096_GSM_SSL_DEFAULT       	1
#define DT_COT_DEVICE_TUINO096_GSM_RX_PIN_DEFAULT       -1
#define DT_COT_DEVICE_TUINO096_GSM_TX_PIN_DEFAULT       -1
#define DT_COT_DEVICE_TUINO096_GSM_RST_PIN_DEFAULT      PIN_BG96_RESET
#define DT_COT_DEVICE_TUINO096_GSM_PWR_PIN_DEFAULT      PIN_BG96_PWRKEY
#define DT_COT_DEVICE_TUINO096_SERIAL_SPEED_DEFAULT		115200

#define SAMD21_UUID_ADDR ((uint32_t *)0x0080A00C)

#define EEPROM_EMULATION_SIZE 1024

#include "DTCoTPrivate.h"
#include "DTCoTExtensionIface.h"
#include "utility/DTCoTDebugOutput.h"
#include "utility/TimeoutHelper.h"
#include "DTCoTDeviceGPRS.h"

#include <FlashAsEEPROM.h>


namespace DTCoT {

/*****************************************************************************/
/**
 * CotEEPROM implementation of SamD.
 */
class CotEepromSamD: public CotEEPROM {

    uint8_t read(int address) { return EEPROM.read(address); };

    bool readBulk(int const address, size_t size, uint8_t* target) { 
        for(int i=0; (size_t)i<size; i++){ *(target+i) = read(address+i);}; return true; 
    };

    void write(int address, uint8_t value) { EEPROM.write(address, value); };

    size_t writeBulk(int const address, size_t size, const uint8_t* source) { 
        for(int i=0; (size_t)i<size; i++){ write(address+i, *(source+i));}; 
        return size;
    };

    void update(int address, uint8_t value) { EEPROM.update(address, value); };

    bool isValid() { return EEPROM.isValid(); };

    size_t length() { return EEPROM.length(); };

    void begin(size_t){};// size) { /*no equivalent*/ };

    void commit() { EEPROM.commit(); };

    void end() {  /*no equivalent*/ };
};

/*****************************************************************************/

static String getTuino96DeviceId(const char* devicePrefix) {
    return String(devicePrefix) + String(SAMD21_UUID_ADDR[2], 10).c_str();
}


/**
 * Config class for Gimasi Tuino 0 96 + BG96 modem.
 */
class CoTConfigDeviceTuino096: public CoTConfigDeviceGPRS {
  public:
    CoTConfigDeviceTuino096(
    		const char* apn = DT_COT_DEVICE_TUINO096_GSM_APN_DEFAULT,
			const char* apnName = DT_COT_DEVICE_TUINO096_GSM_APN_USER_DEFAULT,
			const char* apnPassword = DT_COT_DEVICE_TUINO096_GSM_APN_PWD_DEFAULT,
    		const char* devicePrefix = DT_COT_DEVICE_TUINO096_CLOUD_DEVICE_PREFIX,
            int ssl = DT_COT_DEVICE_TUINO096_GSM_SSL_DEFAULT,
            int rxPin = DT_COT_DEVICE_TUINO096_GSM_RX_PIN_DEFAULT,
			int txPin = DT_COT_DEVICE_TUINO096_GSM_TX_PIN_DEFAULT,
            int rstPin = DT_COT_DEVICE_TUINO096_GSM_RST_PIN_DEFAULT,
			int pwrPin = DT_COT_DEVICE_TUINO096_GSM_PWR_PIN_DEFAULT,
			long speed = DT_COT_DEVICE_TUINO096_SERIAL_SPEED_DEFAULT) :
        CoTConfigDeviceGPRS(apn, apnName, apnPassword, ssl, rxPin, txPin, rstPin,  pwrPin, speed)
    {
        cloudDeviceId = getTuino96DeviceId(devicePrefix);
    }
    void setDevicePrefix(const char* devicePrefix) { cloudDeviceId = getTuino96DeviceId(devicePrefix); };
    void setDeviceId(const char* deviceId) { cloudDeviceId = deviceId; };
};

/**
 * Config class for Gimasi Tuino 0 96 + BG96 modem (NBIOT mode).
 */
class CoTConfigDeviceTuino096NbIoT: public CoTConfigDeviceGPRS {
  public:
	CoTConfigDeviceTuino096NbIoT(
    		const char* apn = DT_COT_DEVICE_TUINO096_NBIOT_APN_DEFAULT,
			const char* apnName = DT_COT_DEVICE_TUINO096_GSM_APN_USER_DEFAULT,
			const char* apnPassword = DT_COT_DEVICE_TUINO096_GSM_APN_PWD_DEFAULT,
    		const char* devicePrefix = DT_COT_DEVICE_TUINO096_CLOUD_DEVICE_PREFIX,
            int rxPin = DT_COT_DEVICE_TUINO096_GSM_RX_PIN_DEFAULT,
			int txPin = DT_COT_DEVICE_TUINO096_GSM_TX_PIN_DEFAULT,
            int rstPin = DT_COT_DEVICE_TUINO096_GSM_RST_PIN_DEFAULT,
			int pwrPin = DT_COT_DEVICE_TUINO096_GSM_PWR_PIN_DEFAULT,
			long speed = DT_COT_DEVICE_TUINO096_SERIAL_SPEED_DEFAULT)
    : CoTConfigDeviceGPRS(apn, apnName, apnPassword, 0, rxPin, txPin, rstPin,  pwrPin, speed)
    {
        cloudDeviceId = getTuino96DeviceId(devicePrefix);
    }

    void setDevicePrefix(const char*) {// devicePrefix) {
    	DT_DEBUG_PRINTLN_ERROR("CoTConfigDeviceTuino096NbIoT::setDevicePrefix() not possible for NB-IoT");
    };
    void setDeviceId(const char*) {// deviceId) {
    	DT_DEBUG_PRINTLN_ERROR("CoTConfigDeviceTuino096NbIoT::setDeviceId() not possible for NB-IoT");
    };
};

/*****************************************************************************/
/**
 * Device class for Gimasi Tuino 0 96 + BG96 modem.
 */
class CoTDeviceTuino096: public CoTDeviceGPRS {
  public:
	/*constructor to configure the device for GSM/LTE mode*/
      CoTDeviceTuino096( const CoTConfigDeviceTuino096& config) 
      : CoTDeviceGPRS(config, Serial2)
      , eeprom()
      {
          DT_DEBUG_PRINTLN_DEBUG("CoTDeviceTuino096::CoTDeviceTuino096 (gprs)");
      };

	/*constructor to configure the device for NBIOT mode*/
	CoTDeviceTuino096( const CoTConfigDeviceTuino096NbIoT& config)
	: CoTDeviceGPRS(config, Serial2, true)
	, eeprom()
	{
		DT_DEBUG_PRINTLN_INFO("CoTDeviceTuino096::CoTDeviceTuino096 (nbiot)");
	};

  public:
	void modemReset(){
    	DT_DEBUG_PRINTLN_INFO("CoTDeviceTuino096::modemReset...");
        CoTConfigDeviceTuino096 &config = (CoTConfigDeviceTuino096&)_deviceConfig;

        // LOW is reset disabled
        digitalWrite(config.getRstPin(),HIGH);
        delay(500);
        digitalWrite(config.getRstPin(),LOW);
        delay(1500);

        DT_DEBUG_PRINTLN_INFO("CoTDeviceTuino096::modemReset... done.");
	}

    bool init(long &timeoutMs){
    	TimeoutHelper toh("CoTDeviceTuino096::init())", timeoutMs);
    	modemReset();
        bool rc = this->CoTDeviceGPRS::init(timeoutMs);
        DT_DEBUG_PRINTLN_INFO("CoTDeviceTuino096::init... done. <%d>", rc);
        toh.checkTimeout();
        return rc;
    };

	void errorHandler(const CoTHandlerParam&){};// error) {};

	/** Get the Arduino standard Client interface for this device */
	Client* getClient() const { return this->CoTDeviceGPRS::getClient(); };

    CotEEPROM& getEEPROM() { return (CotEEPROM&)eeprom; };

  private:
    CotEepromSamD eeprom;
};

} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_TUINO096_HEADER_FILE */

