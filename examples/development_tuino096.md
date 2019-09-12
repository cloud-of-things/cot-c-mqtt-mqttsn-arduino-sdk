# Development with *Tuino 096*

## Board Design
The *Tuino 096* utilizes a *SAMD21* MCU as primary processor.  
This uses two different logical serial ports, one for programming and one for common serial line.  
So it happens that the MCU (or you, if thee automatic mechanism hangs) has to switch the port every time you either upload a sketch.

At the board the two "modes" are switched manually by pressing the reset button.  
The board changes to *programming mode* for **8 sec** and then falls back to *default mode*.

For more information about this board see http://www.gimasi.ch/productions-platforms/tuino096/ and the developer's site https://github.com/gimasi/TUINO_ZERO_96 .

:warning: :bomb: :electric_plug: **Caution**: The microcontroller on the Tuino runs at **3.3V**, while most common boards like the Arduino Mega runs at 5V. You must never apply more than 3.3V to the Tuino's inputs or outputs. When you connect sensors and actuators to the Tuino always take care that the maximum voltage limits are not exceeded on the pins. Connecting higher voltages, like the 5V commonly used with the other Arduino boards, will damage the Zero. If in doubt measure the IORef or the VCC pin, which supplies the voltage corresponding to the i/o of your board. 

## Driver Installation

* Connect the Tuino to your PC with the USB cable 
  ![Tuino096_overview](doc/Tuino096_overview.png)
* the driver should be initialized automatically. Wait until this is finished.
* After this step check the result: your new board should be listed in the OS's *Device Manager* and shouldn't be marked with issues.
* If not, please follow the hints from the [boards page](https://github.com/gimasi/TUINO_ZERO_96).  
* :sweat_drops: In this situation it is recommended to press the *reset button*. Then the programming port should be opended by the board and the driver instance for this link should be initialized also.

As a result you should be able to check the modes in the (windows) **device manager**:
* in **default mode** an  *Arduino M0 Native Port (COM_yy_)* should appear
* in **programming mode** an  *Arduino M0 (Bootloader Mode) Native Port (COM_zz_)* is listed (other port!)

## The Development Process

### The *standard* Development Process

:exclamation: First try this process. Only if you have problems, try the *manual* Development Process described below.

* start up your Arduino IDE and select the "Tuino 096" board support in Arduino (menu: Tools->Board->"Tuino 096").  
If you can't find the board [please re-check your Arduino IDE setup](../../arduino_setup.md).
* open your sketch (which should be made for the Tuino096)
* Compile the sketch and start uploading
* if an error "Serial Port not found" (or that alike) rises, then press the *RESET* button on the board
* Open the *Serial Monitor* to see the device's activity (menu Tools->Serial Monitor)

### The *manual* Development Process

The following section describes the *manual* Development Pocess, which you have to follow, if the automatic USB port switch mechanism hangs; this is, if you have problems with the common process of using the default port in the Arduino IDE.

* start up your Arduino IDE and select the "Tuino 096" board support in Arduino (menu: Tools->Board->"Tuino 096").  
If you can't find the board [please re-check your Arduino IDE setup](../../arduino_setup.md).
* open your sketch (which should be made for the Tuino096)
* **in advance to each upload sequence** close your *Serial Monitor* if it is open (because else the programming port may be blocked later)
* Compile the sketch and start uploading
* **just in the moment** when the message "`Sketch uses [...] bytes [...]`" (english) respectively "`Sketch verwendet [...]`" in the lower log window appears...
 * press the board's reset button to bring the Tuino into _programming mode_ .
 * in the IDE select the *programming port* (menu: Tools->Port...).  
* After a while the success of the programming should be printed.
* Maybe you have to press the reset button multiple times until the programming starts.
* While programming the board holds the *Bootloader Mode*.  
* After programming - or by common power up - the board opens the default port.  
This is the serial monitor interface!
So to see what's going on... 
 * in the IDE reselect the Port to use that link
 * Open the *Serial Monitor* to see the device's activity (menu Tools->Serial Monitor)

### Other Problems

* if there is logged an error like  
`[C]DTCoTModemBase::_readAndParseResponse(select operator) `**`Modem receive problem! Please check SIM card and modem firmware!`**  
then you should try to **update the modems firmware**. For an complete guide please consulte the [GIMASIs TUINO 096 Technical Page](https://github.com/gimasi/TUINO_ZERO_96), chapter "*FIRMWARE UPGRADE OF BG96*".
