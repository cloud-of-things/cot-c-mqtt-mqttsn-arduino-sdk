/*
 * ModemBg96.h
 *
 *  Created on: 19.06.218
 *      Author: hhenkens
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_MODEMBG96_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_MODEMBG96_H_

#include <protocols/DTCoTModemBase.h>
#include "utility/DTCoTDebugOutput.h"
#include "utility/TimeoutHelper.h"

#define MODEM_BG96_URC_NUMBER 	2
#define MODEM_BG96_URC_LIST 	"\"closed\"", "\"recv\""
#define MODEM_BG96_URC_RC_LIST 	DTCOT_MODEM_SOCKET_DATA_STATUS_CLOSED_DETECTED, DTCOT_MODEM_SOCKET_DATA_STATUS_DATA_AVAILABLE

enum MODEM_BG96_ACCESSTECHNOLOGY {
	MODEM_BG96_ACCESSTECHNOLOGY_GSM	= 0,
	MODEM_BG96_ACCESSTECHNOLOGY_LTE_CAT_M1 = 8,
	MODEM_BG96_ACCESSTECHNOLOGY_LTE_CAT_NB1 = 9
};


class ModemBg96: public DTCoTModemBase {
public:

	ModemBg96(Stream & serial, long serialSpeed, int resetPin, void (*callback)(), uint16_t fetchSize) :
			DTCoTModemBase(serial, serialSpeed, resetPin, callback),
			_lastPrintedSocketConnectionState(DTCOT_MODEM_UNITIALIZED), rxFetchSize(fetchSize) {
		rxSocketFifo = new FiFo("RX", DTCOT_MODEM_BASE_RECEIVE_FIFO_SIZE);
	}

	virtual ~ModemBg96() {
		delete rxSocketFifo;
	}

	DTCOT_MODEM_STATUS init() {
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);

		DTCOT_MODEM_STATUS init_status = DTCOT_MODEM_OK;
	    pinMode(PIN_BG96_PWRKEY, OUTPUT);
		pinMode(resetPin, OUTPUT);

		_modemIsDead = false;

		((Uart*)stream)->begin(streamSpeed);
		_power(true);
		_printParameterInfo();

	//	if (init_status == DTCOT_MODEM_OK) {
	//		init_status = _atSpeed();
	//	}
		if (init_status == DTCOT_MODEM_OK) {
			init_status = _atZ();
		}
		if (init_status == DTCOT_MODEM_OK) {
			init_status = _atCmdFactoryDefault();
		}
		if (init_status == DTCOT_MODEM_OK) {
			init_status = _atCommTest();
		}
		if (init_status == DTCOT_MODEM_OK) {
			init_status = _atEcho(false);
		}

		return init_status;
	}

	DTCOT_MODEM_STATUS radioOn(bool on) {
		if (!on) {
			_sendAndParse("radioOn(off)", "AT+CFUN=0\r", 1000L, 15000L);
		} else {
			_sendAndParse("radioOn(on)", "AT+CFUN=1\r", 5000L, 15000L);
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS isApnConnected() {
		if (_sendAndParse("isApnConnected", "AT+QIACT?\r", 0, 10000)) {
			if (_dummyResponse.indexOf("+QIACT: 1") > -1) {
				return DTCOT_MODEM_APN_CONNECTED;
			} else {
				return DTCOT_MODEM_APN_NOT_CONNECTED;
			}
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS connectApn(const char* apn, const char* user = NULL, const char* pwd = NULL, int ssl = 1) {
		_apnName = apn;
		apnUser = user;
		apnPwd = pwd;
		_ssl = ssl;
		return reconnectApn();
	}

	DTCOT_MODEM_STATUS reconnectApn() {
		if (DTCOT_MODEM_APN_CONNECTED == isApnConnected()) {
			return DTCOT_MODEM_APN_CONNECTED;
		}
		tmpRc = _selectOperator(DTCOT_MODEM_OPERATOR_ID_SHORT_TELEKOM, MODEM_BG96_ACCESSTECHNOLOGY_GSM);
		if (tmpRc == DTCOT_MODEM_OPERATOR_SELECTED) {
			long toMs = 30000;
			tmpRc = _attachPacketService(toMs);
		}
		if (tmpRc == DTCOT_MODEM_PACKETSERVICE_ATTACHED) {
			tmpRc = disconnectApn();
		}
		if (tmpRc == DTCOT_MODEM_OK	&&
			_sendAndParse("configure PDP context",
						String("AT+QICSGP=1,1,\"") + _apnName + String("\",\"")	+ apnUser + String("\",\"") + apnPwd + String("\",1\r"),
						0, 60000) &&
			_sendAndParse("activate PDP context", String("AT+QIACT=1\r"), 0, 150000)) {
		}
		if (tmpRc == DTCOT_MODEM_OK) {
			DT_DEBUG_PRINTLN_INFO("ModemBg96::reconnectApn()... done. -> <%d>", tmpRc);
			return DTCOT_MODEM_APN_CONNECTED;
		}
		DT_DEBUG_PRINTLN_ERROR("ModemBg96::reconnectApn()... FAILED! -> <%d>", tmpRc);
		return DTCOT_MODEM_APN_NOT_CONNECTED;
	}

	DTCOT_MODEM_STATUS reconnectNbiot()
	{
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
		if(DTCOT_MODEM_PACKETSERVICE_ATTACHED == _isPacketServiceAttached())
		{
			nbiotDisconnect();
		}
		if(_reset() != DTCOT_MODEM_ON) {
			DT_DEBUG_PRINTLN_ERROR("Modem BG96: reset failed");
		}
		return connectNbiot(_apnName.c_str(), apnUser.c_str(), apnPwd.c_str());
	}



	DTCOT_MODEM_STATUS disconnectApn() {
		_sendAndParse("disconnectApn",	"AT+QIDEACT=1\r", 0, 40000);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS isSocketConnected() {
		if (_socket_num == DTCOT_MODEM_BASE_INVALID_SOCKET) {
			return DTCOT_MODEM_SOCKET_NOT_CONNECTED;
		}
		return DTCOT_MODEM_SOCKET_CONNECTED;
	}

	DTCOT_MODEM_STATUS connectSocket(String host, int port, DTCOT_MODEM_SERVICE_TYPE serviceType) {
		unsigned long start = millis();
		tmpRc = isSocketConnected();
		if (DTCOT_MODEM_SOCKET_CONNECTED == tmpRc && _socket_host.equals(host)	&& _port == port && _serviceType == serviceType) {
			return DTCOT_MODEM_SOCKET_CONNECTED;
		}
		DT_DEBUG_PRINTLN_DEBUG("ModemBg96::connectSocket() host<%s> port<%d> serviceType<%d> ssl<%d>...", host.c_str(), port, serviceType, _ssl);
		if (DTCOT_MODEM_SOCKET_CONNECTED == tmpRc) {
			DT_DEBUG_PRINTLN_INFO("ModemBg96::connectSocket() Parameter changed! Close socket...");
			closeSocket();
		}

		tmpRc = reconnectApn();

		if (tmpRc != DTCOT_MODEM_APN_CONNECTED) {
			DT_DEBUG_PRINTLN_ERROR("ModemBg96::connectSocket() host<%s> port<%d> serviceType<%d> ssl<%d>... done in <%lu>ms. FAILED! rc<%d>",
					host.c_str(), port, serviceType, _ssl, millis() - start, tmpRc);
			return tmpRc;
		}

		_socket_host = host;
		_port = port;
		_serviceType = serviceType;
		_socket_num = DTCOT_MODEM_BASE_INVALID_SOCKET;

		if (_ssl) {
			if (_sendAndParse("configure sslversion", String("AT+QSSLCFG=\"sslversion\",1,3\r"),
					0, 20000)	&&
				_sendAndParse("connect ssl socket", String("AT+QSSLOPEN=1,1,0,\"") + host + String("\",") + String(port) + "\r",
					1000, 150000, true, "QSSLOPEN:(.*,.*)", NULL)) {

				int delPos = _dummyResponse.indexOf(',');
				_socket_err = atoi(_dummyResponse.substring(delPos + 1).c_str());
				if (_socket_err != 0) {
					tmpRc = DTCOT_MODEM_SOCKET_ERROR;
				} else {
					_socket_num = atoi(_dummyResponse.substring(0, delPos - 1).c_str());
					tmpRc = DTCOT_MODEM_SOCKET_CONNECTED;
				}

			}
		} else {
			String st( (_serviceType == DTCOT_MODEM_SERVICE_TYPE_UDP) ? "UDP" : "TCP");
			if (_sendAndParse("connect socket",
					String("AT+QIOPEN=1,0,\"") + st + String("\",\"") + host + String("\",") + String(port) + ",0,0\r",
					1000, 150000, true, "QIOPEN:(.*,.*)", NULL)) {

				int delPos = _dummyResponse.indexOf(',');
				_socket_err = atoi(_dummyResponse.substring(delPos + 1).c_str());
				if (_socket_err != 0) {
					tmpRc = DTCOT_MODEM_SOCKET_ERROR;
				} else {
					_socket_num = atoi(_dummyResponse.substring(0, delPos - 1).c_str());
					tmpRc = DTCOT_MODEM_SOCKET_CONNECTED;
				}

			}
		}

		if (tmpRc != DTCOT_MODEM_SOCKET_CONNECTED) {
			DT_DEBUG_PRINTLN_ERROR(
					"ModemBg96::connectSocket() host<%s> port<%d> serviceType<%d> ssl<%d>... done in <%lu>ms. socket_err<%d> socket_num<%d> -> rc<%d>",
					_socket_host.c_str(), _port, _serviceType, _ssl, millis() - start, _socket_err, _socket_num, tmpRc);
		} else {
			DT_DEBUG_PRINTLN_INFO(
					"ModemBg96::connectSocket() host<%s> port<%d> serviceType<%d> ssl<%d>... done in <%lu>ms. socket_err<%d> socket_num<%d> -> rc<%d>",
					_socket_host.c_str(), _port, _serviceType, _ssl, millis() - start, _socket_err, _socket_num, tmpRc);
		}

		return tmpRc;
	}

	DTCOT_MODEM_STATUS closeSocket() {
		DT_DEBUG_PRINTLN_DEBUG("ModemBg96::closeSocket()...");
		tmpRc = isSocketConnected();
		if (DTCOT_MODEM_SOCKET_NOT_CONNECTED == tmpRc) {
			return DTCOT_MODEM_SOCKET_NOT_CONNECTED;
		}
		if (_ssl) {
			if (_sendAndParse("closeSocket", String("AT+QSSLCLOSE=") + String(_socket_num) + ",10\r", 0, 75000, true)) {
				_socket_num = DTCOT_MODEM_BASE_INVALID_SOCKET;
			}
		} else {
			if (_sendAndParse("closeSocket", String("AT+QICLOSE=") + String(_socket_num) + "\r", 0, 75000, true)) {
				_socket_num = DTCOT_MODEM_BASE_INVALID_SOCKET;
			}
		}
		if (DTCOT_MODEM_OK == tmpRc) {
			tmpRc = DTCOT_MODEM_SOCKET_NOT_CONNECTED;
		}
		DT_DEBUG_PRINTLN_DEBUG("ModemBg96::closeSocket()... done. <%d>", tmpRc);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS getVersion(String& version) {
		_sendAndParse("getVersion", "AT+CGMR\r", version, 0L, 500L, true, "\r\n(.*)\r\nOK");
		return tmpRc;
	}

	DTCOT_MODEM_STATUS getICCID(String& iccid) {
		_sendAndParse("getICCID", "AT+CCID\r", iccid, 0L, 500L, true, ".*+CCID:([^\r\n].*)OK");
		return tmpRc;
	}

	DTCOT_MODEM_STATUS getIMEI(String& imei) {
		_sendAndParse("getIMEI", "AT+CGSN\r", imei, 0L,	500L, true, "\r\n(.*)\r\nOK");
		return tmpRc;
	}

	/** Get IMSI of the inserted SIM card.*/
	DTCOT_MODEM_STATUS getIMSI(String& imsi) {
		_sendAndParse("getIMSI", "AT+CIMI\r", imsi, 0L,	500L);
		DT_DEBUG_PRINTLN_DEBUG("ModemBg96::getIMSI = <%s>", imsi.c_str());
		return tmpRc;
	}

	DTCOT_MODEM_STATUS getCSQ(String& csq) {
		_sendAndParse("getCSQ", "AT+CSQ\r", csq, 0L, 500L, true, ".*+CSQ:(.*),.*OK");
		if (tmpRc == DTCOT_MODEM_OK && csq.equals("99")) {
			tmpRc = DTCOT_MODEM_SIGNAL_QUALITY_FAILED;
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS getLocalIp(String& localIp) {
		_sendAndParse("getLocalIp", "AT+QIACT?\r", localIp,	0, 2000, true);
		int start = localIp.indexOf("\"");
		if (start > -1) {
			localIp = localIp.substring(start + 1, localIp.length() - 1);
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS txData(const uint8_t *buf, size_t size) {
		tmpRc = isSocketConnected();
		if (DTCOT_MODEM_SOCKET_NOT_CONNECTED == tmpRc) {
			DT_DEBUG_PRINTLN_ERROR("ModemBg96::txData() not connected!");
			return tmpRc;
		}
		if (_ssl) {
			if (_sendAndParse("set tx ssl data size",
					String("AT+QSSLSEND=") + String(_socket_num) + ","	+ String(size) + "\r",
					0, 75000, true, ".*(>)") &&
				_sendAndParse("tx ssl data", (const uint8_t*)buf, size, 0, 5000,	true, ".*(SEND OK).*")
			) {
				txSocket += (unsigned long)size;
				if (tmpSerialString.indexOf("+QSSLURC: \"closed\"") > -1) {
					DT_DEBUG_PRINTBUFFER_DEBUG("ModemBg96::txData(ssl)", (uint8_t *)(char*)tmpSerialString.c_str(), tmpSerialString.length());
					DT_DEBUG_PRINTLN_INFO("ModemBg96::txData(ssl) closed socket detected!");
					closeSocket();
					tmpRc = DTCOT_MODEM_SOCKET_ERROR;
				}
			}
		} else {
			if (_sendAndParse("set tx data size",
					String("AT+QISEND=") + String(_socket_num) + "," + String(size) + "\r",
					0, 75000, true, ".*(>)") &&
				_sendAndParse("tx data", (const uint8_t*)buf, size, 0, 5000, true, ".*(SEND OK).*")
			) {
				txSocket += (unsigned long)size;
				if (tmpSerialString.indexOf(", CLOSE") > -1) {
					DT_DEBUG_PRINTBUFFER_DEBUG("ModemBg96::txData()", (uint8_t *)(char*)tmpSerialString.c_str(), tmpSerialString.length());
					DT_DEBUG_PRINTLN_INFO("ModemBg96::txData() closed socket detected!");
					closeSocket();
					tmpRc = DTCOT_MODEM_SOCKET_ERROR;
				}
			}
		}
		return tmpRc;
	}

	DTCOT_MODEM_SOCKET_DATA_STATUS _checkForURC(const uint8_t *buffer, uint16_t bufferLength) {
		if (!bufferLength) {
			return DTCOT_MODEM_SOCKET_DATA_STATUS_NO_DATA;
		}
		DTCOT_MODEM_SOCKET_DATA_STATUS rc = DTCOT_MODEM_SOCKET_DATA_STATUS_UNKWOWN;
		String pattern = String((_ssl) ? "+QSSLURC: " : "+QIURC: ") + "([^\r\n]*),";
		if (_parseByRegexp((char*)buffer, bufferLength, pattern.c_str(), urcItems)) {
			for (int i = 0; i < urcCount; i++) {
				if (urcItems[0].equals(urcPattern[i])) {
					rc = urcReturnCodes[i];
					break;
				}
			}
		}
		if (rc == DTCOT_MODEM_SOCKET_DATA_STATUS_CLOSED_DETECTED) {
			DT_DEBUG_PRINTLN_INFO("ModemBg96::_checkForURC() closed socket detected!");
			closeSocket();
		}
		DT_DEBUG_PRINTBUFFER_DEBUG(String("ModemBg96::_checkForURC() rc<") + String(rc) + "> ", buffer, bufferLength);
		return rc;
	}

	bool _isSocketDataAvailable() {
		return _read() && _checkForURC(rxUartFifo.content(), rxUartFifo.length()) == DTCOT_MODEM_SOCKET_DATA_STATUS_DATA_AVAILABLE;
	}

	int _readOutSocketData() {
		int readOut = 0;
		if (!isSocketConnected() || !_isSocketDataAvailable()) {
			return readOut;
		}
		DT_DEBUG_PRINTLN_INFO("ModemBg96::_readOutSocketData()...");
		int fetched = 0;
		String RECV = (_ssl) ? "+QSSLRECV" : "+QIRD";
		String TERM = "\r\n\r\nOK\r\n";
		String receiveCmd = String("AT") + RECV + String("=") + String(_socket_num) + "," + String(rxFetchSize) + "\r";
		String regexpPattern(RECV + ": (%d+)\r\n.*" + TERM);
		while(true) {
			if (_sendAndParse("readSocket", receiveCmd,	0, 5000, false, regexpPattern.c_str())) {
				if (_checkForURC((const uint8_t*)tmpSerialString.c_str(), tmpSerialString.length()) == DTCOT_MODEM_SOCKET_DATA_STATUS_CLOSED_DETECTED) {
					readOut = -1;
					break;
				} else {
					fetched = atoi(_dummyResponse.c_str());
					if (fetched > 0) {
						readOut += fetched;
						String search = String(RECV+": ") + String(fetched) + "\r\n";
						int offset = tmpSerialString.indexOf(search) + search.length();
						char *p = ((char*) tmpSerialString.c_str()) + offset;
						int length = tmpSerialString.length() - offset - TERM.length();
						if (fetched > length) {
							DT_DEBUG_PRINTLN_CRITICAL("ModemBg96::_readOutSocketData() wrong fetch size! length: actual<%d> expected<%d>", length, fetched);
							readOut = -2;
							fetched = length;
							break;
						}
						for (int i = 0; i < fetched; i++) {
							rxSocketFifo->push((uint8_t)*p++);
							rxSocket++;
						}
					} else {
						break;
					}
				}
			}
		}
		DT_DEBUG_PRINTBUFFER_INFO(String("ModemBg96::_readOutSocketData()... done. fetchSize<") + rxFetchSize +	"> to fetch[" + readOut + "] -> fetched", rxSocketFifo->content(), rxSocketFifo->length());
		return readOut;
	}


	DTCOT_MODEM_STATUS rxDataAvailable(int &available)
	{
		_readOutSocketData();
		available = rxSocketFifo->length();
		if (available > 0) {
			return DTCOT_MODEM_SOCKET_DATA_AVAILABLE;
		}
		return DTCOT_MODEM_SOCKET_NO_DATA_AVAILABLE;
	}

	DTCOT_MODEM_STATUS rxData(uint8_t *buf, int &size) {
		int sizeIn = size;
		size = 0;
		int available = 0;
		tmpRc = rxDataAvailable(available);
		if (tmpRc == DTCOT_MODEM_SOCKET_DATA_AVAILABLE)
		{
			if (sizeIn > available) {
				size = available;
			} else {
				size = sizeIn;
			}
			for (int i = 0; i < size; i++) {
				buf[i] = rxSocketFifo->pop();
			}
			tmpRc = DTCOT_MODEM_OK;
		}
		return tmpRc;
	}



	DTCOT_MODEM_STATUS connectSocketUdp(String host, int port)
	{
		DTCOT_MODEM_SERVICE_TYPE serviceType = DTCOT_MODEM_SERVICE_TYPE_UDP;

		DT_DEBUG_PRINTLN_DEBUG("ModemBg96::connectSocketUdp() host<%s> port<%d> serviceType<%d> ssl<%d>...", host.c_str(), port, serviceType, _ssl);

		tmpRc = isSocketConnected();
		if (DTCOT_MODEM_SOCKET_CONNECTED == tmpRc && _socket_host.equals(host)	&& _port == port && _serviceType == serviceType) {
			DT_DEBUG_PRINTLN_DEBUG("ModemBg96::connectSocketUdp()... already connected!");
			return DTCOT_MODEM_SOCKET_CONNECTED;
		}
		if (DTCOT_MODEM_SOCKET_CONNECTED == tmpRc) {
			closeSocket();
		}

		_socket_host = host;
		_port = port;
		_serviceType = serviceType;
		_ssl = 0 /*disabled*/;
		_socket_num = DTCOT_MODEM_BASE_INVALID_SOCKET;

		String st( (_serviceType == DTCOT_MODEM_SERVICE_TYPE_UDP) ? "UDP" : "TCP");

		if (_sendAndParse("connect socket",
				String("AT+QIOPEN=1,0,\"") + st + String("\",\"") + host + String("\",") + String(port) + ",0,0\r",
				0, 150000, true, "QIOPEN:(.*,.*)", NULL)) {

			int delPos = _dummyResponse.indexOf(',');
			_socket_err = atoi(_dummyResponse.substring(delPos + 1).c_str());
			if (_socket_err != 0) {
				tmpRc = DTCOT_MODEM_SOCKET_ERROR;
			} else {
				_socket_num = atoi(_dummyResponse.substring(0, delPos - 1).c_str());
				tmpRc = DTCOT_MODEM_SOCKET_CONNECTED;
			}

		}

		DT_DEBUG_PRINTLN_DEBUG("ModemBg96::connectSocketUdp() done. socket_num<%d> socket_err<%d> -> rc<%d>",
				_socket_num, _socket_err, tmpRc);
		return tmpRc;
	}



	DTCOT_MODEM_STATUS connectNbiot(const char* apn, const char* user, const char* pwd)
	{
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
		DT_DEBUG_PRINTLN_INFO("ModemBg96 config Nbiot (apn: <%s> user: <%s> pwd: <%s>)",
										apn, user, pwd);
		/*read IMSI for comparison*/
		String myIMSI;
		getIMSI(myIMSI);
		DT_DEBUG_PRINTLN_INFO("Device IMSI = <" + myIMSI +">");

		/*verbose modem debugging*/
		_sendAndParse("verboseDbg", String("AT+CMEE=2\r"), 0, 2000L);
		/*(prelimary) disable power saving mode*/
		_sendAndParse("PSM", String("AT+CPSMS=0\r"), 0, 2000L);
		radioOn(false);
		/*search prio to favour NBIOT*/
		String prio = "030102";			/*Search prio: NBIOT > GSM > LTE Cat M1*/
		_setSearchPrio(prio);
		_setScanMode(3);					/*LTE only*/
		_setNetworkCat(1);				/*LTE Cat NB1*/
		_setNetworkBand(80);				/*Band 8: Telekom / 1nce*/
		radioOn(true);

		_sendAndParse("reset TCP/IP context", String("AT+QIDEACT=1\r"), 500, 1000);
		if (_sendAndParse("configure TCP/IP context",
				String("AT+QICSGP=1,1,\"") + apn + String("\",\"")	+ user + String("\",\"")
					+ pwd + String("\",1\r"), 0, 60000)) {
			DT_DEBUG_PRINTLN_DEBUG("PDP context configured");
		} else {
			DT_DEBUG_PRINTLN_ERROR("PDP context FAILED");
			return DTCOT_MODEM_NBIOT_NOT_CONNECTED;
		}

		/*wait for connection with NBIOT*/
		DT_DEBUG_PRINTLN_INFO("Connecting to NBIoT network ...");
		while (DTCOT_MODEM_PACKETSERVICE_ATTACHED != _isPacketServiceAttached())
		{
			DT_DEBUG_PRINT(".");
			delay(1000);
			_selectOperator(DTCOT_MODEM_OPERATOR_ID_SHORT_TELEKOM, MODEM_BG96_ACCESSTECHNOLOGY_LTE_CAT_NB1);
		}

		DT_DEBUG_PRINTLN_INFO("NBIoT successfully connected.");

		/*set APN*/
		_sendAndParse("apnCfg", String("AT+CGDCONT=1,\"IP\",\"") + String(apn) + "\"\r", 0, 2000L);

		return DTCOT_MODEM_NBIOT_CONNECTED;
	}



	DTCOT_MODEM_STATUS nbiotDisconnect() {
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);

		if(_reset() != DTCOT_MODEM_ON) {
			DT_DEBUG_PRINTLN_ERROR("Modem BG96: reset failed");
		}
		return DTCOT_MODEM_UNITIALIZED;
	}

private:

	//private methods

	/*Set search prio to nbiot ("03" LTE Cat NB1, "01" GSM, "02" LTE Cat M1)*/
	DTCOT_MODEM_STATUS _setSearchPrio(String &prio)
	{
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
		_sendAndParse("setSearchPrio", String("AT+QCFG=\"nwscanseq\", ") + prio + "\r", 0L, 15000L);
		return tmpRc;
	}

	/*Set search prio to nbiot (3: LTE only, 1: GSM only, 0: auto/any)*/
	DTCOT_MODEM_STATUS _setScanMode(int mode)
	{
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
		_sendAndParse("setScanMode", String("AT+QCFG=\"nwscanmode\", ") + String(mode) + "\r", 0L, 15000L);
		return tmpRc;
	}


	/*Set network category (1: LTE Cat NB1, 2: LTE Cat M1 and LTE Cat NB1, 0: LTE Cat M1)*/
	DTCOT_MODEM_STATUS _setNetworkCat(int mode)
	{
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
		_sendAndParse("setNetworkCat", String("AT+QCFG=\"iotopmode\", ") + String(mode) + "\r", 0L, 15000L);
		return tmpRc;
	}


	/*Set network band for LTE CAT NB-IoT (80: Band 8 (Telekom, 1nce); 80000: Band 20 (Vodafone))*/
	DTCOT_MODEM_STATUS _setNetworkBand(int mode)
	{
		DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
		_sendAndParse("setNetworkBand", String("AT+QCFG=\"band\", 0, 1, ") + String(mode) + "\r", 0L, 15000L);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _atSpeed() {
		_sendAndParse("AtSpeed", String("AT+IPR=") + String(streamSpeed) + ";&W\r", 1000L);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _atCommTest() {
		_sendAndParse("AtCommTest", "AT\r", 1000L);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _atEcho(bool on) {
		_sendAndParse("AtEcho", String("ATE") + String(on) + "\r", 1000L);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _atZ() {
		_sendAndParse("AtZ", "ATZ0\r", 1000L);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _atCmdFactoryDefault() {
		_sendAndParse("AtCmdFactoryDefault", "AT&F0\r", 1000L);
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _uartSpeedCheckAndReset() {
		if(_reset() == DTCOT_MODEM_ON) {
			return DTCOT_MODEM_ON;
		}
		long baudRates[12] = {19200, 115200, 921600, 460800, 230400, 76800, 57600, 38400, 31250, 28800, 14400, 9600};
		for (int i = 0; i < 12; i++) {
			streamSpeed = baudRates[i];
			DT_DEBUG_PRINTLN_ERROR("_uartSpeedCheck()... switch uart speed to <%ld>", streamSpeed);
			((Uart*)stream)->begin(streamSpeed);
			if(_reset() == DTCOT_MODEM_ON) {
				return DTCOT_MODEM_ON;
			}
		}
		return DTCOT_MODEM_ERROR;
	}

	DTCOT_MODEM_STATUS _isOperatorSelected(DTCOT_MODEM_OPERATOR_ID_SHORT operatorIdShort, MODEM_BG96_ACCESSTECHNOLOGY accessTechnology) {
		if (_sendAndParse("set operator show format", "AT+COPS=3,2\r", 0, 5000)
			&&
			_sendAndParse("is operator selected", "AT+COPS?\r", 0, 5000, true, "COPS: (.*)\r\nOK", NULL)
		) {
			String pattern = String("0,2,\"") + String(operatorIdShort) + "\"," + String(accessTechnology);
			if (_dummyResponse.equals(pattern)) {
				tmpRc = DTCOT_MODEM_OPERATOR_SELECTED;
			} else {
				tmpRc =_removeOperator();
			}
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _removeOperator() {
		if (_sendAndParse("remove operator", "AT+COPS=2\r", 0, 60000)) {
			tmpRc = DTCOT_MODEM_OPERATOR_NOT_SELECTED;
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _selectOperator(DTCOT_MODEM_OPERATOR_ID_SHORT operatorIdShort, MODEM_BG96_ACCESSTECHNOLOGY accessTechnology) {
		tmpRc = _isOperatorSelected(operatorIdShort, accessTechnology);
		if (tmpRc != DTCOT_MODEM_OPERATOR_SELECTED
			&&
			_sendAndParse("select operator", String("AT+COPS=1,2,\"") + String(operatorIdShort) + "\"," + String(accessTechnology) + '\r', 1000, 60000)) {
			tmpRc = DTCOT_MODEM_OPERATOR_SELECTED;
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _isPacketServiceAttached() {
		if (_sendAndParse("_isPacketServiceAttached",	"AT+CGATT?\r", 0, 2000, true, "CGATT: (.*)\r\nOK", NULL)) {
			if (_dummyResponse.equals("1")) {
				tmpRc = DTCOT_MODEM_PACKETSERVICE_ATTACHED;
			} else {
				tmpRc = DTCOT_MODEM_PACKETSERVICE_NOT_ATTACHED;
			}
		}
		return tmpRc;
	}

	DTCOT_MODEM_STATUS _attachPacketService(long &timeoutMs) {
		TimeoutHelper toh("ModemBg96::_attachPacketService()", timeoutMs);
		if (_sendAndParse("_attachPacketService", "AT+CGATT=1\r", 1000, 60000)) {
			while(!toh.checkTimeout() && tmpRc != DTCOT_MODEM_PACKETSERVICE_ATTACHED) {
				tmpRc = _isPacketServiceAttached();
			}
		} else {
			tmpRc = DTCOT_MODEM_PACKETSERVICE_NOT_ATTACHED;
		}
		return tmpRc;
	}

	bool _waitFor(const char* caller, String toWaitFor, long timeoutMs = 10000) {
		String msg = String("ModemBg96::_waitFor(") + caller + ")";
		if(_modemIsDead) {
			DT_DEBUG_PRINTLN("_waitFor(...): _modemIsDead");
			return false;
		}
		TimeoutHelper toh(msg.c_str(), timeoutMs);
		unsigned int count = 0;
		uint8_t s[1];
		while(!toh.checkTimeout()) {
			if (_read(s, 1) && toWaitFor[count] == (char)s[0] && ++count == toWaitFor.length()) {
				return true;
			}
		}
		return false;
	}

	DTCOT_MODEM_STATUS _power(bool on) {
		if (on) {
			digitalWrite(resetPin, LOW);
			delay(500);
			digitalWrite(PIN_BG96_PWRKEY, HIGH);
			delay(500);
			if(_uartSpeedCheckAndReset() != DTCOT_MODEM_ON) {
				DT_DEBUG_PRINTLN_ERROR("ModemBg96::_power(%d)... DTCOT_MODEM_ERROR", on);
				return DTCOT_MODEM_ERROR;
			}
			DT_DEBUG_PRINTLN_DEBUG("ModemBg96::_power(%d)... DTCOT_MODEM_ON", on);
			return DTCOT_MODEM_ON;
		}
		digitalWrite(PIN_BG96_PWRKEY,LOW);
		delay(500);
		digitalWrite(resetPin, HIGH);
		delay(500);
		DT_DEBUG_PRINTLN_DEBUG("ModemBg96::_power(%d)... DTCOT_MODEM_OFF", on);
		return DTCOT_MODEM_OFF;
	}

	DTCOT_MODEM_STATUS _reset() {
		digitalWrite(resetPin, LOW);
		delay(200);
		digitalWrite(resetPin, HIGH);
		delay(200);
		digitalWrite(resetPin, LOW);
		if (_waitFor("_reset", "RDY")) {
			DT_DEBUG_PRINTLN_DEBUG("ModemBg96::_modemReset()... done. DTCOT_MODEM_ON");
			return DTCOT_MODEM_ON;
		}
		DT_DEBUG_PRINTLN_ERROR("ModemBg96::_modemReset()... done. DTCOT_MODEM_ERROR!");
		return DTCOT_MODEM_ERROR;
	}

	void _printParameterInfo() {
		DT_DEBUG_PRINTLN_DEBUG("## ModemBg96 DTCOT_MODEMBG96_RECEIVE_BUFFER_SIZE <%d>", DTCOT_MODEM_BASE_RECEIVE_FIFO_SIZE);
		DT_DEBUG_PRINTLN_DEBUG("## ModemBg96 DTCOT_MODEMBG96_RECEIVE_FETCH_SIZE  <%d>", rxFetchSize);
		_sendAndParse("check flow control", "AT+IFC?\r", _dummyResponse);
		DT_DEBUG_PRINTBUFFER_DEBUG("## ModemBg96 flow control flag", (uint8_t *)(char*)_dummyResponse.c_str(), _dummyResponse.length());
		_sendAndParse("check frame data", "AT+ICF?\r", _dummyResponse);
		DT_DEBUG_PRINTBUFFER_DEBUG("## ModemBg96 frame data flag", (uint8_t *)(char*)_dummyResponse.c_str(), _dummyResponse.length());
		_sendAndParse("check baud rate", "AT+IPR?\r", _dummyResponse);
		DT_DEBUG_PRINTBUFFER_DEBUG("## ModemBg96 uart baud rate", (uint8_t *)(char*)_dummyResponse.c_str(), _dummyResponse.length());
	}

	//private variables

	FiFo *rxSocketFifo;
	DTCOT_MODEM_STATUS _lastPrintedSocketConnectionState;
	uint16_t rxFetchSize;

	const int urcCount = MODEM_BG96_URC_NUMBER;
	const char* urcPattern[MODEM_BG96_URC_NUMBER] = { MODEM_BG96_URC_LIST };
	const DTCOT_MODEM_SOCKET_DATA_STATUS urcReturnCodes[MODEM_BG96_URC_NUMBER] = { MODEM_BG96_URC_RC_LIST };
	std::vector<String> urcItems;

};


#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_MODEMBG96_H_ */
