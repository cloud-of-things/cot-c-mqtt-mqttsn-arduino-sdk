/**
 * @file TcpIotClient.cpp
 */

extern "C" {
#include "string.h"
}

/*FIXME Get rid of this workaround!*/
#ifndef ARDUINO_ARCH_AVR

#include "utility/Regexp.h"
#include "TcpIoTClient.h"

#include "DTCoTModemBase.h"

#include "utility/DTCoTDebugOutput.h"
#include "utility/TimeoutHelper.h"

TcpIotClient::TcpIotClient(DTCoTModemBase *modem, int resetPin, const char* apn,
		const char* apnUser, const char* apnPwd, int ssl) :
		_modem(modem), _resetPin(resetPin), _modemInitialized(false), apn(apn), apnUser(
				apnUser), apnPwd(apnPwd), ssl(ssl), _lastPrintedConnectState(0), connectedHost(""), connectedPort(0) {
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::TcpIotClient()... done.");
}

TcpIotClient::~TcpIotClient() {
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::~TcpIotClient()... done.");
}

int TcpIotClient::connect(const char* host, uint16_t port) {
	unsigned long start = millis();
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::connect() host<%s> port<%u>...", host, port);
	if (connected()) {
		if (connectedHost.equals(host) && connectedPort == port) {
			DT_DEBUG_PRINTLN_DEBUG( "TcpIotClient::connect() host<%s> port<%u>... already connected in <%lu>ms.", host,	port, millis() - start);
			return 1;
		}
		//wrong connection -> close socket
		stop();
	}
	long timeouMs = 120000;
	if (_initTcpIoTModem(timeouMs) && DTCOT_MODEM_SOCKET_CONNECTED == _modem->connectSocket(host, port)) {
		connectedHost = host;
		connectedPort = port;
		DT_DEBUG_PRINTLN_INFO( "TcpIotClient::connect() host<%s> port<%u>... done in <%lu>ms.", host,	port, millis() - start);
		return 1;
	}
	DT_DEBUG_PRINTLN_ERROR( "TcpIotClient::connect() host<%s> port<%u>... FAILED in <%lu>ms.", host,	port, millis() - start);
	return 0;
}

int TcpIotClient::connect(IPAddress ip, uint16_t port) {
	String ipStr = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::connect() ip<%s> port<%d>", ipStr.c_str(), port);
	return connect(ipStr.c_str(), port);
}

void TcpIotClient::stop() {
	DT_DEBUG_PRINTLN_INFO("TcpIotClient::stop()...");
	DTCOT_MODEM_STATUS status = _modem->closeSocket();
	DT_DEBUG_PRINTLN_INFO("TcpIotClient::stop()... done. <%d>", status);
}

uint8_t TcpIotClient::connected() {
	uint8_t rc = 0;
	if (!_modemInitialized) {
		DT_DEBUG_PRINTLN_ERROR(
				"TcpIotClient::connected() Modem is not initialized!");
	} else {
		rc = (DTCOT_MODEM_SOCKET_CONNECTED == _modem->isSocketConnected());
	}
	if (_lastPrintedConnectState != rc) {
		_lastPrintedConnectState = rc;
		DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::connected() <%u>", rc);
	}
	return rc;
}

size_t TcpIotClient::write(uint8_t b) {
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::write(uint8_t b)...");
	return write(&b, 1);
}

size_t TcpIotClient::write(const uint8_t *buf, size_t size) {
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::write buf[%d]...", size);
	unsigned long start = millis();
	size_t rc = size;
	DTCOT_MODEM_STATUS status = DTCOT_MODEM_SOCKET_NOT_CONNECTED;
	if (this->connected()) {
		status = _modem->txData(buf, size);
		if (status != DTCOT_MODEM_OK) {
			rc = -1;
		}
	}
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::write buf[%d]... done in <%lu>ms. status<%d> [%d]", size, millis() - start, status, rc);
	return rc;
}

int TcpIotClient::available() {
	int availableSize = 0;
	if (_modem->rxDataAvailable(availableSize) == DTCOT_MODEM_SOCKET_DATA_AVAILABLE) {
		return availableSize;
	}
	return 0;
}

int TcpIotClient::read() {
	uint8_t buf[1];
	if (read(buf, 1) == 1) {
		return (int)buf[0];
	}
	return 0;
}

