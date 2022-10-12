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
        *pBool = pMyObject->Cfg.Enable;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_SetParamBoolValue (ANSC_HANDLE hInsContext, char *ParamName, BOOL bValue)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "Enable") == 0)
    {
        pMyObject->Cfg.Enable = bValue;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_GetParamIntValue (ANSC_HANDLE hInsContext, char *ParamName, int *pInt)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusAuthServerPort") == 0)
    {
        *pInt = pMyObject->Cfg.RadiusAuthServerPort;
        return TRUE;
    }
   
    if (strcmp(ParamName, "RadiusAccServerPort") == 0)
    {
        *pInt = pMyObject->Cfg.RadiusAccServerPort;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_SetParamIntValue (ANSC_HANDLE hInsContext, char *ParamName, int value)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusAuthServerPort") == 0)
    {
        pMyObject->Cfg.RadiusAuthServerPort = value;
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAccServerPort") == 0)
    {
        pMyObject->Cfg.RadiusAccServerPort = value;
        return TRUE;
    }

    return FALSE;
}

ULONG X_LGI_COM_TunneledStaticIPService_GetParamStringValue (ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "Username") == 0)
    {
        AnscCopyString(pValue, pMyObject->Cfg.Username);
        return 0;
    }

    if (strcmp(ParamName, "Password") == 0)
    {
        AnscCopyString(pValue, pMyObject->Cfg.Password);
        return 0;
    }

    if (strcmp(ParamName, "RadiusAuthServerIPAddr") == 0)
    {
        AnscCopyString(pValue, pMyObject->Cfg.RadiusAuthServerIPAddr);
        return 0;
    }

    if (strcmp(ParamName, "RadiusAuthSecret") == 0)
    {
        AnscCopyString(pValue, "");
        return 0;
    }

    if (strcmp(ParamName, "NAS-Identifier") == 0)
    {
        AnscCopyString(pValue, pMyObject->Cfg.NASIdentifier);
        return 0;
    }

    if (strcmp(ParamName, "RadiusAccServerIPAddr") == 0)
    {
        AnscCopyString(pValue, pMyObject->Cfg.RadiusAccServerIPAddr);
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
        AnscCopyString(pMyObject->Cfg.Username, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "Password") == 0)
    {
        AnscCopyString(pMyObject->Cfg.Password, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAuthServerIPAddr") == 0)
    {
        AnscCopyString(pMyObject->Cfg.RadiusAuthServerIPAddr, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAuthSecret") == 0)
    {
        AnscCopyString(pMyObject->Cfg.RadiusAuthSecret, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "NAS-Identifier") == 0)
    {
        AnscCopyString(pMyObject->Cfg.NASIdentifier, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAccServerIPAddr") == 0)
    {
        AnscCopyString(pMyObject->Cfg.RadiusAccServerIPAddr, strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "RadiusAccSecret") == 0)
    {
        AnscCopyString(pMyObject->Cfg.RadiusAccSecret, strValue);
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_GetParamUlongValue (ANSC_HANDLE hInsContext, char *ParamName, ULONG *puLong)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusInterface") == 0)
    {
        *puLong = (ULONG) pMyObject->Cfg.RadiusInterface;
        return TRUE;
    }

    return FALSE;
}

BOOL X_LGI_COM_TunneledStaticIPService_SetParamUlongValue (ANSC_HANDLE hInsContext, char *ParamName, ULONG uValuepUlong)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) g_pCosaBEManager->hLgiTunneledStaticIP;

    if (strcmp(ParamName, "RadiusInterface") == 0)
    {
        pMyObject->Cfg.RadiusInterface = (int) uValuepUlong;
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
    BOOL bConfigChanged = FALSE;

    if (pMyObject->Cfg.Enable != pMyObject->OldCfg.Enable)
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetEnable(NULL, pMyObject->Cfg.Enable);
    }

    if (strcmp(pMyObject->Cfg.Username, pMyObject->OldCfg.Username))
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetUsername(NULL, pMyObject->Cfg.Username);
    }

    if (strcmp(pMyObject->Cfg.Password, pMyObject->OldCfg.Password))
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetPassword(NULL, pMyObject->Cfg.Password);
    }

    if (pMyObject->Cfg.RadiusInterface != pMyObject->OldCfg.RadiusInterface)
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetRadiusInterface(NULL, pMyObject->Cfg.RadiusInterface);
    }

    if (strcmp(pMyObject->Cfg.RadiusAuthServerIPAddr, pMyObject->OldCfg.RadiusAuthServerIPAddr))
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetRadiusAuthServerIPAddr(NULL, pMyObject->Cfg.RadiusAuthServerIPAddr);
    }

    if (strcmp(pMyObject->Cfg.RadiusAuthSecret, pMyObject->OldCfg.RadiusAuthSecret))
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetRadiusAuthSecret(NULL, pMyObject->Cfg.RadiusAuthSecret);
    }

    if (pMyObject->Cfg.RadiusAuthServerPort != pMyObject->OldCfg.RadiusAuthServerPort)
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetRadiusAuthServerPort(NULL, pMyObject->Cfg.RadiusAuthServerPort);
    }

    if (strcmp(pMyObject->Cfg.NASIdentifier, pMyObject->OldCfg.NASIdentifier))
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetNASIdentifier(NULL, pMyObject->Cfg.NASIdentifier);
    }

    if (strcmp(pMyObject->Cfg.RadiusAccServerIPAddr, pMyObject->OldCfg.RadiusAccServerIPAddr))
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetRadiusAccServerIPAddr(NULL, pMyObject->Cfg.RadiusAccServerIPAddr);
    }

    if (strcmp(pMyObject->Cfg.RadiusAccSecret, pMyObject->OldCfg.RadiusAccSecret))
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetRadiusAccSecret(NULL, pMyObject->Cfg.RadiusAccSecret);
    }

    if (pMyObject->Cfg.RadiusAccServerPort != pMyObject->OldCfg.RadiusAccServerPort)
    {
        bConfigChanged = TRUE;
        CosaDmlTunneledStaticIPSetRadiusAccServerPort(NULL, pMyObject->Cfg.RadiusAccServerPort);
    }

    if (bConfigChanged)
    {
        syscfg_commit();
        pMyObject->OldCfg = pMyObject->Cfg;
        CosaDmlTunneledStaticIPRestart(g_pCosaBEManager->hLgiTunneledStaticIP);
    }

    return 0;
}

ULONG X_LGI_COM_TunneledStaticIPService_Rollback (ANSC_HANDLE hInsContext)
{
    return 0;
}
