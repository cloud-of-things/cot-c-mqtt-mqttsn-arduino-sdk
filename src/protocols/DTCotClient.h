/*
 * DTCotClient.h
 *
 *  Created on: 26.09.2018
 *      Author: A301620
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_H_


#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_PRINT_READ_INFO
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_RX_BUFFER_SIZE 128

#include "Arduino.h"
#include "Client.h"
#include "utility/DTCoTDebugOutput.h"

class DTCotClient: public Client {
public:

	DTCotClient(String type, Client *client)
	: myType(type), myClient(client), start(millis()), end(millis())
	  , _lastAvailable(0), _rxBufNextWrite(0), _rxBufSize(0)
	{
		_startInfo("DTCotClient()");
		instNumber = DTCotClient::instCounter++;
		_printMsgPrefix = String("#") + myType  + "#["+ String(instNumber) + "]#";
		_txMsg = _printMsgPrefix + String("#TX#");
		_rxMsg = _printMsgPrefix + String("#RX#") + String(LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_RX_BUFFER_SIZE) + "#";
		_printInfo();
	}

	virtual ~DTCotClient() {
		_startInfo("~DTCotClient()");
		_printInfo();
	}

	int connect(IPAddress ip, uint16_t port) {
		String msg = String("connect(") + String(ip) + "," + String(port) + ")";
		_startInfo(msg.c_str());
		int rc = myClient->connect(ip, port);
		_printInfo(rc);
		return rc;
	}

	int connect(const char *host, uint16_t port) {
		String msg = String("connect()") + String(host) + "," + String(port) + ")";
		_startInfo(msg.c_str());
		int rc = myClient->connect(host, port);
		_printInfo(rc);
		return rc;
	}

	size_t write(uint8_t value) {
		_startInfo("write(value)");
		size_t rc = myClient->write(value);
		_endInfo();
		DT_DEBUG_PRINTBUFFER_INFO(_txMsg + "1#", (uint8_t*)&value, rc);
		_printInfo(rc);
		return rc;
	}

	size_t write(const uint8_t *buf, size_t size) {
		String msg = String("write(buf,") + String(size) + ")";
		_startInfo(msg.c_str());
		size_t rc = myClient->write(buf, size);
		_endInfo();
		DT_DEBUG_PRINTBUFFER_INFO(_txMsg + "n#", (uint8_t*)buf, rc);
		_printInfo(rc);
		return rc;
	}

	int available() {
		_startInfo("available()");
		int rc = myClient->available();
		if (rc > _lastAvailable) {
			_printInfo(rc);
			_rxBufSize = rc;
			_rxBufNextWrite = 0;
		}
		_lastAvailable = rc;
		return rc;
	}

	int read() {
#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_PRINT_READ_INFO
		_startInfo("read()");
#endif
		int rc = myClient->read();
#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_PRINT_READ_INFO
		_endInfo();
		DT_DEBUG_PRINTBUFFER_INFO("#RX1#", (uint8_t*)&rc, 1);
		_printInfo(rc);
#endif
		if (_lastAvailable) {
			if (_rxBufNextWrite < _rxBufSize) {
				if (_rxBufNextWrite < LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_RX_BUFFER_SIZE) {
					_rxbuf[_rxBufNextWrite++] = (uint8_t)rc;
				}
			}
			if (_lastAvailable == 1) {
				DT_DEBUG_PRINTBUFFER_INFO(_rxMsg, (uint8_t*)_rxbuf, _rxBufNextWrite);
				_rxBufNextWrite = 0;
				_rxBufSize = 0;
			}
		}
		return rc;
	}

	int read(uint8_t *buf, size_t size) {
		String msg = String("read(buf,") + String(size) + ")";
		_startInfo(msg.c_str());
		int rc = myClient->read(buf, size);
		_endInfo();
		DT_DEBUG_PRINTBUFFER_INFO("#RXn#", (uint8_t*)buf, rc);
		_printInfo(rc);
		return rc;
	}

	int peek() {
		_startInfo("peek()");
		int rc = myClient->peek();
		_printInfo(rc);
		return rc;
	}

	void flush() {
		_startInfo("flush()");
		myClient->flush();
		_printInfo();
	}

	void stop() {
		_startInfo("stop()");
		myClient->stop();
		_printInfo();
	}

	uint8_t connected() {
		_startInfo("connected()");
		uint8_t rc = myClient->connected();
		if (!rc) {
			_printInfo(rc);
		}
		return rc;
	}

	operator bool() {
		_startInfo("operator bool");
		bool rc = myClient->connected() != 0;
		_printInfo(rc);
		return rc;
	}

private:

	void _startInfo(const char* msg) {
		start = millis();
		end = 0;
		this->msg = msg;
		delay(0);
	}

	void _endInfo() {
		if (!end) {
			end = millis();
		}
		delay(0);
	}

	void _printInfo() {
		_endInfo();
		DT_DEBUG_PRINTLN_INFO("%s%s done in <%lu>ms.", _printMsgPrefix.c_str(), msg, end - start);
		delay(0);
	}

	void _printInfo(int rc) {
		_endInfo();
		DT_DEBUG_PRINTLN_INFO("%s%s done in <%lu>ms. -> <%d>", _printMsgPrefix.c_str(), msg, end - start, rc);
		delay(0);
	}

	String myType;
	Client *myClient;
	unsigned long start;
	unsigned long end;
	const char* msg;
	int _lastAvailable;
	int _rxBufNextWrite;
	int _rxBufSize;
	uint8_t _rxbuf[LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_RX_BUFFER_SIZE];
	String _txMsg;
	String _rxMsg;
	String _printMsgPrefix;
	unsigned long instNumber;

	static unsigned long instCounter;
};

unsigned long DTCotClient::instCounter = 0;

#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTCLIENT_H_ */
