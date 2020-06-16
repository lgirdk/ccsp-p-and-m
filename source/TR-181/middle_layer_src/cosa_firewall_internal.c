/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
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
*/

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

/**************************************************************************

    module: cosa_firewall_internal.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the internal apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        Tom Chang

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

#include "cosa_firewall_internal.h"
#include "safec_lib_common.h"

// LGI ADD START
#include "slap_vho_exported_api.h"
#include <syscfg/syscfg.h>

extern void * g_pDslhDmlAgent;
// LGI ADD END

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaFirewallCreate
            (
                VOID
            );

    description:

        This function constructs cosa Firewall object and return handle.

    argument:  

    return:     newly created Firewall object.

**********************************************************************/

ANSC_HANDLE
CosaFirewallCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_FIREWALL        pMyObject    = (PCOSA_DATAMODEL_FIREWALL)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_FIREWALL)AnscAllocateMemory(sizeof(COSA_DATAMODEL_FIREWALL));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_FIREWALL_OID;
    pMyObject->Create            = CosaFirewallCreate;
    pMyObject->Remove            = CosaFirewallRemove;
    pMyObject->Initialize        = CosaFirewallInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    prototype:

        ANSC_STATUS
        CosaFirewallInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa Firewall object and return handle.

    argument:	ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaFirewallInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_FIREWALL        pMyObject       = (PCOSA_DATAMODEL_FIREWALL)hThisObject;
    PCOSA_DML_FIREWALL_CFG          pFirewallCfg    = NULL;
// LGI ADD START
    PSLAP_VARIABLE                     pSlapVariable    = NULL;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoCOSA  = NULL;

    ULONG                              ulFwV4IpFilterCnt = 1;
    ULONG                              ulFwV4IpFilterIdx = 0;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoFwV4IpFilter = NULL;
    COSA_DML_FW_IPFILTER               *pFwV4IpFilter;
    PCOSA_CONTEXT_LINK_OBJECT          pFwV4IpFilterLinkObj = NULL;

    ULONG                              ulFwV6IpFilterCnt = 1;
    ULONG                              ulFwV6IpFilterIdx = 0;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoFwV6IpFilter = NULL;
    COSA_DML_FW_IPFILTER               *pFwV6IpFilter;
    PCOSA_CONTEXT_LINK_OBJECT          pFwV6IpFilterLinkObj = NULL;

    ULONG                              ulFwV4DayOfWeekCnt = 1;
    ULONG                              ulFwV4DayOfWeekIdx = 0;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoFwV4DayOfWeek = NULL;
    COSA_DML_FW_V4_DAYOFWEEK           *pFwV4DayOfWeek;
    PCOSA_CONTEXT_LINK_OBJECT          pFwV4DayOfWeekLinkObj = NULL;

    ULONG                              ulFwV6DayOfWeekCnt = 1;
    ULONG                              ulFwV6DayOfWeekIdx = 0;
    PPOAM_IREP_FOLDER_OBJECT           pPoamIrepFoFwV6DayOfWeek = NULL;
    COSA_DML_FW_V6_DAYOFWEEK           *pFwV6DayOfWeek;
    PCOSA_CONTEXT_LINK_OBJECT          pFwV6DayOfWeekLinkObj = NULL;
// LGI ADD END
    errno_t                         rc              = -1;

    returnStatus = CosaDmlFirewallInit(NULL, NULL);

    if ( returnStatus != ANSC_STATUS_SUCCESS )
    {
        return returnStatus;
    }

    /* Initiation all functions */
    pFirewallCfg = AnscAllocateMemory(sizeof(COSA_DML_FIREWALL_CFG));

    if ( !pFirewallCfg )
    {
        CcspTraceWarning(("CosaFirewallInitialize -- Resource allocation error\n"));

        return FALSE;
    }

    returnStatus = CosaDmlFirewallGetConfig(NULL, pFirewallCfg);

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        pMyObject->FirewallConfig.FirewallLevel = pFirewallCfg->FirewallLevel;

        rc = strcpy_s(pMyObject->FirewallConfig.Version, sizeof(pMyObject->FirewallConfig.Version),pFirewallCfg->Version);
        ERR_CHK(rc);
        rc = strcpy_s(pMyObject->FirewallConfig.LastChange, sizeof(pMyObject->FirewallConfig.LastChange),pFirewallCfg->LastChange);
        ERR_CHK(rc);
    }

