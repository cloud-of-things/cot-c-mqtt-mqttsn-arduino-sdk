/**
 * @file DTCoTDeviceGimasi.h
 * @description header file for Class for management and operations for Gimasi NB-IoT device
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_GIMASI_HEADER_FILE
#define DT_COT_DEVICE_GIMASI_HEADER_FILE

#include "DTCoTPrivate.h"
#include "DTCoTExtensionIface.h"
#include "protocols/NbIoTClient.h"
#include "utility/DTCoTDebugOutput.h"
#include "devices/DTCotEEPROM.h"


namespace DTCoT {
/**
 * CotEEPROM implementation of Gimasi Tuino.
 */
class CotEepromGimasi: public CotEEPROM {

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

	void begin(size_t size) { /*no equivalent*/ };

	void commit() { EEPROM.commit(); };

	void end() {  /*no equivalent*/ };

#ifdef ARDUINO_1284P_STANDARD
	/*NOTE on workaround:
	 * This EEPROM class is a workaround to make the project compilable for Tuino1.
	 */
	class EEPROMdummy {
	public:
		EEPROMdummy(){};
		~EEPROMdummy(){};
		uint8_t read(int address){return 0;};
		void write(int address, uint8_t value){};
		void update(int address, uint8_t value){};
		bool isValid(){return false;};
		size_t length(){return 0;};
		void commit(){};
	};
private:
	EEPROMdummy EEPROM;
#endif /*ARDUINO_1284P_STANDARD*/
};



/**
 * Config class for Tuino 1 IoT device.
 */
class CoTConfigDeviceGimasi: public CoTConfigDevice {
public:
	CoTConfigDeviceGimasi(
			const char* serverIP
			, const unsigned short serverPort
			, const char* imsi
			, const char* password
			, Stream & serial
			, int resetPin)
	: CoTConfigDevice()
	, _serverIP(serverIP), _serverPort(serverPort)
	, _imsi(imsi), _password(password), _apn((const char*)NULL), _serial(serial), _resetPin(resetPin)
{ };

public:
	const char* getServerIP(){ return _serverIP; };
	const unsigned short getServerPort(){ return _serverPort; };
	const char* getIMSI(){ return _imsi; };
	const char* getPassword(){ return _password; };
	const char* getAPN(){ return _apn; };
	Stream & getSerial(){ return _serial; };
	int getResetPin(){ return _resetPin; };

private:
	const char* _serverIP;
	const unsigned short _serverPort;
	const char* _imsi;
	const char* _password;
	const char* _apn;
	Stream& _serial;
	int _resetPin;
};

/**
 * Device class for Tuino 1 IoT device.
 */
class CoTDeviceGimasi : public CoTDeviceBase {
public:
	CoTDeviceGimasi(  CoTConfigDeviceGimasi& nbiotConfig)
	: CoTDeviceBase(nbiotConfig)
    ,_nbiotClient (		nbiotConfig.getServerIP()
                      , nbiotConfig.getServerPort()
                      , nbiotConfig.getIMSI()
                      , nbiotConfig.getPassword()
                      , nbiotConfig.getAPN()
                      , nbiotConfig.getSerial()
                      , nbiotConfig.getResetPin()
      )
	{ };

public:
	bool init(long &timeoutMs) {
        DT_DEBUG_PRINT("CoTDeviceNBIoT::init");
        /* Initialize Gimasi modem*/
        _nbiotClient.init(timeoutMs);
		return false;
	};

	Client* getClient() const{ return (Client*)(&_nbiotClient); };

	void errorHandler( const CoTHandlerParam& error ) {};

	// Overwrite virtual method.
	CotEEPROM& getEEPROM() { return (CotEEPROM&) eeprom; };

private:
	NbiotClient _nbiotClient;
	CotEepromGimasi eeprom;
};


} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_GIMASI_HEADER_FILE */

