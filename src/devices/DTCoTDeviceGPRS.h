/**
 * @file DTCoTDeviceGPRS.h
 * @description header file for base class for management and operations for all GPRS devices
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_DEVICE_GPRS_HEADER_FILE
#define DT_COT_DEVICE_GPRS_HEADER_FILE

//#define DT_COT_DEVICE_GPRS_WITH_TX_RX_LOGGER

//#define DT_COT_DEVICE_GPRS_CLOUD_DEVICE_MODEM_SIM800L

#define DT_COT_DEVICE_GPRS_CLOUD_DEVICE_ID_PREFIX_DEFAULT "GPRS."

#include "DTCoTPrivate.h"
#include "DTCoTExtensionIface.h"
#include "utility/DTCoTDebugOutput.h"
#include "utility/TimeoutHelper.h"

#include "protocols/DTCoTTcpIoTHardware.h"
#include "protocols/TcpIoTClient.h"
#include "protocols/NbIoTClient.h"
#include "HardwareSerial.h"
#include "protocols/ModemBg96.h"
#include "protocols/NbIoTClient.h"

#ifdef DT_COT_DEVICE_GPRS_WITH_TX_RX_LOGGER
	#include "protocols/DTCotClient.h"
#endif

#define DTCOT_MODEM_RECEIVE_FETCH_SIZE_DEFAULT	1012

typedef enum {
	DEVICE_MODEMTYPE_GPRS = 1,
	DEVICE_MODEMTYPE_NBIOT,
	DEVICE_MODEMTYPE_WIFI,
} ModemTypeT;


namespace DTCoT {

/**
 * Config class for generic GPRS capable device.
 */
class CoTConfigDeviceGPRS: public CoTConfigDevice {
public:
    CoTConfigDeviceGPRS(
    		const char* APN,
			const char* userName,
            const char* password,
			int ssl,
			int rxPin,
			int txPin,
			int rstPin,
			int pwrPin,
			long speed,
			int fetchSize = DTCOT_MODEM_RECEIVE_FETCH_SIZE_DEFAULT)
  : CoTConfigDevice(), _APN(APN), _userName(userName), _password(password), _rxPin(rxPin), _txPin(
                txPin), _rstPin(rstPin), _pwrPin(pwrPin), _speed(speed), _ssl(ssl), _fetchSize(fetchSize)
    { }
  public:
    /** Set the 'Device Prefix' to the automatic device (id) name.
     *  The complete name can be read by 'getCloudDeviceId()'. */
    virtual void setDevicePrefix(const char* devicePrefix) = 0;
    /** Set the (complete) Device ID alias Name. */
    virtual void setDeviceId(const char* deviceId) = 0;
    /** Get the mobile network APN name*/
    const char* getAPN() const { return _APN; };
    /** Get the APN user*/
    const char* getUserName() const { return _userName; };
    /** Get the APN password*/
    const char* getPassword() const { return _password; };
    /** Get the Read (RX) Pin of the modem. */
    int getRxPin() const { return _rxPin; };
    /** Get the Write (TX) Pin of the modem. */
    int getTxPin() const { return _txPin; };
    /** Get the Reset Pin of the modem. */
    int getRstPin() const { return _rstPin; };
    /** Get the Power Pin of the modem. */
    int getPwrPin() const { return _pwrPin; };
    /** Get if SSL/TLS shall be used. */
    int getSsl() const { return _ssl; };
    /** Get the communication speed between main MCU and Modem. */
    long getSpeed() const { return _speed; };
    /** Get the communication transfer package size between main MCU and Modem. */
    int getFetchSize() const { return _fetchSize; };

    /** Set the mobile network APN name*/
    void setAPN(const char* value) { _APN = value; };
    /** Set the APN user*/
    void setAPNUser(const char* value) { _userName = value; };
    /** Get the APN password*/
    void setAPNPassword(const char* value) { _password = value; };
    /** Set the Read (RX) Pin of the modem. */
    void setRxPin(int value) { _rxPin = value; };
    /** Set the Write (TX) Pin of the modem. */
    void setTxPin(int value) { _txPin = value; };
    /** Get the Reset Pin of the modem. */
    void setRstPin(int value) { _rstPin = value; };
    /** Get the Power Pin of the modem. */
    void setPwrin(int value) { _pwrPin = value; };
    /** Set if SSL/TLS shall be used. */
    void setSsl(int value) { _ssl = value; };
    /** Set the communication speed between main MCU and Modem. */
    void setSpeed(long value) { _speed = value; };
    /** Set the communication transfer package size between main MCU and Modem. */
    void setFetchSize(int value) { _fetchSize = value; };

