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

#include "cosa_lgi_iperf_internal.h"
#include "cosa_lgi_iperf_apis.h"

static void freeInternalData(PCOSA_DATAMODEL_LGI_IPERF pObj)
{
    if (pObj->result)
    {
        free(pObj->result);
    }
    AnscFreeMemory((ANSC_HANDLE)pObj);
}

ANSC_HANDLE CosaLgiIperfCreate(VOID)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pObj = (PCOSA_DATAMODEL_LGI_IPERF)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_IPERF));

    if (!pObj)
    {
        return (ANSC_HANDLE)NULL;
    }

    if (pthread_mutex_init(&pObj->lock, NULL) != 0)
    {
        CcspTraceError(("pthread_mutex_init() error\n"));
        exit(1);
    }

    if (pthread_cond_init(&pObj->cond, NULL) != 0)
    {
        CcspTraceError(("pthread_cond_init() error\n"));
        exit(1);
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pObj->Oid = COSA_DATAMODEL_LGI_IPERF_OID;
    pObj->Create = CosaLgiIperfCreate;
    pObj->Remove = CosaLgiIperfRemove;
    pObj->Initialize = CosaLgiIperfInitialize;

    pObj->Initialize((ANSC_HANDLE)pObj);

    return (ANSC_HANDLE)pObj;
}

ANSC_STATUS CosaLgiIperfInitialize(ANSC_HANDLE hThisObject)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF)hThisObject;
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    char buf[8];

    pthread_mutex_lock(&pObj->lock);

    strncpy(pObj->diagnosticsState, "None", MAX_STATE_LEN);
    pObj->command[0] = '\0';
    pObj->result = NULL;

    syscfg_get(NULL, "iperf_connection_timeout", buf, sizeof(buf));
    pObj->connectionTimeout = atoi(buf);
    pObj->averageCPUUsage = 0;
    pObj->peakCPUUsage = 0;
    pObj->iperfPid = -1;
    pObj->cancelTimer = FALSE;
    pObj->iperfThreadExists = FALSE;
    pObj->markedForDestruction = FALSE;

    pthread_mutex_unlock(&pObj->lock);

    return returnStatus;
}

ANSC_STATUS CosaLgiIperfRemove(ANSC_HANDLE hThisObject)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_IPERF pObj = (PCOSA_DATAMODEL_LGI_IPERF)hThisObject;

    /* Remove necessary resources */
    returnStatus = CosaDmlDestroyIperfThread(hThisObject);

    if (pthread_mutex_destroy(&pObj->lock) != 0)
    {
        CcspTraceError(("pthread_mutex_destroy() error\n"));
        returnStatus = ANSC_STATUS_FAILURE;
    }

    if (pthread_cond_destroy(&pObj->cond) != 0)
    {
        CcspTraceError(("pthread_cond_destroy() error\n"));
        returnStatus = ANSC_STATUS_FAILURE;
    }
    /* Remove self */
    freeInternalData(pObj);

    return returnStatus;
}
