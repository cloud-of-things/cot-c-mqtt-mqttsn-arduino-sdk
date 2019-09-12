# CoT MQTT Arduino Lib: Coding

## Introdcution
This document describes structure and features of the Deutsche Telekom *Arduino Cloud-Of-Things MQTT* library.

## Prerequisites
The implementation of the Communication Model for *MQTT(-SN)* (see below) requrires the installation of a specific
*SmartREST template* in your cloud instance. To install this please reffer the 
[SmartREST Guide](./src/cot_sdk/resources/README.md).

If you construct an new own derived API class, it is expected that you will need to
 build your own coresponding SmartREST template for that.

## [Optional] Patch a dependent library
In this library the *Adafruit MQTT* Lib is used as dependency: https://github.com/adafruit/Adafruit_MQTT_Library .
Unfortunately this lib currently is not maintained and has - so far - some limitations on the length of the messages
to be processed.  
Therefore currently no long device attributes may be sent with the function `CotSmartRestProcessor::updateDevice()`.
If our pull requests to Adafruit are successful, then long device attributes can be used.

Our Pull Request to Adafruit, if it is not taked on and you yourself may make the changes locally:
https://github.com/adafruit/Adafruit_MQTT_Library/pull/124
Only the files Adafruit_MQTT.h, Adafruit_MQTT.cpp must be changed.

:alien: **Note**: though the Adafruit MQTT Lib is utilized, the [Adafruit IO System](https://io.adafruit.com/)
is **not used**, called or contacted in any way.

## API
### CoTDevice
The element for serving functionality to connect and interact with "things" to the CoT is the [`CoTDevice`](src/cot_sdk/DTCoTDevice.h) class.  
It defines methods to
* send measurements
* send alarms
* handle operations

and is the digital representation of your "thing" that enables you to
* receive the device's credentials 
* creatw and fill up the device's *digital twin* in the cloud (a so called *Managed Object*)

From the programming point of view you usually will only have **one** such object instance in your sketch - as you 
only have one devices.

If you want to model a complex device network in the cloud and your "connection device" is only the IoT *gateway* 
than you may want to use the CoT's *child device* functionality. Please see the appropriate example sketch for more 
details.

For details of all the API methods and the meaning of the parameters please consult the API-Documentaion in the header 
files.

### Init a *CoTDevice* and connect to the cloud
#### 1. Board Configuration
To create and initialize a working *CoTDevice* you first need a proper *board configuration* object; this is a 
subclass of `CoTConfigDevice` and holds all required information. It is quite hardware specific and thus has some 
special subclasses derived.

##### API Doc:
* for the Wifi Boards see the [Base API `CoTConfigDeviceWiFi`](src/devices/DTCoTDeviceWiFi.h)
* for the GSM/LTE Boards see the [Base API of `CoTConfigDeviceGPRS`](src/devices/DTCoTDeviceGPRS.h)

##### Examples:
* for the Wifi Board *Adafruit Huzzah Esp8266* you need to give the local Wifi credentials with `CoTConfigDeviceHuzzahWifi(SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD)`
* for the *Tuino096* LTE Board you give the mobile network APN data with `CoTConfigDeviceTuino096(apn, userName, password)`

#### 2. Board
Next ingredient of the menu is a *board*. It corresponds to the *configuration* and finishes the hardware part 
of the setup. The classes of this type derive from `CoTDeviceBase`. (Sorry, this is confusing and is caused from a code 
redesign.)  
The API is for internal aspects only and should't tangle you - if you not want to extend the lib with further boards...  
The subclasses model various boards: from Wifi bricks to those with NB-IoT modems.  
Please see the appropriate example sketches for the type names.

#### 3. Communication SDK
Now the *communication  model* :  
you have the choice between **MQTT** and **MQTT-SN** - depending on the modem and network type and the cloud's 
registration mechanism.  
The comm model is selected by initializing ether the `CotSdkMqtt` or the `CotSdkMqttSn` *SDK*.

:heavy_exclamation_mark: Please note that not all functions are available in all SDKs!  
Especially the MQTT-SN SDK only has limited functionality. Only this methods are available at the `CoTDevice` 
after setting it up with that SDK:
* `sendMeasurement()`
* `registerOperation()` and the related callback mechanism

#### 4. Device
That's all for preparation.  
Time to finally get your goal: the `CoTDevice`:  
it needs the above SDK and will connect to the cloud via `::setup(...)`.

:heavy_exclamation_mark: For all the above steps please see the source code headers or copy code from the appropriate 
examples.

#### 5. Device Registration
In this step the *MQTT* and *MQTT-SN* connection mechanisms differ fundamentally:
* with **MQTT**, to get the device finally and logically connected to the cloud you need to startup your device 
and - if it is the first connection attempt - it will try to register itself. This is described in detail in the 
[CoT device registration process](cot_device_registration.md), chapter 'Device Registration').
* the **MQTT-SN** way is much easier: you need a *NB-IoT* or *MultiMode* *SIM card* which is prepared and registered 
at the CoT already. Together with that SIM card you get related CoT **Device Credentials** handed out.  
These credentials (a device specific *password*) together with the SIMs *IMSI* has to be patched into your devices 
sketch/firmware. Please see the apropriate [MQTT-SN Example Sketch](examples/tuino096_nbiot_mqttsn/) for details.

### CoT Operations
*Operations* are the mechanism of CoT to send commands to the device.  
This can be done in your *CoT portal* (that stuff behind something like `https://<your_tenant>.ram.m2m.telekom.com`) 
under *Device Management | {your device} | >_Shell* and give an operation in the *Kommando* / *Command* field.
* for a **MQTT** device directly use the command string (e.g. `led on`)
* for a **MQTT-SN** device the prefix `topic:MyCmd;data:` has to be used (e.g. `topic:MyCmd;data:led on`)

then *>Execute* the operation.  
After a while the status should switch to *SUCCESS*.

![CoT Operations ](doc/cot_operations.png)

Which operations are supported is programmed by you with the `CotDevice.registerOperation()` function inside your sketch.

### Log Level Setup
For this library we implemented yet another logging mechanism... :expressionless:

These are the C macros `DT_DEBUG_PRINT...(...)` all over the sketches.

You can use these in your own code - after adding the library and including `"utility/DTCoTDebugOutput.h"`.

You can define the log **level** by directly modifying that header file and  commenting out the requested level at 
the lines *30* - *34*.

## Background
The two *Communication SDKs* for **MQTT** and **MQTT-SN** are caused by the different usecases and network technology 
which is expected to be used here and there.

The **MQTT API** ist designed for use in high- and mid-speed networks, like Wifi and 2G, 3G, 4G.

The prefered network behind the **MQTT-SN API** ist *NB-IoT*, the low-power, wide-range 5G communication technology, 
that has - as accepted downside - verly limited data transport capability.
