/**
 * @file DTCoTMqttSn.cpp
 * @description DT CoT specific MQTT-SN layer
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#include <Arduino.h>
#include "DTCoTMqttSn.h"
#include "utility/DTCoTDebugOutput.h"

#define CLIENT_ID "dtcot-test-client"
#define MQTTSN_CONNECTION_DURATION_SEC 10000
#define MQTTSN_CONNECTION_TIMEOUT 	   120000


DTCoTMqttSn::DTCoTMqttSn( const char* brokerIp
		, unsigned short brokerPort
		, const char* imsi
		, const char* password
		, DTCoT::CoTDeviceGPRS& device )
  : MQTTSN(*device.getClient())
  , _clientId( String(String(imsi) + String(password)))
  , _serverIP(String(brokerIp))
  , _brokerPort( brokerPort)
  , _imsi(imsi)
  , _password(password)
  , _selectedDevice(device)
  { }


int DTCoTMqttSn::init() {
	delay(1000);
	return reconnect();
}


int DTCoTMqttSn::connect(const uint8_t flags, const uint16_t duration) {

	DT_DEBUG_PRINTLN_INFO("DTCoTMqttSn::connect() with clientId: <%s>",_clientId.c_str());
	
	// Open a UDP socket to the MQTTSN broker-
	IPAddress ipa;
	ipa.fromString(_serverIP);
	if (0 == _ioStream.connect(ipa, _brokerPort)) {
		DT_DEBUG_PRINTLN_INFO("DTCoTMqttSn::connect() - Client connect FAILED");
		_ioStream.stop();
		return false; //TODO
	}

	// Send the MQTTSN connect request.
	// CAL-95, Wait 2 min. for connection, then try reconnection.
	unsigned long start = 0;
	unsigned long toEnd = 0;

	MQTTSN::connect(flags, duration, _clientId.c_str());

	while(waiting_for_response) {
		start = millis();
		toEnd = start + MQTTSN_CONNECTION_TIMEOUT;
		DT_DEBUG_PRINTLN_DEBUG("DTCoTMqttSn::connect() - calling parse_stream()");

		while(waiting_for_response && (millis() < toEnd)) {
			parse_stream();
		}
		// CAL-95, Reset the modem if timeout expired.
		if(millis() >= toEnd) {
			DT_DEBUG_PRINTLN_INFO("DTCoTMqttSn::connect() - timeout while waiting for response");
			return false;
		}
	}

	DT_DEBUG_PRINTBUFFER_DEBUG("DTCoTMqttSn::connect() Response: ", response_buffer, responseBufferLen);

	return connected();
}
	
String DTCoTMqttSn::connectErrorString(int error){
	
	return "DBG: DTCoTMqttSn::connectErrorString() Error: " + String(error);
	
}

void DTCoTMqttSn::disconnect() {
	DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
	MQTTSN::disconnect(1);

	//wait for disconnect ACK
	while( ! isIdle()) {
		// just loop
		DT_DEBUG_PRINTLN_DEBUG("wait for disconnect ACK");
		delay(1000);
	}
	_mqttsnConnected = false;
	_ioStream.stop();
}

bool DTCoTMqttSn::publish(const uint8_t flags, int topicId, const void* value)
{
	int ret = reconnect();
	if (ret <= -2) {
		return false;
	}
	MQTTSN::publish( flags, topicId, value, strlen((char *)value));
	return true;
}

int DTCoTMqttSn::RegisterTopicDTCoT(String topic, MqttSnMeasEnumT valueType) {
	return RegisterTopicDTCoT(topic + "/" + String((char)valueType));
}

int DTCoTMqttSn::RegisterTopicDTCoT(String topic)
{
	DT_DEBUG_PRINTLN_INFO("RegisterTopicDTCoT topic: %s", topic.c_str());

	int ret = reconnect();
	if (ret <= -2) {
		return 0;
	}

	_message_id++;
	DT_DEBUG_PRINTLN_DEBUG("_message_id<%d> (RegisterTopicDT)",_message_id);

	/*                               len   type  topicId     msgId*/
	char myPayload[255] = {/*FIXME:*/0x00, 0x0A, 0x00, 0x00, (char)(_message_id / 256), (char)(_message_id % 256)};
	int myStrLen;
	int readChars = 0;
	int mqttsnTopicId = 0;
	
	/*protocol header*/
	myStrLen = 6;
	/*IMSI of the SIM card + topic selection*/
	myStrLen += sprintf(&(myPayload[myStrLen]), ("NBIoT/" + _imsi + "/" + topic).c_str());
	/*FIXME this payload length treatment doesn't work for sizes (>127)!*/
	myPayload[0] = (char)myStrLen;
	DT_DEBUG_PRINTBUFFER_DEBUG("DTCoTMqttSn::RegisterTopicDTCoT() Payload",(uint8_t*)myPayload, myStrLen);

	_ioStream.write((uint8_t*)myPayload, myStrLen); /* TODO: add error handling */

	/*TODO set wait timeout*/
	unsigned long end = millis() + 30000;
	while(!_ioStream.available()) {
		if (millis() > end) {
			DT_DEBUG_PRINTLN_INFO("DTCoTMqttSn Timeout while waiting for RX data");
			break;
		}
	}
	DT_DEBUG_PRINTLN_DEBUG("DTCoTMqttSn::readChars <%d> myStrLen<%d> BEFORE", readChars, myStrLen);
	readChars = _ioStream.read((uint8_t*)myPayload, myStrLen);
	DT_DEBUG_PRINTLN_DEBUG("DTCoTMqttSn::readChars <%d> myStrLen<%d> AFTER", readChars, myStrLen);

	if( readChars <= 0 ){
		DT_DEBUG_PRINTLN_ERROR("DTCoTMqttSn::RegisterTopicDTCoT(): connection to CoT failed.");
	}
	else {
		DT_DEBUG_PRINTBUFFER_INFO("Response Payload: ", (uint8_t*)myPayload, readChars);

		if(readChars == 7) {
			/*TODO detailled check of content!*/
			mqttsnTopicId = myPayload[2] * 256;
			mqttsnTopicId += myPayload[3];
			DT_DEBUG_PRINTLN_INFO("Got topic id: %d", mqttsnTopicId);
			return mqttsnTopicId;
		}
		else {
			DT_DEBUG_PRINTLN_ERROR("unknown topic reg response. (Len != 7)");
		}
	}
	return mqttsnTopicId;
}

