/**
 * @file DTCotDebugOutput.cpp
 * @description Central debug output file
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */
#include <stdarg.h>

#include "DTCoTDebugOutput.h"

#define PRINTF_BUF 256 // define the tmp buffer size (change if desired)

#define DEBUG_PRINTBUFFER_SIGN_END 		""
#define DEBUG_PRINTBUFFER_LINE_LENGTH 	150
//#define DEBUG_PRINTBUFFER_AS_HEX

void debug_printBuffer(String pre, String msg, const uint8_t *buffer, uint16_t len) {

#ifdef DEBUG_PRINTBUFFER_AS_HEX
	String asOneHex;
	String asHex;
#endif

	String asTxt;
	String str0 = "0";
#ifdef DEBUG_PRINTBUFFER_AS_HEX
	String strBlanc = " ";
#else
	String strBlanc = "";
#endif
	String strDot = " .";
	String strCr = "\\r";
	String strLf = "\\n";
	String strNl = "\\0";
	if (!len) {
		return;
	}
	for (uint16_t i = 0; i < len; i++) {

#ifdef DEBUG_PRINTBUFFER_AS_HEX
		asOneHex = String(buffer[i], HEX);
		if (asOneHex.length() == 1) {
			asHex += str0 + asOneHex + DEBUG_PRINTBUFFER_SIGN_END;
		} else {
			asHex += asOneHex + DEBUG_PRINTBUFFER_SIGN_END;
		}
#endif

		if (isprint(buffer[i])) {
			asTxt += strBlanc + (char) buffer[i] + DEBUG_PRINTBUFFER_SIGN_END;
		} else {
			if (buffer[i] == '\r') {
				asTxt += strCr + DEBUG_PRINTBUFFER_SIGN_END;
			} else if (buffer[i] == '\n') {
				asTxt += strLf + DEBUG_PRINTBUFFER_SIGN_END;
			} else if (buffer[i] == '\0') {
				asTxt += strNl + DEBUG_PRINTBUFFER_SIGN_END;
			} else {
				asTxt += strDot + DEBUG_PRINTBUFFER_SIGN_END;
			}
		}
		if (i && !((i+1)%DEBUG_PRINTBUFFER_LINE_LENGTH)) {

#ifdef DEBUG_PRINTBUFFER_AS_HEX
			asHex += "\r\n";
#endif

			asTxt += "\r\n";
		}
	}

	Serial.print(pre.c_str());
	Serial.print(msg.c_str());
	Serial.print("[");
	Serial.print(len);
	Serial.print("]");
	Serial.print("\t<");
	Serial.print(asTxt.c_str());
	Serial.println(">");

#ifdef DEBUG_PRINTBUFFER_AS_HEX
	Serial.print(pre.c_str());
	Serial.print(msg.c_str());
	Serial.print("[");
	Serial.print(len);
	Serial.print("]");
	Serial.print("\t<");
	Serial.print(asHex.c_str());
	Serial.println(">");
#endif


}

void debug_printf(String pre, bool cr, String format, ...) {
	char debug_printf_bufIn[PRINTF_BUF];
	char debug_printf_buf[PRINTF_BUF];
	va_list ap;

	format.toCharArray(debug_printf_bufIn, PRINTF_BUF);
	va_start(ap, debug_printf_bufIn);
	vsnprintf(debug_printf_buf, sizeof(debug_printf_buf), debug_printf_bufIn,
			ap);
	Serial.print(pre.c_str());
	if (cr) {
		Serial.println(debug_printf_buf);
	} else {
		Serial.print(debug_printf_buf);
	}
	va_end(ap);
}
