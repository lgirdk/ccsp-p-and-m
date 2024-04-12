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
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
**********************************************************************/

#ifndef __COSA_SPEEDBOOST_DML_H__
#define __COSA_SPEEDBOOST_DML_H__

#include "cosa_apis.h"

bool setBlobDataOfSpeedBoost(ANSC_HANDLE hInsContext, char* pParamName,char* pParamVal);

bool getBlobDataOfSpeedBoost(ANSC_HANDLE hInsContext, char* pParamName, char *pVal, int iValSize);

bool getParamIntValOfSpeedBoost (ANSC_HANDLE hInsContext, char* pParamName, int* pVal);

bool getParamStringValOfSpeedBoost(ANSC_HANDLE hInsContext, char* pParamName, char* pParamVal, int iParamValSize);

#endif /*__COSA_SPEEDBOOST_DML_H__*/
