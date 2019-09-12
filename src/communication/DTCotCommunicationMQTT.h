/**
 * @file DTCotCommunicationMQTT.h
 * @description header file for Class for management and operations for all MQTT communication
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef MQTTCommunication_h_
#define MQTTCommunication_h_


#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#include "DTCoTPrivate.h"
#include "base-classes/DTCoTCommunicationBase.h"
#include "DTCoTExtensionIface.h"

namespace DTCoT {

/**
 * Communication via MQTT
 * This class allows connection to an MQTT broker on the internet.
 * This uses a direct IP connection without a gateway.
 */
class CoTCommunicationMQTT: public CoTCommunicationBase {

	// == Methods ==
public:
	/**
	 * Constructor
	 * @param device interface to this hardware platform.
	 * @param config config settings matching this hardware paltform
	 * @param authentication login / auth details for this connection.
	 */
	CoTCommunicationMQTT(const CoTDeviceBase& device
			, const CoTConfigCommunicationMQTT& config
			, const CoTAuthBase& authentication	);
    
	/**
	 * Destructor
	 */
	~CoTCommunicationMQTT();

	/**
	 * Startup the MQTT channel
	 */
	void init();

	/**
	 * Publish value on MQTT broker.
	 * Publishes a value to the remote broker. You should see the value change immediately.
	 * @param key the key/topic to publish to. For example /myproject/temperature
	 * @param value the value to publish to the given key/topic. For example: "123", "hello"
	 */
	void publish(const char * key, const char * value);

	int readSubscription(int timeout, uint8_t *responseBuffer, uint16_t responseBufferLen);

	/**
	 * Subscribe value from MQTT broker.
	 * Subscribes a value from the remote broker.
	 * @param topicName Complete Topic name subscribe to. For example /myproject/temperature
	 * @param handler The handler of Callback-function, may be NULL.
	 */
    void subscribe( const char* topicName, const CoTHandler handler = NULL);

	void reconnect();

	void disconnect();

	// == Members ==
private:
	Adafruit_MQTT_Subscribe *_subscribe;
	CoTConfigCommunicationMQTT _config;
	Adafruit_MQTT_Client _mqtt;
    const char *_subBuffer;
};

}

#endif // MQTTCommunication_h_

