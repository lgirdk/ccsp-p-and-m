/*********************************************************************************
 * Copyright 2021 Liberty Global B.V.
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

#ifndef  _COSA_LGI_EVENTLOG_DML_H_
#define  _COSA_LGI_EVENTLOG_DML_H_
#define TIME_NO_NEGATIVE(x) ((long)(x) < 0 ? 0 : (x))

#define LGI_EVENT_LOG_TIME_LEN 64
#define LGI_EVENT_LOG_INFO_LEN 512


typedef  struct
_COSA_DATAMODEL_LGI_EVENTLOG
{
    char                           Tag[128];
    unsigned long int              Pri;
    char                           Timestamp[LGI_EVENT_LOG_TIME_LEN];
    char                           Message[LGI_EVENT_LOG_INFO_LEN];

}
COSA_DATAMODEL_LGI_EVENTLOG, *PCOSA_DATAMODEL_LGI_EVENTLOG;

ULONG
EventLog_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    );
ANSC_HANDLE
EventLog_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );
BOOL
EventLog_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    );
ULONG
EventLog_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    );
BOOL
EventLog_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    );
ULONG
EventLog_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

#endif
