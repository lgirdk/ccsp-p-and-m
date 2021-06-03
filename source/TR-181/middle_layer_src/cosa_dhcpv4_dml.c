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

    module: cosa_dhcpv4_dml.c

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

        01/18/2011    initial revision.

**************************************************************************/

#include "ansc_platform.h"
#include "cosa_dhcpv4_dml.h"
#include "cosa_dhcpv4_apis.h"
#include "cosa_dhcpv4_internal.h"
#include "plugin_main_apis.h"

#include "ccsp_base_api.h"
#include "messagebus_interface_helper.h"
#include "ansc_string_util.h"
#include "msgpack.h"
#include "macbinding_webconfig_param.h"
#include "lan_webconfig_param.h"
#include "cosa_dhcpv4_webconfig_apis.h"
#include "safec_lib_common.h"
#include "syscfg/syscfg.h"

extern void* g_pDslhDmlAgent;
extern ANSC_HANDLE bus_handle;
extern char g_Subsystem[32];

extern ULONG g_currentBsUpdate;
extern char * getRequestorString();
extern char * getTime();

ANSC_STATUS
CosaDmlDhcpsARPing
    (
        PCOSA_DML_DHCPSV4_CLIENT_IPADDRESS    pDhcpsClient
    );

#define BS_SOURCE_WEBPA_STR "webpa"
#define BS_SOURCE_RFC_STR "rfc"
#define  PARTNER_ID_LEN  64
#define   COSA_DML_DHCPV4_STATICADDRESS_ACCESS_INTERVAL   10 /* seconds*/

#define IS_UPDATE_ALLOWED_IN_DM(paramName, requestorStr) ({                                                                                                  \
    if ( g_currentBsUpdate == DSLH_CWMP_BS_UPDATE_firmware ||                                                                                     \
         (g_currentBsUpdate == DSLH_CWMP_BS_UPDATE_rfcUpdate && !AnscEqualString(requestorStr, BS_SOURCE_RFC_STR, TRUE)))                         \
    {                                                                                                                                             \
       CcspTraceWarning(("Do NOT allow override of param: %s bsUpdate = %d, requestor = %s\n", paramName, g_currentBsUpdate, requestorStr));      \
       return FALSE;                                                                                                                              \
    }                                                                                                                                             \
})

// If the requestor is RFC but the param was previously set by webpa, do not override it.
#define IS_UPDATE_ALLOWED_IN_JSON(paramName, requestorStr, UpdateSource) ({                                                                                \
   if (AnscEqualString(requestorStr, BS_SOURCE_RFC_STR, TRUE) && AnscEqualString(UpdateSource, BS_SOURCE_WEBPA_STR, TRUE))                         \
   {                                                                                                                                               \
      CcspTraceWarning(("Do NOT allow override of param: %s requestor = %d updateSource = %s\n", paramName, g_currentWriteEntity, UpdateSource));  \
      return FALSE;                                                                                                                                \
   }                                                                                                                                               \
})

#define MIN 60
#define HOURS 3600
#define DAYS 86400
#define WEEKS 604800
#define MINSECS 120
#define MAXSECS 999
#define DEFAULT_LAN_SUBNET_INST 1

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

    DHCPv4.

    *  DHCPv4_GetParamBoolValue
    *  DHCPv4_GetParamIntValue
    *  DHCPv4_GetParamUlongValue
    *  DHCPv4_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DHCPv4_GetParamBoolValue
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
DHCPv4_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DHCPv4_GetParamIntValue
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
DHCPv4_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DHCPv4_GetParamUlongValue
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
DHCPv4_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(puLong);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DHCPv4_GetParamStringValue
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
DHCPv4_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);

    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}
#ifndef FEATURE_RDKB_WAN_MANAGER
/***********************************************************************

 APIs for Object:

    DHCPv4.Client.{i}.

    *  Client_GetEntryCount
    *  Client_GetEntry
    *  Client_AddEntry
    *  Client_DelEntry
    *  Client_GetParamBoolValue
    *  Client_GetParamIntValue
    *  Client_GetParamUlongValue
    *  Client_GetParamStringValue
    *  Client_SetParamBoolValue
    *  Client_SetParamIntValue
    *  Client_SetParamUlongValue
    *  Client_SetParamStringValue
    *  Client_Validate
    *  Client_Commit
    *  Client_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client_GetEntryCount
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
Client_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    return AnscSListQueryDepth( &pDhcpv4->ClientList );
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Client_GetEntry
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
Client_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PSINGLE_LINK_ENTRY              pSListEntry       = NULL;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = NULL;

    UNREFERENCED_PARAMETER(hInsContext);

    pSListEntry = AnscSListGetEntryByIndex(&pDhcpv4->ClientList, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_DHCPC_LINK_OBJECT(pSListEntry);
        *pInsNumber       = pCxtLink->InstanceNumber;
    }

    return pSListEntry;
    
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Client_AddEntry
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
Client_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = NULL;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = NULL;

    UNREFERENCED_PARAMETER(hInsContext);
    
    pDhcpc  = (PCOSA_DML_DHCPC_FULL)AnscAllocateMemory( sizeof(COSA_DML_DHCPC_FULL) );
    if ( !pDhcpc )
    {
        goto EXIT2;
    }

    /* Set default value */
    DHCPV4_CLIENT_SET_DEFAULTVALUE(pDhcpc);

    /* Add into our link tree*/    
    pCxtLink = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)AnscAllocateMemory( sizeof(COSA_CONTEXT_DHCPC_LINK_OBJECT) );
    if ( !pDhcpc )
    {
        goto EXIT1;
    }

    DHCPV4_CLIENT_INITIATION_CONTEXT(pCxtLink)

    pCxtLink->hContext       = (ANSC_HANDLE)pDhcpc;
    pCxtLink->bNew           = TRUE;
    
    if ( !++pDhcpv4->maxInstanceOfClient )
    {
        pDhcpv4->maxInstanceOfClient = 1;
    }
    pDhcpc->Cfg.InstanceNumber = pDhcpv4->maxInstanceOfClient;
    pCxtLink->InstanceNumber   = pDhcpc->Cfg.InstanceNumber;
    *pInsNumber                = pDhcpc->Cfg.InstanceNumber;

    _ansc_sprintf( pDhcpc->Cfg.Alias, "Client%lu", pDhcpc->Cfg.InstanceNumber);

    /* Put into our list */
    CosaSListPushEntryByInsNum(&pDhcpv4->ClientList, (PCOSA_CONTEXT_LINK_OBJECT)pCxtLink);

    /* we recreate the configuration because we has new delay_added entry for dhcpv4 */
    CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);

    return (ANSC_HANDLE)pCxtLink;


EXIT1:
    
    AnscFreeMemory(pDhcpc);

EXIT2:        
    
    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client_DelEntry
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
Client_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInstance;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;

    UNREFERENCED_PARAMETER(hInsContext);
    /* Normally, two sublinks are empty because our framework will firstly 
            call delEntry for them before coming here. We needn't care them.
         */
    if ( !pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpcDelEntry(NULL, pDhcpc->Cfg.InstanceNumber);
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return returnStatus;
        }
    }
    
    if (AnscSListPopEntryByLink(&pDhcpv4->ClientList, &pCxtLink->Linkage) )
    {
        /* Because the current NextInstanceNumber information need be deleted */
        CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        
        AnscFreeMemory(pCxtLink->hContext);
        AnscFreeMemory(pCxtLink);
    }
    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_GetParamBoolValue
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
Client_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool   = pDhcpc->Cfg.bEnabled;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Renew", TRUE))
    {
        /* collect value */
        *pBool   = FALSE;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_GetParamIntValue
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
Client_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "LeaseTimeRemaining", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);
        
        *pInt   = pDhcpc->Info.LeaseTimeRemaining;
        
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_GetParamUlongValue
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
Client_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;


    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Status", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);

        *puLong   = pDhcpc->Info.Status;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "DHCPStatus", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);

        *puLong   = pDhcpc->Info.DHCPStatus;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "IPAddress", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);
        if ( pDhcpc->Info.DHCPStatus == COSA_DML_DHCPC_STATUS_Bound )
        {
            *puLong = pDhcpc->Info.IPAddress.Value;
        }
        else
        {
            *puLong = 0;
        }
        
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "SubnetMask", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);
        if ( pDhcpc->Info.DHCPStatus == COSA_DML_DHCPC_STATUS_Bound )
        {
            *puLong = pDhcpc->Info.SubnetMask.Value;
        }
        else
        {
            *puLong = 0;
        }
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "DHCPServer", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);
        *puLong = pDhcpc->Info.DHCPServer.Value;

        return TRUE;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client_GetParamStringValue
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
Client_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;
    CHAR                            tmpBuff[128]      = {0};
    ULONG                           i                 = 0;
    ULONG                           len               = 0;
    PUCHAR                          pString           = NULL;
    
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpc->Cfg.Alias) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpc->Cfg.Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpc->Cfg.Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_BootFileName", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpc->Cfg.X_CISCO_COM_BootFileName) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpc->Cfg.X_CISCO_COM_BootFileName);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpc->Cfg.X_CISCO_COM_BootFileName)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Interface", TRUE))
    {
        /* collect value */
        pString = CosaUtilGetFullPathNameByKeyword
                    (
                        (PUCHAR)"Device.IP.Interface.",
                        (PUCHAR)"Name",
                        (PUCHAR)pDhcpc->Cfg.Interface
                    );

        if ( pString )
        {
            if ( AnscSizeOfString((const char*)pString) < *pUlSize)
            {
                AnscCopyString(pValue, (char*)pString);

                AnscFreeMemory(pString);

                return 0;
            }
            else
            {
                *pUlSize = AnscSizeOfString((const char*)pString)+1;

                AnscFreeMemory(pString);
                
                return 1;
            }
        }
        else
        {
            return 0;
        }

    }

    if( AnscEqualString(ParamName, "IPRouters", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);
        if ( pDhcpc->Info.DHCPStatus != COSA_DML_DHCPC_STATUS_Bound )
        {
            *pValue    = '\0';
            return 0;
        }
        
        AnscZeroMemory(tmpBuff, sizeof(tmpBuff));
        for( i=0; i<pDhcpc->Info.NumIPRouters && i<COSA_DML_DHCP_MAX_ENTRIES; i++)
        {
            len = AnscSizeOfString(tmpBuff);
            
            if(i > 0)
                tmpBuff[len++] = ',';
          
            AnscCopyString( &tmpBuff[len], _ansc_inet_ntoa(*((struct in_addr *)(&pDhcpc->Info.IPRouters[i]))) );
        }
        
        if ( AnscSizeOfString(tmpBuff) < *pUlSize)
        {
            AnscCopyString(pValue, tmpBuff);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpc->Cfg.Interface)+1;
            return 1;
        }

        return 0;
    }

    if( AnscEqualString(ParamName, "DNSServers", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetInfo(NULL, pCxtLink->InstanceNumber, &pDhcpc->Info);
        if ( pDhcpc->Info.DHCPStatus != COSA_DML_DHCPC_STATUS_Bound )
        {
            *pValue    = '\0';
            return 0;
        }

        AnscZeroMemory(tmpBuff, sizeof(tmpBuff));
        for( i=0; i<pDhcpc->Info.NumDnsServers && i<COSA_DML_DHCP_MAX_ENTRIES; i++)
        {
            len = AnscSizeOfString(tmpBuff);
            
            if(i > 0)
                tmpBuff[len++] = ',';
          
            AnscCopyString( &tmpBuff[len], _ansc_inet_ntoa(*((struct in_addr *)(&pDhcpc->Info.DNSServers[i]))) );
        }
        
        if ( AnscSizeOfString(tmpBuff) < *pUlSize)
        {
            AnscCopyString(pValue, tmpBuff);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpc->Cfg.Interface)+1;
            return 1;
        }

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_SetParamBoolValue
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
Client_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        pDhcpc->Cfg.bEnabled = bValue;
        
        return  TRUE;
    }

    if( AnscEqualString(ParamName, "Renew", TRUE))
    {
        /* save update to backup */
        if ( bValue )
        {
            returnStatus = CosaDmlDhcpcRenew(NULL, pDhcpc->Cfg.InstanceNumber);
            if ( returnStatus != ANSC_STATUS_SUCCESS )
            {
                return  FALSE;
            }
        }
        
        return  TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return  FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_SetParamIntValue
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
Client_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);

    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_SetParamUlongValue
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
Client_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(uValue);

    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_SetParamStringValue
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
Client_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        AnscCopyString(pDhcpv4->AliasOfClient, pDhcpc->Cfg.Alias);

        AnscCopyString(pDhcpc->Cfg.Alias, pString);
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_BootFileName", TRUE))
    {
        AnscCopyString(pDhcpc->Cfg.X_CISCO_COM_BootFileName, pString);
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Interface", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDhcpc->Cfg.Interface, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client_Validate
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
Client_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;
    PSINGLE_LINK_ENTRY              pSListEntry       = NULL;
    PCOSA_DML_DHCPC_FULL            pDhcpc2           = NULL;
    BOOL                            bFound            = FALSE;

    UNREFERENCED_PARAMETER(puLength);

    /*  only for Alias */
    if ( pDhcpv4->AliasOfClient[0] )
    {
        pSListEntry           = AnscSListGetFirstEntry(&pDhcpv4->ClientList);
        while( pSListEntry != NULL)
        {
            pCxtLink          = ACCESS_COSA_CONTEXT_DHCPC_LINK_OBJECT(pSListEntry);
            pSListEntry       = AnscSListGetNextEntry(pSListEntry);
        
            pDhcpc2 = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;
        
            if( DHCPV4_CLIENT_ENTRY_MATCH2(pDhcpc2->Cfg.Alias, pDhcpc->Cfg.Alias) )
            {
                if ( (ANSC_HANDLE)pCxtLink == hInsContext )
                {
                    continue;
                }

                _ansc_strcpy(pReturnParamName, "Alias");

                bFound = TRUE;
                
                break;
            }
        }
        
        if ( bFound )
        {
#if COSA_DHCPV4_ROLLBACK_TEST        
            Client_Rollback(hInsContext);
#endif
            return FALSE;
        }
    }

    /* some other checking */



    
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client_Commit
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
Client_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if ( pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpcAddEntry(NULL, pDhcpc );

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCxtLink->bNew = FALSE;

            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        }
        else
        {
            DHCPV4_CLIENT_SET_DEFAULTVALUE(pDhcpc);
            
            if ( pDhcpv4->AliasOfClient[0] )
                AnscCopyString( pDhcpc->Cfg.Alias, pDhcpv4->AliasOfClient );
        }
    }
    else
    {
        returnStatus = CosaDmlDhcpcSetCfg(NULL, &pDhcpc->Cfg);

        if ( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CosaDmlDhcpcGetCfg(NULL, &pDhcpc->Cfg);
        }
    }
    
    AnscZeroMemory( pDhcpv4->AliasOfClient, sizeof(pDhcpv4->AliasOfClient) );

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client_Rollback
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
Client_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtLink->hContext;

    if ( pDhcpv4->AliasOfClient[0] )
        AnscCopyString( pDhcpc->Cfg.Alias, pDhcpv4->AliasOfClient );

    if ( !pCxtLink->bNew )
    {
        CosaDmlDhcpcGetCfg( NULL, &pDhcpc->Cfg );
    }
    else
    {
        DHCPV4_CLIENT_SET_DEFAULTVALUE(pDhcpc);
    }
    
    AnscZeroMemory( pDhcpv4->AliasOfClient, sizeof(pDhcpv4->AliasOfClient) );
    
    return returnStatus;
}

/***********************************************************************

 APIs for Object:

    DHCPv4.Client.{i}.SentOption.{i}.

    *  SentOption_GetEntryCount
    *  SentOption_GetEntry
    *  SentOption_AddEntry
    *  SentOption_DelEntry
    *  SentOption_GetParamBoolValue
    *  SentOption_GetParamIntValue
    *  SentOption_GetParamUlongValue
    *  SentOption_GetParamStringValue
    *  SentOption_SetParamBoolValue
    *  SentOption_SetParamIntValue
    *  SentOption_SetParamUlongValue
    *  SentOption_SetParamStringValue
    *  SentOption_Validate
    *  SentOption_Commit
    *  SentOption_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SentOption_GetEntryCount
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
SentOption_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    return AnscSListQueryDepth( &pCxtLink->SendOptionList );
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SentOption_GetEntryStatus
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
BOOL
SentOption_GetEntryStatus
    (
        ANSC_HANDLE                 hInsContext,
        PCHAR                       StatusName
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;

    if( AnscEqualString(StatusName, "Committed", TRUE))
    {
        /* collect value */
        if ( pCxtLink->bNew )
            return FALSE;
        
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        SentOption_GetEntry
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
SentOption_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink        = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry          = NULL;

    pSListEntry = AnscSListGetEntryByIndex(&pCxtDhcpcLink->SendOptionList, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
        *pInsNumber       = pCxtLink->InstanceNumber;
    }

    return pSListEntry;

}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        SentOption_AddEntry
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
SentOption_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink        = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption      = NULL;
    
    pDhcpSendOption  = (PCOSA_DML_DHCP_OPT)AnscAllocateMemory( sizeof(COSA_DML_DHCP_OPT) );
    if ( !pDhcpSendOption )
    {
        goto EXIT2;
    }

    DHCPV4_SENDOPTION_SET_DEFAULTVALUE(pDhcpSendOption);
    
    pCxtLink = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory( sizeof(COSA_CONTEXT_LINK_OBJECT) );
    if ( !pCxtLink )
    {
        goto EXIT1;
    }

    pCxtLink->hContext       = (ANSC_HANDLE)pDhcpSendOption;
    pCxtLink->hParentTable   = (ANSC_HANDLE)pCxtDhcpcLink;
    pCxtLink->bNew           = TRUE;
    
    if ( !++pCxtDhcpcLink->maxInstanceOfSend )
    {
        pCxtDhcpcLink->maxInstanceOfSend = 1;
    }
    
    pDhcpSendOption->InstanceNumber = pCxtDhcpcLink->maxInstanceOfSend; 
    pCxtLink->InstanceNumber       = pDhcpSendOption->InstanceNumber;
    *pInsNumber                    = pDhcpSendOption->InstanceNumber;

    _ansc_sprintf( pDhcpSendOption->Alias, "SentOption%lu", pDhcpSendOption->InstanceNumber);

    /* Put into our list */
    CosaSListPushEntryByInsNum(&pCxtDhcpcLink->SendOptionList, (PCOSA_CONTEXT_LINK_OBJECT)pCxtLink);

    /* we recreate the configuration */
    CosaDhcpv4RegSetDhcpv4Info((ANSC_HANDLE)g_pCosaBEManager->hDhcpv4);

    return (ANSC_HANDLE)pCxtLink;    
       
EXIT1:
        
    AnscFreeMemory(pDhcpSendOption);
    
