/**
 * @file DTCoTMqttSn-config.h
 * @description Defines for supported message types for DT CoT via MQTT-SN
 * State of January 2018
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef __DTCoTMqttSn_config_h
#define __DTCoTMqttSn_config_h


#define MQTTSN_TOPIC_INVALID_ID         0

/*Available topics (use with Mqttsn_RegisterTopic())*/
#define MQTTSN_TOPIC_MEASUREMENT	(char*)"MES"
#define MQTTSN_TOPIC_ALARM			(char*)"ALM"
#define MQTTSN_TOPIC_EVENT			(char*)"EVT"
#define MQTTSN_TOPIC_CMDCMD			(char*)"CMD/MyCmd"
#define MQTTSN_TOPIC_INFCMD			(char*)"INF/MyCmd"

/*Avialable measurement types (use with Mqttsn_RegisterTopic())*/
typedef enum {
	MQTTSN_MEAS_TYPE_INVALID		= 0,
	MQTTSN_MEAS_TYPE_INFCMD			= 1,
	MQTTSN_MEAS_TYPE_TEMPERATURE	= '1',
	MQTTSN_MEAS_TYPE_VOLTAGE		= '2',
	MQTTSN_MEAS_TYPE_ACCELERATION	= '3',
	MQTTSN_MEAS_TYPE_LIGHT			= '4',
	MQTTSN_MEAS_TYPE_HUMIDITY		= '5',
	MQTTSN_MEAS_TYPE_MOISTURE		= '6',
	MQTTSN_MEAS_TYPE_DISTANCE		= '7',
	MQTTSN_MEAS_TYPE_CURRENT		= '8',
	MQTTSN_MEAS_TYPE_SIGNALSTRENGTH	= '9',
	MQTTSN_MEAS_TYPE_PRESSURE		= 'A',
	MQTTSN_MEAS_TYPE_VOLUME			= 'B',
	MQTTSN_MEAS_TYPE_WEIGHT			= 'C',
	MQTTSN_MEAS_TYPE_FREQUENCY		= 'D',
} MqttSnMeasEnumT;


#endif /*__DTCoTMqttSn_config_h*/
