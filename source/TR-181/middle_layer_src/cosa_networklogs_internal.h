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

#ifndef _COSA_NETWORKLOGS_INTERNAL_H_
#define _COSA_NETWORKLOGS_INTERNAL_H_

#include "cosa_apis.h"
#include "ansc_platform.h"
#include "plugin_main_apis.h"


#define REFRESH_INTERVAL                    20
#define TIME_NO_NEGATIVE(x) ((long)(x) < 0 ? 0 : (x))

typedef struct _COSA_DML_NETWORKLOGS_LOG
{
    ULONG                           Index;
    ULONG                           EventID;
    ULONG                           EventLevel;
    CHAR                            Time[64];
    CHAR                            Description[256];   
}COSA_DML_NETWORKLOGS_LOG, *PCOSA_DML_NETWORKLOGS_LOG;


#define COSA_NETWORKLOGS_CLASS_CONTENT                                                      \
    /* duplication of the base object class content */                                      \
        COSA_BASE_CONTENT                                                                   \
        PCOSA_DML_NETWORKLOGS_LOG                   pNetworkLogs_Log;                       \
        ULONG                                       NetworkLogsLogNumber;                   \
        ULONG                                       NetworkLogUpdateTime;                   \

typedef struct _COSA_DML_NETWORKLOGS
{
    COSA_NETWORKLOGS_CLASS_CONTENT
}
COSA_DML_NETWORKLOGS, *PCOSA_DML_NETWORKLOGS;

/**
 * @brief This function is used to create NetworkLogs object and return handle.
 *
 * @return  Returns newly created object handle.
 */
ANSC_HANDLE
CosaNetworkLogsCreate
    (
        VOID
    );

/**
 * @brief This function is used to Initialize NetworkLogs object created (Device.NetworkLogs.)
 *
 * @param[in] hThisObject  Object handle.
 *
 * @return  Returns status of the operation.
 */
ANSC_STATUS
CosaNetworkLogsInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

/**
 * @brief This function is used to remove NetworkLogs object(Device.NetworkLogs).
 *
 * @param[in] hThisObject  Object handle to delete.
 *
 * @return  Returns ANSC_STATUS_SUCCESS on success case.
 */
ANSC_STATUS
CosaNetworkLogsRemove
    (
        ANSC_HANDLE                 hThisObject
    );

#endif

#endif