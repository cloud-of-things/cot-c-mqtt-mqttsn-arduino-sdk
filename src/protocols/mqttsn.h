/**
 * @file mqttsn.h
 * @description config / define file for MQTT-SN serializer / deserialiser 
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 * Based on the implementation from John Donovan from 2014 at https://bitbucket.org/MerseyViking/mqtt-sn-arduino.git
 */

#ifndef __MQTTSN_H__
#define __MQTTSN_H__

#define PROTOCOL_ID 0x01

#define FLAG_DUP 0x80
#define FLAG_QOS_0 0x00
#define FLAG_QOS_1 0x20
#define FLAG_QOS_2 0x40
#define FLAG_QOS_M1 0x60
#define FLAG_RETAIN 0x10
#define FLAG_WILL 0x08
#define FLAG_CLEAN 0x04
#define FLAG_TOPIC_NAME 0x00
#define FLAG_TOPIC_PREDEFINED_ID 0x01
#define FLAG_TOPIC_SHORT_NAME 0x02

#define QOS_MASK (FLAG_QOS_0 | FLAG_QOS_1 | FLAG_QOS_2 | FLAG_QOS_M1)
#define TOPIC_MASK (FLAG_TOPIC_NAME | FLAG_TOPIC_PREDEFINED_ID | FLAG_TOPIC_SHORT_NAME)

// Recommended values for timers and counters. All timers are in seconds.
#define T_ADV 960
#define N_ADV 3
#define T_SEARCH_GW 5
#define T_GW_INFO 5
#define T_WAIT 360
#define T_RETRY 15
#define N_RETRY 5

enum return_code_t {
    ACCEPTED,
    REJECTED_CONGESTION,
    REJECTED_INVALID_TOPIC_ID,
    REJECTED_NOT_SUPPORTED
};

enum __attribute__ ((__packed__)) message_type {
    ADVERTISE 		= 0x00,
    SEARCHGW 		= 0x01,
    GWINFO 			= 0x02,
    CONNECT 		= 0x04,
    CONNACK 		= 0x05,
    WILLTOPICREQ 	= 0x06,
    WILLTOPIC		= 0x07,
    WILLMSGREQ		= 0x08,
    WILLMSG 		= 0x09,
    REGISTER		= 0x0A,
    REGACK			= 0x0B,
    PUBLISH			= 0x0C,
    PUBACK			= 0x0D,
    PUBCOMP			= 0x0E,
    PUBREC			= 0x0F,
    PUBREL			= 0x10,
    SUBSCRIBE 		= 0x12,
    SUBACK			= 0x13,
    UNSUBSCRIBE		= 0x14,
    UNSUBACK		= 0x15,
    PINGREQ			= 0x16,
    PINGRESP		= 0x17,
    DISCONNECT		= 0x18,
    WILLTOPICUPD 	= 0x1A,
    WILLTOPICRESP	= 0x1B,
    WILLMSGUPD		= 0x1C,
    WILLMSGRESP		= 0x1D
};

#if DEBUG
const char* message_names[] = {
    "ADVERTISE",
    "SEARCHGW",
    "GWINFO",
    "unknown",
    "CONNECT",
    "CONNACK",
    "WILLTOPICREQ",
    "WILLTOPIC",
    "WILLMSGREQ",
    "WILLMSG",
    "REGISTER",
    "REGACK",
    "PUBLISH",
    "PUBACK",
    "PUBCOMP",
    "PUBREC",
    "PUBREL",
    "unknown",
    "SUBSCRIBE",
    "SUBACK",
    "UNSUBSCRIBE",
    "UNSUBACK",
    "PINGREQ",
    "PINGRESP",
    "DISCONNECT",
    "unknown",
    "WILLTOPICUPD",
    "WILLTOPICRESP",
    "WILLMSGUPD",
    "WILLMSGRESP"
};
#endif

struct __attribute__ ((__packed__)) message_header {
    uint8_t length;
    message_type type;
};

struct __attribute__ ((__packed__)) msg_advertise : public message_header {
    uint8_t gw_id;
    uint16_t duration;
};

struct __attribute__ ((__packed__)) msg_searchgw : public message_header {
    uint8_t radius;
};

struct __attribute__ ((__packed__)) msg_gwinfo : public message_header {
    uint8_t gw_id;
    char gw_add[0];
};

struct __attribute__ ((__packed__)) msg_connect : public message_header {
    uint8_t flags;
    uint8_t protocol_id;
    uint16_t duration;
    char client_id[0];
};

struct __attribute__ ((__packed__)) msg_connack : public message_header {
    return_code_t return_code;
};

struct __attribute__ ((__packed__)) msg_willtopic : public message_header {
    uint8_t flags;
    char will_topic[0];
};

struct __attribute__ ((__packed__)) msg_willmsg : public message_header {
    char willmsg[0];
};

struct __attribute__ ((__packed__)) msg_register : public message_header {
    uint16_t topic_id;
    uint16_t message_id;
    char topic_name[0];
};

struct __attribute__ ((__packed__)) msg_regack : public message_header {
    uint16_t topic_id;
    uint16_t message_id;
    return_code_t return_code;
};

struct __attribute__ ((__packed__)) msg_publish : public message_header {
    uint8_t flags;
    uint16_t topic_id;
    uint16_t message_id;
    char data[0];
};

struct __attribute__ ((__packed__)) msg_puback : public message_header {
    uint16_t topic_id;
    uint16_t message_id;
    return_code_t return_code;
};

struct __attribute__ ((__packed__)) msg_pubqos2 : public message_header {
    uint16_t message_id;
};

struct __attribute__ ((__packed__)) msg_subscribe : public message_header {
    uint8_t flags;
    uint16_t message_id;
    union {
        char topic_name[0];
        uint16_t topic_id;
    };
};

struct __attribute__ ((__packed__)) msg_suback : public message_header {
    uint8_t flags;
    uint16_t topic_id;
    uint16_t message_id;
    return_code_t return_code;
};

struct __attribute__ ((__packed__)) msg_unsubscribe : public message_header {
    uint8_t flags;
    uint16_t message_id;
    union __attribute__ ((__packed__)) {
        char topic_name[0];
        uint16_t topic_id;
    };
};

struct __attribute__ ((__packed__)) msg_unsuback : public message_header {
    uint16_t message_id;
};

struct __attribute__ ((__packed__)) msg_pingreq : public message_header {
    char client_id[0];
};

struct __attribute__ ((__packed__)) msg_disconnect : public message_header {
    uint16_t duration;
};

struct __attribute__ ((__packed__)) msg_willtopicresp : public message_header {
    return_code_t return_code;
};

struct __attribute__ ((__packed__)) msg_willmsgresp : public message_header {
    return_code_t return_code;
};

#endif /*__MQTTSN_H__*/
