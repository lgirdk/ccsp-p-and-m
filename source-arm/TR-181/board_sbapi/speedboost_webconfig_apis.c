/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
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
****************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ansc_platform.h"
#include "webconfig_framework.h"
#include "cosa_webconfig_api.h"
#include "speedBoostDoc.h"
#include "speedboost_apis.h"
#include "speedboost_webconfig_apis.h"
#include "speedboost_scheduler_apis.h"
#include "speedboost_rbus_handler.h"

extern SpeedBoost sSpeedBoost;

int * pNumOfConfiguredClients = &sSpeedBoost.sClientInfoSpeedBoost.iNumOfConfigOfClients;

// Function to validate MAC address format
bool CheckMacHasValidCharacter (char* pMac)
{
    if (NULL == pMac)
    {
        CcspTraceError(("%s:%d, NULL pointer passed\n", __FUNCTION__, __LINE__));
        return false;
    }

    for (int iCount = 0; iCount < 6; iCount++)
    {
        if ((isxdigit(pMac[0])) &&
            (isxdigit(pMac[1])) &&
            (pMac[2] == ((iCount == 5) ? 0 : ':')))
        {
            pMac += 3;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool ValidateMac(char* pPhysAddress)
{
    bool bValid = false;

    if (NULL == pPhysAddress)
    {
        CcspTraceError(("%s:%d, NULL pointer passed\n", __FUNCTION__, __LINE__));
        return false;
    }

    if(!pPhysAddress || \
            0 == strcmp(pPhysAddress, "00:00:00:00:00:00"))
    {
        return false;
    }

    if(strlen(pPhysAddress) != MAC_ADDR_SIZE-1)
    {
        return false;
    }

    bValid = CheckMacHasValidCharacter(pPhysAddress);

    return bValid;
}

bool validateUnixTime(int64_t unixTime, char * pErrMessage, int iSizeOfBuffer)
{
    int64_t currentTime = 0;
    bool bRet = true;

    currentTime = (int64_t)time(NULL);
    CcspTraceInfo(("%s:%d, unixTime = %lld\n", __FUNCTION__, __LINE__, unixTime));
    if (unixTime > (currentTime + NINETY_DAYS_IN_SECONDS))
    {
        CcspTraceError (("%s:%d, The Unix time is more than 90 days from now.\n", __FUNCTION__, __LINE__));
        if (NULL != pErrMessage)
        {
            snprintf(pErrMessage, iSizeOfBuffer - 1, "%s", "The Unix time is more than 90 days from now.");
        }
        bRet = false;
    }
    else
    {
        CcspTraceInfo (("%s:%d, Unix time is valid\n",__FUNCTION__,__LINE__));
    }
    return bRet;
}

void freeResourcesSpeedBoostDoc(void *pVoidArg)
{
    CcspTraceInfo((" Entering %s \n",__FUNCTION__));
    execData *pBlobExecData  = (execData*) pVoidArg;

    if(NULL == pBlobExecData)
       return;
    speedBoostDoc_t *pSpeedBoostDoc = (speedBoostDoc_t *) pBlobExecData->user_data;

    if (NULL != pSpeedBoostDoc )
    {
        speedBoostDocDestroy(pSpeedBoostDoc );
        pSpeedBoostDoc = NULL;
    }

    if ( pBlobExecData != NULL )
    {
        free(pBlobExecData);
        pBlobExecData = NULL ;
    }
}

pErr processSpeedBoostWebConfigRequest(void *pVoidData)
{
    pErr execRetVal                      = NULL;
    size_t iNumberOfIndexes              = 0;
    int iSpeedBoostClientsCount          = 0;
    char cClientsCount[BUFF_SIZE_16]     = {0};
    char cBridgeMode [BUFF_SIZE_16]      = {0};
    int  iRet                            = 0;

    CcspTraceInfo(("%s:%d, Entry\n", __FUNCTION__, __LINE__));

    execRetVal = (pErr) malloc (sizeof(Err));
    if (NULL == execRetVal)
    {
        CcspTraceError(("%s :%d, Memory Allocation Failed\n", __FUNCTION__, __LINE__));
        return execRetVal;
    }

    memset(execRetVal,0,sizeof(Err));

    execRetVal->ErrorCode = BLOB_EXEC_SUCCESS;

    if (NULL == pVoidData)
    {
        CcspTraceError(("%s:%d, Invalid input parameters\n", __FUNCTION__, __LINE__));
        execRetVal->ErrorCode = NULL_BLOB_EXEC_POINTER;
        snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "NULL pointer passed for call back function");
        return execRetVal;
    }

    iRet = syscfg_get(NULL,"bridge_mode",cBridgeMode,sizeof(cBridgeMode));

    if (0 == iRet && (strcmp(cBridgeMode,"0")))
    {
        CcspTraceWarning(("%s:%d bridge mode enabled \n",__FUNCTION__,__LINE__));
        execRetVal->ErrorCode = VALIDATION_FALIED;
        strncpy(execRetVal->ErrorMsg,"SpeedBoost is Not supported when Gatewaty is in bridge mode",sizeof(execRetVal->ErrorMsg)-1);
        return execRetVal;
    }

    speedBoostDoc_t *pSpeedBoostDoc = (speedBoostDoc_t *) pVoidData;

    CcspTraceInfo(("pSpeedBoostDoc->pSubDocName is %s\n", pSpeedBoostDoc->pSubDocName));
    CcspTraceInfo(("pSpeedBoostDoc->version is %lu\n", (long)pSpeedBoostDoc->ui32Version));
    CcspTraceInfo(("pSpeedBoostDoc->transaction_id %lu\n",(long) pSpeedBoostDoc->ui16TranscationId));
    if (NULL == pSpeedBoostDoc->pSchedulerInfo)
    {
        CcspTraceInfo(("%s: Empty scheduler info.\n", __FUNCTION__));
        if (0 == delete_schedule(XM_SPEED_BOOST_RULES))
        {
            CcspTraceInfo(("%s: Scheduler stopped.\n", __FUNCTION__));
            if (0 != cleanPrevSyscfgParams())
            {
                CcspTraceError(("%s:%d, cleanPrevSyscfgParams failed\n", __FUNCTION__, __LINE__));
            }
            if (false == triggerFirewallRestart())
            {
                CcspTraceError(("%s:%d, triggerFirewallRestart failed\n", __FUNCTION__, __LINE__));
            }
            int iVal = 0;
            publishEventChangeIfSubscribed(SPEEDBOOST_NUMBER_OF_CONFIGURED_DEVICES, pNumOfConfiguredClients, &iVal);
            *pNumOfConfiguredClients = 0;
            sprintf(cClientsCount, "%d",iVal);
            if (0 != syscfg_set_commit(NULL, SYSCFG_SPEEDBOOST_NUM_OF_CONFIG_CLINETS, cClientsCount))
            {
                CcspTraceError(("%s:%d, syscfg_set_commit failed for %s\n", __FUNCTION__, __LINE__, SYSCFG_SPEEDBOOST_NUM_OF_CONFIG_CLINETS));
                execRetVal->ErrorCode = SYSCFG_FAILURE;
                snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "syscfg commit failed");
                return execRetVal;
            }
            execRetVal->ErrorCode = BLOB_EXEC_SUCCESS;
            snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "Empty XmSpeedBoost blob execution Success");
        }
        else
        {
            CcspTraceError(("%s: Failed to stop scheduler.\n", __FUNCTION__));
            execRetVal->ErrorCode = BLOB_EXEC_FAILURE;
            strncpy(execRetVal->ErrorMsg,"Failed to stop scheduler",sizeof(execRetVal->ErrorMsg)-1);
        }
        return execRetVal;
    }

    if ((0 < pSpeedBoostDoc->pSchedulerInfo->absolute_size) && (NULL == pSpeedBoostDoc->pSchedulerInfo->absolute))
    {
        CcspTraceError(("%s:%d, pSpeedBoostDoc->pSchedulerInfo->absolute is NULL\n", __FUNCTION__, __LINE__));
        execRetVal->ErrorCode = NULL_BLOB_EXEC_POINTER;
        snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "NULL pointer passed for absolute");
        return execRetVal;
    }
    CcspTraceInfo(("%s:%d, Number of Unix Time: %d\n", __FUNCTION__, __LINE__, pSpeedBoostDoc->pSchedulerInfo->absolute_size));
    CcspTraceInfo(("%s:%d, Number of Mac Addresses: %d\n", __FUNCTION__, __LINE__, pSpeedBoostDoc->pSchedulerInfo->actions_size));

    for (size_t iVar = 0; iVar < pSpeedBoostDoc->pSchedulerInfo->absolute_size; iVar++)
    {
        char cErrMessage [BUFF_SIZE_64] = {0};
        if (!validateUnixTime(pSpeedBoostDoc->pSchedulerInfo->absolute[iVar].time, cErrMessage, sizeof(cErrMessage)))
        {
            CcspTraceError(("%s:%d, errMessage:%s\n", __FUNCTION__, __LINE__,cErrMessage));
            execRetVal->ErrorCode = VALIDATION_FALIED;
            snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", cErrMessage);
            return execRetVal;
        }
        iNumberOfIndexes += pSpeedBoostDoc->pSchedulerInfo->absolute[iVar].action_count;
    }

    if (MAX_MAC_ADDR_COUNT < pSpeedBoostDoc->pSchedulerInfo->actions_size)
    {
        CcspTraceError(("%s:%d, Number of Mac Addresses(%d) exceeds the limit\n", __FUNCTION__, __LINE__, pSpeedBoostDoc->pSchedulerInfo->actions_size));
        execRetVal->ErrorCode = VALIDATION_FALIED;
        snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "Number of Mac Addresses exceeds the limit");
        return execRetVal;
    }

    if ((0 < pSpeedBoostDoc->pSchedulerInfo->actions_size) && (NULL == pSpeedBoostDoc->pSchedulerInfo->actions))
    {
        CcspTraceError(("%s:%d, pSpeedBoostDoc->pSchedulerInfo->actions is NULL\n", __FUNCTION__, __LINE__));
        execRetVal->ErrorCode = NULL_BLOB_EXEC_POINTER;
        snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "NULL pointer passed for actions");
        return execRetVal;
    }

    for (size_t iVar = 0; iVar < pSpeedBoostDoc->pSchedulerInfo->actions_size; iVar++)
    {
        char * pMac = strdup (pSpeedBoostDoc->pSchedulerInfo->actions[iVar]);
        if (NULL == pMac)
        {
            CcspTraceError(("%s:%d, Unable to find the Mac Addresses\n", __FUNCTION__, __LINE__));
            execRetVal->ErrorCode = NULL_BLOB_EXEC_POINTER;
            snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "Unable to find the Mac Addresses");
            return execRetVal;
        }
        if (!ValidateMac(pMac))
        {
            CcspTraceError(("%s:%d, Invalid MAC Address\n", __FUNCTION__, __LINE__));
            execRetVal->ErrorCode = VALIDATION_FALIED;
            snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "Invalid MAC Address");
            free(pMac);
            return execRetVal;
        }
        else
        {
            iSpeedBoostClientsCount++;
            CcspTraceInfo(("%s:%d, MAC Address[%d]:%s is Valid\n", __FUNCTION__, __LINE__, iVar, pMac));
            free(pMac);
        }
    }

    CcspTraceInfo (("%s:%d,Blob Verification is Success\n",__FUNCTION__,__LINE__));
    CcspTraceInfo(("%s:%d, Number of SpeedBoost clients: %d\n", __FUNCTION__, __LINE__, iSpeedBoostClientsCount));


    if (0 == run_schedule(pSpeedBoostDoc->pSchedulerInfo, XM_SPEED_BOOST_RULES))
    {
        CcspTraceInfo(("%s: Scheduler started..\n", __FUNCTION__));
        publishEventChangeIfSubscribed(SPEEDBOOST_NUMBER_OF_CONFIGURED_DEVICES, pNumOfConfiguredClients, &iSpeedBoostClientsCount);
        *pNumOfConfiguredClients = iSpeedBoostClientsCount;
        sprintf(cClientsCount, "%d",iSpeedBoostClientsCount);
        if (0 != syscfg_set_commit(NULL, SYSCFG_SPEEDBOOST_NUM_OF_CONFIG_CLINETS, cClientsCount))
        {
            CcspTraceError(("%s:%d, syscfg_set_commit failed for %s\n", __FUNCTION__, __LINE__, SYSCFG_SPEEDBOOST_NUM_OF_CONFIG_CLINETS));
            execRetVal->ErrorCode = SYSCFG_FAILURE;
            snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "syscfg commit failed");
            return execRetVal;
        }

        execRetVal->ErrorCode = BLOB_EXEC_SUCCESS;
        snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "XmSpeedBoost blob execution Success");
    }
    else
    {
        CcspTraceInfo(("%s: Failed to start scheduler.\n", __FUNCTION__));
        execRetVal->ErrorCode = BLOB_EXEC_FAILURE;
        snprintf(execRetVal->ErrorMsg, sizeof(execRetVal->ErrorMsg) - 1, "%s", "Failed to start scheduler");
    }
    return execRetVal;
}

