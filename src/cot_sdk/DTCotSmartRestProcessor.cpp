/**
 * @file DTCotSmartRestProcessor.cpp
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#include <cot_sdk/DTCotSdkBase.h>
#include "DTCotSmartRestProcessor.h"
#include "base-classes/DTCoTCommunicationBase.h"
#include "base-classes/DTCoTDeviceBase.h"
#include "DTCoTExtensionIface.h"
#include "utility/DTCoTDebugOutput.h"
#include <TimeLib.h>

using namespace DTCoT;

#ifdef COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
unsigned long CotSmartRestResponseItem::instCount = 0;
unsigned long CotSmartRestResponseItem::instCounter = 0;
#endif

#ifdef COT_SMART_REST_CALL_ITEM_SHOW_INSTANCE_INFO
unsigned long CotSmartRestCallItem::instCount = 0;
unsigned long CotSmartRestCallItem::instCounter = 0;
#endif

CotSmartRestResponseItem::CotSmartRestResponseItem() :
		messageId(0), lineId(-1), isError(false), error(""), delimiter(
		COT_SMARTREST_DELIMITER), callItem(NULL) {
	messageParts.clear();

#ifdef COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
	instNumber = ++instCount;
	instCounter++;
	DT_DEBUG_PRINTLN_INFO("#  CotSmartRestResponseItem()     new    [%lu/%lu/%lu]", instNumber, instCount, instCounter);
#endif
}

CotSmartRestResponseItem::CotSmartRestResponseItem(const CotSmartRestResponseItem& item) {
	messageId = item.messageId;
	lineId = item.lineId;
	isError = item.isError;
	error = item.error;
	delimiter = item.delimiter;

	messageParts.clear();
	for(unsigned int i = 0; i < item.messageParts.size(); i++) {
		messageParts.push_back(item.messageParts[i]);
	}

	callItem = item.callItem;

#ifdef COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
	instNumber = ++instCount;
	instCounter++;
	DT_DEBUG_PRINTLN_INFO("#  CotSmartRestResponseItem(copy) new    [%lu/%lu/%lu]", instNumber, instCount, instCounter);
#endif
}

CotSmartRestResponseItem::~CotSmartRestResponseItem() {
#ifdef COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
	messageParts.clear();
	instCounter--;
	DT_DEBUG_PRINTLN_INFO("##~CotSmartRestResponseItem()     delete [%lu/%lu/%lu]", instNumber, instCount, instCounter);
#endif
}

void CotSmartRestResponseItem::reset() {
	messageId = 0;
	lineId = -1;
	isError = false;
	error = "";
	delimiter = COT_SMARTREST_DELIMITER;
	messageParts.clear();
}

void CotSmartRestResponseItem::copy(CotSmartRestResponseItem &target) {
	target.messageId = messageId;
	target.lineId = lineId;
	target.isError = isError;
	target.error = error;
	target.delimiter = delimiter;

	target.messageParts.clear();
	for(unsigned int i = 0; i < messageParts.size(); i++) {
		target.messageParts.push_back(messageParts[i]);
	}

	target.callItem = callItem;
}

#ifdef COT_SMART_REST_RESPONSE_ITEM_SHOW_INSTANCE_INFO
void CotSmartRestResponseItem::printInstanceInfo(const char* caller) {
	DT_DEBUG_PRINTLN_INFO("# %s CotSmartRestResponseItem [%lu/%lu/%lu]", caller, instNumber, instCount, instCounter);
}
#else
void CotSmartRestResponseItem::printInstanceInfo(const char*) {}
#endif

CotSmartRestCallItem::CotSmartRestCallItem() :
		messageId(0), lineId(-1), delimiter(
		COT_SMARTREST_DELIMITER), message(""), expectResponse(true), nextCallItem(NULL), parentCallItem(
				NULL) {
	responseItem.callItem = this;

#ifdef COT_SMART_REST_CALL_ITEM_SHOW_INSTANCE_INFO
	instNumber = ++instCount;
	instCounter++;
	DT_DEBUG_PRINTLN_INFO("#  CotSmartRestCallItem()         new    [%lu/%lu/%lu]", instNumber, instCount, instCounter);
#endif
}

CotSmartRestCallItem::CotSmartRestCallItem(const CotSmartRestCallItem& item)
: responseItem(item.responseItem)
{
	messageId = item.messageId;
	lineId = item.lineId;
	delimiter = item.delimiter;
	message = item.message;
	expectResponse = item.expectResponse;
	nextCallItem = item.nextCallItem;
	parentCallItem = item.parentCallItem;

#ifdef COT_SMART_REST_CALL_ITEM_SHOW_INSTANCE_INFO
	instNumber = ++instCount;
	instCounter++;
	DT_DEBUG_PRINTLN_INFO("#  CotSmartRestCallItem(copy)     new    [%lu/%lu/%lu]", instNumber, instCount, instCounter);
#endif
}

CotSmartRestCallItem::CotSmartRestCallItem(int smartRestId, String content, bool expectResponse, CotSmartRestCallItem *parentItem) :
		messageId(smartRestId), lineId(-1), delimiter(
		COT_SMARTREST_DELIMITER), message(smartRestId), expectResponse(expectResponse), nextCallItem(NULL), parentCallItem(
				NULL) {
	parentCallItem = parentItem;
	if (parentItem) {
		parentItem->nextCallItem = this;
	}
	message += delimiter + content + '\n';
	responseItem.reset();

#ifdef COT_SMART_REST_CALL_ITEM_SHOW_INSTANCE_INFO
	instNumber = ++instCount;
	instCounter++;
	DT_DEBUG_PRINTLN_INFO("#  CotSmartRestCallItem(...)      new    [%lu/%lu/%lu]", instNumber, instCount, instCounter);
#endif
}

CotSmartRestCallItem::~CotSmartRestCallItem() {
#ifdef COT_SMART_REST_CALL_ITEM_SHOW_INSTANCE_INFO
	instCounter--;
	DT_DEBUG_PRINTLN_INFO("##~CotSmartRestCallItem()         delete [%lu/%lu/%lu]", instNumber, instCount, instCounter);
#endif
}

void CotSmartRestCallItem::setParent(CotSmartRestCallItem *parentItem) {
	this->parentCallItem = parentItem;
	parentItem->nextCallItem = this;
}

void CotSmartRestCallItem::reset() {
	messageId = 0;
	lineId = -1;
	delimiter = COT_SMARTREST_DELIMITER;
	responseItem.reset();
}

void CotSmartRestCallItem::copy(CotSmartRestCallItem &target) {
	target.messageId = messageId;
	target.lineId = lineId;
	target.delimiter = delimiter;
	responseItem.copy(target.responseItem);
	target.responseItem.callItem = &target;
}

#ifdef COT_SMART_REST_CALL_ITEM_SHOW_INSTANCE_INFO
void CotSmartRestCallItem::printInstanceInfo(const char* caller) {
	DT_DEBUG_PRINTLN_INFO("# %s CotSmartRestCallItem [%lu/%lu/%lu]", caller, instNumber, instCount, instCounter);
}
#else
void CotSmartRestCallItem::printInstanceInfo(const char*) {}
#endif

//CotSmartRestProcessor* CotSmartRestProcessor::callBackObject = NULL;

//##TS<2018-05-07T10:40:55.332+02:00>
void CotSmartRestProcessor::setClockByCloudTimestring(String cloudtimestring) {
	int ye = atoi(parseResponse(cloudtimestring, 0, '-', 0).c_str());
	int mo = atoi(parseResponse(cloudtimestring, 0, '-', 1).c_str());
	String day = parseResponse(cloudtimestring, 0, '-', 2);
	String hour = parseResponse(day, 0, 'T', 1);
	int da = atoi(parseResponse(day, 0, 'T', 0).c_str());
	int mi = atoi(parseResponse(hour, 0, ':', 1).c_str());
	String second = parseResponse(hour, 0, ':', 2);
	int ho = atoi(parseResponse(hour, 0, ':', 0).c_str());
	int se = atoi(parseResponse(second, 0, '.', 0).c_str());
	String zone = parseResponse(cloudtimestring, 0, '+', 1);
	int zo = atoi(parseResponse(zone, 0, ':', 0).c_str());
	setTime(ho - zo, mi, se, da, mo, ye);
	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::setClockByCloudTimestring() <%s> cloudTS<%s>",
			cloudtimestring.c_str(), getCloudTimestringNow().c_str());
}

//"2018-05-04T05:00:00.000Z";
String CotSmartRestProcessor::createCloudTimestring(time_t t) {
	String ts = String(year(t));
	appDigits('-', month(t), ts);
	appDigits('-', day(t), ts);
	appDigits('T', hour(t), ts);
	appDigits(':', minute(t), ts);
	appDigits(':', second(t), ts);
	ts += ".000Z";
	return ts;
}

String CotSmartRestProcessor::getCloudTimestringNow() {
	return createCloudTimestring(now());
}

void CotSmartRestProcessor::appDigits(char pre, int value, String& ts) {
	ts += pre;
	if (value < 10) {
		ts += '0';
	}
	ts += String(value);
}

String CotSmartRestProcessor::head() {
	return "15," + xID + '\n';
}

bool CotSmartRestProcessor::hasError(CotSmartRestResponseItem &responseItem) {
	printResponseItem("CotSmartRestProcessor::hasError", 0, responseItem);
	if (responseItem.messageId <= COT_SMARTREST_RESPONSE_LAST_ERROR_MSGID && responseItem.messageId != COT_SMARTREST_RESPONSE_LIB_MSGID) {
		responseItem.isError = true;
		return true;
	}
	return false;
}

int CotSmartRestProcessor::parsePartCount(const String &source,
		char delimiter) {
	int pc = 1;
	int i = source.indexOf(delimiter);
	while (i >= 0) {
		pc++;
		i = source.indexOf(delimiter, ++i);
	}
	return pc;
}

String CotSmartRestProcessor::parseNext(String &source, char delimiter) {
	String part = source;
	if (source.length() > 0) {
		int next = source.indexOf(delimiter, 0);
		if (next >= 0) {
			part = source.substring(0, next);
			source = source.substring(next + 1);
		}
	}
	return part;
}

String CotSmartRestProcessor::parsePart(const String &source, char delimiter,
		int index) {
	String src = source;
	String part;
	while (index-- >= 0) {
		part = parseNext(src, delimiter);
	}
	return part;
}

int CotSmartRestProcessor::parsePartInt(const String &source, char delimiter,
		int index) {
	return atoi(parsePart(source, delimiter, index).c_str());
}

String CotSmartRestProcessor::parseLine(const String &source, int line) {
	return parsePart(source, '\n', line);
}

String CotSmartRestProcessor::parseResponse(const String &source, int line,
		char delimiter, int part) {
	String ret = parseLine(source, line);
	if (ret.length() > 0) {
		ret = parsePart(ret, delimiter, part);
	}
	return ret;
}

const CoTAuthBase& getCloudSmartRestAuthMethodInst() {
	static CoTAuthTLS cloudAuthMethod;
	return cloudAuthMethod;
}

void CotSmartRestProcessor::printCallItem(const char* caller, int index, const CotSmartRestCallItem *item) {
	if (item) {
		String out = String(caller) + " CallItem[" + index + "]["
				+ item->lineId + "] mid<" + item->messageId + "> message<"
				+ item->message + ">";
		DT_DEBUG_PRINTLN_DEBUG(out);
		printResponseItem(caller, 0, item->responseItem);
	}
}

void CotSmartRestProcessor::printResponseItem(const char* caller, int index, const CotSmartRestResponseItem &item) {
	if (item.lineId >= 0) {
		String out = String(caller) + " RespItem[" + index + "]["
				+ item.lineId + "] mid<" + item.messageId + "> child<";
		if (item.callItem) {
			out += "true";
		} else {
			out += "false";
		}
		out += String("> isError<") + item.isError + "> error<" + item.error
				+ "> message<";
		for (unsigned int j = 0; j < item.messageParts.size(); j++) {
			if (j > 0) {
				out += item.delimiter;
			}
			out += item.messageParts[j];
		}

		out += ">";
		DT_DEBUG_PRINTLN_DEBUG(out);
	}
}

int CotSmartRestProcessor::countCallItemsErrors(const std::vector<CotSmartRestCallItem> &items) {
	int errCount = 0;
	const CotSmartRestCallItem *item;
	for (unsigned int i = 0; i < items.size(); i++) {
		item = &items[i];
		while (item) {
			if (item->responseItem.isError) {
				errCount++;
			}
			item = item->nextCallItem;
		}
	}
	return errCount;
}

void CotSmartRestProcessor::printCallItems(const char* caller,
		const std::vector<CotSmartRestCallItem> &items) {
	const CotSmartRestCallItem *item;
	for (unsigned int i = 0; i < items.size(); i++) {
		item = &items[i];
		while (item) {
			printCallItem(caller, (int) i, item);
			item = item->nextCallItem;
		}
	}
}

void CotSmartRestProcessor::printResponseItems(const char* caller, const std::vector<CotSmartRestResponseItem> &items) {
	for (unsigned int i = 0; i < items.size(); i++) {
		printResponseItem(caller, (int) i, items[i]);
	}
}

CotSmartRestCallItem *CotSmartRestProcessor::getCallItemByResponseItem(CotSmartRestResponseItem *responseItem, std::vector<CotSmartRestCallItem> &items) {
	CotSmartRestCallItem *item;

	if (items.size() == 1) {
		item = &items[0];
		return item;
	}

	if (responseItem) {
		String moId = responseItem->messageParts[COT_SMARTREST_RESPONSE_MOID_INDEX];
		DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::getCallItemByResponseItem() messageId<%d> lineId<%d> moId<%s>...", responseItem->messageId, responseItem->lineId, moId.c_str());
		String moIdSearchPattern = String(",") + moId + String(",");
		for (unsigned int i = 0; i < items.size(); i++) {
			item = &items[i];
			//go to top
			while (item->parentCallItem) {
				item = item->parentCallItem;
			}
			while (item) {
				if (item->lineId == responseItem->lineId && item->message.indexOf(moIdSearchPattern) >= 0) {
					DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::getCallItemByResponseItem(%d) [%d] expected item lineId<%d> msgId req<%d> resp<%d> -> <%s>",
						responseItem->lineId, i, item->lineId, item->messageId, responseItem->messageId, item->message.c_str());
					return item;
				}
				DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::getCallItemByResponseItem(%d) [%d]          item lineId<%d> msgId req<%d> resp<%d>",
						responseItem->lineId, i, item->lineId, item->messageId, responseItem->messageId);
				item = item->nextCallItem;
			}
		}
		DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::getCallItemByResponseItem(%d) item with for msgId<%d> not found!", responseItem->lineId, responseItem->messageId);
	}
	return NULL;
}

int CotSmartRestProcessor::responseParser(const char* caller,
		const String &response, int &newCallResponseItems, int &newResponseOnlyItems,
		std::vector<CotSmartRestCallItem> &callItems,
		std::vector<CotSmartRestResponseItem> &operationResponseOnlyItems)
{
	int errCount = 0;
	newCallResponseItems = 0;
	newResponseOnlyItems = 0;
	int lines = 0;
	if (response.length()) {
		String line;
		CotSmartRestCallItem *callItem;
		int lineIndex = 0;
		do {
			line = parseLine(response, lineIndex++);
			if (line.length()) {
				lines++;
				DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::responseParser(%s) line<%s>", caller, line.c_str());
				CotSmartRestResponseItem tempResponseItem;
				int pc = parsePartCount(line, tempResponseItem.delimiter);
				if (pc > COT_SMARTREST_RESPONSE_LINE_INDEX) {
					tempResponseItem.messageId = parsePartInt(line, tempResponseItem.delimiter,
					COT_SMARTREST_RESPONSE_MSGID_INDEX);
					tempResponseItem.lineId = parsePartInt(line, tempResponseItem.delimiter,
					COT_SMARTREST_RESPONSE_LINE_INDEX);
				}
				for (int i = 0; i < pc; i++) {
					tempResponseItem.messageParts.push_back(parsePart(line, tempResponseItem.delimiter, i));
				}
				if (hasError(tempResponseItem)) {
					errCount++;
					tempResponseItem.isError = true;
					tempResponseItem.error = parsePart(line, tempResponseItem.delimiter, COT_SMARTREST_RESPONSE_ERROR_INDEX);
					DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::responseParser(%s) ERROR found in line[%d]<%s> messageId<%d>!", caller, tempResponseItem.lineId, line.c_str(), tempResponseItem.messageId);
				}
				if (tempResponseItem.messageId != COT_SMARTREST_RESPONSE_LIB_MSGID) {

					if (tempResponseItem.messageId >= COT_SMARTREST_RESPONSE_OPERATION_MSGID_FIRST && tempResponseItem.messageId <= COT_SMARTREST_RESPONSE_OPERATION_MSGID_LAST) {
						String opId = tempResponseItem.messageParts[COT_SMARTREST_RESPONSE_MOID_INDEX];
						String opState = tempResponseItem.messageParts[COT_SMARTREST_RESPONSE_STATE_INDEX];
						OperationStatus os = AlarmHelper::stringToOperationStatus(opState);
						if (   os == OperationStatus::EXECUTING
							|| os == OperationStatus::PENDING
						) {
							newResponseOnlyItems++;
							DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::responseParser(%s) new operation found.", caller);
							operationResponseOnlyItems.push_back(tempResponseItem);
							continue;
						} else {
							DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::responseParser(%s) operation<%s> state change in <%s> found. Treat like a response.", caller, opId.c_str(), opState.c_str());
						}
					}
					callItem = getCallItemByResponseItem(&tempResponseItem, callItems);
					if (callItem) {
						newCallResponseItems++;
						DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::responseParser(%s) response found.", caller);
						tempResponseItem.callItem = callItem;
						tempResponseItem.copy(callItem->responseItem);
//                            // Save ResponseItem for possible multiple responses, e.g. device-list
//                            operationResponseOnlyItems.push_back(item);
					} else {
						DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::responseParser(%s) No call item found for response line<%s>\nof response<%s>", caller, line.c_str(), response.c_str());
					}
				}
			}
		} while (line.length());

		DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::responseParser() lines<%d> error<%d> newCallResp<%d> newRespOnly<%d> in response<%s>",
				lines, errCount, newCallResponseItems, newResponseOnlyItems, response.c_str());

	}
	return errCount;
}

int CotSmartRestProcessor::readSubscription(const char* caller, int &errCount, int &receivedLen,
									int &newCallResponseItems, int &newResponseOnlyItems, int timeout) {
	unsigned long start = millis();
	bool rc = true;
	errCount = 0;
	receivedLen = comm->readSubscription(timeout, (uint8_t*)readSubscriptionBuffer,
									(uint16_t)COT_SMARTREST_PROCESSOR_READ_SUBSCRIPTION_BUFFER_SIZE);
	if (receivedLen > 0) {
		String response((const char*)readSubscriptionBuffer);

		if (response.length() == (unsigned int)receivedLen)
		{
			errCount = responseParser(caller, response, newCallResponseItems,
							newResponseOnlyItems, callItems, operationResponseOnlyItems);
			DT_DEBUG_PRINTLN_DEBUG(
					"CotSmartRestProcessor::readSubscription() caller<%s> timeout<%d> errCount<%d> callItems[%d] operationResponseOnlyItems[%d] callRespFound<%d> respOnlyFound<%d>... done in <%lu>ms. len<%d> -> rc<%d>",
					caller ? caller : "unknown", timeout, errCount, callItems.size(), operationResponseOnlyItems.size(), newCallResponseItems, newResponseOnlyItems, millis() - start, receivedLen, rc);
		} else {
			rc = false;
			DT_DEBUG_PRINTLN_CRITICAL(
					"CotSmartRestProcessor::readSubscription() caller<%s> timeout<%d> Not expected length difference between buffer and String! buf<%d> <-> String<%d> -> rc<%d>",
					caller, timeout, receivedLen, response.length(), rc);
		}
	}
	if (receivedLen == 0) {
		DT_DEBUG_PRINTLN_INFO("CotSmartRestProcessor::readSubscription() caller<%s> timeout<%d>... done in <%lu>ms. Empty response.", caller, timeout, millis() - start);
	}
//	if (receivedLen < 0) {
//		DT_DEBUG_PRINTLN_INFO("CotSmartRestProcessor::readSubscription() caller<%s> No response!", caller);
//	}
	return receivedLen;
}

CotSmartRestProcessor::CotSmartRestProcessor(const CoTDeviceBase& device,
		const String host, unsigned long port, const String xId,
		DTCoTPersistence& persistence)
: comm(NULL), xID(xId), _persistence(persistence)
, _device(device), subBuffer(""), pubBuffer(""), deviceTopic(NULL), isClockSet(false)
, callItemHead(15, xID)
{
	DTCoTPersistenceItem &pi = persistence.get();

	DT_DEBUG_PRINTLN_DEBUG(
			"CotSmartRestProcessor::CotSmartRestProcessor() user<%s> pwd<%s>...",
			pi.getCotUser().c_str(), pi.getCotPassword().c_str());

	callItems.clear();
	operationResponseOnlyItems.clear();

	CoTConfigDevice& devConfig = (CoTConfigDevice&) _device.getDeviceConfig();
	String deviceId = devConfig.getCloudDeviceId();
	subBuffer = String("mr/") + deviceId;
	pubBuffer = String("ms/") + deviceId;

	comm = new CoTCommunicationMQTT(device,
			CoTConfigCommunicationMQTT(host, port, pi.getCotPassword(),
					pi.getCotUser()), getCloudSmartRestAuthMethodInst());
	comm->subscribe(subBuffer.c_str(), NULL);

	DT_DEBUG_PRINTLN_DEBUG(
			"CotSmartRestProcessor::CotSmartRestProcessor() user<%s> pwd<%s>... done.",
			pi.getCotUser().c_str(), pi.getCotPassword().c_str());
}

CotSmartRestProcessor::~CotSmartRestProcessor() {
	DT_DEBUG_PRINTLN_DEBUG(
			"CotSmartRestProcessor::~CotSmartRestProcessor()...");
	if (deviceTopic) {
		delete (deviceTopic);
	}
	if (comm) {
		delete (comm);
	}
	subBuffer = "";
	pubBuffer = "";
	callItems.clear();
	operationResponseOnlyItems.clear();
	DT_DEBUG_PRINTLN_DEBUG(
			"CotSmartRestProcessor::~CotSmartRestProcessor()... done.");
}

bool CotSmartRestProcessor::cotRequestSync(const char* stepMsg,
		CotSmartRestCallItem &oneCallItem, int timeout) {
	unsigned long start = millis();
	DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::cotRequestSync() step<%s> sub<%s> pub<%s> to<%d>...",
				stepMsg, subBuffer.c_str(), pubBuffer.c_str(), timeout);
	//cleanup call item list
	callItems.clear();
	//go down to last item
	CotSmartRestCallItem *ci = &oneCallItem;
	while (ci->nextCallItem) {
		ci = ci->nextCallItem;
	}
	//go up to first item and reset on the way all response items
	while (ci->parentCallItem) {
		ci->responseItem.reset();
		ci = ci->parentCallItem;
	}
	//put head item at first call one
	ci->setParent(&callItemHead);
	//accumulate all call messages to request start from head item
	ci = &callItemHead;
	String request("");
	int lineNumber = 0;
	while (ci) {
		lineNumber++;
		request += ci->message;
		ci->lineId = lineNumber;
		ci = ci->nextCallItem;
	}
	//register call item
	callItems.push_back(oneCallItem);
	//publish
	comm->publish(pubBuffer.c_str(), request.c_str());

	//count errors
	int errorCount = 0;
	//received length
	int receivedLen = 0;
	int recvCallResp = 0;
	int recvRespOnly = 0;

	int subLen = readSubscription("cotReq", errorCount, receivedLen, recvCallResp, recvRespOnly, timeout);

	//save result
	callItems[0].copy(oneCallItem);

	//check result
	bool callOk = subLen >= 0 && !errorCount && ( !oneCallItem.expectResponse || ( oneCallItem.expectResponse && recvCallResp) );
	/*dummy variable to be compilable*/
	bool ok = callOk;

	DT_DEBUG_PRINTLN_DEBUG("CotSmartRestProcessor::cotRequestSync() step<%s> sub<%s> pub<%s> to<%d>... done in <%lu>ms. lines<%d> expResp<%d> errCnt<%d> callResp<%d> onlyResp<%d> ok<%d> -> <%d>",
				stepMsg, subBuffer.c_str(), pubBuffer.c_str(), timeout, millis() - start, lineNumber, oneCallItem.expectResponse, errorCount, recvCallResp, recvRespOnly, ok, callOk);

	//clear registered items
	callItems.clear();

