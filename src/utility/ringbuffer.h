/*
 * ringbuffer.h
 *
 *  Created on: 14.03.2018
 *      Author: ak
 */

#ifndef SRC_UTILITY_RINGBUFFER_H_
#define SRC_UTILITY_RINGBUFFER_H_


#include "Arduino.h"
#include "string.h"



extern void NbRingBuffReset(void);
extern int NbRingBuffIsAvailable(void);
extern bool NbRingBuffIsOverflow(void);
extern bool NbRingBuffWriteChar(char c);
extern char NbRingBuffReadChar(void);
extern int NbRingBuffReadString(String &retStr);
extern int NbRingBuffRemoveChars(int numCharacters);
extern int NbRingBuffPeekString(String &retStr);


#endif /* SRC_UTILITY_RINGBUFFER_H_ */
