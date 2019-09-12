/*
 * FiFo.h
 *
 *  Created on: 28.09.2018
 *      Author: A301620
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_FIFO_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_FIFO_H_

#include "Arduino.h"

#include "utility/DTCoTDebugOutput.h"

class FiFo {
public:

	FiFo(const char* name, int size) : name(name), bufferSize(size), writeIndex(0), readIndex(0) {
		buffer = (uint8_t*)malloc(sizeof(uint8_t) * bufferSize);
		DT_DEBUG_PRINTLN_DEBUG("FiFo::FiFo<%s>(uint8_t[%d])", name, bufferSize);
	}

	~FiFo() {
		free(buffer);
		DT_DEBUG_PRINTLN_DEBUG("FiFo::~FiFo<%s>(uint8_t[%d])", name.c_str(), bufferSize);
	}

	void push(uint8_t value) {
		int space = bufferSize - writeIndex;
		if ( space > 0) {
			buffer[writeIndex++] = value;
		} else {
			DT_DEBUG_PRINTLN_CRITICAL("FiFo<%s>(uint8_t[%d])::push() FiFo is full! Data lost! writeIndex<%d> readIndex<%d>", name.c_str(), bufferSize, writeIndex, readIndex);
		}
	}

	uint8_t pop() {
		if( length() > 0) {
			uint8_t value = buffer[readIndex++];
			if (readIndex == writeIndex) {
				// reset indexes
				clear();
			}
			return value;
		}
		DT_DEBUG_PRINTLN_CRITICAL("FiFo<%s>(uint8_t[%d])::pop() FiFo is empty! writeIndex<%d> readIndex<%d>", name.c_str(), bufferSize, writeIndex, readIndex);
		return 0;
	}

	int length() {
		int length = writeIndex - readIndex;
		return length;
	}

	uint8_t *content() {
		return buffer;
	}

	void clear() {
		writeIndex = 0;
		readIndex = 0;
	}

private:

	String name;
	uint8_t *buffer;
	int bufferSize;
	int writeIndex;
	int readIndex;
};

#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_PROTOCOLS_FIFO_H_ */