//	if (errorCount)  {
//		comm->disconnect();
//	}

	return callOk;
}

bool CotSmartRestProcessor::createAndRegisterDevice() {

	String deviceId = _persistence.get().getCotUser();
	String cloudMoId = "";

	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::createAndRegisterDevice() ### Step 1: looking for MO of device<%s>...",
			deviceId.c_str());

	CotSmartRestCallItem call1Item(130, deviceId);

//	if (!call1Item.responseItem.error.equals("404")) {
	cotRequestSync("createAndRegisterDevice() Step 1", call1Item);
	if (call1Item.responseItem.messageId == 131) {
		cloudMoId = call1Item.responseItem.messageParts[3];
		_persistence.get().setMoId(cloudMoId.c_str());
		_persistence.save();
		DT_DEBUG_PRINTLN_INFO(
				"CotSmartRestProcessor::createAndRegisterDevice() device found! -> cloudMoId<%s>", cloudMoId.c_str());
		return true;
	}

	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::createAndRegisterDevice() ### Step 2: create MO for device<%s>...",
			deviceId.c_str());

	CotSmartRestCallItem call2Item(110, deviceId);
	if (!cotRequestSync("createAndRegisterDevice() Step 2", call2Item)) {
		return false;
	}

	if (call2Item.responseItem.messageId != 111) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSmartRestProcessor::createAndRegisterDevice() Create MO for device<%s> failed!",
				deviceId.c_str());
		_persistence.get().setMoId(cloudMoId.c_str());
		_persistence.save();
		return false;
	}

	cloudMoId = call2Item.responseItem.messageParts[2];

	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::createAndRegisterDevice() ### Step 3: register MO for device<%s>...",
			deviceId.c_str());

	String content = cloudMoId + COT_SMARTREST_DELIMITER + deviceId;
	CotSmartRestCallItem call3Item(120, content);
	if (!cotRequestSync("createAndRegisterDevice() Step 3", call3Item)) {
		return false;
	}

	if (call3Item.responseItem.messageId != 131) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSmartRestProcessor::createAndRegisterDevice() Register MO for the deviceId failed!");
		cloudMoId = "";
		_persistence.get().setMoId(cloudMoId.c_str());
		_persistence.save();
		return false;
	}

	DT_DEBUG_PRINTLN_DEBUG(
			"CotSmartRestProcessor::createAndRegisterDevice() ### Device <%s> is successful registered as MO<%s>",
			deviceId.c_str(), cloudMoId.c_str());
	_persistence.get().setMoId(cloudMoId.c_str());
	_persistence.save();
	return true;
}

