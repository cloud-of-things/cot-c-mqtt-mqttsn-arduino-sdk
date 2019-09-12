/**
 * @file mqttsn-messages.cpp
 * @description MQTT-SN serializer / deserialiser 
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 * Based on the implementation from John Donovan from 2014 at https://bitbucket.org/MerseyViking/mqtt-sn-arduino.git
 */


#include <Client.h>
#include <Arduino.h>

#include "utility/DTCoTDebugOutput.h"
#include "mqttsn-messages.h"
#include "mqttsn.h"


MQTTSN::MQTTSN( Client& ioStream) :
	waiting_for_response(true),
	response_to_wait_for(ADVERTISE),
	_message_id(0),
	topic_count(0),
	_gateway_id(0),
	_response_timer(0),
	_response_retries(0),
	_ioStream(ioStream),
	_mqttsnConnected(false),
	responseBufferLen(0),
	_ERR_last_message_id(0),
	_ERR_this_message_id(0)
{
    memset(topic_table, 0, sizeof(topic) * MAX_TOPICS);
    memset(message_buffer, 0, MAX_BUFFER_SIZE);
    memset(response_buffer, 0, MAX_BUFFER_SIZE);
}

MQTTSN::~MQTTSN() {
}


bool MQTTSN::connected() {
	
	DT_DEBUG_PRINTLN_INFO("MQTTSN::connected() = %d",_mqttsnConnected);
	delay(100);
	return _mqttsnConnected;
}


bool MQTTSN::wait_for_response() {
    if (waiting_for_response) {
        // TODO: Watch out for overflow.
        if ((millis() - _response_timer) > (T_RETRY * 1000L)) {
            _response_timer = millis();

            if (_response_retries == 0) {
            	DT_DEBUG_PRINTLN_ERROR("MQTTSN: Timeout waiting for a response - giving up.");
                waiting_for_response = false;
                disconnect_handler(NULL);
            } else {
            	DT_DEBUG_PRINTLN_INFO("MQTTSN: Timeout waiting for a response - retry...");
                send_message();
            }

            --_response_retries;
        }
    }
    return waiting_for_response;
}

uint16_t MQTTSN::bswap(const uint16_t val) {
    return (val << 8) | (val >> 8);
}

uint16_t MQTTSN::find_topic_id(const char* name, uint8_t& index) {
    for (uint8_t i = 0; i < topic_count; ++i) {
        if (strcmp(topic_table[i].name, name) == 0) {
            index = i;
            return topic_table[i].id;
        }
    }

    return 0xffff;
}


void MQTTSN::parse_stream()
{
	responseBufferLen = 0; //reset
	// CAL-109, Reset response buffer every time before reading.
    memset(response_buffer, 0, MAX_BUFFER_SIZE);

	responseBufferLen = _ioStream.read( response_buffer, MAX_BUFFER_SIZE);
	if( responseBufferLen <= 0 ) {
		// Will be repeated too often:
		//DT_DEBUG_PRINTLN_DEBUG("MQTTSN::parse_stream(): nothing read");
		return;
	}
	DT_DEBUG_PRINTBUFFER_INFO("MQTTSN::parse_stream()", response_buffer, responseBufferLen);

	if (responseBufferLen >= MAX_BUFFER_SIZE) {
		responseBufferLen = MAX_BUFFER_SIZE - 1;
	}
	dispatch();
}



