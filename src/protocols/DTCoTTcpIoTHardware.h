/**
 * @file DTCoTNBIoTHardware.h
 * @description header file for Abstraction of hardware specifics of an NBIoT interface
 * 
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */
 
#ifndef _DTCoTTcpIoTHardware_h
#define _DTCoTTcpIoTHardware_h

//#include <SoftwareSerial.h>

#define DTCOT_MODEM_TCP_BOOT_DELAY              10000
#define DTCOT_MODEM_TCP_UART_INTERFACE          1
#define DTCOT_MODEM_TCP_UART_TIMEOUT            10000

/**
 * Perform any steps required to initialise the TcpIoT hardware
 * @param resetPin Pin to reset the TcpIoT hardware
 * @param callback callback for when data is available from the TcpIoT buffer
 */
int DTCoTTcpIoTHardware_init(class Stream &serial, int rstPin, void( *callback)());

/**
 * Change LED state on the hardware
 * @param led_state 1 on 0 off
 */
void DTCoTTcpIoTHardware_led(int ledPin, char led_state);


void DTCoTTcpIoTHardware_reset(int rstPin);

#endif /* _DTCoTTcpIoTHardware_h */
