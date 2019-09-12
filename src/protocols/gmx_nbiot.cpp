/**
 * @file gmx_nbiot.cpp
 * @description AT command based Driver für NB-IoT modules
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 * Based on the GIMASI Drivers Version 1.1 from July 27, 2017 from https://github.com/gimasi/GMX-NBIOT
 */

#include "utility/DTCoTDebugOutput.h"

#include "gmx_nbiot.h"
#include "DTCoTNBIoTHardware.h"
#include "utility/Regexp.h"
#include <string.h>
#include "utility/ringbuffer.h"

#define DEBUG 0
#define INVALID_SOCKET (-1)

// TODO: move to user-specific settings
// #define DT_COT_APN "internet.nbiot.telekom.de.MNC040.MCC901.GPRS"
#define DT_COT_APN "nb-cloud.ic.m2mportal.de"
// #define DT_COT_APN "internet.nbiot.telekom.de"


/*internal utility functions, data conversion binary <-> hex */
String AtResponseTokenize(String atResponse, String delimiter, int &indexStart);
String gmxNB_BinaryToHex(const char *binaryData, int binaryLen );
byte gmxNB_HexToBinary(String hexStr, byte *binaryData);



String        dummyResponse;
byte          gmxNB_interface = GMX_UART_INTERFACE;
int           udp_socket_num = INVALID_SOCKET;
String        udp_socket_ip;
String        udp_port;
const int     udp_rx_timeout = 30; // seconds
const int     udp_max_packet_sz = 255;
int           rxBytesPending = 0;

Stream * stream = 0;


void _log(String data )
{
 /*FIXME param by reference*/
#ifdef DEBUG
	DT_DEBUG_PRINTLN_INFO(data);
#endif
}



void _sendCmd(String in) {
	int len = in.length();
	long int start_timeout;

	_log("----->");
	_log("SENDING: " + in );
	_log("----->");

	//  send data
	for (int i = 0; i < len; i++) {
		if ( gmxNB_interface == GMX_UART_INTERFACE )
		{
			stream->write(in.charAt(i));
		}
		delay(1);
	}

	delay(100);

	if ( gmxNB_interface == GMX_UART_INTERFACE )
	{
		start_timeout = millis();
		while (stream->available() == 0) {
			if (( millis() - start_timeout ) > GMX_UART_TIMEOUT  )
			{
				_log("TIMEOUT on :" + in);
				break;
			}
		}
	}
}




void _checkReadSerialBuffered(void)
{
	// Read from Serial
	while (stream->available() > 0)
	{
		while (stream->available() > 0)
		{ // TODO: What if two or one and a half commands will come one after another?
			NbRingBuffWriteChar((char)stream->read());
			// gmxSerialString += (char)stream->read();
		}
		delay(10);
	}
}



/*overloaded function for backward compatibility. This version can deal with different search strings.*/
byte _parseResponse(String& response, const char *searchPattern) {
	// RegExp Engine
	static MatchState ms;
	static char buf[512];
	char cmd[512];
	String gmxSerialString = "";
	char result;
	int strPos;

	response = "";

	// Read from Serial
	_checkReadSerialBuffered();
	NbRingBuffPeekString(gmxSerialString);

	// TODO: No check for buffer length! Potential buffer overflow!
	gmxSerialString.toCharArray(cmd, gmxSerialString.length());
	if(gmxSerialString.length() > 0){
		_log("<-----");
		_log("RESPONSE: '" + gmxSerialString+"'");
		_log("<-----");
	}

	// Always check for incoming unsolicited packet (+NSONMI)
	ms.Target(cmd);
	result = ms.Match ("+NSONMI:", 0);
	if ( result == REGEXP_MATCHED ) {
		_log("detected rxPacket");
		ms.GetCapture (buf, 0);
		response = String(buf);

		/*decode +NSONMI response. How many bytes are pending?*/
		response.toCharArray(cmd, response.length());
		response = String(cmd);
		response.trim();
		strPos = 0;
		AtResponseTokenize(response, ",", strPos);
		response = response.substring(strPos, response.length());
		if(response == "")
		{
			_log("error decoding +NSONMI");
		}
		else
		{
			/*get hex encoded version of payload from rxStr*/
			rxBytesPending = response.toInt();
			_log("rxBytesPending: " + String(rxBytesPending));
		}
	}

	// Parse Response
	result = ms.Match (searchPattern, 0);
	if ( result == REGEXP_MATCHED ) {
		ms.GetCapture (buf, 0);
		response = String(buf);

		// remove second \r\n => Not very elegant to optimize
		response.toCharArray(cmd, response.length());
		response = String(cmd);

		NbRingBuffRemoveChars(gmxSerialString.length());
		return (GMXNB_OK);
	}

	// Check for Errors
	result = ms.Match ("(.*)\r\nERROR", 0);
	if ( result == REGEXP_MATCHED ) {
		NbRingBuffRemoveChars(gmxSerialString.length());
		return (GMXNB_AT_ERROR);
	}

	/*TODO should we cancel the received characters?*/
	return (GMXNB_AT_GENERIC_ERROR);
}