void MQTTSN::dispatch() {
    message_header* response_message = (message_header*)response_buffer;
    bool handled = true;
	
    DT_DEBUG_PRINTLN_DEBUG("MQTTSN::dispatch() messageType<%d> to_wait_for<%d> waiting_for_response <%d>",
				response_message->type, response_to_wait_for, waiting_for_response);

    switch (response_message->type) {
    case ADVERTISE:
        if (waiting_for_response && response_to_wait_for == ADVERTISE) {
            advertise_handler((msg_advertise*)response_buffer);
        } else {
            handled = false;
        }
        break;

    case GWINFO:
        break;

    case CONNACK:
        if (waiting_for_response && response_to_wait_for == CONNACK) {
			DT_DEBUG_PRINTLN_DEBUG("MQTTSN::dispatch() - CONNACK");
            connack_handler((msg_connack*)response_buffer);
        } else {
            handled = false;
        }
        break;

    case WILLTOPICREQ:
        break;

    case WILLMSGREQ:
        break;

    case REGISTER:
        register_handler((msg_register*)response_buffer);
        break;

    case REGACK:
        if (waiting_for_response && response_to_wait_for == REGACK) {
            regack_handler((msg_regack*)response_buffer);
        } else {
            handled = false;
        }
        break;

    case PUBLISH:
        publish_handler((msg_publish*)response_buffer);
        break;

    case PUBACK:
        if (waiting_for_response && response_to_wait_for == PUBACK) {
        	handled = puback_handler((msg_puback*)response_buffer);
        } else {
            handled = false;
        }
        break;

    case SUBACK:
        if (waiting_for_response && response_to_wait_for == SUBACK) {
        } else {
            handled = false;
        }
        break;

    case UNSUBACK:
        if (waiting_for_response && response_to_wait_for == UNSUBACK) {
        } else {
            handled = false;
        }
        break;

    case PINGREQ:
        pingreq_handler((msg_pingreq*)response_buffer);
        break;

    case PINGRESP:
        if (waiting_for_response && response_to_wait_for == PINGRESP) {
            pingresp_handler();
        } else {
            handled = false;
        }
        break;

    case DISCONNECT:
        if (waiting_for_response) {
			handled = disconnect_handler((msg_disconnect*)response_buffer);
		} else {
			handled = false;
		}
        break;

    case WILLTOPICRESP:
        if (waiting_for_response && response_to_wait_for == WILLTOPICRESP) {
            //nothing
        } else {
            handled = false;
        }
        break;

    case WILLMSGRESP:
        if (waiting_for_response && response_to_wait_for == WILLMSGRESP) {
        	//nothing
        } else {
            handled = false;
        }
        break;

    default:
		DT_DEBUG_PRINTLN_INFO("Unknown Response");
        return;
    }

    if (handled) {
		DT_DEBUG_PRINTLN_DEBUG("MQTTSN::dispatch() handled");
        waiting_for_response = false;
    }
    else{
    	//DT_DEBUG_PRINTBUFFER_INFO("MQTTSN::dispatch() cloudn't dispatch msg;", response_buffer, 20);
    }
}

void MQTTSN::send_message() {
    message_header* hdr = reinterpret_cast<message_header*>(message_buffer);
	
    DT_DEBUG_PRINTBUFFER_DEBUG("MQTTSN::send_message()", message_buffer, hdr->length);

    _ioStream.write(message_buffer, hdr->length); /* TODO: add error handling */
    _ioStream.flush();
	
	if (!waiting_for_response) {
        _response_timer = millis();
        _response_retries = N_RETRY;
    }
}

void MQTTSN::timeout() {
    waiting_for_response = true;
    response_to_wait_for = ADVERTISE;
}

void MQTTSN::advertise_handler(const msg_advertise* msg) {
    _gateway_id = msg->gw_id;
}

void MQTTSN::connack_handler(const msg_connack* msg) {
	/*TODO decode the message correctly*/
	_mqttsnConnected = true;
}


void MQTTSN::regack_handler(const msg_regack* msg) {
    if (msg->return_code == 0 && topic_count < MAX_TOPICS && bswap(msg->message_id) == _message_id) {
        const uint16_t topic_id = bswap(msg->topic_id);

        bool found_topic = false;

        for (uint8_t i = 0; i < topic_count; ++i) {
            if (topic_table[i].id == topic_id) {
                found_topic = true;
                break;
            }
        }

        if (!found_topic) {
            topic_table[topic_count].id = topic_id;
            ++topic_count;
        }
    }
    else{
    	DT_DEBUG_PRINTLN_ERROR("MQTTSN::regack_handler() registering FAILED");
    }
}

bool MQTTSN::puback_handler(const msg_puback* msg) {
	DT_DEBUG_PRINTLN_DEBUG("MQTTSN: got PUBACK");

	static uint32_t failcounter = 0;

	// msg = msg;
	if(bswap(_message_id) == msg->message_id)
	{
		_ERR_last_message_id = 0; //reset
		_ERR_this_message_id = 0; //reset
		return true;
	}
	else if ((_ERR_last_message_id == _message_id) && (_ERR_this_message_id == msg->message_id)) {
		// identicall error -> bypass
		return false;
	}
	else
	{
		failcounter++;

		_ERR_this_message_id = 0; //reset
		String logMsg = "MQTTSN: unexpected puback message id<" + String(bswap(msg->message_id)) +">";
		if(waiting_for_response) {
			logMsg += " but expected <" + String(_message_id) +">;";
		}
		else{
			logMsg += " but no puback expected;";
		}
		logMsg += " Packet will be dropped; (in sum "+ String(failcounter) +" packets delayed or lost)";
		DT_DEBUG_PRINTLN_INFO(logMsg);
		_ERR_last_message_id = _message_id;
		_ERR_this_message_id = msg->message_id;
		/* Won't do anything else, just keep waiting for the correct response. */
		return false;
	}
}

