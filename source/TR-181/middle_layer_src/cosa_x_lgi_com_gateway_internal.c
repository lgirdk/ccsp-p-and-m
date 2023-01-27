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
#include "cosa_x_lgi_com_gateway_internal.h"

extern void * g_pDslhDmlAgent;

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiGatewayCreate
            (
            );

    description:

        This function constructs cosa LgiGateway object and return handle.

    argument:

    return:     newly created LgiGateway object.

**********************************************************************/

ANSC_HANDLE
CosaLgiGatewayCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject    = (PCOSA_DATAMODEL_LGI_GATEWAY)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_GATEWAY));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_GATEWAY_OID;
    pMyObject->Create            = CosaLgiGatewayCreate;
    pMyObject->Remove            = CosaLgiGatewayRemove;
    pMyObject->Initialize        = CosaLgiGatewayInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiGatewayInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiGateway object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiGatewayInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_GATEWAY        pMyObject    = (PCOSA_DATAMODEL_LGI_GATEWAY)hThisObject;
    ULONG                              size;
    ULONG                              ulDNSWhitelistUrlCnt = 1;
    PSLAP_VARIABLE                     pSlapVariable    = NULL;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoDNSWhitelistUrl = NULL;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoCOSA  = NULL;
    COSA_DML_DNS_WHITELIST             *pDNSWhitelistUrl;
    PCOSA_CONTEXT_LINK_OBJECT          pDNSWhitelistUrlLinkObj = NULL;

    /* Initiation all functions */
    CosaDmlLgiGwGetIpv6LanMode(NULL, &pMyObject->ipv6LanMode);

	size = sizeof(pMyObject->dns_ipv4_preferred);
    CosaDmlLgiGwGetDnsIpv4Preferred(pMyObject->dns_ipv4_preferred, &size);
    size = sizeof(pMyObject->dns_ipv4_alternate);
    CosaDmlLgiGwGetDnsIpv4Alternate(pMyObject->dns_ipv4_alternate, &size);
    size = sizeof(pMyObject->dns_ipv6_preferred);
    CosaDmlLgiGwGetDnsIpv6Preferred(pMyObject->dns_ipv6_preferred, &size);
    size = sizeof(pMyObject->dns_ipv6_alternate);
    CosaDmlLgiGwGetDnsIpv6Alternate(pMyObject->dns_ipv6_alternate, &size);
    CosaDmlLgiGwGetDnsOverride(&(pMyObject->dns_override));
    pMyObject->isDnsUpdated = FALSE;
    pMyObject->dns_rebind_protection_enable = 0;
    CosaDmlDNS_Rebind_GetConf(&(pMyObject->dns_rebind_protection_enable));

    CosaDmlLgiGwUpdateDnsIpset(pMyObject->dns_ipv4_preferred, pMyObject->dns_ipv4_alternate,
                               pMyObject->dns_ipv6_preferred, pMyObject->dns_ipv6_alternate);

    if (pMyObject->dns_rebind_protection_enable)
    {
        system("/usr/bin/dns_filter &");
    }

    ulDNSWhitelistUrlCnt = CosaDmlDNS_Whitelist_GetNumberOfEntries();

    AnscSListInitializeHeader(&pMyObject->DNSWhitelistList);
    pMyObject->DNSWhitelistNextInsNum = 1;
    pMyObject->hIrepFolderDNSWhitelistCOSA = g_GetRegistryRootFolder(g_pDslhDmlAgent);

    pPoamIrepFoCOSA = (PPOAM_IREP_FOLDER_OBJECT)pMyObject->hIrepFolderDNSWhitelistCOSA;
    if (!pPoamIrepFoCOSA)
    {
        return ANSC_STATUS_FAILURE;
    }

    pPoamIrepFoDNSWhitelistUrl = (PPOAM_IREP_FOLDER_OBJECT)pPoamIrepFoCOSA->GetFolder((ANSC_HANDLE)pPoamIrepFoCOSA, COSA_IREP_FOLDER_NAME_DNS_WHITELIST_URL);
    if (!pPoamIrepFoDNSWhitelistUrl)
    {
        pPoamIrepFoDNSWhitelistUrl = pPoamIrepFoCOSA->AddFolder((ANSC_HANDLE)pPoamIrepFoCOSA, COSA_IREP_FOLDER_NAME_DNS_WHITELIST_URL, 0);
    }

    if (!pPoamIrepFoDNSWhitelistUrl)
        return ANSC_STATUS_FAILURE;
    else
        pMyObject->hIrepFolderDNSWhitelist = pPoamIrepFoDNSWhitelistUrl;

    pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoDNSWhitelistUrl->GetRecord((ANSC_HANDLE)pPoamIrepFoDNSWhitelistUrl, COSA_DML_RR_NAME_DNS_WHITELIST_URL_NextInsNumber, NULL);
    if (pSlapVariable)
    {
        pMyObject->DNSWhitelistNextInsNum = pSlapVariable->Variant.varUint32;
        SlapFreeVariable(pSlapVariable);
    }

    for (int ulDNSWhitelistUrlIdx = 0; ulDNSWhitelistUrlIdx < ulDNSWhitelistUrlCnt; ulDNSWhitelistUrlIdx++)
    {
        pDNSWhitelistUrl = AnscAllocateMemory(sizeof(COSA_DML_DNS_WHITELIST));
        if (!pDNSWhitelistUrl)
            return ANSC_STATUS_FAILURE;

        if (CosaDmlDNS_Whitelist_GetEntryByIndex(ulDNSWhitelistUrlIdx, pDNSWhitelistUrl) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("%s: CosaDmlDNS_Whitelist_GetEntryByIndex error\n", __FUNCTION__));
            AnscFreeMemory(pDNSWhitelistUrl);
            return ANSC_STATUS_FAILURE;
        }

        pDNSWhitelistUrlLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pDNSWhitelistUrlLinkObj)
        {
            AnscFreeMemory(pDNSWhitelistUrl);
            return ANSC_STATUS_FAILURE;
        }

        if (pDNSWhitelistUrl->InstanceNumber != 0)
        {
            if (pMyObject->DNSWhitelistNextInsNum <= pDNSWhitelistUrl->InstanceNumber)
            {
                pMyObject->DNSWhitelistNextInsNum = pDNSWhitelistUrl->InstanceNumber + 1;
                if (pMyObject->DNSWhitelistNextInsNum == 0)
                {
                    pMyObject->DNSWhitelistNextInsNum = 1;
                }
            }
        }
        else
        {
            pDNSWhitelistUrl->InstanceNumber = pMyObject->DNSWhitelistNextInsNum;
            pMyObject->DNSWhitelistNextInsNum++;
            if (pMyObject->DNSWhitelistNextInsNum == 0)
            {
                pMyObject->DNSWhitelistNextInsNum = 1;
            }
        }

        pDNSWhitelistUrlLinkObj->InstanceNumber  = pDNSWhitelistUrl->InstanceNumber;
        pDNSWhitelistUrlLinkObj->hContext        = pDNSWhitelistUrl;
        pDNSWhitelistUrlLinkObj->hParentTable    = NULL;
        pDNSWhitelistUrlLinkObj->bNew            = FALSE;

        CosaSListPushEntryByInsNum(&pMyObject->DNSWhitelistList, pDNSWhitelistUrlLinkObj);

    }
    CosaDNS_Whitelist_UrlGetInfo((ANSC_HANDLE)pMyObject);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiGatewayRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiGateway object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiGatewayRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_GATEWAY        pMyObject    = (PCOSA_DATAMODEL_LGI_GATEWAY)hThisObject;

    /* Remove necessary resources */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDNS_Whitelist_UrlGetInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function get Info for DNS Whitelist Url.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDNS_Whitelist_UrlGetInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY     pMyObject               = (PCOSA_DATAMODEL_LGI_GATEWAY)hThisObject;
    PSLIST_HEADER                   pListHead               = (PSLIST_HEADER              )&pMyObject->DNSWhitelistList;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilter     = (PPOAM_IREP_FOLDER_OBJECT   )pMyObject->hIrepFolderDNSWhitelist;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilterSp   = (PPOAM_IREP_FOLDER_OBJECT   )NULL;
    COSA_DML_DNS_WHITELIST          *pEntry                 = (COSA_DML_DNS_WHITELIST  *  )NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT  )NULL;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE             )NULL;
    ULONG                           ulEntryCount            = 0;
    ULONG                           ulIndex                 = 0;
    ULONG                           ulInstanceNumber        = 0;
    char*                           pFolderName             = NULL;

    if (!pPoamIrepFoIpFilter)
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Load the newly added but not yet commited entries */

    ulEntryCount = pPoamIrepFoIpFilter->GetFolderCount((ANSC_HANDLE)pPoamIrepFoIpFilter);

    for ( ulIndex = 0; ulIndex < ulEntryCount; ulIndex++ )
    {
        pFolderName = pPoamIrepFoIpFilter->EnumFolder((ANSC_HANDLE)pPoamIrepFoIpFilter, ulIndex);
        if (!pFolderName)
        {
            continue;
        }

        pPoamIrepFoIpFilterSp = pPoamIrepFoIpFilter->GetFolder((ANSC_HANDLE)pPoamIrepFoIpFilter, pFolderName);
        AnscFreeMemory(pFolderName);

        if (!pPoamIrepFoIpFilterSp)
        {
            continue;
        }

        if (TRUE)
        {
            pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoIpFilterSp->GetRecord((ANSC_HANDLE)pPoamIrepFoIpFilterSp, COSA_DML_RR_NAME_DNS_WHITELIST_URL_InsNum, NULL);
            if (pSlapVariable)
            {
                ulInstanceNumber = pSlapVariable->Variant.varUint32;
                SlapFreeVariable(pSlapVariable);
            }
        }

        pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pCosaContext)
        {
            pPoamIrepFoIpFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoIpFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        pEntry = (COSA_DML_DNS_WHITELIST*)AnscAllocateMemory(sizeof(COSA_DML_DNS_WHITELIST));

        if (!pEntry)
        {
            AnscFreeMemory(pCosaContext);
            pPoamIrepFoIpFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoIpFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        pEntry->InstanceNumber              = ulInstanceNumber;
        pCosaContext->InstanceNumber        = ulInstanceNumber;
        pCosaContext->bNew                  = TRUE;
        pCosaContext->hContext              = (ANSC_HANDLE)pEntry;
        pCosaContext->hParentTable          = NULL;
        pCosaContext->hPoamIrepUpperFo      = (ANSC_HANDLE)pPoamIrepFoIpFilter;
        pCosaContext->hPoamIrepFo           = (ANSC_HANDLE)pPoamIrepFoIpFilterSp;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDNS_Whitelist_UrlAddInfo
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCosaContext
            );

    description:

        This function add Info of DNS Whitelist Url.

    argument:   ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.
                ANSC_HANDLE                 hCosaContext
                This handle is actually the pointer of instance object.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDNS_Whitelist_UrlAddInfo
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_GATEWAY     pMyObject               = (PCOSA_DATAMODEL_LGI_GATEWAY)hThisObject;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFiler      = (PPOAM_IREP_FOLDER_OBJECT   )pMyObject->hIrepFolderDNSWhitelist;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilterSp   = (PPOAM_IREP_FOLDER_OBJECT   )NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT  )hCosaContext;
    COSA_DML_DNS_WHITELIST          *pEntry                 = (COSA_DML_DNS_WHITELIST*    )pCosaContext->hContext;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE             )NULL;

    if (!pPoamIrepFoIpFiler)
    {
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        pPoamIrepFoIpFiler->EnableFileSync((ANSC_HANDLE)pPoamIrepFoIpFiler, FALSE);
    }

    if (TRUE)
    {
        SlapAllocVariable(pSlapVariable);
        if (!pSlapVariable)
        {
            returnStatus = ANSC_STATUS_RESOURCES;
            goto  EXIT1;
        }
    }

    if (TRUE)
    {
        returnStatus = pPoamIrepFoIpFiler->DelRecord((ANSC_HANDLE)pPoamIrepFoIpFiler, COSA_DML_RR_NAME_DNS_WHITELIST_URL_NextInsNumber);

        pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
        pSlapVariable->Variant.varUint32 = pMyObject->DNSWhitelistNextInsNum;

        returnStatus = pPoamIrepFoIpFiler->AddRecord(
                    (ANSC_HANDLE)pPoamIrepFoIpFiler,
                    COSA_DML_RR_NAME_DNS_WHITELIST_URL_NextInsNumber,
                    SYS_REP_RECORD_TYPE_UINT,
                    SYS_RECORD_CONTENT_DEFAULT,
                    pSlapVariable,
                    0
                );

        SlapCleanVariable(pSlapVariable);
        SlapInitVariable (pSlapVariable);
    }

    if (TRUE)
    {
        pPoamIrepFoIpFilterSp = pPoamIrepFoIpFiler->AddFolder((ANSC_HANDLE)pPoamIrepFoIpFiler, "DNSWhitelistUrlTable", 0);

        if (!pPoamIrepFoIpFilterSp)
        {
            returnStatus = ANSC_STATUS_FAILURE;
            goto  EXIT1;
        }

        if (TRUE)
        {
            pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
            pSlapVariable->Variant.varUint32 = pEntry->InstanceNumber;

            returnStatus = pPoamIrepFoIpFilterSp->AddRecord(
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_DNS_WHITELIST_URL_InsNum,
                        SYS_REP_RECORD_TYPE_UINT,
                        SYS_RECORD_CONTENT_DEFAULT,
                        pSlapVariable,
                        0
                    );

            SlapCleanVariable(pSlapVariable);
            SlapInitVariable(pSlapVariable);
        }

        pCosaContext->hPoamIrepUpperFo = (ANSC_HANDLE)pPoamIrepFoIpFiler;
        pCosaContext->hPoamIrepFo = (ANSC_HANDLE)pPoamIrepFoIpFilterSp;
    }

