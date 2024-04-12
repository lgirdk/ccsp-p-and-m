/*
 * If not stated otherwise in this file or this component's Licenses.txt file
 * the following copyright and licenses apply:
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
*******************************************************************************/
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <syscfg/syscfg.h>
#include "ansc_platform.h"
#include "scheduler_interface.h"
#include "cosa_drg_common.h"
#include "speedboost_webconfig_apis.h"
#include "speedboost_scheduler_apis.h"
#include "speedboost_apis.h"
#include "speedboost_rbus_handler.h"

extern SpeedBoost sSpeedBoost;

ClientInfoOfSpeedBoost *pClientsDataOfSpeedBoost = &sSpeedBoost.sClientInfoSpeedBoost;

int cleanPrevSyscfgParams(void)
{
    char cParamName [BUFF_SIZE_64] = {0};
    char       cBuff [BUFF_SIZE_8] = {0};
    int    iSpeedBoostClientsCount = 0;

    snprintf(cParamName, sizeof(cParamName), "%s", SYSCFG_SPEEDBOOST_CLIENT_COUNT);
    if (!syscfg_get(NULL, cParamName, cBuff, sizeof(cBuff)))
    {
        iSpeedBoostClientsCount = atoi(cBuff);
        if ( 0 > iSpeedBoostClientsCount)
        {
            CcspTraceError(("%s:%d, Invalid SpeedBoost client count\n", __FUNCTION__, __LINE__));
            return -1;
        }
    }
    else
    {
        CcspTraceError(("%s:%d, syscfg_get failed for %s\n", __FUNCTION__, __LINE__, cParamName));
        return -1;
    }

    CcspTraceInfo(("%s:%d, Number of SpeedBoost clients: %d\n", __FUNCTION__, __LINE__, iSpeedBoostClientsCount));
    for (int iCount = 1; iCount <= iSpeedBoostClientsCount; iCount++)
    {
        snprintf(cParamName, sizeof(cParamName), "%s_%d", SYSCFG_SPEEDBOOST_CLIENT_MAC_PREFIX, iCount);
        if (0 != syscfg_unset(NULL, cParamName))
        {
            CcspTraceError(("%s:%d, syscfg_unset failed for %s\n", __FUNCTION__, __LINE__, cParamName));
            return -1;
        }
    }
    snprintf(cParamName, sizeof(cParamName), "%s", SYSCFG_SPEEDBOOST_CLIENT_COUNT);
    if (0 != syscfg_unset(NULL, cParamName))
    {
        CcspTraceError(("%s:%d, syscfg_unset failed for %s\n", __FUNCTION__, __LINE__, cParamName));
        return -1;
    }
    CcspTraceInfo(("%s:%d, Exit\n",__FUNCTION__,__LINE__))
    return 0;
}

bool triggerFirewallRestart(void)
{
    if (0 != commonSyseventSet("firewall-restart", ""))
    {
        CcspTraceError(("%s:%d, commonSyseventSet failed for firewall-restart\n",__FUNCTION__,__LINE__));
        return false;
    }
    else
    {
        CcspTraceInfo(("%s:%d, commonSyseventSet success for firewall-restart\n",__FUNCTION__,__LINE__));
        return true;
    }
}

