# Example: Huzzah ESP8266 with MQTT over WiFi


## Description
This example is an Arduino sketch that implements an MQTT client over WiFi on an Adafruit HUZZAH ESP8266.  

## Notes
Please check the following things before you try to compile any of the examples:
* Make sure to have all neccesary Arduino libraries and board support installed, [as described here](../../arduino_setup.md).

## Prerequisites
The following prerequisites are neccesary to run this example:
* Huzzah ESP8266

### Add Libraries
in Menu `Sketch` -> `Include Library` -> `Manage Libraries...` find and add these libraries (that are listed 
in the particular example sketch as *dependent*):  
```
This sketch has the following library dependencies:
        - <Adafruit_MQTT_Library> v0.20
        - <ESP8266WiFi> v1.0
        - <Time> v1.5
```
so you have to search for the libraries `Adafruit MQTT Library`, `Time` (by Michael Margolis), and `ESP8266WiFi` (in at least the given releases) and install them.

:alien: **Note**: though the *Adafruit MQTT Lib* is in deed utilized in this library and thus in some sketches, 
the [Adafruit IO System](https://io.adafruit.com/)
is **not used**, called or contacted in any way.

## Step by Step Guide
* Select "Adafruit HUZZAH ESP8266" board support in Arduino before compilation (menu: Tools->Board->"Adafruit HUZZAH ESP8266"). If you can't find the board [please re-check your Arduino IDE setup](../../arduino_setup.md).
* Select the correct serial interface (menu: Tools->Port...). If you can't find your board there check the connection between the board and your computer.
* open this sketch
* check your CoT bootstrap user and password which you got either from the CoT support after registration or from you CoT tenant admin. If it differs from `devicebootstrap` respectively `Fhdt1bb1f` then you have to set your values in the sketch:
 * after the creation of the `CotSdkMqtt` object named `cotSDK` call 
 ```
    CotSdkMqtt *cotSDK = new CotSdkMqtt(*board);
    cotSDK->setBootstrapUser("...your CoT bootstrap user...");
    cotSDK->setBootstrapPwd("...your CoT bootstrap password...");
 ```
 
* in the sketch also fix the missing values for your (local) WIFI network access 
 * `#define SECRET_WIFI_SSID       "...your local Wifi network's SSID..."`
 * `#define SECRET_WIFI_PASSWORD   "...your local Wifi network's password..."`
 
* Compile and programm the sketch onto the board.
* Open the serial monitor to see the device's activity (menu Tools->Serial Monitor)
* follow the [CoT device registration process](cot_device_registration.md) (chapter 'CoT Registration') to get the sketch fully functional