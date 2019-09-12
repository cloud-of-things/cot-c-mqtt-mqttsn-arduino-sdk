/**
 * @file CTCoTNBIoTHardware.dpp
 * @description Abstration layer for the NB-IoT driver
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */
 

#include "DTCoTNBIoTHardware.h"

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

#ifdef TUINO_VARIANT

void DTCoTNBIoTHardware_reset(int resetPin);

void(*_NBRing)();

/*
// Register interrupt. Not every board supports every sort of interrupt.
ISR(PCINT1_vect) {
  if (digitalRead(GMX_INT) == 0) {
    if (_NBRing)
      _NBRing();
  }
}
*/




int DTCoTNBIoTHardware_init(Stream & serial, int resetPin, void( *callback)()) {
	byte init_status = GMXNB_KO;
	
	pinMode(GMX_GPIO1, OUTPUT);
	pinMode(GMX_GPIO2, OUTPUT);
	pinMode(GMX_GPIO3, OUTPUT);
	digitalWrite(GMX_GPIO1, LOW);
	digitalWrite(GMX_GPIO2, LOW);
	digitalWrite(GMX_GPIO3, LOW);
	
	DTCoTNBIoTHardware_reset(resetPin);
	
	HardwareSerial * hwSerial = static_cast<HardwareSerial*>(&serial);
	
	hwSerial->begin(GMX_UART_SPEED);
	DT_DEBUG_PRINT_INFO("GMX Serial Interface");
	init_status = GMXNB_OK;
	// _gmxNB_AtCommTest(response);


  /*FIXME GMX_INT doesn't do anything at all. Can we configure the BC95 to do otherwise?*/
  // Setup Interrupt PIN for Rx
  *digitalPinToPCICR(GMX_INT) |= (1 << digitalPinToPCICRbit(GMX_INT));
  *digitalPinToPCMSK(GMX_INT) |= (1 << digitalPinToPCMSKbit(GMX_INT));

  // set RX callback
  _NBRing = callback;

  // delay to wait BootUp of GMX-LR
  delay(GMX_BOOT_DELAY);
  
  return init_status;
}

void DTCoTNBIoTHardware_reset(int resetPin) {
	pinMode(resetPin, OUTPUT);
	// Reset
	digitalWrite(resetPin, HIGH);
	delay(100);
	digitalWrite(resetPin, LOW);
	delay(100);
	delay(100);
	digitalWrite(resetPin, HIGH);
}

void DTCoTNBIoTHardware_led(char led_state, char ledNumber) {
	int pin = GMX_GPIO1;
	switch((ledNumber+1)%3) {
		case 0: pin = GMX_GPIO1; break;
		case 1: pin = GMX_GPIO2; break;
		case 2: pin = GMX_GPIO3; break;
	}

	 digitalWrite(pin, led_state);
}




#else

// For Feather M0 with uBlox
// Note that standard Arduino doesn't let you set interrupts on UARTS without hacking the variants.cpp file.
// So we would need to setup our own timer interrupts if we want to use the callback

// NOTE: on the uBlox SARA N2 dev board, the TX/RX pins are flipped. So TX goes to TX, and RX goes to RX on the Arduino.
// Also make sure that the correct DIP switches are set on the board to enable the UART. 

void DTCoTNBIoTHardware_reset(int resetPin);

#define UBLOX_SARA_N2_DEFAULT_BAUD_RATE 9600
#define BAUD_RATE UBLOX_SARA_N2_DEFAULT_BAUD_RATE

int DTCoTNBIoTHardware_init(Stream & serial, int resetPin, void( *callback)()) {
	byte init_status = GMXNB_KO;
	
	HardwareSerial * hwSerial = static_cast<HardwareSerial*>(&serial);
	
	DTCoTNBIoTHardware_reset(resetPin);
	hwSerial->begin(BAUD_RATE);
	DT_DEBUG_PRINTLN_INFO("uBlox N2 Serial Interface");
	init_status = GMXNB_OK;
	// _gmxNB_AtCommTest(response);

  // delay to wait BootUp of GMX-LR
  delay(4000);
  
  return init_status;
}

void DTCoTNBIoTHardware_reset(int resetPin) {
	pinMode(resetPin, OUTPUT);
	// Reset
	digitalWrite(resetPin, HIGH);
	delay(100);
	digitalWrite(resetPin, LOW);
	delay(100);
	delay(100);
	digitalWrite(resetPin, HIGH);
}

void DTCoTNBIoTHardware_led(char led_state, char ledNumber) {
	
}




#endif /*ARDUINO_ARCH_AVR*/

