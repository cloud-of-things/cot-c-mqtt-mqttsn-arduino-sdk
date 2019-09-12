/**
 * @description Simple Demo for CoT Library using MQTT
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */
/**
   This project demonstrates how to build a CoT application

   Application description:
   In this demo a LED on the board will blink.

   This sketch has the following library dependencies:
        - <Adafruit_MQTT_Library> v0.20
        - <Time> v1.5
        - <FlashStorage> v0.7.1
        - <ArduinoSTL> v1.1.0
*/

#include "dt-arduino-iot-agent.h"
#include "cot_sdk/DTCotSdkMqtt.h"
#include "cot_sdk/DTCoTDevice.h"

#include "devices/DTCoTDeviceTuino096.h"

#include "DTCotLed.h"


using namespace DTCoT;

/* Application-specific settings */
float tempMin = 20;
float tempMax = 30;
float tempInc = 0.1;
float temp = tempMin;
bool restartFlag = false;
unsigned long startTS;
unsigned long loopCount = 0;
unsigned long loopInterval = 60000; //ms
unsigned long loopStart;
unsigned long lastLoopStart = 0;


//info led
DTCotLed led(LED_BUILTIN, false);

//My CoT device
CoTDevice cotDevice;

void setup() {
    delay(100);
    Serial.begin(115200);
    delay(100);
    DT_DEBUG_PRINTLN_INFO("\nSetup...");

    DT_DEBUG_PRINTLN("DT_COT_ARDUINO_LIBRARY_VERSION <%s>", DT_COT_ARDUINO_LIBRARY_VERSION);

    CoTConfigDeviceTuino096 *boardConfig = new CoTConfigDeviceTuino096();

    CoTDeviceTuino096 *board = new CoTDeviceTuino096(*boardConfig);

    //signal setup is finished
    led.toggle(4, 100);

    led.blink(true);

    DT_DEBUG_PRINTLN_INFO("Setup... done.");
}


void loop() {
    loopStart = millis();

    led.trigger();

    // so only in intervals of 'loopInterval'
    if (!lastLoopStart || loopStart > (lastLoopStart + loopInterval)) {
        lastLoopStart = loopStart;
    	loopCount++;
    	DT_DEBUG_PRINTLN_INFO("Loop[%lu]...", loopCount);

        DT_DEBUG_PRINTLN_INFO("Loop[%ld]... done in <%lu>ms.", loopCount, millis() - loopStart);
        DT_DEBUG_PRINT("|");
    }

}
