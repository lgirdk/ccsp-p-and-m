/*****************************************************************************
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
 ****************************************************************************/

#include <sys/stat.h>
#include <fcntl.h>
#include <ansc_platform.h>
#include <ccsp_trace.h>
#include <syscfg/syscfg.h>
#include "cosa_ssam_apis.h"
#include "cosa_ssam_dml.h"

BOOL X_LGI_COM_DigitalSecurity_GetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG *puLong)
{
    char buf[12];

    if (strcmp("MaxStartDelay", ParamName) == 0) {
        syscfg_get(NULL, "ssam_maxstartdelay", buf, sizeof(buf));
        if (buf[0] != 0) {
            *puLong = (ULONG) atoi(buf);
        } else {
            *puLong = 30;
            AnscTraceWarning(("Error in syscfg_get for ssam_maxstartdelay\n"));
        }
        return TRUE;
    }

    if (strcmp("SigningKeyId", ParamName) == 0) {
        syscfg_get(NULL, "ssam_signingkeyid", buf, sizeof(buf));
        if (buf[0] != 0) {
            *puLong = (ULONG) atoi(buf);
        } else {
            *puLong = 0;
            AnscTraceWarning(("Error in syscfg_get for ssam_signingkeyid\n"));
        }
        return TRUE;
    }

    if (strcmp("ProvisioningModel", ParamName) == 0) {
        syscfg_get(NULL, "ssam_provisioningmodel", buf, sizeof(buf));
        if (buf[0] != 0) {
            *puLong = (ULONG) atoi(buf);
        } else {
            *puLong = 1;
            AnscTraceWarning(("Error in syscfg_get for ssam_provisioningmodel\n"));
        }
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_DigitalSecurity_SetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG uValue)
{
    if (strcmp("MaxStartDelay", ParamName) == 0) {
        if (syscfg_set_u_commit(NULL, "ssam_maxstartdelay", uValue) != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_maxstartdelay\n"));
        }
        return TRUE;
    }

    if (strcmp("SigningKeyId", ParamName) == 0) {
        if (syscfg_set_u_commit(NULL, "ssam_signingkeyid", uValue) != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_signingkeyid\n"));
        }
        return TRUE;
    }

    if (strcmp("ProvisioningModel", ParamName) == 0) {
        if (syscfg_set_u_commit(NULL, "ssam_provisioningmodel", uValue) != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_provisioningmodel\n"));
        }
        return TRUE;
    }

    return FALSE;
}

ULONG X_LGI_COM_DigitalSecurity_GetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize)
{
    if (strcmp("ProvisionedEnvironment", ParamName) == 0) {
        syscfg_get(NULL, "ssam_provisionedenv", pValue, *pUlSize);
        return 0;
    }

    if (strcmp("AgentPassword", ParamName) == 0) {
        strcpy(pValue, "");
        return 0;
    }

    if (strcmp("AgentVersion", ParamName) == 0) {
        char buf[12];

        syscfg_get(NULL, "ssam_enable", buf, sizeof(buf));
        if ((strcmp(buf, "1") != 0) || (access(SSAM_PID_FILE, F_OK) != 0)) {
           strcpy(pValue, "");
        }
        else {
           FILE *fp = NULL;
           char buffer[32] = { 0 };
           char version[32] = { 0 };

           fp = fopen(SSAM_PARTITION "/agent_version", "r");
           if (fp != NULL) {
               if ((fgets(buffer, sizeof(buffer), fp) != NULL) && (strlen(buffer) != 0)) {
                   snprintf(version, sizeof(version), "%s", buffer);
               }
               fclose(fp);
           }
           if (strlen(version) == 0) {
               strcpy(pValue, "");
           } else {
               strcpy(pValue, version);
           }
        }
        return 0;
    }

    if (strcmp("Status", ParamName) == 0) {
        FILE *fp = NULL;
        char buffer[32] = { 0 };
        char status[32] = { 0 };

        fp = fopen(SSAM_PARTITION "/status", "r");
        if (fp != NULL) {
            if ((fgets(buffer, sizeof(buffer), fp) != NULL) && (strlen(buffer) != 0)) {
                snprintf(status, sizeof(status), "%s", buffer);
            }
            fclose(fp);
        }
        if (strlen(status) == 0) {
            strcpy(pValue, "");
        } else {
            strcpy(pValue, status);
        }
        return 0;
    }

    if (strcmp("SecretKey", ParamName) == 0) {
        char *out;
        unsigned int size;

        CosaGetAgentpassword(&out, &size);
        if (out == NULL) {
            printf("decrypt failed\n");
            return -1;
        }

        snprintf(pValue, *pUlSize, "%s", out);
        free(out);
        return 0;
    }

    return -1;
}

BOOL X_LGI_COM_DigitalSecurity_SetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pString)
{
    if (strcmp("ProvisionedEnvironment", ParamName) == 0) {
        if (syscfg_set_commit(NULL, "ssam_provisionedenv", pString) == 0) {
            FILE *fp = fopen(SSAM_ENV, "w");
            if (fp != NULL) {
                fputs(pString, fp);
                fclose(fp);
            }
        }
        else {
            AnscTraceWarning(("Error in syscfg_set for ssam_provisionedenv\n"));
        }
        return TRUE;
    }

    if (strcmp("AgentPassword", ParamName) == 0) {
        if (CosaSetAgentpassword(pString) != 0) {
            return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_DigitalSecurity_GetParamBoolValue(ANSC_HANDLE hInsContext, char *ParamName, BOOL *pBool)
{
    char buf[12];

    if (strcmp("Enable", ParamName) == 0) {
        syscfg_get(NULL, "ssam_enable", buf, sizeof(buf));
        if (buf[0] != 0) {
            *pBool = (BOOL) atoi(buf);
        } else {
            *pBool = 0;
            AnscTraceWarning(("Error in syscfg_get for ssam_enable\n"));
        }
        return TRUE;
    }

    if (strcmp("UpdaterEnable", ParamName) == 0) {
        syscfg_get(NULL, "ssam_updaterenable", buf, sizeof(buf));
        if (buf[0] != 0) {
            *pBool = (BOOL) atoi(buf);
        } else {
            *pBool = 0;
            AnscTraceWarning(("Error in syscfg_get for ssam_updaterenable\n"));
        }
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_DigitalSecurity_SetParamBoolValue(ANSC_HANDLE hInsContext, char *ParamName, BOOL bValue)
{
    if (strcmp("Enable", ParamName) == 0) {
        if (syscfg_set_commit(NULL, "ssam_enable", bValue ? "1" : "0") != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_enable\n"));
        }
        if (bValue == TRUE) {
            ssam_start();
        } else {
            ssam_stop();
        }
        return TRUE;
    }

    if (strcmp("UpdaterEnable", ParamName) == 0) {
        if (syscfg_set_commit(NULL, "ssam_updaterenable", bValue ? "1" : "0") != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_updaterenable\n"));
        }
        return TRUE;
    }

    return FALSE;
}

ULONG X_LGI_COM_DigitalSecurity_Commit(ANSC_HANDLE hInsContext)
{
    return 0;
}
