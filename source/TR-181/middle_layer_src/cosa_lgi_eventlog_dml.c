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
#define _GNU_SOURCE
#include "ctype.h"
#include "ansc_platform.h"
#include "plugin_main_apis.h"
#include "cosa_lgi_eventlog_dml.h"
#include "cosa_lgi_eventlog_internal.h"
#include <sys/time.h>
#include <sys/stat.h>

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply
 the changes to all of the specified Parameters atomically. That is, either
 all of the value changes are applied together, or none of the changes are
 applied at all. In the latter case, the CPE MUST return a fault response
 indicating the reason for the failure to apply the changes.

 The CPE MUST NOT apply any of the specified changes without applying all
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }

***********************************************************************/
#define EVENT_LOG_REFRESH_INTERVAL 20
#define EVENT_LOG_SIZE 100
/* Event Log format: <date> <time> <pri> <tag> <message> */
/* eg. 2021-10-26 09:37:08 [4][WI] Radar signal detected, DFS sequence applied, channel changed from 100/80 to 36/80 */
#define EVENT_LOG_LINE_FORMAT "%127s %127s [%127[^]]][%127[^]]] %511[^\t\n]"
#define EVENT_LOG_STR_SIZE 128
#define EVENT_LOG_PARAM_NUM_IN_LINE 5
#define LGI_EVENT_LOG_FILE "/tmp/channel_event_1.log"
#define LGI_EVENT_LOG_FILE_TEMP "/tmp/EventLogTemp"

static ulong logupdatetime;

static ULONG getLogLines(char *filename)
{
    FILE *fp = NULL;
    char ch = '\0';
    ULONG lines = 0;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return 0;
    }

    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            lines++;
        }
    }
    fclose(fp);
    return lines;
}

ANSC_STATUS
CosaDmlGetEventLog
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DATAMODEL_LGI_EVENTLOG    *ppConf
    )
{
    int log_max_num = 0;
    int count = 0;
    int i = 0;
    int param_num = 0;
    PCOSA_DATAMODEL_LGI_EVENTLOG pEVENTLog = NULL;
    unsigned long long pos = 0;

    char str[EVENT_LOG_PARAM_NUM_IN_LINE*EVENT_LOG_STR_SIZE] = {0};
    char cmd[EVENT_LOG_STR_SIZE] = {0};
    char date[EVENT_LOG_STR_SIZE] = {0};
    char time[EVENT_LOG_STR_SIZE] = {0};
    char tag[EVENT_LOG_STR_SIZE] = {0};
    char pri[EVENT_LOG_STR_SIZE] = {0};
    char desc[LGI_EVENT_LOG_INFO_LEN] = {0};
    char tmp[EVENT_LOG_STR_SIZE] = {0};
    FILE *fp = NULL;
    int log0_lines =  0;

    if (access(LGI_EVENT_LOG_FILE, F_OK) == 0)
    {
        log0_lines = getLogLines(LGI_EVENT_LOG_FILE);
    }
    if(log0_lines == 0)
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Read the last 100 lines from /tmp/channel_event_1.log */
    sprintf(cmd, "tail -100 %s > %s", LGI_EVENT_LOG_FILE, LGI_EVENT_LOG_FILE_TEMP);
 
    system(cmd);
    log_max_num =( log0_lines < EVENT_LOG_SIZE) ? (log0_lines) : (EVENT_LOG_SIZE);
    pEVENTLog= (PCOSA_DATAMODEL_LGI_EVENTLOG)AnscAllocateMemory(log_max_num * sizeof(COSA_DATAMODEL_LGI_EVENTLOG));
    if(pEVENTLog== NULL)
    {
        unlink(LGI_EVENT_LOG_FILE_TEMP);
        return ANSC_STATUS_FAILURE;
    }

    fp = fopen(LGI_EVENT_LOG_FILE_TEMP, "r");
    if(!fp)
    {
        AnscFreeMemory(pEVENTLog);
        unlink(LGI_EVENT_LOG_FILE_TEMP);
        return ANSC_STATUS_FAILURE;
    }
    if (fseek(fp, 0, SEEK_END))
    {
        AnscFreeMemory(pEVENTLog);
        fclose(fp);
        unlink(LGI_EVENT_LOG_FILE_TEMP);
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        fseek(fp, 0, SEEK_SET);
        pos = ftell(fp);

    while (pos)
    {
        if (!fseek(fp, --pos, SEEK_SET))
            {
                if (fgetc(fp) == '\n')
                {
                    if (count++ == EVENT_LOG_SIZE)
                    {
                        break;
                    }
                }
            }
        }
        while (fgets(str, sizeof(str), fp))
        {
            if (i >= log_max_num)
                {
                    break;
                }
            /* Event Log format: <date> <time> <pri> <tag> <message> */
            param_num = sscanf(str, EVENT_LOG_LINE_FORMAT, date, time, pri, tag, desc);

        if (param_num != EVENT_LOG_PARAM_NUM_IN_LINE)
            {
                //ignore this line if the format is not correct
                continue;
            }
            snprintf(tmp,LGI_EVENT_LOG_TIME_LEN,"%s,%s.0", date,time);
            strncpy(pEVENTLog[i].Timestamp,tmp,LGI_EVENT_LOG_TIME_LEN);

         strncpy(pEVENTLog[i].Tag, tag, EVENT_LOG_STR_SIZE);
         sscanf(pri, "%d", &pEVENTLog[i].Pri);
         strncpy(pEVENTLog[i].Message, desc, LGI_EVENT_LOG_INFO_LEN);
            i++;
        
       }
    }
    fclose(fp);
    unlink(LGI_EVENT_LOG_FILE_TEMP);
    *pulCount = i;
    *ppConf = pEVENTLog;
    return ANSC_STATUS_SUCCESS;
}

