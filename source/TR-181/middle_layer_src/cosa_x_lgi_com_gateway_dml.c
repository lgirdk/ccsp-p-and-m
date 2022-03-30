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
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;
    
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "IPv6LANMode") == 0)
    {
        CosaDmlLgiGwGetIpv6LanMode(NULL, puLong);
        return TRUE;
    }

    if (strcmp(ParamName, "ErouterModeControl") == 0)
    {
        if (CosaDml_Gateway_GetErouterInitMode(&pMyObject->ErouterInitMode) == ANSC_STATUS_SUCCESS)
        {
            *puLong = pMyObject->ErouterInitMode;
            return TRUE;
        }

        return FALSE;
    }

    if (strcmp(ParamName, "IPv6LeaseTimeRemaining") == 0)
    {
        CosaDml_Gateway_GetIPv6LeaseTimeRemaining(puLong);
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
    
    if (strcmp(ParamName, "IPv6LANMode") == 0)
    {
        pMyObject->ipv6LanMode = uValuepUlong;
        return TRUE;
    }

    if (strcmp(ParamName, "ErouterModeControl") == 0)
    {
        pMyObject->ErouterInitMode = uValuepUlong;
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
    char customer_db_dns_enabled[6];

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "DNS_Override") == 0)
    {
        if (syscfg_get(NULL, "dns_override", customer_db_dns_enabled, sizeof(customer_db_dns_enabled)) == 0)
        {
            pMyObject->dns_override = bValue;
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL isValidIPv4DNS (char *dnsAddr)
{
    ANSC_IPV4_ADDRESS dnsIPBuf;
    char IPAddr[16] = {0};
    ULONG ulSize = sizeof(IPAddr);

    // Allow default empty string
    if (strlen(dnsAddr) == 0)
    {
        return TRUE;
    }

    if(!inet_pton(AF_INET, dnsAddr, &dnsIPBuf))
        return FALSE;

    if ((dnsIPBuf.Dot[0] == 127) ||    /* loopback */
        (dnsIPBuf.Dot[0] >= 224) ||    /* multicast */
        (dnsIPBuf.Dot[0] == 169 && dnsIPBuf.Dot[1] == 254) ||    /* link local */
        (dnsIPBuf.Dot[0] == 10) ||    /* private */
        (dnsIPBuf.Dot[0] == 172 && (dnsIPBuf.Dot[1] >> 4) == 1) ||    /* private */
        (dnsIPBuf.Dot[0] == 192 && dnsIPBuf.Dot[1] == 168) ||    /* private */
        (dnsIPBuf.Dot[0] == 0 && dnsIPBuf.Dot[1] == 0 && dnsIPBuf.Dot[2] == 0 && dnsIPBuf.Dot[3] == 0))
        return FALSE;

    if (CosaDmlDiGetMTAIPAddress(NULL, IPAddr, &ulSize) && strncmp(dnsAddr, IPAddr, ulSize) == 0)
        return FALSE;

    IPAddr[0] = 0;
    ulSize = sizeof(IPAddr);
    if (CosaDmlDiGetCMIPv4Address(NULL, IPAddr, &ulSize) && strncmp(dnsAddr, IPAddr, ulSize) == 0)
        return FALSE;

    IPAddr[0] = 0;
    ulSize = sizeof(IPAddr);
    if (CosaDmlDiGetRouterIPAddress(NULL, IPAddr, &ulSize) && strncmp(dnsAddr, IPAddr, ulSize) == 0)
        return FALSE;

    IPAddr[0] = 0;
    ulSize = sizeof(IPAddr);
    if (LanBlockedSubnetTable_GetGuestNetworkIP(IPAddr) && strncmp(dnsAddr, IPAddr, ulSize) == 0)
        return FALSE;

    return TRUE;
}

static BOOL isValidIPv6DNS (char *dnsAddr)
{
    unsigned char buf[sizeof(struct in6_addr)];
    unsigned char v6_prefix[sizeof(struct in6_addr)];
    unsigned char v6_addr[sizeof(struct in6_addr)];
    char erouter_mode[16];
    char wan_prefix[64];
    char wan_ipaddr[64];
    int mode;

    // Allow default empty string
    if (strlen(dnsAddr) == 0)
    {
        return TRUE;
    }

    if(!inet_pton(AF_INET6, dnsAddr, buf))
        return FALSE;

    syscfg_get(NULL, "last_erouter_mode", erouter_mode, sizeof(erouter_mode));
    commonSyseventGet("wan6_prefix", wan_prefix, sizeof(wan_prefix));
    commonSyseventGet("wan6_ipaddr", wan_ipaddr, sizeof(wan_ipaddr));
    mode = atoi(erouter_mode);

    if (mode == 2 || mode == 3)    //erouter is in IPv6 or dual mode
    {
        inet_pton(AF_INET6, wan_prefix, v6_prefix);
        inet_pton(AF_INET6, wan_ipaddr, v6_addr);

        /* check for wan IPv6 address and prefix */
        if (!memcmp(dnsAddr, v6_addr, sizeof(struct in6_addr)) ||
            !memcmp(dnsAddr, v6_prefix, sizeof(struct in6_addr)/2))
            return FALSE;
    }

    /* check for Multicast and Link-local unicast address */
    if ((buf[0] == 0xFF && buf[1] == 0x00) ||
        (buf[0] == 0xFE && buf[1] == 0x80))
        return FALSE;

    /* check for loopback address */
    if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00 && buf[4] == 0x00 &&
        buf[5] == 0x00 && buf[6] == 0x00 && buf[7] == 0x00 && buf[8] == 0x00 && buf[9] == 0x00 &&
        buf[10] == 0x00 && buf[11] == 0x00 && buf[12] == 0x00 && buf[13] == 0x00 && buf[14] == 0x00 &&
        (buf[15] == 0x00 || buf[15] == 0x01))
        return FALSE;

    return TRUE;
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

    if (pMyObject->dns_override) {
        /* Validate DNS IPv4 address */
        if (!isValidIPv4DNS(pMyObject->dns_ipv4_preferred))
        {
            CcspTraceWarning(("DNS_IPv4Preferred validation failed"));
            AnscCopyString(pReturnParamName,"DNS_IPv4Preferred");
            return FALSE;
        }
        if (!isValidIPv4DNS(pMyObject->dns_ipv4_alternate))
        {
            CcspTraceWarning(("DNS_IPv4Alternate validation failed"));
            AnscCopyString(pReturnParamName,"DNS_IPv4Alternate");
            return FALSE;
        }

        /* Validate DNS IPv6 address */
        if (!isValidIPv6DNS(pMyObject->dns_ipv6_preferred))
        {
            CcspTraceWarning(("DNS_IPv6Preferred validation failed"));
            AnscCopyString(pReturnParamName,"DNS_IPv6Preferred");
            return FALSE;
        }
        if (!isValidIPv6DNS(pMyObject->dns_ipv6_alternate))
        {
            CcspTraceWarning(("DNS_IPv6Alternate validation failed"));
            AnscCopyString(pReturnParamName,"DNS_IPv6Alternate");
            return FALSE;
        }
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
    ret |= (ULONG) CosaDml_Gateway_SetErouterInitMode(pMyObject->ErouterInitMode);

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
