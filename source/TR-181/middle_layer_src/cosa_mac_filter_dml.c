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

#include <ctype.h>
#include "ansc_platform.h"
#include "cosa_mac_filter_apis.h"
#include "cosa_mac_filter_dml.h"
#include "cosa_mac_filter_internal.h"
#include "syscfg/syscfg.h"
/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply
 the changes to all of the specified Parameters atomically. That is, either
 all of the value changes are applied together, or none of the changes are
 applied at all. In the latter case, the CPE MUST return a fault response
 indicating the reason for the failure to apply the changes.

 The CPE MUST NOT apply any of the specified changes without applying all
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }

***********************************************************************/
#define BTMASK_ALWAYS               0
#define DAYOFWEEK_BT_MASK_LEN       25

ULONG
MACFilter_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    COSA_DATAMODEL_MAC_FILTER  *pMacFilter = (COSA_DATAMODEL_MAC_FILTER*)g_pCosaBEManager->hMacFilter;
    return AnscSListQueryDepth(&pMacFilter->MACFilterList);
}

ANSC_HANDLE
MACFilter_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    COSA_DATAMODEL_MAC_FILTER       *pMacFilter = (COSA_DATAMODEL_MAC_FILTER*)g_pCosaBEManager->hMacFilter;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry = NULL;

    pSLinkEntry = AnscQueueGetEntryByIndex((ANSC_HANDLE)&pMacFilter->MACFilterList, nIndex);
    if (pSLinkEntry)
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pLinkObj->InstanceNumber;
    }

    return pLinkObj;
}

ANSC_HANDLE
MACFilter_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    COSA_DATAMODEL_MAC_FILTER      *pMacFilter = (COSA_DATAMODEL_MAC_FILTER*)g_pCosaBEManager->hMacFilter;
    PCOSA_CONTEXT_LINK_OBJECT      pLinkObj    = NULL;
    COSA_DML_FW_MACFILTER          *pFwMACFilter = NULL;

    pLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
    if (!pLinkObj)
        return NULL;
    pFwMACFilter = AnscAllocateMemory(sizeof(COSA_DML_FW_MACFILTER));
    if (!pFwMACFilter)
    {
        AnscFreeMemory(pLinkObj);
        return NULL;
    }
     /* now we have this link content */
    pLinkObj->InstanceNumber = pMacFilter->MACFilterNextInsNum;
    pFwMACFilter->InstanceNumber = pMacFilter->MACFilterNextInsNum;
    pMacFilter->MACFilterNextInsNum++;
    if (pMacFilter->MACFilterNextInsNum == 0)
        pMacFilter->MACFilterNextInsNum = 1;
    _ansc_sprintf(pFwMACFilter->Alias, "MACFilter-%d", (int)pLinkObj->InstanceNumber);
    pLinkObj->hContext      = (ANSC_HANDLE)pFwMACFilter;
    pLinkObj->hParentTable  = NULL;
    pLinkObj->bNew          = TRUE;

    CosaSListPushEntryByInsNum((PSLIST_HEADER)&pMacFilter->MACFilterList, pLinkObj);
    CosaFwReg_MACFilterAddInfo((ANSC_HANDLE)pMacFilter, (ANSC_HANDLE)pLinkObj);

    *pInsNumber = pLinkObj->InstanceNumber;
    return pLinkObj;
}

ULONG
MACFilter_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                    returnStatus   = ANSC_STATUS_SUCCESS;
    COSA_DATAMODEL_MAC_FILTER      *pMacFilter    = (COSA_DATAMODEL_MAC_FILTER*)g_pCosaBEManager->hMacFilter;
    PCOSA_CONTEXT_LINK_OBJECT      pLinkObj       = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    COSA_DML_FW_MACFILTER          *pFwMACFilter  = (COSA_DML_FW_MACFILTER*)pLinkObj->hContext;

    if (pLinkObj->bNew)
    {
        /* Set bNew to FALSE to indicate this node is not going to save to SysRegistry */
        pLinkObj->bNew = FALSE;
        returnStatus = CosaFwReg_MACFilterDelInfo((ANSC_HANDLE)pMacFilter, (ANSC_HANDLE)pLinkObj);
    }
    else
    {
        returnStatus = CosaDmlFW_MACFilter_DelEntry(pLinkObj->InstanceNumber);
    }

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscSListPopEntryByLink((PSLIST_HEADER)&pMacFilter->MACFilterList, &pLinkObj->Linkage);
        AnscFreeMemory(pFwMACFilter);
        AnscFreeMemory(pLinkObj);
    }
    return returnStatus;

}

BOOL
MACFilter_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MACFILTER          *pFwMACFilter  = (COSA_DML_FW_MACFILTER*)pLinkObj->hContext;

    if (strcmp(ParamName, "Enable") == 0)
    {
        *pBool = pFwMACFilter->Enable;
        return TRUE;
    }
    return FALSE;
}