EXIT2:   
        
    return NULL;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SentOption_DelEntry
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
SentOption_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink        = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpClient          = (PCOSA_DML_DHCPC_FULL)pCxtDhcpcLink->hContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption      = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if ( !pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpcDelSentOption(NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpSendOption->InstanceNumber);
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return returnStatus;
        }
    }

    if ( AnscSListPopEntryByLink(&pCxtDhcpcLink->SendOptionList, &pCxtLink->Linkage) )
    {
        CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        
        AnscFreeMemory(pCxtLink->hContext);
        AnscFreeMemory(pCxtLink);
    }

    return returnStatus;    

}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_GetParamBoolValue
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
SentOption_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption      = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool  = pDhcpSendOption->bEnabled;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_GetParamIntValue
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
SentOption_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_GetParamUlongValue
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
SentOption_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption      = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Tag", TRUE))
    {
        /* collect value */
        *puLong = pDhcpSendOption->Tag;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SentOption_GetParamStringValue
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
SentOption_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption      = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpSendOption->Alias) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpSendOption->Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpSendOption->Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Value", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pDhcpSendOption->Value) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pDhcpSendOption->Value);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pDhcpSendOption->Value)+1;
            return 1;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_SetParamBoolValue
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
SentOption_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption      = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        pDhcpSendOption->bEnabled  = bValue;
        
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_SetParamIntValue
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
SentOption_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);

    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_SetParamUlongValue
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
SentOption_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption      = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Tag", TRUE))
    {
        /* save update to backup */
        pDhcpSendOption->Tag  = (UCHAR)uValue;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_SetParamStringValue
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
SentOption_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption   = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        AnscCopyString(pCxtDhcpcLink->AliasOfSend, pDhcpSendOption->Alias);

        AnscCopyString(pDhcpSendOption->Alias, pString);
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Value", TRUE))
    {
        /* save update to backup */
        AnscCopyString((char*)pDhcpSendOption->Value, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SentOption_Validate
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
SentOption_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(puLength);
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption   = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink2         = NULL;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption2  = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry       = NULL;
    BOOL                            bFound            = FALSE;

    /* Parent hasn't set, we don't permit child is set.*/
    if ( pCxtDhcpcLink->bNew )
    {
#if COSA_DHCPV4_ROLLBACK_TEST        
        SentOption_Rollback(hInsContext);
#endif
        return FALSE;
    }

    /* This is for alias */
    if ( pCxtDhcpcLink->AliasOfSend[0] )
    {
        bFound                = FALSE;
        pSListEntry           = AnscSListGetFirstEntry(&pCxtDhcpcLink->SendOptionList);
        while( pSListEntry != NULL)
        {
            pCxtLink2         = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
            pSListEntry       = AnscSListGetNextEntry(pSListEntry);

            pDhcpSendOption2  = (PCOSA_DML_DHCP_OPT)pCxtLink2->hContext;

            if( DHCPV4_SENDOPTION_ENTRY_MATCH2(pDhcpSendOption->Alias, pDhcpSendOption2->Alias) )
            {
                if ( (ANSC_HANDLE)pCxtLink2 == hInsContext )
                {
                    continue;
                }

                _ansc_strcpy(pReturnParamName, "Alias");

                bFound = TRUE;
                
                break;
            }
        }
        
        if ( bFound )
        {
#if COSA_DHCPV4_ROLLBACK_TEST        
            SentOption_Rollback(hInsContext);
#endif
            return FALSE;
        }
    }

    /* For other check */

    
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SentOption_Commit
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
SentOption_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption   = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPC_FULL            pDhcpClient       = (PCOSA_DML_DHCPC_FULL)pCxtDhcpcLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if ( pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpcAddSentOption(NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpSendOption );

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCxtLink->bNew = FALSE;

            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        }
        else
        {
            DHCPV4_SENDOPTION_SET_DEFAULTVALUE(pDhcpSendOption);

            if ( pCxtDhcpcLink->AliasOfSend[0] )
                AnscCopyString( pDhcpSendOption->Alias, pCxtDhcpcLink->AliasOfSend );
        }
    }
    else
    {
        returnStatus = CosaDmlDhcpcSetSentOption(NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpSendOption);

        if ( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CosaDmlDhcpcGetSentOptionbyInsNum(NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpSendOption);
        }
    }
    
    AnscZeroMemory( pCxtDhcpcLink->AliasOfSend, sizeof(pCxtDhcpcLink->AliasOfSend) );
    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SentOption_Rollback
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
SentOption_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCP_OPT              pDhcpSendOption   = (PCOSA_DML_DHCP_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtDhcpcLink->hContext;

    if ( pCxtDhcpcLink->AliasOfSend[0] )
        AnscCopyString( pDhcpSendOption->Alias, pCxtDhcpcLink->AliasOfSend );

    if ( !pCxtLink->bNew )
    {
        CosaDmlDhcpcGetSentOptionbyInsNum(NULL, pDhcpc->Cfg.InstanceNumber, pDhcpSendOption);
    }
    else
    {
        DHCPV4_SENDOPTION_SET_DEFAULTVALUE(pDhcpSendOption);
    }
    
    AnscZeroMemory( pCxtDhcpcLink->AliasOfSend, sizeof(pCxtDhcpcLink->AliasOfSend) );
    
    return returnStatus;
}

/***********************************************************************

 APIs for Object:

    DHCPv4.Client.{i}.ReqOption.{i}.

    *  ReqOption_GetEntryCount
    *  ReqOption_GetEntry
    *  ReqOption_AddEntry
    *  ReqOption_DelEntry
    *  ReqOption_GetParamBoolValue
    *  ReqOption_GetParamIntValue
    *  ReqOption_GetParamUlongValue
    *  ReqOption_GetParamStringValue
    *  ReqOption_SetParamBoolValue
    *  ReqOption_SetParamIntValue
    *  ReqOption_SetParamUlongValue
    *  ReqOption_SetParamStringValue
    *  ReqOption_Validate
    *  ReqOption_Commit
    *  ReqOption_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ReqOption_GetEntryCount
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
ReqOption_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtLink          = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;

    return AnscSListQueryDepth( &pCxtLink->ReqOptionList );
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        ReqOption_GetEntry
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
ReqOption_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink        = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry          = NULL;
    
    pSListEntry = AnscSListGetEntryByIndex(&pCxtDhcpcLink->ReqOptionList, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
        *pInsNumber       = pCxtLink->InstanceNumber;
    }

    return pSListEntry;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        ReqOption_AddEntry
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
ReqOption_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink        = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption       = NULL;
    
    /* We need ask from backend */
    pDhcpReqOption  = (PCOSA_DML_DHCPC_REQ_OPT)AnscAllocateMemory( sizeof(COSA_DML_DHCPC_REQ_OPT) );
    if ( !pDhcpReqOption )
    {
        goto EXIT2;
    }

    DHCPV4_REQOPTION_SET_DEFAULTVALUE(pDhcpReqOption);
    
    pCxtLink = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory( sizeof(COSA_CONTEXT_LINK_OBJECT) );
    if ( !pCxtLink )
    {
        goto EXIT1;
    }

    pCxtLink->hContext       = (ANSC_HANDLE)pDhcpReqOption;
    pCxtLink->hParentTable   = (ANSC_HANDLE)pCxtDhcpcLink;
    pCxtLink->bNew           = TRUE;
    
    if ( !++pCxtDhcpcLink->maxInstanceOfReq )
    {
        pCxtDhcpcLink->maxInstanceOfReq = 1;
    }
    pDhcpReqOption->InstanceNumber = pCxtDhcpcLink->maxInstanceOfReq;
    pCxtLink->InstanceNumber       = pDhcpReqOption->InstanceNumber; 
    *pInsNumber                    = pDhcpReqOption->InstanceNumber;

    _ansc_sprintf( pDhcpReqOption->Alias, "ReqOption%lu", pDhcpReqOption->InstanceNumber);

    /* Put into our list */
    CosaSListPushEntryByInsNum(&pCxtDhcpcLink->ReqOptionList, (PCOSA_CONTEXT_LINK_OBJECT)pCxtLink);

    /* we recreate the configuration */
    CosaDhcpv4RegSetDhcpv4Info((ANSC_HANDLE)g_pCosaBEManager->hDhcpv4);

    return (ANSC_HANDLE)pCxtLink;    
    
EXIT1:
        
    AnscFreeMemory(pDhcpReqOption);
    
EXIT2:   
        
    return NULL;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ReqOption_DelEntry
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
ReqOption_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink        = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_FULL            pDhcpClient          = (PCOSA_DML_DHCPC_FULL)pCxtDhcpcLink->hContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption       = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if ( !pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpcDelReqOption( NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpReqOption->InstanceNumber );
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return returnStatus;
        }
    }

    if ( AnscSListPopEntryByLink(&pCxtDhcpcLink->ReqOptionList, &pCxtLink->Linkage) )
    {
        CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);

        AnscFreeMemory(pCxtLink->hContext);
        AnscFreeMemory(pCxtLink);
    }

    return returnStatus;    
        
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_GetParamBoolValue
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
ReqOption_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption       = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool    =  pDhcpReqOption->bEnabled;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_GetParamIntValue
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
ReqOption_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_GetParamUlongValue
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
ReqOption_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption       = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Order", TRUE))
    {
        /* collect value */
        *puLong  =  pDhcpReqOption->Order;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Tag", TRUE))
    {
        /* collect value */
        *puLong  =  pDhcpReqOption->Tag;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ReqOption_GetParamStringValue
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
ReqOption_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption       = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtDhcpcLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpReqOption->Alias) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpReqOption->Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpReqOption->Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Value", TRUE))
    {
        /* collect value */
        CosaDmlDhcpcGetReqOptionbyInsNum(NULL, pDhcpc->Cfg.InstanceNumber, pDhcpReqOption);
        
        if ( AnscSizeOfString((const char*)pDhcpReqOption->Value) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pDhcpReqOption->Value);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pDhcpReqOption->Value)+1;
            return 1;
        }
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_SetParamBoolValue
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
ReqOption_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption       = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        pDhcpReqOption->bEnabled   =  bValue;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_SetParamIntValue
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
ReqOption_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);

    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_SetParamUlongValue
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
ReqOption_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption       = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Order", TRUE))
    {
        /* save update to backup */
        pDhcpReqOption->Order   = uValue;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Tag", TRUE))
    {
        /* save update to backup */
        pDhcpReqOption->Tag   = (UCHAR)uValue;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_SetParamStringValue
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
ReqOption_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption    = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* Backup old alias firstly */
        AnscCopyString(pCxtDhcpcLink->AliasOfReq, pDhcpReqOption->Alias);

        AnscCopyString(pDhcpReqOption->Alias, pString);

        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ReqOption_Validate
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
ReqOption_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption    = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink2         = NULL;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption2   = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry       = NULL;
    BOOL                            bFound            = FALSE;

    UNREFERENCED_PARAMETER(puLength);

    /* Parent hasn't set, we don't permit child is set.*/
    if ( pCxtDhcpcLink->bNew )
    {
#if COSA_DHCPV4_ROLLBACK_TEST        
        ReqOption_Rollback(hInsContext);
#endif

        return FALSE;
    }

    /* For other check */
    if ( pCxtDhcpcLink->AliasOfReq[0] )
    {
        /* save update to backup */
        bFound                = FALSE;
        pSListEntry           = AnscSListGetFirstEntry(&pCxtDhcpcLink->ReqOptionList);
        while( pSListEntry != NULL)
        {
            pCxtLink2         = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
            pSListEntry       = AnscSListGetNextEntry(pSListEntry);

            pDhcpReqOption2  = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink2->hContext;

            if( DHCPV4_REQOPTION_ENTRY_MATCH2(pDhcpReqOption->Alias, pDhcpReqOption2->Alias ) )
            {
                if ( (ANSC_HANDLE)pCxtLink2 == hInsContext )
                {
                    continue;
                }
                
                _ansc_strcpy(pReturnParamName, "Alias");

                bFound = TRUE;
                
                break;
            }
        }

        if ( bFound )
        {
#if COSA_DHCPV4_ROLLBACK_TEST        
            ReqOption_Rollback(hInsContext);
#endif
            return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ReqOption_Commit
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
ReqOption_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption    = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPC_FULL            pDhcpClient       = (PCOSA_DML_DHCPC_FULL)pCxtDhcpcLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if ( pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpcAddReqOption(NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpReqOption );

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCxtLink->bNew = FALSE;

            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        }
        else
        {
            DHCPV4_REQOPTION_SET_DEFAULTVALUE(pDhcpReqOption);

            if ( pCxtDhcpcLink->AliasOfReq[0] )
                AnscCopyString( pDhcpReqOption->Alias, pCxtDhcpcLink->AliasOfReq );
        }
    }
    else
    {
        returnStatus = CosaDmlDhcpcSetReqOption(NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpReqOption);

        if ( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CosaDmlDhcpcGetReqOptionbyInsNum(NULL, pDhcpClient->Cfg.InstanceNumber, pDhcpReqOption);
        }
    }
    
    AnscZeroMemory( pCxtDhcpcLink->AliasOfReq, sizeof(pCxtDhcpcLink->AliasOfReq) );
    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ReqOption_Rollback
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
ReqOption_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPC_REQ_OPT         pDhcpReqOption    = (PCOSA_DML_DHCPC_REQ_OPT)pCxtLink->hContext;
    PCOSA_CONTEXT_DHCPC_LINK_OBJECT pCxtDhcpcLink     = (PCOSA_CONTEXT_DHCPC_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPC_FULL            pDhcpc            = (PCOSA_DML_DHCPC_FULL)pCxtDhcpcLink->hContext;

    if ( pCxtDhcpcLink->AliasOfReq[0] )
        AnscCopyString( pDhcpReqOption->Alias, pCxtDhcpcLink->AliasOfReq );

    if ( !pCxtLink->bNew )
    {
        CosaDmlDhcpcGetReqOptionbyInsNum(NULL, pDhcpc->Cfg.InstanceNumber, pDhcpReqOption);
    }
    else
    {
        DHCPV4_REQOPTION_SET_DEFAULTVALUE(pDhcpReqOption);
    }
    
    AnscZeroMemory( pCxtDhcpcLink->AliasOfReq, sizeof(pCxtDhcpcLink->AliasOfReq) );
    
    return returnStatus;
}
#endif
/***********************************************************************

 APIs for Object:

    DHCPv4.Server.

    *  Server_GetParamBoolValue
    *  Server_GetParamIntValue
    *  Server_GetParamUlongValue
    *  Server_GetParamStringValue
    *  Server_SetParamBoolValue
    *  Server_SetParamIntValue
    *  Server_SetParamUlongValue
    *  Server_SetParamStringValue
    *  Server_Validate
    *  Server_Commit
    *  Server_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Server_GetParamBoolValue
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
Server_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool  =  CosaDmlDhcpsGetState(NULL);
        
        return TRUE;
    }

    else if (strcmp(ParamName, "X_LGI-COM_DAD") == 0)
    {
        char strBuf[8];

        syscfg_get (NULL, "dhcp_disable_ip_conflict_det", strBuf, sizeof(strBuf));

        /*
           Note that the "dhcp_disable_ip_conflict_det" syscfg value is the
           inverse of the Device.DHCPv4.Server.X_LGI-COM_DAD data model object.
        */
        *pBool = (strcmp (strBuf, "0") == 0) ? TRUE : FALSE;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Server_GetParamIntValue
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
Server_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Server_GetParamUlongValue
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
Server_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(puLong);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Server_GetParamStringValue
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
Server_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    /* check the parameter name and return the corresponding value */
    if(AnscEqualString(ParamName, "StaticClientsData", TRUE) || AnscEqualString(ParamName, "Lan", TRUE))
    {
        CcspTraceWarning(("Data Get Not supported\n"));
        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Server_SetParamBoolValue
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
Server_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    ANSC_STATUS                     returnStatus  = ANSC_STATUS_SUCCESS;
        
    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        returnStatus = CosaDmlDhcpsEnable((ANSC_HANDLE)NULL, bValue );

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return FALSE;
        }
        
        return TRUE;
    }

    else if (strcmp(ParamName, "X_LGI-COM_DAD") == 0)
    {
        /*
           Note that the "dhcp_disable_ip_conflict_det" syscfg value is the
           inverse of the Device.DHCPv4.Server.X_LGI-COM_DAD data model object.
        */
        if (syscfg_set (NULL, "dhcp_disable_ip_conflict_det", bValue ? "0" : "1") != 0)
        {
            CcspTraceWarning(("syscfg_set failed\n"));
            return -1;
        }

        if (syscfg_commit() != 0)
        {
            CcspTraceWarning(("syscfg_commit failed\n"));
            return -1;
        }

        system ("sysevent set dhcp_server-restart");

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Server_SetParamIntValue
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
Server_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);

    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Server_SetParamUlongValue
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
Server_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(uValue);

    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Server_SetParamStringValue
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
Server_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */
    if(AnscEqualString(ParamName, "StaticClientsData", TRUE))
    {
        char * decodeMsg =NULL;
        int size =0;
        int retval = 0;
        msgpack_unpack_return unpack_ret = MSGPACK_UNPACK_SUCCESS;
 
        retval = get_base64_decodedbuffer(pString, &decodeMsg, &size);
        if (retval == 0)
        {
            unpack_ret = get_msgpack_unpack_status(decodeMsg,size);
        }
        else
        {   
            if (decodeMsg)
            {
                free(decodeMsg);
                decodeMsg = NULL;
            }
            return FALSE;
        }
        if(unpack_ret == MSGPACK_UNPACK_SUCCESS)
        {
            macbindingdoc_t *pStaticClients = NULL;
            pStaticClients = macbindingdoc_convert( decodeMsg, size+1 );

           if (decodeMsg)
            {
                free(decodeMsg);
                decodeMsg = NULL;
            }
            if (NULL != pStaticClients)
            {
                CcspTraceWarning(("pStaticClients->entries_count is %ld\n", pStaticClients->entries_count));
                CcspTraceWarning(("pStaticClients->subdoc_name is %s\n", pStaticClients->subdoc_name));
                CcspTraceWarning(("pStaticClients->version is %lu\n", (unsigned long)pStaticClients->version));
                CcspTraceWarning(("pStaticClients->transaction_id is %d\n", pStaticClients->transaction_id));
                CcspTraceWarning(("StaticClients configuration received\n"));

                execData *execDataStaticClients = NULL ;

                execDataStaticClients = (execData*) malloc (sizeof(execData));

                if ( execDataStaticClients != NULL )
                {

                    memset(execDataStaticClients, 0, sizeof(execData));

                    execDataStaticClients->txid = pStaticClients->transaction_id;
                    execDataStaticClients->version = pStaticClients->version;
                    execDataStaticClients->numOfEntries = pStaticClients->entries_count;

                    strncpy(execDataStaticClients->subdoc_name,"macbinding",sizeof(execDataStaticClients->subdoc_name)-1);

                    execDataStaticClients->user_data = (void*) pStaticClients ;
                    execDataStaticClients->calcTimeout = NULL ;
                    execDataStaticClients->executeBlobRequest = Process_StaticClients_WebConfigRequest;
                    execDataStaticClients->rollbackFunc = rollback_StaticClients;
                    execDataStaticClients->freeResources = FreeResources_StaticClients;
                    PushBlobRequest(execDataStaticClients);

                    CcspTraceWarning(("PushBlobRequest complete\n"));
                }
                else
                {
                    CcspTraceWarning(("execData memory allocation failed\n"));
                    macbindingdoc_destroy(pStaticClients);
                    return FALSE;

                }

            }
            return TRUE;

        }
        else
        {
            if (decodeMsg)
            {
                free(decodeMsg);
                decodeMsg = NULL;
            }
            CcspTraceWarning(("Corrupted StaticClientsData value\n"));
            return FALSE;
        }
        return TRUE;

    }

    if (AnscEqualString(ParamName, "Lan", TRUE))
    {
        char * decodeMsg =NULL;
        int size =0;
        int retval = 0;
        msgpack_unpack_return unpack_ret = MSGPACK_UNPACK_SUCCESS;
 
        retval = get_base64_decodedbuffer(pString, &decodeMsg, &size);
        if (retval == 0)
        {
            unpack_ret = get_msgpack_unpack_status(decodeMsg,size);
        }
        else
        {   
            if (decodeMsg)
            {
                free(decodeMsg);
                decodeMsg = NULL;
            }
            return FALSE;
        }
        if(unpack_ret == MSGPACK_UNPACK_SUCCESS)
        {
            landoc_t *pLanInfo = NULL;
            pLanInfo = landoc_convert( decodeMsg, size+1 );

           if (decodeMsg)
            {
                free(decodeMsg);
                decodeMsg = NULL;
            }
            if (NULL != pLanInfo)
            {
		        pLanInfo->entries_count = 1;// Assigned 1 by default.
                CcspTraceWarning(("pLanInfo->entries_count is %ld\n", pLanInfo->entries_count));
                CcspTraceWarning(("pLanInfo->subdoc_name is %s\n", pLanInfo->subdoc_name));
                CcspTraceWarning(("pLanInfo->version is %lu\n", (unsigned long)pLanInfo->version));
                CcspTraceWarning(("pLanInfo->transaction_id is %d\n", pLanInfo->transaction_id));
                CcspTraceWarning(("Lan configuration received\n"));

                execData *execDataLan = NULL;

                execDataLan = (execData*) malloc (sizeof(execData));

                if ( execDataLan != NULL )
                {

                    memset(execDataLan, 0, sizeof(execData));

                    execDataLan->txid = pLanInfo->transaction_id;
                    execDataLan->version = pLanInfo->version;
                    execDataLan->numOfEntries = pLanInfo->entries_count;

                    strncpy(execDataLan->subdoc_name,"lan",sizeof(execDataLan->subdoc_name)-1);

                    execDataLan->user_data = (void*) pLanInfo ;
                    execDataLan->calcTimeout = NULL ;
                    execDataLan->executeBlobRequest = Process_Lan_WebConfigRequest;
                    execDataLan->rollbackFunc = rollback_Lan;
                    execDataLan->freeResources = FreeResources_Lan;
                    PushBlobRequest(execDataLan);

                    CcspTraceWarning(("PushBlobRequest complete\n"));
                }
                else
                {
                    CcspTraceWarning(("execData memory allocation failed\n"));
                    landoc_destroy(pLanInfo);
                    return FALSE;

                }

            }
            return TRUE;

        }
        else
        {
            if (decodeMsg)
            {
                free(decodeMsg);
                decodeMsg = NULL;
            }
            CcspTraceWarning(("Corrupted StaticClientsData value\n"));
            return FALSE;
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
        Server_Validate
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
Server_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Server_Commit
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
Server_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Server_Rollback
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
Server_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/***********************************************************************

 APIs for Object:

    DHCPv4.Server.X_CISCO_COM_StaticAddress.{i}.

    *  X_CISCO_COM_StaticAddress_GetEntryCount
    *  X_CISCO_COM_StaticAddress_GetEntry
    *  X_CISCO_COM_StaticAddress_AddEntry
    *  X_CISCO_COM_StaticAddress_DelEntry
    *  X_CISCO_COM_StaticAddress_GetParamBoolValue
    *  X_CISCO_COM_StaticAddress_GetParamIntValue
    *  X_CISCO_COM_StaticAddress_GetParamUlongValue
    *  X_CISCO_COM_StaticAddress_GetParamStringValue
    *  X_CISCO_COM_StaticAddress_SetParamBoolValue
    *  X_CISCO_COM_StaticAddress_SetParamIntValue
    *  X_CISCO_COM_StaticAddress_SetParamUlongValue
    *  X_CISCO_COM_StaticAddress_SetParamStringValue
    *  X_CISCO_COM_StaticAddress_Validate
    *  X_CISCO_COM_StaticAddress_Commit
    *  X_CISCO_COM_StaticAddress_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_StaticAddress_GetEntryCount
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
X_CISCO_COM_StaticAddress_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    return AnscSListQueryDepth( &pDhcpv4->X_CISCO_COM_StaticAddressList );
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        X_CISCO_COM_StaticAddress_GetEntry
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
X_CISCO_COM_StaticAddress_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4   )g_pCosaBEManager->hDhcpv4;
    PSLIST_HEADER                   pListHead            = (PSLIST_HEADER            )&pDhcpv4->X_CISCO_COM_StaticAddressList;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry          = NULL;

    pSListEntry = AnscSListGetEntryByIndex(pListHead, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
        *pInsNumber       = pCxtLink->InstanceNumber;
    }

    return pSListEntry;
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        X_CISCO_COM_StaticAddress_AddEntry
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
X_CISCO_COM_StaticAddress_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pDhcpX_COM_CISCO_StaticAddress = NULL;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    
    /* We need ask from backend */
    pDhcpX_COM_CISCO_StaticAddress  = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)AnscAllocateMemory( sizeof(COSA_DML_DHCPS_X_CISCO_COM_SADDR) );
    if ( !pDhcpX_COM_CISCO_StaticAddress )
    {
        goto EXIT2;
    }

    DHCPV4_X_COM_CISCO_SADDR_SET_DEFAULTVALUE(pDhcpX_COM_CISCO_StaticAddress);

    pCxtLink = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory( sizeof(COSA_CONTEXT_LINK_OBJECT) );
    if ( !pCxtLink )
    {
        goto EXIT1;
    }

    pCxtLink->hContext       = (ANSC_HANDLE)pDhcpX_COM_CISCO_StaticAddress;
    pCxtLink->hParentTable   = NULL;
    pCxtLink->bNew           = TRUE;
    
    if ( !++pDhcpv4->maxInstanceX_CISCO_COM_SAddr )
    {
        pDhcpv4->maxInstanceX_CISCO_COM_SAddr = 1;
    }
    pDhcpX_COM_CISCO_StaticAddress->InstanceNumber = pDhcpv4->maxInstanceX_CISCO_COM_SAddr;
    pCxtLink->InstanceNumber           = pDhcpX_COM_CISCO_StaticAddress->InstanceNumber; 
    *pInsNumber                        = pDhcpX_COM_CISCO_StaticAddress->InstanceNumber;

    _ansc_sprintf( pDhcpX_COM_CISCO_StaticAddress->Alias, "cpe-X_COM_CISCO_SAddr%lu", pDhcpX_COM_CISCO_StaticAddress->InstanceNumber);

    /* Put into our list */
    CosaSListPushEntryByInsNum(&pDhcpv4->X_CISCO_COM_StaticAddressList, (PCOSA_CONTEXT_LINK_OBJECT)pCxtLink);

    /* we recreate the configuration */
    CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);

    return (ANSC_HANDLE)pCxtLink;    

EXIT1:
        
    AnscFreeMemory(pDhcpX_COM_CISCO_StaticAddress);
    
EXIT2:   
        
    return NULL;
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_StaticAddress_DelEntry
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
X_CISCO_COM_StaticAddress_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pDhcpX_COM_CISCO_StaticAddress = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    
    if ( !pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpsDelX_COM_CISCO_Saddr( NULL, pDhcpX_COM_CISCO_StaticAddress->InstanceNumber );
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return returnStatus;
        }
    }

    /* Firstly we del this entry */
    if ( AnscSListPopEntryByLink(&pDhcpv4->X_CISCO_COM_StaticAddressList, &pCxtLink->Linkage) )
    {
        CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        AnscFreeMemory(pCxtLink->hContext);
        AnscFreeMemory(pCxtLink);
    }
    
    return returnStatus;
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_GetParamBoolValue
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
X_CISCO_COM_StaticAddress_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_GetParamIntValue
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
X_CISCO_COM_StaticAddress_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_GetParamUlongValue
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
X_CISCO_COM_StaticAddress_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT               pContextLinkObject = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR       pEntry = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pContextLinkObject->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Yiaddr", TRUE))
    {
        /* collect value */
        *puLong = pEntry->Yiaddr.Value;
        
        return TRUE;
    }

   /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_StaticAddress_GetParamStringValue
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
X_CISCO_COM_StaticAddress_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT               pContextLinkObject = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pEntry = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pContextLinkObject->hContext;
    
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pEntry->Alias) <= *pUlSize)
        {
            AnscCopyString(pValue, pEntry->Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pEntry->Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Chaddr", TRUE))
    {
        /* collect value */
        if ( *pUlSize >= 18 )
        {
            _ansc_sprintf
                (
                    pValue,
                    "%02x:%02x:%02x:%02x:%02x:%02x",
                    pEntry->Chaddr[0],
                    pEntry->Chaddr[1],
                    pEntry->Chaddr[2],
                    pEntry->Chaddr[3],
                    pEntry->Chaddr[4],
                    pEntry->Chaddr[5]
                );
            pValue[17] = '\0'; 
            *pUlSize = AnscSizeOfString(pValue);
           
            return 0;
        }
        else
        {
            *pUlSize = sizeof(pEntry->Chaddr);
            return 1;
        }
    }
    
    if( AnscEqualString(ParamName, "DeviceName", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pEntry->DeviceName) <= *pUlSize)
        {
            AnscCopyString(pValue, pEntry->DeviceName);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pEntry->DeviceName)+1;
            return 1;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_SetParamBoolValue
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
X_CISCO_COM_StaticAddress_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(bValue);
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_SetParamIntValue
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
X_CISCO_COM_StaticAddress_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_SetParamUlongValue
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
X_CISCO_COM_StaticAddress_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT               pContextLinkObject = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pEntry = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pContextLinkObject->hContext;
    
    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Yiaddr", TRUE))
    {

        /* save update to backup */
        pEntry->Yiaddr.Value  =  uValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_SetParamStringValue
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
X_CISCO_COM_StaticAddress_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_DATAMODEL_DHCPV4            pDhcpv4            = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_LINK_OBJECT         pContextLinkObject = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pEntry = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pContextLinkObject->hContext;
    int                               rc                 = -1;
    UINT                              chAddr[7]          = {'\0'};

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        AnscCopyString(pDhcpv4->AliasOfX_CISCO_COM_SAddr, pEntry->Alias);

        AnscCopyString(pEntry->Alias, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Chaddr", TRUE))
    {
        /* save update to a temp array - This is required as sscanf puts a NULL character at the end which we dont have space for */
        rc = _ansc_sscanf
                (
                    pString,
                    "%x:%x:%x:%x:%x:%x",
                    chAddr,
                    chAddr+1,
                    chAddr+2,
                    chAddr+3,
                    chAddr+4,
                    chAddr+5
                );

        if(rc != 6)
        {
            CcspTraceWarning(("MAC Address not proerly formatted \n"));
            return FALSE;
        }
       
        /* Now copy the temp value to our back-up without NULL */ 
        pEntry->Chaddr[0] = chAddr[0];
        pEntry->Chaddr[1] = chAddr[1];
        pEntry->Chaddr[2] = chAddr[2];
        pEntry->Chaddr[3] = chAddr[3];
        pEntry->Chaddr[4] = chAddr[4];
        pEntry->Chaddr[5] = chAddr[5];

        return TRUE;
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_DeviceName", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pEntry->DeviceName,pString);
        return TRUE;

    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_StaticAddress_Validate
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
X_CISCO_COM_StaticAddress_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(puLength);
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4 )g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pDhcpStaAddr    = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pCxtLink->hContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink2         = NULL;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pDhcpStaAddr2   = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry       = NULL;
    BOOL                            bFound            = FALSE;
    CHAR                            strIP[32]         = {'\0'};
    UINT                            ip[4]             = {0};
    int                             rc                = -1;
    int                             i                 = 0;

    if ( pDhcpv4->AliasOfX_CISCO_COM_SAddr[0] )
    {
        /* save update to backup */
        bFound                = FALSE;
        pSListEntry           = AnscSListGetFirstEntry(&pDhcpv4->X_CISCO_COM_StaticAddressList);
        while( pSListEntry != NULL)
        {
            pCxtLink2         = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
            pSListEntry       = AnscSListGetNextEntry(pSListEntry);

            pDhcpStaAddr2  = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pCxtLink2->hContext;

            if( DHCPV4_STATICADDRESS_ENTRY_MATCH2(pDhcpStaAddr->Alias, pDhcpStaAddr2->Alias ) )
            {
                if ( (ANSC_HANDLE)pCxtLink2 == hInsContext )
                {
                    continue;
                }

                _ansc_strcpy(pReturnParamName, "Alias");

                bFound = TRUE;

                break;
            }
        }

        if ( bFound )
        {
#if COSA_DHCPV4_ROLLBACK_TEST        
            X_COM_CISCO_StaticAddress_Rollback(hInsContext);
#endif
            return FALSE;
        }
    }

    /* Make sure Static IP Address is properly formatted and isnt a network or multicast address */
    _ansc_memset(strIP,0,32);
    _ansc_strcpy(strIP,_ansc_inet_ntoa(*((struct in_addr *)(&pDhcpStaAddr->Yiaddr))));
    rc = sscanf(strIP,"%d.%d.%d.%d",ip,ip+1,ip+2,ip+3);
    if(rc != 4)
    {
        CcspTraceWarning(("Static IP Address is not properly formatted \n"));
        _ansc_strcpy(pReturnParamName, "Yiaddr");
        return FALSE;
    }

    for(i = 0; i < 4; i++)
    {
        if((ip[i] == 0) || (ip[i] == 255))
        {
            CcspTraceWarning(("Static IP Address can not be a network address or multicast \n"));
            _ansc_strcpy(pReturnParamName, "Yiaddr");
            return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_StaticAddress_Commit
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
X_CISCO_COM_StaticAddress_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if ( pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpsAddX_COM_CISCO_Saddr(NULL, pDhcpStaticAddress );

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCxtLink->bNew = FALSE;

            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        }
        else
        {
            DHCPV4_X_COM_CISCO_SADDR_SET_DEFAULTVALUE(pDhcpStaticAddress);

            if ( pDhcpv4->AliasOfX_CISCO_COM_SAddr[0] )
                AnscCopyString( pDhcpStaticAddress->Alias, pDhcpv4->AliasOfX_CISCO_COM_SAddr );
        }
    }
    else
    {
        returnStatus = CosaDmlDhcpsSetX_COM_CISCO_Saddr(NULL, pDhcpStaticAddress);

        if ( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CosaDmlDhcpsGetX_COM_CISCO_SaddrbyInsNum(NULL, pDhcpStaticAddress);
        }
    }
    
    AnscZeroMemory( pDhcpv4->AliasOfX_CISCO_COM_SAddr, sizeof(pDhcpv4->AliasOfX_CISCO_COM_SAddr) );
    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_StaticAddress_Rollback
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
X_CISCO_COM_StaticAddress_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_X_CISCO_COM_SADDR pDhcpStaAddr    = (PCOSA_DML_DHCPS_X_CISCO_COM_SADDR)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if ( pDhcpv4->AliasOfX_CISCO_COM_SAddr[0] )
        AnscCopyString( pDhcpStaAddr->Alias, pDhcpv4->AliasOfX_CISCO_COM_SAddr );

    if ( !pCxtLink->bNew )
    {
        CosaDmlDhcpsGetX_COM_CISCO_SaddrbyInsNum(NULL, (PCOSA_DML_DHCPS_SADDR)pDhcpStaAddr);
    }
    else
    {
        DHCPV4_X_COM_CISCO_SADDR_SET_DEFAULTVALUE(pDhcpStaAddr);
    }

    AnscZeroMemory( pDhcpv4->AliasOfX_CISCO_COM_SAddr, sizeof(pDhcpv4->AliasOfX_CISCO_COM_SAddr) );
    
    return returnStatus;
}

/***********************************************************************

 APIs for Object:

    DHCPv4.Server.Pool.{i}.

    *  Pool_GetEntryCount
    *  Pool_GetEntry
    *  Pool_AddEntry
    *  Pool_DelEntry
    *  Pool_GetParamBoolValue
    *  Pool_GetParamIntValue
    *  Pool_GetParamUlongValue
    *  Pool_GetParamStringValue
    *  Pool_SetParamBoolValue
    *  Pool_SetParamIntValue
    *  Pool_SetParamUlongValue
    *  Pool_SetParamStringValue
    *  Pool_Validate
    *  Pool_Commit
    *  Pool_Rollback

***********************************************************************/
static int is_invalid_unicast_ip_addr(unsigned int gw, unsigned int mask, unsigned int ipaddr)
{
    unsigned int subnet, bcast;

    /*Check whether ipaddr is zero, or a multicast address , or a loopback address*/
    if((ipaddr==0)||(ipaddr>=0xE0000000)||((ipaddr & 0xFF000000)==0x7F000000))
        return(1);
    subnet = gw & mask;
    bcast = subnet | (~mask);
    /*Check if ip addr is subnet or subnet broadcast address, or it is the same as gw*/
    if((ipaddr<=subnet)||(ipaddr>=bcast)||(ipaddr==gw))
        return(1);
    return(0);
}

static int is_pool_invalid(void *hInsContext)
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    ULONG                           minaddr, maxaddr, netmask, gateway;
    COSA_DML_DHCPS_POOL_CFG poolCfg;

    minaddr = ntohl(pPool->Cfg.MinAddress.Value);
    maxaddr = ntohl(pPool->Cfg.MaxAddress.Value);
    if(minaddr > maxaddr)
        return(1);
    poolCfg.InstanceNumber = pPool->Cfg.InstanceNumber;
    /*To get real GW & Netmask info*/
    CosaDmlDhcpsGetPoolCfg(NULL, &poolCfg);
    gateway = ntohl(poolCfg.IPRouters[0].Value);
    netmask = ntohl(poolCfg.SubnetMask.Value);
    if(is_invalid_unicast_ip_addr(gateway,netmask,minaddr)||is_invalid_unicast_ip_addr(gateway,netmask,maxaddr))
        return(1);
    return(0);
}

static BOOL isValidSubnetMask(u_int32_t netmask)
{
    int i = 32;

    while (i > 0 && !(netmask & 0x01))
    {
        i--;
	netmask >>= 1;
    }

    while (i > 0)
    {
        if (!(netmask & 0x01))
            return FALSE;
	i--;
	netmask >>= 1;
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Pool_GetEntryCount
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
Pool_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    return AnscSListQueryDepth( &pDhcpv4->PoolList );
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Pool_GetEntry
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
Pool_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry       = NULL;

    pSListEntry = AnscSListGetEntryByIndex(&pDhcpv4->PoolList, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_POOL_LINK_OBJECT(pSListEntry);
        *pInsNumber       = pCxtLink->InstanceNumber;
    }

    return pSListEntry;
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Pool_AddEntry
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
Pool_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = NULL;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = NULL;

    // return NULL; /* No dynamic addition/deletion of ServerPools at this point */
    
    pPool  = (PCOSA_DML_DHCPS_POOL_FULL)AnscAllocateMemory( sizeof(COSA_DML_DHCPS_POOL_FULL) );
    if ( !pPool )
    {
        goto EXIT2;
    }

    /* Set default value */
    DHCPV4_POOL_SET_DEFAULTVALUE(pPool);

    pCxtLink = (PCOSA_CONTEXT_POOL_LINK_OBJECT)AnscAllocateMemory( sizeof(COSA_CONTEXT_POOL_LINK_OBJECT) );
    if ( !pCxtLink )
    {
        goto EXIT1;
    }
    
    DHCPV4_POOL_INITIATION_CONTEXT(pCxtLink)

    pCxtLink->hContext       = (ANSC_HANDLE)pPool;
    pCxtLink->bNew           = TRUE;

    if ( !++pDhcpv4->maxInstanceOfPool )
    {
        pDhcpv4->maxInstanceOfPool = 1;
    }

    pPool->Cfg.InstanceNumber = pDhcpv4->maxInstanceOfPool;
    pCxtLink->InstanceNumber  = pPool->Cfg.InstanceNumber;
    *pInsNumber               = pPool->Cfg.InstanceNumber;

    _ansc_sprintf( pPool->Cfg.Alias, "Pool%lu", pPool->Cfg.InstanceNumber);

    /* Put into our list */
    CosaSListPushEntryByInsNum(&pDhcpv4->PoolList, (PCOSA_CONTEXT_LINK_OBJECT)pCxtLink);

    /* we recreate the configuration */
    // not saving pool entry, but saving maxInstanceOfPool
    pCxtLink->bNew           = FALSE;
    CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);

    // Write to SBAPI directly
    pPool->Cfg.bEnabled      = FALSE;
    CosaDmlDhcpsAddPool(NULL, pPool );

    return (ANSC_HANDLE)pCxtLink;

EXIT1:
    
    AnscFreeMemory(pPool);

EXIT2:        
    
    return NULL; /* return the handle */
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Pool_DelEntry
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
Pool_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInstance;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;

    //return ANSC_STATUS_FAILURE; /* No dynamic addition/deletion of ServerPools at this moment */

    /* Normally, two sublinks are empty because our framework will firstly 
              call delEntry for them before coming here. We needn't care them.
           */
    if(pPool->Cfg.InstanceNumber == 1)
    {
        AnscTraceFlow(("%s: pool instance %d is not allowed to delete\n", __FUNCTION__, pPool->Cfg.InstanceNumber));
        return ANSC_STATUS_FAILURE;
    }

    if ( !pCxtLink->bNew )
    {
        if(!pPool->Cfg.bAllowDelete)
        {
            AnscTraceFlow(("%s: pool instance %d is not allowed to delete\n", __FUNCTION__, pPool->Cfg.InstanceNumber));
            return ANSC_STATUS_FAILURE;
        }

        returnStatus = CosaDmlDhcpsDelPool(NULL, pPool->Cfg.InstanceNumber);
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return returnStatus;
        }
    }

    if (AnscSListPopEntryByLink(&pDhcpv4->PoolList, &pCxtLink->Linkage) )
    {
        // it should be safe to comment this out, 
        // because we never let bNew to be true, the entry is always saved on SBAPI only.
        //CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);

        AnscFreeMemory(pCxtLink->hContext);
        AnscFreeMemory(pCxtLink);
    }
    
    return returnStatus;
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_GetParamBoolValue
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
Pool_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pDhcpc            = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool   =  pDhcpc->Cfg.bEnabled;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "VendorClassIDExclude", TRUE))
    {
        /* collect value */
        /* *pBool   =  pDhcpc->Cfg.VendorClassIDExclude; */

        *pBool   = FALSE; /* Dummy Value */

        return TRUE;
    }

    if( AnscEqualString(ParamName, "ClientIDExclude", TRUE))
    {
        /* collect value */
        /* *pBool   =  pDhcpc->Cfg.ClientIDExclude; */
        *pBool   = FALSE; /* Dummy Value */ 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "UserClassIDExclude", TRUE))
    {
        /* collect value */
        /* *pBool   =  pDhcpc->Cfg.UserClassIDExclude; */
        *pBool   = FALSE; /* Dummy Value */ 

        return TRUE;
    }

    if( AnscEqualString(ParamName, "ChaddrExclude", TRUE))
    {
        /* collect value */
        /* *pBool   =  pDhcpc->Cfg.ChaddrExclude; */
        *pBool   = FALSE; /* Dummy Value */ 

        return TRUE;
    }


    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "DNSServersEnabled", TRUE))
    {
        /* collect value */
        *pBool   =  pDhcpc->Cfg.DNSServersEnabled;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "X_LGI-COM_ClearLanAllowedSubnetTable", TRUE))
    {
        /* collect value */
        *pBool   =  FALSE;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_GetParamIntValue
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
Pool_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "LeaseTime", TRUE))
    {
        char wanevent[12];
        char buf[12];

        wanevent[0] = 0;
        commonSyseventGet("wan-status", wanevent, sizeof(wanevent));
        if (strcmp (wanevent, "stopped") == 0)
        {
            // when modem is offline, lease time should be 120 seconds.

            if( syscfg_get(NULL, "dhcp_offline_lease_time", buf, sizeof(buf)) == 0)
            {
                *pInt = atoi(buf);
            }
            else
            {
                *pInt = 120;
            }
        }
        else
        {
            /* collect value */
            *pInt = pPool->Cfg.LeaseTime;
        }
        return TRUE;
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_TimeOffset", TRUE))
    {
        /* collect value */
        *pInt = pPool->Cfg.X_CISCO_COM_TimeOffset;
        
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_GetParamUlongValue
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
Pool_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    COSA_DML_DHCPS_POOL_FULL        poolTemp;

    AnscZeroMemory(&poolTemp, sizeof(COSA_DML_DHCPS_POOL_FULL));
    AnscCopyMemory(&poolTemp, pPool, sizeof(COSA_DML_DHCPS_POOL_FULL)); 

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Status", TRUE))
    {
        /* collect value */
        CosaDmlDhcpsGetPoolInfo( hInsContext, pPool->Cfg.InstanceNumber, &poolTemp.Info );

        *puLong  = poolTemp.Info.Status;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Order", TRUE))
    {
        /* collect value */
        *puLong  = pPool->Cfg.Order;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_Connected_Device_Number", TRUE))
    {
        /* collect value */
        CosaDmlDhcpsGetPoolInfo( hInsContext, pPool->Cfg.InstanceNumber, &poolTemp.Info );

        *puLong  = poolTemp.Info.X_CISCO_COM_Connected_Device_Number;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "VendorClassIDMode", TRUE))
    {
        /* collect value */
        /* *puLong  = pPool->Cfg.VendorClassIDMode; */
        *puLong  = 0; /* Not supported now */
        
        return TRUE;
    }
     
    if( AnscEqualString(ParamName, "MinAddress", TRUE))
    {
        /* collect value */
	*puLong  = pPool->Cfg.MinAddress.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "MaxAddress", TRUE))
    {
        /* collect value */
	*puLong  = pPool->Cfg.MaxAddress.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SubnetMask", TRUE))
    {
        /* collect value */
        CosaDmlDhcpsGetPoolCfg(NULL,&poolTemp.Cfg);
        *puLong  = poolTemp.Cfg.SubnetMask.Value;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}


BOOL validateNFixDomainName(char *DomainName, int len)
{
  int i = 0;
  BOOL valid = TRUE;
  
  while (i < len && valid)
  {
    if ((DomainName[i] >= 'a' && DomainName[i] <= 'z') || 
	(DomainName[i] >= 'A' && DomainName[i] <= 'Z') || 
	(DomainName[0] >= '0' && DomainName[i] <= '9') || 
	DomainName[i] == '.' || DomainName[i] == '-' ||
	DomainName[i] == '_')
      i++;
    else
    {
      if (DomainName[i] == 0)
	break;
      else
	valid = FALSE;
    }
  }
  
  if (valid == FALSE || i == len)
  {
    if (valid == FALSE)
      DomainName[0] = 0;
    else
      DomainName[i-1] = 0;
  }
  return valid;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Pool_GetParamStringValue
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
Pool_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    PUCHAR                          pString           = NULL;
    COSA_DML_DHCPS_POOL_CFG tmpCfg;
    BOOL bValidDomainName;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pPool->Cfg.Alias) < *pUlSize)
        {
            AnscCopyString(pValue, pPool->Cfg.Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pPool->Cfg.Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Interface", TRUE))
    {
        /* collect value */
        // TO DO: when changing pool 1 from syscfg to PSM, 
        // please following the other logic
        if(pPool->Cfg.InstanceNumber == 1)
        {
            // pPool->Cfg.Interface is name for pool 1
            pString = CosaUtilGetFullPathNameByKeyword
                    (
                        (PUCHAR)"Device.IP.Interface.",
                        (PUCHAR)"Name",
                        (PUCHAR)pPool->Cfg.Interface
                    );
            AnscTraceFlow(("%s: interface %s\n", __FUNCTION__, pPool->Cfg.Interface));  
            AnscTraceFlow(("%s: pString %s\n", __FUNCTION__, pString));               
            if ( pString )
            {
                if ( AnscSizeOfString((const char*)pString) < *pUlSize)
                {
                    AnscCopyString(pValue, (char*)pString);

                    AnscFreeMemory(pString);

                    return 0;
                }
                else
                {
                    *pUlSize = AnscSizeOfString((const char*)pString)+1;

                    AnscFreeMemory(pString);
                
                    return 1;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            //pPool->Cfg.Interface is instance number for other pool
            //_ansc_sprintf(pValue,"Device.IP.Interface.%s", pPool->Cfg.Interface);
            // pPool->Cfg.Interface is full path name
            AnscCopyString(pValue, pPool->Cfg.Interface);
            return 0;
        }

    }

    if( AnscEqualString(ParamName, "VendorClassID", TRUE))
    {
        /* collect value */
#if 0
        if ( AnscSizeOfString(pPool->Cfg.VendorClassID) < *pUlSize)
        {
            AnscCopyString(pValue, pPool->Cfg.VendorClassID);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pPool->Cfg.VendorClassID)+1;
            return 1;
        }
#endif
        AnscCopyString(pValue, ""); /* Not Supported */
    }

    if( AnscEqualString(ParamName, "ClientID", TRUE))
    {
        /* collect value */
#if 0
        if ( AnscSizeOfString(pPool->Cfg.ClientID) < *pUlSize)
        {
            AnscCopyString(pValue, pPool->Cfg.ClientID);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pPool->Cfg.ClientID)+1;
            return 1;
        }
#endif
        AnscCopyString(pValue, ""); /* Not Supported */
    }

    if( AnscEqualString(ParamName, "UserClassID", TRUE))
    {
        /* collect value */
#if 0
        if ( AnscSizeOfString(pPool->Cfg.UserClassID) < *pUlSize)
        {
            AnscCopyString(pValue, pPool->Cfg.UserClassID);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pPool->Cfg.UserClassID)+1;
            return 1;
        }
#endif
        AnscCopyString(pValue, ""); /* Not Supported */
    }

    if( AnscEqualString(ParamName, "Chaddr", TRUE))
    {
        /* collect value */
#if 0
        if ( AnscSizeOfString(pPool->Cfg.Chaddr) < *pUlSize)
        {
            _ansc_sprintf
                (
                    pValue, 
                    "%2x:%2x:%2x:%2x:%2x:%2x", 
                    pPool->Cfg.Chaddr[0],
                    pPool->Cfg.Chaddr[1],
                    pPool->Cfg.Chaddr[2],
                    pPool->Cfg.Chaddr[3],
                    pPool->Cfg.Chaddr[4],
                    pPool->Cfg.Chaddr[5]
                );
            
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pPool->Cfg.Chaddr)+1;
            return 1;
        }
#endif
        AnscCopyString(pValue, ""); /* Not Supported */
    }

    if( AnscEqualString(ParamName, "ChaddrMask", TRUE))
    {
        /* collect value */
#if 0
        if ( AnscSizeOfString(pPool->Cfg.ChaddrMask) < *pUlSize)
        {
            _ansc_sprintf
                (
                    pValue, 
                    "%2x:%2x:%2x:%2x:%2x:%2x", 
                    pPool->Cfg.ChaddrMask[0],
                    pPool->Cfg.ChaddrMask[1],
                    pPool->Cfg.ChaddrMask[2],
                    pPool->Cfg.ChaddrMask[3],
                    pPool->Cfg.ChaddrMask[4],
                    pPool->Cfg.ChaddrMask[5]
                );
            
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pPool->Cfg.ChaddrMask)+1;
            return 1;
        }
#endif
        AnscCopyString(pValue, ""); /* Not Supported */
    }

    if( AnscEqualString(ParamName, "ReservedAddresses", TRUE))
    {
        /* collect value */        
#if 0
        if ( CosaDmlGetIpaddrString(pValue, pUlSize, (PULONG)&pPool->Cfg.ReservedAddresses[0], COSA_DML_DHCP_MAX_RESERVED_ADDRESSES ) )
        {
            return 0;
        }
        else
        {
            return 1;
        }
#endif
        AnscCopyString(pValue, ""); /* Not Supported */
    }

    if( AnscEqualString(ParamName, "DNSServers", TRUE))
    {
        /* collect value */
        if ( CosaDmlGetIpaddrString((PUCHAR)pValue, pUlSize, (PULONG)&pPool->Cfg.DNSServers[0].Value, COSA_DML_DHCP_MAX_ENTRIES ) )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "DomainName", TRUE))
    {
        if (pPool->Cfg.InstanceNumber == 1){
            tmpCfg.InstanceNumber = pPool->Cfg.InstanceNumber;
            CosaDmlDhcpsGetPoolCfg(NULL,&tmpCfg);
            snprintf(pValue,sizeof(tmpCfg.DomainName),"%s", tmpCfg.DomainName);
        }else {
	  CcspTraceWarning(("%s: pPool->Cfg.DomainName: %s  0x%1x 0x%1x 0x%1x 0x%1x, sizeof: %d\n", 
			    __FUNCTION__, pPool->Cfg.DomainName, (signed) (pPool->Cfg.DomainName[0]), 
			    (signed) (pPool->Cfg.DomainName[1]), (signed) (pPool->Cfg.DomainName[2]), 
			    (signed) (pPool->Cfg.DomainName[3]), sizeof(pPool->Cfg.DomainName)));
	  bValidDomainName = validateNFixDomainName(pPool->Cfg.DomainName, sizeof(pPool->Cfg.DomainName));
	  snprintf(pValue,sizeof(pPool->Cfg.DomainName),"%s", pPool->Cfg.DomainName);
	  CcspTraceWarning(("%s: DomainName: %s, InstanceNumber: %d, bValidDomainName: %d\n", 
			    __FUNCTION__, pValue, pPool->Cfg.InstanceNumber, bValidDomainName));
	}
        return 0;
    }

    if( AnscEqualString(ParamName, "IPRouters", TRUE))
    {
        PULONG pTmpAddr;
        if(pPool->Cfg.InstanceNumber == 1)
        {
            memset(&tmpCfg, 0, sizeof(tmpCfg));
            tmpCfg.InstanceNumber = pPool->Cfg.InstanceNumber;
            CosaDmlDhcpsGetPoolCfg(NULL,&tmpCfg);
            pTmpAddr = &tmpCfg.IPRouters[0].Value;
        }
        else
            pTmpAddr = &pPool->Cfg.IPRouters[0].Value;

        /* collect value */
        if ( CosaDmlGetIpaddrString((PUCHAR)pValue, pUlSize, pTmpAddr, COSA_DML_DHCP_MAX_ENTRIES ) )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_SetParamBoolValue
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
Pool_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool            = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    BOOL bridgeInd = FALSE;

    AnscTraceFlow(("%s: ParamName = %s\n", __FUNCTION__, ParamName));
    /*CID: 65626 Unchecked return value*/
    if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeInd)) &&
       ( TRUE == bridgeInd ))
		return(FALSE);

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        if (Dhcpv4_Lan_MutexTryLock() != 0)
        {
            CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",ParamName));
            return FALSE;
        }
       /* save update to backup */
        pPool->Cfg.bEnabled   = bValue;
        Dhcpv4_Lan_MutexUnLock(); 
        return TRUE;
    }

    if( AnscEqualString(ParamName, "VendorClassIDExclude", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /* CID: 60425 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "ClientIDExclude", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /* CID: 60425 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "UserClassIDExclude", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /* CID: 60425 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "ChaddrExclude", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /* CID: 60425 Structurally dead code*/
    }

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "DNSServersEnabled", TRUE))
    {
        /* save update to backup */
        pPool->Cfg.DNSServersEnabled   = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "X_LGI-COM_ClearLanAllowedSubnetTable", TRUE))
    {
        if (CosaDmlClearLanAllowedSubnetTable() == 0)
        {
            return TRUE;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_SetParamIntValue
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
Pool_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    BOOL bridgeInd = FALSE;
       /*CID: 57411 Unchecked return value*/	
        if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeInd)) &&
           ( TRUE == bridgeInd ))
		return(FALSE);

    if (Dhcpv4_Lan_MutexTryLock() != 0)
    {
        CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",ParamName));
        return FALSE;
    }

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "LeaseTime", TRUE))
    {
        /*  enter only valid values 
            UNITS:
            seconds=iValue;(min-120 max-999)
            minutes=iValue/60;
            hours=iValue/3600;
            days=iValue/86400;
            weeks=iValue/604800;
            forever=-1;
        */
        if((iValue>=0) && (iValue<MINSECS))
	{
	    Dhcpv4_Lan_MutexUnLock();
            return FALSE;
	}
        if((iValue%WEEKS==0)  ||
            (iValue%DAYS==0)  ||
            (iValue%HOURS==0) || 
            (iValue%MIN==0)   ||
            (iValue<=MAXSECS) ||
            (iValue==-1)){
                /* save update to backup */
                pPool->Cfg.LeaseTime  = iValue;
                Dhcpv4_Lan_MutexUnLock();
                return TRUE;
        }
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_TimeOffset", TRUE))
    {
        /* save update to backup */
        pPool->Cfg.X_CISCO_COM_TimeOffset  = iValue;
        Dhcpv4_Lan_MutexUnLock();
        return TRUE;
    }

    Dhcpv4_Lan_MutexUnLock();
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_SetParamUlongValue
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
Pool_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    //COSA_DML_DHCPS_POOL_CFG poolCfg;    
    BOOL bridgeInd = FALSE;	

        /*CID: 70042 Unchecked return value*/
    if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeInd)) &&
       ( TRUE == bridgeInd ))
		return(FALSE);

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Order", TRUE))
    {
        /* save update to backup */
        pPool->Cfg.Order  = uValue;
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "VendorClassIDMode", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */

         /* CID: 68759 Structurally dead code*/
    }

    char * requestorStr = getRequestorString();
    char * currentTime = getTime();

    IS_UPDATE_ALLOWED_IN_DM(ParamName, requestorStr);

    if( AnscEqualString(ParamName, "MinAddress", TRUE))
    {
        IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pPool->Cfg.MinAddressUpdateSource);
        /* save update to backup */
        //poolCfg.InstanceNumber = pPool->Cfg.InstanceNumber;
        /* CosaDmlDhcpsGetPoolCfg(NULL, &poolCfg);*/
        char pFullName[256] = {0};
        char strval[64] = {0};
        ULONG size = sizeof(strval);
        ULONG gw = 0, mask = 0;
        /* Suppose LanManagementEntry to Pool 1:1 mapping for now */
        _ansc_sprintf(pFullName, "Device.X_CISCO_COM_DeviceControl.LanManagementEntry.%lu.LanIPAddress", pPool->Cfg.InstanceNumber);
        g_GetParamValueString(g_pDslhDmlAgent, pFullName, strval, &size);
        gw = _ansc_inet_addr(strval);

        size = sizeof(strval);
        AnscZeroMemory(strval, size);
        _ansc_sprintf(pFullName, "Device.X_CISCO_COM_DeviceControl.LanManagementEntry.%lu.LanSubnetMask", pPool->Cfg.InstanceNumber);
        g_GetParamValueString(g_pDslhDmlAgent, pFullName, strval, &size);
        mask = _ansc_inet_addr(strval);

        if( pPool->Cfg.InstanceNumber == 1 && 
            is_invalid_unicast_ip_addr(ntohl(gw),ntohl(mask), ntohl(uValue)))
            return(FALSE);

        if (Dhcpv4_Lan_MutexTryLock() != 0)
        {
            CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",ParamName));
            return FALSE;
        }

        pPool->Cfg.MinAddress.Value  = uValue;
        CcspTraceWarning(("RDKB_LAN_CONFIG_CHANGED: MinAddress of DHCP Range Changed ...\n"));
 
        memset( pPool->Cfg.MinAddressUpdateSource, 0, sizeof( pPool->Cfg.MinAddressUpdateSource ));
        AnscCopyString( pPool->Cfg.MinAddressUpdateSource, requestorStr );

        /*char buff[16] = {'\0'};
        snprintf(buff,sizeof(buff),"%ld",uValue);*/
        char buff[INET_ADDRSTRLEN] = {'\0'};
        inet_ntop(AF_INET, &(pPool->Cfg.MinAddress.Value) , buff, INET_ADDRSTRLEN); //convert IP address to string format
        char PartnerID[PARTNER_ID_LEN] = {0};
        if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
             UpdateJsonParam("Device.DHCPv4.Server.Pool.1.MinAddress",PartnerID, buff, requestorStr, currentTime);

        Dhcpv4_Lan_MutexUnLock();
        return TRUE;
    }

    if( AnscEqualString(ParamName, "MaxAddress", TRUE))
    {
        IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pPool->Cfg.MaxAddressUpdateSource);
        /* save update to backup */
        //poolCfg.InstanceNumber = pPool->Cfg.InstanceNumber;
        /* CosaDmlDhcpsGetPoolCfg(NULL, &poolCfg); */
        char pFullName[256] = {0};
        char strval[64] = {0};
        ULONG size = sizeof(strval);
        ULONG gw = 0, mask = 0;
        /* Suppose LanManagementEntry to Pool 1:1 mapping for now */
        _ansc_sprintf(pFullName, "Device.X_CISCO_COM_DeviceControl.LanManagementEntry.%lu.LanIPAddress", pPool->Cfg.InstanceNumber);
        g_GetParamValueString(g_pDslhDmlAgent, pFullName, strval, &size);
        gw = _ansc_inet_addr(strval);

        size = sizeof(strval);
        AnscZeroMemory(strval, size);
        _ansc_sprintf(pFullName, "Device.X_CISCO_COM_DeviceControl.LanManagementEntry.%lu.LanSubnetMask", pPool->Cfg.InstanceNumber);
        g_GetParamValueString(g_pDslhDmlAgent, pFullName, strval, &size);
        mask = _ansc_inet_addr(strval);

        if( ((pPool->Cfg.InstanceNumber == 1) && (is_invalid_unicast_ip_addr(ntohl(gw),ntohl(mask), ntohl(uValue)))) || (uValue < pPool->Cfg.MinAddress.Value))
            return(FALSE);

        if (Dhcpv4_Lan_MutexTryLock() != 0)
        {
            CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",ParamName));
            return FALSE;
        }

        pPool->Cfg.MaxAddress.Value  = uValue;
        CcspTraceWarning(("RDKB_LAN_CONFIG_CHANGED: MaxAddress of DHCP Range Changed ...\n"));
        
        memset( pPool->Cfg.MaxAddressUpdateSource, 0, sizeof( pPool->Cfg.MaxAddressUpdateSource ));
        AnscCopyString( pPool->Cfg.MaxAddressUpdateSource, requestorStr );

        /*char buff[16] = {'\0'};
        snprintf(buff,sizeof(buff),"%ld",uValue);*/
	char buff[INET_ADDRSTRLEN] = {'\0'};
        inet_ntop(AF_INET, &(pPool->Cfg.MaxAddress.Value) , buff, INET_ADDRSTRLEN);
        char PartnerID[PARTNER_ID_LEN] = {0};
        if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
             UpdateJsonParam("Device.DHCPv4.Server.Pool.1.MaxAddress",PartnerID, buff, requestorStr, currentTime);
        Dhcpv4_Lan_MutexUnLock();
        return TRUE;
    }

    if( AnscEqualString(ParamName, "SubnetMask", TRUE))
    {
        if (Dhcpv4_Lan_MutexTryLock() != 0)
        {
            CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",ParamName));
            return FALSE;
        }

        /* save update to backup */
        pPool->Cfg.SubnetMask.Value  = uValue;
        Dhcpv4_Lan_MutexUnLock();        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_SetParamStringValue
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
Pool_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    BOOL bridgeInd = FALSE;
		
    AnscTraceFlow(("%s: ParamName %s, \npString %s\n", __FUNCTION__, ParamName, pString));    
	is_usg_in_bridge_mode(&bridgeInd);
	if(bridgeInd)
		return(FALSE);

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {        
        /* Backup old alias firstly */
        AnscCopyString(pDhcpv4->AliasOfPool, pPool->Cfg.Alias);

        AnscCopyString(pPool->Cfg.Alias, pString);
        
        return TRUE;
    }

    if( AnscEqualString(ParamName, "Interface", TRUE))
    {
        /* save update to backup */
        int     length  = strlen(pString);
        if( length > 0 &&
            *(pString+length-1) == '.')
        {
            *(pString+length-1) = '\0';
        }

        if(pPool->Cfg.InstanceNumber == 1)
        {
            // Need to convert to Device.IP.Interface.x.Name
            CHAR    ucEntryParamName[256]       = {0};
            ULONG   ulEntryNameLen              = 256;
            CHAR    ucEntryNameValue[256]       = {0};

            _ansc_sprintf(ucEntryParamName, "%s%s", pString, ".Name");
               
            ulEntryNameLen = sizeof(ucEntryNameValue);
            if ( 0 == CosaGetParamValueString(ucEntryParamName, ucEntryNameValue, &ulEntryNameLen))
            {

                AnscTraceFlow(("%s:paramName %s, paramValue %s\n", __FUNCTION__, ucEntryParamName, ucEntryNameValue));
                if(strlen(ucEntryNameValue) == 0)
                {
                    // if the Name string is empty, return failure
                    AnscTraceFlow(("%s: %s is empty, can't set \n", __FUNCTION__, ucEntryParamName));
                    return FALSE;
                }
                AnscCopyString(pPool->Cfg.Interface, ucEntryNameValue);
            }
            else
            {
                // this should happen, but just in case it happens, we return failure
                //AnscCopyString(pPool->Cfg.Interface, pString);
                return FALSE;
            }
        }
        else
        {
            // For other pools, keep full path in Interface.
            if(length < 256)
            {
#if defined (MULTILAN_FEATURE)
                AnscCopyString(pPool->Cfg.Interface, pString);
#endif
                // extra error checking
#if defined (MULTILAN_FEATURE)
                if(!strstr(pPool->Cfg.Interface, "Device.IP.Interface."))
#else
		if(strstr(pPool->Cfg.Interface, "Device.IP.Interface."))
		{
                    AnscCopyString(pPool->Cfg.Interface, pString);
                }
                else
#endif
                {
                    AnscTraceFlow(("%s: interface path wrong %s\n", __FUNCTION__, pString));
                    return FALSE;
                }
            }
            else
            {
                AnscTraceFlow(("%s: interface too long %s\n", __FUNCTION__, pString));
                return FALSE;
            }
        }

        return TRUE;
    }

    if( AnscEqualString(ParamName, "VendorClassID", TRUE))
    {
            /* save update to backup */
        return FALSE; /* Not supported */
        /*CID: 56152 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "ClientID", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /*CID: 56152 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "UserClassID", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /*CID: 56152 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "Chaddr", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /*CID: 56152 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "ChaddrMask", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /*CID: 56152 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "ReservedAddresses", TRUE))
    {
        /* save update to backup */
        return FALSE; /* Not supported */
        /*CID: 56152 Structurally dead code*/
    }

    if( AnscEqualString(ParamName, "DNSServers", TRUE))
    {
        /* save update to backup */
        return CosaDmlSetIpaddr((PULONG)&pPool->Cfg.DNSServers[0].Value, pString, COSA_DML_DHCP_MAX_ENTRIES);
    }

    if( AnscEqualString(ParamName, "DomainName", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pPool->Cfg.DomainName, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IPRouters", TRUE))
    {
        BOOL ret = FALSE;
        if (Dhcpv4_Lan_MutexTryLock() != 0)
        {
            CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",ParamName));
            return FALSE;
        }

        /* save update to backup */
        ret = CosaDmlSetIpaddr((PULONG)&pPool->Cfg.IPRouters[0].Value, pString, COSA_DML_DHCP_MAX_ENTRIES);
        Dhcpv4_Lan_MutexUnLock();
        return ret;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Pool_Validate
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
Pool_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Pool_Commit
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
Pool_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    // only validate it for first pool, should changed to better validation for second pool
    if( pPool->Cfg.InstanceNumber == 1 && 
        is_pool_invalid(hInsContext))
    {
        AnscTraceFlow(("%s: not valid pool\n", __FUNCTION__));
        return(ANSC_STATUS_FAILURE);
    }
    else
    {
        AnscTraceFlow(("%s: valid pool, pPool->Cfg.InstanceNumber = %d\n", __FUNCTION__, pPool->Cfg.InstanceNumber));
    }

    if (Dhcpv4_Lan_MutexTryLock() != 0)
    {
        CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",__FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }

    if ( pCxtLink->bNew )
    {
        AnscTraceFlow(("%s: new pool, add to SBAPI\n", __FUNCTION__));
        returnStatus = CosaDmlDhcpsAddPool(NULL, pPool );

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCxtLink->bNew = FALSE;

            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        }
        else
        {
            DHCPV4_POOL_SET_DEFAULTVALUE(pPool);
            
            if ( pDhcpv4->AliasOfClient[0] )
                AnscCopyString( pPool->Cfg.Alias, pDhcpv4->AliasOfPool );
        }
    }
    else
    {
        returnStatus = CosaDmlDhcpsSetPoolCfg(NULL, &pPool->Cfg);

        if ( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CosaDmlDhcpsGetPoolCfg(NULL, &pPool->Cfg);
        }
    }
    
    AnscZeroMemory( pDhcpv4->AliasOfPool, sizeof(pDhcpv4->AliasOfPool) );
    Dhcpv4_Lan_MutexUnLock();
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Pool_Rollback
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
Pool_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtLink->hContext;

    if ( pDhcpv4->AliasOfPool[0] )
        AnscCopyString( pPool->Cfg.Alias, pDhcpv4->AliasOfPool );

    if ( !pCxtLink->bNew )
    {
        CosaDmlDhcpsGetPoolCfg( NULL, &pPool->Cfg );
    }
    else
    {
        DHCPV4_POOL_SET_DEFAULTVALUE(pPool);
    }
    
    AnscZeroMemory( pDhcpv4->AliasOfPool, sizeof(pDhcpv4->AliasOfPool) );
    
    return returnStatus;
}

/***********************************************************************

 APIs for Object:

    DHCPv4.Server.Pool.{i}.StaticAddress.{i}.

    *  StaticAddress_GetEntryCount
    *  StaticAddress_GetEntry
    *  StaticAddress_AddEntry
    *  StaticAddress_IsUpdated
    *  StaticAddress_Synchronize
    *  StaticAddress_DelEntry
    *  StaticAddress_GetParamBoolValue
    *  StaticAddress_GetParamIntValue
    *  StaticAddress_GetParamUlongValue
    *  StaticAddress_GetParamStringValue
    *  StaticAddress_SetParamBoolValue
    *  StaticAddress_SetParamIntValue
    *  StaticAddress_SetParamUlongValue
    *  StaticAddress_SetParamStringValue
    *  StaticAddress_Validate
    *  StaticAddress_Commit
    *  StaticAddress_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        StaticAddress_GetEntryCount
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
StaticAddress_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    return AnscSListQueryDepth( &pCxtLink->StaticAddressList );
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        StaticAddress_GetEntry
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
StaticAddress_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry          = NULL;

    pSListEntry = AnscSListGetEntryByIndex(&pCxtPoolLink->StaticAddressList, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
        *pInsNumber       = pCxtLink->InstanceNumber;
    }

    return pSListEntry;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        StaticAddress_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/

BOOL
StaticAddress_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4          pMyObject    = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    BOOL                            bIsUpdated   = TRUE;


    /*
        We can use one rough granularity interval to get whole table in case
        that the updating is too frequent.
        */
    if ( ( AnscGetTickInSeconds() - pMyObject->PreviousVisitTime ) < COSA_DML_DHCPV4_STATICADDRESS_ACCESS_INTERVAL )
    {
        bIsUpdated  = FALSE;
    }
    else
    {
        pMyObject->PreviousVisitTime =  AnscGetTickInSeconds();
        bIsUpdated  = TRUE;
    }

    return bIsUpdated;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        StaticAddress_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
StaticAddress_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DHCPV4  pDhcpv4 = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    ANSC_STATUS returnStatus      = ANSC_STATUS_FAILURE;
    int ret =0;
    if (!pDhcpv4)
    {
        return returnStatus;
    }
    if (TRUE == pDhcpv4->syncStaticClientsTable)
    {
        ret = Dhcpv4_StaticClients_Synchronize();
        pDhcpv4->syncStaticClientsTable = FALSE;
        CcspTraceWarning((" %s return %d \n",__FUNCTION__,ret));
    }
    if (ret == 0)
    {
        returnStatus =  ANSC_STATUS_SUCCESS;
    }
    return returnStatus;
}
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        StaticAddress_AddEntry
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
StaticAddress_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = NULL;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    
    /* We need ask from backend */
    pDhcpStaticAddress  = (PCOSA_DML_DHCPS_SADDR)AnscAllocateMemory( sizeof(COSA_DML_DHCPS_SADDR) );
    if ( !pDhcpStaticAddress )
    {
        goto EXIT2;
    }

    DHCPV4_STATICADDRESS_SET_DEFAULTVALUE(pDhcpStaticAddress);

    pCxtLink = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory( sizeof(COSA_CONTEXT_LINK_OBJECT) );
    if ( !pCxtLink )
    {
        goto EXIT1;
    }

    pCxtLink->hContext       = (ANSC_HANDLE)pDhcpStaticAddress;
    pCxtLink->hParentTable   = (ANSC_HANDLE)pCxtPoolLink;
    pCxtLink->bNew           = TRUE;
    
    if ( !++pCxtPoolLink->maxInstanceOfStaticAddress )
    {
        pCxtPoolLink->maxInstanceOfStaticAddress = 1;
    }
    pDhcpStaticAddress->InstanceNumber = pCxtPoolLink->maxInstanceOfStaticAddress;
    pCxtLink->InstanceNumber           = pDhcpStaticAddress->InstanceNumber; 
    *pInsNumber                        = pDhcpStaticAddress->InstanceNumber;

    _ansc_sprintf( pDhcpStaticAddress->Alias, "StaticAddress%lu", pDhcpStaticAddress->InstanceNumber);

    /* Put into our list */
    CosaSListPushEntryByInsNum(&pCxtPoolLink->StaticAddressList, (PCOSA_CONTEXT_LINK_OBJECT)pCxtLink);

    /* we recreate the configuration */
    CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);

    return (ANSC_HANDLE)pCxtLink;    

EXIT1:
        
    AnscFreeMemory(pDhcpStaticAddress);
    
EXIT2:   
        
    return NULL;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        StaticAddress_DelEntry
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
StaticAddress_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL       pDhcsPool            = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if (Dhcpv4_StaticClients_MutexTryLock() != 0)
    {
        CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",__FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }

    AnscTraceFlow(("%s: pool instance %d, addr instance %d\n", __FUNCTION__, pDhcsPool->Cfg.InstanceNumber, pDhcpStaticAddress->InstanceNumber));    
    if ( !pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpsDelSaddr( NULL, pDhcsPool->Cfg.InstanceNumber, pDhcpStaticAddress->InstanceNumber );
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            Dhcpv4_StaticClients_MutexUnLock();
            return returnStatus;
        }
    }

    /* Firstly we del this entry */
    if ( AnscSListPopEntryByLink(&pCxtPoolLink->StaticAddressList, &pCxtLink->Linkage) )
    {
        CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        AnscFreeMemory(pCxtLink->hContext);
        AnscFreeMemory(pCxtLink);
    }
    Dhcpv4_StaticClients_MutexUnLock();
    return returnStatus;
        
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_GetParamBoolValue
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
StaticAddress_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool  =  pDhcpStaticAddress->bEnabled;
        
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_GetParamIntValue
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
StaticAddress_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_GetParamUlongValue
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
StaticAddress_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Yiaddr", TRUE))
    {
        *puLong = pDhcpStaticAddress->Yiaddr.Value;
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        StaticAddress_GetParamStringValue
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
StaticAddress_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpStaticAddress->Alias) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpStaticAddress->Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpStaticAddress->Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Chaddr", TRUE))
    {
        /* collect value */
        if ( sizeof(pDhcpStaticAddress->Chaddr) <= *pUlSize)
        {
            _ansc_sprintf
                (
                    pValue,
                    "%02x:%02x:%02x:%02x:%02x:%02x",
                    pDhcpStaticAddress->Chaddr[0],
                    pDhcpStaticAddress->Chaddr[1],
                    pDhcpStaticAddress->Chaddr[2],
                    pDhcpStaticAddress->Chaddr[3],
                    pDhcpStaticAddress->Chaddr[4],
                    pDhcpStaticAddress->Chaddr[5]
                );
            pValue[17] = '\0'; 
            *pUlSize = AnscSizeOfString(pValue);
           
            return 0;
        }
        else
        {
            *pUlSize = sizeof(pDhcpStaticAddress->Chaddr);
            return 1;
        }
    }
    
    if( AnscEqualString(ParamName, "X_CISCO_COM_DeviceName", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpStaticAddress->DeviceName) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpStaticAddress->DeviceName);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpStaticAddress->DeviceName)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_Comment", TRUE))
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpStaticAddress->Comment) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpStaticAddress->Comment);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpStaticAddress->Comment)+1;
            return 1;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_SetParamBoolValue
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
StaticAddress_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    BOOL bridgeInd = FALSE;
		
	is_usg_in_bridge_mode(&bridgeInd);
	if(bridgeInd)
		return(FALSE);

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        pDhcpStaticAddress->bEnabled  = bValue;
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_SetParamIntValue
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
StaticAddress_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_SetParamUlongValue
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
StaticAddress_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    BOOL bridgeInd = FALSE;
        /*CID: 75188 Unchecked return value*/
    if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeInd)) &&
       ( TRUE == bridgeInd ))
		return(FALSE);

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Yiaddr", TRUE))
    {
#if 1/*added by song to check whether the IP is in the range*/
        COSA_DML_DHCPS_POOL_CFG poolCfg;
        poolCfg.InstanceNumber = 1;
        CosaDmlDhcpsGetPoolCfg(NULL, &poolCfg);
        if(( ntohl(uValue)< ntohl(poolCfg.MinAddress.Value))||( ntohl(uValue)> ntohl(poolCfg.MaxAddress.Value) )){
            return(FALSE);
        }
#endif
        /* save update to backup */
        pDhcpStaticAddress->Yiaddr.Value  =  uValue;

        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_SetParamStringValue
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
StaticAddress_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pPoolLink            = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;
    int                             rc                   = -1;
    UINT                            chAddr[7]            = {'\0'}, zeroMac[6];
    BOOL bridgeInd = FALSE;
        /*CID: 73040 Unchecked return value*/
        if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeInd)) &&
           ( TRUE == bridgeInd ))
		return(FALSE);

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        AnscCopyString(pPoolLink->AliasOfStaAddr, pDhcpStaticAddress->Alias);

        AnscCopyString(pDhcpStaticAddress->Alias, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Chaddr", TRUE))
    {
        /* check if pString doesn't hold null or whitespaces */
        if(AnscValidStringCheck((PUCHAR)pString) != TRUE)
            return FALSE;

        /* save update to a temp array - This is required as sscanf puts a NULL character at the end which we dont have space for */
        rc = _ansc_sscanf
                (
                    pString,
                    "%x:%x:%x:%x:%x:%x",
                    chAddr,
                    chAddr+1,
                    chAddr+2,
                    chAddr+3,
                    chAddr+4,
                    chAddr+5
                );

        if(rc != 6)
        {
            CcspTraceWarning(("MAC Address not proerly formatted \n"));
            return FALSE;
        }
        memset(zeroMac, 0, sizeof(zeroMac));
        if((chAddr[0] & 1)||(memcmp(zeroMac,chAddr,sizeof(zeroMac))==0))
            return(FALSE);
        /* Now copy the temp value to our back-up without NULL */ 
        pDhcpStaticAddress->Chaddr[0] = chAddr[0];
        pDhcpStaticAddress->Chaddr[1] = chAddr[1];
        pDhcpStaticAddress->Chaddr[2] = chAddr[2];
        pDhcpStaticAddress->Chaddr[3] = chAddr[3];
        pDhcpStaticAddress->Chaddr[4] = chAddr[4];
        pDhcpStaticAddress->Chaddr[5] = chAddr[5];

        return TRUE;
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_DeviceName", TRUE))
    {
        /* save update to backup */
	    if ( ( sizeof(pDhcpStaticAddress->DeviceName) - 1 )  > AnscSizeOfString(pString))
    	{
			AnscCopyString(pDhcpStaticAddress->DeviceName,pString);
			return TRUE;
    	}
		else
		{
			CcspTraceWarning(("'%s' value should be less than (%d) charecters\n", ParamName, ( sizeof(pDhcpStaticAddress->DeviceName) - 1 )));
		}
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_Comment", TRUE))
    {
        /* save update to backup */
        if ( sizeof(pDhcpStaticAddress->Comment) > AnscSizeOfString(pString))
        {
            AnscCopyString(pDhcpStaticAddress->Comment,pString);
            return TRUE;
        }
        else
        {
            CcspTraceWarning(("'%s' value should be less than (%d) charecters\n", ParamName, ( sizeof(pDhcpStaticAddress->Comment) - 1 )));
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        StaticAddress_Validate
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
StaticAddress_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);
#if 0 /*removed by song*/
    /* Parent hasn't set, we don't permit child is set.*/
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaAddr      = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtPoolLink      = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink2         = NULL;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaAddr2     = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry       = NULL;
    BOOL                            bFound            = FALSE;
    UCHAR                           strIP[32]         = {'\0'};
    UINT                            ip[4]             = {0};
    int                             rc                = -1;
    int                             i                 = 0;
    ULONG                           ipaddr, netmask, gateway;

    if ( pCxtPoolLink->bNew )
    {
#if COSA_DHCPV4_ROLLBACK_TEST        
        StaticAddress_Rollback(hInsContext);
#endif
        return FALSE;
    }
    if ( pCxtPoolLink->AliasOfStaAddr[0] )
    {
        /* save update to backup */
        bFound                = FALSE;
        pSListEntry           = AnscSListGetFirstEntry(&pCxtPoolLink->StaticAddressList);
        while( pSListEntry != NULL)
        {
            pCxtLink2         = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
            pSListEntry       = AnscSListGetNextEntry(pSListEntry);

            pDhcpStaAddr2  = (PCOSA_DML_DHCPS_SADDR)pCxtLink2->hContext;

            if( DHCPV4_STATICADDRESS_ENTRY_MATCH2(pDhcpStaAddr->Alias, pDhcpStaAddr2->Alias ) )
            {
                if ( (ANSC_HANDLE)pCxtLink2 == hInsContext )
                {
                    continue;
                }

                _ansc_strcpy(pReturnParamName, "Alias");

                bFound = TRUE;

                break;
            }
        }

        if ( bFound )
        {
#if COSA_DHCPV4_ROLLBACK_TEST        
            StaticAddress_Rollback(hInsContext);
#endif
            return FALSE;
        }
    }
    /* Make sure Static IP Address is properly formatted and isnt a network or multicast address */
    ipaddr = pDhcpStaAddr->Yiaddr.Value;
    netmask = pPool->Cfg.SubnetMask.Value;
    gateway = pPool->Cfg.IPRouters[0].Value;
    if (IPv4Addr_IsLoopback(ipaddr)
            || IPv4Addr_IsMulticast(ipaddr)
            || IPv4Addr_IsBroadcast(ipaddr, gateway, netmask)
            || IPv4Addr_IsNetworkAddr(ipaddr, gateway, netmask)
            || !IPv4Addr_IsSameNetwork(ipaddr, gateway, netmask))
    {
        CcspTraceWarning(("%s: Invalid Static IP Address \n", __FUNCTION__));
        _ansc_strcpy(pReturnParamName, "Yiaddr");
        return FALSE;
    }

    /* Make sure Static IP Address is not same as our GW address */
    if((pDhcpStaAddr->Yiaddr.Value == pPool->Cfg.IPRouters[0].Value))
    {
        CcspTraceWarning(("Static IP Address same as the GW \n"));
        _ansc_strcpy(pReturnParamName, "Yiaddr");
        return FALSE;
    }

    if((pDhcpStaAddr->Yiaddr.Value < pPool->Cfg.MinAddress.Value) || (pDhcpStaAddr->Yiaddr.Value > pPool->Cfg.MaxAddress.Value))
    {
        CcspTraceWarning(("Static IP Address not in Range \n"));
        _ansc_strcpy(pReturnParamName, "Yiaddr");
        return FALSE;
    }
#endif

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        StaticAddress_Commit
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
StaticAddress_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaticAddress   = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPS_POOL_FULL       pPool                = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    if (Dhcpv4_StaticClients_MutexTryLock() != 0)
    {
        CcspTraceWarning(("%s not supported already macbinding blob update is inprogress \n",__FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }

    if ( pCxtLink->bNew ){
	if(pDhcpStaticAddress->bEnabled==FALSE){
		pDhcpStaticAddress->ActiveFlag = FALSE;
        Dhcpv4_StaticClients_MutexUnLock();
		return(ANSC_STATUS_SUCCESS);
	}
        returnStatus = CosaDmlDhcpsAddSaddr(NULL, pPool->Cfg.InstanceNumber, pDhcpStaticAddress );

        if ( returnStatus == ANSC_STATUS_SUCCESS ){
            pCxtLink->bNew = FALSE;
            pDhcpStaticAddress->ActiveFlag = TRUE;
            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        }else{
            if ( pCxtPoolLink->AliasOfStaAddr[0] )
                AnscCopyString( pDhcpStaticAddress->Alias, pCxtPoolLink->AliasOfStaAddr );
        }
    }else{
	if(pDhcpStaticAddress->ActiveFlag==TRUE){
		if(pDhcpStaticAddress->bEnabled==FALSE){/*to del this entry from backend*/
			returnStatus = CosaDmlDhcpsDelSaddr( NULL, pPool->Cfg.InstanceNumber, pDhcpStaticAddress->InstanceNumber );
			if(returnStatus == ANSC_STATUS_SUCCESS)
				pDhcpStaticAddress->ActiveFlag = FALSE;
		}else{/*to update this entry*/
			returnStatus = CosaDmlDhcpsSetSaddr(NULL, pPool->Cfg.InstanceNumber, pDhcpStaticAddress);
		}
	}else{
		if(pDhcpStaticAddress->bEnabled==FALSE){
            Dhcpv4_StaticClients_MutexUnLock();
			return(ANSC_STATUS_SUCCESS);
		}else{/*Add this entry to backend*/
			returnStatus = CosaDmlDhcpsAddSaddr(NULL, pPool->Cfg.InstanceNumber, pDhcpStaticAddress );
			if(returnStatus == ANSC_STATUS_SUCCESS)
				pDhcpStaticAddress->ActiveFlag = TRUE;
		}
	}
    }
    
    AnscZeroMemory( pCxtPoolLink->AliasOfStaAddr, sizeof(pCxtPoolLink->AliasOfStaAddr) );
    Dhcpv4_StaticClients_MutexUnLock();
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        StaticAddress_Rollback
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
StaticAddress_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_SADDR           pDhcpStaAddr      = (PCOSA_DML_DHCPS_SADDR)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT  pCxtPoolLink      = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;


    if ( pCxtPoolLink->AliasOfStaAddr[0] )
        AnscCopyString( pDhcpStaAddr->Alias, pCxtPoolLink->AliasOfStaAddr );
#if 0/*Removed by song*/
    PCOSA_DML_DHCPS_POOL_FULL       pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;
    if ( !pCxtLink->bNew )
    {
        CosaDmlDhcpsGetSaddrbyInsNum(NULL, pPool->Cfg.InstanceNumber, pDhcpStaAddr);
    }
    else
    {
        DHCPV4_STATICADDRESS_SET_DEFAULTVALUE(pDhcpStaAddr);
    }

    AnscZeroMemory( pCxtPoolLink->AliasOfStaAddr, sizeof(pCxtPoolLink->AliasOfStaAddr) );
#endif    
    return returnStatus;
}


/***********************************************************************

 APIs for Object:

    DHCPv4.Server.Pool.{i}.Option.{i}.

    *  Option1_GetEntryCount
    *  Option1_GetEntry
    *  Option1_AddEntry
    *  Option1_DelEntry
    *  Option1_GetParamBoolValue
    *  Option1_GetParamIntValue
    *  Option1_GetParamUlongValue
    *  Option1_GetParamStringValue
    *  Option1_SetParamBoolValue
    *  Option1_SetParamIntValue
    *  Option1_SetParamUlongValue
    *  Option1_SetParamStringValue
    *  Option1_Validate
    *  Option1_Commit
    *  Option1_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Option1_GetEntryCount
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
Option1_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;

    return AnscSListQueryDepth( &pCxtLink->OptionList );
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Option1_GetEntry
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
Option1_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink             = NULL;
    PSINGLE_LINK_ENTRY                pSListEntry          = NULL;

    pSListEntry = AnscSListGetEntryByIndex(&pCxtPoolLink->OptionList, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
        *pInsNumber       = pCxtLink->InstanceNumber;
    }

    return pSListEntry;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Option1_AddEntry
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
Option1_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL         pPool                = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink             = NULL;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpOption          = NULL;
    PCOSA_DATAMODEL_DHCPV4            pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    
    /* We need ask from backend */
    //printf("%s\n", __FUNCTION__);
    if(pPool->Cfg.InstanceNumber == 1)
    {
        AnscTraceFlow(("%s: not supporting addinging option for pool 1.\n", __FUNCTION__));
        return (ANSC_HANDLE)ANSC_STATUS_FAILURE;
    }

    pDhcpOption  = (PCOSA_DML_DHCPSV4_OPTION)AnscAllocateMemory( sizeof(COSA_DML_DHCPSV4_OPTION) );
    if ( !pDhcpOption )
    {
        goto EXIT2;
    }

    DHCPV4_POOLOPTION_SET_DEFAULTVALUE(pDhcpOption);

    pCxtLink = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory( sizeof(COSA_CONTEXT_LINK_OBJECT) );
    if ( !pCxtLink )
    {
        goto EXIT1;
    }

    pCxtLink->hContext       = (ANSC_HANDLE)pDhcpOption;
    pCxtLink->hParentTable   = (ANSC_HANDLE)pCxtPoolLink;
    pCxtLink->bNew           = TRUE;
    
    //printf("%s: pCxtPoolLink->maxInstanceOfOption = %d\n", __FUNCTION__, pCxtPoolLink->maxInstanceOfOption);
    if ( !++pCxtPoolLink->maxInstanceOfOption )
    {
        pCxtPoolLink->maxInstanceOfOption = 1;
    }
    pDhcpOption->InstanceNumber = pCxtPoolLink->maxInstanceOfOption;
    pCxtLink->InstanceNumber           = pDhcpOption->InstanceNumber; 
    *pInsNumber                        = pDhcpOption->InstanceNumber;

    _ansc_sprintf( (char*)pDhcpOption->Alias, "Option%lu", pDhcpOption->InstanceNumber);

    /* Put into our list */
    CosaSListPushEntryByInsNum(&pCxtPoolLink->OptionList, (PCOSA_CONTEXT_LINK_OBJECT)pCxtLink);

    /* we recreate the configuration */
    pCxtLink->bNew = FALSE;
    CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
    
    //printf("%s: option tag %d, option value %s\n", __FUNCTION__, pDhcpOption->Tag, pDhcpOption->Value);
    CosaDmlDhcpsAddOption(NULL, pPool->Cfg.InstanceNumber, pDhcpOption );

    return (ANSC_HANDLE)pCxtLink;    

EXIT1:
        
    AnscFreeMemory(pDhcpOption);
    
EXIT2:   
        
    return NULL;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Option1_DelEntry
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
Option1_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                       returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPS_POOL_FULL         pDhcpPool            = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpOption          = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;


    AnscTraceFlow(("%s: pool instance %d, option instance %d\n", __FUNCTION__, pDhcpPool->Cfg.InstanceNumber, pDhcpOption->InstanceNumber));    
    //printf("%s: pool instance %d, option instance %d\n", __FUNCTION__, pDhcpPool->Cfg.InstanceNumber, pDhcpOption->InstanceNumber);
    if(pDhcpPool->Cfg.InstanceNumber == 1)
    {
        AnscTraceFlow(("%s: not supporting deleting option for pool 1.\n", __FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }
    if ( !pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpsDelOption( NULL, pDhcpPool->Cfg.InstanceNumber, pDhcpOption->InstanceNumber );
        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            return returnStatus;
        }
    }

    /* Firstly we del this entry */
    //printf("%s: remove entry.\n", __FUNCTION__);
    if ( AnscSListPopEntryByLink(&pCxtPoolLink->OptionList, &pCxtLink->Linkage) )
    {
//        PCOSA_DATAMODEL_DHCPV4            pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
//        if ( pCxtLink->bNew )
//            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        
        AnscFreeMemory(pCxtLink->hContext);
        AnscFreeMemory(pCxtLink);
    }
    
    return returnStatus;
        
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option1_GetParamBoolValue
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
Option1_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION        pDhcpOption   = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool  =  pDhcpOption->bEnabled;
        
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option1_GetParamIntValue
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
Option1_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option1_GetParamUlongValue
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
Option1_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION        pDhcpOption   = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Tag", TRUE) )
    {
        /* collect value */
        *puLong    =  pDhcpOption->Tag;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Option1_GetParamStringValue
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
Option1_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION        pDhcpOption   = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpOption->Alias) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpOption->Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpOption->Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Value", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pDhcpOption->Value) < *pUlSize)
        {
            //printf("%s: size, %d, %d\n", __FUNCTION__, AnscSizeOfString(pDhcpOption->Value), *pUlSize);
            AnscCopyString(pValue, (char*)pDhcpOption->Value);
            return 0;
        }
        else
        {
            //printf("%s: size two, %d, %d\n", __FUNCTION__, AnscSizeOfString(pDhcpOption->Value), *pUlSize);
            *pUlSize = AnscSizeOfString((const char*)pDhcpOption->Value) + 1;
        
            return 1;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option1_SetParamBoolValue
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
Option1_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpOption     = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        pDhcpOption->bEnabled  = bValue;
        
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option1_SetParamIntValue
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
Option1_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);

    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option1_SetParamUlongValue
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
Option1_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION        pDhcpOption   = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Tag", TRUE) )
    {
        /* save update to backup */        
        pDhcpOption->Tag =  uValue;

        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option1_SetParamStringValue
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
Option1_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpOption          = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pPoolLink            = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;
    errno_t     rc =  -1;
    int ind = -1;

    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("Alias", strlen("Alias"),ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
       rc = strcpy_s(pPoolLink->AliasOfOption, sizeof(pPoolLink->AliasOfOption), pDhcpOption->Alias);
       if(rc != EOK)
       {
          ERR_CHK(rc);
          return FALSE;
       }
       rc = STRCPY_S_NOCLOBBER(pDhcpOption->Alias, sizeof(pDhcpOption->Alias), pString);
       if(rc != EOK)
       {
          ERR_CHK(rc);
          return FALSE;
       }
       return TRUE;
    }

    rc = strcmp_s("Value", strlen("Value"),ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
       /* save update to backup */
       rc = STRCPY_S_NOCLOBBER((char *)pDhcpOption->Value, sizeof(pDhcpOption->Value), pString);
       if(rc != EOK)
       {
          ERR_CHK(rc);
          return FALSE;
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
        Option1_Validate
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
Option1_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(puLength);
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpPoolOption   = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtPoolLink      = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink2         = NULL;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpPoolOption2  = NULL;
    PSINGLE_LINK_ENTRY                pSListEntry       = NULL;
    BOOL                              bFound            = FALSE;

    /* Parent hasn't set, we don't permit child is set.*/
    if ( pCxtPoolLink->AliasOfOption[0] )
    {
        /* save update to backup */
        bFound                = FALSE;
        pSListEntry           = AnscSListGetFirstEntry(&pCxtPoolLink->OptionList);
        while( pSListEntry != NULL)
        {
            pCxtLink2         = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
            pSListEntry       = AnscSListGetNextEntry(pSListEntry);

            pDhcpPoolOption2  = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink2->hContext;

            if( DHCPV4_POOLOPTION_ENTRY_MATCH(pDhcpPoolOption->Alias, pDhcpPoolOption2->Alias ) )
            {
                if ( (ANSC_HANDLE)pCxtLink2 == hInsContext )
                {
                    continue;
                }

                _ansc_strcpy(pReturnParamName, "Alias");

                bFound = TRUE;

                break;
            }
        }

        if ( bFound )
        {
            return FALSE;
        }
    }

    
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Option1_Commit
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
Option1_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                       returnStatus         = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpOption          = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtPoolLink         = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPS_POOL_FULL         pPool                = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;
    PCOSA_DATAMODEL_DHCPV4            pDhcpv4              = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    //printf("%s: %d \n", __FUNCTION__, pCxtLink->bNew);
    if ( pCxtLink->bNew )
    {
        returnStatus = CosaDmlDhcpsAddOption(NULL, pPool->Cfg.InstanceNumber, pDhcpOption );

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCxtLink->bNew = FALSE;

            CosaDhcpv4RegSetDhcpv4Info(pDhcpv4);
        }
        else
        {
            DHCPV4_POOLOPTION_SET_DEFAULTVALUE(pDhcpOption);

            if ( pCxtPoolLink->AliasOfOption[0] )
                AnscCopyString( pDhcpOption->Alias, pCxtPoolLink->AliasOfOption );
        }
    }
    else
    {
        returnStatus = CosaDmlDhcpsSetOption(NULL, pPool->Cfg.InstanceNumber, pDhcpOption);

        if ( returnStatus != ANSC_STATUS_SUCCESS)
        {
            CosaDmlDhcpsGetOptionbyInsNum(NULL, pPool->Cfg.InstanceNumber, pDhcpOption);
        }
    }
    
    AnscZeroMemory( pCxtPoolLink->AliasOfOption, sizeof(pCxtPoolLink->AliasOfOption) );
    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Option1_Rollback
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
Option1_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                       returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT         pCxtLink          = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_OPTION          pDhcpPoolOption   = (PCOSA_DML_DHCPSV4_OPTION)pCxtLink->hContext;
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtPoolLink      = (PCOSA_CONTEXT_POOL_LINK_OBJECT)pCxtLink->hParentTable;
    PCOSA_DML_DHCPS_POOL_FULL         pPool             = (PCOSA_DML_DHCPS_POOL_FULL)pCxtPoolLink->hContext;

    //printf("%s:\n", __FUNCTION__);
    if ( pCxtPoolLink->AliasOfOption[0] )
        AnscCopyString( pDhcpPoolOption->Alias, pCxtPoolLink->AliasOfOption );

    if ( !pCxtLink->bNew )
    {
        CosaDmlDhcpsGetOptionbyInsNum(NULL, pPool->Cfg.InstanceNumber, pDhcpPoolOption);
    }
    else
    {
        DHCPV4_POOLOPTION_SET_DEFAULTVALUE(pDhcpPoolOption);
    }

    AnscZeroMemory( pCxtPoolLink->AliasOfOption, sizeof(pCxtPoolLink->AliasOfOption) );
    
    return returnStatus;
}


/***********************************************************************

 APIs for Object:

    DHCPv4.Server.Pool.{i}.Client.{i}.

    *  Client2_GetEntryCount
    *  Client2_GetEntry
    *  Client2_IsUpdated
    *  Client2_Synchronize
    *  Client2_GetParamBoolValue
    *  Client2_GetParamIntValue
    *  Client2_GetParamUlongValue
    *  Client2_GetParamStringValue
    *  Client2_SetParamBoolValue
    *  Client2_SetParamIntValue
    *  Client2_SetParamUlongValue
    *  Client2_SetParamStringValue
    *  Client2_Validate
    *  Client2_Commit
    *  Client2_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client2_GetEntryCount
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
Client2_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;

    return pCxtLink->NumberOfClient;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Client2_GetEntry
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
Client2_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtLink          = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;

    if ( nIndex >= pCxtLink->NumberOfClient )
        return NULL;
    
    *pInsNumber  = nIndex + 1; 
    
    return (ANSC_HANDLE)&pCxtLink->pClientList[nIndex];
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
BOOL
Client2_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtLink        = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    BOOL                              bIsUpdated   = TRUE;

    /* 
        We can use one rough granularity interval to get whole table in case 
        that the updating is too frequent.
        */
    if ( ( AnscGetTickInSeconds() - pCxtLink->PreviousVisitTimeOfClient ) < COSA_DML_DHCPV4_ACCESS_INTERVAL_POOLCLIENT )
    {
        bIsUpdated  = FALSE;
    }
    else
    {
        pCxtLink->PreviousVisitTimeOfClient =  AnscGetTickInSeconds();
        bIsUpdated  = TRUE;
    }

    return bIsUpdated;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client2_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Client2_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                       returnStatus    = ANSC_STATUS_SUCCESS;    
    PCOSA_CONTEXT_POOL_LINK_OBJECT    pCxtLink        = (PCOSA_CONTEXT_POOL_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_CLIENT          pDhcpsClient    = NULL;
    PCOSA_DML_DHCPSV4_CLIENTCONTENT   pDhcpsClientContent = NULL;
    ULONG                             uIndex           = 0;
    ULONG                             count           = 0;
    BOOL bridgeInd = FALSE;

    /* Free all */
    if ( pCxtLink->pClientList )
    {
        AnscFreeMemory(pCxtLink->pClientList);
        pCxtLink->pClientList    = NULL;

        pDhcpsClientContent = pCxtLink->pClientContentList;
        for( uIndex =0; uIndex < pCxtLink->NumberOfClient; uIndex++ )
        {
            if ( pDhcpsClientContent[uIndex].pIPAddress )
            {
                AnscFreeMemory(pDhcpsClientContent[uIndex].pIPAddress);
                pDhcpsClientContent[uIndex].pIPAddress = NULL;
            }
            
            if ( pDhcpsClientContent[uIndex].pOption )
            {
                AnscFreeMemory(pDhcpsClientContent[uIndex].pOption);            
                pDhcpsClientContent[uIndex].pOption = NULL;
            }
        }

        AnscFreeMemory(pCxtLink->pClientContentList);
        pCxtLink->pClientContentList    = NULL;

        pCxtLink->NumberOfClient = 0;
    }

        is_usg_in_bridge_mode(&bridgeInd);
	if(bridgeInd){
        pCxtLink->NumberOfClient= 0;
        returnStatus = ANSC_STATUS_SUCCESS;
    }else{
        /* Get all again */
        returnStatus = CosaDmlDhcpsGetClient
                    (
                        NULL,
                        pCxtLink->InstanceNumber,
                        &pDhcpsClient,
                        &pDhcpsClientContent,
                        &count
                    );

        if ( returnStatus == ANSC_STATUS_SUCCESS ){
            pCxtLink->pClientList        = pDhcpsClient;
            pCxtLink->pClientContentList = pDhcpsClientContent;
            pCxtLink->NumberOfClient  = count;
        }
    }

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_GetParamBoolValue
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

/*
 *  TBC --  Define the function prototype in the header file!!!
 */
extern void CosaDmlDhcpsGetPrevClientNumber(ULONG ulPoolInstanceNumber, ULONG *pNumber);

BOOL
Client2_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;    
    PCOSA_DML_DHCPSV4_CLIENT        pDhcpsClient    = (PCOSA_DML_DHCPSV4_CLIENT)hInsContext;
    PCOSA_DML_DHCPSV4_CLIENTCONTENT PClientContent    = NULL;
    ULONG n;
    
    PClientContent = (PCOSA_DML_DHCPSV4_CLIENTCONTENT)CosaDhcpv4GetClientContentbyClient(hInsContext);
    /* CID: 59902 Dereference null return value*/
    if (!PClientContent){
       CcspTraceError(("PClientContent is NULL\n")); 
       return FALSE;
    }   

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Active", TRUE) ){
        n = 0;
        CosaDmlDhcpsGetPrevClientNumber(1, &n);
        if(n<=6){
         /* collect value */
            if(!strncmp("172.16.12.", _ansc_inet_ntoa(*((struct in_addr*)&(PClientContent->pIPAddress[0].IPAddress))),10)) 
             	returnStatus = CosaDmlDhcpsPing(&(PClientContent->pIPAddress[0]));
            else
            	returnStatus = CosaDmlDhcpsARPing(&(PClientContent->pIPAddress[0]));
            if ( returnStatus == ANSC_STATUS_SUCCESS ){
                *pBool = TRUE;
            }else{
                *pBool = FALSE;
            }
        }else /*there are many clients, don't ping the clients*/
            *pBool = pDhcpsClient->Active;
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_GetParamIntValue
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
Client2_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_GetParamUlongValue
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
Client2_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(puLong);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client2_GetParamStringValue
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
Client2_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_DHCPSV4_CLIENT        pDhcpsClient    = (PCOSA_DML_DHCPSV4_CLIENT)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString(pDhcpsClient->Alias) < *pUlSize)
        {
            AnscCopyString(pValue, pDhcpsClient->Alias);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(pDhcpsClient->Alias)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_Comment", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pDhcpsClient->X_CISCO_COM_Comment) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pDhcpsClient->X_CISCO_COM_Comment);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pDhcpsClient->X_CISCO_COM_Comment)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_Interface", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pDhcpsClient->X_CISCO_COM_Interface) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pDhcpsClient->X_CISCO_COM_Interface);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pDhcpsClient->X_CISCO_COM_Interface)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_HostName", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pDhcpsClient->X_CISCO_COM_HostName) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pDhcpsClient->X_CISCO_COM_HostName);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pDhcpsClient->X_CISCO_COM_HostName)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "ClassId", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pDhcpsClient->ClassId) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pDhcpsClient->ClassId);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pDhcpsClient->ClassId)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "Chaddr", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pDhcpsClient->Chaddr) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pDhcpsClient->Chaddr);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pDhcpsClient->Chaddr)+1;
            return 1;
        }
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_SetParamBoolValue
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
Client2_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(bValue);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_SetParamIntValue
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
Client2_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_SetParamUlongValue
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
Client2_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(uValue);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_SetParamStringValue
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
Client2_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pString);

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE) )
    {
        /* save update to backup */
        return FALSE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Client2_Validate
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
Client2_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client2_Commit
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
Client2_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Client2_Rollback
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
Client2_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/***********************************************************************

 APIs for Object:

    DHCPv4.Server.Pool.{i}.Client.{i}.IPv4Address.{i}.

    *  IPv4Address2_GetEntryCount
    *  IPv4Address2_GetEntry
    *  IPv4Address2_IsUpdated
    *  IPv4Address2_Synchronize
    *  IPv4Address2_GetParamBoolValue
    *  IPv4Address2_GetParamIntValue
    *  IPv4Address2_GetParamUlongValue
    *  IPv4Address2_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        IPv4Address2_GetEntryCount
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
IPv4Address2_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_DHCPSV4_CLIENTCONTENT   PClientContent    = NULL;
    
    PClientContent = (PCOSA_DML_DHCPSV4_CLIENTCONTENT)CosaDhcpv4GetClientContentbyClient(hInsContext);

    if ( PClientContent )
    {
        return PClientContent->NumberofIPAddress;
    }
    else
        return 0;    
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        IPv4Address2_GetEntry
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
IPv4Address2_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DML_DHCPSV4_CLIENTCONTENT   PClientContent    = NULL;
    
    PClientContent = (PCOSA_DML_DHCPSV4_CLIENTCONTENT)CosaDhcpv4GetClientContentbyClient(hInsContext);

    if ( PClientContent )
    {
        *pInsNumber  = nIndex + 1; 
        return &PClientContent->pIPAddress[nIndex];
    }
    else
        return NULL;    

}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        IPv4Address2_GetParamBoolValue
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
IPv4Address2_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        IPv4Address2_GetParamIntValue
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
IPv4Address2_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        IPv4Address2_GetParamUlongValue
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
IPv4Address2_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_DHCPSV4_CLIENT_IPADDRESS     pIPAddress      = (PCOSA_DML_DHCPSV4_CLIENT_IPADDRESS)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "IPAddress", TRUE) )
    {
        /* collect value */
        *puLong = pIPAddress->IPAddress;

        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        IPv4Address2_GetParamStringValue
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
IPv4Address2_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_DHCPSV4_CLIENT_IPADDRESS     pIPAddress      = (PCOSA_DML_DHCPSV4_CLIENT_IPADDRESS)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "LeaseTimeRemaining", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pIPAddress->LeaseTimeRemaining) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pIPAddress->LeaseTimeRemaining);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pIPAddress->LeaseTimeRemaining)+1;
            return 1;
        }
    }

    if( AnscEqualString(ParamName, "X_CISCO_COM_LeaseTimeCreation", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pIPAddress->X_CISCO_COM_LeaseTimeCreation) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pIPAddress->X_CISCO_COM_LeaseTimeCreation);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pIPAddress->X_CISCO_COM_LeaseTimeCreation)+1;
            return 1;
        }
    }
    
    if( AnscEqualString(ParamName, "X_CISCO_COM_LeaseTimeDuration", TRUE) )
    {
        CosaDmlDhcpsGetLeaseTimeDuration(pIPAddress);
        /* collect value */
        if ( AnscSizeOfString((const char*)pIPAddress->X_CISCO_COM_LeaseTimeDuration) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pIPAddress->X_CISCO_COM_LeaseTimeDuration);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pIPAddress->X_CISCO_COM_LeaseTimeDuration)+1;
            return 1;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    DHCPv4.Server.Pool.{i}.Client.{i}.Option.{i}.

    *  Option2_GetEntryCount
    *  Option2_GetEntry
    *  Option2_IsUpdated
    *  Option2_Synchronize
    *  Option2_GetParamBoolValue
    *  Option2_GetParamIntValue
    *  Option2_GetParamUlongValue
    *  Option2_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Option2_GetEntryCount
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
Option2_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DML_DHCPSV4_CLIENTCONTENT   PClientContent    = NULL;
    
    PClientContent = (PCOSA_DML_DHCPSV4_CLIENTCONTENT)CosaDhcpv4GetClientContentbyClient(hInsContext);

    if ( PClientContent )
    {
        return PClientContent->NumberofOption;
    }
    else
        return 0;    
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Option2_GetEntry
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
Option2_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DML_DHCPSV4_CLIENTCONTENT   PClientContent    = NULL;
    
    PClientContent = (PCOSA_DML_DHCPSV4_CLIENTCONTENT)CosaDhcpv4GetClientContentbyClient(hInsContext);

    if ( PClientContent )
    {
        *pInsNumber  = nIndex + 1; 
        return &PClientContent->pOption[nIndex];
    }
    else
        return NULL;    
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option2_GetParamBoolValue
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
Option2_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option2_GetParamIntValue
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
Option2_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Option2_GetParamUlongValue
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
Option2_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_DHCPSV4_CLIENT_OPTION        pIPv4Option     = (PCOSA_DML_DHCPSV4_CLIENT_OPTION)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Tag", TRUE) )
    {
        /* collect value */
        *puLong  = pIPv4Option->Tag;

        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Option2_GetParamStringValue
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
Option2_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_DHCPSV4_CLIENT_OPTION        pIPv4Option     = (PCOSA_DML_DHCPSV4_CLIENT_OPTION)hInsContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Value", TRUE) )
    {
        /* collect value */
        if ( AnscSizeOfString((const char*)pIPv4Option->Value) < *pUlSize)
        {
            AnscCopyString(pValue, (char*)pIPv4Option->Value);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString((const char*)pIPv4Option->Value)+1;
            return 1;
        }
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}


/***********************************************************************

 APIs for Object:

    Device.DHCPv4.Server.Pool.{i}. X_LGI-COM_LanAllowedSubnetTable.{i}.

    *  LanAllowedSubnetTable_GetEntryCount
    *  LanAllowedSubnetTable_GetEntry
    *  LanAllowedSubnetTable_AddEntry
    *  LanAllowedSubnetTable_DelEntry
    *  LanAllowedSubnetTable_GetParamStringValue
    *  LanAllowedSubnetTable_SetParamStringValue
    *  LanAllowedSubnetTable_Commit
    *  LanAllowedSubnetTable_Validate
    *  LanAllowedSubnetTable_Rollback

***********************************************************************/
ULONG
LanAllowedSubnetTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_DHCPV4             pLgGw         = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    return AnscSListQueryDepth(&pLgGw->LanAllowedSubnetList);
}

