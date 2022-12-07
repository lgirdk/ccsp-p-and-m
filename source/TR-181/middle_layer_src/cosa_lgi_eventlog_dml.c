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

#define EVENT_LOG_SIZE 100
#define EVENT_LOG_REFRESH_INTERVAL 20

#define LGI_EVENT_LOG_FILE          "/tmp/channel_event_1.log"
#define LGI_EVENT_LOG_FILE_TEMP     "/tmp/EventLogTemp"

#define TIME_NO_NEGATIVE(x) ((long)(x) < 0 ? 0 : (x))

static unsigned long logupdatetime;

static int update_pValue (char *pValue, PULONG pulSize, char *str)
{
    if (!str)
        return -1;

    size_t len = strlen(str);

    if (len < *pulSize)
    {
        memcpy(pValue, str, len + 1);
        return 0;
    }

    *pulSize = len + 1;
    return 1;
}

#if defined (_PUMA6_ARM_)

static ANSC_STATUS CosaDmlGetEventLog (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject)
{
    char str[ LGI_EVENT_LOG_TIME_LEN + 32 + LGI_EVENT_LOG_TAG_LEN + LGI_EVENT_LOG_INFO_LEN + 32 ];
    PCOSA_DATAMODEL_LGI_EVENTLOG pEVENTLog;
    FILE *fp;
    int log_max_num;
    int i;

    /*
       Note that max RPC command output is 4k bytes (?) so limit to 30 lines
       instead of 100 to try to avoid filling the RPC buffer.
       Fixme: needs review.
    */

    log_max_num = 30;

    pEVENTLog = AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_EVENTLOG) * log_max_num);

    if (pEVENTLog == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Read the last 30 lines (see above) from /tmp/channel_event_1.log from the Atom */

    if ((fp = popen("rpcclient2 'tail -n 30 " LGI_EVENT_LOG_FILE "'", "r")) == NULL)
    {
        AnscFreeMemory(pEVENTLog);
        return ANSC_STATUS_FAILURE;
    }

    i = 0;

    while (fgets(str, sizeof(str), fp))
    {
        char time[20];
        /*
           Event Log format: <date> <time> <pri> <tag> <message>
           Example: 2021-10-26 09:37:08 [4][WI] Radar signal detected, DFS sequence applied, channel changed from 100/80 to 36/80
        */
        if (sscanf(str, "%31s %19s [%lu][%127[^]]] %511[^\t\n]", pEVENTLog[i].Timestamp, time, &pEVENTLog[i].Pri, pEVENTLog[i].Tag, pEVENTLog[i].Message) != 5)
        {
            continue;
        }

        strcat(pEVENTLog[i].Timestamp, "T");
        strcat(pEVENTLog[i].Timestamp, time);

        i++;
    }

    pclose(fp);

    pMyObject->pEventLogTable = pEVENTLog;
    pMyObject->EventLogEntryCount = i;

    return ANSC_STATUS_SUCCESS;
}

#else

static int getLogLines (char *filename)
{
    FILE *fp;
    int lines = 0;

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return 0;
    }

    while (!feof(fp))
    {
        if (fgetc(fp) == '\n')
        {
            lines++;
        }
    }

    fclose(fp);

    return lines;
}

