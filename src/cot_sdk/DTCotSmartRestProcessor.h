/**
 * @file DTCotSmartRestProcessor.h
 * @description header file for an Interface for handling communicatin via Cumulocity 'SmartREST'.
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef SmartREST_h_
#define SmartREST_h_

#include <Time.h>
#include "DTCotSdkTypes.h"
#include "base-classes/DTCoTCommunicationBase.h"
#include "base-classes/DTCotOperation.h"
#include "DTCoTPersistence.h"
#include <vector>
#include "communication/DTCotCommunicationMQTT.h"
#include "DTCotOperationsEngine.h"

#define COT_SMARTREST_PROCESSOR_TIMEOUT_DEFAULT 				5000
#define COT_SMARTREST_DELIMITER 								','
#define COT_SMARTREST_PROCESSOR_READ_SUBSCRIPTION_BUFFER_SIZE 	1000

#define COT_SMARTREST_RESPONSE_LIB_MSGID						87
#define COT_SMARTREST_RESPONSE_MSGID_INDEX						0
#define COT_SMARTREST_RESPONSE_LINE_INDEX						1
#define COT_SMARTREST_RESPONSE_MOID_INDEX						2
#define COT_SMARTREST_RESPONSE_ERROR_INDEX						2
#define COT_SMARTREST_RESPONSE_STATE_INDEX						3
#define COT_SMARTREST_RESPONSE_OPERATION_MSGID_FIRST			201
#define COT_SMARTREST_RESPONSE_OPERATION_MSGID_LAST				299
#define COT_SMARTREST_RESPONSE_LAST_ERROR_MSGID					100


//#define COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
//#define COT_SMART_REST_CALL_ITEM_SHOW_INSTANCE_INFO

class CotSmartRestCallItem;

class CotSmartRestResponseItem {
public:
    CotSmartRestResponseItem();
    CotSmartRestResponseItem(const CotSmartRestResponseItem& item);
    ~CotSmartRestResponseItem();
    void reset();
    void copy(CotSmartRestResponseItem &target);
    int messageId;
    int lineId;
    bool isError;
    String error;
    char delimiter;
    CotSmartRestCallItem *callItem;
    std::vector<String> messageParts;

    void printInstanceInfo(const char* caller);
#ifdef COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
    unsigned long instNumber;
    static unsigned long instCount;
    static unsigned long instCounter;
#endif
};

class CotSmartRestCallItem {
public:
    CotSmartRestCallItem();
    CotSmartRestCallItem(const CotSmartRestCallItem& item);
    CotSmartRestCallItem(int smartRestId, String content, bool expectResponse = true, CotSmartRestCallItem *parentItem = NULL);
    ~CotSmartRestCallItem();
    void setParent(CotSmartRestCallItem *parentItem);
    void reset();
    void copy(CotSmartRestCallItem &target);
    int messageId;
    int lineId;
    char delimiter;
    String message;
    bool expectResponse;
    CotSmartRestResponseItem responseItem;
    CotSmartRestCallItem *nextCallItem;
    CotSmartRestCallItem *parentCallItem;

    void printInstanceInfo(const char* caller);
#ifdef COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
    unsigned long instNumber;
    static unsigned long instCount;
    static unsigned long instCounter;
#endif
};

namespace DTCoT {

/**
 * This interface shall summarize and define the cloud connection via 
 * Cumulocity's SmartREST protocol.
 * It uses either the MQTT or the REST communication channel for this.
 * 
 * The SmartREST always is crucial dependent from the utilized
 * <b>SmartREST Template Collection</b>.
 * So this interface shall be an abstraction on this;
 * concrete Template Collection implementations have to be realized by
 * classes implementing this interface.
 */
class CotSmartRestProcessor : public CotOperationsEngineAdapter {
    // == Methods ==
public:
    /**
     * C-Tor defines the object dependencies:
     * the communication channel to us (MQTT or REST) and
     * the credential to get access to the cloud.
     *
     * @param device interface to this hardware platform.
     */
    CotSmartRestProcessor(const CoTDeviceBase& device, const String host,
            unsigned long port, const String xId,
            DTCoTPersistence& persistence);

    ~CotSmartRestProcessor();

    /**
     * Creates and registers a 'ManagedObject' in the Cloud for this device.
     * @return the 'ManagedObject-ID' of the newly created cloud object.
     */
    bool createAndRegisterDevice();

