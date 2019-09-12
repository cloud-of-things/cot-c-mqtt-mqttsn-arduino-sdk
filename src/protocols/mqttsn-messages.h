/**
 * @file mqttsn-messages.h
 * @description Header file for MQTT-SN serializer / deserialiser 
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 * Based on the implementation from John Donovan from 2014 at https://bitbucket.org/MerseyViking/mqtt-sn-arduino.git
 */

#ifndef __MQTTSN_MESSAGES_H__
#define __MQTTSN_MESSAGES_H__

#include "Client.h"
#include "mqttsn.h"

#define MAX_TOPICS 10
#define MAX_BUFFER_SIZE 66


typedef void (*PublishHandlerCbT)(void *payload);

/**
 * Third-party MQTT-SN library
 */
class MQTTSN {
public:
    MQTTSN(Client& ioStream);
    virtual ~MQTTSN();

    uint16_t find_topic_id(const char* name, uint8_t& index);
    bool wait_for_response();
    void parse_stream();

    void searchgw(const uint8_t radius);
    bool connected();
    void willtopic(const uint8_t flags, const char* will_topic, const bool update = false);
    void willmsg(const void* will_msg, const uint8_t will_msg_len, const bool update = false);
    bool register_topic(const char* name);
    void publish(const uint8_t flags, const uint16_t topic_id, const void* data, const uint8_t data_len);
#ifdef USE_QOS2
    void pubrec();
    void pubrel();
    void pubcomp();
#endif
    void subscribe_by_name(const uint8_t flags, const char* topic_name, const PublishHandlerCbT handler);
    void subscribe_by_id(const uint8_t flags, const uint16_t topic_id);
    void unsubscribe_by_name(const uint8_t flags, const char* topic_name);
    void unsubscribe_by_id(const uint8_t flags, const uint16_t topic_id);
    void pingreq(const char* client_id);
    void pingresp();
    void disconnect(const uint16_t duration);

    virtual void timeout();

protected:
    void connect(const uint8_t flags, const uint16_t duration, const char* client_id);

    virtual void advertise_handler(const msg_advertise* msg);
    virtual void connack_handler(const msg_connack* msg);
    virtual void regack_handler(const msg_regack* msg);
    virtual void publish_handler(const msg_publish* msg);
    virtual void register_handler(const msg_register* msg);
    virtual bool puback_handler(const msg_puback* msg);
#ifdef USE_QOS2
    virtual void pubrec_handler(const msg_pubqos2* msg);
    virtual void pubrel_handler(const msg_pubqos2* msg);
    virtual void pubcomp_handler(const msg_pubqos2* msg);
#endif
    virtual void pingreq_handler(const msg_pingreq* msg);
    virtual void pingresp_handler();
    virtual bool disconnect_handler(const msg_disconnect* msg);

    void regack(const uint16_t topic_id, const uint16_t message_id, const return_code_t return_code);
    void puback(const uint16_t topic_id, const uint16_t message_id, const return_code_t return_code);

private:
    struct topic {
        const char* name;
        uint16_t id;
    };

    void dispatch();
    uint16_t bswap(const uint16_t val);
    void send_message();

    
    uint8_t response_to_wait_for;
    uint8_t topic_count;
    topic topic_table[MAX_TOPICS];
    uint8_t _gateway_id;
    PublishHandlerCbT publishHandler = NULL;

protected:
    bool _mqttsnConnected;
	uint8_t message_buffer[MAX_BUFFER_SIZE];
    uint8_t response_buffer[MAX_BUFFER_SIZE];
    int16_t responseBufferLen;
	uint32_t _response_timer;
    uint8_t _response_retries;
	// Set to true when we're waiting for some sort of acknowledgement from the
    //server that will transition our state.
    bool waiting_for_response;
	uint16_t _message_id;
	Client& _ioStream;
	uint16_t _ERR_last_message_id;
	uint16_t _ERR_this_message_id;
};

#endif
