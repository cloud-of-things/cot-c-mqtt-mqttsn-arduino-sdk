/*
 * DTCotModemBase.cpp
 *
 *  Created on: 19.06.2018
 *      Author: hhenkens
 */

#include <protocols/DTCoTModemBase.h>
#include "utility/DTCoTDebugOutput.h"
#include <vector>

//#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_UART_TX_INFO
//#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_UART_RX_INFO
//#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_SENDANDPARSE_TX_INFO
//#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_SENDANDPARSE_RX_INFO

//namespace DTCoT {

DTCoTModemBase::DTCoTModemBase(Stream & serial, long serialSpeed, int resetPin,	void (*callback)())
:	stream(&serial),
	streamSpeed(serialSpeed),
	resetPin(resetPin), callback(callback), _dummyResponse(""), _interface(DTCOT_MODEM_TCP_UART_INTERFACE),
	_socket_num(DTCOT_MODEM_BASE_INVALID_SOCKET), _socket_err(0), _socket_host(""), _port(0),
	_serviceType(DTCOT_MODEM_SERVICE_TYPE_TCP),	_apnName(""), apnUser(""), apnPwd(""), _ssl(0), tmpSerialString(""),
	tmpRc(DTCOT_MODEM_UNITIALIZED), txUart(0), rxUart(0), txSocket(0), rxSocket(0),
	lastPrintedTxString(""), lastPrintedRxString(""), lastAvailable(0),
	rxUartFifo("UA", DTCOT_MODEM_BASE_UART_FIFO_SIZE), _modemIsDead(false)
{
}

DTCoTModemBase::~DTCoTModemBase() {
}

unsigned int DTCoTModemBase::_parseByRegexp(const char* src, unsigned int srcLength, const char* regexp, std::vector<String> &items) {
	unsigned long start = millis();
	items.clear();
	matchState.Target((char*) src, srcLength);
	char myMatchStateResult = matchState.Match(regexp, 0);
	switch (myMatchStateResult) {
	case REGEXP_MATCHED:
		matchState.GetMatch(matchStateBuf);
		for (int j = 0; j < matchState.level; j++) {
			String item = String(matchState.GetCapture(matchStateBuf, j));
			items.push_back(item);
		}
		break;
	case REGEXP_NOMATCH:
		break;
	default:
		DT_DEBUG_PRINTBUFFER_ERROR("DTCoTModemBase::_parseByRegexp() src", (uint8_t*)(char*)src, srcLength);
		DT_DEBUG_PRINTBUFFER_ERROR("DTCoTModemBase::_parseByRegexp() regexp", (uint8_t*)(char*)regexp, strlen(regexp));
		DT_DEBUG_PRINTLN_ERROR("DTCoTModemBase::_parseByRegexp() not expected regexp match state<%d>", myMatchStateResult);
		DT_DEBUG_PRINTLN_ERROR("DTCoTModemBase::_parseByRegexp()... done in rt<%lu>ms. count<%u>", millis() - start, items.size());
		break;
	}
	return items.size();
}

void DTCoTModemBase::_send(const uint8_t *buf, uint16_t size) {
	if(_modemIsDead) {
		DT_DEBUG_PRINTLN("_send: _modemIsDead");
		return;
	}
	for (unsigned int i = 0; i < size; i++) {
		stream->write(buf[i]);
		txUart++;
	}
#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_UART_TX_INFO
	DT_DEBUG_PRINTBUFFER_INFO("#TX#", (uint8_t*)buf, size);
#endif
}

uint16_t DTCoTModemBase::_read() {
	if(_modemIsDead) {
		DT_DEBUG_PRINTLN("_read(): _modemIsDead");
		return 0;
	}
	if(stream->available()) {
#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_UART_RX_INFO
		int addMode = rxUartFifo.length();
		uint8_t *added = rxUartFifo.content() + rxUartFifo.length();
		int addedLength = 0;
#endif
		while(stream->available()) {
			rxUartFifo.push((uint8_t)stream->read());
			delay(1);
			rxUart++;
#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_UART_RX_INFO
		}
	}
#else
			addedLength++;
		}
		if (addMode > 0) {
			DT_DEBUG_PRINTBUFFER_DEBUG(String("#RX# fifo[") + rxUartFifo.length() + "] appended", added , addedLength);
		}
		DT_DEBUG_PRINTBUFFER_DEBUG(String("#RX# fifo[") + rxUartFifo.length() + "]  content", rxUartFifo.content(), rxUartFifo.length());

	}
	if (rxUartFifo.length() > 0) {
		DT_DEBUG_PRINTLN_DEBUG("DTCoTModemBase::_read() fifo[%d]", rxUartFifo.length());
	}
#endif
	return rxUartFifo.length();
}

