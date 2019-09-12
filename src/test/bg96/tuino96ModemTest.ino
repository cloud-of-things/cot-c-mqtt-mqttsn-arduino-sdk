


#include <ArduinoSTL.h>
#include <vector>

#include "dt-arduino-iot-agent.h"
#include "cot_sdk/DTCotSdkMqtt.h"
#include "devices/DTCoTDeviceTuino096.h"

//#define GSM_APN                 "internet.t-mobile"
//#define GSM_APN_USER            "tm"
//#define GSM_APN_PASSWORD        "tm"

#define GSM_APN                 "iot.1nce.net"
#define GSM_APN_USER            "1n"
#define GSM_APN_PASSWORD        "1n"

#define GSM_RST_PIN             11
#define GSM_PWR_PIN             12
#define GSM_SPEED               115200

CoTDeviceTuino096 *device = NULL;
Client *tcp;
int useSSL = 0;

//initializing
void setup() {
    delay(100);
    //start logger uart
    Serial.begin(115200);
    delay(100);
    Serial.println("setup()...");

    //LED_BUILTIN pin
    pinMode(LED_BUILTIN, OUTPUT);
    //Led blink
    int n = 20;
    while(n--) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(300);
        digitalWrite(LED_BUILTIN, LOW);
        delay(300);
    }


    useSSL = 1;

    CoTConfigDeviceTuino096 *devConfig =  new CoTConfigDeviceTuino096(GSM_APN, GSM_APN_USER, GSM_APN_PASSWORD,-1,-1,
            GSM_RST_PIN, GSM_PWR_PIN, useSSL, "", GSM_SPEED);
    CoTDeviceTuino096 *device = new CoTDeviceTuino096(*devConfig);
    device->init();
    tcp = device->getClient();

    Serial.println("setup()... done.");
}

unsigned long loopNextStartTS = 0;
unsigned long loopInterval = 30000;

//never ending loop
void loop() {
	loopNextStartTS = millis() + loopInterval;
	Serial.println("loop()...");


	const char *host = "cti.ram.m2m.telekom.com";
//	const char *host = "nb-iot.test-ram.m2m.telekom.com";
	Serial.print("host <"); Serial.print(host);Serial.println(">...");
	uint16_t port = 443;
//	uint16_t port = 8883;
	Serial.print("port <"); Serial.print(port);Serial.println(">...");

	tcp->connect(host, port);

	char responseBuffer[256];
	const char *tosendbuffer = "abcde12345";
	Serial.print("sending <"); Serial.print(tosendbuffer);Serial.println(">...");
	Serial.println("---------------------------------------");

	tcp->write((uint8_t *)tosendbuffer, (unsigned int)strlen(tosendbuffer));

	int available = 0;
	while(millis() < loopNextStartTS) {
		available = tcp->available();
		if (available > 0) {
			memset(responseBuffer, 0, sizeof(responseBuffer));
			while(tcp->read((uint8_t*)responseBuffer, available) < 0);
			Serial.print(responseBuffer);
		}
	}
	Serial.println("\n---------------------------------------");

	tcp->stop();

	delay(1000);
	Serial.println("loop()... done.");
}
