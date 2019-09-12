/*
 * DTCoTSDKBase.cpp
 *
 *  Created on: 08.06.2018
 *      Author: akrause
 */

#include "utility/DTCoTDebugOutput.h"
#include "cot_sdk/DTCotSdkMqttSN.h"
#include "utility/Regexp.h"
#ifdef ARDUINO_1284P_STANDARD
/*FIXME ak this shouldn't be here!*/
#include "protocols/gmx_nbiot.h"
#endif /*ARDUINO_1284P_STANDARD*/

namespace DTCoT {

CotSdkMqttSN *activeCotSdkMqttSN = NULL;



bool CotSdkMqttSN::init(long &) //timeoutMs)
{
    activeCotSdkMqttSN = this;
    int ret = mqttsn->init();
	operationsEngine.init( this );
	if ((ret == 0) || (ret == -1)) {
		return true;
	}
	return false;
}


//bool CotSdkMqttSN::updateDevice(const String hwModel, const String hwVersion,
//		const String swVersion, const String imei , const String iccid,
//		const String imsi, int maxlen)
bool CotSdkMqttSN::updateDevice(const String, const String,
		const String, const String, const String,
		const String, int)
{
    DT_DEBUG_PRINTLN_ERROR("CotSdkMqttSN::updateDevice: Not supported for the MQTT-SN protocol.");
    return false;
}


int CotSdkMqttSN::GetTopicId(const String type, const String model, const String measurement)
{
	String myTopicString = type + model + measurement;
	MqttSnMeasEnumT measurementId = MQTTSN_MEAS_TYPE_INVALID;
	int myMqttsnTopicId = MQTTSN_TOPIC_INVALID_ID;

	DT_DEBUG_PRINTLN_DEBUG("CotSdkMqttSN::GetTopicId: type<%s> model<%s> measurement<%s>",
						type.c_str(), model.c_str(), measurement.c_str());

	if ( ! mqttsn->connected() ) {
		DT_DEBUG_PRINTLN_INFO("CotSdkMqttSN::GetTopicId: network not connected => reset Topics");
		vActiveTopic.clear();
	}

	for(std::vector<TopicStructT>::iterator it = vActiveTopic.begin(); it != vActiveTopic.end(); ++it)
	{
		if(myTopicString == it->name) {
			/*topic id already registered*/
			DT_DEBUG_PRINTLN_DEBUG("Already registered topic. ID: %d", it->id);
			return (it->id);
		}
	}

	/*No matching topic found, find related measurement id*/
	DT_DEBUG_PRINTLN_DEBUG("Registering new topic...");
	for(std::vector<TopicNameIdT>::iterator it = vMeasType.begin(); it != vMeasType.end(); ++it)
	{
		if(measurement == it->name) {
			measurementId = it->measTypeId;
		}
	}

	/*obtain topic id now*/
	if(measurementId == MQTTSN_MEAS_TYPE_INVALID)
	{
		DT_DEBUG_PRINTLN_INFO("Registering non-measurement topic: <%s>",myTopicString.c_str());
		myMqttsnTopicId = mqttsn->RegisterTopicDTCoT((char*)(myTopicString.c_str()));
	}
	else{
		DT_DEBUG_PRINTLN_INFO("registering measurement topic: Name<%s> ID<%d>",
						measurement.c_str(), measurementId);
		myMqttsnTopicId = mqttsn->RegisterTopicDTCoT(MQTTSN_TOPIC_MEASUREMENT, measurementId);
	}

	if(myMqttsnTopicId == MQTTSN_TOPIC_INVALID_ID)
	{
		/*TODO topic reg failed, disconnect / restart connect*/
		DT_DEBUG_PRINTLN_ERROR("topic registering failed (0)");
	}
	else {
		DT_DEBUG_PRINTLN_INFO("topic registered");
		DT_DEBUG_PRINTLN_INFO("myMqttsnTopicId: %d", myMqttsnTopicId);

		/*topic registered, store it in vector*/
		TopicStructT mynewTopic;
		mynewTopic.name = myTopicString;
		mynewTopic.id = myMqttsnTopicId;
		vActiveTopic.push_back(mynewTopic);
		DT_DEBUG_PRINTLN_INFO("mynewTopic: %s %d", mynewTopic.name.c_str(), mynewTopic.id);
	}
	return myMqttsnTopicId;
}



bool CotSdkMqttSN::sendMeasurement(const String type, const String model,
        const String measurement, const String unit, const float value,
        const time_t timeStamp)
{
	DT_DEBUG_PRINTLN_INFO("CotSdkMqttSN::sendMeasurement: type<%s> model<%s> measurement<%s> unit<%s> value<%f> timeStamp<%li>",
						type.c_str(), model.c_str(), measurement.c_str(), unit.c_str(), value, timeStamp);

	/*get topic id, automatically register the topic if necessary */
	int myMqttsnTopicId = GetTopicId(type, model, measurement);

	if(myMqttsnTopicId == MQTTSN_TOPIC_INVALID_ID)
	{
		DT_DEBUG_PRINTLN_ERROR("topic not registered");
		return false;
	}

	/*send measurement*/
	char tempCstr[15];
#ifdef ARDUINO_ARCH_AVR
    dtostrf(value, 2, 1, tempCstr);
#else
    sprintf(tempCstr, "%3.1f", value);
#endif /*ARDUINO_ARCH_AVR*/

    // Sending of measurements at QoS 1 is more secure, than QoS 0.
    if ( ! mqttsn->publish(FLAG_QOS_1, myMqttsnTopicId, tempCstr) ) {
    	DT_DEBUG_PRINTLN_ERROR("Could not publish measurement");
		return false;
    }
    return true;
}



//bool CotSdkMqttSN::sendAlarm(const String text, const String type, const AlarmSeverity severity, const AlarmStatus status)
bool CotSdkMqttSN::sendAlarm(const String, const String, const AlarmSeverity, const AlarmStatus)
{
    DT_DEBUG_PRINTLN_ERROR("CotSdkMqttSN::sendAlarm: Not supported for the MQTT-SN protocol.");
    return false;
}


String _parseByRegexp(const String &src, const char* regexp)
{
	/*
	 * for Syntax of the RegEx-Engine see
	 * http://www.gammon.com.au/scripts/doc.php?lua=string.find
	 */
	MatchState matchState;
	const uint16_t DTCOT_MODEM_BASE_MATCH_STATE_BUFFER_SIZE = 1024;
	char matchStateBuf[DTCOT_MODEM_BASE_MATCH_STATE_BUFFER_SIZE]; // large enough to hold expected string
	std::vector<String> resultItems;

	matchState.Target((char*) src.c_str(), src.length());
	char myMatchStateResult = matchState.Match(regexp, 0);
	switch (myMatchStateResult) {
	case REGEXP_MATCHED:
		matchState.GetMatch(matchStateBuf);
		for (int j = 0; j < matchState.level; j++) {
			String item = String(matchState.GetCapture(matchStateBuf, j));
			resultItems.push_back(item);
		}
		break;
	case REGEXP_NOMATCH:
		break;
	default:
		DT_DEBUG_PRINTBUFFER_DEBUG("_parseByRegexp() src",(uint8_t*)(char*)src.c_str(), src.length());
		DT_DEBUG_PRINTBUFFER_DEBUG("_parseByRegexp() regexp",(uint8_t*)(char*)regexp, strlen(regexp));
		break;
	}
	if (resultItems.size() > 0) {
		return resultItems[0];
	}
	else{
		DT_DEBUG_PRINTLN_DEBUG("_parseByRegexp() no match");
		return "";
	}
}


void CotSdkMqttSN::OperationDefaultHandler(void* p)
{
    //DT_DEBUG_PRINTLN_INFO(__PRETTY_FUNCTION__);
    const char *rxStr1 = (const char*)p;
    String rxStr = _parseByRegexp(rxStr1, "([%w%p%s]*)");
    DT_DEBUG_PRINTLN_INFO("retrieve Operation: <%s>", rxStr.c_str());

    std::vector<String> messageParts;
    for (uint8_t i = 0; i < COT_SMARTREST_RESPONSE_CMD_INDEX; i++) {
    	messageParts.push_back("");
    }
    messageParts.push_back(rxStr);

    CotOperationData opData(defaultCommandMessageId, messageParts);
    activeCotSdkMqttSN->retrievedOperations.push_back(opData);
}


bool CotSdkMqttSN::registerOperation(DTCotOperationCallBack callBackFunction,
        int operationId, String cmd, bool ignoreCmdCase)
{
	DT_DEBUG_PRINTLN_INFO("CotSdkMqttSN::registerOperation: operationId<%d> cmd<%s> ignoreCmdCase<%d>",
				operationId, cmd.c_str(), ignoreCmdCase);

	bool subsc = mqttsn->SubscribeTopicDTCoT(FLAG_QOS_1, MQTTSN_TOPIC_CMDCMD, OperationDefaultHandler);
	if(! subsc) {
		DT_DEBUG_PRINTLN_ERROR("Could not subscribe to topic");
		return false;
	}
	/*get topic id, automatically register the topic if necessary */
	String infTopic = MQTTSN_TOPIC_INFCMD;
	int myMqttsnTopicId = GetTopicId(infTopic, "", "");
	if(myMqttsnTopicId == MQTTSN_TOPIC_INVALID_ID) {
		DT_DEBUG_PRINTLN_ERROR("Could not register topic for Operation! (was 0)");
		return false;
	}

	return operationsEngine.registerOperation(callBackFunction, operationId, cmd, ignoreCmdCase);
}


int CotSdkMqttSN::executeOperations(int timeout, bool trigger) {
    //DT_DEBUG_PRINTLN_INFO(__PRETTY_FUNCTION__);
	int execRes = operationsEngine.executeOperations("", timeout, trigger);
	return execRes;
}


void CotSdkMqttSN::triggerRetrievingOperations(String, int timeout, bool) { // trigger) {
	/* triggers the Substriction Channels to be processed */
	if (!timeout) {
		mqttsn->isIdle();
	} else {
		unsigned long loopstart = millis();
		unsigned long end = loopstart + timeout;
		while ((loopstart <= end) || ( ! mqttsn->isIdle() )) {
			loopstart = millis();
		}
	}
}

std::vector<CotOperationData> CotSdkMqttSN::getRetrievedOperations() {
	// make a COPY of the operations!
	std::vector<CotOperationData> resultCopy = retrievedOperations;

	// clean original OPss
	retrievedOperations.clear();

	return resultCopy;
}

String CotSdkMqttSN::getOperationStatusAsString(OperationStatus operationStatus) {
	switch(operationStatus) {
	case OperationStatus::SUCCESSFUL:
		return String("0");
		break;
	case OperationStatus::FAILED:
		return String("1");
		break;
	case OperationStatus::EXECUTING:
		return String("2");
		break;
	case OperationStatus::PENDING:
		return String("3");
		break;
	case OperationStatus::UNKNOWN:
	default:
		return String("-1");
		break;
	}
}

void CotSdkMqttSN::respondOperationsState(std::vector<String>,	OperationStatus status, const String, const String)
{
	int infCmdTopicId = GetTopicId(MQTTSN_TOPIC_INFCMD, "", "");

	if(infCmdTopicId == MQTTSN_TOPIC_INVALID_ID) {
		DT_DEBUG_PRINTLN_ERROR("respondOperationsState FAILED: topic not registered");
		return;
	}

	String respMsg = getOperationStatusAsString(status);

	DT_DEBUG_PRINTLN_INFO("Answering for Command: " + respMsg);

	// CAL-100: QoS-Level-0 is sufficient.
	if ( ! mqttsn->publish(FLAG_QOS_0, infCmdTopicId, respMsg.c_str()))
	{
		DT_DEBUG_PRINTLN_ERROR("Answering for Command Topic<%d> failed", infCmdTopicId);
		/* TODO: process error here */
	}

	/*wait for the mqttsn processor to get ready*/
	triggerRetrievingOperations("", 30000/*timeout 30sec*/, false);

	// Workaround: force MQTT-SN re-connect

}



// Void functions, for API compatibility.

bool CotSdkMqttSN::createAndRegisterDevice()
{
	DT_DEBUG_PRINTLN_INFO("CotSdkMqttSN::createAndRegisterDevice");
    return true;
}



bool CotSdkMqttSN::requestDeviceCredentials()
{
	DT_DEBUG_PRINTLN_INFO("CotSdkMqttSN::requestDeviceCredentials");
    return true;
}


CotSmartRestProcessor* CotSdkMqttSN::getSmRProc()
{
	DT_DEBUG_PRINTLN_DEBUG("CotSdkMqttSN::getSmRProc");
	return NULL;
}



bool CotSdkMqttSN::isConnectedToNetwork(void)
{
	return mqttsn->isConnectedToNetwork();
}



#ifdef ARDUINO_ARCH_AVR
/*power save modes*/
void CotSdkMqttSN::setPowerSaveMode(bool enable)
{
	gmxNB_setPowerSaveMode(enable);
}


/*support functions*/
void CotSdkMqttSN::getIMSI(String& imsi)
{
	gmxNB_getIMSI(imsi);
}



void CotSdkMqttSN::getIMEI(String& imei)
{
	gmxNB_getIMEI(imei);
}




void CotSdkMqttSN::getVersion(String& fwVersion)
{
	gmxNB_getVersion(fwVersion);
}




void CotSdkMqttSN::getTrafficStats(String& traStats)
{
	gmxNB_getTrafficStats(traStats);
}




void CotSdkMqttSN::getUeStats(String& ueStats)
{
	gmxNB_getUeStats(ueStats);
}



void CotSdkMqttSN::getCSQ(String& csq)
{
	gmxNB_getCSQ(csq);
}

#endif /*ARDUINO_ARCH_AVR*/

} /*namespace DTCoT*/