bool CotSmartRestProcessor::checkUpdateMsgLength(String updateMsg,
		int threshold) {
	DT_DEBUG_PRINTLN_DEBUG(
			"CotSmartRestProcessor::checkUpdateMsgLength() MsgLength<%d> threshold<%d> Adafruit_MQTT.h.MAXBUFFERSIZE<%d>",
			updateMsg.length(), threshold, MAXBUFFERSIZE);

	int mqttPubLen = updateMsg.length();

	// Get device name length:
	mqttPubLen += _persistence.get().getCotUser().length();

	// Get templates name length:
	mqttPubLen += xID.length();

	// threshold (137) is empirical maximal sum of MQTT strings,
	// if  #define MAXBUFFERSIZE (150)  is defined in Adafruit_MQTT.h
	// If MAXBUFFERSIZE is greater than 150, this threshold may be corresponding greater.
	if (mqttPubLen > threshold) {
		DT_DEBUG_PRINTLN_ERROR(
				"### MQTT PUB length %d greater than threshold %d, possible buffer overflow !",
				mqttPubLen, threshold); DT_DEBUG_PRINTLN_ERROR(
				"### Please reduce the length of string parameters for updateDevice() !");
		return false;
	}

	return true;
}

bool CotSmartRestProcessor::updateDevice(String moId, const String hwModel,
		const String hwVersion, const String swVersion, const String imei,
		const String iccid, const String imsi, int maxlen) {
	String content = moId + COT_SMARTREST_DELIMITER + hwModel
			+ COT_SMARTREST_DELIMITER + hwVersion + COT_SMARTREST_DELIMITER
			+ _persistence.get().getCotUser() + COT_SMARTREST_DELIMITER
			+ swVersion + COT_SMARTREST_DELIMITER + imei
			+ COT_SMARTREST_DELIMITER + iccid + COT_SMARTREST_DELIMITER + imsi;

	// Check MQTT content length.
	if (!checkUpdateMsgLength(content, maxlen)) {
		return false;
	}

	CotSmartRestCallItem callItem(300, content);
	if (!cotRequestSync("updateDevice()", callItem, 30000)) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSmartRestProcessor::updateDevice() hwModel<%s> hwVersion<%s> swVersion<%s> imei<%s> iccid<%s> imsi<%s>... failed!",
				hwModel.c_str(), hwVersion.c_str(), swVersion.c_str(),
				imei.c_str(), iccid.c_str(), imsi.c_str());
		return false;
	}

	if (callItem.responseItem.messageId != 111) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSmartRestProcessor::updateDevice() hwModel<%s> hwVersion<%s> swVersion<%s> imei<%s> iccid<%s> imsi<%s>... failed! Not expected msgId<%d>",
				hwModel.c_str(), hwVersion.c_str(), swVersion.c_str(),
				imei.c_str(), iccid.c_str(), imsi.c_str(), callItem.responseItem.messageId);
		return false;
	}

	if (!isClockSet) {
		setClockByCloudTimestring(callItem.responseItem.messageParts[3]);
		isClockSet = true;
	}

	return true;
}