// LGI ADD START
    //FW_V4IPFilter_Init();
    ulFwV4IpFilterCnt = CosaDmlFW_V4_IPFilter_GetNumberOfEntries();

    AnscSListInitializeHeader(&pMyObject->FwV4IpFilterList);
    pMyObject->FwV4IpFilterNextInsNum = 1;
    pMyObject->hIrepFolderV4IPCOSA = g_GetRegistryRootFolder(g_pDslhDmlAgent);

    pPoamIrepFoCOSA = (PPOAM_IREP_FOLDER_OBJECT)pMyObject->hIrepFolderV4IPCOSA;
    if (!pPoamIrepFoCOSA)
    {
        return ANSC_STATUS_FAILURE;
    }
    pPoamIrepFoFwV4IpFilter = (PPOAM_IREP_FOLDER_OBJECT)pPoamIrepFoCOSA->GetFolder(
            (ANSC_HANDLE)pPoamIrepFoCOSA,
            COSA_IREP_FOLDER_NAME_IP_FILTER_FIREWALL);
    if (!pPoamIrepFoFwV4IpFilter)
    {
        pPoamIrepFoFwV4IpFilter =
            pPoamIrepFoCOSA->AddFolder(
                    (ANSC_HANDLE)pPoamIrepFoCOSA,
                    COSA_IREP_FOLDER_NAME_IP_FILTER_FIREWALL,
                    0);
    }

    if (!pPoamIrepFoFwV4IpFilter)
        return ANSC_STATUS_FAILURE;
    else
        pMyObject->hIrepFolderFwV4IpFiler = pPoamIrepFoFwV4IpFilter;

    pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoFwV4IpFilter->GetRecord(
            (ANSC_HANDLE)pPoamIrepFoFwV4IpFilter,
            COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NextInsNumber,
            NULL);
    if (pSlapVariable)
    {
        pMyObject->FwV4IpFilterNextInsNum = pSlapVariable->Variant.varUint32;
        SlapFreeVariable(pSlapVariable);
    }

    for (ulFwV4IpFilterIdx = 0; ulFwV4IpFilterIdx < ulFwV4IpFilterCnt; ulFwV4IpFilterIdx++)
    {
        pFwV4IpFilter = AnscAllocateMemory(sizeof(COSA_DML_FW_IPFILTER));
        if (!pFwV4IpFilter)
            return ANSC_STATUS_FAILURE;

        if (CosaDmlFW_V4_IPFilter_GetEntryByIndex(ulFwV4IpFilterIdx, pFwV4IpFilter) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("%s: CosaDmlFW_V4_IPFilter_GetEntryByIndex error\n", __FUNCTION__));
            AnscFreeMemory(pFwV4IpFilter);
            return ANSC_STATUS_FAILURE;
        }

        pFwV4IpFilterLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pFwV4IpFilterLinkObj)
        {
            AnscFreeMemory(pFwV4IpFilter);
            return ANSC_STATUS_FAILURE;
        }

        if (pFwV4IpFilter->InstanceNumber != 0)
        {
            if (pMyObject->FwV4IpFilterNextInsNum <= pFwV4IpFilter->InstanceNumber)
            {
                pMyObject->FwV4IpFilterNextInsNum = pFwV4IpFilter->InstanceNumber + 1;
                if (pMyObject->FwV4IpFilterNextInsNum == 0)
                {
                    pMyObject->FwV4IpFilterNextInsNum = 1;
                }
            }
        }
        else
        {
            pFwV4IpFilter->InstanceNumber = pMyObject->FwV4IpFilterNextInsNum;

            pMyObject->FwV4IpFilterNextInsNum++;
            if (pMyObject->FwV4IpFilterNextInsNum == 0)
            {
                pMyObject->FwV4IpFilterNextInsNum = 1;
            }

            _ansc_sprintf(pFwV4IpFilter->Alias, "cpe-V4IpFilter-%d", (int)pFwV4IpFilter->InstanceNumber);
            CosaDmlFW_V4_IPFilter_SetValues(ulFwV4IpFilterIdx, pFwV4IpFilter->InstanceNumber, pFwV4IpFilter->Alias);
        }

        pFwV4IpFilterLinkObj->InstanceNumber  = pFwV4IpFilter->InstanceNumber;
        pFwV4IpFilterLinkObj->hContext        = pFwV4IpFilter;
        pFwV4IpFilterLinkObj->hParentTable    = NULL;
        pFwV4IpFilterLinkObj->bNew            = FALSE;

        CosaSListPushEntryByInsNum(&pMyObject->FwV4IpFilterList, pFwV4IpFilterLinkObj);

    }
    CosaFwReg_V4_IpFilterGetInfo((ANSC_HANDLE)pMyObject);

    //FW V6 IPFilter Init();
    ulFwV6IpFilterCnt = CosaDmlFW_V6_IPFilter_GetNumberOfEntries();

    AnscSListInitializeHeader(&pMyObject->FwV6IpFilterList);
    pMyObject->FwV6IpFilterNextInsNum = 1;
    pMyObject->hIrepFolderV6IPCOSA = g_GetRegistryRootFolder(g_pDslhDmlAgent);

    pPoamIrepFoCOSA = (PPOAM_IREP_FOLDER_OBJECT)pMyObject->hIrepFolderV6IPCOSA;
    if (!pPoamIrepFoCOSA)
    {
        return ANSC_STATUS_FAILURE;
    }
    pPoamIrepFoFwV6IpFilter = (PPOAM_IREP_FOLDER_OBJECT)pPoamIrepFoCOSA->GetFolder(
            (ANSC_HANDLE)pPoamIrepFoCOSA,
            COSA_IREP_FOLDER_NAME_IP_FILTER_FIREWALL);
    if (!pPoamIrepFoFwV6IpFilter)
    {
        pPoamIrepFoFwV6IpFilter =
            pPoamIrepFoCOSA->AddFolder(
                    (ANSC_HANDLE)pPoamIrepFoCOSA,
                    COSA_IREP_FOLDER_NAME_IP_FILTER_FIREWALL,
                    0);
    }

    if (!pPoamIrepFoFwV6IpFilter)
        return ANSC_STATUS_FAILURE;
    else
        pMyObject->hIrepFolderFwV6IpFiler = pPoamIrepFoFwV6IpFilter;

    pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoFwV6IpFilter->GetRecord(
            (ANSC_HANDLE)pPoamIrepFoFwV6IpFilter,
            COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NextInsNumber,
            NULL);
    if (pSlapVariable)
    {
        pMyObject->FwV6IpFilterNextInsNum = pSlapVariable->Variant.varUint32;
        SlapFreeVariable(pSlapVariable);
    }

    for (ulFwV6IpFilterIdx = 0; ulFwV6IpFilterIdx < ulFwV6IpFilterCnt; ulFwV6IpFilterIdx++)
    {
        pFwV6IpFilter = AnscAllocateMemory(sizeof(COSA_DML_FW_IPFILTER));
        if (!pFwV6IpFilter)
            return ANSC_STATUS_FAILURE;

        if (CosaDmlFW_V6_IPFilter_GetEntryByIndex(ulFwV6IpFilterIdx, pFwV6IpFilter) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("%s: CosaDmlFW_V6_IPFilter_GetEntryByIndex error\n", __FUNCTION__));
            AnscFreeMemory(pFwV6IpFilter);
            return ANSC_STATUS_FAILURE;
        }

        pFwV6IpFilterLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pFwV6IpFilterLinkObj)
        {
            AnscFreeMemory(pFwV6IpFilter);
            return ANSC_STATUS_FAILURE;
        }

        if (pFwV6IpFilter->InstanceNumber != 0)
        {
            if (pMyObject->FwV6IpFilterNextInsNum <= pFwV6IpFilter->InstanceNumber)
            {
                pMyObject->FwV6IpFilterNextInsNum = pFwV6IpFilter->InstanceNumber + 1;
                if (pMyObject->FwV6IpFilterNextInsNum == 0)
                {
                    pMyObject->FwV6IpFilterNextInsNum = 1;
                }
            }
        }
        else
        {
            pFwV6IpFilter->InstanceNumber = pMyObject->FwV6IpFilterNextInsNum;

            pMyObject->FwV6IpFilterNextInsNum++;
            if (pMyObject->FwV6IpFilterNextInsNum == 0)
            {
                pMyObject->FwV6IpFilterNextInsNum = 1;
            }

            _ansc_sprintf(pFwV6IpFilter->Alias, "cpe-V6IpFilter-%d", (int)pFwV6IpFilter->InstanceNumber);
            CosaDmlFW_V6_IPFilter_SetValues(ulFwV6IpFilterIdx, pFwV6IpFilter->InstanceNumber, pFwV6IpFilter->Alias);
        }

        pFwV6IpFilterLinkObj->InstanceNumber  = pFwV6IpFilter->InstanceNumber;
        pFwV6IpFilterLinkObj->hContext        = pFwV6IpFilter;
        pFwV6IpFilterLinkObj->hParentTable    = NULL;
        pFwV6IpFilterLinkObj->bNew            = FALSE;

        CosaSListPushEntryByInsNum(&pMyObject->FwV6IpFilterList, pFwV6IpFilterLinkObj);

    }
    CosaFwReg_V6_IpFilterGetInfo((ANSC_HANDLE)pMyObject);

    //FW_V4DayOfWeek_Init
    pSlapVariable    = NULL;
    pPoamIrepFoCOSA  = NULL;
    ulFwV4DayOfWeekCnt = CosaDmlFW_V4DayOfWeek_GetNumberOfEntries();
    AnscSListInitializeHeader(&pMyObject->V4DayOfWeekList);
    pMyObject->V4DayOfWeekNextInsNum = 1;
    pMyObject->hIrepFolderV4DayOfWeekCOSA = g_GetRegistryRootFolder(g_pDslhDmlAgent);

    pPoamIrepFoCOSA = (PPOAM_IREP_FOLDER_OBJECT)pMyObject->hIrepFolderV4DayOfWeekCOSA;
    if (!pPoamIrepFoCOSA)
        return ANSC_STATUS_FAILURE;

    pPoamIrepFoFwV4DayOfWeek = (PPOAM_IREP_FOLDER_OBJECT)pPoamIrepFoCOSA->GetFolder(
            (ANSC_HANDLE)pPoamIrepFoCOSA,
            COSA_IREP_FOLDER_NAME_V4DAYOFWEEK);
    if (!pPoamIrepFoFwV4DayOfWeek)
    {
        pPoamIrepFoFwV4DayOfWeek =
            pPoamIrepFoCOSA->AddFolder(
                    (ANSC_HANDLE)pPoamIrepFoCOSA,
                    COSA_IREP_FOLDER_NAME_V4DAYOFWEEK,
                    0);
    }

    if (!pPoamIrepFoFwV4DayOfWeek)
        return ANSC_STATUS_FAILURE;
    else
        pMyObject->hIrepFolderFwV4DayOfWeek = pPoamIrepFoFwV4DayOfWeek;

    pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoFwV4DayOfWeek->GetRecord(
            (ANSC_HANDLE)pPoamIrepFoFwV4DayOfWeek,
            COSA_DML_RR_NAME_V4DayOfWeekNextInsNumber,
            NULL);
    if (pSlapVariable)
    {
        pMyObject->V4DayOfWeekNextInsNum = pSlapVariable->Variant.varUint32;
        SlapFreeVariable(pSlapVariable);
    }

    for (ulFwV4DayOfWeekIdx = 0; ulFwV4DayOfWeekIdx < ulFwV4DayOfWeekCnt; ulFwV4DayOfWeekIdx++)
    {
        pFwV4DayOfWeek = AnscAllocateMemory(sizeof(COSA_DML_FW_V4_DAYOFWEEK));
        if (!pFwV4DayOfWeek) {
            return ANSC_STATUS_FAILURE;
        }
        if (CosaDmlFW_V4DayOfWeek_GetEntryByIndex(ulFwV4DayOfWeekIdx, pFwV4DayOfWeek) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("%s: CosaDmlFW_V4DayOfWeek_GetEntryByIndex error\n", __FUNCTION__));
            AnscFreeMemory(pFwV4DayOfWeek);
            return ANSC_STATUS_FAILURE;
        }

        pFwV4DayOfWeekLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pFwV4DayOfWeekLinkObj)
        {
            AnscFreeMemory(pFwV4DayOfWeek);
            return ANSC_STATUS_FAILURE;
        }

        if (pFwV4DayOfWeek->InstanceNumber != 0)
        {
            if (pMyObject->V4DayOfWeekNextInsNum <= pFwV4DayOfWeek->InstanceNumber)
            {
                pMyObject->V4DayOfWeekNextInsNum = pFwV4DayOfWeek->InstanceNumber + 1;
                if (pMyObject->V4DayOfWeekNextInsNum == 0)
                {
                    pMyObject->V4DayOfWeekNextInsNum = 1;
                }
            }
        }
        else
        {
            pFwV4DayOfWeek->InstanceNumber = pMyObject->V4DayOfWeekNextInsNum;

            pMyObject->V4DayOfWeekNextInsNum++;
            if (pMyObject->V4DayOfWeekNextInsNum == 0)
            {
                pMyObject->V4DayOfWeekNextInsNum = 1;
            }

            _ansc_sprintf(pFwV4DayOfWeek->Alias, "cpe-V4DayOfWeek-%d", (int)pFwV4DayOfWeek->InstanceNumber);
            CosaDmlFW_V4DayOfWeek_SetValues(ulFwV4DayOfWeekIdx, pFwV4DayOfWeek->InstanceNumber, pFwV4DayOfWeek->Alias, pFwV4DayOfWeek->V4DayOfWeek_BlockTimeBitMask);
        }
        pFwV4DayOfWeekLinkObj->InstanceNumber  = pFwV4DayOfWeek->InstanceNumber;
        pFwV4DayOfWeekLinkObj->hContext        = pFwV4DayOfWeek;
        pFwV4DayOfWeekLinkObj->hParentTable    = NULL;
        pFwV4DayOfWeekLinkObj->bNew            = FALSE;

        CosaSListPushEntryByInsNum(&pMyObject->V4DayOfWeekList, pFwV4DayOfWeekLinkObj);
    }
    CosaFwReg_V4DayOfWeekGetInfo((ANSC_HANDLE)pMyObject);
    //FW_V6DayOfWeek_Init
    pSlapVariable    = NULL;
    pPoamIrepFoCOSA  = NULL;
    ulFwV6DayOfWeekCnt = CosaDmlFW_V6DayOfWeek_GetNumberOfEntries();
    AnscSListInitializeHeader(&pMyObject->V6DayOfWeekList);
    pMyObject->V6DayOfWeekNextInsNum = 1;
    pMyObject->hIrepFolderV6DayOfWeekCOSA = g_GetRegistryRootFolder(g_pDslhDmlAgent);

    pPoamIrepFoCOSA = (PPOAM_IREP_FOLDER_OBJECT)pMyObject->hIrepFolderV6DayOfWeekCOSA;
    if (!pPoamIrepFoCOSA)
        return ANSC_STATUS_FAILURE;

    pPoamIrepFoFwV6DayOfWeek = (PPOAM_IREP_FOLDER_OBJECT)pPoamIrepFoCOSA->GetFolder(
            (ANSC_HANDLE)pPoamIrepFoCOSA,
            COSA_IREP_FOLDER_NAME_V6DAYOFWEEK);
    if (!pPoamIrepFoFwV6DayOfWeek)
    {
        pPoamIrepFoFwV6DayOfWeek =
            pPoamIrepFoCOSA->AddFolder(
                    (ANSC_HANDLE)pPoamIrepFoCOSA,
                    COSA_IREP_FOLDER_NAME_V6DAYOFWEEK,
                    0);
    }

    if (!pPoamIrepFoFwV6DayOfWeek)
        return ANSC_STATUS_FAILURE;
    else
        pMyObject->hIrepFolderFwV6DayOfWeek = pPoamIrepFoFwV6DayOfWeek;

    pSlapVariable = (PSLAP_VARIABLE)pPoamIrepFoFwV6DayOfWeek->GetRecord(
            (ANSC_HANDLE)pPoamIrepFoFwV6DayOfWeek,
            COSA_DML_RR_NAME_V6DayOfWeekNextInsNumber,
            NULL);
    if (pSlapVariable)
    {
        pMyObject->V6DayOfWeekNextInsNum = pSlapVariable->Variant.varUint32;
        SlapFreeVariable(pSlapVariable);
    }

    for (ulFwV6DayOfWeekIdx = 0; ulFwV6DayOfWeekIdx < ulFwV6DayOfWeekCnt; ulFwV6DayOfWeekIdx++)
    {
        pFwV6DayOfWeek = AnscAllocateMemory(sizeof(COSA_DML_FW_V6_DAYOFWEEK));
        if (!pFwV6DayOfWeek) {
            return ANSC_STATUS_FAILURE;
        }
        if (CosaDmlFW_V6DayOfWeek_GetEntryByIndex(ulFwV6DayOfWeekIdx, pFwV6DayOfWeek) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("%s: CosaDmlFW_V6DayOfWeek_GetEntryByIndex error\n", __FUNCTION__));
            AnscFreeMemory(pFwV6DayOfWeek);
            return ANSC_STATUS_FAILURE;
        }

        pFwV6DayOfWeekLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
        if (!pFwV6DayOfWeekLinkObj)
        {
            AnscFreeMemory(pFwV6DayOfWeek);
            return ANSC_STATUS_FAILURE;
        }

        if (pFwV6DayOfWeek->InstanceNumber != 0)
        {
            if (pMyObject->V6DayOfWeekNextInsNum <= pFwV6DayOfWeek->InstanceNumber)
            {
                pMyObject->V6DayOfWeekNextInsNum = pFwV6DayOfWeek->InstanceNumber + 1;
                if (pMyObject->V6DayOfWeekNextInsNum == 0)
                {
                    pMyObject->V6DayOfWeekNextInsNum = 1;
                }
            }
        }
        else
        {
            pFwV6DayOfWeek->InstanceNumber = pMyObject->V6DayOfWeekNextInsNum;

            pMyObject->V6DayOfWeekNextInsNum++;
            if (pMyObject->V6DayOfWeekNextInsNum == 0)
            {
                pMyObject->V6DayOfWeekNextInsNum = 1;
            }

            _ansc_sprintf(pFwV6DayOfWeek->Alias, "cpe-V6DayOfWeek-%d", (int)pFwV6DayOfWeek->InstanceNumber);
            CosaDmlFW_V6DayOfWeek_SetValues(ulFwV6DayOfWeekIdx, pFwV6DayOfWeek->InstanceNumber, pFwV6DayOfWeek->Alias, pFwV6DayOfWeek->V6DayOfWeek_BlockTimeBitMask);
        }
        pFwV6DayOfWeekLinkObj->InstanceNumber  = pFwV6DayOfWeek->InstanceNumber;
        pFwV6DayOfWeekLinkObj->hContext        = pFwV6DayOfWeek;
        pFwV6DayOfWeekLinkObj->hParentTable    = NULL;
        pFwV6DayOfWeekLinkObj->bNew            = FALSE;

        CosaSListPushEntryByInsNum(&pMyObject->V6DayOfWeekList, pFwV6DayOfWeekLinkObj);
    }
    CosaFwReg_V6DayOfWeekGetInfo((ANSC_HANDLE)pMyObject);
