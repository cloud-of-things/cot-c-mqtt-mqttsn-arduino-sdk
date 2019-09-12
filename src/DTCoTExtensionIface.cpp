/**
 * @file DTCoTExtensionIface.cpp
 * @description Abstraction Layer for MQTT-SN Interaction
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#include "DTCoTExtensionIface.h"

#include "utility/DTCoTDebugOutput.h"

using namespace DTCoT;

CoTConfigCommunicationMQTT::CoTConfigCommunicationMQTT( 
	const String serverUrl
	, unsigned long portNumber 
	, const String password
	, const String userId )
	: _serverUrl( serverUrl)
	, _portNumber( portNumber)
	, _password( password)
	, _userId( userId)
{
//    DT_DEBUG_PRINT_INFO("CoTConfigCommunicationMQTT::CoTConfigCommunicationMQTT()...");
	CoTConfigBase();
    DT_DEBUG_PRINTLN_INFO("CoTConfigCommunicationMQTT::CoTConfigCommunicationMQTT()... done.");
}

CoTConfigCommunicationMQTT::CoTConfigCommunicationMQTT( const CoTConfigCommunicationMQTT& origin)
    : _serverUrl(origin.getUrl())
    , _portNumber(origin.getPortNumber())
    , _password(origin.getPassword())
    , _userId(origin.getUserId()) 
 {}

const char* CoTConfigCommunicationMQTT::getUrl() const { 
	return _serverUrl.c_str();
}

unsigned long CoTConfigCommunicationMQTT::getPortNumber() const { 
	return _portNumber;
}

const char* CoTConfigCommunicationMQTT::getPassword() const { 
	return _password.c_str();
}

const char* CoTConfigCommunicationMQTT::getUserId() const { 
	return _userId.c_str();
}



CoTConfigCommunicationMQTTSN::CoTConfigCommunicationMQTTSN( 
		const char* password
		, const char* serverIP
		, const unsigned short serverPort
	)
	: _serverIP (serverIP)
	, _serverPort (serverPort)
	, _password( password)
{
	CoTConfigBase();
	//ak serial not yet opened for debugging: DT_DEBUG_PRINTLN_INFO("CoTConfigCommunicationMQTTSN::CoTConfigCommunicationMQTTSN");
}

const char* CoTConfigCommunicationMQTTSN::getServerIP() { 
	return _serverIP;
}

unsigned short CoTConfigCommunicationMQTTSN::getServerPort() {
	return _serverPort;
}

const char* CoTConfigCommunicationMQTTSN::getPassword() { 
	return _password;
}


CoTConfigDevice::CoTConfigDevice(): CoTConfigBase(), cloudDeviceId("") { }

String CoTConfigDevice::getCloudDeviceId() {
	return cloudDeviceId;
}