ANSC_HANDLE
LanAllowedSubnetTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_DHCPV4     pMyObject         = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_LINK_OBJECT  pLinkObj          = NULL;
    PSINGLE_LINK_ENTRY         pSLinkEntry       = NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(&pMyObject->LanAllowedSubnetList, nIndex);
    if ( pSLinkEntry )
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber   =  pLinkObj->InstanceNumber;
        pMyObject->LanAllowedSubnetNextInsNum = pLinkObj->InstanceNumber+1;
    }

    return (ANSC_HANDLE)pLinkObj;
}

ANSC_HANDLE
LanAllowedSubnetTable_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_DHCPV4   pMyObject          = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj           = NULL;
    COSA_DML_LAN_Allowed_Subnet  *pLanAllowedSubnet  = NULL;

    pLinkObj = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
    if (!pLinkObj)
    {
        return NULL;
    }

    pLanAllowedSubnet = (COSA_DML_LAN_Allowed_Subnet *)AnscAllocateMemory(sizeof(COSA_DML_LAN_Allowed_Subnet));

    if (!pLanAllowedSubnet)
    {
        AnscFreeMemory(pLinkObj);
        return NULL;
    }

    if (pMyObject->LanAllowedSubnetNextInsNum == 0)
    {
        pMyObject->LanAllowedSubnetNextInsNum = 1;
    }
    /* now we have this link content */
    pLinkObj->InstanceNumber = pMyObject->LanAllowedSubnetNextInsNum;
    pLanAllowedSubnet->InstanceNumber = pMyObject->LanAllowedSubnetNextInsNum;
    pMyObject->LanAllowedSubnetNextInsNum++;

    _ansc_sprintf(pLanAllowedSubnet->Alias, "LanAllowedSubnet-%d", (int)pLinkObj->InstanceNumber);

    pLinkObj->hContext      = (ANSC_HANDLE)pLanAllowedSubnet;
    pLinkObj->hParentTable  = NULL;
    pLinkObj->bNew          = TRUE;

    CosaSListPushEntryByInsNum((PSLIST_HEADER)&pMyObject->LanAllowedSubnetList, pLinkObj);
    CosaLanAllowedSubnetListAddInfo((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pLinkObj);

    *pInsNumber = pLinkObj->InstanceNumber;
    return pLinkObj;
}
ULONG
LanAllowedSubnetTable_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    PCOSA_DATAMODEL_DHCPV4       pMyObject          = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_LINK_OBJECT    pLinkObj           = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    COSA_DML_LAN_Allowed_Subnet  *pLanAllowedSubnet = (COSA_DML_LAN_Allowed_Subnet*)pLinkObj->hContext;

    if (pLinkObj->InstanceNumber == DEFAULT_LAN_SUBNET_INST)
    {
        /* MVXREQ-675: Default LAN subnet shouldn't be deleted */
        return ANSC_STATUS_FAILURE;
    }

    AnscSListPopEntryByLink((PSLIST_HEADER)&pMyObject->LanAllowedSubnetList, &pLinkObj->Linkage);

    if (pLinkObj->bNew)
    {
        CosaLanAllowedSubnetListDelInfo((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pLinkObj);
    }
    else
    {
        CosaDmlLAN_Allowed_Subnet_DelEntry(pLinkObj->InstanceNumber);
    }

    AnscFreeMemory(pLanAllowedSubnet);
    AnscFreeMemory(pLinkObj);
    return ANSC_STATUS_SUCCESS;
}