#ifdef USE_QOS2
void MQTTSN::pubrec_handler(const msg_pubqos2* msg) {
	msg = msg;
}

void MQTTSN::pubrel_handler(const msg_pubqos2* msg) {
	msg = msg;
}

void MQTTSN::pubcomp_handler(const msg_pubqos2* msg) {
	msg = msg;
}
#endif

void MQTTSN::pingreq_handler(const msg_pingreq* msg) {
    pingresp();
}

bool MQTTSN::disconnect_handler(const msg_disconnect* msg) {
	DT_DEBUG_PRINTLN_INFO("Got DISCONNECT request from remote.");
	_mqttsnConnected = false;
	return true;
}

void MQTTSN::pingresp_handler() {
}

void MQTTSN::publish_handler(const msg_publish* msg) {
	DT_DEBUG_PRINTLN_DEBUG("MQTTSN::publish_handler() msg_publish*<%p> data<%s>", msg, msg->data);

    if (msg->flags & FLAG_QOS_1) {
        return_code_t ret = REJECTED_INVALID_TOPIC_ID;
        const uint16_t topic_id = bswap(msg->topic_id);
        // CAL-100: Swap the message_id bytes too.
        const uint16_t message_id = bswap(msg->message_id);
    	DT_DEBUG_PRINTLN_DEBUG("MQTTSN::publish_handler() msg->topic_id<%d> topic_id<%d> msg->message_id<%d> message_id<%d>",
    			msg->topic_id, topic_id, msg->message_id, message_id);

        for (uint8_t i = 0; i < topic_count; ++i) {
            if (topic_table[i].id == topic_id) {
                ret = ACCEPTED;
                break;
            }
        }
        // CAL-100: Send swapped values, they then will be swapped again in puback().
        puback(topic_id, message_id, ACCEPTED);
    }

    /*deliver this msg into callback function*/
    if(publishHandler != NULL)
    {
    	publishHandler((void*)(msg->data));
    }

}

void MQTTSN::register_handler(const msg_register* msg) {
    return_code_t ret = REJECTED_INVALID_TOPIC_ID;
    uint8_t index = 0;
    find_topic_id(msg->topic_name, index);

    if (index != 0xffff) {
        topic_table[index].id = bswap(msg->topic_id);
        ret = ACCEPTED;
    }

    regack(msg->topic_id, msg->message_id, ret);
}

void MQTTSN::searchgw(const uint8_t radius) {
    msg_searchgw* msg = reinterpret_cast<msg_searchgw*>(message_buffer);

    msg->length = sizeof(msg_searchgw);
    msg->type = SEARCHGW;
    msg->radius = radius;

    send_message();
    waiting_for_response = true;
    response_to_wait_for = GWINFO;
}

void MQTTSN::connect( const uint8_t flags
	, const uint16_t duration
	, const char* client_id ) {

	DT_DEBUG_PRINTLN_DEBUG("send MQTTSN connect request");
    msg_connect* msg = reinterpret_cast<msg_connect*>(message_buffer);

    msg->length = sizeof(msg_connect) + strlen(client_id);
    msg->type = CONNECT;
    msg->flags = flags;
    msg->protocol_id = PROTOCOL_ID;
    msg->duration = bswap(duration);
    strcpy(msg->client_id, client_id);

    send_message();

    waiting_for_response = true;
    response_to_wait_for = CONNACK;

    DT_DEBUG_PRINTLN_DEBUG("MQTTSN: waiting for 'CONNACK'");
}

void MQTTSN::willtopic(const uint8_t flags, const char* will_topic, const bool update) {
    if (will_topic == NULL) {
        message_header* msg = reinterpret_cast<message_header*>(message_buffer);

        msg->type = update ? WILLTOPICUPD : WILLTOPIC;
        msg->length = sizeof(message_header);
    } else {
        msg_willtopic* msg = reinterpret_cast<msg_willtopic*>(message_buffer);

        msg->type = update ? WILLTOPICUPD : WILLTOPIC;
        msg->flags = flags;
        strcpy(msg->will_topic, will_topic);
    }

    send_message();

//    if ((flags & QOS_MASK) == FLAG_QOS_1 || (flags & QOS_MASK) == FLAG_QOS_2) {
//        waiting_for_response = true;
//        response_to_wait_for = WILLMSGREQ;
//    }
}

