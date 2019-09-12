# Example: Huzzah ESP8266 with DHT22 sensor over WiFi

## Description
This example is an Arduino sketch that implements an MQTT client over WiFi on an Adafruit HUZZAH ESP8266,
using the DHT22 humidity and temperature sensor.  Actual ambient temperature will be sent to the CoT cloud.
If the temperature exceeds a threshold, an alert will be also sent to the cloud.

Additional information and wiring shema at this side:
https://www.losant.com/blog/getting-started-with-the-esp8266-and-dht22-sensor

## Important Notes
Please check the following things before you try to compile any of the examples:
* Make sure to have all neccesary Arduino libraries and board support installed, [as described here](../../arduino_setup.md).

## Prerequisites

The following prerequisites are neccesary to run this example:
* Huzzah ESP8266
* DHT22 sensor

### Add Libraries
in Menu `Sketch` -> `Include Library` -> `Manage Libraries...` find and add these libraries (that are listed 
in the particular example sketch as *dependent*):  
```
This sketch has the following library dependencies:
        - <Adafruit_MQTT_Library> v0.20
        - <ESP8266WiFi> v1.0
        - <Time> v1.5
        - <DHT sensor library (Default)> v1.3.
```
so you have to search for the libraries `Adafruit MQTT Library`, `Time` (by Michael Margolis), `DHT sensor library` and `ESP8266WiFi` (in at least the given releases) and install them.

:alien: **Note**: though the *Adafruit MQTT Lib* is in deed utilized in this library and thus in some sketches, 
the [Adafruit IO System](https://io.adafruit.com/)
is **not used**, called or contacted in any way.

## Step by Step Guide
follow the [Step by Step Guide for the Huzzah MQTT over WiFi](huzzah-wifi-mqtt/).