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

    module: cosa_x_cisco_com_ddns_dml.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/14/2011    initial revision.

**************************************************************************/

#include "ansc_platform.h"
#include "cosa_x_cisco_com_ddns_dml.h"
#include "cosa_x_cisco_com_ddns_internal.h"
#include "dml_tr181_custom_cfg.h"

extern void * g_pDslhDmlAgent;

struct detail
{
    char word[ 256 ];
};

BOOL Service_IsDomainStringHaveRepeatedWord ( char* pStringDomain );

int Service_CheckRepeatString( struct detail stDetailArray[], const char unit[], int count, int* pIsHaveRepeatedWord );


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
/***********************************************************************

 APIs for Object:

    X_CISCO_COM_DDNS.

    *  X_CISCO_COM_DDNS_GetParamBoolValue
    *  X_CISCO_COM_DDNS_GetParamIntValue
    *  X_CISCO_COM_DDNS_GetParamUlongValue
    *  X_CISCO_COM_DDNS_GetParamStringValue
    *  X_CISCO_COM_DDNS_SetParamBoolValue
    *  X_CISCO_COM_DDNS_SetParamIntValue
    *  X_CISCO_COM_DDNS_SetParamUlongValue
    *  X_CISCO_COM_DDNS_SetParamStringValue
    *  X_CISCO_COM_DDNS_Validate
    *  X_CISCO_COM_DDNS_Commit
    *  X_CISCO_COM_DDNS_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns     = (PCOSA_DATAMODEL_DDNS)g_pCosaBEManager->hDdns;
    PCOSA_DML_DDNS_CFG              pDdnsCfg        = (PCOSA_DML_DDNS_CFG  )&pCosaDMDdns->DdnsConfig;
    COSA_DML_DDNS_CFG               DdnsCfg;

     _ansc_memset(&DdnsCfg, 0, sizeof(COSA_DML_DDNS_CFG));

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        CosaDmlDdnsGetConfig(NULL, &DdnsCfg);
        /* *pBool = pDdnsCfg->bEnabled; */
        *pBool = DdnsCfg.bEnabled; 

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_DDNS_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
X_CISCO_COM_DDNS_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns = (PCOSA_DATAMODEL_DDNS)g_pCosaBEManager->hDdns;
    PCOSA_DML_DDNS_CFG              pDdnsCfg    = (PCOSA_DML_DDNS_CFG  )&pCosaDMDdns->DdnsConfig;

    if ( !pDdnsCfg )
    {
        CcspTraceWarning(("X_CISCO_COM_DDNS_SetParamBoolValue -- pDdnsCfg is NULL\n"));

        return FALSE;
    }

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        pDdnsCfg->bEnabled = bValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_DDNS_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
X_CISCO_COM_DDNS_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_DDNS_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
X_CISCO_COM_DDNS_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns = (PCOSA_DATAMODEL_DDNS)g_pCosaBEManager->hDdns;
    PCOSA_DML_DDNS_CFG              pDdnsCfg    = (PCOSA_DML_DDNS_CFG  )&pCosaDMDdns->DdnsConfig;

    if ( !pDdnsCfg )
    {
        CcspTraceWarning(("X_CISCO_COM_DDNS_SetParamBoolValue -- pDdnsCfg is NULL\n"));

        return ANSC_STATUS_FAILURE;
    }

    return CosaDmlDdnsSetConfig(NULL, pDdnsCfg);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_DDNS_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
