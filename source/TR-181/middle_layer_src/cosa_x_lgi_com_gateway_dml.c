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
    /* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "IPv6LANMode", TRUE))
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
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    
    if( AnscEqualString(ParamName, "IPv6LANMode", TRUE))
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
    ULONG retVal = -1;
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "DNS_IPv4Preferred", TRUE))
    {
        retVal = (ULONG) CosaDmlLgiGwGetDnsIpv4Preferred(pValue, pUlSize);
    }
    else if( AnscEqualString(ParamName, "DNS_IPv4Alternate", TRUE))
    {
        retVal =  (ULONG) CosaDmlLgiGwGetDnsIpv4Alternate(pValue, pUlSize);
    }
    else if( AnscEqualString(ParamName, "DNS_IPv6Preferred", TRUE))
    {
        retVal =  (ULONG) CosaDmlLgiGwGetDnsIpv6Preferred(pValue, pUlSize);
    }
    else if( AnscEqualString(ParamName, "DNS_IPv6Alternate", TRUE))
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
    if( AnscEqualString(ParamName, "DNS_IPv4Preferred", TRUE))
    {
        AnscCopyString(pMyObject->dns_ipv4_preferred, strValue);
        return TRUE;
    }
    if( AnscEqualString(ParamName, "DNS_IPv4Alternate", TRUE))
    {
        AnscCopyString(pMyObject->dns_ipv4_alternate, strValue);
        return TRUE;
    }
    if( AnscEqualString(ParamName, "DNS_IPv6Preferred", TRUE))
    {
        AnscCopyString(pMyObject->dns_ipv6_preferred, strValue);
        return TRUE;
    }
    if( AnscEqualString(ParamName, "DNS_IPv6Alternate", TRUE))
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
    if( AnscEqualString(ParamName, "DNS_Override", TRUE))
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
    if( AnscEqualString(ParamName, "DNS_Override", TRUE))
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
    return TRUE;
}

ULONG
LgiGateway_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ULONG ret;
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    CosaDmlLgiGwSetIpv6LanMode(NULL, pMyObject->ipv6LanMode);

    if (syscfg_init() == 0)
    {
        syscfg_set(NULL, "dns_ipv4_preferred", pMyObject->dns_ipv4_preferred);
        syscfg_set(NULL, "dns_ipv4_alternate", pMyObject->dns_ipv4_alternate);
        syscfg_set(NULL, "dns_ipv6_preferred", pMyObject->dns_ipv6_preferred);
        syscfg_set(NULL, "dns_ipv6_alternate", pMyObject->dns_ipv6_alternate);
    }

    ret = (ULONG) CosaDmlLgiGwSetDnsOverride(pMyObject->dns_override);
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