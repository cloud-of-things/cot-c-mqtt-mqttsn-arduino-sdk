/**
 * @file NbiotClient.cpp
 * @description Abstracted Network interface of an NB-IoT Modem.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 * 
 * Code pieces based on Arduino WiFiClient and GIMASI TUINO example project(s):
 * Company: http://www.tuino.io/
 * GitHub:  https://github.com/gimasi
 */

#ifdef ARDUINO_ARCH_SAMD

extern "C" {
	#include "string.h"
}

#include "dt-arduino-iot-agent.h"
#include "utility/Regexp.h"
#include "protocols/ModemBg96.h"
#include "protocols/NbIoTClient.h"

#define MAX_SOCK_NUM  0xFF

uint16_t NbiotClient::_srcport = 1883;


NbiotClient::NbiotClient(DTCoTModemBase *modem, Stream& serial, int resetPin, const char* apn,
			const char* mqttsnUser, const char* mqttsnPwd)
	: _socket(MAX_SOCK_NUM)
	, _serverPort(0)
	, _serverIP("")
	, _imsi(mqttsnUser) /*IMSI == username*/
	, _password(mqttsnPwd)
	, _apn(apn)
	, _serial(serial)
	, _resetPin(resetPin)
	, _modem(modem)
	, _modemInitialized( false)
{
}

NbiotClient::~NbiotClient() {
}

/**
 * @brief Dummy method which has to be there. Does nothing but return an error if called.
 * @note This function always returns an error, since NBIOT does not support host name resolution. Use IP addresses, instead.
 * @param host Host IP address
 * @param port Destination port.
 * @returns Returns always error code 0, since NBIOT cannot resolve host names. Use IP addresses, instead.
 */
int NbiotClient::connect(const char*, uint16_t) {
	DT_DEBUG_PRINTLN_ERROR(__PRETTY_FUNCTION__, " not implemented");
	return 0;
}

int NbiotClient::connect(IPAddress ip, uint16_t port) {
	String ipStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
	DT_DEBUG_PRINTLN_DEBUG("NbiotClient::connect() ip<%s> port<%d>", ipStr.c_str(), port);
	if (_modem->connectSocketUdp(ipStr.c_str(), port) == DTCOT_MODEM_SOCKET_CONNECTED) {
		_serverIP = ipStr;
		_serverPort = port;
		return 1;
	}
	return 0;
}

/**
 * @brief Bytewise data transmission.
 * @note Will not be implemented (yet?).
 * @param b Byte to be transmitted.
 * @returns Number of bytes that were sent.
 */
size_t NbiotClient::write(uint8_t b) {
	return write(&b, 1);
}

/**
 * @brief Packet-wise data transmission.
 * @param buf Payload buffer to be sent.
 * @param size Number of bytes to be sent.
 * @returns Number of bytes that were sent.
 */
size_t NbiotClient::write(const uint8_t *buf, size_t size) {
	if(DTCOT_MODEM_OK != _modem->txData(buf, size)) {
		return 0;
	}
	return size;
}

/**
 * @brief Returns a value > 0 for a received / pending packet.
 * @returns 0 for no pending received data , >0 otherwise.
 */
int NbiotClient::available() {
	int availableSize = 0;
	if (_modem->rxDataAvailable(availableSize) == DTCOT_MODEM_SOCKET_DATA_AVAILABLE) {
		return availableSize;
	}
	return 0;
}

/**
 * @brief Byte-wise read.
 */
int NbiotClient::read() {
	uint8_t buf[1];
	if (read(buf, 1) == 1) {
		return (int)buf[0];
	}
	return 0;
}

int NbiotClient::read(uint8_t* buf, size_t size) {
	int readSize = size;
	if (DTCOT_MODEM_OK != _modem->rxData(buf, readSize)) {
		readSize = -1;
	}
	return readSize;
}

int NbiotClient::peek() {
	DT_DEBUG_PRINTLN_DEBUG(String(__PRETTY_FUNCTION__)+" not implemented");
	return -1;
}

