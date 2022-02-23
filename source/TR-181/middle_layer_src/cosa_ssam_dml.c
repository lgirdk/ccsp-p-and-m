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

static int read_param_string_from_file (char *filename, char *pValue, ULONG *pUlSize)
{
    FILE *fp;

    if ((fp = fopen(filename, "r")) != NULL) {
        if (fgets(pValue, *pUlSize, fp) == NULL) {
            *pValue = 0;
        }
        else {
            size_t len = strlen(pValue);
            if ((len > 0) && (pValue[len - 1] == '\n'))
                pValue[len - 1] = 0;
        }
        fclose(fp);
    }
    else {
        *pValue = 0;
    }

    return 0;
}

BOOL X_LGI_COM_DigitalSecurity_GetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG *puLong)
{
    char buf[12];

    if (strcmp(ParamName, "MaxStartDelay") == 0) {
        syscfg_get(NULL, "ssam_maxstartdelay", buf, sizeof(buf));
        if (buf[0] != 0) {
            *puLong = (ULONG) atoi(buf);
        } else {
            *puLong = 30;
            AnscTraceWarning(("Error in syscfg_get for ssam_maxstartdelay\n"));
        }
        return TRUE;
    }

    if (strcmp(ParamName, "SigningKeyId") == 0) {
        syscfg_get(NULL, "ssam_signingkeyid", buf, sizeof(buf));
        if (buf[0] != 0) {
            *puLong = (ULONG) atoi(buf);
        } else {
            *puLong = 0;
            AnscTraceWarning(("Error in syscfg_get for ssam_signingkeyid\n"));
        }
        return TRUE;
    }

    if (strcmp(ParamName, "ProvisioningModel") == 0) {
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
    if (strcmp(ParamName, "MaxStartDelay") == 0) {
        if (syscfg_set_u_commit(NULL, "ssam_maxstartdelay", uValue) != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_maxstartdelay\n"));
        }
        return TRUE;
    }

    if (strcmp(ParamName, "SigningKeyId") == 0) {
        if (syscfg_set_u_commit(NULL, "ssam_signingkeyid", uValue) != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_signingkeyid\n"));
        }
        return TRUE;
    }

    if (strcmp(ParamName, "ProvisioningModel") == 0) {
        if (syscfg_set_u_commit(NULL, "ssam_provisioningmodel", uValue) != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_provisioningmodel\n"));
        }
        return TRUE;
    }

    return FALSE;
}

ULONG X_LGI_COM_DigitalSecurity_GetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize)
{
    if (strcmp(ParamName, "ProvisionedEnvironment") == 0) {
        syscfg_get(NULL, "ssam_provisionedenv", pValue, *pUlSize);
        return 0;
    }

    if (strcmp(ParamName, "AgentPassword") == 0) {
        strcpy(pValue, "");
        return 0;
    }

    if (strcmp(ParamName, "AgentVersion") == 0) {
        if (*pUlSize <= 32) {
            *pUlSize = 32 + 1;
            return 1;
        }
        return read_param_string_from_file("/var/sam/agent_version", pValue, pUlSize);
    }

    if (strcmp(ParamName, "Status") == 0) {
        if (*pUlSize <= 32) {
            *pUlSize = 32 + 1;
            return 1;
        }
        return read_param_string_from_file("/var/sam/status", pValue, pUlSize);
    }

    if (strcmp(ParamName, "SecretKey") == 0) {
        syscfg_get(NULL, "ssam_agentpasswd", pValue, *pUlSize);
        return 0;
    }

    return -1;
}

BOOL X_LGI_COM_DigitalSecurity_SetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pString)
{
    if (strcmp(ParamName, "ProvisionedEnvironment") == 0) {
        if (syscfg_set_commit(NULL, "ssam_provisionedenv", pString) == 0) {
            FILE *fp = fopen("/var/tmp/environment", "w");
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

    if (strcmp(ParamName, "AgentPassword") == 0) {
        if (syscfg_set_commit(NULL, "ssam_agentpasswd", pString) != 0) {
            AnscTraceWarning(("Error in syscfg_set for ssam_agentpasswd\n"));
        }
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_DigitalSecurity_GetParamBoolValue(ANSC_HANDLE hInsContext, char *ParamName, BOOL *pBool)
{
    char buf[8];

    if (strcmp(ParamName, "Enable") == 0) {
        syscfg_get(NULL, "ssam_enable", buf, sizeof(buf));
        *pBool = (strcmp(buf, "1") == 0) ? TRUE : FALSE;
        return TRUE;
    }

    if (strcmp(ParamName, "UpdaterEnable") == 0) {
        syscfg_get(NULL, "ssam_updaterenable", buf, sizeof(buf));
        *pBool = (strcmp(buf, "1") == 0) ? TRUE : FALSE;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_DigitalSecurity_SetParamBoolValue(ANSC_HANDLE hInsContext, char *ParamName, BOOL bValue)
{
    if (strcmp(ParamName, "Enable") == 0) {
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

    if (strcmp(ParamName, "UpdaterEnable") == 0) {
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