ULONG
MACFilter_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj       = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MACFILTER           *pFwMACFilter  = (COSA_DML_FW_MACFILTER*)pLinkObj->hContext;

    if (strcmp(ParamName, "Hostname") == 0)
    {
        AnscCopyString(pValue, pFwMACFilter->Hostname);
        return 0;
    }
    if (strcmp(ParamName, "MACAddress") == 0)
    {
        AnscCopyString(pValue, pFwMACFilter->MACAddress);
        return 0;
    }
    return -1;
}

BOOL
MACFilter_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MACFILTER          *pFwMACFilter  = (COSA_DML_FW_MACFILTER*)pLinkObj->hContext;

    if (strcmp(ParamName, "Enable") == 0)
    {
        pFwMACFilter->Enable = pBool;
        return TRUE;
    }
    return FALSE;
}

BOOL
MACFilter_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MACFILTER           *pFwMACFilter     = (COSA_DML_FW_MACFILTER*)pLinkObj->hContext;

    if (strcmp(ParamName, "Hostname") == 0)
    {
        /* Return error for SPV if size is more than expected, currently max limit is sizeof(pFwMACFilter->Hostname)-1 */
        if (strlen (strValue) < sizeof(pFwMACFilter->Hostname))
        {
            _ansc_sprintf(pFwMACFilter->Hostname, "%s", strValue);
            return TRUE;
        }
    }
    if (strcmp(ParamName, "MACAddress") == 0)
    {
        _ansc_snprintf(pFwMACFilter->MACAddress, sizeof(pFwMACFilter->MACAddress), "%s", strValue);
        return TRUE;
    }
    return FALSE;
}

BOOL
MACFilter_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MACFILTER          *pFwMACFilter    = (COSA_DML_FW_MACFILTER*)pLinkObj->hContext;
    PCOSA_DATAMODEL_MAC_FILTER      pDmlMacFilter   = (PCOSA_DATAMODEL_MAC_FILTER)g_pCosaBEManager->hMacFilter;
    PSLIST_HEADER                   pMaclisthd      = (PSLIST_HEADER)&pDmlMacFilter->MACFilterList;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = AnscSListGetFirstEntry(pMaclisthd);

    //walk through the DML structure to find any duplicates
    while (pSLinkEntry)
    {
        PCOSA_CONTEXT_LINK_OBJECT pobj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        COSA_DML_FW_MACFILTER *pFilter = (COSA_DML_FW_MACFILTER *)(pobj->hContext);

        if (strcasecmp(pFilter->MACAddress, pFwMACFilter->MACAddress) == 0)
        {
            if (pFilter == pFwMACFilter)
            {
                pSLinkEntry = AnscSListGetNextEntry(pSLinkEntry);
                continue;
            }

            // clear macaddress of new dml entry, to avoid displaying in dmcli
            pFwMACFilter->MACAddress[0] = 0;
            _ansc_strcpy(pReturnParamName, "MACAddress");
            return FALSE;
        }

        pSLinkEntry = AnscSListGetNextEntry(pSLinkEntry);
    }

    return TRUE;
}

ULONG
MACFilter_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MACFILTER           *pFwMACFilter   = (COSA_DML_FW_MACFILTER*)pLinkObj->hContext;
    PCOSA_DATAMODEL_MAC_FILTER      pMacFilter      = (PCOSA_DATAMODEL_MAC_FILTER)g_pCosaBEManager->hMacFilter;

    if (pLinkObj->bNew)
    {
        if (CosaDmlFW_MACFilter_AddEntry(pFwMACFilter) != ANSC_STATUS_SUCCESS)
            return -1;
        CosaFwReg_MACFilterDelInfo((ANSC_HANDLE)pMacFilter, (ANSC_HANDLE)pLinkObj);
        pLinkObj->bNew = FALSE;
    }
    else
    {
        if (CosaDmlFW_MACFilter_SetConf(pFwMACFilter->InstanceNumber, pFwMACFilter) != ANSC_STATUS_SUCCESS)
        {
            CosaDmlFW_MACFilter_GetConf(pFwMACFilter->InstanceNumber, pFwMACFilter);
            return -1;
        }
    }
    return 0;
}

ULONG
MACFilter_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj         = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MACFILTER           *pFwMACFilter    = (COSA_DML_FW_MACFILTER *)pLinkObj->hContext;

    if (CosaDmlFW_MACFilter_GetConf(pFwMACFilter->InstanceNumber, pFwMACFilter) != ANSC_STATUS_SUCCESS)
        return -1;

    return 0;
}

BOOL
MacFltTimeMask_GetParamUlongValue 
    (
        ANSC_HANDLE hInsContext,
        char*       ParamName,
        ULONG*      puLong
    )
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "ScheduleEnable") == 0) {
        if(ANSC_STATUS_SUCCESS == CosaDmlFW_MACDayOfWeek_GetBlockTimeBitMaskType(puLong)) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL
MacFltTimeMask_SetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char*       ParamName,
        ULONG       ulValue
    )
{
    PCOSA_DATAMODEL_MAC_FILTER      pMacFilter      = (PCOSA_DATAMODEL_MAC_FILTER)g_pCosaBEManager->hMacFilter;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "ScheduleEnable") == 0) {
        if(ANSC_STATUS_SUCCESS != CosaDmlFW_MACDayOfWeek_SetBlockTimeBitMaskType(ulValue)) {
            return FALSE;
        }
        pMacFilter->MacDayOfWeekBlockTimeBitMaskType = ulValue;
    }
    return TRUE;
}

