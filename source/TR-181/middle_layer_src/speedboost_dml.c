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
#include <stdbool.h>
#include "ansc_platform.h"
#include "speedboost_apis.h"
#include "speedboost_webconfig_apis.h"

bool setBlobDataOfSpeedBoost(ANSC_HANDLE hInsContext, char* pParamName,char* pParamVal)
{
    UNREFERENCED_PARAMETER(hInsContext);

    if(NULL == pParamName || NULL == pParamVal)
    {
        CcspTraceError(("%s:%d, Invalid input parameters\n", __FUNCTION__, __LINE__));
        return false;
    }

    if (strcmp(pParamName, "Data") == 0)
    {
        CcspTraceInfo(("%s:%d, ParamName: %s\n", __FUNCTION__, __LINE__, pParamName));
        return unpackAndProcessSpeedBoostData(pParamVal);
    }
    return false;
}

bool getBlobDataOfSpeedBoost(ANSC_HANDLE hInsContext, char* pParamName, char *pVal, int iValSize)
{
    UNREFERENCED_PARAMETER(hInsContext);
    bool bRet = true;

    if ((NULL == pParamName) || (NULL == pVal) || (0 == iValSize))
    {
        CcspTraceError(("%s:%d, NULL parameters passed\n", __FUNCTION__, __LINE__));
        bRet = false;
    }

    if (strcmp(pParamName, "Data") == 0)
    {
        CcspTraceInfo(("%s:%d, ParamName: %s\n", __FUNCTION__, __LINE__, pParamName));
        snprintf(pVal, iValSize, "%s", "   ");
    }
    else
    {
        CcspTraceError(("%s:%d, Invalid Parameter Name:%s\n",__FUNCTION__,__LINE__,pParamName));
        bRet = false;
    }
    return bRet;
}

bool getParamIntValOfSpeedBoost (ANSC_HANDLE hInsContext, char* pParamName, int* pVal)
{
    UNREFERENCED_PARAMETER(hInsContext);
    bool bRet = true;

    if ((NULL == pParamName) || (NULL == pVal))
    {
        CcspTraceError(("%s:%d, Invalid input parameters\n", __FUNCTION__, __LINE__));
        bRet = false;
    }
    if (strcmp(pParamName, "NumberConfiguredDevices") == 0)
    {
        CcspTraceInfo(("%s:%d, ParamName: %s\n", __FUNCTION__, __LINE__, pParamName));
        *pVal = getNumOfConfiguredClients();
    }
    else if (strcmp(pParamName, "NumberEligibleDevices") == 0)
    {
        CcspTraceInfo(("%s:%d, ParamName:%s\n",__FUNCTION__, __LINE__, pParamName));
        *pVal = getNumOfActiveClients();
    }
    else
    {
        CcspTraceError(("%s:%d, Invalid Parameter Name:%s\n",__FUNCTION__,__LINE__,pParamName));
        bRet = false;
    }
    return bRet;
}

bool getParamStringValOfSpeedBoost(ANSC_HANDLE hInsContext, char* pParamName, char* pParamVal, int iParamValSize)
{
    UNREFERENCED_PARAMETER(hInsContext);
    bool bRet = true;

    if ((NULL == pParamName) || (NULL == pParamVal) || (0 == iParamValSize))
    {
        CcspTraceError(("%s:%d, NULL parameter passed\n", __FUNCTION__, __LINE__));
        bRet = false;
    }
    if (strcmp(pParamName, "CurrentDeviceList") == 0)
    {
        CcspTraceInfo(("%s:%d, ParamName: %s\n", __FUNCTION__, __LINE__, pParamName));
        if(!getActiveDevicesListOfSpeedBoost(pParamVal, iParamValSize))
        {
            CcspTraceError(("%s:%d, Failed to get active devices list\n", __FUNCTION__, __LINE__));
            bRet = false;
        }
    }
    else
    {
        CcspTraceError(("%s:%d, Invalid Parameter Name:%s\n",__FUNCTION__,__LINE__,pParamName));
        bRet = false;
    }
    return bRet;
}