bool unpackAndProcessSpeedBoostData(char *pString)
{
    char * pDecodeMsg             = NULL;
    int                     iSize = 0;
    int                   iRetVal = 0;
    bool                  bReturn = false;

    msgpack_unpack_return eMsgUnpackRet = MSGPACK_UNPACK_SUCCESS;

    CcspTraceInfo(("%s:%d, Entry\n", __FUNCTION__, __LINE__));

    if (NULL == pString)
    {
        CcspTraceError(("%s:%d, Invalid input parameters\n", __FUNCTION__, __LINE__));
        return bReturn;
    }

    iRetVal = get_base64_decodedbuffer(pString, &pDecodeMsg, &iSize);
    if (0 == iRetVal)
    {
        eMsgUnpackRet = get_msgpack_unpack_status(pDecodeMsg,iSize);
        CcspTraceInfo(("%s:%d, Unpack Status: %d\n", __FUNCTION__, __LINE__, eMsgUnpackRet));
    }
    else
    {
        if (pDecodeMsg)
        {
            free(pDecodeMsg);
            pDecodeMsg = NULL;
        }
        return bReturn;
    }

    if(MSGPACK_UNPACK_SUCCESS == eMsgUnpackRet)
    {
        speedBoostDoc_t *pSpeedBoostDoc = NULL;
        pSpeedBoostDoc = speedBoostDocConvert (pDecodeMsg, iSize);

        if (NULL != pDecodeMsg)
        {
            CcspTraceError(("%s:%d, Freeing the Decode message\n", __FUNCTION__, __LINE__));
            free(pDecodeMsg);
            pDecodeMsg = NULL;
        }

        if (NULL != pSpeedBoostDoc)
        {
            CcspTraceInfo (("%s:%d, Subdoc Name: %s\n", __FUNCTION__, __LINE__, pSpeedBoostDoc->pSubDocName));
            CcspTraceInfo (("%s:%d, Subdoc Version: %d\n",__FUNCTION__, __LINE__, pSpeedBoostDoc->ui32Version));
            CcspTraceInfo (("%s:%d, Subdoc Transaction ID: %d\n",__FUNCTION__, __LINE__, pSpeedBoostDoc->ui16TranscationId));

            execData *pExecDataSubDoc = NULL;
            pExecDataSubDoc = (execData *) malloc (sizeof(execData));

            if (NULL != pExecDataSubDoc)
            {
                memset (pExecDataSubDoc, 0, sizeof(execData));
                pExecDataSubDoc->txid = pSpeedBoostDoc->ui16TranscationId;
                pExecDataSubDoc->version = pSpeedBoostDoc->ui32Version;
                pExecDataSubDoc->numOfEntries = 0;

                snprintf (pExecDataSubDoc->subdoc_name, sizeof(pExecDataSubDoc->subdoc_name) - 1, "%s", pSpeedBoostDoc->pSubDocName);
                pExecDataSubDoc->user_data = pSpeedBoostDoc;
                pExecDataSubDoc->calcTimeout = NULL;
                pExecDataSubDoc->executeBlobRequest = processSpeedBoostWebConfigRequest;
                pExecDataSubDoc->rollbackFunc = NULL;
                pExecDataSubDoc->freeResources = freeResourcesSpeedBoostDoc;
                PushBlobRequest(pExecDataSubDoc);
                CcspTraceInfo(("%s:%d, Pushed the Blob Request\n", __FUNCTION__, __LINE__));
                bReturn = true;
            }
            else
            {
                CcspTraceError(("%s:%d, Memory Allocation Failed\n", __FUNCTION__, __LINE__));
                speedBoostDocDestroy(pSpeedBoostDoc);
                bReturn = false;
            }
        }
        else if (NULL == pSpeedBoostDoc)
        {
            CcspTraceError(("%s:%d, SpeedBoostDoc is NULL\n", __FUNCTION__, __LINE__));
            bReturn = false;
        }
    }
    else
    {
        if (NULL != pDecodeMsg)
        {
            CcspTraceError(("%s:%d, Freeing the Decode message\n", __FUNCTION__, __LINE__));
            free(pDecodeMsg);
            pDecodeMsg = NULL;
        }
        CcspTraceError(("%s:%d, Message Unpack Failed\n", __FUNCTION__, __LINE__));
        bReturn = false;
    }
    return bReturn;
}