/***********************************************************************

 APIs for Object:

    X_LGI_COM_EventLogging.EventLog

    *  EventLog_GetEntryCount
    *  EventLog_GetEntry
    *  EventLog_IsUpdated
    *  EventLog_Synchronize
    *  EventLog_GetParamUlongValue
    *  EventLog_GetParamStringValue

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EventLog_GetEntryCount
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
EventLog_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE            pMyObject     = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)g_pCosaBEManager->hLgiEventlog;
    return pMyObject->EventLogEntryCount;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        EventLog_GetEntry
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
EventLog_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE            pMyObject     = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)g_pCosaBEManager->hLgiEventlog;
    if (nIndex < pMyObject->EventLogEntryCount)
    {
        *pInsNumber  = nIndex + 1;
        return  &pMyObject->pEventLogTable[nIndex];
    }
    return NULL; /* return the handle */
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EventLog_IsUpdated
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
EventLog_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE            pMyObject     = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)g_pCosaBEManager->hLgiEventlog;
    if ( !logupdatetime)
    {
        logupdatetime = AnscGetTickInSeconds();
        return TRUE;
    }
    if ( logupdatetime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - EVENT_LOG_REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else
    {
        logupdatetime = AnscGetTickInSeconds();
        return TRUE;
    }
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EventLog_Synchronize
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
EventLog_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE            pMyObject     = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)g_pCosaBEManager->hLgiEventlog;
    ANSC_STATUS                     ret           = ANSC_STATUS_SUCCESS;
    
    if ( pMyObject->pEventLogTable )
    {
        AnscFreeMemory(pMyObject->pEventLogTable);
        pMyObject->pEventLogTable = NULL;
        pMyObject->EventLogEntryCount = 0;
    }
    ret = CosaDmlGetEventLog
        (
            (ANSC_HANDLE)NULL,
            &pMyObject->EventLogEntryCount,
            &pMyObject->pEventLogTable
        );
    if ( ret != ANSC_STATUS_SUCCESS )
    {
        pMyObject->pEventLogTable = NULL;
        pMyObject->EventLogEntryCount = 0;
    }
    return ANSC_STATUS_SUCCESS;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EventLog_GetParamUlongValue
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
EventLog_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    
    PCOSA_DATAMODEL_LGI_EVENTLOG    pEventLogInfo  = (PCOSA_DATAMODEL_LGI_EVENTLOG)hInsContext;
    if (strcmp(ParamName, "Pri") == 0)
     {
         *puLong = pEventLogInfo->Pri;
         return TRUE;
     }
    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EventLog_GetParamStringValue
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
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
EventLog_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{

    PCOSA_DATAMODEL_LGI_EVENTLOG    pEventLogInfo  = (PCOSA_DATAMODEL_LGI_EVENTLOG)hInsContext;
    if (strcmp(ParamName, "Tag") == 0)
    {
        if ( _ansc_strlen(pEventLogInfo->Tag) >= *pUlSize )
        {
            *pUlSize = _ansc_strlen(pEventLogInfo->Tag);
            return 1;
        }
        AnscCopyString(pValue, pEventLogInfo->Tag);
        return 0;
    }
    if (strcmp(ParamName, "Timestamp") == 0)
    {
        if ( _ansc_strlen(pEventLogInfo->Timestamp) >= *pUlSize )
        {
            *pUlSize = _ansc_strlen(pEventLogInfo->Timestamp);
            return 1;
        }
        AnscCopyString(pValue, pEventLogInfo->Timestamp);
        return 0;
    }
    if (strcmp(ParamName, "Message") == 0)
    {
        if ( _ansc_strlen(pEventLogInfo->Message) >= *pUlSize )
        {
            *pUlSize = _ansc_strlen(pEventLogInfo->Message);
            return 1;
        }
        AnscCopyString(pValue, pEventLogInfo->Message);
        return 0;
    }

    return -1;
}
