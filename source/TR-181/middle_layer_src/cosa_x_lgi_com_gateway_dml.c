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
    BOOL retValue = FALSE;
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "IPv6LANMode") == 0)
    {
        CosaDmlLgiGwGetIpv6LanMode(NULL, puLong);
        retValue = TRUE;
    }
    else if (strcmp(ParamName, "ErouterModeControl") == 0){
        if (CosaDml_Gateway_GetErouterInitMode(&pMyObject->ErouterInitMode) == ANSC_STATUS_SUCCESS)
        {
            *puLong = pMyObject->ErouterInitMode;
            retValue = TRUE;
        }
    }
    else if (strcmp(ParamName, "IPv6LeaseTimeRemaining") == 0)
    {
        CosaDml_Gateway_GetIPv6LeaseTimeRemaining(puLong);
        retValue = TRUE;
    }

    return retValue;
}

BOOL
LgiGateway_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    BOOL retValue = FALSE;
    
    if (strcmp(ParamName, "IPv6LANMode") == 0)
    {
        pMyObject->ipv6LanMode = uValuepUlong;
        retValue = TRUE;
    }
    else if (strcmp(ParamName, "ErouterModeControl") == 0){
        pMyObject->ErouterInitMode = uValuepUlong;
        retValue = TRUE;
    }

    return retValue;
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
    ULONG retVal = -1;
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "DNS_IPv4Preferred") == 0)
    {
        retVal = (ULONG) CosaDmlLgiGwGetDnsIpv4Preferred(pValue, pUlSize);
    }
    else if (strcmp(ParamName, "DNS_IPv4Alternate") == 0)
    {
        retVal =  (ULONG) CosaDmlLgiGwGetDnsIpv4Alternate(pValue, pUlSize);
    }
    else if (strcmp(ParamName, "DNS_IPv6Preferred") == 0)
    {
        retVal =  (ULONG) CosaDmlLgiGwGetDnsIpv6Preferred(pValue, pUlSize);
    }
    else if (strcmp(ParamName, "DNS_IPv6Alternate") == 0)
    {
        retVal =  (ULONG) CosaDmlLgiGwGetDnsIpv6Alternate(pValue, pUlSize);
    }

    return retVal;
}

BOOL
LgiGateway_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "DNS_IPv4Preferred") == 0)
    {
        AnscCopyString(pMyObject->dns_ipv4_preferred, strValue);
        return TRUE;
    }
    if (strcmp(ParamName, "DNS_IPv4Alternate") == 0)
    {
        AnscCopyString(pMyObject->dns_ipv4_alternate, strValue);
        return TRUE;
    }
    if (strcmp(ParamName, "DNS_IPv6Preferred") == 0)
    {
        AnscCopyString(pMyObject->dns_ipv6_preferred, strValue);
        return TRUE;
    }
    if (strcmp(ParamName, "DNS_IPv6Alternate") == 0)
    {
        AnscCopyString(pMyObject->dns_ipv6_alternate, strValue);
        return TRUE;
    }
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
    if (strcmp(ParamName, "DNS_Override") == 0)
    {
        CosaDmlLgiGwGetDnsOverride(pBool);
        return TRUE;
    }
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
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "DNS_Override") == 0)
    {
        pMyObject->dns_override = bValue;
        return TRUE;
    }
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
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    char buf[64];
    /* Validate DNS IPv4 address */
    if(inet_pton(AF_INET, pMyObject->dns_ipv4_preferred, buf) < 1)
    {
        CcspTraceWarning(("DNS_IPv4Preferred validation failed"));
        AnscCopyString(pReturnParamName,"DNS_IPv4Preferred");
        return FALSE;
    }
    if(inet_pton(AF_INET, pMyObject->dns_ipv4_alternate, buf) < 1)
    {
        CcspTraceWarning(("DNS_IPv4Preferred validation failed"));
        AnscCopyString(pReturnParamName,"DNS_IPv4Alternate");
        return FALSE;
    }
    /* Validate DNS IPv6 address */
    if(inet_pton(AF_INET6, pMyObject->dns_ipv6_preferred, buf) < 1)
    {
        CcspTraceWarning(("DNS_IPv6Preferred validation failed"));
        AnscCopyString(pReturnParamName,"DNS_IPv6Preferred");
        return FALSE;
    }
    if(inet_pton(AF_INET6, pMyObject->dns_ipv6_alternate, buf) < 1)
    {
        CcspTraceWarning(("DNS_IPv6Preferred validation failed"));
        AnscCopyString(pReturnParamName,"DNS_IPv6Alternate");
        return FALSE;
    }
    if (pMyObject->ErouterInitMode && ((pMyObject->ErouterInitMode < 1) || (pMyObject->ErouterInitMode > 5)))
    {
        CcspTraceWarning(("ErouterModeControl validation failed"));
        AnscCopyString(pReturnParamName,"ErouterModeControl");
        return FALSE;
    }

    return TRUE;
}

ULONG
LgiGateway_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ULONG ret = ANSC_STATUS_FAILURE;
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    CosaDmlLgiGwSetIpv6LanMode(NULL, pMyObject->ipv6LanMode);

    syscfg_set(NULL, "dns_ipv4_preferred", pMyObject->dns_ipv4_preferred);
    syscfg_set(NULL, "dns_ipv4_alternate", pMyObject->dns_ipv4_alternate);
    syscfg_set(NULL, "dns_ipv6_preferred", pMyObject->dns_ipv6_preferred);
    syscfg_set(NULL, "dns_ipv6_alternate", pMyObject->dns_ipv6_alternate);

    ret = (ULONG) CosaDmlLgiGwSetDnsOverride(pMyObject->dns_override);

    ret = ret | CosaDml_Gateway_SetErouterInitMode(pMyObject->ErouterInitMode);

    return ret;
}

ULONG
LgiGateway_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    ULONG size;
    size = sizeof(pMyObject->dns_ipv4_preferred);
    CosaDmlLgiGwGetDnsIpv4Preferred(pMyObject->dns_ipv4_preferred, &size);
    size = sizeof(pMyObject->dns_ipv4_alternate);
    CosaDmlLgiGwGetDnsIpv4Alternate(pMyObject->dns_ipv4_alternate, &size);
    size = sizeof(pMyObject->dns_ipv6_preferred);
    CosaDmlLgiGwGetDnsIpv6Preferred(pMyObject->dns_ipv6_preferred, &size);
    size = sizeof(pMyObject->dns_ipv6_alternate);
    CosaDmlLgiGwGetDnsIpv6Alternate(pMyObject->dns_ipv6_alternate, &size);
    return 0;
}
