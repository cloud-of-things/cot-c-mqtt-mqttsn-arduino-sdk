/**
 * @file DTCotPersistence.cpp
 * @implementation file for a persistence singleton class
 * @to store CoT parameters.
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */

#include "DTCoTPersistence.h"
#include "utility/DTCoTDebugOutput.h"

using namespace DTCoT;

#define DTPPERSISTENCELOGGER DT_DEBUG_PRINTLN_DEBUG
//#define DTPPERSISTENCELOGGER DT_DEBUG_PRINTLN_INFO

/* FIXME
 * This is a nasty workaround to get rid of compilation errors, since the AVR
 * EEPROM interface is incompatible with Huzzah.
 * Have to fix this asap.
 */
#ifndef ARDUINO_ARCH_AVR

DTCoTPersistenceItem::DTCoTPersistenceItem() :
		cotUser(""), cotPassword(""), tempUser(""), tempKey(""), moId("") {
}

DTCoTPersistenceItem::DTCoTPersistenceItem(String cotUser, String cotPassword,
		String tempUser, String tempKey, String moId) :
		cotUser(cotUser), cotPassword(cotPassword), tempUser(tempUser), tempKey(
				tempKey), moId(moId) {
}

DTCoTPersistenceItem::DTCoTPersistenceItem(const DTCoTPersistenceItem &obj) {
	this->cotUser = obj.cotUser;
	this->cotPassword = obj.cotPassword;
	this->tempUser = obj.tempUser;
	this->tempKey = obj.tempKey;
	this->moId = obj.moId;
}

DTCoTPersistenceItem::~DTCoTPersistenceItem() {
}

void DTCoTPersistenceItem::clear() {
	cotUser = "";
	cotPassword = "";
	tempUser = "";
	tempKey = "";
	moId = "";
}

bool DTCoTPersistenceItem::isCleared() {
	if (cotUser.length() || cotPassword.length() || tempUser.length()
			|| tempKey.length() || moId.length()) {
		return false;
	}
	return true;
}