ULONG
LanAllowedSubnetTable_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj         = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_LAN_Allowed_Subnet *pLanAllowedSubnet = (COSA_DML_LAN_Allowed_Subnet*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "LanAllowedSubnetIP", TRUE))
    {
        AnscCopyString(pValue, pLanAllowedSubnet->SubnetIP);
        return 0;
    }
    if( AnscEqualString(ParamName, "LanAllowedSubnetMask", TRUE))
    {
        AnscCopyString(pValue, pLanAllowedSubnet->SubnetMask);
        return 0;
    }
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        AnscCopyString(pValue, pLanAllowedSubnet->Alias);
        return 0;
    }

    return -1;
}

BOOL
LanAllowedSubnetTable_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj         = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_LAN_Allowed_Subnet *pLanAllowedSubnet = (COSA_DML_LAN_Allowed_Subnet*)pLinkObj->hContext;

    if (pLinkObj->InstanceNumber == DEFAULT_LAN_SUBNET_INST)
    {
        /* MVXREQ-675: Default LAN subnet is read-only */
        return FALSE;
    }

    if (AnscEqualString(ParamName, "LanAllowedSubnetMask", TRUE))
    {
        _ansc_snprintf(pLanAllowedSubnet->SubnetMask, sizeof(pLanAllowedSubnet->SubnetMask), "%s", pString);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "LanAllowedSubnetIP", TRUE))
    {
        _ansc_snprintf(pLanAllowedSubnet->SubnetIP, sizeof(pLanAllowedSubnet->SubnetIP), "%s", pString);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Alias", TRUE))
    {
        _ansc_snprintf(pLanAllowedSubnet->Alias, sizeof(pLanAllowedSubnet->Alias), "%s", pString);
        return TRUE;
    }

    return FALSE;
}

