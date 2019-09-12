/*
 * DTCotOperation.h
 *
 *  Created on: 28.05.2018
 *      Author: hhenkens
 */

#ifndef LIBRARIES_DT_ARDUINO_IOT_AGENT_COT_SDK_DTCOTOPERATION_H_
#define LIBRARIES_DT_ARDUINO_IOT_AGENT_COT_SDK_DTCOTOPERATION_H_

#include "../cot_sdk/DTCotSdkTypes.h"

namespace DTCoT {

#define DTCOTOPERATIONID_C8Y_COMMAND    201
#define DTCOTOPERATIONID_C8Y_RESTART    202
#define DTCOTOPERATIONID_C8Y_FIRMWARE   203
#define DTCOTOPERATIONID_C8Y_QUERY   	204

typedef OperationStatus (*DTCotOperationCallBack)(std::vector<String> &messageParts);

class DTCotOperation {
public:
    DTCotOperation(DTCotOperationCallBack callBackFunction, int operationId, String cmd = "", bool ignoreCmdCase = true);
    DTCotOperation(const DTCotOperation &copy);
    ~DTCotOperation();
    int getOperationId();
    String getCmd();
    bool isIgnoreCmdCase();
    DTCotOperationCallBack getCallBackFunction();
private:
    DTCotOperationCallBack callBackFunction;
    int operationId;
    String cmd;
    bool ignoreCmdCase;
};

} /* namespace DTCoT */

#endif /* LIBRARIES_DT_ARDUINO_IOT_AGENT_COT_SDK_DTCOTOPERATION_H_ */