X_CISCO_COM_DDNS_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/***********************************************************************

 APIs for Object:

    X_CISCO_COM_DDNS.Service.{i}.

    *  Service_GetEntryCount
    *  Service_GetEntry
    *  Service_AddEntry
    *  Service_DelEntry
    *  Service_GetParamBoolValue
    *  Service_GetParamIntValue
    *  Service_GetParamUlongValue
    *  Service_GetParamStringValue
    *  Service_SetParamBoolValue
    *  Service_SetParamIntValue
    *  Service_SetParamUlongValue
    *  Service_SetParamStringValue
    *  Service_Validate
    *  Service_Commit
    *  Service_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Service_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
Service_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns  = (PCOSA_DATAMODEL_DDNS)g_pCosaBEManager->hDdns;
    
    return AnscSListQueryDepth(&pCosaDMDdns->ContextHead);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Service_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
Service_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns  = (PCOSA_DATAMODEL_DDNS     )g_pCosaBEManager->hDdns;
    PSLIST_HEADER                   pListHead    = (PSLIST_HEADER            )&pCosaDMDdns->ContextHead;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )NULL;
    PCOSA_DML_DDNS_SERVICE          pDdnsEntry   = (PCOSA_DML_DDNS_SERVICE   )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry  = (PSINGLE_LINK_ENTRY       )NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(&pCosaDMDdns->ContextHead, nIndex);

    if ( pSLinkEntry )
    {
        pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);

        *pInsNumber = pCosaContext->InstanceNumber;
    }

    return pCosaContext; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Service_AddEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to add a new entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle of new added entry.

**********************************************************************/
ANSC_HANDLE
Service_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns  = (PCOSA_DATAMODEL_DDNS     )g_pCosaBEManager->hDdns;
    PSLIST_HEADER                   pListHead    = (PSLIST_HEADER            )&pCosaDMDdns->ContextHead;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )NULL;

    pDdnsService = (PCOSA_DML_DDNS_SERVICE)AnscAllocateMemory(sizeof(COSA_DML_DDNS_SERVICE));

    if ( !pDdnsService )
    {
        return NULL;
    }

    _ansc_sprintf(pDdnsService->Alias, "DdnsService%d", pCosaDMDdns->ulNextInstance);

    /* Update middle layer cache */
    if ( TRUE )
    {
        pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));

        if ( !pCosaContext )
        {
            AnscFreeMemory(pDdnsService);

            return NULL;
        }

        pCosaContext->InstanceNumber = pDdnsService->InstanceNumber = pCosaDMDdns->ulNextInstance;

        pCosaDMDdns->ulNextInstance++;

        if ( pCosaDMDdns->ulNextInstance == 0 )
        {
            pCosaDMDdns->ulNextInstance = 1;
        }

        pCosaContext->hContext     = (ANSC_HANDLE)pDdnsService;
        pCosaContext->hParentTable = NULL;
        pCosaContext->bNew         = TRUE;

        CosaSListPushEntryByInsNum(pListHead, pCosaContext);

        CosaDdnsRegAddDdnsInfo((ANSC_HANDLE)pCosaDMDdns, (ANSC_HANDLE)pCosaContext);
    }

    *pInsNumber = pCosaContext->InstanceNumber;

    return pCosaContext; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Service_DelEntry
            (
                ANSC_HANDLE                 hInsContext,
                ANSC_HANDLE                 hInstance
            );

    description:

        This function is called to delete an exist entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ANSC_HANDLE                 hInstance
                The exist entry handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Service_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus  = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext  = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_DDNS_SERVICE          pDdnsService  = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns   = (PCOSA_DATAMODEL_DDNS     )g_pCosaBEManager->hDdns;
    PSLIST_HEADER                   pListHead     = (PSLIST_HEADER            )&pCosaDMDdns->ContextHead;

    if ( pCosaContext->bNew )
    {
        returnStatus = CosaDdnsRegDelDdnsInfo((ANSC_HANDLE)pCosaDMDdns, (ANSC_HANDLE)pCosaContext);
    }
    else
    {
        returnStatus = CosaDmlDdnsDelService(NULL, pDdnsService->InstanceNumber);

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            AnscSListPopEntryByLink(pListHead, &pCosaContext->Linkage);

            AnscFreeMemory(pDdnsService);
            AnscFreeMemory(pCosaContext);
        }
    }

    return returnStatus;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Service_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;
    COSA_DML_DDNS_SERVICE           DdnsService;

    _ansc_memset(&DdnsService, 0, sizeof(COSA_DML_DDNS_SERVICE));

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        CosaDmlDdnsGetInfo(NULL, pDdnsService->InstanceNumber, &DdnsService);
        /* *pBool = pDdnsService->bEnabled; */
        *pBool = DdnsService.bEnabled;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Wildcard", TRUE))
    {
        *pBool = pDdnsService->Wildcard;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MXHostAsSecondary", TRUE))
    {
        *pBool = pDdnsService->Backup_mx;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Service_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Service_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;
    COSA_DML_DDNS_SERVICE           DdnsService;

    _ansc_memset(&DdnsService, 0, sizeof(COSA_DML_DDNS_SERVICE));
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "ConnectionState", TRUE))
    {
        /* collect value */
        CosaDmlDdnsGetInfo(NULL, pDdnsService->InstanceNumber, &DdnsService);
        /* *puLong = pDdnsService->ConnectionState; */
        *puLong = DdnsService.ConnectionState; 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "ServiceName", TRUE))
    {
        /* collect value */
        if ( AnscEqualString(pDdnsService->ServiceName, "dyndns", FALSE) )
        {
            *puLong = 1;
        }
        else if ( AnscEqualString(pDdnsService->ServiceName, "tzo", FALSE) )
        {
            *puLong = 2;
        }
		else if ( AnscEqualString(pDdnsService->ServiceName, "changeip", FALSE) )
        {
            *puLong = 3;
        }
		else if ( AnscEqualString(pDdnsService->ServiceName, "afraid", FALSE) )
        {
            *puLong = 4;
        }
        else
        {
            *puLong = 5;
        }

        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Service_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Service_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;
    PUCHAR                          pLowerLayer      = NULL;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "AssociatedConnection", TRUE))
    {
        /* collect value */
        /* AnscCopyString(pValue, pDdnsService->AssociatedConnection); */
        pLowerLayer = CosaUtilGetLowerLayers("Device.IP.Interface.", CFG_TR181_Ddns_IfName);
        if ( pLowerLayer != NULL )
        {
            AnscCopyString(pValue, pLowerLayer);
            AnscFreeMemory(pLowerLayer);
        }
        return 0;
    }

    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDdnsService->Alias);

        return 0;
    }

    if( AnscEqualString(ParamName, "Domain", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDdnsService->Domain);

        return 0;
    }

    if( AnscEqualString(ParamName, "Password", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDdnsService->Password);

        return 0;
    }

    if( AnscEqualString(ParamName, "Username", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDdnsService->Username);

        return 0;
    }

    if( AnscEqualString(ParamName, "MXHostName", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDdnsService->Mail_exch);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Service_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        pDdnsService->bEnabled = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Wildcard", TRUE))
    {
        pDdnsService->Wildcard = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MXHostAsSecondary", TRUE))
    {
        pDdnsService->Backup_mx = bValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Service_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Service_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    /* check the parameter name and set the corresponding value */
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;
	
	if( AnscEqualString(ParamName, "ServiceName", TRUE))
    {
        /* collect value */
        if ( uValue == 1 )
        {
			AnscCopyString(pDdnsService->ServiceName, "dyndns");
        }
        else if ( uValue == 2 )
        {
			AnscCopyString(pDdnsService->ServiceName, "tzo");
        }
		else if ( uValue == 3 )
        {
			AnscCopyString(pDdnsService->ServiceName, "changeip");
        }
		else if ( uValue == 4 )
        {
			AnscCopyString(pDdnsService->ServiceName, "afraid");
        }
        else
        {
			//AnscCopyString(pDdnsService->ServiceName, "None");
        }

        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Service_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "AssociatedConnection", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDdnsService->AssociatedConnection, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDdnsService->Alias, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Domain", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDdnsService->Domain, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Password", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDdnsService->Password, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Username", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDdnsService->Username, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MXHostName", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDdnsService->Mail_exch, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/* Service_IsDomainStringHaveRepeatedWord() */
BOOL Service_IsDomainStringHaveRepeatedWord ( char* pStringDomain )
{
	struct detail stDetailArray[ 64 ] = { 0 };
	char   		  acSubstring[ 256 ]  = { 0 };
	int    		  i = 0, j= 0, count = 0;

	memset( &stDetailArray, 0, sizeof( stDetailArray  ) );
	
	for ( i = 0; i < strlen( pStringDomain ); i++ )
    {
        while ( ( i < strlen( pStringDomain ) ) && \
			    ( pStringDomain[i] != ',' ) && \
			    ( isalnum( pStringDomain[i] ) ) )
        {
            acSubstring[ j++ ] = pStringDomain[ i++ ];
        }
		
        if ( j != 0 )
        {
			int IsHaveRepeatedWord = 0;

            acSubstring[j] = '\0';
            count = Service_CheckRepeatString( stDetailArray, acSubstring, count, &IsHaveRepeatedWord );
            j = 0;

			// Check if repeated
			if( 1 == IsHaveRepeatedWord )
			{
				return TRUE;
			}
        }
    }

	return FALSE;
}

/* Service_CheckRepeatString() */
int Service_CheckRepeatString( struct detail stDetailArray[], const char unit[], int count, int* pIsHaveRepeatedWord )
{
    int i;
 
    for (i = 0; i < count; i++)
    {
        if ( strcmp(stDetailArray[i].word, unit) == 0)
        {
			/* If control reaches here, it means match found in struct */
           *pIsHaveRepeatedWord  = 1;
		   return count;
        }
    }

    /* If control reaches here, it means no match found in struct */
    strcpy( stDetailArray[count].word, unit );
 
    /* count represents the number of fields updated in array stDetailArray */
    return ( count + 1);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Service_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Service_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns     = (PCOSA_DATAMODEL_DDNS     )g_pCosaBEManager->hDdns;
    PSLIST_HEADER                   pListHead       = (PSLIST_HEADER            )&pCosaDMDdns->ContextHead;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService    = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2   = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_DDNS_SERVICE          pDdnsService2   = (PCOSA_DML_DDNS_SERVICE   )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry     = (PSINGLE_LINK_ENTRY       )NULL;
    BOOL                            bResult         = FALSE;

    pSLinkEntry = AnscSListGetFirstEntry(pListHead);

    while ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry   = AnscSListGetNextEntry(pSLinkEntry);

        pDdnsService2 = (PCOSA_DML_DDNS_SERVICE)pCosaContext2->hContext;

        if ( 
                 pDdnsService2 && 
                 ((ULONG)pDdnsService2 != (ULONG)pDdnsService) && 
                 AnscEqualString(pDdnsService2->Alias, pDdnsService->Alias, TRUE) 
           )
        {
            AnscCopyString(pReturnParamName, "Alias");

            *puLength = AnscSizeOfString("Alias");
             
            return FALSE;
        }
    }

	//Check whether Domain string contains repeated word or not
	if( TRUE == Service_IsDomainStringHaveRepeatedWord( pDdnsService->Domain ) )
	{
		AnscCopyString(pReturnParamName, "Domain");
		
		*puLength = AnscSizeOfString("Domain");
		 
		return FALSE;
	}

    if ( FALSE )
    {
        if ( g_ValidateInterface )
        {
            bResult = g_ValidateInterface(g_pDslhDmlAgent, pDdnsService->AssociatedConnection, DSLH_WAN_LAYER3_CONNECTION_INTERFACE);
        }

        if ( bResult == FALSE )
        {
            AnscCopyString(pReturnParamName, "AssociatedConnection");

            *puLength = AnscSizeOfString(pReturnParamName);

            return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Service_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Service_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_DDNS            pCosaDMDdns  = (PCOSA_DATAMODEL_DDNS     )g_pCosaBEManager->hDdns;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;

    if ( pCosaContext->bNew )
    {
        returnStatus = CosaDmlDdnsAddService (NULL, pDdnsService);

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCosaContext->bNew = FALSE;

            CosaDdnsRegDelDdnsInfo((ANSC_HANDLE)pCosaDMDdns, (ANSC_HANDLE)pCosaContext);
        }
    }
    else
    {
        returnStatus = CosaDmlDdnsSetService(NULL, pDdnsService);
    }

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Service_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Service_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DDNS_SERVICE          pDdnsService = (PCOSA_DML_DDNS_SERVICE   )pCosaContext->hContext;

    return CosaDmlDdnsGetServiceByInstNum
               (
                   NULL, 
                   pDdnsService->InstanceNumber, 
                   pDdnsService
               );
}