BOOL
LanAllowedSubnetTable_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    unsigned int subnetFirstMask = 0, subnetSecondMask = 0, subnetThirdMask = 0, subnetFourthMask = 0;
    unsigned int noOfAllowedSubnet = 0, index = 0;
    PCOSA_CONTEXT_LINK_OBJECT    pLinkObj           = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_LAN_Allowed_Subnet  *pLanAllowedSubnet = (COSA_DML_LAN_Allowed_Subnet*)pLinkObj->hContext;
    COSA_DML_LAN_Allowed_Subnet  LanAllowedSubnetExist;
    ANSC_IPV4_ADDRESS               lanSubnetBuf;

    if (AnscSizeOfString(pLanAllowedSubnet->SubnetMask))
    {
        inet_pton(AF_INET, pLanAllowedSubnet->SubnetMask, &lanSubnetBuf);

        if (!(sscanf(pLanAllowedSubnet->SubnetMask, "%d.%d.%d.%d", &subnetFirstMask, &subnetSecondMask,
              &subnetThirdMask, &subnetFourthMask) == 4) ||
              !((subnetFirstMask <= 255) && (subnetSecondMask <= 255) && (subnetThirdMask <= 255) && (subnetFourthMask <= 255)))
        {
            return FALSE;
        }

        if (lanSubnetBuf.Value == 0x000000FE ||  //7
            lanSubnetBuf.Value == 0x000000FC ||  //6
            lanSubnetBuf.Value == 0x000000F8 ||  //5
            lanSubnetBuf.Value == 0x000000F0 ||  //4
            lanSubnetBuf.Value == 0x000000E0 ||  //3
            lanSubnetBuf.Value == 0x000000C0 ||  //2
            lanSubnetBuf.Value == 0x00000080 ||  //1
            lanSubnetBuf.Value == 0x00000000 ||  //0
            lanSubnetBuf.Value == 0xFEFFFFFF)
       {
            return FALSE;
       }
    }

    //   DUPLICATE / OVERLAP Value Check
    if (AnscSizeOfString(pLanAllowedSubnet->SubnetIP))
    {
        noOfAllowedSubnet = CosaDmlLAN_Allowed_Subnet_GetNumberOfEntries();
        for (index = 0; index < noOfAllowedSubnet; index++)
        {
            // Avoid checking overlap entry for same index
            if (LANAllowedSubnet_InsGetIndex(pLanAllowedSubnet->InstanceNumber) == index)
            {
                continue;
            }

            CosaDmlLAN_Allowed_Subnet_GetEntryByIndex(index, &LanAllowedSubnetExist);

            if ((strncmp(LanAllowedSubnetExist.SubnetIP, pLanAllowedSubnet->SubnetIP, sizeof(LanAllowedSubnetExist.SubnetIP)) == 0))
            {
                CcspTraceError(("FUNC - %s: ERROR: Subnet IP %s Already exist\n", __FUNCTION__, pLanAllowedSubnet->SubnetIP));
                AnscCopyString(pReturnParamName, "LanAllowedSubnetIP");
                *puLength = AnscSizeOfString("LanAllowedSubnetIP");
                return FALSE;
            }
        }

        //   RANGE IP Check
        if (sscanf(pLanAllowedSubnet->SubnetIP, "%d.%d.%d.%d", &subnetFirstMask, &subnetSecondMask,
              &subnetThirdMask, &subnetFourthMask) == 4)
        {
            //Range 10.0.0.0   -   10.255.255.255
            if (subnetFirstMask == 10)
            {
                if ((subnetSecondMask > 255) || (subnetThirdMask > 255) || (subnetFourthMask > 253))
                    return FALSE;
            }
            //Range 172.16.0.0   -   172.31.255.255
            else if (subnetFirstMask == 172)
            {
                if (!(subnetSecondMask >=16 && subnetSecondMask <=31) || (subnetThirdMask > 255) || (subnetFourthMask > 253))
                    return FALSE;
            }
            //Range 192.168.0.0 – 192.168.255.255
            else if ((subnetFirstMask == 192) && (subnetSecondMask == 168))
            {
                if ((subnetThirdMask > 255) || (subnetFourthMask > 253))
                    return FALSE;
            }
	    // Invalid Private IP range
	    else {
                return FALSE;
	    }
        }
    }

    /* MVXREQ-674: Auto-correct subnet mask according to the subnet IP.
     * If the subnet mask is not correct as per RFC 1918,
     * set the subnet mask as default mask of that network class.
     */
    if (AnscSizeOfString(pLanAllowedSubnet->SubnetIP))
    {
        unsigned int subnetIP[4];
	int retCnt = 0;
	sscanf(pLanAllowedSubnet->SubnetIP, "%d.%d.%d.%d", &subnetIP[0], &subnetIP[1], &subnetIP[2], &subnetIP[3]);
	retCnt = sscanf(pLanAllowedSubnet->SubnetMask, "%d.%d.%d.%d", &subnetFirstMask, &subnetSecondMask, &subnetThirdMask, &subnetFourthMask);

	if (10 == subnetIP[0])                                                        /* 10.x.x.x/8 - 10.x.x.x/24*/
	{
            if (0 == retCnt || !isValidSubnetMask(lanSubnetBuf.Value & 0xFFFFFFFF) ||
                !(255 == subnetFirstMask && 255 >= subnetSecondMask &&
                  255 >= subnetThirdMask && 0 == subnetFourthMask))
            {
                /* Setting default Subnet Mask for class A network */
                _ansc_snprintf(pLanAllowedSubnet->SubnetMask, sizeof(pLanAllowedSubnet->SubnetMask), "%s", "255.0.0.0");
            }
	}
	else if (172 == subnetIP[0] && 16 <= subnetIP[1] && 31 >= subnetIP[1])        /*172.16.x.x/16 - 172.31.x.x/24 */
	{
	    if (0 == retCnt || !isValidSubnetMask(lanSubnetBuf.Value & 0xFFFFFFFF) ||
                !(255 == subnetFirstMask && 255 == subnetSecondMask &&
                  255 >= subnetThirdMask && 0 == subnetFourthMask))
	    {
                /* Setting default Subnet Mask for class B network */
                _ansc_snprintf(pLanAllowedSubnet->SubnetMask, sizeof(pLanAllowedSubnet->SubnetMask), "%s", "255.255.0.0");
	    }
	}
	else                                                                          /* 192.168.0.x/24 */
	{
	    if (0 == retCnt || !isValidSubnetMask(lanSubnetBuf.Value & 0xFFFFFFFF) ||
                !(255 == subnetFirstMask && 255 == subnetSecondMask &&
                  255 == subnetThirdMask && 0 == subnetFourthMask))
            {
                /* Setting default Subnet Mask for class C network */
                _ansc_snprintf(pLanAllowedSubnet->SubnetMask, sizeof(pLanAllowedSubnet->SubnetMask), "%s", "255.255.255.0");
            }
	}
    }

    return TRUE;
}

