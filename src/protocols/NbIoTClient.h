/**
 * @file NbIoTClient.h
 * @description Header file for Abstracted Network interface of an NB-IoT Modem. 
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 * 
 * Code pieces based on Arduino WiFiClient and GIMASI TUINO example project(s):
 * Company: http://www.tuino.io/
 * GitHub:  https://github.com/gimasi
 */

#ifndef _nbiotclient_h
#define _nbiotclient_h

#include "Arduino.h"	
#include "Client.h"
#include "IPAddress.h"
#include "DTCoTModemBase.h"

/**
 * Arduino Client interface for accessing NbIOT networks.
 */
class NbiotClient : public Client {

public:

  explicit NbiotClient(DTCoTModemBase *modem, Stream& serial, int resetPin, const char* apn,
			const char* mqttsnUser, const char* mqttsnPwd);

  virtual ~NbiotClient();

//  /**
//   * Get the status of this connection. See standard Arduino Client class docs.
//   */
//  uint8_t status();

  /**
   * Connect to address
   * @param ip ip address
   * @param port port
   */
  virtual int connect( IPAddress ip, uint16_t port);
  
 /**
  * Connect to address
  * @param host host IP address
  * @param port port
  *
  * @note Currently, there is no NB-IoT provider which supports host name resolution.
  */
  virtual int connect( const char *host, uint16_t port);

  
 /**
  * Write single byte
  * @param byte to write
  * @return 1 on success
  */
  virtual size_t write( uint8_t);
  
 /**
  * Write bytes
  * @param bytes to write
  * @param size num of bytes to write
  * @return 1 on success
  */
  virtual size_t write( const uint8_t *buf, size_t size);
  
 /**
  * Bytes available
  * @return num of bytes available to read
  */
  virtual int available();

 /**
  * Read a byte (NOT IMPLEMETED)
  * @return byte
  */
  virtual int read();
  
 /**
  * Read bytes
  * @param buffer to read to
  * @param size max num of bytes to read
  * @return num of bytes read
  */
  virtual int read( uint8_t *buf, size_t size);
  
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
   * Setup the NBIoTClient interface
   */
  bool init(long &timeoutMs);

  using Print::write;

private:

  bool initNBIoTModem(long &timeoutMs);

  static uint16_t _srcport;
  uint16_t  _socket;
  unsigned short _serverPort;
  String _serverIP;
  String _imsi;
  String _password;
  String _apn;
  Stream& _serial;
  int _resetPin;
  DTCoTModemBase *_modem;
  bool _modemInitialized;
};

#endif /*_nbiotclient_h*/
