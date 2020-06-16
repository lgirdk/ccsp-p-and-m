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

#include "cosa_mac_filter_internal.h"
#include "slap_vho_exported_api.h"
#include <syscfg/syscfg.h>

extern void * g_pDslhDmlAgent;
/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaMacFilterCreate
            (
            );

    description:

        This function constructs cosa MAC Filter parameters and return handle.

    argument:

    return:     newly created MacFilter object.

**********************************************************************/

ANSC_HANDLE
CosaMacFilterCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_MAC_FILTER  pMyObject    = (PCOSA_DATAMODEL_MAC_FILTER)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_MAC_FILTER)AnscAllocateMemory(sizeof(COSA_DATAMODEL_MAC_FILTER));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_MAC_FILTER_OID;
    pMyObject->Create            = CosaMacFilterCreate;
    pMyObject->Remove            = CosaMacFilterRemove;
    pMyObject->Initialize        = CosaMacFilterInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMacFilterInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa MacFilter parameters and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaMacFilterInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MAC_FILTER         pMyObject    = (PCOSA_DATAMODEL_MAC_FILTER)hThisObject;
    PSLAP_VARIABLE                     pSlapVariable    = NULL;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoCOSA  = NULL;

    ULONG                              ulFwMACFilterCnt = 1;
    ULONG                              ulFwMACFilterIdx = 0;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoFwMACFilter = NULL;
    COSA_DML_FW_MACFILTER              *pFwMACFilter;
    PCOSA_CONTEXT_LINK_OBJECT          pFwMACFilterLinkObj = NULL;

    ULONG                              ulFwMacDayOfWeekCnt = 1;
    ULONG                              ulFwMacDayOfWeekIdx = 0;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoFwMacDayOfWeek = NULL;
    COSA_DML_FW_MAC_DAYOFWEEK          *pFwMacDayOfWeek;
    PCOSA_CONTEXT_LINK_OBJECT          pFwMacDayOfWeekLinkObj = NULL;

    /* Initiation all functions */
    //Fw_MAC_Filter_init
    ulFwMACFilterCnt = CosaDmlFW_MACFilter_GetNumberOfEntries();
    pSlapVariable    = NULL;
    pPoamIrepFoCOSA  = NULL;
    AnscSListInitializeHeader(&pMyObject->MACFilterList);
    pMyObject->MACFilterNextInsNum = 1;
    pMyObject->hIrepFolderMACCOSA = g_GetRegistryRootFolder(g_pDslhDmlAgent);

    pPoamIrepFoCOSA = (PPOAM_IREP_FOLDER_OBJECT)pMyObject->hIrepFolderMACCOSA;
    if (!pPoamIrepFoCOSA)
        return ANSC_STATUS_FAILURE;

    pPoamIrepFoFwMACFilter = (PPOAM_IREP_FOLDER_OBJECT)pPoamIrepFoCOSA->GetFolder(
            (ANSC_HANDLE)pPoamIrepFoCOSA,
            COSA_IREP_FOLDER_NAME_MAC_FILTER_FIREWALL);
    if (!pPoamIrepFoFwMACFilter)
    {
        pPoamIrepFoFwMACFilter =
            pPoamIrepFoCOSA->AddFolder(
                    (ANSC_HANDLE)pPoamIrepFoCOSA,
                    COSA_IREP_FOLDER_NAME_MAC_FILTER_FIREWALL,
                    0);
    }

    if (!pPoamIrepFoFwMACFilter)
        return ANSC_STATUS_FAILURE;
    else
        pMyObject->hIrepFolderFwMACFilter = pPoamIrepFoFwMACFilter;

    pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoFwMACFilter->GetRecord(
            (ANSC_HANDLE)pPoamIrepFoFwMACFilter,
            COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_NextInsNumber,
            NULL);
    if (pSlapVariable)
    {
        pMyObject->MACFilterNextInsNum = pSlapVariable->Variant.varUint32;
        SlapFreeVariable(pSlapVariable);
    }

    for (ulFwMACFilterIdx = 0; ulFwMACFilterIdx < ulFwMACFilterCnt; ulFwMACFilterIdx++)
    {
        pFwMACFilter = AnscAllocateMemory(sizeof(COSA_DML_FW_MACFILTER));
        if (!pFwMACFilter) {
            return ANSC_STATUS_FAILURE;
        }
        if (CosaDmlFW_MACFilter_GetEntryByIndex(ulFwMACFilterIdx, pFwMACFilter) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("%s: CosaDmlFW_MACFilter_GetEntryByIndex error\n", __FUNCTION__));
            AnscFreeMemory(pFwMACFilter);
            return ANSC_STATUS_FAILURE;
        }

        pFwMACFilterLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pFwMACFilterLinkObj)
        {
            AnscFreeMemory(pFwMACFilter);
            return ANSC_STATUS_FAILURE;
        }

        if (pFwMACFilter->InstanceNumber != 0)
        {
            if (pMyObject->MACFilterNextInsNum <= pFwMACFilter->InstanceNumber)
            {
                pMyObject->MACFilterNextInsNum = pFwMACFilter->InstanceNumber + 1;
                if (pMyObject->MACFilterNextInsNum == 0)
                {
                    pMyObject->MACFilterNextInsNum = 1;
                }
            }
        }
        else
        {
            pFwMACFilter->InstanceNumber = pMyObject->MACFilterNextInsNum;

            pMyObject->MACFilterNextInsNum++;
            if (pMyObject->MACFilterNextInsNum == 0)
            {
                pMyObject->MACFilterNextInsNum = 1;
            }

            _ansc_sprintf(pFwMACFilter->Alias, "cpe-MACFilter-%d", (int)pFwMACFilter->InstanceNumber);
            CosaDmlFW_MACFilter_SetValues(ulFwMACFilterIdx, pFwMACFilter->InstanceNumber, pFwMACFilter->Alias);
        }

        pFwMACFilterLinkObj->InstanceNumber  = pFwMACFilter->InstanceNumber;
        pFwMACFilterLinkObj->hContext        = pFwMACFilter;
        pFwMACFilterLinkObj->hParentTable    = NULL;
        pFwMACFilterLinkObj->bNew            = FALSE;

        CosaSListPushEntryByInsNum(&pMyObject->MACFilterList, pFwMACFilterLinkObj);

    }
    CosaFwReg_MACFilterGetInfo((ANSC_HANDLE)pMyObject);

    //FW_MacDayOfWeek_Init
    pSlapVariable    = NULL;
    pPoamIrepFoCOSA  = NULL;
    ulFwMacDayOfWeekCnt = CosaDmlFW_MacDayOfWeek_GetNumberOfEntries();
    AnscSListInitializeHeader(&pMyObject->MacDayOfWeekList);
    pMyObject->MacDayOfWeekNextInsNum = 1;
    pMyObject->hIrepFolderMacDayOfWeekCOSA = g_GetRegistryRootFolder(g_pDslhDmlAgent);

    pPoamIrepFoCOSA = (PPOAM_IREP_FOLDER_OBJECT)pMyObject->hIrepFolderMacDayOfWeekCOSA;
    if (!pPoamIrepFoCOSA)
        return ANSC_STATUS_FAILURE;

    pPoamIrepFoFwMacDayOfWeek = (PPOAM_IREP_FOLDER_OBJECT)pPoamIrepFoCOSA->GetFolder(
            (ANSC_HANDLE)pPoamIrepFoCOSA,
            COSA_IREP_FOLDER_NAME_MACDAYOFWEEK);
    if (!pPoamIrepFoFwMacDayOfWeek)
    {
        pPoamIrepFoFwMacDayOfWeek =
            pPoamIrepFoCOSA->AddFolder(
                    (ANSC_HANDLE)pPoamIrepFoCOSA,
                    COSA_IREP_FOLDER_NAME_MACDAYOFWEEK,
                    0);
    }

    if (!pPoamIrepFoFwMacDayOfWeek)
        return ANSC_STATUS_FAILURE;
    else
        pMyObject->hIrepFolderFwMacDayOfWeek = pPoamIrepFoFwMacDayOfWeek;

    pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoFwMacDayOfWeek->GetRecord(
            (ANSC_HANDLE)pPoamIrepFoFwMacDayOfWeek,
            COSA_DML_RR_NAME_MacDayOfWeekNextInsNumber,
            NULL);
    if (pSlapVariable)
    {
        pMyObject->MacDayOfWeekNextInsNum = pSlapVariable->Variant.varUint32;
        SlapFreeVariable(pSlapVariable);
    }

    for (ulFwMacDayOfWeekIdx = 0; ulFwMacDayOfWeekIdx < ulFwMacDayOfWeekCnt; ulFwMacDayOfWeekIdx++)
    {
        pFwMacDayOfWeek = AnscAllocateMemory(sizeof(COSA_DML_FW_MAC_DAYOFWEEK));
        if (!pFwMacDayOfWeek) {
            return ANSC_STATUS_FAILURE;
        }
        if (CosaDmlFW_MacDayOfWeek_GetEntryByIndex(ulFwMacDayOfWeekIdx, pFwMacDayOfWeek) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("%s: CosaDmlFW_MacDayOfWeek_GetEntryByIndex error\n", __FUNCTION__));
            AnscFreeMemory(pFwMacDayOfWeek);
            return ANSC_STATUS_FAILURE;
        }

        pFwMacDayOfWeekLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pFwMacDayOfWeekLinkObj)
        {
            AnscFreeMemory(pFwMacDayOfWeek);
            return ANSC_STATUS_FAILURE;
        }

        if (pFwMacDayOfWeek->InstanceNumber != 0)
        {
            if (pMyObject->MacDayOfWeekNextInsNum <= pFwMacDayOfWeek->InstanceNumber)
            {
                pMyObject->MacDayOfWeekNextInsNum = pFwMacDayOfWeek->InstanceNumber + 1;
                if (pMyObject->MacDayOfWeekNextInsNum == 0)
                {
                    pMyObject->MacDayOfWeekNextInsNum = 1;
                }
            }
        }
        else
        {
            pFwMacDayOfWeek->InstanceNumber = pMyObject->MacDayOfWeekNextInsNum;

            pMyObject->MacDayOfWeekNextInsNum++;
            if (pMyObject->MacDayOfWeekNextInsNum == 0)
            {
                pMyObject->MacDayOfWeekNextInsNum = 1;
            }
            _ansc_sprintf(pFwMacDayOfWeek->Alias, "cpe-MacDayOfWeek-%d", (int)pFwMacDayOfWeek->InstanceNumber);
            CosaDmlFW_MacDayOfWeek_SetValues(ulFwMacDayOfWeekIdx, pFwMacDayOfWeek->InstanceNumber, pFwMacDayOfWeek->Alias, pFwMacDayOfWeek->MacDayOfWeek_BlockTimeBitMask);
        }
        pFwMacDayOfWeekLinkObj->InstanceNumber  = pFwMacDayOfWeek->InstanceNumber;
        pFwMacDayOfWeekLinkObj->hContext        = pFwMacDayOfWeek;
        pFwMacDayOfWeekLinkObj->hParentTable    = NULL;
        pFwMacDayOfWeekLinkObj->bNew            = FALSE;

        CosaSListPushEntryByInsNum(&pMyObject->MacDayOfWeekList, pFwMacDayOfWeekLinkObj);
    }
    CosaFwReg_MacDayOfWeekGetInfo((ANSC_HANDLE)pMyObject);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaMacFilterRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa MacFilter parameters and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaMacFilterRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MAC_FILTER         pMyObject    = (PCOSA_DATAMODEL_MAC_FILTER)hThisObject;

    /* Remove necessary resounce */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);
    return returnStatus;
}

