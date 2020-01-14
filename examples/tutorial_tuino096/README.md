# Tutorial for the *Tuino 096* board to connect to the *Cloud of Things*

This tutorial describes how to connect to the *Cloud of Things* with the *Tuino 096* board.
# 0. Notice for the device tuino 096
it seems that there is a problem with the Arduino IDE on Linux and the tuino 096. If it stops responding and the LED flashes quickly, use the IDE in Windows to reactivate the device.

# 1. Install the Arduino IDE
* Download and unpack the Arduino IDE from https://www.arduino.cc/en/Main/Software
* start the IDE

# 2. Install the libraries and the driver

## 2.1. Install this Library
* Download this repository as ZIP:
  * on the GitHub Page click on the green *\[Clone or download\]* button at the upper right
  * *\[Download ZIP\]* to save the file `dt-arduino-iot-agent-master.zip` locally
* in Menu  `Sketch` -> `Include Library` -> `Add .ZIP Library...`
* choose the previously downloaded .ZIP and `Open` it

(see [more about installing this Library](../arduino_setup.md#installation))

## 2.2. Add the *Gimasi Tuino 096* board
* in Menu  `File` -> `Preferences`
* find the option "Additional Boards Manager URL" and add the following in the related text box:
`https://adafruit.github.io/arduino-board-index/package_adafruit_index.json,https://raw.githubusercontent.com/gimasi/TUINO_ZERO_96/master/arduino_ide/package_tuino0_index.json`  
(There must not be any blanks inside.)
* if you get errors after closing the dialog, reopen the `Preferences` dialog and in the `Network` tab check the *proxy* configuration.
* in the Menu `Tools` -> `Board` -> `Boards Manager...` -> search for `Tuino SAMD Boards (32-bits ARM Cortex-M0+)` and install it
* **on MS Windows**
  * in the lower black Panel of the IDE a litte message will appear:   
```Warning: non trusted contribution, skipping script execution (C:\Users\<you>\AppData\Local\Arduino15\packages\tuino0\hardware\samd\1.0.6\post_install.bat)```
(or the like)
  * open the file explorer (windows explorer) and navigate to that directory 
  * go deeper into `drivers` directory
  * if you have a *Windows 64bit* system (most likely) execute `dpinst-amd64.exe`
  * if you have a *Windows 32bit* system execute `dpinst-x86.exe`
  * you will be asked for administration rights, which you have to commit
  * and give it your trust
* (the device doesn't need to be pluged in your PC for now)
* Check the result: your new board should be listed in the OS's *Device Manager* and shouldn't be marked with issues.

(see [more about the basic setup](../arduino_setup.md#arduino-basic-setup))

## 2.3. Add 3rd party libraries
in Menu `Sketch` -> `Include Library` -> `Manage Libraries...` find and add these libraries (that are listed 
in the particular example sketch as *dependent*):  
```
        - <Adafruit_MQTT_Library> v0.20
        - <Time> v1.5
        - <FlashStorage> v0.7.1
        - <ArduinoSTL> v1.1.0
```
so you have to search for the libraries `Adafruit MQTT Library`, `Time` (by Michael Margolis), 
`FlashStorage` and `ArduinoSTL` (in at least the given releases) and install them.  
The `Time` library is hard to find. As advice filter the list for *Topic* "Timing"; the desired entry is (currently) at the bottom end of the upcoming list.

## 2.4 Prepare your Cloud *Tenant*
Concept: your *CoT Tenant* is that stuff behind something like `https://<your_tenant>.ram.m2m.telekom.com` which you got provided after registering at the *Coud of Things*. It's your playground in the CoT.

**If you are the first (or only) user of your CoT tenant**, you have to prepare it a little bit:
* login to this tenant with your provided User's *name* and *password*
  * Note: your credentials are case sensitive!
* In your Tenant go to *\[Administration\]* (in the upper right hand corner, click the icon with the nine squares) *> \[Applications\]* and check, if an entry *Cotarduionosdkhelper* is already listed. If not...
* klick *\[Add  application\]*, choose *\[Upload ZIP file\]* and point to [https://github.com/tsi5/cot_arduino_sdk_helper_app/releases/download/v1.0/cotarduionosdkhelper.zip](https://github.com/tsi5/cot_arduino_sdk_helper_app/releases/download/v1.0/cotarduionosdkhelper.zip) (you may need to download it first).
* After that you should have a new application `Cotarduionosdkhelper` available in you *Apps* menu. 
  * If no page is displayed after opening this app, choose *Template manager* in the left menu bar.
  * Also you may need to clear you browser cache, if there was an old version of the plugin installed before.
* You finally should get the *SmartREST Template Collection Manager* displayed.
* upload the content of [smartrest_templates.csv](src/cot_sdk/resources/smartrest_templates.csv) - that is, copy and paste the text into the input field below the \[upload Template\] button, set the name **`CoT-MQTT-Arduino_Lib`** into the field at the left of the \[upload Template\] button and finally press that button.

(see more about the [Cloud's SmartREST Preparation steps](./src/cot_sdk/resources/README.md))

# 3. Fire up the board
* Connect the Tuino to your PC with the USB cable 
  ![Tuino096_overview](doc/Tuino096_overview.png)
* the driver should be initialized automatically. Wait until this is finished.
* After this step check the result: your new board should be listed in the OS's *Device Manager* and shouldn't be marked with issues.
* If not, please follow the hints from the [boards page](https://github.com/gimasi/TUINO_ZERO_96).  
* :sweat_drops: In this situation it is recommended to press the *reset button*. Then the programming port should be opended by the board and the driver instance for this link should be initialized also.

As a result you should be able to check the modes in the (windows) **device manager**:
* in **default mode** an  *Arduino M0 Native Port (COM_yy_)* should appear
* in **programming mode** an  *Arduino M0 (Bootloader Mode) Native Port (COM_zz_)* is listed (other port!)

# 4. Start the Tutorial
Now your development environment and the hardware is ready.  
Time to start the game.

## Lesson 01: The Board
* In the Arduino IDE open the example `File`->`Examples`->`dt-arduino-iot-agent`-> `tutorial_tuino096`->`lesson_01_tuino096`
* In `Tools`->`Boards` choose *Tuino 096*
* In `Tools`->`Port` choose the COM Port that should be assigned to your board after driver installation
* Open the *Serial Monitor* (alias *Console* alias *Terminal*) by `Tools`->`Serial Monitor`, set the *speed* to `115200` and place the window beneath
* program the board with `Sketch`->`Upload`
* If an error occurrs (saying that it can't find the board) press the board's *Reset Button* right before clicking *Upload* again
* **Result**
  * in the *Serial Monitor* the line `avrdude done.  Thank you.` should be printed finally
  * the yello LED aside of the antenna plug names `STAT` should blink
  * in the *Serial Monitor* some text should be printed continuous

## Lesson 02: Device Registration (in the Cloud)
* open `File`->`Examples`->`dt-arduino-iot-agent`-> `tutorial_tuino096`->`lesson_02_device_registration`
* read the sketch and optionally use the uncommented functions.
* give your device a **Name**: 
  * set it in the line `const char* DEVICE_NAME = "<my_DEVICE_NAME>";`
  * :zap: its important that name is **globaly unique**!  
    As advice you can prefix your name with your tenant, e.g. `myTenant.testDevice_1`
* upload the sketch and also open the *Serial Monitor*.  
After some lines a block like this should be printed:   
```
 #################################################################
 ##
 ##       Please register device now in the cloud
 ##       (or save valid credentials for it, if registered)
 ##
 ##       <your device's name>
 ##
 ##       and push button to acknowledge.
 ##
 ##       Then please be patient and wait (about 1 min).
 ##
 #################################################################
```
* in your *tenant* register your board:
  * under *\[Device Management\] > \[Register Device\]* (respectively *\[Registration\]*)  
.
![Tuino096_overview](doc/cot_device_registration.PNG)  
.
  * first _Register device_, do a _General device registration_ and give it the _Device ID_ 
    which is your *Device Name* from the devices *Serial Monitor*. Ignore the *group* for now.
 * a new entry should raise with the status _Waiting_, but it should change to _Accept_ 
   after some seconds. 
 * Confirm by pressing \[Accept\].
* in the consol of your hardware the CoT's credentials get printed out:   
```
 ######################################################
 ##
 ##       Device is registered in the cloud.
 ##       Credentials:
 ##         moId     <moId>
 ##         name     <name>
 ##         password <password>
 ##
 ##       Attention!
 ##
 ##       Credentials will not be displayed again!
 ##       Please note carefully!
 ##
 ######################################################
```
 * At the end in your CoT portal there should appear a new entity in *\[Device Management\] > \[All Devices\]*. 
 * A click on the entry should show the details of your new device, the posted measurements, operations and alarms. Note the URL; it is your **Device's CoT Page**.
* in your sketch save the above device *credentials* in the lines 
```
const char* DEVICE_NAME =	"<my_DEVICE_NAME>";
const char* DEVICE_Passwd =	"<my_device_passwd>";
const char* DEVICE_moID =	"<my_device_moID>";
```
* check the credentials by uploading the patched sketch again:
  * the *Serial Monitor* log should look different to the first run (no registration sequence) and 
  * in the cloud look at the devices page: in *Last updated* it should show a recent timepoint
* (more details and problem descriptions can be found in the [Device Registration Process Page](cot_device_registration.md#device-registration))

## Lesson 03: Send Measurements
* open `File`->`Examples`->`dt-arduino-iot-agent`-> `tutorial_tuino096`->`lesson_03_send_measurements`
* copy *device credentials* lines from the previous sketch
* only new line is with `cotDevice.sendMeasurement(...)`
* upload sketch and wait 30s
* go to your devices cloud page and in **Measurements** you should see a new diagram named "Room 1"
* watch the (simulated) growing graph

## Lesson 04: Send Alarms
* open `File`->`Examples`->`dt-arduino-iot-agent`-> `tutorial_tuino096`->`lesson_04_send_alarms`
* copy *device credentials* lines from the previous sketch
* only new line is with `cotDevice.sendAlarm(...)`
* upload sketch and wait ~ 2min
* go to your devices cloud page and in **Measurements** you should see a new diagram named "room 1"
* watch the growing graph
* after the (simulated) temerature reaches the treshold (22°C) an *alarm* is displayed in **Alarms**

## Lesson 05: Operations
* open `File`->`Examples`->`dt-arduino-iot-agent`-> `tutorial_tuino096`->`lesson_05_operations`
* copy *device credentials* lines from the previous sketch
* upload sketch
* go to your devices cloud page, in **>_Shell** type `led blink` and press *\[Execute\]*
* after some second the command alias operation gets EXECUTING and then *v/* SUCCESSFUL finished.
* your device should blink now

## Lesson 06: NB-IoT
### Prerequisites
* you need a  SIM card (at least) for **NB-IoT** access
* your CoT Tenant needs to be prepared for NB-IoT with MQTT-SN
* your NB-IoT SIM has to be registered at your CoT tenant
* as result you get prepared with a **device password**. (Don't mess this password up with your CoT user password)
### Programming
* open `File`->`Examples`->`dt-arduino-iot-agent`-> `tutorial_tuino096`->`lesson_06_nbiot`
* fill in the *device NB-IoT CoT passwort* by patching the sketch:  
paste it in at  
``` #define NB_IOT_MQTTSN_PWD     "<password for access to the MQTT-SN connector>" ```
* the sketch sends measurements. The function is `cotDevice.sendMeasurement(...)` and looks same as in the GSM case, but it has essential differences:
  * the 'measurement' type is restricted, 
  * the parameters 'type' and 'unit' are ignored
* upload sketch and wait 30s
* go to your devices cloud page this in **not** the page of the above situation, but the ID and thus the page have the name of your IMSI+Password
* in **Measurements** you should see a new diagram named "Temperature"
* watch the (simulated) growing graph

### Notes for NB-IoT "Mode"
* the identity of the devices in "NB-IoT mode" is diffenent to that of the "GSM mode"
  * this isn't a problem, as you can switch the "identities" by switching the sketches
  * but - for now - you can't make a failover from one mode to the other (or kind like that)
* you can't send Alarms
* you can't *update* the device infos
* you can't return a result (text) message for operations (but the status)
* you can't take another (NB-IoT) identity for your device
* you can't use tenants at will; the NB-IoT credential is bound to a specific tenant 
