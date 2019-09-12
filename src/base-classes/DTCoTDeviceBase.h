/**
 * @file DTCoTDeviceBase.h
 * @description Basic class für all device classes
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 * 
 */

#ifndef DeviceBase_h_
#define DeviceBase_h_

#include <Client.h>
#include "DTCoTConfigBase.h"
#include "devices/DTCotEEPROM.h"

namespace DTCoT {

/**
 * Base class for each device.
 * A device is a specific piece of hardware (i.e, Tuino, HUZZAH).
 * This class abstracts away the hardware details of the platform. It should not be used directly.
 */
class CoTDeviceBase {
  public:
    /**
     * Constructor with config for a device.
     * @param deviceConfig The config file class should match the physical device's class.
     */
    CoTDeviceBase( const CoTConfigBase& deviceConfig);
    virtual ~CoTDeviceBase() {};

    /**
     * Begin using device
     */
    virtual bool init(long &timeoutMs) = 0;

	/**
	 * @brief Executes periodic processing (polling).
	 *
	 * More specific device classes should be re-implement this method.
	 */
	virtual bool isIdle() {return true;}

    /**
     * Get client for this device.
     * Every Arduino platform should have a Client implementation. This is a standard communication channel
     * such as WiFi or Ethernet, which we can use for communication with our CoT server.
     *
     * @TODO this may be abstracted further in the future - https://www.arduino.cc/en/Reference/ClientConstructor
     */
    virtual Client * getClient() const = 0;

    /** Returns the Config object. */
    const CoTConfigBase& getDeviceConfig() const;
    
    /** returns the Persistance object */
    // virtual CotEEPROM& getEEPROM() const = 0;
    virtual CotEEPROM& getEEPROM() = 0;

  protected:
    const CoTConfigBase& _deviceConfig;
};

}

#endif 
