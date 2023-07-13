/*********************************************************************************
 * Copyright 2023 Liberty Global B.V.
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
#ifdef FEATURE_STATIC_IPV4

#include "ansc_platform.h"
#include "plugin_main_apis.h"
#include "cosa_lgi_staticip_dml.h"
#include "cosa_lgi_staticip_apis.h"
#include "cosa_lgi_staticip_internal.h"
#include <syscfg/syscfg.h>

BOOL X_LGI_COM_StaticIPv4Address_GetParamUlongValue ( ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong )
{
    PCOSA_DATAMODEL_LGI_STATICIP  pMyObject = (PCOSA_DATAMODEL_LGI_STATICIP)g_pCosaBEManager->hLgiStaticIP;

    if (strcmp(ParamName, "AdministrativeStatus") == 0)
    {
        *puLong = pMyObject->AdministrativeStatus;
        return TRUE;
    }

    if (strcmp(ParamName, "OperationalStatus") == 0)
    {
        CosaDmlStaticIPGetOperationalStatus(NULL, puLong);
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_StaticIPv4Address_SetParamUlongValue ( ANSC_HANDLE hInsContext, char* ParamName, ULONG uValuepUlong )
{
    PCOSA_DATAMODEL_LGI_STATICIP  pMyObject = (PCOSA_DATAMODEL_LGI_STATICIP)g_pCosaBEManager->hLgiStaticIP;

    /* No Support to enable Static IPv4 service in bridge or dslite mode */

    BRIDGE_AND_IPv6_MODE_JUDGEMENT_IFTRUE_RETURNFALSE

    if (strcmp(ParamName, "AdministrativeStatus") == 0)
    {
        pMyObject->AdministrativeStatus = uValuepUlong;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_StaticIPv4Address_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength )
{
    return TRUE;
}

ULONG X_LGI_COM_StaticIPv4Address_Commit ( ANSC_HANDLE hInsContext )
{
    PCOSA_DATAMODEL_LGI_STATICIP  pMyObject = (PCOSA_DATAMODEL_LGI_STATICIP)g_pCosaBEManager->hLgiStaticIP;

    CosaDmlStaticIPSetAdministrativeStatus(NULL, pMyObject->AdministrativeStatus);

    return 0;
}

ULONG X_LGI_COM_StaticIPv4Address_Rollback ( ANSC_HANDLE hInsContext )
{
    return 0;
}

#endif
