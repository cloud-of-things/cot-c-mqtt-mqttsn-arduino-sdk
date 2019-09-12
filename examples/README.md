# Usage Examples For The Cloud Of Things Arduino Library

This document describes how to setup and run the usage examples of the Cloud Of Things library. All examples are Arduino sketches for different types of hardware platforms and different protocols. 

## Notes
Please check the following things before you try to compile any of the examples:
* Make sure to have all [neccesary Arduino board support installed](../arduino_setup.md).
* Each example requires specific pieces of hardware, access to the Cloud of Things or credentials for usage of NB-IoT. Please find the details below.
* The Wifi examples requires a set of credentials to access the local WiFi service.

## [Tuino 096 Tutorial](tutorial_tuino096/README.md)
A step-by-step guidance with some leasons to guid you from pluggin in the board to two-way communication wit the *Cloud of Things* both via GSM/G2 and via NB-IoT.

## Examples
The following examples are available (click on the links below to learn more):

### General Examples for the various hardware and uplinks

#### [Tuino 096: MQTT over GSM](tuino096-gsm-mqtt/)
This example is an Arduino sketch that implements an MQTT client over GSM on a *Tuino 096*.

#### [Tuino 096: MQTT-SN over NB-IoT](tuino096_nbiot_mqttsn/)
This example is an Arduino sketch that implements an MQTT-SN client over Nb-IoT on a *Tuino 096*.

#### [Huzzah: MQTT over WiFi](huzzah-wifi-mqtt/)
This example is an Arduino sketch that implements an MQTT client over WiFi on a *Huzzah ESP8266*.

#### [Tuino1: MQTT-SN over NB-IoT](mqttsn_tuino1/README.md)
This example is an Arduino sketch that implements an MQTT-SN client over Narrowband Internet of Things (NB-IoT) on a Tuino1. 

### Examples for special features

#### [Child Devices](huzzah-childDevices-mqtt/)
This example is an Arduino sketch that implements an MQTT master device client over WiFi, that 
may add/manage a few child devices.

#### [real Sensor Data](huzzah-dht22-mqtt/)
This example is an Arduino sketch that implements an MQTT client over WiFi on an Adafruit HUZZAH ESP8266,
using the DHT22 humidity and temperature sensor.  Actual ambient temperature will be sent to the CoT cloud.
If the temperature exceeds a threshold, an alert will be also sent to the cloud.

## Details and further reading
For more details and further reading please read the [Library Docu](src/), the [Device Registration Process](cot_device_registration.md) and the [Cloud's SmartREST Preparation steps](./src/cot_sdk/resources/README.md).