    /**
     * Update of device attributes in the CoT cloud.
     *
     * @param maxlen Maximal total length of update strings, device name and template name.
     *    It is necessary to avoid buffer overflow at PUB / SUB MQTT messages in Adafruit_MQTT.
     *    This value 137 applies to #define MAXBUFFERSIZE (150)  in Adafruit_MQTT.h.
     *    If MAXBUFFERSIZE will be greater, then this default value may be corresponding increased.
     */
    bool updateDevice(String moId, const String hwModel, const String hwVersion,
            const String swVersion, const String imei, const String iccid,
            const String imsi, int maxlen);

    bool sendMeasurement(String moId, const String type, const String model,
            const String measurement, const String unit, const float value,
            const time_t timeStamp);

    bool sendAlarm(String moId, const String text, const String type,
            const AlarmSeverity severity, const AlarmStatus status);

    static String createCloudTimestring(time_t t);

    static String getCloudTimestringNow();

    int readSubscription(const char* caller, int &errCount, int &receivedLen, int &newCallResponseItems, int &newResponseOnlyItems, int timeout = 100);

    String getOrCreateChildDevice(String moId, String childDeviceName);

    void triggerRetrievingOperations(String moId, int timeout, bool trigger);

    std::vector<CotOperationData> getRetrievedOperations();

    void respondOperationsState(std::vector<String> messageParts,
					OperationStatus status, const String result, const String text);

protected:
    /// Builds the first line of a SmartREST Request: "15,<X-ID>\n"
    String head();
    /// Makes a Request via MQTT to the Cloud and waits for a response synchonoulsy 
    /// until the timeout expires.
    bool cotRequestSync(const char* stepMsg, CotSmartRestCallItem &callItem,
            int timeout =
            COT_SMARTREST_PROCESSOR_TIMEOUT_DEFAULT);
    /// scans the SmartREST Response for Error
    static bool hasError(CotSmartRestResponseItem &responseItem);

private:
    bool triggerCloud(String moId);

    /**
     * Check if the message to the cloud exceeds the maximum possible size.
     *
     * @param updateMsg Essential part of the message.
     * @param threshold Threshold for the composite message (137/487).
     */
    bool checkUpdateMsgLength(String updateMsg, int threshold);

    bool setCloudOperationState(String operationId, OperationStatus status,
            const char* result = NULL, const char* text = NULL);

    static void appDigits(char pre, int value, String& ts);
    static void setClockByCloudTimestring(String cloudtimestring);

    static int parsePartCount(const String& source, char delimiter);
    static String parseNext(String& source, char delimiter);
    static String parsePart(const String &source, char delimiter, int index);
    static int parsePartInt(const String &source, char delimiter, int index);
    static String parseLine(const String &source, int line);
    static String parseResponse(const String &source, int line, char delimiter,
            int part);
    static int responseParser(const char* caller, const String &response, int &newCallResponseItems, int &newResponseOnlyItems,
            std::vector<CotSmartRestCallItem> &callItems,
            std::vector<CotSmartRestResponseItem> &operationResponseOnlyItems);
    static void printCallItem(const char* caller, int index, const CotSmartRestCallItem *item);
    static void printResponseItem(const char* caller, int index,
            const CotSmartRestResponseItem &item);
    static void printCallItems(const char* caller, const std::vector<CotSmartRestCallItem> &items);
    static int countCallItemsErrors(const std::vector<CotSmartRestCallItem> &items);
    static void printResponseItems(const char* caller,
            const std::vector<CotSmartRestResponseItem> &items);

    static CotSmartRestCallItem *getCallItemByResponseItem(CotSmartRestResponseItem *callItem, std::vector<CotSmartRestCallItem> &items);

//  == Members ==
protected:
    CoTCommunicationMQTT *comm;
    String xID; ///< the X-ID to be used

private:
    DTCoTPersistence& _persistence;
    const CoTDeviceBase& _device;
    String subBuffer;
    String pubBuffer;
    Adafruit_MQTT_Subscribe *deviceTopic;
    bool isClockSet;
    CotSmartRestCallItem callItemHead;

    std::vector<CotSmartRestCallItem> callItems;
    std::vector<CotSmartRestResponseItem> operationResponseOnlyItems;

    unsigned char readSubscriptionBuffer[COT_SMARTREST_PROCESSOR_READ_SUBSCRIPTION_BUFFER_SIZE];
};

}

#endif // SmartREST_h_

