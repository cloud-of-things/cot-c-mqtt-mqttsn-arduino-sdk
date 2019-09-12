/**
 * @description Simple Demo for CoT Library using MQTT-SN
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */
/** 
   This project demonstrates how simple it is to build a DTCoT application.

   This example also shows that DTCoT does not depend on device type,
   network and protocol libraies, and can be easily extended by adding more
   devices and underlaying protocols (as soon as the cloud starts supporting them).

   Application description:
   In this demo a simulated temperature measurement and alarm are sent to the 
   cloud (using MQTT-SN via NBIoT).

   This sketch has the following library dependencies:
   	    - <Adafruit MQTT Library> v0.20.1
        - <ArduinoSTL> v1.1.0
        - <Time> v1.5
 */

#include "dt-arduino-iot-agent.h"
#include "devices/DTCotDeviceGimasi.h"
#include "cot_sdk/DTCotSDKmqttSN.h"

#if !defined(ARDUINO_1284P_STANDARD)
#error This project should be compiled with Tuino1.
#endif /*ARDUINO_1284P_STANDARD*/




/**************************** TD CoT Setup ***********************************/


#define NB_IOT_SERVER_PORT		1883



#if 1
/*AK T-Systems default:*/
// #define NB_IOT_SERVER_IP		"217.74.206.150"	/*DT public MQTTSN connector*/
// #define NB_IOT_SERVER_IP		"195.201.137.136"	/*Test: nbiot.mm1-technology.net*/
#define NB_IOT_SERVER_IP		"172.25.102.151"	/*DT CoT m2m MQTTSN connector*/
#define NB_IOT_IMSI				"901405800020888"
#define NB_IOT_MQTTSN_PWD		"SWNjhAgL"
#endif



#if 0
/*AK default:*/
#define NB_IOT_SERVER_IP		"172.25.102.151"	/*DT CoT m2m MQTTSN connector*/
#define NB_IOT_IMSI				"901405800006627"
#define NB_IOT_MQTTSN_PWD		"MyUsx6tX"
#endif



#if 0
/*AK reserve:*/
#define NB_IOT_SERVER_IP		"172.25.102.151"	/*DT CoT m2m MQTTSN connector*/
#define NB_IOT_IMSI				"901405800018944"
#define NB_IOT_MQTTSN_PWD		"34kyBcZn"
#endif



/*
#define NB_IOT_IMSI				"<IMSI, as found in the logs of your device>"
#define NB_IOT_MQTTSN_PWD		"<password for access to the MQTT-SN connector>"
*/

/*************************** Sketch Code ************************************/

float tempMin = 20;
float tempMax = 30;
float tempInc = 0.2;
float temp = tempMin;
byte alarmSeverityMin = 1;
byte alarmSeverityMax = 4;
byte alarmSeverity = alarmSeverityMin;

using namespace DTCoT;


/*** Setup the Cloud access and the device to communicate to cloud. ***/

/*Set the device configuration for the NBIOT modem*/
CoTConfigDeviceGimasi devConfig = CoTConfigDeviceGimasi
		(
				NB_IOT_SERVER_IP,
				NB_IOT_SERVER_PORT,
				NB_IOT_IMSI,
				NB_IOT_MQTTSN_PWD,
				Serial1, 				// serial port to use for the NBIoT hardware
				GMX_RESET				// reset pin to use for the NBIoT hardware
		);

/*Start the NBIOT modem device using the configuration above.*/
CoTDeviceGimasi device = CoTDeviceGimasi(devConfig);

/*Set the configuration for MQTT-SN protocol*/
CoTConfigCommunicationMQTTSN mqttsnConfig(NB_IOT_MQTTSN_PWD);

/*Start MQTT-SN protocol using the configuration above.*/
CotSdkMqttSN cotSDK = CotSdkMqttSN(device, mqttsnConfig);



/*** Definition of operations for device remote control. ***/

OperationStatus operationLedOn(std::vector<String>&) {
	DT_DEBUG_PRINTLN_INFO(__PRETTY_FUNCTION__);
	return OperationStatus::SUCCESSFUL;
}

OperationStatus operationLedOff(std::vector<String>&) {
	DT_DEBUG_PRINTLN_INFO(__PRETTY_FUNCTION__);
	return OperationStatus::SUCCESSFUL;
}

OperationStatus operationLedToggle(std::vector<String>&) {
	DT_DEBUG_PRINTLN_INFO(__PRETTY_FUNCTION__);
	return OperationStatus::SUCCESSFUL;
}



void setup() {
	long timeoutMs = /*ignored*/0;
	
	delay(100);
	Serial.begin(115200);
	delay(100);

	DT_DEBUG_PRINTLN("\nSetup...");

	/*library related initialisation*/
	device.init(timeoutMs);
	cotSDK.init(timeoutMs);
	
	/*Operation initialisation for device remote control*/
	cotSDK.registerOperation(operationLedOn, DTCOTOPERATIONID_C8Y_COMMAND, "led on", true);
	// cotSDK.registerOperation(operationLedOff, DTCOTOPERATIONID_C8Y_COMMAND, "led off", true);
	// cotSDK.registerOperation(operationLedToggle, DTCOTOPERATIONID_C8Y_COMMAND, "led toggle", true);
	
	delay(0);

	DT_DEBUG_PRINTLN("Setup done.");
}



void loop() {
	/*is the protocol stack busy?*/
	if(0 == cotSDK.executeOperations(100, false))
	{
		/*just wait for the protocol stack to get ready again...*/
	}
	else
	{
		/*Protocol stack is idle*/
		delay(5000);
		
		DT_DEBUG_PRINTLN("\nLoop...");

#if 0
		String retString;
		
		/*Print various NBIOT status infos*/
		
		/*Receive parameters: RSSI, BER*/
		cotSDK.getCSQ(retString);
		DT_DEBUG_PRINTLN("CSQ(RSSI,BER): " + retString);
		/*IMSI of the used SIM card*/
		cotSDK.getIMSI(retString);
		DT_DEBUG_PRINTLN("IMSI:          " + retString);
		/*Version information of the NBIOT modem:*/
		cotSDK.getVersion(retString);
		DT_DEBUG_PRINTLN("Version:       " + retString);
		/*NBIOT network device statistics*/
		cotSDK.getUeStats(retString);
		DT_DEBUG_PRINTLN("UE stats:      " + retString);
		/*NBIOT traffic statistics Tx/Rx*/
		cotSDK.getTrafficStats(retString);
		DT_DEBUG_PRINTLN("traffic stats: " + retString);
#endif /*0*/
		
		/*NBIOT connection status*/
		if(cotSDK.isConnectedToNetwork())
		{
			DT_DEBUG_PRINTLN("connected");
			
			/*ready to send a messages*/
			// cotSDK.sendAlarm("alarm", "type", AlarmSeverity::WARNING, AlarmStatus::ACTIVE);
			cotSDK.sendMeasurement("ty", "Room1", "Temperature", "C", temp, now());
			temp += tempInc;
			if (temp > tempMax) {
				temp = tempMin;
			}
		}
		else
		{
			DT_DEBUG_PRINTLN("disconnected");
		}
		DT_DEBUG_PRINTLN("Loop done.");
	}
	
}

