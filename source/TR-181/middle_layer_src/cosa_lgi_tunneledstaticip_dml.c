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

#include "ansc_platform.h"
#include "plugin_main_apis.h"
#include "cosa_lgi_tunneledstaticip_dml.h"
#include "cosa_lgi_tunneledstaticip_apis.h"
#include "cosa_lgi_tunneledstaticip_internal.h"
#include <syscfg/syscfg.h>


BOOL X_LGI_COM_TunneledStaticIPService_GetParamBoolValue (ANSC_HANDLE hInsContext, char *ParamName, BOOL* pBool)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "Enable") == 0)
    {
        *pBool = pMyObject->Enable;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_SetParamBoolValue (ANSC_HANDLE hInsContext, char *ParamName, BOOL bValue)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "Enable") == 0)
    {
        pMyObject->Enable = bValue;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_GetParamIntValue (ANSC_HANDLE hInsContext, char *ParamName, int *pInt)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusAuthServerPort") == 0)
    {
        *pInt = pMyObject->RadiusAuthServerPort;
        return TRUE;
    }
   
    if (strcmp(ParamName, "RadiusAccServerPort") == 0)
    {
        *pInt = pMyObject->RadiusAccServerPort;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_SetParamIntValue (ANSC_HANDLE hInsContext, char *ParamName, int value)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusAuthServerPort") == 0)
    {
        pMyObject->RadiusAuthServerPort = value;
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAccServerPort") == 0)
    {
        pMyObject->RadiusAccServerPort = value;
        return TRUE;
    }

    return FALSE;
}

ULONG X_LGI_COM_TunneledStaticIPService_GetParamStringValue (ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "Username") == 0)
    {
        AnscCopyString(pValue, pMyObject->Username);
        return 0;
    }

    if (strcmp(ParamName, "Password") == 0)
    {
        AnscCopyString(pValue, pMyObject->Password);
        return 0;
    }

    if (strcmp(ParamName, "RadiusAuthServerIPAddr") == 0)
    {
        AnscCopyString(pValue, pMyObject->RadiusAuthServerIPAddr);
        return 0;
    }

    if (strcmp(ParamName, "RadiusAuthSecret") == 0)
    {
        AnscCopyString(pValue, "");
        return 0;
    }

    if (strcmp(ParamName, "NAS-Identifier") == 0)
    {
        AnscCopyString(pValue, pMyObject->NASIdentifier);
        return 0;
    }

    if (strcmp(ParamName, "RadiusAccServerIPAddr") == 0)
    {
        AnscCopyString(pValue, pMyObject->RadiusAccServerIPAddr);
        return 0;
    }

    if (strcmp(ParamName, "RadiusAccSecret") == 0)
    {
        AnscCopyString(pValue, "");
        return 0;
    }

    return -1;
}

BOOL X_LGI_COM_TunneledStaticIPService_SetParamStringValue (ANSC_HANDLE hInsContext, char *ParamName, char *strValue)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "Username") == 0)
    {
        AnscCopyString(pMyObject->Username, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "Password") == 0)
    {
        AnscCopyString(pMyObject->Password, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAuthServerIPAddr") == 0)
    {
        AnscCopyString(pMyObject->RadiusAuthServerIPAddr, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAuthSecret") == 0)
    {
        AnscCopyString(pMyObject->RadiusAuthSecret, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "NAS-Identifier") == 0)
    {
        AnscCopyString(pMyObject->NASIdentifier, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAccServerIPAddr") == 0)
    {
        AnscCopyString(pMyObject->RadiusAccServerIPAddr, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAccSecret") == 0)
    {
        AnscCopyString(pMyObject->RadiusAccSecret, strValue);
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_GetParamUlongValue (ANSC_HANDLE hInsContext, char *ParamName, ULONG *puLong)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusInterface") == 0)
    {
        *puLong = (ULONG) pMyObject->RadiusInterface;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_SetParamUlongValue (ANSC_HANDLE hInsContext, char *ParamName, ULONG uValuepUlong)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusInterface") == 0)
    {
        pMyObject->RadiusInterface = (int) uValuepUlong;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_Validate (ANSC_HANDLE hInsContext, char *pReturnParamName, ULONG *puLength)
{
    return TRUE;
}

ULONG X_LGI_COM_TunneledStaticIPService_Commit (ANSC_HANDLE hInsContext)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    CosaDmlTunneledStaticIPSetEnable(NULL, pMyObject->Enable);
    CosaDmlTunneledStaticIPSetUsername(NULL, pMyObject->Username);
    CosaDmlTunneledStaticIPSetPassword(NULL, pMyObject->Password);
    CosaDmlTunneledStaticIPSetRadiusInterface(NULL, pMyObject->RadiusInterface);
    CosaDmlTunneledStaticIPSetRadiusAuthServerIPAddr(NULL, pMyObject->RadiusAuthServerIPAddr);
    CosaDmlTunneledStaticIPSetRadiusAuthSecret(NULL, pMyObject->RadiusAuthSecret);
    CosaDmlTunneledStaticIPSetRadiusAuthServerPort(NULL, pMyObject->RadiusAuthServerPort);
    CosaDmlTunneledStaticIPSetNASIdentifier(NULL, pMyObject->NASIdentifier);
    CosaDmlTunneledStaticIPSetRadiusAccServerIPAddr(NULL, pMyObject->RadiusAccServerIPAddr);
    CosaDmlTunneledStaticIPSetRadiusAccSecret(NULL, pMyObject->RadiusAccSecret);
    CosaDmlTunneledStaticIPSetRadiusAccServerPort(NULL, pMyObject->RadiusAccServerPort);

    return 0;
}

ULONG X_LGI_COM_TunneledStaticIPService_Rollback (ANSC_HANDLE hInsContext)
{
    return 0;
}