ULONG
LanAllowedSubnetTable_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj         = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_LAN_Allowed_Subnet *pLanAllowedSubnet = (COSA_DML_LAN_Allowed_Subnet*)pLinkObj->hContext;
    PCOSA_DATAMODEL_DHCPV4   pMyObject        = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    if (pLinkObj->bNew)
    {
        if (CosaDmlLAN_Allowed_Subnet_AddEntry(pLanAllowedSubnet) != ANSC_STATUS_SUCCESS)
            return -1;
        CosaLanAllowedSubnetListDelInfo((ANSC_HANDLE)pMyObject, (ANSC_HANDLE)pLinkObj);
        pLinkObj->bNew = FALSE;
    }
    else
    {
        if (CosaDmlLAN_Allowed_Subnet_SetConf(pLanAllowedSubnet->InstanceNumber, pLanAllowedSubnet) != ANSC_STATUS_SUCCESS)
        {
            CosaDmlLAN_Allowed_Subnet_GetConf(pLanAllowedSubnet->InstanceNumber, pLanAllowedSubnet);
            return -1;
        }
    }

    return 0;
}
ULONG
LanAllowedSubnetTable_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj         = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_LAN_Allowed_Subnet *pLanAllowedSubnet = (COSA_DML_LAN_Allowed_Subnet *)pLinkObj->hContext;

    if(CosaDmlLAN_Allowed_Subnet_GetConf(pLanAllowedSubnet->InstanceNumber, pLanAllowedSubnet) != ANSC_STATUS_SUCCESS)
    {
        memset(pLanAllowedSubnet->SubnetIP, 0,sizeof(pLanAllowedSubnet->SubnetIP));
        memset(pLanAllowedSubnet->SubnetMask, 0,sizeof(pLanAllowedSubnet->SubnetMask));
        return -1;
    }

    return 0;
}