EXIT1:

    if (pSlapVariable)
    {
        SlapFreeVariable(pSlapVariable);
    }
    pPoamIrepFoIpFiler->EnableFileSync((ANSC_HANDLE)pPoamIrepFoIpFiler, TRUE);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDNS_Whitelist_UrlDelInfo
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCosaContext
            );

    description:

        This function delete Info of DNS Whitelist Url.

    argument:   ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.
                ANSC_HANDLE                 hCosaContext
                This handle is actually the pointer of instance object.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDNS_Whitelist_UrlDelInfo
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT) hCosaContext;
    PPOAM_IREP_FOLDER_OBJECT pPoamIrepUpperFo = (PPOAM_IREP_FOLDER_OBJECT) pCosaContext->hPoamIrepUpperFo;
    PPOAM_IREP_FOLDER_OBJECT pPoamIrepFo = (PPOAM_IREP_FOLDER_OBJECT) pCosaContext->hPoamIrepFo;

    if (!pPoamIrepUpperFo || !pPoamIrepFo)
    {
        return ANSC_STATUS_FAILURE;
    }

    pPoamIrepUpperFo->EnableFileSync((ANSC_HANDLE)pPoamIrepUpperFo, FALSE);

    if (TRUE)
    {
        pPoamIrepFo->Close((ANSC_HANDLE)pPoamIrepFo);
        pPoamIrepUpperFo->DelFolder((ANSC_HANDLE)pPoamIrepUpperFo, pPoamIrepFo->GetFolderName((ANSC_HANDLE)pPoamIrepFo));
        pPoamIrepUpperFo->EnableFileSync((ANSC_HANDLE)pPoamIrepUpperFo, TRUE);
        AnscFreeMemory(pPoamIrepFo);
    }

    return ANSC_STATUS_SUCCESS;
}