// LGI ADD END

    AnscFreeMemory(pFirewallCfg);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaFirewallRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa Firewall object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaFirewallRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_FIREWALL        pMyObject    = (PCOSA_DATAMODEL_FIREWALL)hThisObject;

	/* Remove necessary resounce */


    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

	return returnStatus;
}


// LGI ADD START
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaFwReg_V4_IpFilterGetInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function get Info for V4 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V4_IpFilterGetInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_FIREWALL        pMyObject               = (PCOSA_DATAMODEL_FIREWALL)hThisObject;
    PSLIST_HEADER                   pListHead               = (PSLIST_HEADER            )&pMyObject->FwV4IpFilterList;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilter     = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwV4IpFiler;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilterSp   = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    COSA_DML_FW_IPFILTER           *pEntry                  = (COSA_DML_FW_IPFILTER    *)NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE           )NULL;
    ULONG                           ulEntryCount            = 0;
    ULONG                           ulIndex                 = 0;
    ULONG                           ulInstanceNumber        = 0;
    char*                           pFolderName             = NULL;
    char*                           pAlias                  = NULL;

    if ( !pPoamIrepFoIpFilter )
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Load the newly added but not yet commited entries */

    ulEntryCount = pPoamIrepFoIpFilter->GetFolderCount((ANSC_HANDLE)pPoamIrepFoIpFilter);

    for ( ulIndex = 0; ulIndex < ulEntryCount; ulIndex++ )
    {
        pFolderName =
            pPoamIrepFoIpFilter->EnumFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFilter,
                    ulIndex
                );

        if ( !pFolderName )
        {
            continue;
        }
        pPoamIrepFoIpFilterSp = pPoamIrepFoIpFilter->GetFolder((ANSC_HANDLE)pPoamIrepFoIpFilter, pFolderName);

        AnscFreeMemory(pFolderName);

        if ( !pPoamIrepFoIpFilterSp )
        {
            continue;
        }

        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoIpFilterSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_InsNum,
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
                (PSLAP_VARIABLE)pPoamIrepFoIpFilterSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_Alias,
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

            pPoamIrepFoIpFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoIpFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        pEntry = (COSA_DML_FW_IPFILTER*)AnscAllocateMemory(sizeof(COSA_DML_FW_IPFILTER));

        if ( !pEntry )
        {
            AnscFreeMemory(pAlias);
            AnscFreeMemory(pCosaContext);

            pPoamIrepFoIpFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoIpFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        AnscCopyString(pEntry->Alias, pAlias ? pAlias : "");

        pEntry->InstanceNumber = ulInstanceNumber;

        pCosaContext->InstanceNumber        = ulInstanceNumber;
        pCosaContext->bNew                  = TRUE;
        pCosaContext->hContext              = (ANSC_HANDLE)pEntry;
        pCosaContext->hParentTable          = NULL;
        pCosaContext->hPoamIrepUpperFo      = (ANSC_HANDLE)pPoamIrepFoIpFilter;
        pCosaContext->hPoamIrepFo           = (ANSC_HANDLE)pPoamIrepFoIpFilterSp;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);

        if ( pAlias )
        {
            AnscFreeMemory(pAlias);

            pAlias = NULL;
        }
    }


    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaFwReg_V4_IpFilterAddInfo
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCosaContext
            );

    description:

        This function add Info of V4 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.
                ANSC_HANDLE                 hCosaContext
                This handle is actually the pointer of instance object.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V4_IpFilterAddInfo
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_FIREWALL        pMyObject               = (PCOSA_DATAMODEL_FIREWALL)hThisObject;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFiler      = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwV4IpFiler;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilterSp   = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT)hCosaContext;
    COSA_DML_FW_IPFILTER           *pEntry                  = (COSA_DML_FW_IPFILTER *   )pCosaContext->hContext;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE           )NULL;

    if ( !pPoamIrepFoIpFiler )
    {
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        pPoamIrepFoIpFiler->EnableFileSync((ANSC_HANDLE)pPoamIrepFoIpFiler, FALSE);
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
            pPoamIrepFoIpFiler->DelRecord
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFiler,
                    COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NextInsNumber
                );

        pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
        pSlapVariable->Variant.varUint32 = pMyObject->FwV4IpFilterNextInsNum;

        returnStatus =
            pPoamIrepFoIpFiler->AddRecord
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFiler,
                    COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NextInsNumber,
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
        pPoamIrepFoIpFilterSp =
            pPoamIrepFoIpFiler->AddFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFiler,
                    pEntry->Alias,
                    0
                );

        if ( !pPoamIrepFoIpFilterSp )
        {
            returnStatus = ANSC_STATUS_FAILURE;

            goto  EXIT1;
        }

        if ( TRUE )
        {
            pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
            pSlapVariable->Variant.varUint32 = pEntry->InstanceNumber;

            returnStatus =
                pPoamIrepFoIpFilterSp->AddRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_InsNum,
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
                pPoamIrepFoIpFilterSp->AddRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_Alias,
                        SYS_REP_RECORD_TYPE_ASTR,
                        SYS_RECORD_CONTENT_DEFAULT,
                        pSlapVariable,
                        0
                    );

            SlapCleanVariable(pSlapVariable);
            SlapInitVariable (pSlapVariable);
        }

        pCosaContext->hPoamIrepUpperFo = (ANSC_HANDLE)pPoamIrepFoIpFiler;
        pCosaContext->hPoamIrepFo      = (ANSC_HANDLE)pPoamIrepFoIpFilterSp;
    }

