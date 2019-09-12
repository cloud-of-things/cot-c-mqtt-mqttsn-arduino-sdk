/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

//Bremen
#define SECRET_WIFI_SSID_HB		"IOT-LAB-BRE"
#define SECRET_WIFI_PASSWORD_HB	"ds1&tnova%49265++"

//at home
#define SECRET_WIFI_SSID_AH		"henkboxEG"
#define SECRET_WIFI_PASSWORD_AH	"H54cf7329OOO6349tts4w290kk"

#define MIRROR_SERVER_PORT		4711


// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(MIRROR_SERVER_PORT);

bool ledState = false;
void led() {
	digitalWrite(LED_BUILTIN, !ledState);
}
void ledOn(bool on) {
	ledState = on;
	led();
}
void ledToggle() {
	ledState = !ledState;
	led();
}

void setup() {
    delay(10);
	Serial.begin(115200);
    delay(100);
	Serial.println();
	Serial.println();

    //led
    pinMode(LED_BUILTIN, OUTPUT);
    ledOn(0);

    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(SECRET_WIFI_SSID_HB, SECRET_WIFI_PASSWORD_HB);
    wifiMulti.addAP(SECRET_WIFI_SSID_AH, SECRET_WIFI_PASSWORD_AH);

	Serial.print("Connecting Wifi");
	while(true) {
		if(wifiMulti.run() != WL_CONNECTED) {
			Serial.print(".");
			delay(500);
		} else {
			Serial.println(" done.");
			break;
		}
	}

    // Start the server
    server.begin();

    Serial.print("Mirror server Port<"); Serial.print(MIRROR_SERVER_PORT);
    Serial.print("> MAC<");	Serial.print(WiFi.macAddress().c_str());
    Serial.print("> IP<");	Serial.print(WiFi.localIP());
    Serial.println(">");

}

uint8_t rxBuffer[2000];
int available;
int readLength;
unsigned int written;

unsigned long clientReceiveTimeout = 30000;
unsigned long clientReceiveTimestamp = 0;
WiFiClient client;
unsigned long start;
unsigned long end;

void loop() {
	if (client.connected()) {
		// Check if a client has data available
		available = client.available();
		if (available > 0) {
			ledOn(1);

			start = millis();
			readLength = client.read(rxBuffer, available);
			written = client.write(rxBuffer, readLength);
			client.flush();
			end = millis();

			Serial.print("Mirror server start to mirror<"); Serial.print(available); Serial.print(">... ");
			Serial.print("read<"); Serial.print(readLength); Serial.print(">... ");
			Serial.print("written<"); Serial.print(written);
			if (written != (unsigned int)readLength) {
				Serial.print(" -> write problem! close socket...");
				client.stopAll();
			}
			Serial.print("> in <"); Serial.print(end - start);
			Serial.println(">ms. END");

			ledOn(0);
			clientReceiveTimestamp = millis();
		} else {
			if (millis() > (clientReceiveTimestamp + clientReceiveTimeout) ) {
				Serial.println("Mirror server connection timeout! close socket!");
				client.stopAll();
			}
		}
	} else {
		client = server.available();
		clientReceiveTimestamp = millis();
		if (client.connected()) {
			Serial.println("Mirror server new connection");
		}
	}
}