void NbiotClient::flush() {
	//DT_DEBUG_PRINTLN_DEBUG(String(__PRETTY_FUNCTION__)+" not implemented");
	// TODO: a real check to ensure transmission has been completed
}

void NbiotClient::stop() {
	DT_DEBUG_PRINTLN_INFO("NbiotClient::stop()...");
	DTCOT_MODEM_STATUS status = _modem->closeSocket();
	_modemInitialized = false;
	DT_DEBUG_PRINTLN_INFO("NbiotClient::stop()... done. <%d>", status);
}

uint8_t NbiotClient::connected() {
	uint8_t rc = 0;
	if (!_modemInitialized) {
		DT_DEBUG_PRINTLN_ERROR(
				"NbiotClient::connected() Modem is not initialized!");
	} else {
		rc = (DTCOT_MODEM_SOCKET_CONNECTED == _modem->isSocketConnected());
	}
	return rc;
}

NbiotClient::operator bool() {
	DT_DEBUG_PRINTLN_DEBUG("NbiotClient::operator bool()");
	return (_socket != MAX_SOCK_NUM);
}

// Private Methods

bool NbiotClient::initNBIoTModem(long &timeoutMs) {
	if (_modemInitialized) {
		DT_DEBUG_PRINTLN_DEBUG("NbiotClient::initNBIoTModem() already done");
		return true;
	}

	TimeoutHelper toh("NbiotClient::initNBIoTModem()", timeoutMs);
	DT_DEBUG_PRINTLN_DEBUG("NbiotClient::initNBIoTModem() initializing modem...");

	if (toh.checkTimeout() || _modem->init() != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() Failed to initialize modem");
		return false;
	}

	String versionInfo;
	if (toh.checkTimeout() || _modem->getVersion(versionInfo) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() Failed to aquire modem version");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() modem version<%s>", versionInfo.c_str());

	String iccidInfo;
	if (toh.checkTimeout() || _modem->getICCID(iccidInfo) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() Failed to aquire modem ICCID! Is SIM card inserted?");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() ICCID<%s>", iccidInfo.c_str());

	String imeiInfo;
	if (toh.checkTimeout() || _modem->getIMEI(imeiInfo) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() Failed to aquire modem IMEI");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() IMEI<%s>", imeiInfo.c_str());

	if (toh.checkTimeout() || _modem->connectNbiot(_apn.c_str(), _imsi.c_str(), _password.c_str()) != DTCOT_MODEM_NBIOT_CONNECTED) {
		DT_DEBUG_PRINTLN_ERROR("NbiotClient::initNBIoTModem() Failed to connect APN");
		return false;
	}
	DT_DEBUG_PRINTLN_DEBUG("NbiotClient::initNBIoTModem() NB connected");

	String csqInfo;
	DTCOT_MODEM_STATUS getCSQStatus = _modem->getCSQ(csqInfo);
	if (getCSQStatus == DTCOT_MODEM_SIGNAL_QUALITY_FAILED) {
		DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() waiting for signal..",	csqInfo.c_str());
		while (!toh.checkTimeout() && getCSQStatus == DTCOT_MODEM_SIGNAL_QUALITY_FAILED) {
			DT_DEBUG_PRINT(".");
			delay(500);
			getCSQStatus = _modem->getCSQ(csqInfo);
		}
		DT_DEBUG_PRINTLN("");
	}
	if (toh.checkTimeout() ||getCSQStatus != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_ERROR("NbiotClient::initNBIoTModem() Failed to aquire modem signal quality");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("NbiotClient::initNBIoTModem() Signal quality<%s>",	csqInfo.c_str());

	DT_DEBUG_PRINTLN_DEBUG("NbiotClient::initNBIoTModem() initializing modem... done.");

	toh.checkTimeout();

	_modemInitialized = true;
	return _modemInitialized;
}

bool NbiotClient::init(long &timeoutMs){
	if (!initNBIoTModem(timeoutMs)) {
		DT_DEBUG_PRINTLN_ERROR("NbiotClient::init(): ERROR: Failed to initialize the modem");
		return 0;
	}
	return 1;
}

#endif /*ARDUINO_ARCH_SAMD*/
