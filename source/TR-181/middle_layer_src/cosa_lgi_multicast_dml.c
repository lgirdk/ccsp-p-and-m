/*********************************************************************************
* Copyright 2019 Liberty Global B.V.
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

#include <ctype.h>
#include "ansc_platform.h"
#include "cosa_lgi_multicast_apis.h"
#include "cosa_lgi_multicast_dml.h"
#include "cosa_lgi_multicast_internal.h"
#include "ccsp/platform_hal.h"
#include "cosa_drg_common.h"
#include <syscfg/syscfg.h>

BOOL LgiMulticast_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pValue )
{
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) g_pCosaBEManager->hLgiMulticast;

    if (strcmp (ParamName, "Enable") == 0)
    {
        CosaDmlMulticastGetEnable(NULL, &pMyObject->Cfg.bEnable);
        *pValue = pMyObject->Cfg.bEnable;
        return TRUE;
    }

    if (strcmp(ParamName, "SnoopingEnable") == 0)
    {
        CosaDmlMulticastGetSnoopingEnable(NULL, &pMyObject->Cfg.bSnoopingEnable);
        *pValue = pMyObject->Cfg.bSnoopingEnable;
        return TRUE;
    }

    if (strcmp(ParamName, "IGMPv3ProxyEnable") == 0)
    {
        CosaDmlMulticastGetIGMPv3ProxyEnable(NULL, &pMyObject->Cfg.bIGMPv3ProxyEnable);
        *pValue = pMyObject->Cfg.bIGMPv3ProxyEnable;
        return TRUE;
    }

    return FALSE;
}

BOOL LgiMulticast_SetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue )
{
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) g_pCosaBEManager->hLgiMulticast;

    if (strcmp(ParamName, "IGMPv3ProxyEnable") == 0)
    {
        pMyObject->Cfg.bIGMPv3ProxyEnable = bValue;
        return TRUE;
    }

    return FALSE;
}

BOOL LgiMulticast_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength )
{
    return TRUE;
}

ULONG LgiMulticast_Commit ( ANSC_HANDLE hInsContext )
{
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) g_pCosaBEManager->hLgiMulticast;

    if (IS_PARAM_CHANGED(pMyObject, bIGMPv3ProxyEnable))
    {
        CosaDmlMulticastSetIGMPv3ProxyEnable(NULL, pMyObject->Cfg.bIGMPv3ProxyEnable);
    }
    if (IS_IPV4PROXY_RESTART_NEEDED(pMyObject))
    {
        system("/etc/utopia/service.d/service_mcastproxy.sh mcastproxy-restart &");
    }
    /* commit the changes and cache the new config */
    if (IS_CFG_CHANGED(pMyObject))
    {
        syscfg_commit();
        CACHE_CFG(pMyObject);
    }

    return 0;
}

ULONG LgiMulticast_Rollback ( ANSC_HANDLE hInsContext )
{
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) g_pCosaBEManager->hLgiMulticast;

    CosaDmlMulticastGetEnable(NULL, &pMyObject->Cfg.bEnable);
    CosaDmlMulticastGetSnoopingEnable(NULL, &pMyObject->Cfg.bSnoopingEnable);
    CosaDmlMulticastGetIGMPv3ProxyEnable(NULL, &pMyObject->Cfg.bIGMPv3ProxyEnable);

    return 0;
}