/***********************************************************************

 APIs for Object:

    Device.DHCPv4.Server.Pool.{i}. X_LGI-COM_LanBlockedSubnetTable.{i}.

    *  LanBlockedSubnetTable_GetEntryCount
    *  LanBlockedSubnetTable_GetEntry
    *  LanBlockedSubnetTable_GetParamUlongValue
***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        LanBlockedSubnetTable_GetEntryCount
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
LanBlockedSubnetTable_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;

    return AnscSListQueryDepth( &pDhcpv4->LanBlockedSubnetList );
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
            LanBlockedSubnetTable_GetGuestNetworkIP
            (
                char                 *pValueIP
            );

    description:

        This function is called to retrieve the guest network ip

    argument:   char                 *pValueIP,
                buffer to get the ip address;

    return:     TRUE (on success) or FALSE (on failure)

**********************************************************************/
BOOL
LanBlockedSubnetTable_GetGuestNetworkIP
    (
        char                 *pValueIP
    )
{
    char    *strValue = NULL;
    int     retPsmGet = CCSP_SUCCESS;

    retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l3net.6.V4Addr", NULL, &strValue);
    if (retPsmGet == CCSP_SUCCESS) {
        strncpy(pValueIP, strValue, strlen(strValue));

        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
    } else {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
            LanBlockedSubnetTable_GetGuestNetworkMask
            (
                char                 *pValueMask
            );

    description:

        This function is called to retrieve the guest network subnet mask

    argument:   char                 *pValueMask,
                buffer to get the ip subnet mask;

    return:     TRUE (on success) or FALSE (on failure)

**********************************************************************/
BOOL
LanBlockedSubnetTable_GetGuestNetworkMask
    (
        char                 *pValueMask
    )
{
    char    *strValue = NULL;
    int     retPsmGet = CCSP_SUCCESS;

    retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l3net.6.V4SubnetMask", NULL, &strValue);
    if (retPsmGet == CCSP_SUCCESS) {
        strncpy(pValueMask, strValue, strlen(strValue));

        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
    } else {
        return FALSE;
    }

    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        LanBlockedSubnetTable_GetEntry
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
LanBlockedSubnetTable_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_DHCPV4          pDhcpv4           = (PCOSA_DATAMODEL_DHCPV4)g_pCosaBEManager->hDhcpv4;
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = NULL;
    PSINGLE_LINK_ENTRY              pSListEntry          = NULL;

    pSListEntry = AnscSListGetEntryByIndex(&pDhcpv4->LanBlockedSubnetList, nIndex);

    if ( pSListEntry )
    {
        pCxtLink          = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSListEntry);
        *pInsNumber       = nIndex + 1;
    }

    return pSListEntry;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        LanBlockedSubnetTable_GetParamUlongValue
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
LanBlockedSubnetTable_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCxtLink             = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_DHCPSV4_LANBLOCKED    pDhcpLanBlockedSubnet= (PCOSA_DML_DHCPSV4_LANBLOCKED)pCxtLink->hContext;
    ANSC_STATUS                     returnStatus         = ANSC_STATUS_SUCCESS;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "LanBlockedSubnetIP", TRUE))
    {
        char                        IPAddr[80]={0};
        ULONG                       ulSize = sizeof(IPAddr);
        BOOL                        update_subnetip = FALSE; 

         // 1st 3 slots are reserved for MTA/CM/Router info
         if (pCxtLink->InstanceNumber == 1)
         {
             // get MTA IP
             CosaDmlDiGetMTAIPAddress(NULL, IPAddr, &ulSize);
             update_subnetip = TRUE;
         }
         else if (pCxtLink->InstanceNumber == 2)
         {
             // get CM IP
             // The IPv4 address is needed, so use CosaDmlDiGetCMIPv4Address()
             // since CosaDmlDiGetCMIPAddress() returns the IPv6 address is available.
             CosaDmlDiGetCMIPv4Address(NULL, IPAddr, &ulSize);
             update_subnetip = TRUE;
         }
         else if (pCxtLink->InstanceNumber == 3)
         {
             // get Router IP
             CosaDmlDiGetRouterIPAddress(NULL, IPAddr, &ulSize);
             update_subnetip = TRUE;
         }
         else if (pCxtLink->InstanceNumber == 4)
         {
             // get guest network IP
             LanBlockedSubnetTable_GetGuestNetworkIP(IPAddr);
             update_subnetip = TRUE;
         }
         else if (pCxtLink->InstanceNumber == 14)
         {
             char *interface = "brlan0";
             uint32_t ip = (uint32_t) CosaUtilGetIfAddr (interface);
             unsigned char *a = (unsigned char *) &ip;

             /*
                The value returned by CosaUtilGetIfAddr() is in network byte order
                (ie it's always big endian). Processing as bytes allows this code to
                be agnostic of target endianness.
             */
             sprintf (IPAddr, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
             update_subnetip = TRUE;
         }
 
        if (update_subnetip)
        { 
             sscanf (IPAddr, "%hhu.%hhu.%hhu.%hhu",
                 &pDhcpLanBlockedSubnet->SubnetIP.Dot[0],
                 &pDhcpLanBlockedSubnet->SubnetIP.Dot[1],
                 &pDhcpLanBlockedSubnet->SubnetIP.Dot[2],
                 &pDhcpLanBlockedSubnet->SubnetIP.Dot[3]);
        }

        *puLong = pDhcpLanBlockedSubnet->SubnetIP.Value;

        return TRUE;
    }
    else if ( AnscEqualString(ParamName, "LanBlockedSubnetMask", TRUE))
    {
        char                        IPMask[80]={0};
        ULONG                       ulSize = sizeof(IPMask);
        extern ANSC_HANDLE          bus_handle;
        BOOL                        update_subnetmask=FALSE;

        // 1st 3 slots are reserved for MTA/CM/Router info
        if (pCxtLink->InstanceNumber == 1)
        {
            //get MTA SubnetMask
            parameterValStruct_t    varStruct;
            int                     size = sizeof(IPMask);

            varStruct.parameterName = "Device.X_CISCO_COM_MTA.SubnetMask";
            varStruct.parameterValue = IPMask;
            if (COSAGetParamValueByPathName(bus_handle, &varStruct, &size) != ANSC_STATUS_SUCCESS)
            {
                return FALSE;
            }
            update_subnetmask=TRUE;

        }
        else if (pCxtLink->InstanceNumber == 2)
        {
            // get CM SubnetMask
            parameterValStruct_t    varStruct;
            int                     size = sizeof(IPMask);

            varStruct.parameterName = "Device.X_CISCO_COM_CableModem.SubnetMask";
            varStruct.parameterValue = IPMask;
            if (COSAGetParamValueByPathName(bus_handle, &varStruct, &size) != ANSC_STATUS_SUCCESS)
            {
                return FALSE;
            }
            update_subnetmask=TRUE;
        }
        else if (pCxtLink->InstanceNumber == 3)
        {
            // get Router SubnetMask
            commonSyseventGet("ipv4_wan_subnet",IPMask, sizeof(IPMask));
            update_subnetmask=TRUE;
        }
        else if (pCxtLink->InstanceNumber == 4)
        {
            // get guest network Mask
            LanBlockedSubnetTable_GetGuestNetworkMask(IPMask);
            update_subnetmask=TRUE;
        }
        else if (pCxtLink->InstanceNumber == 14)
        {
            // get brlan0 network Mask
	    ULONG netmask=CosaUtilIoctlXXX("brlan0","netmask",NULL);
#if defined (_XB6_PRODUCT_REQ_) ||  defined (_COSA_BCM_ARM_)
        sprintf(IPMask, "%d.%d.%d.%d",(netmask & 0xff),((netmask >> 8) & 0xff),
			((netmask >> 16) & 0xff),(netmask >> 24));
#else
        sprintf(IPMask, "%d.%d.%d.%d", (netmask >> 24),((netmask >> 16) & 0xff),
			((netmask >> 8) & 0xff),(netmask & 0xff));
#endif
            update_subnetmask=TRUE;
        }

        if (update_subnetmask)
        {
            sscanf (IPMask, "%hhu.%hhu.%hhu.%hhu",
                &pDhcpLanBlockedSubnet->SubnetMask.Dot[0],
                &pDhcpLanBlockedSubnet->SubnetMask.Dot[1],
                &pDhcpLanBlockedSubnet->SubnetMask.Dot[2],
                &pDhcpLanBlockedSubnet->SubnetMask.Dot[3]);
        }

        *puLong = pDhcpLanBlockedSubnet->SubnetMask.Value;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}