int DTCoTMqttSn::SubscribeTopicDTCoT(const uint8_t flags, const char* topicName, const DTCoT::CoTHandler handler)
{
	DT_DEBUG_PRINTLN_DEBUG("DTCoTMqttSn::SubscribeTopicDTCoT() flags<%d> topicName<%s> handler<%p>",
			flags, topicName, handler);
	reconnect();

	String fullTopic = "NBIoT/" + String(_imsi) + "/" + topicName;
	DT_DEBUG_PRINTLN_DEBUG("SubscribeTopic: fullTopic<%s> ", fullTopic.c_str());

	MQTTSN::subscribe_by_name(flags, fullTopic.c_str(), (PublishHandlerCbT)handler);

	/*Block here until suback has arrived (if QOS_1 or QOS_2) */
	if ((flags & QOS_MASK) == FLAG_QOS_1 || (flags & QOS_MASK) == FLAG_QOS_2) {
		while( ! DTCoTMqttSn::isIdle()) {
			// just loop
		}
	}
	DT_DEBUG_PRINTLN_DEBUG("%s DONE.", __PRETTY_FUNCTION__);
	return 1;
}

void DTCoTMqttSn::gwinfo_handler( const msg_gwinfo* msg) {
	DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
	DT_DEBUG_PRINTLN_DEBUG( "GW ID: " + String( msg->gw_id) );

	reconnect();
}

void DTCoTMqttSn::advertise_handler(const msg_advertise* msg) {
	DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
	DT_DEBUG_PRINTLN_DEBUG( "DBG: GW ID: " + String( msg->gw_id) );

	MQTTSN::advertise_handler( msg);
}

bool DTCoTMqttSn::disconnect_handler(const msg_disconnect* msg) {
	DT_DEBUG_PRINTLN_DEBUG(__PRETTY_FUNCTION__);
	return MQTTSN::disconnect_handler( msg);
}

void DTCoTMqttSn::willtopicreq_handler(const message_header* msg) {
	DT_DEBUG_PRINTLN_DEBUG("DBG: DTCoTMqttSn::willtopicreq_handler()" );

	const uint8_t WILL_TOPIC_FLAGS =  0;
	willtopic( WILL_TOPIC_FLAGS, "Will: " CLIENT_ID);
}

void DTCoTMqttSn::willmsgreq_handler(const message_header* msg) {
	DT_DEBUG_PRINTLN_DEBUG("DBG: DTCoTMqttSn::willtopicreq_handler()" );

	const char* millMsg = "See you next time";
	willmsg( millMsg, sizeof( millMsg) + 1);	
}

bool DTCoTMqttSn::isIdle()
{
	/*check for pending responses*/
	MQTTSN::parse_stream();
	/*return true if mqttsn is NOT busy.*/
	return !wait_for_response();
}

int DTCoTMqttSn::reconnect()
{
	if (connected()) {
		DT_DEBUG_PRINTLN_DEBUG("DTCoTMqttSn::reconnect() - mqttsn.connected() == true");
		return 0;
	}
	DT_DEBUG_PRINTLN_INFO("Connecting to MQTTSN Gateway ... ");

	while ( ! connect(FLAG_CLEAN, MQTTSN_CONNECTION_DURATION_SEC) ) {

		DT_DEBUG_PRINTLN_ERROR("MQTT-SN connecting FAILED; Retrying in 5 seconds...");
		disconnect();
		delay(5000);
		return -2; // "hard" reset
	}
	DT_DEBUG_PRINTLN_DEBUG("MQTTSN Connected!");
	return -1; // connection was done "soft"
}


bool DTCoTMqttSn::isConnectedToNetwork(void)
{
	return _ioStream.connected();
}