  private:
    const char* _APN;
    const char* _userName;
    const char* _password;
    int _rxPin;
    int _txPin;
    int _rstPin;
    int _pwrPin;
    long _speed;
    int _ssl;
    int _fetchSize;
};



/**
 * Device class for generic GPRS capable device.
 */
class CoTDeviceGPRS: public CoTDeviceBase {
public:
	/** Constructor to initialise GSM/LTE mode */
    CoTDeviceGPRS(const CoTConfigDeviceGPRS& gprsConfig, Stream &serial)
	: CoTDeviceBase(gprsConfig),
            _serialAT(serial),
            _modem(NULL),
            _gprsClient(NULL),
            _nbiotClient(NULL)
	{
        DT_DEBUG_PRINTLN_DEBUG("CoTDeviceGPRS::CoTDeviceGPRS()...");

        _modem = new ModemBg96(_serialAT, gprsConfig.getSpeed(), gprsConfig.getRstPin(), NULL, gprsConfig.getFetchSize());

        _gprsClient = new TcpIotClient(_modem, gprsConfig.getRstPin(),
                gprsConfig.getAPN(), gprsConfig.getUserName(),
                gprsConfig.getPassword(), gprsConfig.getSsl());

#ifdef DT_COT_DEVICE_GPRS_WITH_TX_RX_LOGGER
        _logClient = new DTCotClient("MODEM", _gprsClient);
#endif
        DT_DEBUG_PRINTLN_DEBUG("CoTDeviceGPRS::CoTDeviceGPRS()... done.");
    }

    /** Constructor to configure the device for NBIoT
     * @param isNbIoT only to differ this C-Tor from the other :-/
     */
    CoTDeviceGPRS(const CoTConfigDeviceGPRS& nbiotConfig, Stream &serial, bool isNbIoT)
    : CoTDeviceBase(nbiotConfig),
            _serialAT(serial),
            _modem(NULL),
            _gprsClient(NULL),
            _nbiotClient(NULL)
    {
        DT_DEBUG_PRINTLN_DEBUG("CoTDeviceGPRS::CoTDeviceGPRS(nbiot)...");
        _modem = new ModemBg96(_serialAT, nbiotConfig.getSpeed(), nbiotConfig.getRstPin(), NULL, nbiotConfig.getFetchSize());

        _nbiotClient = new NbiotClient(_modem, _serialAT, nbiotConfig.getRstPin(),
        		nbiotConfig.getAPN(), nbiotConfig.getUserName(),
				nbiotConfig.getPassword());

        DT_DEBUG_PRINTLN_DEBUG("CoTDeviceGPRS::CoTDeviceGPRS(CoTConfigDeviceNbIoT&...) done.");
    }


    ~CoTDeviceGPRS() {
        DT_DEBUG_PRINTLN_INFO("CoTDeviceGPRS::~CoTDeviceGPRS()...");
        delete (_gprsClient);
        delete (_nbiotClient);
        delete (_modem);
        DT_DEBUG_PRINTLN_INFO("CoTDeviceGPRS::~CoTDeviceGPRS()... done.");
    }


    bool init(long &timeoutMs) {
    	bool ok = false;
    	TimeoutHelper toh("CoTDeviceGPRS::init()", timeoutMs);
        if(NULL != _gprsClient) {
        	ok = _gprsClient->init(timeoutMs);
        }
        else if(NULL != _nbiotClient) {
        	ok = _nbiotClient->init(timeoutMs);
        }
        toh.checkTimeout();
        return ok;
    }


    Client * getClient() const {
    	if(NULL != _nbiotClient){
    		return (Client*)_nbiotClient;
    	}
    	else if(NULL != _gprsClient){
#ifdef DT_COT_DEVICE_GPRS_WITH_TX_RX_LOGGER
			return _logClient;
#else
			return _gprsClient;
#endif
    	}
    	return NULL;
    };

    String getImsi() {
    	String myIMSI;
    	DTCOT_MODEM_STATUS ms = _modem->getIMSI(myIMSI);
    	if (DTCOT_MODEM_OK != ms) {
    		return "";
    	}
    	return myIMSI;
    }

    DTCoTModemBase* getModem() const {
    	return _modem;
    }

private:
    Stream &_serialAT;
    DTCoTModemBase* _modem;
    TcpIotClient *_gprsClient;
#ifdef DT_COT_DEVICE_GPRS_WITH_TX_RX_LOGGER
    DTCotClient *_logClient;
#endif
    NbiotClient *_nbiotClient;
};

} /* namespace DTCoT */

#endif /* DT_COT_DEVICE_GPRS_HEADER_FILE */

