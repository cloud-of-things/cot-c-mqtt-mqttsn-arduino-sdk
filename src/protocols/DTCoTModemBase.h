/*
 * DTCotModemBase.h
 *
 *  Created on: 19.06.2018
 *      Author: hhenkens
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_H_

#include <Arduino.h>

#include "dt-arduino-iot-agent.h"
#include "DTCoTTcpIoTHardware.h"
#include "utility/Regexp.h"
#ifdef ARDUINO_ARCH_AVR
#include <ArduinoSTL.h>
#endif /*ARDUINO_ARCH_AVR*/

#include <vector>

#include "FiFo.h"

//namespace DTCoT {

enum DTCOT_MODEM_OPERATOR_ID_SHORT {
	DTCOT_MODEM_OPERATOR_ID_SHORT_TELEKOM = 26201
};


enum DTCOT_MODEM_SERVICE_TYPE {
	DTCOT_MODEM_SERVICE_TYPE_TCP = 0,
	DTCOT_MODEM_SERVICE_TYPE_UDP = 1
};


enum DTCOT_MODEM_STATUS {
	DTCOT_MODEM_UNITIALIZED = -1,
	DTCOT_MODEM_OK = 0,
	DTCOT_MODEM_APN_NOT_CONNECTED = 1,
	DTCOT_MODEM_APN_CONNECTED = 2,
	DTCOT_MODEM_WRONG_SIZE = 3,
	DTCOT_MODEM_SOCKET_NOT_CONNECTED = 4,
	DTCOT_MODEM_SOCKET_CONNECTING = 5,
	DTCOT_MODEM_SOCKET_CONNECTED = 6,
	DTCOT_MODEM_SOCKET_DATA_AVAILABLE = 7,
	DTCOT_MODEM_SOCKET_NO_DATA_AVAILABLE = 8,
	DTCOT_MODEM_SOCKET_ERROR = 9,
	DTCOT_MODEM_AT_NO_RESPONSE = 10,
	DTCOT_MODEM_AT_ERROR = 11,
	DTCOT_MODEM_AT_GENERIC_ERROR = 12,
	DTCOT_MODEM_AT_TIMEOUT = 13,
	DTCOT_MODEM_ON = 14,
	DTCOT_MODEM_OFF = 15,
	DTCOT_MODEM_ERROR = 16,
	DTCOT_MODEM_SIGNAL_QUALITY_FAILED = 17,
	DTCOT_MODEM_PACKETSERVICE_ATTACHED = 18,
	DTCOT_MODEM_PACKETSERVICE_NOT_ATTACHED = 19,
	DTCOT_MODEM_OPERATOR_SELECTED = 20,
	DTCOT_MODEM_OPERATOR_NOT_SELECTED = 21,
	DTCOT_MODEM_NBIOT_CONNECTED = 22,
	DTCOT_MODEM_NBIOT_NOT_CONNECTED = 23
};


enum DTCOT_MODEM_SOCKET_DATA_STATUS {
	DTCOT_MODEM_SOCKET_DATA_STATUS_UNKWOWN = -1,
	DTCOT_MODEM_SOCKET_DATA_STATUS_NO_DATA = 0,
	DTCOT_MODEM_SOCKET_DATA_STATUS_DATA_AVAILABLE = 1,
	DTCOT_MODEM_SOCKET_DATA_STATUS_CLOSED_DETECTED = 2
};

#define DTCOT_MODEM_BASE_INVALID_SOCKET 			(-1)

// CAL-95, int because #define leads to compiler error
const int DTCOT_MODEM_BASE_MATCH_STATE_BUFFER_SIZE = 1024;

#define DTCOT_MODEM_BASE_UART_FIFO_SIZE 			1024
#define DTCOT_MODEM_BASE_RECEIVE_FIFO_SIZE 			1024


class DTCoTModemBase {
public:
	DTCoTModemBase(Stream & serial, long serialSpeed, int resetPin, void (*callback)());
	virtual ~DTCoTModemBase() = 0;

	virtual DTCOT_MODEM_STATUS init() = 0;
	virtual DTCOT_MODEM_STATUS radioOn(bool on) = 0;

	/*TCP/IP support*/
	virtual DTCOT_MODEM_STATUS isApnConnected() = 0;
	virtual DTCOT_MODEM_STATUS connectApn(const char* apn, const char* user = NULL, const char* pwd = NULL, int ssl = 1) = 0;
	virtual DTCOT_MODEM_STATUS reconnectApn() = 0;
	virtual DTCOT_MODEM_STATUS disconnectApn() = 0;

