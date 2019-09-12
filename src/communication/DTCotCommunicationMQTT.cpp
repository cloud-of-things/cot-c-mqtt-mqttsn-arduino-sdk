/**
 * @file DTCotCommunicationMQTT.cpp
 * @description Class for management and operations for all MQTT communication
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#include "DTCotCommunicationMQTT.h"
#include "base-classes/DTCoTCommunicationBase.h"
#include "base-classes/DTCoTDeviceBase.h"
#include "DTCoTExtensionIface.h"
#include "utility/DTCoTDebugOutput.h"

using namespace DTCoT;

CoTCommunicationMQTT::CoTCommunicationMQTT(const CoTDeviceBase& device,
		const CoTConfigCommunicationMQTT& conf,
		const CoTAuthBase& authentication) :
		CoTCommunicationBase(device, _config, authentication), _config(conf), _mqtt(
				_device.getClient(), _config.getUrl(), _config.getPortNumber(),
				_config.getUserId(), _config.getPassword()) {
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::CoTCommunicationMQTT() user<%s> pwd<%s>...", _config.getUserId(), _config.getPassword());
	_subscribe = NULL;
	_subBuffer = NULL;
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::CoTCommunicationMQTT() user<%s> pwd<%s>... done.", _config.getUserId(), _config.getPassword());
}

CoTCommunicationMQTT::~CoTCommunicationMQTT() {
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::~CoTCommunicationMQTT()...");
	if (_subscribe) {
		delete (_subscribe);
	} DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::~CoTCommunicationMQTT()... done.");
}

void CoTCommunicationMQTT::init() {
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::init()...");
	reconnect();
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::init()... done.");
}

void CoTCommunicationMQTT::subscribe(const char* topicName,
		const CoTHandler handler) {
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::subscribe() topicName<%s> handler<%p>...",	topicName, handler);
	if (strlen(topicName) == false) {
		DT_DEBUG_PRINTLN_ERROR("CoTCommunicationMQTT::subscribe() no topicName!");
		return;
	}
	if (_subBuffer == topicName) {
		DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::subscribe() topicName<%s> handler<%p>... already done.", topicName, handler);
		return;
	}
	_subBuffer = topicName;
	_subscribe = new Adafruit_MQTT_Subscribe(&_mqtt, _subBuffer, 0);
	if (handler) {
		DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::subscribe() setCallback...");
		// TODO: Typecast for other types too? (See readSubscription() )
		_subscribe->setCallback((SubscribeCallbackBufferType) handler);
		DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::subscribe() setCallback... done.");
	}
	bool success = _mqtt.subscribe(_subscribe);
	if (!success) {
		delete (_subscribe);
		_subscribe = NULL;
		_subBuffer = NULL;
	}
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::subscribe() topicName<%s> handler<%p>... done. success<%d>", _subBuffer, handler, success);
}

void CoTCommunicationMQTT::publish(const char * key, const char * value) {
	unsigned long start = millis();
	DT_DEBUG_PRINTLN_DEBUG("++++++++++++++++++++++++++++++++++++++++");
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::publish() key<%s> value<%s>...", key,	value);
	reconnect();
	Adafruit_MQTT_Publish topic = Adafruit_MQTT_Publish(&_mqtt, key);
	topic.publish(value);
	DT_DEBUG_PRINTLN_INFO("CoTCommunicationMQTT::publish() key<%s> value<%s>... done in <%lu>ms.", key,	value, millis() - start);
	DT_DEBUG_PRINTLN_DEBUG("++++++++++++++++++++++++++++++++++++++++");
}


int CoTCommunicationMQTT::readSubscription(int timeout, uint8_t *responseBuffer, uint16_t responseBufferLen) {
	unsigned long start = millis();
	unsigned long toEnd = start + (unsigned long)timeout;
	Adafruit_MQTT_Subscribe *subscription = NULL;
	int responseLen = -1;
	//cleanup
	memset(responseBuffer, 0, responseBufferLen);
	reconnect();
	while(responseLen <= 0 && !subscription && millis() < toEnd) {
		subscription = _mqtt.readSubscription((int16_t) timeout);
		if (subscription) {
			responseLen = (int) subscription->datalen;
		}
	}

	if (!subscription) {
//		DT_DEBUG_PRINTLN_DEBUG(
//				"CoTCommunicationMQTT::readSubscription() responseBufferLen<%d> timeout<%d>... no subscription found in <%lu>ms.",
//				responseBufferLen, timeout, millis() - start);
		return responseLen;
	}

	if (responseLen > 0) {
		if (responseLen > responseBufferLen) {
			DT_DEBUG_PRINTLN_CRITICAL(
					"CoTCommunicationMQTT::readSubscription() response buffer[%d] timeout<%d>...  response buffer is to small for response size<%d>! Data lost!",
					responseBufferLen, timeout, responseBufferLen);
			responseLen = responseBufferLen;
		}
		memcpy(responseBuffer, subscription->lastread, responseLen);
		if (subscription->callback_buffer != NULL) {
			subscription->callback_buffer((char *) subscription->lastread, subscription->datalen);
		}
		DT_DEBUG_PRINTLN_INFO(
				"CoTCommunicationMQTT::readSubscription() responseBufferLen<%d> timeout<%d>... done in <%lu>ms. reponse[%d]<%s>",
				responseBufferLen, timeout, millis() - start, responseLen, (char*)responseBuffer);
	}

	if (responseLen == 0) {
		DT_DEBUG_PRINTLN_DEBUG(
				"CoTCommunicationMQTT::readSubscription() responseBufferLen<%d> timeout<%d>... done in <%lu>ms. response[%d] is empty!",
				responseBufferLen, timeout, millis() - start, responseLen);
	}

	if (responseLen < 0) {
		DT_DEBUG_PRINTLN_DEBUG(
				"CoTCommunicationMQTT::readSubscription() responseBufferLen<%d> timeout<%d>... no response in subscription found in <%lu>ms.",
				responseBufferLen, timeout, millis() - start);
	}

	return responseLen;
}

void CoTCommunicationMQTT::reconnect() {
	if (_mqtt.connected()) {
		return;
	}
	unsigned long start = millis();
	CoTConfigCommunicationMQTT & config = (CoTConfigCommunicationMQTT&) _config;
	DT_DEBUG_PRINTLN_INFO("CoTCommunicationMQTT::reconnect() url<%s> user<%s> pwd<%s>...",
			config.getUrl(), config.getUserId(), config.getPassword());
	uint8_t retries = 25;
	int8_t ret;
	_mqtt.connectServer();
	while ((ret = _mqtt.connect()) != 0) { // connect will return 0 for connected
		DT_DEBUG_PRINTLN_INFO("CoTCommunicationMQTT::reconnect()...  _mqtt.disconnect()...",
				config.getUrl(), config.getUserId(), config.getPassword(), millis() - start);
		this->disconnect();
		_mqtt.connectServer();
		if (--retries == 0) {
			DT_DEBUG_PRINTLN_CRITICAL(_mqtt.connectErrorString(ret));
			DT_DEBUG_PRINTLN_CRITICAL("CoTCommunicationMQTT::reconnect(): Problem on connecting cloud! ");
			DT_DEBUG_PRINTLN_CRITICAL("please check your Device Credentials!");
			DT_DEBUG_PRINTLN_CRITICAL("I used DeviceName=<%s> and Passwd=<%s>",config.getUserId(), config.getPassword());
			DT_DEBUG_PRINTLN_CRITICAL("Waiting for watchdog reset to reboot...");
			while (1);
		}
		delay(1000);
	}
	DT_DEBUG_PRINTLN_INFO("CoTCommunicationMQTT::reconnect() url<%s> user<%s> pwd<%s>... done in <%lu>ms. MQTT connected.",
			config.getUrl(), config.getUserId(), config.getPassword(), millis() - start);
}

void CoTCommunicationMQTT::disconnect() {
	if (!_mqtt.connected()) {
		DT_DEBUG_PRINTLN_INFO("CoTCommunicationMQTT::disconnect() MQTT already disconnected!");
		return;
	}
	DT_DEBUG_PRINTLN_DEBUG("CoTCommunicationMQTT::disconnect()...");
	uint8_t retries = 25;
	while (!_mqtt.disconnect() && retries--) {
		delay(100);
		if (retries == 0) {
			DT_DEBUG_PRINTLN_ERROR("CoTCommunicationMQTT::disconnect()... Problem on disconnecting cloud!");
		}
	}
	DT_DEBUG_PRINTLN_INFO("CoTCommunicationMQTT::disconnect()... done. MQTT disconnected.");
}
