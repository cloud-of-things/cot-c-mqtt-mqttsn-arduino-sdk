/**
 * @description Simple Demo for CoT Library using MQTT
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */
/**
   This project demonstrates how to build a CoT application

   Application description:
   In this demo the board is registered in the cloud.

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

/* = Global Variables =
 * Set them here as constants and use those in the code below if needed.
 */
const char* DEVICE_NAME =	"<my_DEVICE_NAME>";
const char* DEVICE_Passwd =	"<my_device_passwd>";
const char* DEVICE_moID =	"<my_device_moID>";
//const char* APN = 			"<my_APN>";
//const char* APN_User =		"<my_APN_User>";
//const char* APN_Password =	"<my_APN_Passwd>";


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

    /* Setup the Cloud access and the device to communicate to cloud */

    CoTConfigDeviceTuino096 *boardConfig = new CoTConfigDeviceTuino096();

    /* = Set non-default APN =
     *
     * The default APN is that of T-Mobile D1 network.
     *
     * If you use a SIM-Card of another mobile network provider, please
     * get it's APN Adress, your User and Password and set it with the following commands:
     */
    //boardConfig->setAPN(APN);
    //boardConfig->setAPNUser(APN_User);
    //boardConfig->setAPNPassword(APN_Passwd>);

    /* = Set non-default device name in the CoT =
     *
     * You can give your device a name... in the cloud.
     * The name is set with the below command.
     *
     * If you don't set a name, the library will generate one.
     *
     * In either case the name will be printed in the console log later.
     */
    boardConfig->setDeviceId(DEVICE_NAME);

    CoTDeviceTuino096 *board = new CoTDeviceTuino096(*boardConfig);

    CotSdkMqtt *cotSDK = new CotSdkMqtt(*board);

    /* = Set the cloud credentials =
     *
     * After succesfull registration the cloud credentials get persisted in the devices
     * flash memory.
     * But as the code of a skech is also stored in this flash memory
     * the credentials get overwritten together with an old sketch if you upload the device
     * with another sketch.
     * So you would need to re-register your device everytime you upload a sketch!
     *
     * As a workaround we provide the following function to set the credentials by hand.
     *
     * So the process is this:
     * 1. run the device with this sketch once.
     * 2. Do the registration (as desribed in the README files).
     * 3. Copy the new credentials from the console log
     * 4. Paste the credentials into this function in ALL sketches, you are working on and
     *    where the device shall use this "personality".
     */
    //cotSDK->setCotCredentials(DEVICE_NAME, DEVICE_Passwd, DEVICE_moID);

    //initialize the device
    while(!cotDevice.setup(cotSDK));

    //signal setup is finished
    led.toggle(4, 100);

    //timestamp of device start
    startTS = now();

    // tell the CoT Dashboard that I am online
    cotDevice.updateDevice("Tu96", "1", "1", "ime", "icc", "ims", 120);

    DT_DEBUG_PRINTLN_INFO("Setup... done.");
}


void loop() {
    loopStart = millis();

    //trigger led status, because no interrupt is in use for!
    led.trigger();

    led.blink(true);

    // send the "temperature" measurement only in intervals of 'loopInterval'
    if (!lastLoopStart || loopStart > (lastLoopStart + loopInterval)) {
        lastLoopStart = loopStart;
    	loopCount++;
    	DT_DEBUG_PRINTLN_INFO("Loop[%lu]...", loopCount);

        DT_DEBUG_PRINTLN_INFO("Loop[%ld]... done in <%lu>ms.", loopCount, millis() - loopStart);
        DT_DEBUG_PRINT("|");
    }

}