bool CotSmartRestProcessor::sendMeasurement(String moId, const String type,
		const String model, const String measurement, const String unit,
		const float value, const time_t timeStamp) {

	String content = type + COT_SMARTREST_DELIMITER + model
			+ COT_SMARTREST_DELIMITER + measurement + COT_SMARTREST_DELIMITER
			+ value + COT_SMARTREST_DELIMITER + unit + COT_SMARTREST_DELIMITER
			+ createCloudTimestring(timeStamp) + COT_SMARTREST_DELIMITER + moId;

	CotSmartRestCallItem callItem(410, content, false);
	return cotRequestSync("sendMeasurement()", callItem, 30000);
}

bool CotSmartRestProcessor::sendAlarm(String moId, const String text,
		const String type, const AlarmSeverity severity,
		const AlarmStatus status) {
	String content = type + COT_SMARTREST_DELIMITER
			+ createCloudTimestring(now()) + COT_SMARTREST_DELIMITER + text
			+ COT_SMARTREST_DELIMITER + AlarmHelper::alarmStatusToString(status)
			+ COT_SMARTREST_DELIMITER
			+ AlarmHelper::alarmSeverityToString(severity)
			+ COT_SMARTREST_DELIMITER + moId;

	CotSmartRestCallItem callItem(560, content, false);
	return cotRequestSync("sendAlarm()", callItem, 30000);
}


