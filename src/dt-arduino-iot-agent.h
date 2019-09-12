#ifndef DT_COT_ARDUINO_H
#define DT_COT_ARDUINO_H

#include <TimeLib.h>

#define DT_COT_ARDUINO_LIBRARY_VERSION "CAL V1.0.23"


/* sadly we need some board specifics here :-( 
 */
#if defined(ESP8266)
    // guess who :-)

#elif defined(ARDUINO_ARCH_SAMD) 
	// for FeatherM0 and Tuino96
    #include <ArduinoSTL.h>

#else
    //for all the others 

#endif

#define MAX_NETWORK_JOIN_RETRIES 100

#endif /* DT_COT_ARDUINO_H */

