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

#include "cosa_networklogs_apis.h"
#include "plugin_main_apis.h"
#include "safec_lib_common.h"

#define NETWORK_EVENT_LOG_FILE "/var/log/networklog"
#define NETWORK_EVENT_LOG_FILE_TEMP "/var/log/networklog_temp"

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
CosaDmlGetNetworkLogs
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_NETWORKLOGS_LOG   *ppConf
    )
{
    PCOSA_DML_NETWORKLOGS_LOG pEVENTLog = (PCOSA_DML_NETWORKLOGS_LOG) NULL;

    FILE *fp = NULL;
    int log0_lines =  0;

    int log_max_num = 0;
    int i = 0;
    int j = 0;
    int param_num = 0;

    char str[1024];
    char cmd[128];
    char tmp[128];

    /* The below arguments are to be initialised using sscanf */
    char evt[16] = {0};
    char date[16] = {0};
    char time[16] = {0};
    char month[16] = {0};
    char desc[512] = {0};

    if (access(NETWORK_EVENT_LOG_FILE, F_OK) == 0)
    {
        log0_lines = getLogLines(NETWORK_EVENT_LOG_FILE);
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }
    
    if(log0_lines == 0)
    {
        return ANSC_STATUS_FAILURE;
    }


    if(log0_lines > 100)
    {
        j = log0_lines-100;
    }
    sprintf(cmd, "tail -100 %s > %s", NETWORK_EVENT_LOG_FILE, NETWORK_EVENT_LOG_FILE_TEMP);
    system(cmd);

    // The maximum number of logs populated in DML is 100
    log_max_num = log0_lines < 100 ? log0_lines : 100;

    pEVENTLog= (PCOSA_DML_NETWORKLOGS_LOG)AnscAllocateMemory(log_max_num * sizeof(COSA_DML_NETWORKLOGS_LOG));
    if(pEVENTLog== NULL)
    {
        unlink(NETWORK_EVENT_LOG_FILE_TEMP);
        return ANSC_STATUS_FAILURE;
    }

    fp = fopen(NETWORK_EVENT_LOG_FILE_TEMP, "r");
    if(!fp)
    {
        goto RET_ERROR;
    }
    if (fseek(fp, 0, SEEK_END))
    {
        goto RET_ERROR;
    }
    else
    {
        fseek(fp, 0, SEEK_SET);
        while (fgets(str, sizeof(str), fp))
        {
            if (i >= log_max_num)
            {
                break;
            }
            /* Event Log format: <month> <date> <time> <local0.event level> <message> , sscanf is expected to return exact 5 parameter values */
            param_num = sscanf(str,"%15s %15s %15s %*s local0.%15s %*s %511[^\t\n]", month, date, time, evt, desc);

            if (param_num != 5)
            {
                //ignore this line if the format is not correct
                continue;
            }

            //TODO Will fix this EventID once we get clarity on event ID requirement
            pEVENTLog[i].EventID = 0;

            snprintf(tmp,sizeof(pEVENTLog[i].Time),"%s %s %s", date, month, time);
            strcpy(pEVENTLog[i].Time,tmp);

            if(!strcmp(evt, "unknown"))
                pEVENTLog[i].EventLevel = 0;
            else if(!strcmp(evt, "emergency"))
                pEVENTLog[i].EventLevel = 1;
            else if(!strcmp(evt, "alert"))
                pEVENTLog[i].EventLevel = 2;
            else if(!strcmp(evt, "critical"))
                pEVENTLog[i].EventLevel = 3;
            else if(!strcmp(evt, "error"))
                pEVENTLog[i].EventLevel = 4;
            else if(!strcmp(evt, "warning"))
                pEVENTLog[i].EventLevel = 5;
            else if(!strcmp(evt, "notice"))
                pEVENTLog[i].EventLevel = 6;
             else if(!strcmp(evt, "information"))
                pEVENTLog[i].EventLevel = 7;
             else if(!strcmp(evt, "debug"))
                pEVENTLog[i].EventLevel = 8;

            strcpy(pEVENTLog[i].Description, desc);
            i++;
	}
    }
    fclose(fp);
    unlink(NETWORK_EVENT_LOG_FILE_TEMP);
    *pulCount = i;
    if(*ppConf)
        AnscFreeMemory(*ppConf);
    *ppConf = pEVENTLog;
    return ANSC_STATUS_SUCCESS;

RET_ERROR:
    if(fp)
        fclose(fp);
    if(pEVENTLog)
        AnscFreeMemory(pEVENTLog);
    unlink(NETWORK_EVENT_LOG_FILE_TEMP);
    return ANSC_STATUS_FAILURE;
}

#endif