bool CotSmartRestProcessor::triggerCloud(String moId) {
	String content = moId + COT_SMARTREST_DELIMITER
			+ AlarmHelper::operationStatusToString(OperationStatus::EXECUTING);
	CotSmartRestCallItem callItem(200, content);
	return cotRequestSync("triggerCloud()", callItem);
}

bool CotSmartRestProcessor::setCloudOperationState(String operationId,
		OperationStatus status, const char* result, const char* text) {
	int srId = 211;
	String content = operationId + COT_SMARTREST_DELIMITER
			+ AlarmHelper::operationStatusToString(status);
	String strResult(COT_SMARTREST_DELIMITER);
	if (result != NULL && strlen(result)) {
		srId = 212;
		strResult += result;
	}
	String strText(COT_SMARTREST_DELIMITER);
	if (text != NULL && strlen(text)) {
		srId = 212;
		strText += text;
	}
	if (srId == 212) {
		content += strResult + strText;
	}
	CotSmartRestCallItem callItem(srId, content);
	return cotRequestSync("setCloudOperationState()", callItem);
}

String CotSmartRestProcessor::getOrCreateChildDevice(String moId,
		String childDeviceName) {

	String childMoId = "";

	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::getOrCreateChildDevice() moId<%s>, childDeviceName<%s>",
			moId.c_str(), childDeviceName.c_str());

	// Is some String parameter is void, return.
	if ((moId.length() == 0) || (childDeviceName.length() == 0)) {
		DT_DEBUG_PRINTLN_ERROR(
				"### ::getOrCreateChildDevice()  Void parameter: moId<%s>, childDeviceName<%s>",
				moId.c_str(), childDeviceName.c_str());

		return childMoId;	// Return void string
	}

	///// Step 0: Check if ChildDevice with this Name already available ?!
	// See c8yDevices.listChildren(mo) // moId of Gateway-Device
	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::getOrCreateChildDevice() *** Step 0: check MO childDevice availability");

	CotSmartRestCallItem call1Item(160, moId);

	operationResponseOnlyItems.clear();	// TODO: Clear it at begin and end !?
	// Request for child devices of current parent device.
	if (!cotRequestSync("getOrCreateChildDevice() Step 0", call1Item)) {
		DT_DEBUG_PRINTLN_ERROR(
				"### ::getOrCreateChildDevice()  cotRequestSync() failed!");
	} else if (call1Item.responseItem.messageId != 161) {
		DT_DEBUG_PRINTLN_ERROR(
				"### ::getOrCreateChildDevice() list Children MO for GW-Device<%s> failed!",
				moId.c_str());
	} else {
		// Evaluate Response-List. Response structure is e.g.: message<161,2,211079,childDevice_6>
		// childMoId = call1Item.responseItem.messageParts[2];
		// Use iterators for better accessibility
		CotSmartRestResponseItem respItem;
		for (unsigned int i = 0; i < operationResponseOnlyItems.size(); i++) {
			respItem = operationResponseOnlyItems[i];

			int count = 0;
			for (auto part = respItem.messageParts.begin();
					part != respItem.messageParts.end(); part++, count++) {
				//DT_DEBUG_PRINTLN_INFO("**  ::getOrCreateChildDevice(), part: %s, count: %d", part->c_str(), count);

				if (part->equals(childDeviceName)) {
					//DT_DEBUG_PRINTLN_INFO("*** ::getOrCreateChildDevice(), found childDeviceName: %s, count: %d",
					//		part->c_str(), count);
					childMoId = respItem.messageParts[count - 1];// Assign previous part-String as childMoId

					DT_DEBUG_PRINTLN_INFO(
							" ::getOrCreateChildDevice(), return childMoId: %s",
							childMoId.c_str());
					return childMoId;	// Return correct found childMoId String
				}
			}
		}
	}

	// If NOT:
	///// Step A: Create MO as com_IsChildDevice {},
	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::getOrCreateChildDevice() *** Step A: create MO for childDevice<%s>...",
			childDeviceName.c_str());

	CotSmartRestCallItem call2Item(140, childDeviceName);
	if (!cotRequestSync("getOrCreateChildDevice() Step A", call2Item)) {
		return childMoId;	// Return void string
	}

	if (call2Item.responseItem.messageId != 141) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSmartRestProcessor::getOrCreateChildDevice() Create MO for childDevice<%s> failed!",
				childDeviceName.c_str());
		return childMoId;	// Return void string
	}

	childMoId = call2Item.responseItem.messageParts[2];	// TODO: Is this 2 correct ?
	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::getOrCreateChildDevice()  Assigned childMoId: %s",
			childMoId.c_str());

	///// Step B: Assign newly created ChildDevice to parent MO
	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::getOrCreateChildDevice() *** Step B: register MO for childDevice<%s>...",
			childDeviceName.c_str());

	String content = moId + COT_SMARTREST_DELIMITER + childMoId;
	CotSmartRestCallItem call3Item(150, content);
	if (!cotRequestSync("getOrCreateChildDevice() Step B", call3Item)) {

		return childMoId;	// Return void string
	}

	// Status 201 , DataLength in Response is 0 => OK !
	if (call3Item.responseItem.isError == true) {
		DT_DEBUG_PRINTLN_ERROR(
				"CotSmartRestProcessor::getOrCreateChildDevice() Register MO for the childDevice<%s> failed!, <%s>",
				childDeviceName.c_str(), call3Item.responseItem.error.c_str());
		childMoId = "";

		return childMoId;	// Return void string
	}

	DT_DEBUG_PRINTLN_INFO(
			"CotSmartRestProcessor::getOrCreateChildDevice() *** ChildDevice <%s> is successful registered as MO<%s>",
			childDeviceName.c_str(), childMoId.c_str());

	return childMoId;	// Return correct string
}

void CotSmartRestProcessor::triggerRetrievingOperations(String moId, int timeout, bool trigger) {
	if (trigger) {
		triggerCloud(moId);
	}
	int errCount = 0;
	int receivedLen = 0;
	int recvCallResp = 0;
	int recvRespOnly = 0;
	readSubscription("exOps", errCount, receivedLen, recvCallResp, recvRespOnly, timeout);
}

std::vector<CotOperationData> CotSmartRestProcessor::getRetrievedOperations() {
	std::vector<CotOperationData> result;

	if (operationResponseOnlyItems.size() > 0) {
		String opResult;
		String opText;
		String cmd;
		for (unsigned int i = 0; i < operationResponseOnlyItems.size(); i++) {
			CotSmartRestResponseItem &item = operationResponseOnlyItems[i];
			CotOperationData opData(item.messageId, item.messageParts);
			result.push_back(opData);
		}
	}
	operationResponseOnlyItems.clear();

	return result;
}

void CotSmartRestProcessor::respondOperationsState(std::vector<String> messageParts,
		OperationStatus status, const String result, const String text)
{
	setCloudOperationState(messageParts[COT_SMARTREST_RESPONSE_MOID_INDEX],
											status, result.c_str(), text.c_str());
}