/* Overloaded function version for backward compatibility. 
 * This is the original version that searches for "OK" only.
 */
byte _parseResponse(String& response) {
	return _parseResponse(response, "(.*)\r\nOK");
}



/**
 * @brief Poll this function in short intervals.
 *
 * Reads incoming characters from serial interface, puts them into ring buffer.
 */
bool gmxNB_isIdle(void)
{
	_log("gmxNB_isIdle()");

	/*TODO poll serial interface, monitor for +NSONMI*/
	// _log("read/parse...");
	// _log("parse status = " + String(status));

	if(gmxNB_Available() > 0)
	{
		DT_DEBUG_PRINTLN_INFO("==================================");
		DT_DEBUG_PRINTLN_INFO("RX packets still pending. Do something!");
		DT_DEBUG_PRINTLN_INFO("==================================");

		/* TODO
		 * Do not halt here!!!
		 * Read out / send up received packet from here!
		 */
		// while(1);
		return false;
	}

	return true;
}



byte _gmxNB_AtCommTest(String& response){
	_sendCmd( "AT\r" );
	return _parseResponse(response);
}



/* GMXNB Commands Interface */
byte gmxNB_connect(String ipAddress, int udpPort)
{
	udp_socket_ip = ipAddress;
	udp_port = udpPort;

	DT_DEBUG_PRINTLN("GMXNB ipAddress:" + udp_socket_ip);
	DT_DEBUG_PRINTLN("GMXNB udpPort: %d", udpPort);
	return GMXNB_OK;
}



byte gmxNB_init(bool forceReset, String ipAddress, int udpPort, Stream & serial, int resetPin, void( *callback)())
{
	String response;

	stream = &serial;

	gmxNB_connect(ipAddress, udpPort);

	_log("GMXNB Init");

	byte init_status = DTCoTNBIoTHardware_init(serial, resetPin, callback);
	NbRingBuffReset();

	if(init_status == GMXNB_OK)
	{
		DT_DEBUG_PRINTLN("Testing hardware...");
		/*NOTE _gmxNB_AtCommTest() is intentionally run twice!*/
		_gmxNB_AtCommTest(response);
		init_status = _gmxNB_AtCommTest(response);

	}
	return init_status;
}



/* Version */
byte gmxNB_getVersion(String& fwVersion)
{
	byte retval;
	_sendCmd( "AT+CGMR\r" );
	retval = _parseResponse(fwVersion);
	// fwVersion.trim();
	return ( retval );
}



/* UE statistics */
byte gmxNB_getUeStats(String& ueStats)
{
	byte retval;
	_sendCmd( "AT+NUESTATS\r" );
	retval = _parseResponse(ueStats);
	ueStats.trim();
	return ( retval );
}



/* Traffic statistics */
byte gmxNB_getTrafficStats(String& traStats)
{
	byte retval;
	_sendCmd( "AT+NUESTATS=THP\r" );
	retval = _parseResponse(traStats);
	traStats.trim();
	return ( retval );
}



/* IMEI */
byte gmxNB_getIMEI(String& imei)
{
	byte retval;
	_sendCmd( "AT+CGSN=1\r" );
	retval = _parseResponse(imei);
	imei.trim();
	return ( retval );
}



/* IMSI */
byte gmxNB_getIMSI(String& imsi)
{
	byte retval;
	_sendCmd( "AT+CIMI\r" );
	retval = _parseResponse(imsi);
	imsi.trim();
	return ( retval );
}



/* Signal Strength Indicator */
byte gmxNB_getCSQ(String& csq)
{
	_sendCmd( "AT+CSQ\r" );
	return ( _parseResponse(csq) );
}



/* Radio on */
byte gmxNB_radioON(String& param)
{
	_sendCmd( "AT+CFUN=1\r" );
	return ( _parseResponse(param) );
}



/* all powered down */
byte gmxNB_radioOFF(String& param)
{
	_sendCmd( "AT+CFUN=0\r" );
	return ( _parseResponse(param) );
}



/* Set APN */
byte gmxNB_setAPN(String APN)
{
	_sendCmd( "at+cgdcont=1,\"IP\",\"" + APN + "\"\r" );
	return ( _parseResponse(dummyResponse) );
}



/* Perform Ping */
byte gmxNB_ping(String hostIp)
{
	_sendCmd("at+nping=" + hostIp + "\r");
	return ( _parseResponse(dummyResponse) );
}



