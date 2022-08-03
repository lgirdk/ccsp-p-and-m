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
#ifdef FEATURE_RDKB_WAN_MANAGER

#include "cosa_networklogs_apis.h"
#include "plugin_main_apis.h"
#include "safec_lib_common.h"

ANSC_STATUS
CosaDmlGetNetworkLogs
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_NETWORKLOGS_LOG   *ppConf
    )
{
    UNREFERENCED_PARAMETER(hContext);
    PCOSA_DML_NETWORKLOGS_LOG pMyLog = (PCOSA_DML_NETWORKLOGS_LOG)NULL;

    //Initialize values for datamodel population
    char *description = "";
    char *time        = "00:00:00";
    errno_t rc        = -1;
    int no_of_logs    = 1;
    int count         = 0;

    pMyLog = (PCOSA_DML_NETWORKLOGS_LOG)AnscAllocateMemory(no_of_logs * sizeof(COSA_DML_NETWORKLOGS_LOG));

    if(pMyLog == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    for(int i = 0; i < no_of_logs; i++)
    {
        pMyLog[i].Index      = 0;
        pMyLog[i].EventID    = 1000;
        pMyLog[i].EventLevel = 0;

        rc = memcpy_s(pMyLog[i].Description, sizeof(pMyLog[i].Description), description, sizeof(pMyLog[i].Description));
        if(rc != EOK)
        {
            ERR_CHK(rc);
            free(pMyLog);

            return ANSC_STATUS_FAILURE;
        }   

        rc = memcpy_s(pMyLog[i].Time, sizeof(pMyLog[i].Time), time, sizeof(pMyLog[i].Time));
        if(rc != EOK)
        {
            ERR_CHK(rc);
            free(pMyLog);

            return ANSC_STATUS_FAILURE;
        }
        count++;
    }

    *pulCount = count;
    *ppConf   = pMyLog;

    return ANSC_STATUS_SUCCESS;
}

#endif