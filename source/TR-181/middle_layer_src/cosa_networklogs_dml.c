/*****************************************************************************
 * Copyright 2022 Liberty Global B.V.
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

#ifdef FEATURE_NETWORK_LOGS

#include "ansc_platform.h"
#include "plugin_main_apis.h"
#include "cosa_networklogs_dml.h"
#include "cosa_networklogs_internal.h"
#include "cosa_networklogs_apis.h"
#include "safec_lib_common.h"

/***********************************************************************

 APIs for Object:

    NetworkLogs.Logs.{i}.

    *   Logs_GetEntryCount
    *   Logs_GetEntry
    *   Logs_IsUpdated
    *   Logs_Synchronize
    *   Logs_GetParamUlongValue
    *   Logs_GetParamStringValue

***********************************************************************/

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Logs_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/

ULONG
Logs_GetEntryCount
    (
        ANSC_HANDLE         hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DML_NETWORKLOGS pMyObject = (PCOSA_DML_NETWORKLOGS)g_pCosaBEManager->hNetworkLogs;

    return pMyObject->NetworkLogsLogNumber;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Logs_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/

ANSC_HANDLE
Logs_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DML_NETWORKLOGS pMyObject = (PCOSA_DML_NETWORKLOGS)g_pCosaBEManager->hNetworkLogs;

    if (nIndex < pMyObject->NetworkLogsLogNumber)
    {
        *pInsNumber = nIndex + 1;

        return  (ANSC_HANDLE)&pMyObject->pNetworkLogs_Log[nIndex];
    }

    return NULL;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Logs_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/

BOOL
Logs_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DML_NETWORKLOGS pMyObject = (PCOSA_DML_NETWORKLOGS)g_pCosaBEManager->hNetworkLogs;

    if( !pMyObject->NetworkLogUpdateTime )
    {
        pMyObject->NetworkLogUpdateTime = AnscGetTickInSeconds();
        return TRUE;
    }

    if( pMyObject->NetworkLogUpdateTime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else
    {
        pMyObject->NetworkLogUpdateTime = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Logs_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/

ULONG
Logs_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DML_NETWORKLOGS pMyObject = (PCOSA_DML_NETWORKLOGS)g_pCosaBEManager->hNetworkLogs;
    ANSC_STATUS ret = ANSC_STATUS_SUCCESS;

    if( pMyObject->pNetworkLogs_Log )
    {
        AnscFreeMemory(pMyObject->pNetworkLogs_Log);
        pMyObject->pNetworkLogs_Log = NULL;
    }

    pMyObject->NetworkLogsLogNumber = 0;

    ret = CosaDmlGetNetworkLogs
        (
            (ANSC_HANDLE)NULL,
            &pMyObject->NetworkLogsLogNumber,
            &pMyObject->pNetworkLogs_Log
        );

    if( ret != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pNetworkLogs_Log = NULL;
        pMyObject->NetworkLogsLogNumber = 0;
    }

    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Logs_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
Logs_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_NETWORKLOGS_LOG pConf = (PCOSA_DML_NETWORKLOGS_LOG)hInsContext;
    errno_t rc = -1;
    int ind = -1;

    /* check the parameter name and return the corresponding value */
    rc = strcmp_s("Index", strlen("Index"), ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->Index;

        return TRUE;
    }

    rc = strcmp_s("EventID", strlen("EventID"), ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->EventID;

        return TRUE;
    }

    rc = strcmp_s("EventLevel", strlen("EventLevel"), ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        *puLong = pConf->EventLevel;

        return TRUE;
    }

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Logs_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 4095 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/

ULONG
Logs_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize  
    )
{
    PCOSA_DML_NETWORKLOGS_LOG pConf = (PCOSA_DML_NETWORKLOGS_LOG)hInsContext;
    errno_t rc = -1;
    int ind = -1;

    /* check the parameter name and return the corresponding value */
    rc =  strcmp_s("Description", strlen("Description"), ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        if( strlen(pConf->Description) >= *pUlSize )
        {
            *pUlSize = strlen(pConf->Description);
            return 1;
        }

        rc = strcpy_s(pValue, *pUlSize, pConf->Description);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }

    rc = strcmp_s("Time", strlen("Time"), ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* collect value */
        if ( strlen(pConf->Time) >= *pUlSize )
        {
            *pUlSize = strlen(pConf->Time);
            return 1;
        }

        rc = strcpy_s(pValue, *pUlSize, pConf->Time);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }

        return 0;
    }

    return -1;
}

#endif