/*Network provider dependent setup: Deutsche Telekom*/
void gmxNB_startDT()
{
	/* TODO: Provide meaningful return values */
	/* TODO: move to provider-specific code - this is not a part of the driver ! */

	gmxNB_radioOFF(dummyResponse);

	_sendCmd( "AT+NCONFIG=AUTOCONNECT,TRUE\r" );
	_parseResponse(dummyResponse);

	_sendCmd( "AT+NCONFIG=CR_0354_0338_SCRAMBLING,TRUE\r" );
	_parseResponse(dummyResponse);

	_sendCmd( "AT+NCONFIG=CR_0859_SI_AVOID,TRUE\r" );
	_parseResponse(dummyResponse);

	// TODO: should be auto-configured by network provider
	gmxNB_setAPN( DT_COT_APN );

	_sendCmd( "AT+NBAND=8\r" );
	_parseResponse(dummyResponse);

	gmxNB_radioON(dummyResponse);

	_sendCmd( "AT+COPS=1,2,\"26201\"\r" );
	_parseResponse(dummyResponse);

	/*TODO use IMSI as authentication parameter*/
	gmxNB_getIMSI(dummyResponse);
}



/* Check Network */

byte gmxNB_isNetworkJoined(void)
{
	byte status;
	char cmd[16];
	String tmp;

	_sendCmd( "at+cgatt?\r" );

	if ( (status = _parseResponse(dummyResponse)) == GMXNB_OK )
	{
		int index = dummyResponse.indexOf(":");
		if ( index != -1 )
		{
			tmp = dummyResponse.substring(index + 1, dummyResponse.length());
			tmp.toCharArray(cmd, tmp.length());

			switch(cmd[0])
			{
			case '1':
				return NB_NETWORK_JOINED;
				break;

			case '0':
				return NB_NETWORK_NOT_JOINED;
				break;

			default:
				return status;
			}
		}
	}
	return status;
}




void gmxNB_Reset(void) {
	String command = String("AT+NRB\r");
	int len = command.length();

	//  send data
	for (int i = 0; i < len; i++) {
		if ( gmxNB_interface == GMX_UART_INTERFACE )
		{
			stream->write(command.charAt(i));
		}
		delay(1);
	}

	delay(GMX_BOOT_DELAY);
}



byte gmxNB_HexToBinary(String hexStr, byte *binaryData)
{
	char temp;
	int j = 0;
	int binaryLen;

	if(hexStr.length() % 2 != 0)
	{
		_log("Invalid format of received hex data.");
		return GMXNB_AT_GENERIC_ERROR;
	}
	else
	{
		hexStr.toUpperCase();
		binaryLen = hexStr.length() / 2;

		for (int i = 0; i < binaryLen * 2; i += 2)
		{
			/*Convert hex string to numeric binary (and make sure to have uppcase characters)*/
			temp = hexStr.charAt(i);
			binaryData[j] = (temp <= '9') ? (temp - '0') : (temp - 'A' + 10);
			binaryData[j] *= 16;
			temp = hexStr.charAt(i+1);
			binaryData[j] += (temp <= '9') ? (temp - '0') : (temp - 'A' + 10);

			j++;
		}

	}

	return GMXNB_OK;
}



String gmxNB_BinaryToHex(const char *binaryData, int binaryLen )
{
	char myData[256];

	for (int i = 0; i < binaryLen; i++)
	{
		sprintf(&(myData[2 * i]), "%02X", (byte)binaryData[i]);
	}

	/*add string termination*/
	myData[2 * binaryLen] = 0;
	/*convert to String object*/
	return String(myData);
}







// TX & RX Data

/**
 * @brief Opens a socket for tx/rx data.
 * @returns The socket number of the socket.
 * @fixme The socket number is a dummy and maybe not what we got assigned! See comment below!
 */
int gmxNB_SocketOpen(void)
{
	_sendCmd("at+nsocr=DGRAM,17," + udp_port + "\r");
	_parseResponse(dummyResponse);
	/*FIXME / NOTE on workaround
	 * BC95 always returns 0 for the first socket.
	 * But which socket number have we really got?
	 * Does String::trim() already do the job?
	 */
	udp_socket_num = 0;
	return udp_socket_num;
}



byte gmxNB_SocketClose(int sock)
{
	if (udp_socket_num != INVALID_SOCKET)
	{
		_sendCmd("at+nsocl=" + String(sock) + "\r");
		return ( _parseResponse(dummyResponse) );
	}
	else
	{
		return GMXNB_AT_ERROR;
	}
}



byte gmxNB_SocketClose(void)
{
	return gmxNB_SocketClose(udp_socket_num);
}