void DTCoTPersistenceItem::serialize(char* serializedObject) {
	memset(serializedObject, 0,
	DT_COT_PERSISTENCE_ITEM_COUNT * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	char* p = serializedObject;
	int n = 0;
	strncpy(p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
			this->cotUser.c_str(), DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
			this->cotPassword.c_str(), DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
			this->tempUser.c_str(), DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
			this->tempKey.c_str(), DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX), this->moId.c_str(),
	DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
}

void DTCoTPersistenceItem::deserialize(const char* serializedObject) {
	char* p = (char*) serializedObject;
	int n = 0;
	char b[DT_COT_PERSISTENCE_ITEM_LENGTH_MAX];
	memset(b, 0, DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(b, p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
	DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	cotUser = b;
	memset(b, 0, DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(b, p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
	DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	cotPassword = b;
	memset(b, 0, DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(b, p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
	DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	tempUser = b;
	memset(b, 0, DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(b, p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
	DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	tempKey = b;
	memset(b, 0, DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	strncpy(b, p + (n++ * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX),
	DT_COT_PERSISTENCE_ITEM_LENGTH_MAX);
	moId = b;
}

String DTCoTPersistenceItem::toString() {
	return String(
			"DTCoTPersistenceItem cotUser<" + cotUser + "> cotPassword<"
					+ cotPassword + "> tempUser<" + tempUser + "> tempKey<"
					+ tempKey + "> moId<" + moId + ">");
}

String DTCoTPersistenceItem::getCotUser() {
	return cotUser;
}

String DTCoTPersistenceItem::getCotPassword() {
	return cotPassword;
}

String DTCoTPersistenceItem::getTempUser() {
	return tempUser;
}

String DTCoTPersistenceItem::getTempKey() {
	return tempKey;
}

String DTCoTPersistenceItem::getMoId() {
	return moId;
}

void DTCoTPersistenceItem::setCotUser(const char* value) {
	cotUser = value;
}

void DTCoTPersistenceItem::setCotPassword(const char* value) {
	cotPassword = value;
}

void DTCoTPersistenceItem::setTempUser(const char* value) {
	tempUser = value;
}

void DTCoTPersistenceItem::setTempKey(const char* value) {
	tempKey = value;
}

void DTCoTPersistenceItem::setMoId(const char* value) {
	moId = value;
}

DTCoTPersistence::DTCoTPersistence(CotEEPROM& eeprom) :
		lastItemIndex(0), eeprom(eeprom) {
	load();
}
DTCoTPersistence::~DTCoTPersistence() {

}

int DTCoTPersistence::nextIndex(int index) {
	if (++index == DT_COT_PERSISTENCE_SET_COUNT) {
		return 0;
	}
	return index;
}

bool DTCoTPersistence::setActualItemByUser(String user) {
	bool found = false;
	for (int i = 0; i < DT_COT_PERSISTENCE_SET_COUNT; i++) {
		if (data[i].getTempUser().equals(user)
				|| data[i].getCotUser().equals(user)) {
			lastItemIndex = i;
			found = true;
			DTPPERSISTENCELOGGER("DTCoTPersistence::setActualItemByUser() user<%s> -> [%d]<%s>", user.c_str(), lastItemIndex, get().toString().c_str());
			break;
		}
	}
	return found;
}

void DTCoTPersistence::setActualItemCleared() {
	for (int i = 0; i < DT_COT_PERSISTENCE_SET_COUNT; i++) {
		if (data[i].isCleared()) {
			lastItemIndex = i;
			break;
		}
		lastItemIndex = 0;
	} DTPPERSISTENCELOGGER("DTCoTPersistence::setActualItemCleared()... done. <%d>", lastItemIndex);
	data[nextIndex(lastItemIndex)].clear();
}

DTCoTPersistenceItem& DTCoTPersistence::get() {
	return data[lastItemIndex];
}

void DTCoTPersistence::clearAllItems() {
	for (int i = 0; i < DT_COT_PERSISTENCE_SET_COUNT; i++) {
		data[i].clear();
	}
	lastItemIndex = 0;
}

void DTCoTPersistence::save() {
	DTPPERSISTENCELOGGER("DTCoTPersistence::save()[%d]...", sizeof(serializeBuffer));
	char *p = serializeBuffer;
	for (int i = 0; i < DT_COT_PERSISTENCE_SET_COUNT; i++) {
		data[i].serialize(p);
		p += DT_COT_PERSISTENCE_ITEM_COUNT * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX;
	}
	eeprom.begin(sizeof(serializeBuffer));
	//eeprom.put(DT_COT_PERSISTENCE_DATA_START, serializeBuffer);
	eeprom.writeBulk(DT_COT_PERSISTENCE_DATA_START, sizeof(serializeBuffer),
			(const uint8_t*) serializeBuffer);
	delay(200);
	eeprom.commit();
	eeprom.end();
	DTPPERSISTENCELOGGER("DTCoTPersistence::save()[%d]... done.", sizeof(serializeBuffer));
}

void DTCoTPersistence::load() {
	eeprom.begin(sizeof(serializeBuffer));
	//eeprom.get(DT_COT_PERSISTENCE_DATA_START, serializeBuffer);
	eeprom.readBulk(DT_COT_PERSISTENCE_DATA_START, sizeof(serializeBuffer),
			(uint8_t*) serializeBuffer);
	eeprom.end();
	char *p = serializeBuffer;
	for (int i = 0; i < DT_COT_PERSISTENCE_SET_COUNT; i++) {
		data[i].deserialize(p);
		p += DT_COT_PERSISTENCE_ITEM_COUNT * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX;
	}
	lastItemIndex = 0;
}

void DTCoTPersistence::printData(const char* caller) {
	DTPPERSISTENCELOGGER("---- %s ------------------------------------", caller);
	for (int i = 0; i < DT_COT_PERSISTENCE_SET_COUNT; i++) {
		DTPPERSISTENCELOGGER("[%d] %s", i, data[i].toString().c_str());
	} DTPPERSISTENCELOGGER("----------------------------------------"); DTPPERSISTENCELOGGER("act. item[%d] %s", lastItemIndex, data[lastItemIndex].toString().c_str()); DTPPERSISTENCELOGGER("---- %s ------------------------------------", caller);
}
#endif /*ARDUINO_ARCH_AVR*/