static ANSC_STATUS CosaDmlGetEventLog (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject)
{
    char str[ LGI_EVENT_LOG_TIME_LEN + 32 + LGI_EVENT_LOG_TAG_LEN + LGI_EVENT_LOG_INFO_LEN + 32 ];
    PCOSA_DATAMODEL_LGI_EVENTLOG pEVENTLog;
    FILE *fp;
    int log0_lines = 0;
    int log_max_num;
    int count;
    long pos;
    int i;

    if (access(LGI_EVENT_LOG_FILE, F_OK) == 0)
    {
        log0_lines = getLogLines(LGI_EVENT_LOG_FILE);
    }

    if (log0_lines == 0)
    {
        return ANSC_STATUS_FAILURE;
    }

    log_max_num = (log0_lines < EVENT_LOG_SIZE) ? log0_lines : EVENT_LOG_SIZE;

    pEVENTLog = AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_EVENTLOG) * log_max_num);

    if (pEVENTLog == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Read the last 100 lines (ie EVENT_LOG_SIZE) from /tmp/channel_event_1.log into new temp file */

    system("tail -100 " LGI_EVENT_LOG_FILE " > " LGI_EVENT_LOG_FILE_TEMP);

    if ((fp = fopen(LGI_EVENT_LOG_FILE_TEMP, "r")) == NULL)
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

    fseek(fp, 0, SEEK_SET);
    pos = ftell(fp);
    count = 0;

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

    i = 0;

    while (fgets(str, sizeof(str), fp))
    {
        char time[20];

        if (i >= log_max_num)
        {
            break;
        }

        /*
           Event Log format: <date> <time> <pri> <tag> <message>
           Example: 2021-10-26 09:37:08 [4][WI] Radar signal detected, DFS sequence applied, channel changed from 100/80 to 36/80
        */
        if (sscanf(str, "%31s %19s [%lu][%127[^]]] %511[^\t\n]", pEVENTLog[i].Timestamp, time, &pEVENTLog[i].Pri, pEVENTLog[i].Tag, pEVENTLog[i].Message) != 5)
        {
            continue;
        }

        strcat(pEVENTLog[i].Timestamp, "T");
        strcat(pEVENTLog[i].Timestamp, time);

        i++;
    }

    fclose(fp);

    unlink(LGI_EVENT_LOG_FILE_TEMP);

    pMyObject->pEventLogTable = pEVENTLog;
    pMyObject->EventLogEntryCount = i;

    return ANSC_STATUS_SUCCESS;
}

#endif

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

ULONG EventLog_GetEntryCount (ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE) g_pCosaBEManager->hLgiEventlog;

    return pMyObject->EventLogEntryCount;
}

ANSC_HANDLE EventLog_GetEntry (ANSC_HANDLE hInsContext, ULONG nIndex, ULONG *pInsNumber)
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE) g_pCosaBEManager->hLgiEventlog;

    if (nIndex < pMyObject->EventLogEntryCount)
    {
        *pInsNumber = nIndex + 1;
        return &pMyObject->pEventLogTable[nIndex];
    }

    return NULL;
}

BOOL EventLog_IsUpdated (ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE) g_pCosaBEManager->hLgiEventlog;

    if (logupdatetime == 0)
    {
        logupdatetime = AnscGetTickInSeconds();

        return TRUE;
    }

    if (logupdatetime >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - EVENT_LOG_REFRESH_INTERVAL))
    {
        return FALSE;
    }

    logupdatetime = AnscGetTickInSeconds();

    return TRUE;
}

ULONG EventLog_Synchronize (ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE) g_pCosaBEManager->hLgiEventlog;

    if (pMyObject->pEventLogTable)
    {
        AnscFreeMemory(pMyObject->pEventLogTable);
    }

    pMyObject->pEventLogTable = NULL;
    pMyObject->EventLogEntryCount = 0;

    CosaDmlGetEventLog(pMyObject);

    return ANSC_STATUS_SUCCESS;
}

BOOL EventLog_GetParamUlongValue (ANSC_HANDLE hInsContext, char *ParamName, ULONG *puLong)
{
    PCOSA_DATAMODEL_LGI_EVENTLOG pEventLogInfo = (PCOSA_DATAMODEL_LGI_EVENTLOG) hInsContext;

    if (strcmp(ParamName, "Pri") == 0)
    {
        *puLong = pEventLogInfo->Pri;

        return TRUE;
    }

    return FALSE;
}

ULONG EventLog_GetParamStringValue (ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize)
{
    PCOSA_DATAMODEL_LGI_EVENTLOG pEventLogInfo = (PCOSA_DATAMODEL_LGI_EVENTLOG) hInsContext;

    if (strcmp(ParamName, "Tag") == 0)
    {
        return update_pValue(pValue, pUlSize, pEventLogInfo->Tag);
    }

    if (strcmp(ParamName, "Timestamp") == 0)
    {
        return update_pValue(pValue, pUlSize, pEventLogInfo->Timestamp);
    }

    if (strcmp(ParamName, "Message") == 0)
    {
        return update_pValue(pValue, pUlSize, pEventLogInfo->Message);
    }

    return -1;
}