ULONG
MacFilter_DayOfWeek_GetEntryCount
    (
         ANSC_HANDLE                 hInsContext
    )
{
    COSA_DATAMODEL_MAC_FILTER  *pMacFilter = (COSA_DATAMODEL_MAC_FILTER*)g_pCosaBEManager->hMacFilter;
    return AnscSListQueryDepth(&pMacFilter->MacDayOfWeekList);
}

ANSC_HANDLE
MacFilter_DayOfWeek_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    COSA_DATAMODEL_MAC_FILTER       *pMacFilter = (COSA_DATAMODEL_MAC_FILTER*)g_pCosaBEManager->hMacFilter;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry = NULL;

    pSLinkEntry = AnscQueueGetEntryByIndex((ANSC_HANDLE)&pMacFilter->MacDayOfWeekList, nIndex);
    if (pSLinkEntry)
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pLinkObj->InstanceNumber;
    }
    return pLinkObj;
}

ULONG
MacFilter_DayOfWeek_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MAC_DAYOFWEEK       *pMacDayOfWeek  = (COSA_DML_FW_MAC_DAYOFWEEK*)pLinkObj->hContext;

    if (strcmp(ParamName, "BlockTimeBitMask") == 0) {
        AnscCopyString(pValue, pMacDayOfWeek->MacDayOfWeek_BlockTimeBitMask);
        return 0;
    }
    return 1;
}

BOOL
MacFilter_DayOfWeek_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MAC_DAYOFWEEK       *pMacDayOfWeek  = (COSA_DML_FW_MAC_DAYOFWEEK*)pLinkObj->hContext;
    int iMaskLen = 0;
    int i = 0;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "BlockTimeBitMask") == 0)
    {
        // Ensure Mask Len is 24 characters
        iMaskLen = strlen(strValue);
        if(iMaskLen != (DAYOFWEEK_BT_MASK_LEN - 1))
        {
            return FALSE;
        }
        // Ensure Mask is a valid Binary String
        for(i = 0; i < iMaskLen; i++)
        {
            if((int)strValue[i] < 48 || (int)strValue[i] > 49)
            {
                return FALSE; // Not a Binary character
            }
        }
        _ansc_snprintf(pMacDayOfWeek->MacDayOfWeek_BlockTimeBitMask, sizeof(pMacDayOfWeek->MacDayOfWeek_BlockTimeBitMask), "%s", strValue);
        return TRUE;
    }
    return FALSE;
}

BOOL
MacFilter_DayOfWeek_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_DATAMODEL_MAC_FILTER      pMacFilter      = (PCOSA_DATAMODEL_MAC_FILTER)g_pCosaBEManager->hMacFilter;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MAC_DAYOFWEEK       *pMacDayOfWeek  = (COSA_DML_FW_MAC_DAYOFWEEK*)pLinkObj->hContext;

    int iMaskLen = 0;
    int i = 0;

    if(BTMASK_ALWAYS != pMacFilter->MacDayOfWeekBlockTimeBitMaskType)
    {
        // Ensure Mask Len is 24 characters
        iMaskLen = strlen(pMacDayOfWeek->MacDayOfWeek_BlockTimeBitMask);
        if(iMaskLen != (DAYOFWEEK_BT_MASK_LEN - 1))
        {
            return FALSE;
        }
        // Ensure Mask is a valid Binary String
        for(i = 0; i < iMaskLen; i++)
        {
            if((int)pMacDayOfWeek->MacDayOfWeek_BlockTimeBitMask[i] < 48 || (int)pMacDayOfWeek->MacDayOfWeek_BlockTimeBitMask[i] > 49)
            {
                return FALSE; // Not a Binary character
            }
        }
    }

    return TRUE;
}

ULONG
MacFilter_DayOfWeek_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MAC_DAYOFWEEK       *pMacDayOfWeek  = (COSA_DML_FW_MAC_DAYOFWEEK*)pLinkObj->hContext;

    if (CosaDmlFW_MacDayOfWeek_SetConf(pMacDayOfWeek->InstanceNumber, pMacDayOfWeek) != ANSC_STATUS_SUCCESS)
    {
        CosaDmlFW_MacDayOfWeek_GetConf(pMacDayOfWeek->InstanceNumber, pMacDayOfWeek);
        return -1;
    }

    return 0;
}

ULONG
MacFilter_DayOfWeek_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_MAC_DAYOFWEEK       *pMacDayOfWeek   = (COSA_DML_FW_MAC_DAYOFWEEK*)pLinkObj->hContext;

    if (CosaDmlFW_MacDayOfWeek_GetConf(pMacDayOfWeek->InstanceNumber, pMacDayOfWeek) != ANSC_STATUS_SUCCESS)
        return -1;

    return 0;
}