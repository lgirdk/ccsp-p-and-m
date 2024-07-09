/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __SPEEDBOOST_WEBCONFIG_APIS_H__
#define __SPEEDBOOST_WEBCONFIG_APIS_H__

#include "webconfig_framework.h"

#define SPEEDBOOST_BLOB_DATA                    "Device.X_RDK_Speedboost.Data"
#define SPEEDBOOST_NUMBER_OF_CONFIGURED_DEVICES "Device.X_RDK_Speedboost.NumberConfiguredDevices"
#define SPEEDBOOST_NUMBER_OF_ELIGIBLE_DEVICES   "Device.X_RDK_Speedboost.NumberEligibleDevices"
#define SPEEDBOOST_CURRENT_ACTIVE_DEVICE_LIST   "Device.X_RDK_Speedboost.CurrentDeviceList"

/* As per Schema*/
#define XM_SPEED_BOOST_SUBDOC "xmspeedboost"
#define XM_SPEED_BOOST_RULES  "xm_speedboost_rules"

#define NINETY_DAYS_IN_SECONDS  (90 * 24 * 60 * 60)
#define MAX_DELAY_TIME_FOR_BLOB_RECEIVE 60

#define BUFF_SIZE_256 256
#define BUFF_SIZE_64  64
#define BUFF_SIZE_16  16
#define BUFF_SIZE_8   8

int getNumOfConfiguredClients(void);
// Function to validate MAC address format
bool CheckMacHasValidCharacter (char* pMac);
bool ValidateMac(char* pPhysAddress);
bool validateUnixTime(int64_t unixTime, char * pErrMessage, int iSizeOfBuffer);
void freeResourcesSpeedBoostDoc(void *pVoidArg);
pErr processSpeedBoostWebConfigRequest(void *pVoidData);
bool unpackAndProcessSpeedBoostData(char *pString);
#endif /*__SPEEDBOOST_WEBCONFIG_APIS_H__ */