ANSC_STATUS
CosaFwReg_MACFilterGetInfo(
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_MAC_FILTER      pMyObject               = (PCOSA_DATAMODEL_MAC_FILTER)hThisObject;
    PSLIST_HEADER                   pListHead               = (PSLIST_HEADER            )&pMyObject->MACFilterList;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoMACFilter    = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwMACFilter;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoMACFilterSp  = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    COSA_DML_FW_MACFILTER           *pEntry                 = (COSA_DML_FW_MACFILTER    *)NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE           )NULL;
    ULONG                           ulEntryCount            = 0;
    ULONG                           ulIndex                 = 0;
    ULONG                           ulInstanceNumber        = 0;
    char*                           pFolderName             = NULL;
    char*                           pAlias                  = NULL;

    if ( !pPoamIrepFoMACFilter )
    {
        return ANSC_STATUS_FAILURE;
    }
    /* Load the newly added but not yet commited entries */

    ulEntryCount = pPoamIrepFoMACFilter->GetFolderCount((ANSC_HANDLE)pPoamIrepFoMACFilter);

    for ( ulIndex = 0; ulIndex < ulEntryCount; ulIndex++ )
    {
        pFolderName =
            pPoamIrepFoMACFilter->EnumFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoMACFilter,
                    ulIndex
                );

        if ( !pFolderName )
        {
            continue;
        }
        pPoamIrepFoMACFilterSp = pPoamIrepFoMACFilter->GetFolder((ANSC_HANDLE)pPoamIrepFoMACFilter, pFolderName);

        AnscFreeMemory(pFolderName);

        if ( !pPoamIrepFoMACFilterSp )
        {
            continue;
        }
        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoMACFilterSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoMACFilterSp,
                        COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_InsNum,
                        NULL
                    );

            if ( pSlapVariable )
            {
                ulInstanceNumber = pSlapVariable->Variant.varUint32;

                SlapFreeVariable(pSlapVariable);
            }
        }

        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoMACFilterSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoMACFilterSp,
                        COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_Alias,
                        NULL
                    );

            if ( pSlapVariable )
            {
                pAlias = AnscCloneString(pSlapVariable->Variant.varString);

                SlapFreeVariable(pSlapVariable);
            }
        }
        pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));

        if ( !pCosaContext )
        {
            AnscFreeMemory(pAlias);

            pPoamIrepFoMACFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoMACFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        pEntry = (COSA_DML_FW_MACFILTER*)AnscAllocateMemory(sizeof(COSA_DML_FW_MACFILTER));

        if ( !pEntry )
        {
            AnscFreeMemory(pAlias);
            AnscFreeMemory(pCosaContext);

            pPoamIrepFoMACFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoMACFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        AnscCopyString(pEntry->Alias, pAlias ? pAlias : "");

        pEntry->InstanceNumber = ulInstanceNumber;

        pCosaContext->InstanceNumber        = ulInstanceNumber;
        pCosaContext->bNew                  = TRUE;
        pCosaContext->hContext              = (ANSC_HANDLE)pEntry;
        pCosaContext->hParentTable          = NULL;
        pCosaContext->hPoamIrepUpperFo      = (ANSC_HANDLE)pPoamIrepFoMACFilter;
        pCosaContext->hPoamIrepFo           = (ANSC_HANDLE)pPoamIrepFoMACFilterSp;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);
        if ( pAlias )
        {
            AnscFreeMemory(pAlias);

            pAlias = NULL;
        }
    }


    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaFwReg_MACFilterAddInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_MAC_FILTER      pMyObject               = (PCOSA_DATAMODEL_MAC_FILTER   )hThisObject;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoMACFiler     = (PPOAM_IREP_FOLDER_OBJECT     )pMyObject->hIrepFolderFwMACFilter;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoMACFilterSp  = (PPOAM_IREP_FOLDER_OBJECT     )NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT    )hCosaContext;
    COSA_DML_FW_MACFILTER           *pEntry                 = (COSA_DML_FW_MACFILTER *      )pCosaContext->hContext;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE               )NULL;

    if ( !pPoamIrepFoMACFiler )
    {
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        pPoamIrepFoMACFiler->EnableFileSync((ANSC_HANDLE)pPoamIrepFoMACFiler, FALSE);
    }

    if ( TRUE )
    {
        SlapAllocVariable(pSlapVariable);

        if ( !pSlapVariable )
        {
            returnStatus = ANSC_STATUS_RESOURCES;

            goto  EXIT1;
        }
    }

    if ( TRUE )
    {
        returnStatus =
            pPoamIrepFoMACFiler->DelRecord
                (
                    (ANSC_HANDLE)pPoamIrepFoMACFiler,
                    COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_NextInsNumber
                );

        pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
        pSlapVariable->Variant.varUint32 = pMyObject->MACFilterNextInsNum;

        returnStatus =
            pPoamIrepFoMACFiler->AddRecord
                (
                    (ANSC_HANDLE)pPoamIrepFoMACFiler,
                    COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_NextInsNumber,
                    SYS_REP_RECORD_TYPE_UINT,
                    SYS_RECORD_CONTENT_DEFAULT,
                    pSlapVariable,
                    0
                );

        SlapCleanVariable(pSlapVariable);
        SlapInitVariable (pSlapVariable);
    }

    if ( TRUE )
    {
        pPoamIrepFoMACFilterSp =
            pPoamIrepFoMACFiler->AddFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoMACFiler,
                    pEntry->Alias,
                    0
                );

        if ( !pPoamIrepFoMACFilterSp )
        {
            returnStatus = ANSC_STATUS_FAILURE;

            goto  EXIT1;
        }

        if ( TRUE )
        {
            pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
            pSlapVariable->Variant.varUint32 = pEntry->InstanceNumber;

            returnStatus =
                pPoamIrepFoMACFilterSp->AddRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoMACFilterSp,
                        COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_InsNum,
                        SYS_REP_RECORD_TYPE_UINT,
                        SYS_RECORD_CONTENT_DEFAULT,
                        pSlapVariable,
                        0
                    );

            SlapCleanVariable(pSlapVariable);
            SlapInitVariable (pSlapVariable);
        }

        if ( TRUE )
        {
            pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_string;
            pSlapVariable->Variant.varString = AnscCloneString(pEntry->Alias);

            returnStatus =
                pPoamIrepFoMACFilterSp->AddRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoMACFilterSp,
                        COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_Alias,
                        SYS_REP_RECORD_TYPE_ASTR,
                        SYS_RECORD_CONTENT_DEFAULT,
                        pSlapVariable,
                        0
                    );

            SlapCleanVariable(pSlapVariable);
            SlapInitVariable (pSlapVariable);
        }

        pCosaContext->hPoamIrepUpperFo = (ANSC_HANDLE)pPoamIrepFoMACFiler;
        pCosaContext->hPoamIrepFo      = (ANSC_HANDLE)pPoamIrepFoMACFilterSp;
    }

