/**
 * @file DTCoTDeviceGimasi.h
 * @description header file for Class for management and operations for Gimasi NB-IoT device
 * with the Deutsche Telekom Cloud of things MQTT-SN connector.
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_EEPROM_HEADER_FILE
#define DT_COT_EEPROM_HEADER_FILE

#include "utility/DTCoTDebugOutput.h"

namespace DTCoT {

   /**
    * Interface to abstract an enhanced EEPROM API for CoT Devices.
    */
class CotEEPROM {
  public:

    virtual ~CotEEPROM() {};

    /** Read one byte from given address */
    virtual uint8_t read(int address) =0;

    /** 
     * Read 'size' consecutive bytes from given address and copies them into 'target' buffer.
     * The 'target' has to have at least 'size' bytes space left.
     * Returns 'true' if ok, 'false' in case of error.
     */
    virtual bool readBulk(int const address, size_t size, uint8_t* target) =0;

    /** Writes the given bye into the given address. */
    virtual void write(int address, uint8_t value) =0;

    /** 
     * Write 'size' bytes from 'source' buffer into storage given by 'address'.
     * Returns the number of written bytes.
     */
    virtual size_t writeBulk(int const address, size_t size, const uint8_t* source) =0;

    /** Update a eeprom cell. */ 
    virtual void update(int address, uint8_t value) =0;

    /** 
     * Check whether the eeprom data is valid
     * @return true, if eeprom data is valid (has been written at least once), false if not
     */
    virtual bool isValid() =0;

    /** Get overall length of Memory Block. Returns 0 if not avaliable. */
    virtual size_t length() =0;

    /** Some implementations need to setup a memory block at beginning. */
    virtual void begin(size_t size) =0;

    /** Some implementations will only write/update the memory by calling this method. */
    virtual void commit() =0;

    /** Closing pair for 'begin'. */
    virtual void end() =0;
};

} /* namespace DTCoT */

#endif /* DT_COT_EEPROM_HEADER_FILE */