byte gmxNB_TXData(const char *binaryData, int len)
{
	if (udp_socket_num == INVALID_SOCKET) {
		_log("Opening socket:");
		gmxNB_SocketOpen();
	}

	_sendCmd( "at+nsost="
			+ String( udp_socket_num) + ","
			+ udp_socket_ip + ","
			+ udp_port + ","
			+ String(len) + ","
			+ gmxNB_BinaryToHex( binaryData, len) + "\r"
	);

	return _parseResponse(dummyResponse);
}





String AtResponseTokenize(String atResponse, String delimiter, int &indexStart)
{
	String subString = "";
	int indexEnd = atResponse.indexOf(delimiter, indexStart);

	if(indexEnd == -1)
	{
		/*could not find delimiter in string*/
		_log("delimiter \"" + delimiter + "\" not found.");
	}
	else
	{
		subString = atResponse.substring(indexStart, indexEnd);
		indexStart = indexEnd + 1;
	}

	return subString;
}




int gmxNB_Available(void)
{
	static String dummyResponse;
	byte status;

	// return (stream->available());
	/*TODO check for received packets here (i.e. keep monitoring serial if for +NSONMI)*/
	dummyResponse = "";
	status = _parseResponse(dummyResponse, "+NSONMI:");
	return rxBytesPending;
}


/*Wait for incoming data packet*/
byte gmxNB_RXData(String &remoteIp, int udpPortNr, byte *binaryData, int &len)
{
	byte status;
	String rxStr, tmpStr, subStr;
	int maxRetrials;
	int currentStrIndex;

	if(gmxNB_Available() == 0)
	{
		return GMXNB_AT_GENERIC_ERROR;
	}
	else
	{
		_log("Reading incoming UDP packet...");
		_sendCmd("at+nsorf=" + String(udp_socket_num) + "," + udp_max_packet_sz + "\r" );
		status = _parseResponse(rxStr);

		/*Dissecting received string
		 * Format:  [socketNr],[srcIpAddr],[rxUdpPort],[numBytes],[hexPayload],[rxBytesStillPending]
		 * Example: 0,172.25.102.147,1883,3,030500,0
		 */
		remoteIp        = "";
		udpPortNr       = -1;
		len             = 0;
		currentStrIndex = 0;

		/*socket number (will be skipped)*/
		subStr = AtResponseTokenize(rxStr, ",", currentStrIndex);
		if(subStr == "")
		{
			_log("error decoding socket nr");
			return GMXNB_AT_GENERIC_ERROR;
		}

		/*source ip address*/
		subStr = AtResponseTokenize(rxStr, ",", currentStrIndex);
		if(subStr == "")
		{
			_log("error decoding src ip addr");
			return GMXNB_AT_GENERIC_ERROR;
		}
		else
		{
			remoteIp = subStr;
			_log("srcip " + remoteIp);
		}

		/*UDP port*/
		subStr = AtResponseTokenize(rxStr, ",", currentStrIndex);
		if(subStr == "")
		{
			_log("error decoding udp port");
			return GMXNB_AT_GENERIC_ERROR;
		}
		else
		{
			udpPortNr = subStr.toInt();
			_log("port " + String(udpPortNr));
		}

		/*payload length*/
		subStr = AtResponseTokenize(rxStr, ",", currentStrIndex);
		if(subStr == "")
		{
			_log("error decoding udp port");
			return GMXNB_AT_GENERIC_ERROR;
		}
		else
		{
			len = subStr.toInt();
			_log("payloadlen " + String(len));
		}

		/*payload (hex encoded)*/
		subStr = AtResponseTokenize(rxStr, ",", currentStrIndex);
		if(subStr == "")
		{
			_log("error decoding udp port");
			return GMXNB_AT_GENERIC_ERROR;
		}
		else
		{
			/*get hex encoded version of payload from rxStr*/
			_log("payload: " + subStr);
			gmxNB_HexToBinary(subStr, binaryData);
			// don't do that, stupid!: _log("binaryData length: " + String(sizeof(binaryData)));
		}

		/*rxBytesStillPending*/
		subStr = rxStr.substring(currentStrIndex, rxStr.length());
		if(subStr == "")
		{
			_log("error decoding rxBytesStillPending");
			return GMXNB_AT_GENERIC_ERROR;
		}
		else
		{
			/*get hex encoded version of payload from rxStr*/
			rxBytesPending = subStr.toInt();
			_log("rxBytesPending: " + String(rxBytesPending));
		}
	}

	return GMXNB_OK;
}



byte gmxNB_setPowerSaveMode(bool enable)
{
	String paramList;

	if(enable)
	{
		paramList = "1";
	}
	else
	{
		paramList = "0";
	}

	_sendCmd( "at+cpsms=" + paramList + "\r" );
	return ( _parseResponse(dummyResponse) );
}


