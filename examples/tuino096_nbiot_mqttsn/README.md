# Example: Gimasi Tuino 096 with MQTT-SN over NB-IoT

## Description
This example is an Arduino sketch that implements an MQTT-SN client over NB-IoT network on an Gimasi Tuino 096.  
For more information about this board see http://www.gimasi.ch/productions-platforms/tuino096/ and the developer's site https://github.com/gimasi/TUINO_ZERO_96 .

## Prerequisites

The following prerequisites are neccesary to run this example:
* Tuino 096 board
* network SIM card (at least for NB-IoT access) which is registered at your CoT tenant and for which you got a device passwort. See below for details.
* properly [prepare your Arduino IDE](../../arduino_setup.md).

### Add Libraries
in Menu `Sketch` -> `Include Library` -> `Manage Libraries...` find and add these libraries (that are listed 
in the particular example sketch as *dependent*):  
```
This sketch has the following library dependencies:
        - <Adafruit_MQTT_Library> v0.20
        - <Time> v1.5
        - <FlashStorage> v0.7.1
        - <ArduinoSTL> v1.1.0
```
so you have to search for the libraries `Adafruit MQTT Library`, `Time` (by Michael Margolis), 
`FlashStorage` and `ArduinoSTL` (in at least the given releases) and install them.

:alien: **Note**: though the *Adafruit MQTT Lib* is in deed utilized in this library and thus in some sketches, 
the [Adafruit IO System](https://io.adafruit.com/)
is **not used**, called or contacted in any way.

## Registration at the CoT for NB-IoT Devices

If you got an **IoT Starter Kit** then you got sent a *CoT Device NB-IoT Password* for your device.  
(Don't mess this password up with your *CoT user password*)  
With this password you get access to the CoT via NB-IoT and MQTT-SN.  
To be more precisely the account is bound to the SIM card respectively its IMSI number.

That IMSI also it the logical *device name* in the CoT - that is, if the uplink is done via NB-IoT; if the link is build via GSM then the device name can be choosen freely, but it anyway has to be unique.  

## Step by Step Guide

* open the sketch
* fill in the *device NB-IoT CoT passwort** by patching the sketch:  
paste it in at  
``` #define NB_IOT_MQTTSN_PWD     "<password for access to the MQTT-SN connector>" ```

* the IMSI is automatically grabed from the SIM
* follow the steps as describted in [Development with *Tuino 096*](examples/development_tuino096.md)
