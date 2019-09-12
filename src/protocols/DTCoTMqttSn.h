/**
 * @file DTCoTMqttSn.h
 * @description header file for DT CoT specific MQTT-SN layer
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 *
 * dtcotmqttsn
 */

#ifndef DTCOT_MQTTSN_HEADER_FILE
#define DTCOT_MQTTSN_HEADER_FILE

#include <Arduino.h>
#include <Client.h>

#include "mqttsn-messages.h"
#include "protocols/DTCoTMqttSn-config.h"
#include "DTCoTPrivate.h"
#include "devices/DTCoTDeviceGPRS.h"


/**
 * DTCOT's impementation of the MQTT-SN protocol
 */
class DTCoTMqttSn: public MQTTSN {
public:

	/**
	 * Constructor
	 * @param brokerIp ip adress of the mqttsn broker.
	 * @param brokerPort port of the mqttsn broker.
	 * @param imsi as used for CoT's MQTT-SN User ID
	 * @param password password for the connection
	 */
	DTCoTMqttSn( const char* brokerIp
			, unsigned short brokerPort
			, const char* imsi
			, const char* password
			, DTCoT::CoTDeviceGPRS& device );

public:
	/**
	 * Start DTCoT's MQTT-SN interface
	 */
	int init();

	/**
	 * Publish value on MQTT broker.
	 * Publishes a value to the remote broker. You should see the value change immediately.
	 * @param topicId the key/topic to publish to.
	 * @param value the value to publish to the given key/topic. For example: 123, "hello"
	 * @param flags flags to be used by sending
	 */
	bool publish( const uint8_t flags, int topicId, const void* value );

	/**
	 * Get printable error string
	 * @param error number for MQTT-SN error
	 * @return string explaining this error
	 */
	String connectErrorString(int error);

	/**
	 * Disconnect from broker.
	 */
	void disconnect();

	/**
	 * Register to an MQTT-SN topic
	 * @param topic topic name
	 * @param valueType value type. See official MQTT-SN documentation for details.
	 */
	int RegisterTopicDTCoT(String topic, MqttSnMeasEnumT valueType);
	int RegisterTopicDTCoT(String topic);


	/**
	 * Subscribes to a remote topic.
	 * @param topicName Name of the remote topic on which the device shall subscribe
	 * 		without leading "NBIoT/{_imsi}/".
	 * @param handler Callback function for incoming events on the subscribed topic.
	 * @param flags MQTT-SN flags to be used
	 * @returns
	 */
	int SubscribeTopicDTCoT(const uint8_t flags, const char* topicName, const DTCoT::CoTHandler handler);

	bool isIdle();

	bool isConnectedToNetwork(void);

public:

	/**
	 * Called when gateway info available.
	 */
	void gwinfo_handler( const msg_gwinfo* msg);
	void advertise_handler(const msg_advertise* msg);
	bool disconnect_handler(const msg_disconnect* msg);
	void willtopicreq_handler(const message_header* msg);
	void willmsgreq_handler(const message_header* msg);

private:
	/**
	 * connect to the MQTT-SN broker
	 * @param flags for the MQTT-SN CONNECT message. See official MQTT-SN documentation for details.
	 * @param duration duration for the MQTT-SN CONNECT message. See official MQTT-SN documentation for details.
	 * @return true if connected, false else
	 */
	int connect(const uint8_t flags, const uint16_t duration);

	/**
	 * @return
	 * 		 0 if no reset was done
	 * 		-1 if only a "soft" reset was done
	 * 		-2 if a hard network reconnect was done
	 */
	int reconnect();

private:
	String _clientId;
	String _serverIP;
	uint16_t _brokerPort;
	String _imsi;
	String _password;
	DTCoT::CoTDeviceGPRS& _selectedDevice;
};

#endif /* DTCOT_MQTTSN_HEADER_FILE */

