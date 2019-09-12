/*
 * DTCotLed.cpp
 *
 *  Created on: 29.05.2018
 *      Author: hhenkens
 */

#include <DTCotLed.h>
#include "Arduino.h"

namespace DTCoT {

DTCotLed::DTCotLed(int pin, bool on) :
        pin(pin), ledState(on), ledStateChange(0), blinkOn(false), blinkInterval(
                0) {
    pinMode(pin, OUTPUT);
    turnOn(on);
}

DTCotLed::~DTCotLed() {
}

void DTCotLed::turnOn(bool on) {
    ledState = on;
    if (!on) {
    	blinkOn = on;
    }
    digitalWrite(pin, !ledState);
}

void DTCotLed::toggle(int number, unsigned long interval) {
    while (number-- > 0) {
        ledState = !ledState;
        digitalWrite(pin, !ledState);
        delay(interval);
    }
}

void DTCotLed::blink(bool on, unsigned long intervalMS) {
    turnOn(on);
    blinkOn = on;
    blinkInterval = intervalMS;
}

void DTCotLed::trigger() {
    if (blinkOn && blinkInterval) {
        unsigned long n = millis();
        if (n > (ledStateChange + blinkInterval)) {
            ledStateChange = n;
            toggle();
        }
    }
}

} /* namespace DTCoT */