EXIT1:

    if ( pSlapVariable )
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
        CosaFwReg_V4_IpFilterDelInfo
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCosaContext
            );

    description:

        This function delete Info of V4 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.
                ANSC_HANDLE                 hCosaContext
                This handle is actually the pointer of instance object.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V4_IpFilterDelInfo
    (
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

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaFwReg_V6_IpFilterGetInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function get Info for V6 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V6_IpFilterGetInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_FIREWALL        pMyObject               = (PCOSA_DATAMODEL_FIREWALL)hThisObject;
    PSLIST_HEADER                   pListHead               = (PSLIST_HEADER            )&pMyObject->FwV6IpFilterList;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilter     = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwV6IpFiler;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilterSp   = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    COSA_DML_FW_IPFILTER           *pEntry                  = (COSA_DML_FW_IPFILTER    *)NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE           )NULL;
    ULONG                           ulEntryCount            = 0;
    ULONG                           ulIndex                 = 0;
    ULONG                           ulInstanceNumber        = 0;
    char*                           pFolderName             = NULL;
    char*                           pAlias                  = NULL;

    if ( !pPoamIrepFoIpFilter )
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Load the newly added but not yet commited entries */

    ulEntryCount = pPoamIrepFoIpFilter->GetFolderCount((ANSC_HANDLE)pPoamIrepFoIpFilter);

    for ( ulIndex = 0; ulIndex < ulEntryCount; ulIndex++ )
    {
        pFolderName =
            pPoamIrepFoIpFilter->EnumFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFilter,
                    ulIndex
                );

        if ( !pFolderName )
        {
            continue;
        }
        pPoamIrepFoIpFilterSp = pPoamIrepFoIpFilter->GetFolder((ANSC_HANDLE)pPoamIrepFoIpFilter, pFolderName);

        AnscFreeMemory(pFolderName);

        if ( !pPoamIrepFoIpFilterSp )
        {
            continue;
        }

        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoIpFilterSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_InsNum,
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
                (PSLAP_VARIABLE)pPoamIrepFoIpFilterSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_Alias,
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

            pPoamIrepFoIpFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoIpFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        pEntry = (COSA_DML_FW_IPFILTER*)AnscAllocateMemory(sizeof(COSA_DML_FW_IPFILTER));

        if ( !pEntry )
        {
            AnscFreeMemory(pAlias);
            AnscFreeMemory(pCosaContext);

            pPoamIrepFoIpFilterSp->Remove((ANSC_HANDLE)pPoamIrepFoIpFilterSp);
            return ANSC_STATUS_RESOURCES;
        }

        AnscCopyString(pEntry->Alias, pAlias ? pAlias : "");

        pEntry->InstanceNumber = ulInstanceNumber;

        pCosaContext->InstanceNumber        = ulInstanceNumber;
        pCosaContext->bNew                  = TRUE;
        pCosaContext->hContext              = (ANSC_HANDLE)pEntry;
        pCosaContext->hParentTable          = NULL;
        pCosaContext->hPoamIrepUpperFo      = (ANSC_HANDLE)pPoamIrepFoIpFilter;
        pCosaContext->hPoamIrepFo           = (ANSC_HANDLE)pPoamIrepFoIpFilterSp;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);

        if ( pAlias )
        {
            AnscFreeMemory(pAlias);

            pAlias = NULL;
        }
    }


    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaFwReg_V6_IpFilterAddInfo
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCosaContext
            );

    description:

        This function add Info of V6 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.
                ANSC_HANDLE                 hCosaContext
                This handle is actually the pointer of instance object.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V6_IpFilterAddInfo
    (
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    )
{
    ANSC_STATUS                     returnStatus            = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_FIREWALL        pMyObject               = (PCOSA_DATAMODEL_FIREWALL )hThisObject;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFiler      = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwV6IpFiler;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoIpFilterSp   = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext            = (PCOSA_CONTEXT_LINK_OBJECT)hCosaContext;
    COSA_DML_FW_IPFILTER            *pEntry                 = (COSA_DML_FW_IPFILTER *   )pCosaContext->hContext;
    PSLAP_VARIABLE                  pSlapVariable           = (PSLAP_VARIABLE           )NULL;

    if ( !pPoamIrepFoIpFiler )
    {
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        pPoamIrepFoIpFiler->EnableFileSync((ANSC_HANDLE)pPoamIrepFoIpFiler, FALSE);
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
            pPoamIrepFoIpFiler->DelRecord
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFiler,
                    COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NextInsNumber
                );

        pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
        pSlapVariable->Variant.varUint32 = pMyObject->FwV6IpFilterNextInsNum;

        returnStatus =
            pPoamIrepFoIpFiler->AddRecord
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFiler,
                    COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NextInsNumber,
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
        pPoamIrepFoIpFilterSp =
            pPoamIrepFoIpFiler->AddFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoIpFiler,
                    pEntry->Alias,
                    0
                );

        if ( !pPoamIrepFoIpFilterSp )
        {
            returnStatus = ANSC_STATUS_FAILURE;

            goto  EXIT1;
        }

        if ( TRUE )
        {
            pSlapVariable->Syntax            = SLAP_VAR_SYNTAX_uint32;
            pSlapVariable->Variant.varUint32 = pEntry->InstanceNumber;

            returnStatus =
                pPoamIrepFoIpFilterSp->AddRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_InsNum,
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
                pPoamIrepFoIpFilterSp->AddRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoIpFilterSp,
                        COSA_DML_RR_NAME_IP_FILTER_FIREWALL_Alias,
                        SYS_REP_RECORD_TYPE_ASTR,
                        SYS_RECORD_CONTENT_DEFAULT,
                        pSlapVariable,
                        0
                    );

            SlapCleanVariable(pSlapVariable);
            SlapInitVariable (pSlapVariable);
        }

        pCosaContext->hPoamIrepUpperFo = (ANSC_HANDLE)pPoamIrepFoIpFiler;
        pCosaContext->hPoamIrepFo      = (ANSC_HANDLE)pPoamIrepFoIpFilterSp;
    }

