/**
 * @file TcpIotClient.h
 * @description Header file for Abstracted Network interface of an IoT Modem.
 */

#ifndef _tcpiotclient_h
#define _tcpiotclient_h

#include "Arduino.h"
#include "Client.h"
#include "IPAddress.h"
#include "DTCoTModemBase.h"

/**
 * Arduino Client interface for accessing TCPIOT networks over TCP modem.
 */
class TcpIotClient: public Client {

public:

	explicit TcpIotClient(DTCoTModemBase *modem, int resetPin, const char* apn,
			const char* apnUser, const char* apnPwd, int ssl);

	virtual ~TcpIotClient();

//    /**
//     * Get the status of this connection. See standard Arduino Client class docs.
//     */
//    uint8_t status();

	/**
	 * Connect to address
	 * @param ip ip address
	 * @param port port
	 */
	virtual int connect(IPAddress ip, uint16_t port);

	/**
	 * Connect to address
	 * @param host hostname
	 * @param port port
	 */
	virtual int connect(const char *host, uint16_t port);

	/**
	 * Write single byte
	 * @param byte to write
	 * @return 1 on success
	 */
	virtual size_t write(uint8_t);

	/**
	 * Write bytes
	 * @param bytes to write
	 * @param size num of bytes to write
	 * @return 1 on success
	 */
	virtual size_t write(const uint8_t *buf, size_t size);

	/**
	 * Bytes available
	 * @return num of bytes available to read
	 */
	virtual int available();

	/**
	 * Read a byte
	 * @return byte
	 */
	virtual int read();

	/**
	 * Read bytes
	 * @param buffer to read to
	 * @param size max num of bytes to read
	 * @return num of bytes read
	 */
	virtual int read(uint8_t *buf, size_t size);

	virtual int peek();

	/**
	 * Send all data
	 */
	virtual void flush();

	/**
	 * End connection
	 */
	virtual void stop();

	/**
	 * Is connected
	 * @return 1 if connected
	 */
	virtual uint8_t connected();

	/**
	 * Operator true if sockets available
	 */
	virtual operator bool();

	/**
	 * Setup the TcpIoTClient interface
	 */
	int init(long &timeoutMs);

	using Print::write;

private:

	bool _initTcpIoTModem(long &timeoutMs);

	DTCoTModemBase *_modem;
	int _resetPin;
	bool _modemInitialized;
	String apn;
	String apnUser;
	String apnPwd;
	int ssl;
	uint8_t _lastPrintedConnectState;
	String connectedHost;
	uint16_t connectedPort;
};

#endif /*_tcpiotclient_h*/
