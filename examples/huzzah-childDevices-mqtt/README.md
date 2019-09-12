# Example: Huzzah ESP8266 with child-devices over MQTT and WiFi

-------------------------------------------------------------------------------
## Description
This example is an Arduino sketch that implements an MQTT master device client over WiFi, that 
may add/manage a few child devices.  

## Prerequisites
The following prerequisites are neccesary to run this example:
* Huzzah ESP8266
* the CoT MQTT library is installed

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
follow the [Step by Step Guide for the Huzzah MQTT over WiFi](huzzah-wifi-mqtt/).