/**
 * @file DTCoTPrivate.h
 * @description header file for base classes for Authentification and devices 
 * @author mm1 Technology GmbH
 * @copyright (C) 2017-2018 Deutsche Telekom AG- all rights reserved. 
 * @licence MIT licence
 */

#ifndef DT_COT_PRIVATE_HEADER_FILE 
#define DT_COT_PRIVATE_HEADER_FILE 

#include "base-classes/DTCoTConfigBase.h"
#include "base-classes/DTCoTDeviceBase.h"
#include "base-classes/DTCoTCommunicationBase.h"

/* DTCoT Private abstraction interface */

namespace DTCoT 
{

/* Handler and payload-related infrastructure */
typedef void* CoTHandlerParam;
typedef void (*CoTHandler)(CoTHandlerParam);

/**
 * Private base class for all authentication.
 */
class CoTAuthBase { };


} /* namespace DTCoT */

#endif /* DT_COT_PRIVATE_HEADER_FILE */