int TcpIotClient::read(uint8_t* buf, size_t size) {
	int readSize = size;
	if (DTCOT_MODEM_OK != _modem->rxData(buf, readSize)) {
		readSize = -1;
	}
	return readSize;
}

TcpIotClient::operator bool() {
	return connected() != 0;;
}

int TcpIotClient::peek() {
	DT_DEBUG_PRINTLN_CRITICAL("TcpIotClient::peek() NOT YET IMPLEMENTED!");
	return -1;
}

void TcpIotClient::flush() {
	DT_DEBUG_PRINTLN_CRITICAL("TcpIotClient::flush() NOT YET IMPLEMENTED!");
}

//// Private Methods

bool TcpIotClient::_initTcpIoTModem(long &timeoutMs) {
	if (_modemInitialized) {
		DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::initTcpIoTModem() already done");
		return true;
	}

	TimeoutHelper toh("TcpIotClient::_initTcpIoTModem()", timeoutMs);
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::initTcpIoTModem() initializing modem...");

	if (toh.checkTimeout() || _modem->init() != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() Failed to initialize modem");
		return false;
	}

	String versionInfo;
	if (toh.checkTimeout() || _modem->getVersion(versionInfo) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() Failed to aquire modem version");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() modem version<%s>", versionInfo.c_str());

	String iccidInfo;
	if (toh.checkTimeout() || _modem->getICCID(iccidInfo) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() Failed to aquire modem ICCID! Is SIM card inserted?");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() ICCID<%s>", iccidInfo.c_str());

	String imeiInfo;
	if (toh.checkTimeout() || _modem->getIMEI(imeiInfo) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() Failed to aquire modem IMEI");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() IMEI<%s>", imeiInfo.c_str());

	if (toh.checkTimeout() || _modem->radioOn(true) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_ERROR("TcpIotClient::initTcpIoTModem() Failed to switch radio on");
		return false;
	}
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::initTcpIoTModem() radio is on");

	String imsiInfo;
	if (toh.checkTimeout() || _modem->getIMSI(imsiInfo) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_ERROR("TcpIotClient::initTcpIoTModem() Failed to aquire modem IMSI");
		return false;
	}
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::initTcpIoTModem() IMSI<%s>", imsiInfo.c_str());

	if (toh.checkTimeout() || _modem->connectApn(apn.c_str(), apnUser.c_str(), apnPwd.c_str(), ssl) != DTCOT_MODEM_APN_CONNECTED) {
		DT_DEBUG_PRINTLN_ERROR("TcpIotClient::initTcpIoTModem() Failed to connect APN");
		return false;
	}
	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::connectApn() GPRS connected");

	String csqInfo;
	DTCOT_MODEM_STATUS getCSQStatus = _modem->getCSQ(csqInfo);
	if (getCSQStatus == DTCOT_MODEM_SIGNAL_QUALITY_FAILED) {
		DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() waiting for signal..",	csqInfo.c_str());
		while (!toh.checkTimeout() && getCSQStatus == DTCOT_MODEM_SIGNAL_QUALITY_FAILED) {
			DT_DEBUG_PRINT(".");
			delay(500);
			getCSQStatus = _modem->getCSQ(csqInfo);
		}
		DT_DEBUG_PRINTLN("");
	}
	if (toh.checkTimeout() ||getCSQStatus != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_ERROR("TcpIotClient::initTcpIoTModem() Failed to aquire modem signal quality");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() Signal quality<%s>",	csqInfo.c_str());

	String localIp;
	if (toh.checkTimeout() || _modem->getLocalIp(localIp) != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTLN_ERROR("TcpIotClient::initTcpIoTModem() Failed to aquire local ip address");
		return false;
	}
	DT_DEBUG_PRINTLN_INFO("TcpIotClient::initTcpIoTModem() local ip address<%s>", localIp.c_str());

	DT_DEBUG_PRINTLN_DEBUG("TcpIotClient::initTcpIoTModem() initializing modem... done.");

	toh.checkTimeout();

	_modemInitialized = true;
	return _modemInitialized;
}

int TcpIotClient::init(long &timeoutMs) {
	if (!_initTcpIoTModem(timeoutMs)) {
		DT_DEBUG_PRINTLN_ERROR("TcpIotClient::init(): ERROR: Failed to initialize the modem");
		return 0;
	}
	return 1;
}

#endif /*ARDUINO_ARCH_AVR*/
