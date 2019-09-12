/**
 * @file DTCoTDebugOutput.h
 * @description Global debug defines. Change these to change the debug logging target.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DEBUG_PRINT_h_
#define DEBUG_PRINT_h_

#include <Arduino.h>
#include <stdarg.h>

/**
 * Debug levels.
 */
//never print anything
#define DT_DEBUG_LEVEL_NONE		0
//print only critical  logs
#define DT_DEBUG_LEVEL_CRITICAL	1
//print also error     logs
#define DT_DEBUG_LEVEL_ERROR	2
//print also info      logs
#define DT_DEBUG_LEVEL_INFO		3
//print also debug     logs
#define DT_DEBUG_LEVEL_DEBUG	4

#ifndef DT_DEBUG_LEVEL
//#define DT_DEBUG_LEVEL DT_DEBUG_LEVEL_NONE
//#define DT_DEBUG_LEVEL DT_DEBUG_LEVEL_CRITICAL
//#define DT_DEBUG_LEVEL DT_DEBUG_LEVEL_ERROR
#define DT_DEBUG_LEVEL DT_DEBUG_LEVEL_INFO
//#define DT_DEBUG_LEVEL DT_DEBUG_LEVEL_DEBUG
#endif /* DT_DEBUG_LEVEL */

/**
 * Wrapper for formatting to debug output.
 * If you want to format Arduino strings, just concatenate them with a + operator.
 */
void debug_printf(String pre, bool cr, String format, ...);
void debug_printBuffer(String pre, String msg, const uint8_t *buffer, uint16_t len);

#if DT_DEBUG_LEVEL == DT_DEBUG_LEVEL_NONE
#define DT_DEBUG_PRINTLN(...)
#define DT_DEBUG_PRINT(...)
#define DT_DEBUG_PRINTLN_CRITICAL(...)
#define DT_DEBUG_PRINT_CRITICAL(...)
#define DT_DEBUG_PRINTLN_ERROR(...)
#define DT_DEBUG_PRINT_ERROR(...)
#define DT_DEBUG_PRINTLN_INFO(...)
#define DT_DEBUG_PRINT_INFO(...)
#define DT_DEBUG_PRINTLN_DEBUG(...)
#define DT_DEBUG_PRINT_DEBUG(...)
#define DT_DEBUG_PRINTBUFFER(msg, buffer, len) { }
#define DT_DEBUG_PRINTBUFFER_CRITICAL(msg, buffer, len) { }
#define DT_DEBUG_PRINTBUFFER_ERROR(msg, buffer, len) { }
#define DT_DEBUG_PRINTBUFFER_INFO(msg, buffer, len) { }
#define DT_DEBUG_PRINTBUFFER_DEBUG(msg, buffer, len) { }

#elif DT_DEBUG_LEVEL == DT_DEBUG_LEVEL_CRITICAL
#define DT_DEBUG_PRINTLN(...)  debug_printf("", true, __VA_ARGS__)
#define DT_DEBUG_PRINT(...)  debug_printf("", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_CRITICAL(...)  debug_printf("[C]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_CRITICAL(...)  debug_printf("[C]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_ERROR(...)
#define DT_DEBUG_PRINT_ERROR(...)
#define DT_DEBUG_PRINTLN_INFO(...)
#define DT_DEBUG_PRINT_INFO(...)
#define DT_DEBUG_PRINTLN_DEBUG(...)
#define DT_DEBUG_PRINT_DEBUG(...)
#define DT_DEBUG_PRINTBUFFER(msg, buffer, len) { debug_printBuffer("", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_CRITICAL(msg, buffer, len) { debug_printBuffer("[C]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_ERROR(msg, buffer, len) { }
#define DT_DEBUG_PRINTBUFFER_INFO(msg, buffer, len) { }
#define DT_DEBUG_PRINTBUFFER_DEBUG(msg, buffer, len) { }

#elif DT_DEBUG_LEVEL == DT_DEBUG_LEVEL_ERROR
#define DT_DEBUG_PRINTLN(...)  debug_printf("", true, __VA_ARGS__)
#define DT_DEBUG_PRINT(...)  debug_printf("", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_CRITICAL(...)  debug_printf("[C]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_CRITICAL(...)  debug_printf("[C]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_ERROR(...)  debug_printf("[E]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_ERROR(...)  debug_printf("[E]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_INFO(...)
#define DT_DEBUG_PRINT_INFO(...)
#define DT_DEBUG_PRINTLN_DEBUG(...)
#define DT_DEBUG_PRINT_DEBUG(...)
#define DT_DEBUG_PRINTBUFFER(msg, buffer, len) { debug_printBuffer("", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_CRITICAL(msg, buffer, len) { debug_printBuffer("[C]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_ERROR(msg, buffer, len) { debug_printBuffer("[E]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_INFO(msg, buffer, len) { }
#define DT_DEBUG_PRINTBUFFER_DEBUG(msg, buffer, len) { }

#elif DT_DEBUG_LEVEL == DT_DEBUG_LEVEL_INFO
#define DT_DEBUG_PRINTLN(...)  debug_printf("", true, __VA_ARGS__)
#define DT_DEBUG_PRINT(...)  debug_printf("", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_CRITICAL(...)  debug_printf("[C]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_CRITICAL(...)  debug_printf("[C]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_ERROR(...)  debug_printf("[E]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_ERROR(...)  debug_printf("[E]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_INFO(...)  debug_printf("[I]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_INFO(...)  debug_printf("[I]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_DEBUG(...)
#define DT_DEBUG_PRINT_DEBUG(...)
#define DT_DEBUG_PRINTBUFFER(msg, buffer, len) { debug_printBuffer("", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_CRITICAL(msg, buffer, len) { debug_printBuffer("[C]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_ERROR(msg, buffer, len) { debug_printBuffer("[E]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_INFO(msg, buffer, len) { debug_printBuffer("[I]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_DEBUG(msg, buffer, len) { }

#else
#define DT_DEBUG_PRINTLN(...)  debug_printf("", true, __VA_ARGS__)
#define DT_DEBUG_PRINT(...)  debug_printf("", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_CRITICAL(...)  debug_printf("[C]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_CRITICAL(...)  debug_printf("[C]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_ERROR(...)  debug_printf("[E]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_ERROR(...)  debug_printf("[E]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_INFO(...)  debug_printf("[I]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_INFO(...)  debug_printf("[I]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTLN_DEBUG(...)  debug_printf("[D]", true, __VA_ARGS__)
#define DT_DEBUG_PRINT_DEBUG(...)  debug_printf("[D]", false, __VA_ARGS__)
#define DT_DEBUG_PRINTBUFFER(msg, buffer, len) { debug_printBuffer("", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_CRITICAL(msg, buffer, len) { debug_printBuffer("[C]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_ERROR(msg, buffer, len) { debug_printBuffer("[E]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_INFO(msg, buffer, len) { debug_printBuffer("[I]", msg, buffer, len); }
#define DT_DEBUG_PRINTBUFFER_DEBUG(msg, buffer, len) { debug_printBuffer("[D]", msg, buffer, len); }

#endif /* DT_DEBUG_LEVEL */

#endif /* DEBUG_PRINT_h_ */
