/*********************************************************************
 * Copyright 2017-2019 ARRIS Enterprises, LLC.
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
 **********************************************************************/

#include "cosa_x_lgi_com_gateway_apis.h"
#include "cosa_x_lgi_com_gateway_dml.h"
#include "cosa_x_lgi_com_gateway_internal.h"
#include <syscfg/syscfg.h>

extern int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger);

/***********************************************************************
 *
 *  APIs for Object:
 *
 *      X_LGI-COM_Gateway.
 *
 *          *  LgiGateway_GetParamUlongValue
 *          *  LgiGateway_SetParamUlongValue
 *          *  LgiGateway_GetParamStringValue
 *          *  LgiGateway_SetParamStringValue
 *          *  LgiGateway_GetParamBoolValue
 *          *  LgiGateway_SetParamBoolValue
 *          *  LgiGateway_Validate
 *          *  LgiGateway_Commit
 *          *  LgiGateway_Rollback
 *
 ************************************************************************/

BOOL
LgiGateway_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "IPv6LANMode") == 0)
    {
        CosaDmlLgiGwGetIpv6LanMode(NULL, puLong);
        return TRUE;
    }

    return FALSE;
}

BOOL
LgiGateway_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    )
{
    PCOSA_DATAMODEL_LGI_IPV6LAN_MODE  pMyObject = (PCOSA_DATAMODEL_LGI_IPV6LAN_MODE)g_pCosaBEManager->hLgiIPv6LANMode;
    
    if (strcmp(ParamName, "IPv6LANMode") == 0)
    {
        pMyObject->ipv6LanMode = uValuepUlong;
        return TRUE;
    }

    return FALSE;
}

ULONG
LgiGateway_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    return -1;
}

BOOL
LgiGateway_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
    PCOSA_DATAMODEL_LGI_IPV6LAN_MODE  pMyObject = (PCOSA_DATAMODEL_LGI_IPV6LAN_MODE)g_pCosaBEManager->hLgiIPv6LANMode;

    /* check the parameter name and return the corresponding value */
    return FALSE;
}

BOOL
LgiGateway_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    return FALSE;
}

BOOL
LgiGateway_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_LGI_IPV6LAN_MODE  pMyObject = (PCOSA_DATAMODEL_LGI_IPV6LAN_MODE)g_pCosaBEManager->hLgiIPv6LANMode;

    /* check the parameter name and return the corresponding value */
    return FALSE;
}

BOOL
LgiGateway_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
  )
{
    PCOSA_DATAMODEL_LGI_IPV6LAN_MODE  pMyObject = (PCOSA_DATAMODEL_LGI_IPV6LAN_MODE)g_pCosaBEManager->hLgiIPv6LANMode;
    char buf[64];
    return TRUE;
}

ULONG
LgiGateway_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_IPV6LAN_MODE  pMyObject = (PCOSA_DATAMODEL_LGI_IPV6LAN_MODE)g_pCosaBEManager->hLgiIPv6LANMode;
    CosaDmlLgiGwSetIpv6LanMode(NULL, pMyObject->ipv6LanMode);
    syscfg_commit();
    system("/bin/sh /etc/utopia/service.d/set_resolv_conf.sh");
    CosaDmlDHCPv6sTriggerRestart(TRUE);
    return 0;
}

ULONG
LgiGateway_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_IPV6LAN_MODE  pMyObject = (PCOSA_DATAMODEL_LGI_IPV6LAN_MODE)g_pCosaBEManager->hLgiIPv6LANMode;
    return 0;
}
