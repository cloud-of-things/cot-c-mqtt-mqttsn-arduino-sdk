/*
 * DTCotOperation.cpp
 *
 *  Created on: 28.05.2018
 *      Author: hhenkens
 */

#include "DTCotOperation.h"

using namespace DTCoT;

DTCotOperation::DTCotOperation(DTCotOperationCallBack callBackFunction, int operationId, String cmd, bool ignoreCmdCase) :
        callBackFunction(callBackFunction), operationId(operationId), cmd(cmd), ignoreCmdCase(ignoreCmdCase)
{
}

DTCotOperation::DTCotOperation(const DTCotOperation &operation)
{
    this->callBackFunction = operation.callBackFunction;
    this->operationId = operation.operationId;
    this->cmd = operation.cmd;
    this->ignoreCmdCase = operation.ignoreCmdCase;
}

DTCotOperation::~DTCotOperation() {
}

int DTCotOperation::getOperationId() {
    return operationId;
}

String DTCotOperation::getCmd() {
    return cmd;
}

bool DTCotOperation::isIgnoreCmdCase() {
    return ignoreCmdCase;
}

DTCotOperationCallBack DTCotOperation::getCallBackFunction() {
    return callBackFunction;
}
