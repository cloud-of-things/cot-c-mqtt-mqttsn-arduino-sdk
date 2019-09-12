/**
 * @file DTCotOperationsEngine.h
 * @description header file for an Interface for handling Operations via Cumulocity.
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef OperationsEngine_h_
#define OperationsEngine_h_

#include <Time.h>
#include "DTCotSdkTypes.h"
#include "base-classes/DTCotOperation.h"
#include <vector>


#define COT_SMARTREST_RESPONSE_CMD_INDEX						4


namespace DTCoT {

/**
 * A value class that holds the data of operations from the cloud and to transfer
 * to the OperationsEngine.
 */
class CotOperationData
{
  public:
	CotOperationData(int msgId, std::vector<String> msgParts)
  	  	: messageId(msgId), messageParts(msgParts) {}

	CotOperationData(const CotOperationData& data)
  		: messageId(data.messageId), messageParts(data.messageParts) {}

    ~CotOperationData() {}

    int messageId;
    std::vector<String> messageParts;
};

/* ************************************************************************* */
/**
 *
 */
class CotOperationsEngineAdapter
{
public:
	virtual ~CotOperationsEngineAdapter() {};
	virtual void triggerRetrievingOperations(String moId, int timeout, bool trigger) = 0;
	virtual std::vector<CotOperationData> getRetrievedOperations() = 0;
	virtual void respondOperationsState(std::vector<String> messageParts,
			OperationStatus status, const String result, const String text) = 0;
};


/* ************************************************************************* */
class CotOperationsEngine
{
  // == Methods ==
  public:

    /** C-Tor  */
    CotOperationsEngine() : _adapter(NULL) { }
    /** D-Tor */
    ~CotOperationsEngine() { clear(); }

    void init(CotOperationsEngineAdapter* adapter) { _adapter = adapter; }

    void clear() { registeredOperations.clear(); }

    bool registerOperation(
    		DTCotOperationCallBack callBackFunction, int operationId, String cmd, bool ignoreCmdCase)
    {
    	if (!getOperationByIdAndCmd(operationId, cmd, false)) {
    		registeredOperations.push_back(
    				DTCotOperation(callBackFunction, operationId, cmd, ignoreCmdCase));
    		DT_DEBUG_PRINTLN_INFO("registerOperation operationId<%d> cmd<%s>... done.", operationId, cmd.c_str());
    	}
    	else{
    		DT_DEBUG_PRINTLN_INFO("registerOperation operationId<%d> cmd<%s>... already registered!", operationId, cmd.c_str());
    	}
    	return getOperationByIdAndCmd(operationId, cmd, false) != NULL;
    }


    int executeOperations(String moId, int timeout, bool trigger)
    {
    	//DT_DEBUG_PRINTLN_DEBUG("executeOperations()");
    	int execCount = 0;
    	_adapter->triggerRetrievingOperations(moId, timeout, trigger);

    	std::vector<CotOperationData> opData =_adapter->getRetrievedOperations();
    	if (opData.size() <= 0) {
    		//DT_DEBUG_PRINTLN_DEBUG("executeOperations: no OPs");
    	}
		String opResult;
		String opText;
		int msgId;
		String cmd;
		OperationStatus os;
		DTCotOperation *operation;
		for (unsigned int i = 0; i < opData.size(); i++) {
			CotOperationData &item = opData[i];
			msgId = item.messageId;
			if (item.messageParts.size() > COT_SMARTREST_RESPONSE_CMD_INDEX) {
				cmd = item.messageParts[COT_SMARTREST_RESPONSE_CMD_INDEX].c_str();
			}
			os = OperationStatus::FAILED;
			operation = getOperationByIdAndCmd(msgId, cmd, true);
			if (operation) {
				DT_DEBUG_PRINTLN_DEBUG("executeOperations()[%d] call callback opId<%d> cmd<%s>...",
						i, msgId, cmd.c_str());
				os = (operation->getCallBackFunction())(item.messageParts);
				opResult = "Command '" + cmd + "' executed";
				opText = "'" + cmd + "'";
				DT_DEBUG_PRINTLN_DEBUG("executeOperations()[%d] call callback opId<%d> cmd<%s>... done. <%d>",
						i, msgId, cmd.c_str(), os);
				execCount++;
			}
			else{
				DT_DEBUG_PRINTLN_ERROR("executeOperations() opId<%d> cmd<%s> is NOT registered!", msgId, cmd.c_str());
				os = OperationStatus::FAILED;
				opText = String("unknown '") + cmd + "'";
				opResult = String("Please register command '") + cmd + "' in device!";
			}
			if (os != OperationStatus::PENDING && os != OperationStatus::EXECUTING) {
				_adapter->respondOperationsState(item.messageParts, os, opResult, opText);
			}
		}
    	return execCount;
    }


  private:

    DTCotOperation* getOperationByIdAndCmd(int operationId, String cmd, bool allowEmptyCmd)
    {
    	DTCotOperation *op = NULL;
    	for (unsigned int i = 0; i < registeredOperations.size(); i++) {
    		DTCotOperation &o = registeredOperations[i];
    		if (operationId == o.getOperationId()) {
    			if (o.isIgnoreCmdCase()) {
    				if (cmd.equalsIgnoreCase(o.getCmd())) {
    					op = &o;
    					break;
    				}
    			} else {
    				if (cmd.equals(o.getCmd())) {
    					op = &o;
    					break;
    				}
    			}
    			if (allowEmptyCmd && o.getCmd().length() == 0) {
    				op = &o;
    			}
    		}
    	}
    	return op;
    }


  //  == Members ==
  private:
    CotOperationsEngineAdapter* _adapter;
    std::vector<DTCotOperation> registeredOperations;

};

}

#endif // OperationsEngine_h_
