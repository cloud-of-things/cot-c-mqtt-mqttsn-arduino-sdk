/**
 * @file DTCoTExtensionIface.h
 * @description Header file for Abstraction Layer for MQTT-SN Interaction
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_EXTENSION_IFACE_HEADER_FILE 
#define DT_COT_EXTENSION_IFACE_HEADER_FILE

/* DTCoT Library Extension Interface 
 * ----------------------------
 */

#include "DTCoTPrivate.h"


// #define NB_IOT_SERVER_IP		"217.74.206.150"	/*DT public MQTTSN connector*/
#define NB_IOT_SERVER_IP		"172.25.102.151"	/*DT CoT m2m MQTTSN connector*/
#define NB_IOT_SERVER_PORT		1883


namespace DTCoT 
{

/**
 * Configuration for every supported device.
 * This contains any configuration info that is common to all devices.
 */
class CoTConfigDevice: public CoTConfigBase { 
public:
	CoTConfigDevice();
	/** Get the 'Device Prefix' to the automatic device (id) name. */
	String getCloudDeviceId();

protected:
	String cloudDeviceId;
};

/**
 * Configuration for every communication method.
 * This contains any configuration info that is common to all communication methods.
 */
class CoTConfigCommunication: public CoTConfigBase { };

/**
 * Configuration for any authentication method.
 */
class CoTConfigAuth: public CoTConfigBase { };

/* Communication types we support */
/* Note: communication type does not belong to the Library public interface.
 * 	It should be hidden and selectable compile-time ( #define in DTCoTSetup.h? )
 */

/**
 * Configuration for MQTT.
 * This class is used to configure an MQTT communications channel.
 * There are standard parameters for connecting to an MQTT broker,
 * and this class encapsulates them.
 * 
 */
class CoTConfigCommunicationMQTT: public CoTConfigBase {
public:
	/**
	 * Constructor
	 * @param serverUrl whare to find the mqtt broker
	 * @param portNumber port to connect to MQTT broker on.
	 * @param password password for the broker account
	 * @param userId user ID/name for the MQTT broker
	 */
	CoTConfigCommunicationMQTT( const String serverUrl
		, unsigned long portNumber 
		, const String password
		, const String userID );

    /// Copy-C-Tor
    CoTConfigCommunicationMQTT( const CoTConfigCommunicationMQTT& origin);

public:
	/**
	 * Getter for URL of the MQTT broker
	 */
	const char* getUrl() const;
	
/**
 * Getter for URL of the MQTT port
 */	
	unsigned long getPortNumber() const;
	
/**
 * Getter for password for the MQTT account
 */
	const char* getPassword() const;
	
   /**
    * Getter for username / id for the MQTT account
    */
	const char* getUserId() const;

private:
	const String _serverUrl;
	unsigned long _portNumber;
	const String _password;
	const String _userId;
};

/**
 * Configuration for MQTT - SN.
 * This class is used to configure an MQTT-SN communications channel.
 * MQTT-SN is a cutdown version of MQTT, designed for small IoT devices.
 * There are standard parameters for connecting to an MQTT-SN broker,
 * and this class encapsulates them.
 * Typically, it conneccts to a local gateway, which converts MQTT-SN
 * traffic to MQTT and relays this to a broker on the internet.
 * 
 */
class CoTConfigCommunicationMQTTSN: public CoTConfigBase {
public:
	
	/**
	 * Constructor
	 * @param password password for the gateway account
	 * @param serverIP whare to find the mqtt-sn gateway;
	 * 		DEFAULT = "172.25.102.151"	(DT CoT m2m MQTTSN connector)
	 * @param serverPort port to connect to MQTT gateway on;
	 * 		DEFAULT = "1883"
	 */
	CoTConfigCommunicationMQTTSN(
		const char* password
		, const char* serverIP = NB_IOT_SERVER_IP
		, const unsigned short serverPort = NB_IOT_SERVER_PORT
	);

public:
	const char* getServerIP();
	unsigned short getServerPort();
	const char* getPassword();


private:
	const char* _serverIP;
	const unsigned short _serverPort;
	const char* _password;

};

/**
 * REST interface (not implemented)
 */
class CoTCommunicationREST: public CoTCommunicationBase { };

/* Authentication mechanisms we support */

/**
 * Fingerprint authentication
 */
class CoTAuthFingerPrint: public CoTAuthBase { };

/**
 * TLS authentication
 */
class CoTAuthTLS: public CoTAuthBase { };

/**
 * No authentication
 */
class CoTAuthNone: public CoTAuthBase { };

} /* namespace DTCoT */

#endif /* DT_COT_EXTENSION_IFACE_HEADER_FILE */