void MQTTSN::willmsg(const void* will_msg, const uint8_t will_msg_len, const bool update) {
    msg_willmsg* msg = reinterpret_cast<msg_willmsg*>(message_buffer);

    msg->length = sizeof(msg_willmsg) + will_msg_len;
    msg->type = update ? WILLMSGUPD : WILLMSG;
    memcpy(msg->willmsg, will_msg, will_msg_len);

    send_message();
}

void MQTTSN::disconnect(const uint16_t duration) {
    msg_disconnect* msg = reinterpret_cast<msg_disconnect*>(message_buffer);

    msg->length = sizeof(message_header);
    msg->type = DISCONNECT;

    if (duration > 0) {
        msg->length += sizeof(msg_disconnect);
        msg->duration = bswap(duration);
    }

    send_message();
    waiting_for_response = true;
    response_to_wait_for = DISCONNECT;
}

bool MQTTSN::register_topic(const char* name) {
    if (!waiting_for_response && topic_count < (MAX_TOPICS - 1)) {
        ++_message_id;
    	DT_DEBUG_PRINTLN_DEBUG("_message_id<%d> (register_topic)",_message_id);

        // Fill in the next table entry, but we only increment the counter to
        // the next topic when we get a REGACK from the broker. So don't issue
        // another REGISTER until we have resolved this one.
        topic_table[topic_count].name = name;
        topic_table[topic_count].id = 0;

        msg_register* msg = reinterpret_cast<msg_register*>(message_buffer);

        msg->length = sizeof(msg_register) + strlen(name);
        msg->type = REGISTER;
        msg->topic_id = 0;
        msg->message_id = bswap(_message_id);
        strcpy(msg->topic_name, name);

        send_message();
        waiting_for_response = true;
        response_to_wait_for = REGACK;
        return true;
    }

    return false;
}

void MQTTSN::regack(const uint16_t topic_id, const uint16_t message_id, const return_code_t return_code) {
    msg_regack* msg = reinterpret_cast<msg_regack*>(message_buffer);

    msg->length = sizeof(msg_regack);
    msg->type = REGACK;
    msg->topic_id = bswap(topic_id);
    msg->message_id = bswap(message_id);
    msg->return_code = return_code;

    send_message();
}

void MQTTSN::publish(const uint8_t flags, const uint16_t topic_id, const void* data, const uint8_t data_len) {
    msg_publish* msg = reinterpret_cast<msg_publish*>(message_buffer);

    ++_message_id;
	DT_DEBUG_PRINTLN_DEBUG("_message_id<%d> (publish)",_message_id);

    msg->length = sizeof(msg_publish) + data_len;
    msg->type = PUBLISH;
    msg->flags = flags;
    msg->topic_id = bswap(topic_id);
    msg->message_id = bswap(_message_id);
    memcpy(msg->data, data, data_len);

    send_message();

    if ((flags & QOS_MASK) == FLAG_QOS_1 || (flags & QOS_MASK) == FLAG_QOS_2) {
        waiting_for_response = true;
        response_to_wait_for = PUBACK;
    }
}

#ifdef USE_QOS2
void MQTTSN::pubrec() {
    msg_pubqos2* msg = reinterpret_cast<msg_pubqos2*>(message_buffer);

    ++_message_id;

    msg->length = sizeof(msg_pubqos2);
    msg->type = PUBREC;
    msg->message_id = bswap(_message_id);

    send_message();
}

void MQTTSN::pubrel() {
    msg_pubqos2* msg = reinterpret_cast<msg_pubqos2*>(message_buffer);
    msg->length = sizeof(msg_pubqos2);
    msg->type = PUBREL;
    msg->message_id = bswap(_message_id);

    send_message();
}

void MQTTSN::pubcomp() {
    msg_pubqos2* msg = reinterpret_cast<msg_pubqos2*>(message_buffer);

    ++_message_id;

    msg->length = sizeof(msg_pubqos2);
    msg->type = PUBCOMP;
    msg->message_id = bswap(_message_id);

    send_message();
}
#endif

void MQTTSN::puback(const uint16_t topic_id, const uint16_t message_id, const return_code_t return_code) {
	DT_DEBUG_PRINTLN_INFO("send PUBACK topic_id<%d> message_id<%d> return_code<%d>",
																topic_id, message_id, return_code);
    msg_puback* msg = reinterpret_cast<msg_puback*>(message_buffer);

    msg->length = sizeof(msg_puback);
    msg->type = PUBACK;
    msg->topic_id = bswap(topic_id);
    msg->message_id = bswap(message_id);
    msg->return_code = return_code;

    send_message();
}