uint16_t DTCoTModemBase::_read(uint8_t *buf, uint16_t size, long timeout) {
	if(_modemIsDead) {
		DT_DEBUG_PRINTLN("_read(...): _modemIsDead");
		return 0;
	}
	uint16_t readSize = 0;
	unsigned long ts = millis();
	unsigned long end = ts + (unsigned long)timeout;

	while(readSize < size && ts <= end) {
		while (readSize < size && _read() > 0) {
			buf[readSize++] = rxUartFifo.pop();
		}
		ts = millis();
	}

#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_UART_RX_INFO
	if (readSize) {
		String m = String("#_read_U[") + size + "] TO<" + String(timeout) + ">";
		DT_DEBUG_PRINTBUFFER_INFO(m, buf, readSize);
	}
#endif
	return readSize;
}

uint16_t DTCoTModemBase::_read(String &strBuf, long timeout) {
	uint16_t readSize = 0;
	unsigned long ts = millis();
	unsigned long end = ts + (unsigned long)timeout;

	while(ts <= end) {
		while (_read() > 0) {
			strBuf += (char)rxUartFifo.pop();
			readSize++;
		}
		ts = millis();
	}

#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_UART_RX_INFO
	if (readSize) {
		String m = String("#_read_S[") + readSize + "] TO<" + String(timeout) + ">";
		DT_DEBUG_PRINTBUFFER_INFO(m, (uint8_t*)(char*)strBuf.c_str(), strBuf.length());
	}
#endif
	return readSize;
}

bool DTCoTModemBase::_contains(const uint8_t *buffer, const uint16_t bufferLength, const char *pattern, const uint16_t patternLength) {
	if(bufferLength && bufferLength >= patternLength) {
		char* pp = (char*)pattern;
		uint16_t pc = 0;
		for (uint16_t i = 0; i < bufferLength; i++) {
			if ((char)buffer[i] == *pp++) {
				if (++pc == patternLength) {
					return true;
				}
			} else {
				pp = (char*)pattern;
				pc = 0;
			}
		}
	}
	return false;
}


void DTCoTModemBase::_clearFifo() {
	rxUartFifo.clear();
}

DTCOT_MODEM_STATUS DTCoTModemBase::_readAndParseResponse(const char* comment, String& gmxSerialString, String& response,
		const char *regexp1, const char *regexp2, const char *regexpAfterOk, unsigned long endTs)
{
	if(_modemIsDead) {
		DT_DEBUG_PRINTLN("_readAndParseResponse: _modemIsDead");
		return DTCOT_MODEM_ERROR;
	}
	unsigned long start = millis();
	String msg = "";
	DTCOT_MODEM_STATUS returnCode = DTCOT_MODEM_AT_NO_RESPONSE;
	response = "";
	gmxSerialString = "";
	std::vector < String > resultItems;

	while (true) {
		// Read from Serial
		if (_read(gmxSerialString)) {
			if (regexp1 || regexp2) {
				// Parse Response by regexp1
				if (returnCode != DTCOT_MODEM_OK && regexp1 && _parseByRegexp(gmxSerialString.c_str(), gmxSerialString.length(), regexp1, resultItems)) {	// Parse Response by regexp1
					response = resultItems[0];
					msg = "regExp 1 DTCOT_MODEM_OK";
					returnCode = DTCOT_MODEM_OK;
				}
				// Parse Response by regexp2
				if (returnCode != DTCOT_MODEM_OK && regexp2 && _parseByRegexp(gmxSerialString.c_str(), gmxSerialString.length(), regexp2, resultItems)) { // Parse Response by regexp2
					response = resultItems[0];
					msg = "regExp 2 DTCOT_MODEM_OK";
					returnCode = DTCOT_MODEM_OK;
				}
				if (returnCode == DTCOT_MODEM_OK) {
					// Parse Response by regexpAfterOk
					if (regexpAfterOk) {
						if (_parseByRegexp(gmxSerialString.c_str(), gmxSerialString.length(), regexpAfterOk, resultItems)) {
							response = resultItems[0];
							msg = "regexpAfterOk DTCOT_MODEM_OK";
							break;
						}
					} else {
						break;
					}
				}
			} else {
				returnCode = DTCOT_MODEM_OK;
				break;
			}
			// Check for Errors
			if (_parseByRegexp(gmxSerialString.c_str(), gmxSerialString.length(), "(.*)\r\nERROR", resultItems)) {
				msg = "DTCOT_MODEM_AT_ERROR";
				returnCode = DTCOT_MODEM_AT_ERROR;
				break;
			}
		}
		if (millis() > endTs) {
			returnCode = DTCOT_MODEM_AT_TIMEOUT;
			if (gmxSerialString.length())
			{
				DT_DEBUG_PRINTBUFFER_DEBUG("DTCoTModemBase::_readAndParseResponse()      response", (uint8_t* )(char* )gmxSerialString.c_str(), gmxSerialString.length());
				DT_DEBUG_PRINTBUFFER_DEBUG("DTCoTModemBase::_readAndParseResponse()      regExp 1", (uint8_t* )(char* )regexp1, strlen(regexp1));
				DT_DEBUG_PRINTBUFFER_DEBUG("DTCoTModemBase::_readAndParseResponse()      regExp 2", (uint8_t* )(char* )regexp2, strlen(regexp2));
				DT_DEBUG_PRINTBUFFER_DEBUG("DTCoTModemBase::_readAndParseResponse() regexpAfterOk", (uint8_t* )(char* )regexpAfterOk, strlen(regexpAfterOk));

				DT_DEBUG_PRINTLN_CRITICAL("ERROR: Modem connection problem! Is mobile network available? Is SIM card ok? Is modem firmware up to date? (%s)", comment);

				_modemIsDead = true;
				msg = "incomplete response. Modem seems dead. DTCOT_MODEM_AT_TIMEOUT";
			}
			else {
				msg = "no response! DTCOT_MODEM_AT_TIMEOUT";
			}
			break;
		}
	}

	if (returnCode != DTCOT_MODEM_OK) {
		DT_DEBUG_PRINTBUFFER_DEBUG("DTCoTModemBase::_readAndParseResponse()      response", (uint8_t* )(char* )gmxSerialString.c_str(), gmxSerialString.length());
		DT_DEBUG_PRINTLN_INFO("DTCoTModemBase::_readAndParseResponse(%s)... FAILED in <%lu>ms. %s -> returnCode<%d>", comment,
				millis() - start, msg.c_str(), returnCode);
		if (_modemIsDead) {
			_dummyResponse = ""; // to break loop in <ModemBg96::_readOutSocketData()>
			DT_DEBUG_PRINTLN_ERROR("DTCoTModemBase::_readAndParseResponse() skip 'closeSocket'");
		}else{
			closeSocket();
		}
	}
	return returnCode;
}

