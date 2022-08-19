/*********************************************************************************
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
 *********************************************************************************/

#include "cosa_lgi_iperf_dml.h"
#include "cosa_lgi_iperf_apis.h"
#include "cosa_lgi_iperf_internal.h"
#include "safec_lib_common.h"

/***********************************************************************
 *
 *  APIs for Object:
 *
 *      X_LGI-COM_Iperf
 *
 *          *  LgiIperf_GetParamUlongValue
 *          *  LgiIperf_SetParamUlongValue
 *          *  LgiIperf_GetParamStringValue
 *          *  LgiIperf_SetParamStringValue
 *          *  LgiIperf_Validate
 *          *  LgiIperf_Commit
 *          *  LgiIperf_Rollback
 *
 ************************************************************************/

BOOL LgiIperf_GetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG *pUlong)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF) g_pCosaBEManager->hLgiIperf;

    pthread_mutex_lock(&pObj->lock);

    if (strcmp(ParamName, "PeakCPUUsage") == 0)
    {
        *pUlong = pObj->peakCPUUsage;
    }
    else if (strcmp(ParamName, "AverageCPUUsage") == 0)
    {
        *pUlong = pObj->averageCPUUsage;
    }
    else if (strcmp(ParamName, "ConnectionTimeout") == 0)
    {
        CosaDmlIperfGetConnectionTimeout(&pObj->connectionTimeout);
        *pUlong = pObj->connectionTimeout;
    }
    else
    {
        pthread_mutex_unlock(&pObj->lock);
        return FALSE;
    }

    pthread_mutex_unlock(&pObj->lock);
    return TRUE;
}

ULONG LgiIperf_GetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF) g_pCosaBEManager->hLgiIperf;
    errno_t rc = EOK;

    pthread_mutex_lock(&pObj->lock);

    if (strcmp(ParamName, "DiagnosticsState") == 0)
    {
        rc = strcpy_s(pValue, *pUlSize, pObj->diagnosticsState);
    }
    else if (strcmp(ParamName, "Command") == 0)
    {
        rc = strcpy_s(pValue, *pUlSize, pObj->command);
    }
    else if (strcmp(ParamName, "Result") == 0)
    {
        if (pObj->result && strlen(pObj->result) >= *pUlSize)
        {
            *pUlSize = strlen(pObj->result) + 1;
            pthread_mutex_unlock(&pObj->lock);
            return 1;
        }
        rc = strcpy_s(pValue, *pUlSize, pObj->result ? pObj->result : "N/A");
    }
    else
    {
        pthread_mutex_unlock(&pObj->lock);
        return -1;
    }

    pthread_mutex_unlock(&pObj->lock);
    if (rc != EOK)
    {
        ERR_CHK(rc);
        return -1;
    }
    return 0;
}

BOOL LgiIperf_SetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG uValue)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF)g_pCosaBEManager->hLgiIperf;

    pthread_mutex_lock(&pObj->lock);

    if (strcmp(ParamName, "ConnectionTimeout") == 0)
    {
        ULONG uOldValue;

        CosaDmlIperfGetConnectionTimeout(&uOldValue);
        if (uOldValue != uValue)
        {
            CosaDmlIperfSetConnectionTimeout(pObj, uValue);
            CosaDmlIperfSetDiagnosticsState(pObj, "None", TRUE);
        }
    }
    else
    {
        pthread_mutex_unlock(&pObj->lock);
        return FALSE;
    }

    pthread_mutex_unlock(&pObj->lock);
    return TRUE;
}

BOOL LgiIperf_SetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *strValue)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF)g_pCosaBEManager->hLgiIperf;

    pthread_mutex_lock(&pObj->lock);

    if (strcmp(ParamName, "DiagnosticsState") == 0)
    {
        CosaDmlIperfSetDiagnosticsState(pObj, strValue, TRUE);
    }
    else if (strcmp(ParamName, "Command") == 0)
    {
        if (strcmp(pObj->command, strValue) != 0)
        {
            snprintf(pObj->command, MAX_CMD_LEN, "%s", strValue);
            CosaDmlIperfSetDiagnosticsState(pObj, "None", TRUE);
        }
    }
    else
    {
        pthread_mutex_unlock(&pObj->lock);
        return FALSE;
    }

    pthread_mutex_unlock(&pObj->lock);
    return TRUE;
}

BOOL LgiIperf_Validate(ANSC_HANDLE hInsContext, char *pReturnParamName, ULONG *puLength)
{
    return TRUE;
}

ULONG LgiIperf_Commit(ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF)g_pCosaBEManager->hLgiIperf;

    if (pObj->diagnosticsStateChanged)
    {
        ANSC_STATUS ret;

        ret = CosaDmlDiagnosticsStateChangeAction(pObj);
        if (ret != ANSC_STATUS_SUCCESS)
        {
            return -1;
        }
        pObj->diagnosticsStateChanged = FALSE;
    }

    return 0;
}

ULONG LgiIperf_Rollback(ANSC_HANDLE hInsContext)
{
    return 0;
}