EXIT1:

    if ( pSlapVariable )
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
        CosaFwReg_V6_IpFilterDelInfo
            (
                ANSC_HANDLE                 hThisObject,
                ANSC_HANDLE                 hCosaContext
            );

    description:

        This function delete Info of V6 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject,
                This handle is actually the pointer of this object
                itself.
                ANSC_HANDLE                 hCosaContext
                This handle is actually the pointer of instance object.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V6_IpFilterDelInfo
    (
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

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaFwReg_V4DayOfWeekGetInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function get Info for DayOfWeek entries of V4 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V4DayOfWeekGetInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_FIREWALL        pMyObject                   = (PCOSA_DATAMODEL_FIREWALL )hThisObject;
    PSLIST_HEADER                   pListHead                   = (PSLIST_HEADER            )&pMyObject->V4DayOfWeekList;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoV4DayOfWeek      = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwV4DayOfWeek;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoV4DayOfWeekSp    = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    COSA_DML_FW_V4_DAYOFWEEK        *pEntry                     = (COSA_DML_FW_V4_DAYOFWEEK *)NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext                = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSLAP_VARIABLE                  pSlapVariable               = (PSLAP_VARIABLE           )NULL;
    ULONG                           ulEntryCount                = 0;
    ULONG                           ulIndex                     = 0;
    ULONG                           ulInstanceNumber            = 0;
    char*                           pFolderName                 = NULL;
    char*                           pAlias                      = NULL;

    if ( !pPoamIrepFoV4DayOfWeek )
    {
        return ANSC_STATUS_FAILURE;
    }
    /* Load the newly added but not yet commited entries */

    ulEntryCount = pPoamIrepFoV4DayOfWeek->GetFolderCount((ANSC_HANDLE)pPoamIrepFoV4DayOfWeek);

    for ( ulIndex = 0; ulIndex < ulEntryCount; ulIndex++ )
    {
        pFolderName =
            pPoamIrepFoV4DayOfWeek->EnumFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoV4DayOfWeek,
                    ulIndex
                );
        if ( !pFolderName )
        {
            continue;
        }
        pPoamIrepFoV4DayOfWeekSp = pPoamIrepFoV4DayOfWeek->GetFolder((ANSC_HANDLE)pPoamIrepFoV4DayOfWeek, pFolderName);

        AnscFreeMemory(pFolderName);

        if ( !pPoamIrepFoV4DayOfWeekSp )
        {
            continue;
        }
        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoV4DayOfWeekSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoV4DayOfWeekSp,
                        COSA_DML_RR_NAME_V4DayOfWeekNextInsNumber,
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
                (PSLAP_VARIABLE)pPoamIrepFoV4DayOfWeekSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoV4DayOfWeekSp,
                        COSA_DML_RR_NAME_V4DayOfWeekAlias,
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

        pEntry = (COSA_DML_FW_V4_DAYOFWEEK*)AnscAllocateMemory(sizeof(COSA_DML_FW_V4_DAYOFWEEK));

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
        pCosaContext->hPoamIrepUpperFo      = (ANSC_HANDLE)pPoamIrepFoV4DayOfWeek;
        pCosaContext->hPoamIrepFo           = (ANSC_HANDLE)pPoamIrepFoV4DayOfWeekSp;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);
        if ( pAlias )
        {
            AnscFreeMemory(pAlias);

            pAlias = NULL;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaFwReg_V6DayOfWeekGetInfo
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function get Info for DayOfWeek entries of V6 IP Filter.

    argument:   ANSC_HANDLE                 hThisObject
                This handle is actually the pointer of this object
                itself.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaFwReg_V6DayOfWeekGetInfo
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DATAMODEL_FIREWALL        pMyObject                   = (PCOSA_DATAMODEL_FIREWALL )hThisObject;
    PSLIST_HEADER                   pListHead                   = (PSLIST_HEADER            )&pMyObject->V6DayOfWeekList;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoV6DayOfWeek      = (PPOAM_IREP_FOLDER_OBJECT )pMyObject->hIrepFolderFwV6DayOfWeek;
    PPOAM_IREP_FOLDER_OBJECT        pPoamIrepFoV6DayOfWeekSp    = (PPOAM_IREP_FOLDER_OBJECT )NULL;
    COSA_DML_FW_V6_DAYOFWEEK        *pEntry                     = (COSA_DML_FW_V6_DAYOFWEEK *)NULL;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext                = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSLAP_VARIABLE                  pSlapVariable               = (PSLAP_VARIABLE           )NULL;
    ULONG                           ulEntryCount                = 0;
    ULONG                           ulIndex                     = 0;
    ULONG                           ulInstanceNumber            = 0;
    char*                           pFolderName                 = NULL;
    char*                           pAlias                      = NULL;

    if ( !pPoamIrepFoV6DayOfWeek )
    {
        return ANSC_STATUS_FAILURE;
    }
    /* Load the newly added but not yet commited entries */

    ulEntryCount = pPoamIrepFoV6DayOfWeek->GetFolderCount((ANSC_HANDLE)pPoamIrepFoV6DayOfWeek);

    for ( ulIndex = 0; ulIndex < ulEntryCount; ulIndex++ )
    {
        pFolderName =
            pPoamIrepFoV6DayOfWeek->EnumFolder
                (
                    (ANSC_HANDLE)pPoamIrepFoV6DayOfWeek,
                    ulIndex
                );
        if ( !pFolderName )
        {
            continue;
        }
        pPoamIrepFoV6DayOfWeekSp = pPoamIrepFoV6DayOfWeek->GetFolder((ANSC_HANDLE)pPoamIrepFoV6DayOfWeek, pFolderName);

        AnscFreeMemory(pFolderName);

        if ( !pPoamIrepFoV6DayOfWeekSp )
        {
            continue;
        }
        if ( TRUE )
        {
            pSlapVariable =
                (PSLAP_VARIABLE)pPoamIrepFoV6DayOfWeekSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoV6DayOfWeekSp,
                        COSA_DML_RR_NAME_V6DayOfWeekNextInsNumber,
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
                (PSLAP_VARIABLE)pPoamIrepFoV6DayOfWeekSp->GetRecord
                    (
                        (ANSC_HANDLE)pPoamIrepFoV6DayOfWeekSp,
                        COSA_DML_RR_NAME_V6DayOfWeekAlias,
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

        pEntry = (COSA_DML_FW_V6_DAYOFWEEK*)AnscAllocateMemory(sizeof(COSA_DML_FW_V6_DAYOFWEEK));

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
        pCosaContext->hPoamIrepUpperFo      = (ANSC_HANDLE)pPoamIrepFoV6DayOfWeek;
        pCosaContext->hPoamIrepFo           = (ANSC_HANDLE)pPoamIrepFoV6DayOfWeekSp;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);
        if ( pAlias )
        {
            AnscFreeMemory(pAlias);

            pAlias = NULL;
        }
    }
    return ANSC_STATUS_SUCCESS;
}
//LGI ADD END