	/*NB-IoT support*/
	virtual DTCOT_MODEM_STATUS connectNbiot(const char* apn, const char* user =	NULL, const char* pwd = NULL) = 0;
	virtual DTCOT_MODEM_STATUS reconnectNbiot() = 0;
	virtual DTCOT_MODEM_STATUS nbiotDisconnect() = 0;

	virtual DTCOT_MODEM_STATUS connectSocketUdp(String host, int port) = 0;

	virtual DTCOT_MODEM_STATUS connectSocket(String host, int port,	DTCOT_MODEM_SERVICE_TYPE serviceType = DTCOT_MODEM_SERVICE_TYPE_TCP) = 0;

	virtual DTCOT_MODEM_STATUS isSocketConnected() = 0;
	virtual DTCOT_MODEM_STATUS closeSocket() = 0;

	virtual DTCOT_MODEM_STATUS getVersion(String& version) = 0;
	virtual DTCOT_MODEM_STATUS getICCID(String& iccid) = 0;
	virtual DTCOT_MODEM_STATUS getIMEI(String& imei) = 0;
	virtual DTCOT_MODEM_STATUS getIMSI(String& imsi) = 0;
	virtual DTCOT_MODEM_STATUS getCSQ(String& csq) = 0;

	virtual DTCOT_MODEM_STATUS getLocalIp(String& localIp) = 0;

	virtual DTCOT_MODEM_STATUS txData(const uint8_t *buf, size_t size) = 0;
	virtual DTCOT_MODEM_STATUS rxDataAvailable(int &available) = 0;
	virtual DTCOT_MODEM_STATUS rxData(uint8_t *buf, int &size) = 0;

	virtual DTCOT_MODEM_SOCKET_DATA_STATUS _checkForURC(const uint8_t *buffer, uint16_t bufferLength) = 0;

protected:

	bool _sendAndParse(const char* comment,
			const uint8_t *in, unsigned int inLen,
			long waitOnOk = 0L, long timeout = 30000L, bool trim = true, const char *regexp1 = NULL, const char *regexp2 = "(.*)\r\nOK",
			const char *regexpAfterOk = NULL
			);

	bool _sendAndParse(const char* comment,
			String input,
			long waitOnOk = 0L, long timeout = 30000L, bool trim = true, const char *regexp1 = NULL, const char *regexp2 = "(.*)\r\nOK"
			);

	bool _sendAndParse(const char* comment,
			String input,
			String &response,
			long waitOnOk = 0L, long timeout = 30000L, bool trim = true, const char *regexp1 = NULL, const char *regexp2 = "(.*)\r\nOK"
			);

	uint16_t _read(uint8_t *buf, uint16_t size, long timeout = 5);
	uint16_t _read();

	bool _contains(const uint8_t *buffer, const uint16_t bufferLength, const char *pattern, const uint16_t patternLength);
	unsigned int _parseByRegexp(const char* src, unsigned int srcLength, const char* regexp, std::vector<String> &items);

private:


	bool _sendAndParse(const char* comment,
			String &gmxSerialString, DTCOT_MODEM_STATUS &returnCode,
			const uint8_t *in, unsigned int inLen,
			String &response,
			long waitOnOk = 0L, long timeout = 30000L, bool trim = true, const char *regexp1 = NULL, const char *regexp2 = "(.*)\r\nOK",
			const char *regexpAfterOk = NULL
			);

	DTCOT_MODEM_STATUS _readAndParseResponse(const char* comment, String& gmxSerialString, String& response,
			const char *regexp1, const char *regexp2, const char *regexpAfterOk, unsigned long endTs = 0);

	void _send(const uint8_t *buf, uint16_t size);

	uint16_t _read(String &strBuf, long timeout = 0);

	void _clearFifo();

// -- Members --
protected:
	MatchState matchState;
	char matchStateBuf[DTCOT_MODEM_BASE_MATCH_STATE_BUFFER_SIZE]; // large enough to hold expected string
	Stream * stream;
	long streamSpeed;
	int resetPin;
	void (*callback)();
	String _dummyResponse;
	byte _interface;
	int _socket_num;
	int _socket_err;
	String _socket_host;
	int _port;
	DTCOT_MODEM_SERVICE_TYPE _serviceType;
	String _apnName;
	String apnUser;
	String apnPwd;
	int _ssl;
	String tmpSerialString;
	DTCOT_MODEM_STATUS tmpRc;
	unsigned long txUart;
	unsigned long rxUart;
	unsigned long txSocket;
	unsigned long rxSocket;

	String lastPrintedTxString;
	String lastPrintedRxString;
	int lastAvailable;

	FiFo rxUartFifo;

	bool _modemIsDead; ///< somethomes modem is wired and needs shutdown
};

//} /* namespace DTCoT */

#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_H_ */