EXIT1:

    if ( pSlapVariable )
    {
        SlapFreeVariable(pSlapVariable);
    }

    pPoamIrepFoMACFiler->EnableFileSync((ANSC_HANDLE)pPoamIrepFoMACFiler, TRUE);

    return returnStatus;
}


ANSC_STATUS
CosaFwReg_MACFilterDelInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext      = (PCOSA_CONTEXT_LINK_OBJECT)hCosaContext;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepUpperFo  = (PPOAM_IREP_FOLDER_OBJECT )pCosaContext->hPoamIrepUpperFo;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFo       = (PPOAM_IREP_FOLDER_OBJECT )pCosaContext->hPoamIrepFo;
    if ( !pPoamIrepUpperFo || !pPoamIrepFo )
    {
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        pPoamIrepUpperFo->EnableFileSync((ANSC_HANDLE)pPoamIrepUpperFo, FALSE);
    }

    if ( TRUE )
    {
        pPoamIrepFo->Close((ANSC_HANDLE)pPoamIrepFo);

        pPoamIrepUpperFo->DelFolder
            (
                (ANSC_HANDLE)pPoamIrepUpperFo,
                pPoamIrepFo->GetFolderName((ANSC_HANDLE)pPoamIrepFo)
            );

        pPoamIrepUpperFo->EnableFileSync((ANSC_HANDLE)pPoamIrepUpperFo, TRUE);

        AnscFreeMemory(pPoamIrepFo);
    }
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaFwReg_MacDayOfWeekGetInfo(
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_MAC_FILTER      pMyObject                   = (PCOSA_DATAMODEL_MAC_FILTER)hThisObject;
    PSLIST_HEADER                   pListHead                   = (PSLIST_HEADER            )&pMyObject->MacDayOfWeekList;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoMacDayOfWeek     = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwMacDayOfWeek;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoMacDayOfWeekSp   = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    COSA_DML_FW_MAC_DAYOFWEEK       *pEntry                     = (COSA_DML_FW_MAC_DAYOFWEEK *)NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext                = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSLAP_VARIABLE                  pSlapVariable               = (PSLAP_VARIABLE           )NULL;
    ULONG                           ulEntryCount                = 0;
    ULONG                           ulIndex                     = 0;
    ULONG                           ulInstanceNumber            = 0;
    char*                           pFolderName                 = NULL;
    char*                           pAlias                      = NULL;

    if ( !pPoamIrepFoMacDayOfWeek )
    {
        return ANSC_STATUS_FAILURE;
    }
    /* Load the newly added but not yet commited entries */

    ulEntryCount = pPoamIrepFoMacDayOfWeek->GetFolderCount((ANSC_HANDLE)pPoamIrepFoMacDayOfWeek);

    for ( ulIndex = 0; ulIndex < ulEntryCount; ulIndex++ )
    {
        pFolderName =
            pPoamIrepFoMacDayOfWeek->EnumFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoMacDayOfWeek,
                    ulIndex
                );
        if ( !pFolderName )
        {
            continue;
        }
        pPoamIrepFoMacDayOfWeekSp = pPoamIrepFoMacDayOfWeek->GetFolder((ANSC_HANDLE)pPoamIrepFoMacDayOfWeek, pFolderName);

        AnscFreeMemory(pFolderName);

        if ( !pPoamIrepFoMacDayOfWeekSp )
        {
            continue;
        }
        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoMacDayOfWeekSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoMacDayOfWeekSp,
                        COSA_DML_RR_NAME_MacDayOfWeekNextInsNumber,
                        NULL
                    );

            if ( pSlapVariable )
            {
                ulInstanceNumber = pSlapVariable->Variant.varUint32;

                SlapFreeVariable(pSlapVariable);
            }
        }

        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoMacDayOfWeekSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoMacDayOfWeekSp,
                        COSA_DML_RR_NAME_MacDayOfWeekAlias,
                        NULL
                    );

            if ( pSlapVariable )
            {
                pAlias = AnscCloneString(pSlapVariable->Variant.varString);

                SlapFreeVariable(pSlapVariable);
            }
        }
        pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));

        if ( !pCosaContext )
        {
            AnscFreeMemory(pAlias);
            return ANSC_STATUS_RESOURCES;
        }

        pEntry = (COSA_DML_FW_MAC_DAYOFWEEK*)AnscAllocateMemory(sizeof(COSA_DML_FW_MAC_DAYOFWEEK));

        if ( !pEntry )
        {
            AnscFreeMemory(pAlias);
            AnscFreeMemory(pCosaContext);

            return ANSC_STATUS_RESOURCES;
        }

        AnscCopyString(pEntry->Alias, pAlias ? pAlias : "");
        pEntry->InstanceNumber = ulInstanceNumber;

        pCosaContext->InstanceNumber        = ulInstanceNumber;
        pCosaContext->bNew                  = TRUE;
        pCosaContext->hContext              = (ANSC_HANDLE)pEntry;
        pCosaContext->hParentTable          = NULL;
        pCosaContext->hPoamIrepUpperFo      = (ANSC_HANDLE)pPoamIrepFoMacDayOfWeek;
        pCosaContext->hPoamIrepFo           = (ANSC_HANDLE)pPoamIrepFoMacDayOfWeekSp;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);
        if ( pAlias )
        {
            AnscFreeMemory(pAlias);

            pAlias = NULL;
        }
    }
    return ANSC_STATUS_SUCCESS;
}
