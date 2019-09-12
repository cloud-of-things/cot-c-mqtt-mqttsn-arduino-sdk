/*
 * DTCotLed.h
 *
 *  Created on: 29.05.2018
 *      Author: hhenkens
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_DTCOTLED_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_DTCOTLED_H_

namespace DTCoT {

class DTCotLed {
public:
    DTCotLed(int pin, bool on = false);
    virtual ~DTCotLed();
    void turnOn(bool on = true);
    void toggle(int number = 1, unsigned long interval = 0);
    void blink(bool on = true, unsigned long intervalMS = 300);
    void trigger();
private:
    int pin;
    bool ledState;
    unsigned long ledStateChange;
    bool blinkOn;
    unsigned long blinkInterval;
};

} /* namespace DTCoT */

#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_DTCOTLED_H_ */
