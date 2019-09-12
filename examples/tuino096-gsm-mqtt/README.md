# Example: Gimasi Tuino 096 with MQTT over GSM

## Description
This example is an Arduino sketch that implements an MQTT client over GSM network on an Gimasi Tuino 096.  
For more information about this board see http://www.gimasi.ch/productions-platforms/tuino096/ and the developer's site https://github.com/gimasi/TUINO_ZERO_96 .

## Prerequisites

The following prerequisites are neccesary to run this example:
* Tuino 096 board
* network SIM card, at least for GSM access
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

## Step by Step Guide

* follow the steps as described in [Development with *Tuino 096*](examples/development_tuino096.md) 

* If this is the first run of the sketch at all - follow the [CoT device registration process](cot_device_registration.md) to get the sketch fully functional