void MQTTSN::subscribe_by_name(const uint8_t flags, const char* topic_name, const PublishHandlerCbT handler) {
    msg_subscribe* msg = reinterpret_cast<msg_subscribe*>(message_buffer);

    ++_message_id;
	DT_DEBUG_PRINTLN_DEBUG("_message_id<%d> (subscribe_by_name)",_message_id);

    // The -2 here is because we're unioning a 0-length member (topic_name)
    // with a uint16_t in the msg_subscribe struct.
    msg->length = sizeof(msg_subscribe) + strlen(topic_name) - 2;
    msg->type = SUBSCRIBE;
    msg->flags = (flags & QOS_MASK) | FLAG_TOPIC_NAME;
    msg->message_id = bswap(_message_id);
    strcpy(msg->topic_name, topic_name);

    send_message();

    if ((flags & QOS_MASK) == FLAG_QOS_1 || (flags & QOS_MASK) == FLAG_QOS_2) {
        waiting_for_response = true;
        response_to_wait_for = SUBACK;
    }

    publishHandler = handler;
}

void MQTTSN::subscribe_by_id(const uint8_t flags, const uint16_t topic_id) {
    msg_subscribe* msg = reinterpret_cast<msg_subscribe*>(message_buffer);

    ++_message_id;
	DT_DEBUG_PRINTLN_DEBUG("_message_id<%d> (subscribe_by_id)",_message_id);

    msg->length = sizeof(msg_subscribe);
    msg->type = SUBSCRIBE;
    msg->flags = (flags & QOS_MASK) | FLAG_TOPIC_PREDEFINED_ID;
    msg->message_id = bswap(_message_id);
    msg->topic_id = bswap(topic_id);

    send_message();

    if ((flags & QOS_MASK) == FLAG_QOS_1 || (flags & QOS_MASK) == FLAG_QOS_2) {
        waiting_for_response = true;
        response_to_wait_for = SUBACK;
    }
}

void MQTTSN::unsubscribe_by_name(const uint8_t flags, const char* topic_name) {
    msg_unsubscribe* msg = reinterpret_cast<msg_unsubscribe*>(message_buffer);

    ++_message_id;
	DT_DEBUG_PRINTLN_DEBUG("_message_id<%d> (unsubscribe_by_name)",_message_id);

    // The -2 here is because we're unioning a 0-length member (topic_name)
    // with a uint16_t in the msg_unsubscribe struct.
    msg->length = sizeof(msg_unsubscribe) + strlen(topic_name) - 2;
    msg->type = UNSUBSCRIBE;
    msg->flags = (flags & QOS_MASK) | FLAG_TOPIC_NAME;
    msg->message_id = bswap(_message_id);
    strcpy(msg->topic_name, topic_name);

    send_message();

    if ((flags & QOS_MASK) == FLAG_QOS_1 || (flags & QOS_MASK) == FLAG_QOS_2) {
        waiting_for_response = true;
        response_to_wait_for = UNSUBACK;
    }
}

void MQTTSN::unsubscribe_by_id(const uint8_t flags, const uint16_t topic_id) {
    msg_unsubscribe* msg = reinterpret_cast<msg_unsubscribe*>(message_buffer);

    ++_message_id;
	DT_DEBUG_PRINTLN_DEBUG("_message_id<%d> (unsubscribe_by_id)",_message_id);

    msg->length = sizeof(msg_unsubscribe);
    msg->type = UNSUBSCRIBE;
    msg->flags = (flags & QOS_MASK) | FLAG_TOPIC_PREDEFINED_ID;
    msg->message_id = bswap(_message_id);
    msg->topic_id = bswap(topic_id);

    send_message();

    if ((flags & QOS_MASK) == FLAG_QOS_1 || (flags & QOS_MASK) == FLAG_QOS_2) {
        waiting_for_response = true;
        response_to_wait_for = UNSUBACK;
    }
}

void MQTTSN::pingreq(const char* client_id) {
    msg_pingreq* msg = reinterpret_cast<msg_pingreq*>(message_buffer);
    msg->length = sizeof(msg_pingreq) + strlen(client_id);
    msg->type = PINGREQ;
    strcpy(msg->client_id, client_id);

    send_message();

    waiting_for_response = true;
    response_to_wait_for = PINGRESP;
}

void MQTTSN::pingresp() {
    message_header* msg = reinterpret_cast<message_header*>(message_buffer);
    msg->length = sizeof(message_header);
    msg->type = PINGRESP;

    send_message();
}
