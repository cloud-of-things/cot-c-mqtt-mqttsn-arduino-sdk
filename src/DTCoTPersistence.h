/**
 * @file DTCotPersistence.h
 * @description header file for a persistence singleton class
 * @to store CoT parameters.
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved.
 * @licence MIT licence
 */

#ifndef DT_COT_PERSISTENCE_HEADER_FILE
#define DT_COT_PERSISTENCE_HEADER_FILE

#define DT_COT_PERSISTENCE_DATA_START				0
#define DT_COT_PERSISTENCE_ITEM_COUNT               5
#define DT_COT_PERSISTENCE_SET_COUNT                5
#define DT_COT_PERSISTENCE_ITEM_LENGTH_MAX          32

#include <Arduino.h>
#include "devices/DTCotEEPROM.h"

/**
 * DTCoTPersistence.
 *
 * Singleton to use EEPROM emulation to store items.
 */

namespace DTCoT {

class DTCoTPersistenceItem {
public:
    DTCoTPersistenceItem();
    DTCoTPersistenceItem(String cotUser, String cotPassword,
            String tempUser, String tempKey, String moId);
    DTCoTPersistenceItem(const DTCoTPersistenceItem &obj);
    virtual ~DTCoTPersistenceItem();
    void clear();
    bool isCleared();
    void serialize(char* serializedObject);
    void deserialize(const char* serializedObject);

    String toString();

    String getCotUser();
    String getCotPassword();
    String getTempUser();
    String getTempKey();
    String getMoId();

    void setCotUser(const char* value);
    void setCotPassword(const char* value);
    void setTempUser(const char* value);
    void setTempKey(const char* value);
    void setMoId(const char* value);

private:
    String cotUser;
    String cotPassword;
    String tempUser;
    String tempKey;
    String moId;
};

class DTCoTPersistence {
public:
    DTCoTPersistence(CotEEPROM& eeprom);
    DTCoTPersistence(const DTCoTPersistence&);
    ~DTCoTPersistence();

public:
    /**
     * Read data from EEPROM to internal buffer.
     */
    void load();
    /**
     * Write internal buffer data to EEPROM.
     */
    void save();
    /**
     * Erase complete internal buffer data.
     */
    void clearAllItems();
    /**
     * Set actual item by user.
     *
     * @param String
     *      User to match.
     * @return true|false
     *      The persistence data item for matching user found.
     */
    bool setActualItemByUser(String user);
    /**
     * Set first cleared DTCoTPersistenceItem from internal array as actual set item..
     * !! Attention !! the next Item will be cleared!
     */
    void setActualItemCleared();
    /**
     * Get actual DTCoTPersistenceItem from internal array.
     *
     * @return DTCoTPersistenceItem
     *      The actual persistence data item.
     */
    DTCoTPersistenceItem& get();

    void printData(const char* caller = "");

private:
    DTCoTPersistence& operator =(const DTCoTPersistence&);
    int nextIndex(int index);

private:
    DTCoTPersistenceItem data[DT_COT_PERSISTENCE_SET_COUNT];
    int lastItemIndex;
    CotEEPROM& eeprom;
    char serializeBuffer[DT_COT_PERSISTENCE_SET_COUNT
            * DT_COT_PERSISTENCE_ITEM_COUNT * DT_COT_PERSISTENCE_ITEM_LENGTH_MAX];
};

} /* namespace DTCoT */

#endif /* DT_COT_PERSISTENCE_HEADER_FILE */