void speedBoostMacOperation(char* pSpeedBoostMacAddress)
{
    char cMacAddress[MAC_ADDR_SIZE]    = {0};
    char  cParamName [BUFF_SIZE_64]    = {0};
    char cPreviousList [BUFF_SIZE_256] = {0};
    char cCurrentList [BUFF_SIZE_256]  = {0};
    char        cBuff [BUFF_SIZE_8]    = {0};
    int iSpeedBoostClientsCount        = 0;
    int iPrevNumOfActiveClients        = 0;

    CcspTraceInfo(("%s:%d, Received Mac address: %s\n", __FUNCTION__, __LINE__, pSpeedBoostMacAddress));

    for (int iCount = 0; iCount < pClientsDataOfSpeedBoost->iNumOfActiveClients; iCount++)
    {
        //After copying mac address append the pipe symbol
        strncat(cPreviousList, pClientsDataOfSpeedBoost->cCurrentDeviceList[iCount], MAC_ADDR_SIZE);
        if (iCount + 1 < pClientsDataOfSpeedBoost->iNumOfActiveClients)
            strncat(cPreviousList, ",", 1);
    }
    CcspTraceInfo(("%s:%d, Previous Active devices list: %s\n", __FUNCTION__, __LINE__, cPreviousList));

    if (0 != cleanPrevSyscfgParams())
    {
        CcspTraceError(("%s:%d, cleanPrevSyscfgParams failed\n", __FUNCTION__, __LINE__));
    }
    iPrevNumOfActiveClients = pClientsDataOfSpeedBoost->iNumOfActiveClients;
    pClientsDataOfSpeedBoost->iNumOfActiveClients = 0;
    if (NULL == pSpeedBoostMacAddress)
    {
        CcspTraceInfo(("%s: Empty speedboost mac bundle \n", __FUNCTION__));
        if(false == triggerFirewallRestart())
        {
            CcspTraceError(("%s: failed firewall restart.\n", __FUNCTION__));
        }
        publishEventChangeIfSubscribed(SPEEDBOOST_NUMBER_OF_ELIGIBLE_DEVICES, &iPrevNumOfActiveClients, &iSpeedBoostClientsCount);
        publishEventChangeIfSubscribed(SPEEDBOOST_CURRENT_ACTIVE_DEVICE_LIST,cPreviousList, cCurrentList);
        return;
    }
    char * pMacAddressBuffer = strdup (pSpeedBoostMacAddress);
    if (NULL == pMacAddressBuffer)
    {
        CcspTraceError(("%s:%d, strdup failed\n", __FUNCTION__, __LINE__));
        return;
    }
    //memset the two dimenosional array
    memset(pClientsDataOfSpeedBoost->cCurrentDeviceList, 0, MAX_MAC_ADDR_COUNT * MAC_ADDR_SIZE * sizeof(char));
    char *pLine = pMacAddressBuffer;
    while (NULL != pLine)
    {
        if (sscanf(pLine, "%17s", cMacAddress) == 1)
        {
            if (CheckMacHasValidCharacter(cMacAddress))
            {
                snprintf(cParamName, sizeof(cParamName), "%s_%d", SYSCFG_SPEEDBOOST_CLIENT_MAC_PREFIX, (iSpeedBoostClientsCount+1));
                if (0 != syscfg_set_commit(NULL, cParamName, cMacAddress))
                {
                    CcspTraceError(("%s:%d, syscfg_set_commit failed for %s\n", __FUNCTION__, __LINE__, cParamName));
                    pLine = NULL;
                    free(pMacAddressBuffer);
                    return;
                }
                strncpy(pClientsDataOfSpeedBoost->cCurrentDeviceList[iSpeedBoostClientsCount], cMacAddress, MAC_ADDR_SIZE);
                iSpeedBoostClientsCount++;
                CcspTraceInfo(("%s:%d, Added entry for Mac address:%s,to SpeedBoost Configuration\n", __FUNCTION__, __LINE__, cMacAddress));
            }
            else
            {
                CcspTraceError(("%s:%d, Invalid Mac address: %s\n", __FUNCTION__, __LINE__, cMacAddress));
            }
        }
        pLine = strchr(pLine, '\n');
        if (NULL != pLine)
        {
            pLine++;
        }
    }
    free(pMacAddressBuffer);
    pLine = NULL;

    CcspTraceInfo(("%s:%d, Number of SpeedBoost clients: %d\n", __FUNCTION__, __LINE__, iSpeedBoostClientsCount));

    pClientsDataOfSpeedBoost->iNumOfActiveClients = iSpeedBoostClientsCount;
    snprintf(cParamName, sizeof(cParamName), "%s", SYSCFG_SPEEDBOOST_CLIENT_COUNT);
    snprintf(cBuff, sizeof(cBuff), "%d", iSpeedBoostClientsCount);
    if (0 != syscfg_set_commit(NULL, cParamName, cBuff))
    {
        CcspTraceError(("%s:%d, syscfg_set_commit failed for %s\n", __FUNCTION__, __LINE__, cParamName));
        return;
    }
    //Re used the cParamName buffer to store the previous active devices list count
    publishEventChangeIfSubscribed(SPEEDBOOST_NUMBER_OF_ELIGIBLE_DEVICES, &iPrevNumOfActiveClients, &iSpeedBoostClientsCount);
    if (false == triggerFirewallRestart())
    {
        CcspTraceError(("%s:%d, triggerFirewallRestart failed\n", __FUNCTION__, __LINE__));
    }

    for (int iCount = 0; iCount < pClientsDataOfSpeedBoost->iNumOfActiveClients; iCount++)
    {
        //After copying mac address append the pipe symbol
        strncat(cCurrentList, pClientsDataOfSpeedBoost->cCurrentDeviceList[iCount], MAC_ADDR_SIZE);
        if ((iCount + 1) < pClientsDataOfSpeedBoost->iNumOfActiveClients)
            strncat(cCurrentList, ",", 1);
    }
    CcspTraceInfo(("%s:%d, Current Active devices list: %s\n", __FUNCTION__, __LINE__, cCurrentList));
    publishEventChangeIfSubscribed(SPEEDBOOST_CURRENT_ACTIVE_DEVICE_LIST,cPreviousList, cCurrentList);
    return;
}

int speedBoostSchdeulerInit(void)
{
    SchedulerData sSchedulerData =  {
                                .data_file = SPEEDBOOST_DATAFILE,
                                .md5_file = SPEEDBOOST_MD5FILE,
                                .max_actions = 0,
                                .scheduler_action = speedBoostMacOperation,
                                .scheduler_action_key = XM_SPEED_BOOST_RULES,
                                .instanceNum = 0
                            };

    SchedulerData sSchedulerDataArr [] = {sSchedulerData};

    int iSchedulerDataArrSize = sizeof(sSchedulerDataArr)/sizeof(sSchedulerDataArr[0]);

    CcspTraceInfo(("%s:%d, Initializing scheduler\n", __FUNCTION__, __LINE__));
    return scheduler_init (sSchedulerDataArr, iSchedulerDataArrSize, PAM_RDK_LOG_MODULE);
}
