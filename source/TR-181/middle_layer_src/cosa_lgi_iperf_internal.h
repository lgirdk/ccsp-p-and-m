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

#ifndef  _COSA_LGI_IPERF_INTERNAL_H
#define  _COSA_LGI_IPERF_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include <pthread.h>

#define MAX_STATE_LEN 33 // Includes null termination
#define MAX_CMD_LEN 257 // Includes null termination
#define UTILITY_NAME "iperf3"

#define  COSA_DATAMODEL_LGI_IPERF_CLASS_CONTENT      \
    /* duplication of the base object class content */ \
    COSA_BASE_CONTENT                                  \

typedef  struct
_COSA_DATAMODEL_LGI_IPERF_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_IPERF_CLASS_CONTENT
    /* start of LGI object class content */
    pthread_cond_t cond;
    pthread_mutex_t lock;
    ULONG connectionTimeout;
    ULONG peakCPUUsage;
    ULONG averageCPUUsage;
    char *result;
    char command[MAX_CMD_LEN];
    char diagnosticsState[MAX_STATE_LEN];
    BOOL diagnosticsStateChanged;
    BOOL markedForDestruction;
    pthread_t iperfTid;
    int iperfPid;
    BOOL iperfThreadExists;
    BOOL cancelTimer;
}
COSA_DATAMODEL_LGI_IPERF, *PCOSA_DATAMODEL_LGI_IPERF;

/*
    Standard function declaration
*/
ANSC_HANDLE CosaLgiIperfCreate(VOID);

ANSC_STATUS CosaLgiIperfInitialize(ANSC_HANDLE hThisObject);

ANSC_STATUS CosaLgiIperfRemove(ANSC_HANDLE hThisObject);

#endif
