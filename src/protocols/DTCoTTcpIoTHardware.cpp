/**
 * @file CTCoTNBIoTHardware.dpp
 * @description Abstration layer for the NB-IoT driver
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */
 

#include "DTCoTTcpIoTHardware.h"
#include "DTCoTModemBase.h"

#include "utility/DTCoTDebugOutput.h"

// Takes the board-specific pinouts and defines, so we know which board we are compiling for
#include <Arduino.h>

/*
 Notes for adding further hardware implementations.
 The parent GMX-NBIoT library is classless C++, so we won't use class heirarchies. Add hardware variants here.
 Put your hardware variant within an ifdef block for that specific hardware type (ie Tuino, other 3rd party board).
 Fill in the required functions, with correct pins, init sequence, and reset procedure for the hardware.
 
 DTCoTNBIoTHardware_init takes a callback as a parameter. This should be called whenever there is new data in the
 NBIoT buffer. This interrupt will be hardware dependent.
 
 @todo move this to the DTCotDevice class, if the gmx_nbiot or equivalent is brought into the DTCoT class hierarchy.
*/

void(*_TcpRing)();

/*
// Register interrupt. Not every board supports every sort of interrupt.
ISR(PCINT1_vect) {
  if (digitalRead(DTCOT_MODEM_INT) == 0) {
    if (_NBRing)
      _NBRing();
  }
}
*/

int DTCoTTcpIoTHardware_init(Stream &serial, int rstPin, void( *callback)()) {
    DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_init()...");
    byte init_status = DTCOT_MODEM_UNITIALIZED;
    DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_init()... reset...");
    DTCoTTcpIoTHardware_reset(rstPin);
    DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_init()... reset... done.");
	// set RX callback
	_TcpRing = callback;
	//delay to wait BootUp of GMX-LR
    DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_init()... boot delay...");
	delay(DTCOT_MODEM_TCP_BOOT_DELAY);
    DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_init()... boot delay... done.");
	DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_init() peek<%d>", serial.peek());
    init_status = DTCOT_MODEM_OK;
	DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_init()... done. init_status<%d>", init_status);
	return init_status;
}

void DTCoTTcpIoTHardware_reset(int rstPin) {
    DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_reset() pin<%d>...", rstPin);
	pinMode(rstPin, OUTPUT);
	// Reset
	digitalWrite(rstPin, HIGH);
	delay(100);
	digitalWrite(rstPin, LOW);
	delay(100);
	delay(100);
	digitalWrite(rstPin, HIGH);
    DT_DEBUG_PRINTLN_INFO("DTCoTTcpIoTHardware_reset() pin<%d>... done.", rstPin);
}

void DTCoTTcpIoTHardware_led(int ledPin, char led_state) {
    pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, led_state);
}


