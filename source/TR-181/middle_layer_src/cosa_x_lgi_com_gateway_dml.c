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
#include "cosa_deviceinfo_apis_custom.h"
#include "cosa_dhcpv4_apis.h"
#include "cosa_drg_common.h"
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
    char buf[6];
    if (strcmp(ParamName, "DNS_Override") == 0)
    {
        CosaDmlLgiGwGetDnsOverride(pBool);
        return TRUE;
    }
    if (strcmp(ParamName, "DNS_ForceStatic") == 0)
    {
        syscfg_get(NULL, "dns_forcestatic", buf, sizeof(buf));
        *pBool = (strcmp (buf, "true") == 0) ? TRUE : FALSE;
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
            pMyObject->isDnsUpdated = TRUE;
            return TRUE;
        }
    }
    if (strcmp(ParamName, "DNS_ForceStatic") == 0)
    {
        pMyObject->dns_forcestatic = bValue;
        syscfg_set_commit(NULL, "dns_forcestatic", (bValue == true) ? "true" : "false");
        commonSyseventSet("firewall-restart", "");
        return TRUE;
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

    if (pMyObject->isDnsUpdated)
    {
        ret = (ULONG) CosaDmlLgiGwSetDnsOverride(pMyObject->dns_override);
        pMyObject->isDnsUpdated = FALSE;
    }

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

BOOL
DNS_Rebind_Protection_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;

    if (strcmp(ParamName, "Enable") == 0)
    {
        *pBool = pMyObject->dns_rebind_protection_enable;
        return TRUE;
    }

    return FALSE;
}

BOOL
DNS_Rebind_Protection_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)g_pCosaBEManager->hLgiGateway;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if (bValue != pMyObject->dns_rebind_protection_enable)
        {
            pMyObject->dns_rebind_protection_enable = bValue;
            CosaDmlDNS_Rebind_SetConf(pMyObject->dns_rebind_protection_enable);
        }
        return TRUE;
    }

    return FALSE;
}

ULONG
Whitelist_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    COSA_DATAMODEL_LGI_GATEWAY *pMyObject = (COSA_DATAMODEL_LGI_GATEWAY*)g_pCosaBEManager->hLgiGateway;
    return AnscSListQueryDepth(&pMyObject->DNSWhitelistList);
}

ANSC_HANDLE
Whitelist_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    COSA_DATAMODEL_LGI_GATEWAY *pMyObject = (COSA_DATAMODEL_LGI_GATEWAY*)g_pCosaBEManager->hLgiGateway;
    PCOSA_CONTEXT_LINK_OBJECT pLinkObj = NULL;
    PSINGLE_LINK_ENTRY pSLinkEntry = NULL;

    pSLinkEntry = AnscQueueGetEntryByIndex((ANSC_HANDLE)&pMyObject->DNSWhitelistList, nIndex);

    if (pSLinkEntry)
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pLinkObj->InstanceNumber;
    }

    return pLinkObj;
}

ANSC_HANDLE
Whitelist_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    COSA_DATAMODEL_LGI_GATEWAY *pMyObject = (COSA_DATAMODEL_LGI_GATEWAY*)g_pCosaBEManager->hLgiGateway;
    PCOSA_CONTEXT_LINK_OBJECT pLinkObj = NULL;
    COSA_DML_DNS_WHITELIST *pWhitelistEntry = NULL;

    pLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
    if (!pLinkObj)
        return NULL;

    pWhitelistEntry = AnscAllocateMemory(sizeof(COSA_DML_DNS_WHITELIST));
    if (!pWhitelistEntry)
    {
        AnscFreeMemory(pLinkObj);
        return NULL;
    }

     /* now we have this link content */
    pLinkObj->InstanceNumber = pMyObject->DNSWhitelistNextInsNum;
    pWhitelistEntry->InstanceNumber = pMyObject->DNSWhitelistNextInsNum;
    pMyObject->DNSWhitelistNextInsNum++;
    if (pMyObject->DNSWhitelistNextInsNum == 0)
        pMyObject->DNSWhitelistNextInsNum = 1;

    pLinkObj->hContext = (ANSC_HANDLE)pWhitelistEntry;
    pLinkObj->hParentTable = NULL;
    pLinkObj->bNew = TRUE;

    CosaSListPushEntryByInsNum((PSLIST_HEADER)&pMyObject->DNSWhitelistList, pLinkObj);
    CosaDNS_Whitelist_UrlAddInfo((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pLinkObj);

    *pInsNumber = pLinkObj->InstanceNumber;

    return pLinkObj;
}