bool DTCoTModemBase::_sendAndParse(const char* comment, String &gmxSerialString,
		DTCOT_MODEM_STATUS &returnCode, const uint8_t *in, unsigned int inLen,
		String &response, long waitOnOk, long timeout, bool trim,
		const char *regexp1, const char *regexp2, const char *regexpAfterOk)
{
	if(_modemIsDead) {
		DT_DEBUG_PRINTLN("_sendAndParse: _modemIsDead");
		return false;
	}

	unsigned long start = millis();

#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_SENDANDPARSE_TX_INFO
	String strIn((const char*)in);
	if (!lastPrintedTxString.equals(strIn)) {
		lastPrintedTxString = strIn;
		DT_DEBUG_PRINTBUFFER_INFO(String("#TX-") + comment, (uint8_t*)in, inLen);
	}
#endif

	_send(in, inLen);

	returnCode = _readAndParseResponse(comment, gmxSerialString, response, regexp1, regexp2, regexpAfterOk, start + timeout);

	if (trim) {
		response.trim();
	}
	int additionalReadSize = 0;

	if (waitOnOk && DTCOT_MODEM_OK == returnCode) {
		additionalReadSize = _read(response, waitOnOk);
	}

#ifdef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_DTCOTMODEMBASE_SENDANDPARSE_RX_INFO
	if (!lastPrintedRxString.equals(gmxSerialString)) {
		lastPrintedRxString = gmxSerialString;
		DT_DEBUG_PRINTBUFFER_INFO(String("RX-") + comment, (uint8_t*)(char*)gmxSerialString.c_str(), gmxSerialString.length());
		DT_DEBUG_PRINTBUFFER_INFO(String("RX-REGEX-") + comment, (uint8_t*)(char*)response.c_str(), response.length());
	}
#endif

	if (DTCOT_MODEM_OK != returnCode) {
		DT_DEBUG_PRINTLN_DEBUG("DTCoTModemBase::_sendAndParse() <%s> FAILED in <%lu>ms. uart<TX[%lu] RX[%lu] socket<TX[%lu] RX[%lu] additionalReadSize<%d> -> <%d>",
				comment, millis() - start, txUart, rxUart, txSocket, rxSocket, additionalReadSize, returnCode);
	}

	return DTCOT_MODEM_OK == returnCode;
}


bool DTCoTModemBase::_sendAndParse(const char* comment,	const uint8_t *in, unsigned int inLen,
		long waitOnOk, long timeout, bool trim,	const char *regexp1, const char *regexp2, const char *regexpAfterOk) {
	return _sendAndParse(comment, tmpSerialString, tmpRc, in, inLen, _dummyResponse, waitOnOk, timeout, trim, regexp1, regexp2, regexpAfterOk);
}

bool DTCoTModemBase::_sendAndParse(const char* comment,	String input,
		long waitOnOk, long timeout, bool trim, const char *regexp1,	const char *regexp2) {
	return _sendAndParse(comment, tmpSerialString, tmpRc, (uint8_t*)(char*)input.c_str(), input.length(), _dummyResponse, waitOnOk, timeout, trim, regexp1, regexp2);
}

bool DTCoTModemBase::_sendAndParse(const char* comment,	String input, String &response,
		long waitOnOk, long timeout, bool trim, const char *regexp1, const char *regexp2) {
	return _sendAndParse(comment, tmpSerialString, tmpRc, (uint8_t*)(char*)input.c_str(), input.length(), response, waitOnOk, timeout, trim, regexp1, regexp2);
}

//} /* namespace DTCoT */