ULONG
Whitelist_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    COSA_DATAMODEL_LGI_GATEWAY *pMyObject = (COSA_DATAMODEL_LGI_GATEWAY*)g_pCosaBEManager->hLgiGateway;
    PCOSA_CONTEXT_LINK_OBJECT pLinkObj = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    COSA_DML_DNS_WHITELIST *pWhitelistEntry = (COSA_DML_DNS_WHITELIST*)pLinkObj->hContext;

    if (pLinkObj->bNew)
    {
        /* Set bNew to FALSE to indicate this node is not going to save to SysRegistry */
        pLinkObj->bNew = FALSE;
        returnStatus = CosaDNS_Whitelist_UrlDelInfo((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pLinkObj);
    }
    else
    {
        returnStatus = CosaDmlDNS_Whitelist_DelEntry(pLinkObj->InstanceNumber, pMyObject->dns_rebind_protection_enable);
    }

    if (returnStatus == ANSC_STATUS_SUCCESS)
    {
        AnscSListPopEntryByLink((PSLIST_HEADER)&pMyObject->DNSWhitelistList, &pLinkObj->Linkage);
        AnscFreeMemory(pWhitelistEntry);
        AnscFreeMemory(pLinkObj);
    }

    return returnStatus;
}

ULONG
Whitelist_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT pLinkObj = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_DNS_WHITELIST *pWhitelistEntry = (COSA_DML_DNS_WHITELIST*)pLinkObj->hContext;

    if (strcmp(ParamName, "Url") == 0)
    {
        AnscCopyString(pValue, pWhitelistEntry->Url);
        return 0;
    }

    if (strcmp(ParamName, "Description") == 0)
    {
        AnscCopyString(pValue, pWhitelistEntry->Description);
        return 0;
    }

    return -1;
}

BOOL
Whitelist_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT pLinkObj = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_DNS_WHITELIST *pWhitelistEntry = (COSA_DML_DNS_WHITELIST*)pLinkObj->hContext;

    if (strcmp(ParamName, "Url") == 0)
    {
        snprintf(pWhitelistEntry->Url, sizeof(pWhitelistEntry->Url), "%s", strValue);
        return TRUE;
    }

    if (strcmp(ParamName, "Description") == 0)
    {
        snprintf(pWhitelistEntry->Description, sizeof(pWhitelistEntry->Description), "%s", strValue);
        return TRUE;
    }

    return FALSE;
}

BOOL
Whitelist_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

ULONG
Whitelist_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT pLinkObj = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_DNS_WHITELIST *pWhitelistEntry = (COSA_DML_DNS_WHITELIST*)pLinkObj->hContext;
    COSA_DATAMODEL_LGI_GATEWAY *pLGIGateway = (COSA_DATAMODEL_LGI_GATEWAY*)g_pCosaBEManager->hLgiGateway;

    if (pLinkObj->bNew)
    {
        if (CosaDmlDNS_Whitelist_AddEntry(pWhitelistEntry, pLGIGateway->dns_rebind_protection_enable) != ANSC_STATUS_SUCCESS)
            return -1;
        CosaDNS_Whitelist_UrlDelInfo((ANSC_HANDLE)pLGIGateway, (ANSC_HANDLE)pLinkObj);
        pLinkObj->bNew = FALSE;
    }
    else
    {
        if (CosaDmlDNS_Whitelist_SetConf(pWhitelistEntry->InstanceNumber, pWhitelistEntry, pLGIGateway->dns_rebind_protection_enable) != ANSC_STATUS_SUCCESS)
        {
            CosaDmlDNS_Whitelist_GetConf(pWhitelistEntry->InstanceNumber, pWhitelistEntry);
            return -1;
        }
        else
            CosaDmlDNS_Whitelist_GetConf(pWhitelistEntry->InstanceNumber, pWhitelistEntry);
    }

    return 0;
}

ULONG
Whitelist_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT pLinkObj = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_DNS_WHITELIST *pWhitelistEntry = (COSA_DML_DNS_WHITELIST*)pLinkObj->hContext;

    if (CosaDmlDNS_Whitelist_GetConf(pWhitelistEntry->InstanceNumber, pWhitelistEntry) != ANSC_STATUS_SUCCESS)
        return -1;

    return 0;
}
