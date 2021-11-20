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

    module: cosa_firewall_dml.c

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

#include <arpa/inet.h>
#include "ansc_platform.h"
#include "cosa_firewall_dml.h"
#include "cosa_firewall_internal.h"
#include "safec_lib_common.h"

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
// LGI ADD START
#define BTMASK_ALWAYS               0
#define DAYOFWEEK_BT_MASK_LEN       25
// LGI ADD END
/***********************************************************************

 APIs for Object:

    Firewall.

    *  Firewall_GetParamBoolValue
    *  Firewall_GetParamIntValue
    *  Firewall_GetParamUlongValue
    *  Firewall_GetParamStringValue
    *  Firewall_SetParamBoolValue
    *  Firewall_SetParamIntValue
    *  Firewall_SetParamUlongValue
    *  Firewall_SetParamStringValue
    *  Firewall_Validate
    *  Firewall_Commit
    *  Firewall_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall_GetParamBoolValue
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
Firewall_GetParamBoolValue
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
        Firewall_GetParamIntValue
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
Firewall_GetParamIntValue
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
        Firewall_GetParamUlongValue
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
Firewall_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;
    PCOSA_DML_FIREWALL_CFG          pFirewallCfg    = &pCosaDMFirewall->FirewallConfig;

   UNREFERENCED_PARAMETER(hInsContext);

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Config", TRUE))
    {
        /* collect value */
        *puLong = pFirewallCfg->FirewallLevel;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Firewall_GetParamStringValue
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
Firewall_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;
    PCOSA_DML_FIREWALL_CFG          pFirewallCfg    = &pCosaDMFirewall->FirewallConfig;
    errno_t                         rc              = -1;

    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pUlSize);

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Version", TRUE))
    {
        /* collect value */
        rc = strcpy_s(pValue, *pUlSize, pFirewallCfg->Version);
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return -1;
        }

        return 0;
    }

    if( AnscEqualString(ParamName, "LastChange", TRUE))
    {
        pFirewallCfg = AnscAllocateMemory(sizeof(COSA_DML_FIREWALL_CFG));

        if ( !pFirewallCfg )
        {
            CcspTraceWarning(("Firewall_GetParamUlongValue -- Resource allocation error\n"));

            return -1;
        }

        CosaDmlFirewallGetConfig(NULL, pFirewallCfg);
        /* collect value */
        rc = strcpy_s(pValue,*pUlSize, pFirewallCfg->LastChange);
        if(rc != EOK)
        {
           ERR_CHK(rc);
           AnscFreeMemory(pFirewallCfg);
           return -1;
        }
        AnscFreeMemory(pFirewallCfg);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall_SetParamBoolValue
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
Firewall_SetParamBoolValue
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
        Firewall_SetParamIntValue
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
Firewall_SetParamIntValue
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
        Firewall_SetParamUlongValue
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
Firewall_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Config", TRUE))
    {
        /* save update to backup */
        pCosaDMFirewall->FirewallConfig.FirewallLevel = uValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall_SetParamStringValue
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
Firewall_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pString);
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall_Validate
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
Firewall_Validate
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
        Firewall_Commit
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
Firewall_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;

    CosaDmlFirewallSetConfig(NULL, &pCosaDMFirewall->FirewallConfig);

    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Firewall_Rollback
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
Firewall_Rollback
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

        BOOL
        V4_GetParamBoolValue
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
V4_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (AnscEqualString(ParamName, "BlockFragIPPkts", TRUE)) {

        CosaDmlGatewayV4GetBlockFragIPPkts(pBool);
        return TRUE;
    }
    else if (AnscEqualString(ParamName, "PortScanProtect", TRUE)) {

      CosaDmlGatewayV4GetPortScanProtect(pBool);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "IPFloodDetect", TRUE)) {

      CosaDmlGatewayV4GetIPFloodDetect(pBool);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "ICMPFloodDetect", TRUE)) {

      CosaDmlGatewayV4GetICMPFloodDetect(pBool);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "Enable", TRUE)) {

      CosaDmlGatewayV4GetFwEnable(pBool);
      return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        V4_SetParamBoolValue
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
V4_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (AnscEqualString(ParamName, "BlockFragIPPkts", TRUE)) {

        CosaDmlGatewayV4SetBlockFragIPPkts(bValue);
        return TRUE;
    }
    else if (AnscEqualString(ParamName, "PortScanProtect", TRUE)) {

      CosaDmlGatewayV4SetPortScanProtect(bValue);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "IPFloodDetect", TRUE)) {

      CosaDmlGatewayV4SetIPFloodDetect(bValue);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "ICMPFloodDetect", TRUE)) {

      CosaDmlGatewayV4SetICMPFloodDetect(bValue);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "Enable", TRUE)) {

      CosaDmlGatewayV4SetFwEnable(bValue);
      return TRUE;
    }

    return FALSE;
}

BOOL V4_GetParamUlongValue ( ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong )
{
    if (AnscEqualString(ParamName, "ScheduleEnable", TRUE))
    {
        if(ANSC_STATUS_SUCCESS == CosaDmlFW_V4DayOfWeek_GetBlockTimeBitMaskType(puLong))
	{
            return TRUE;
        }
    }

    if (AnscEqualString(ParamName, "ICMPFloodDetectRate", TRUE))
    {
        if(ANSC_STATUS_SUCCESS == CosaDmlGatewayV4GetICMPFloodDetectRate(puLong))
        {
            return TRUE;
        }
    }

    return FALSE;
}

BOOL V4_SetParamUlongValue ( ANSC_HANDLE hInsContext, char* ParamName, ULONG ulValue )
{
    PCOSA_DATAMODEL_FIREWALL pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;

    if (AnscEqualString(ParamName, "ScheduleEnable", TRUE))
    {
        if(ANSC_STATUS_SUCCESS != CosaDmlFW_V4DayOfWeek_SetBlockTimeBitMaskType(ulValue))
	{
            return FALSE;
        }

        pCosaDMFirewall->V4DayOfWeekBlockTimeBitMaskType = ulValue;
    }

    if (AnscEqualString(ParamName, "ICMPFloodDetectRate", TRUE))
    {
        if(ANSC_STATUS_SUCCESS != CosaDmlGatewayV4SetICMPFloodDetectRate(ulValue))
        {
            return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        V4_Validate
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
V4_Validate
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
        V4_Commit
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
V4_Commit
   (
       ANSC_HANDLE                  hInsContext
   )
{
  UNREFERENCED_PARAMETER(hInsContext);
  system("sysevent set firewall-restart");

  return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        V4_Rollback
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
V4_Rollback
   (
       ANSC_HANDLE                  hInsContext
   )
{
  UNREFERENCED_PARAMETER(hInsContext);
  return ANSC_STATUS_SUCCESS;
}

// V4 IP Filter----------------------------------------------------------------

ULONG FW_V4_IpFilter_GetEntryCount ( ANSC_HANDLE hInsContext )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    return AnscSListQueryDepth(&pCosaDMFirewall->FwV4IpFilterList);
}

ANSC_HANDLE FW_V4_IpFilter_GetEntry ( ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry = NULL;

    pSLinkEntry = AnscQueueGetEntryByIndex((ANSC_HANDLE)&pCosaDMFirewall->FwV4IpFilterList, nIndex);

    if (pSLinkEntry)
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pLinkObj->InstanceNumber;
    }

    return pLinkObj;
}

ANSC_HANDLE FW_V4_IpFilter_AddEntry ( ANSC_HANDLE hInsContext, ULONG* pInsNumber )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT     pLinkObj      = NULL;
    COSA_DML_FW_IPFILTER          *pFwIpFilter    = NULL;

    pLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
    if (!pLinkObj)
        return NULL;

    pFwIpFilter = AnscAllocateMemory(sizeof(COSA_DML_FW_IPFILTER));
    if (!pFwIpFilter)
    {
        AnscFreeMemory(pLinkObj);
        return NULL;
    }

     /* now we have this link content */
    pLinkObj->InstanceNumber = pCosaDMFirewall->FwV4IpFilterNextInsNum;
    pFwIpFilter->InstanceNumber = pCosaDMFirewall->FwV4IpFilterNextInsNum;
    pCosaDMFirewall->FwV4IpFilterNextInsNum++;
    if (pCosaDMFirewall->FwV4IpFilterNextInsNum == 0)
        pCosaDMFirewall->FwV4IpFilterNextInsNum = 1;

    _ansc_sprintf(pFwIpFilter->Alias, "FW-V4IpFilter-%d", (int)pLinkObj->InstanceNumber);
    pLinkObj->hContext      = (ANSC_HANDLE)pFwIpFilter;
    pLinkObj->hParentTable  = NULL;
    pLinkObj->bNew          = TRUE;

    CosaSListPushEntryByInsNum((PSLIST_HEADER)&pCosaDMFirewall->FwV4IpFilterList, pLinkObj);
    CosaFwReg_V4_IpFilterAddInfo((ANSC_HANDLE)pCosaDMFirewall, (ANSC_HANDLE)pLinkObj);

    *pInsNumber = pLinkObj->InstanceNumber;

    return pLinkObj;
}

ULONG FW_V4_IpFilter_DelEntry ( ANSC_HANDLE hInsContext, ANSC_HANDLE hInstance )
{
    ANSC_STATUS                    returnStatus = ANSC_STATUS_SUCCESS;
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT      pLinkObj     = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    COSA_DML_FW_IPFILTER          *pFwIpFilter  = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

   if (pLinkObj->bNew)
    {
        /* Set bNew to FALSE to indicate this node is not going to save to SysRegistry */
        pLinkObj->bNew = FALSE;
        returnStatus = CosaFwReg_V4_IpFilterDelInfo((ANSC_HANDLE)pCosaDMFirewall, (ANSC_HANDLE)pLinkObj);
    }
    else
    {
        returnStatus = CosaDmlFW_V4_IPFilter_DelEntry(pLinkObj->InstanceNumber);
    }

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscSListPopEntryByLink((PSLIST_HEADER)&pCosaDMFirewall->FwV4IpFilterList, &pLinkObj->Linkage);
        AnscFreeMemory(pFwIpFilter);
        AnscFreeMemory(pLinkObj);
    }
    return returnStatus;

}

BOOL FW_V4_IpFilter_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj            = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER            *pFwIpFilter        = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        *pBool = pFwIpFilter->Enable;
        return TRUE;
    }
    return FALSE;
}

ULONG FW_V4_IpFilter_GetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pUlSize )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj            = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER            *pFwIpFilter        = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "Description", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->Description);
        return 0;
    }
    if (AnscEqualString(ParamName, "SrcStartAddr", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->SrcStartIPAddress);
        return 0;
    }
    if (AnscEqualString(ParamName, "SrcEndAddr", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->SrcEndIPAddress);
        return 0;
    }
    if (AnscEqualString(ParamName, "DstStartAddr", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->DstStartIPAddress);
        return 0;
    }
    if (AnscEqualString(ParamName, "DstEndAddr", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->DstEndIPAddress);
        return 0;
    }

    return 1;
}

BOOL FW_V4_IpFilter_GetParamUlongValue ( ANSC_HANDLE hInsContext, char *ParamName, ULONG *pUlong )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter   = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "SrcPortStart", TRUE))
    {
        *pUlong = pFwIpFilter->SrcStartPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "SrcPortEnd", TRUE))
    {
        *pUlong = pFwIpFilter->SrcEndPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortStart", TRUE))
    {
        *pUlong = pFwIpFilter->DstStartPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortEnd", TRUE))
    {
        *pUlong = pFwIpFilter->DstEndPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Protocol", TRUE))
    {
        *pUlong = pFwIpFilter->ProtocolType;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Action", TRUE))
    {
        *pUlong = pFwIpFilter->FilterAction;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Direction", TRUE))
    {
        *pUlong = pFwIpFilter->FilterDirec;
        return TRUE;
    }

    return FALSE;
}

BOOL FW_V4_IpFilter_SetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj            = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter     = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        pFwIpFilter->Enable = bValue;
        return TRUE;
    }
    return FALSE;
}

BOOL FW_V4_IpFilter_SetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* strValue )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj            = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter     = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;
    unsigned char buf[sizeof(struct in_addr)];

    if (AnscEqualString(ParamName, "Description", TRUE))
    {
        _ansc_snprintf(pFwIpFilter->Description, sizeof(pFwIpFilter->Description), "%s", strValue);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "SrcStartAddr", TRUE))
    {
        _ansc_snprintf(pFwIpFilter->SrcStartIPAddress, sizeof(pFwIpFilter->SrcStartIPAddress), "%s", strValue);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "SrcEndAddr", TRUE))
    {
        _ansc_snprintf(pFwIpFilter->SrcEndIPAddress, sizeof(pFwIpFilter->SrcEndIPAddress), "%s", strValue);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstStartAddr", TRUE))
    {
       if(inet_pton(AF_INET,strValue,buf))
       {
               _ansc_snprintf(pFwIpFilter->DstStartIPAddress, sizeof(pFwIpFilter->DstStartIPAddress), "%s", strValue);
               return TRUE;
       }
    }
    if (AnscEqualString(ParamName, "DstEndAddr", TRUE))
    {
        _ansc_snprintf(pFwIpFilter->DstEndIPAddress, sizeof(pFwIpFilter->DstEndIPAddress), "%s", strValue);
        return TRUE;
    }

    return FALSE;
}

BOOL FW_V4_IpFilter_SetParamUlongValue ( ANSC_HANDLE hInsContext, char *ParamName, ULONG ulValue )
{
    PCOSA_CONTEXT_LINK_OBJECT     pLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER         *pFwIpFilter   = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "SrcPortStart", TRUE))
    {
        pFwIpFilter->SrcStartPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "SrcPortEnd", TRUE))
    {
        pFwIpFilter->SrcEndPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortStart", TRUE))
    {
        pFwIpFilter->DstStartPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortEnd", TRUE))
    {
        pFwIpFilter->DstEndPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Protocol", TRUE))
    {
        pFwIpFilter->ProtocolType= ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Action", TRUE))
    {
        pFwIpFilter->FilterAction= ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Direction", TRUE))
    {
        pFwIpFilter->FilterDirec= ulValue;
        return TRUE;
    }

    return FALSE;
}

BOOL FW_V4_IpFilter_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength )
{
    return TRUE;
}

ULONG FW_V4_IpFilter_Commit ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter  = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;

    if (pLinkObj->bNew)
    {
        if (CosaDmlFW_V4_IPFilter_AddEntry(pFwIpFilter) != ANSC_STATUS_SUCCESS)
            return -1;
        CosaFwReg_V4_IpFilterDelInfo((ANSC_HANDLE)pCosaDMFirewall, (ANSC_HANDLE)pLinkObj);
        pLinkObj->bNew = FALSE;
    }
    else
    {
        if (CosaDmlFW_V4_IPFilter_SetConf(pFwIpFilter->InstanceNumber, pFwIpFilter) != ANSC_STATUS_SUCCESS)
        {
            CosaDmlFW_V4_IPFilter_GetConf(pFwIpFilter->InstanceNumber, pFwIpFilter);
            return -1;
        }
    }
    CosaDmlFW_V4_IPFilter_GetConf(pFwIpFilter->InstanceNumber, pFwIpFilter);
    return 0;
}

ULONG FW_V4_IpFilter_Rollback ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj            = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter         = (COSA_DML_FW_IPFILTER *)pLinkObj->hContext;

    if (CosaDmlFW_V4_IPFilter_GetConf(pFwIpFilter->InstanceNumber, pFwIpFilter) != ANSC_STATUS_SUCCESS)
        return -1;

    return 0;
}

ULONG V4_IPFilter_DayOfWeek_GetEntryCount ( ANSC_HANDLE hInsContext )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    return AnscSListQueryDepth(&pCosaDMFirewall->V4DayOfWeekList);
}

ANSC_HANDLE V4_IPFilter_DayOfWeek_GetEntry ( ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry = NULL;

    pSLinkEntry = AnscQueueGetEntryByIndex((ANSC_HANDLE)&pCosaDMFirewall->V4DayOfWeekList, nIndex);
    if (pSLinkEntry)
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pLinkObj->InstanceNumber;
    }
    return pLinkObj;
}

ULONG V4_IPFilter_DayOfWeek_GetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pUlSize )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V4_DAYOFWEEK       *pV4DayOfWeek    = (COSA_DML_FW_V4_DAYOFWEEK*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "BlockTimeBitMask", TRUE)) {
        AnscCopyString(pValue, pV4DayOfWeek->V4DayOfWeek_BlockTimeBitMask);
        return 0;
    }
    return 1;
}

BOOL V4_IPFilter_DayOfWeek_SetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* strValue )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj         = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V4_DAYOFWEEK       *pV4DayOfWeek    = (COSA_DML_FW_V4_DAYOFWEEK*)pLinkObj->hContext;
    int iMaskLen = 0;
    int i = 0;

    /* check the parameter name and set the corresponding value */
    if (AnscEqualString(ParamName, "BlockTimeBitMask", TRUE))
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
        _ansc_snprintf(pV4DayOfWeek->V4DayOfWeek_BlockTimeBitMask, sizeof(pV4DayOfWeek->V4DayOfWeek_BlockTimeBitMask), "%s", strValue);
        return TRUE;
    }
    return FALSE;
}

BOOL V4_IPFilter_DayOfWeek_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength )
{
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V4_DAYOFWEEK       *pV4DayOfWeek = (COSA_DML_FW_V4_DAYOFWEEK*)pLinkObj->hContext;

    int iMaskLen = 0;
    int i = 0;

    if(BTMASK_ALWAYS != pCosaDMFirewall->V4DayOfWeekBlockTimeBitMaskType)
    {
        // Ensure Mask Len is 24 characters
        iMaskLen = strlen(pV4DayOfWeek->V4DayOfWeek_BlockTimeBitMask);
        if(iMaskLen != (DAYOFWEEK_BT_MASK_LEN - 1))
        {
            return FALSE;
        }
        // Ensure Mask is a valid Binary String
        for(i = 0; i < iMaskLen; i++)
        {
            if((int)pV4DayOfWeek->V4DayOfWeek_BlockTimeBitMask[i] < 48 || (int)pV4DayOfWeek->V4DayOfWeek_BlockTimeBitMask[i] > 49)
            {
                return FALSE; // Not a Binary character
            }
        }
    }

    return TRUE;
}

ULONG V4_IPFilter_DayOfWeek_Commit ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V4_DAYOFWEEK       *pV4DayOfWeek = (COSA_DML_FW_V4_DAYOFWEEK*)pLinkObj->hContext;

    if (CosaDmlFW_V4DayOfWeek_SetConf(pV4DayOfWeek->InstanceNumber, pV4DayOfWeek) != ANSC_STATUS_SUCCESS)
    {
        CosaDmlFW_V4DayOfWeek_GetConf(pV4DayOfWeek->InstanceNumber, pV4DayOfWeek);
        return -1;
    }
    return 0;
}

ULONG V4_IPFilter_DayOfWeek_Rollback ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V4_DAYOFWEEK       *pV4DayOfWeek = (COSA_DML_FW_V4_DAYOFWEEK*)pLinkObj->hContext;

    if (CosaDmlFW_V4DayOfWeek_GetConf(pV4DayOfWeek->InstanceNumber, pV4DayOfWeek) != ANSC_STATUS_SUCCESS)
        return -1;

    return 0;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        V6_GetParamBoolValue
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
V6_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (AnscEqualString(ParamName, "BlockFragIPPkts", TRUE)) {

        CosaDmlGatewayV6GetBlockFragIPPkts(pBool);
        return TRUE;
    }
    else if (AnscEqualString(ParamName, "PortScanProtect", TRUE)) {

      CosaDmlGatewayV6GetPortScanProtect(pBool);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "IPFloodDetect", TRUE)) {

      CosaDmlGatewayV6GetIPFloodDetect(pBool);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "Enable", TRUE)) {

      CosaDmlGatewayV6GetFwEnable(pBool);
      return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        V6_SetParamBoolValue
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
V6_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (AnscEqualString(ParamName, "BlockFragIPPkts", TRUE)) {

        CosaDmlGatewayV6SetBlockFragIPPkts(bValue);
        return TRUE;
    }
    else if (AnscEqualString(ParamName, "PortScanProtect", TRUE)) {

      CosaDmlGatewayV6SetPortScanProtect(bValue);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "IPFloodDetect", TRUE)) {

      CosaDmlGatewayV6SetIPFloodDetect(bValue);
      return TRUE;
    }
    else if (AnscEqualString(ParamName, "Enable", TRUE)) {

      CosaDmlGatewayV6SetFwEnable(bValue);
      return TRUE;
    }

    return FALSE;
}

BOOL V6_GetParamUlongValue ( ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong )
{
    /* check the parameter name and return the corresponding value */
    if (AnscEqualString(ParamName, "ScheduleEnable", TRUE)) {
        if(ANSC_STATUS_SUCCESS == CosaDmlFW_V6DayOfWeek_GetBlockTimeBitMaskType(puLong)) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL V6_SetParamUlongValue ( ANSC_HANDLE hInsContext, char* ParamName, ULONG ulValue )
{
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;
    /* check the parameter name and set the corresponding value */
    if (AnscEqualString(ParamName, "ScheduleEnable", TRUE)) {
        if(ANSC_STATUS_SUCCESS != CosaDmlFW_V6DayOfWeek_SetBlockTimeBitMaskType(ulValue)) {
            return FALSE;
        }
        pCosaDMFirewall->V6DayOfWeekBlockTimeBitMaskType = ulValue;
    }
    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        V6_Validate
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
V6_Validate
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
        V6_Commit
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
V6_Commit
   (
       ANSC_HANDLE                  hInsContext
   )
{
    UNREFERENCED_PARAMETER(hInsContext);
    system("sysevent set firewall-restart");

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        V6_Rollback
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
V6_Rollback
   (
       ANSC_HANDLE                  hInsContext
   )
{
  UNREFERENCED_PARAMETER(hInsContext);
  system("sysevent set firewall-restart");

  return 0;
}

// V6 IP Filter----------------------------------------------------------------

ULONG FW_V6_IpFilter_GetEntryCount ( ANSC_HANDLE hInsContext )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    return AnscSListQueryDepth(&pCosaDMFirewall->FwV6IpFilterList);
}

ANSC_HANDLE FW_V6_IpFilter_GetEntry ( ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT     pLinkObj    = NULL;
    PSINGLE_LINK_ENTRY            pSLinkEntry = NULL;

    pSLinkEntry = AnscQueueGetEntryByIndex((ANSC_HANDLE)&pCosaDMFirewall->FwV6IpFilterList, nIndex);

    if (pSLinkEntry)
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pLinkObj->InstanceNumber;
    }

    return pLinkObj;
}

ANSC_HANDLE FW_V6_IpFilter_AddEntry ( ANSC_HANDLE hInsContext, ULONG* pInsNumber )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT      pLinkObj    = NULL;
    COSA_DML_FW_IPFILTER          *pFwIpFilter   = NULL;

    pLinkObj = AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));
    if (!pLinkObj)
        return NULL;

    pFwIpFilter = AnscAllocateMemory(sizeof(COSA_DML_FW_IPFILTER));
    if (!pFwIpFilter)
    {
        AnscFreeMemory(pLinkObj);
        return NULL;
    }

     /* now we have this link content */
    pLinkObj->InstanceNumber = pCosaDMFirewall->FwV6IpFilterNextInsNum;
    pFwIpFilter->InstanceNumber = pCosaDMFirewall->FwV6IpFilterNextInsNum;
    pFwIpFilter->Enable = TRUE;
    pCosaDMFirewall->FwV6IpFilterNextInsNum++;
    if (pCosaDMFirewall->FwV6IpFilterNextInsNum == 0)
        pCosaDMFirewall->FwV6IpFilterNextInsNum = 1;

    _ansc_sprintf(pFwIpFilter->Alias, "FW-V6IpFilter-%d", (int)pLinkObj->InstanceNumber);
    pLinkObj->hContext      = (ANSC_HANDLE)pFwIpFilter;
    pLinkObj->hParentTable  = NULL;
    pLinkObj->bNew          = TRUE;

    CosaSListPushEntryByInsNum((PSLIST_HEADER)&pCosaDMFirewall->FwV6IpFilterList, pLinkObj);
    CosaFwReg_V6_IpFilterAddInfo((ANSC_HANDLE)pCosaDMFirewall, (ANSC_HANDLE)pLinkObj);

    *pInsNumber = pLinkObj->InstanceNumber;

    return pLinkObj;
}

ULONG FW_V6_IpFilter_DelEntry ( ANSC_HANDLE hInsContext, ANSC_HANDLE hInstance )
{
    ANSC_STATUS                    returnStatus = ANSC_STATUS_SUCCESS;
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT      pLinkObj = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    COSA_DML_FW_IPFILTER          *pFwIpFilter  = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

   if (pLinkObj->bNew)
    {
        /* Set bNew to FALSE to indicate this node is not going to save to SysRegistry */
        pLinkObj->bNew = FALSE;
        returnStatus = CosaFwReg_V6_IpFilterDelInfo((ANSC_HANDLE)pCosaDMFirewall, (ANSC_HANDLE)pLinkObj);
    }
    else
    {
        returnStatus = CosaDmlFW_V6_IPFilter_DelEntry(pLinkObj->InstanceNumber);
    }

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscSListPopEntryByLink((PSLIST_HEADER)&pCosaDMFirewall->FwV6IpFilterList, &pLinkObj->Linkage);
        AnscFreeMemory(pFwIpFilter);
        AnscFreeMemory(pLinkObj);
    }
    return returnStatus;
}

BOOL FW_V6_IpFilter_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER            *pFwIpFilter  = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;
    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        *pBool = pFwIpFilter->Enable;
        return TRUE;
    }
    return TRUE;
}

ULONG FW_V6_IpFilter_GetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pUlSize )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER            *pFwIpFilter  = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "Description", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->Description);
        return 0;
    }
    if (AnscEqualString(ParamName, "SrcStartAddr", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->SrcStartIPAddress);
        return 0;
    }
    if (AnscEqualString(ParamName, "DstStartAddr", TRUE))
    {
        AnscCopyString(pValue, pFwIpFilter->DstStartIPAddress);
        return 0;
    }

    return 1;
}

BOOL FW_V6_IpFilter_GetParamUlongValue ( ANSC_HANDLE hInsContext, char *ParamName, ULONG *pUlong )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter   = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "SrcPortStart", TRUE))
    {
        *pUlong = pFwIpFilter->SrcStartPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "SrcPortEnd", TRUE))
    {
        *pUlong = pFwIpFilter->SrcEndPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortStart", TRUE))
    {
        *pUlong = pFwIpFilter->DstStartPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortEnd", TRUE))
    {
        *pUlong = pFwIpFilter->DstEndPort;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "IPv6SrcPrefixLen", TRUE))
    {
        *pUlong = pFwIpFilter->IPv6SrcPrefixLen;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "IPv6DstPrefixLen", TRUE))
    {
        *pUlong = pFwIpFilter->IPv6DstPrefixLen;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Protocol", TRUE))
    {
        *pUlong = pFwIpFilter->ProtocolType;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Action", TRUE))
    {
        *pUlong = pFwIpFilter->FilterAction;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Direction", TRUE))
    {
        *pUlong = pFwIpFilter->FilterDirec;
        return TRUE;
    }

    return FALSE;
}

BOOL FW_V6_IpFilter_SetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter     = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;
    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        pFwIpFilter->Enable = bValue;
        return TRUE;
    }
    return FALSE;
}

BOOL FW_V6_IpFilter_SetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* strValue )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter     = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;
    unsigned char buf[sizeof(struct in6_addr)];

    if (AnscEqualString(ParamName, "Description", TRUE))
    {
        _ansc_snprintf(pFwIpFilter->Description, sizeof(pFwIpFilter->Description), "%s", strValue);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "SrcStartAddr", TRUE))
    {
        _ansc_snprintf(pFwIpFilter->SrcStartIPAddress, sizeof(pFwIpFilter->SrcStartIPAddress), "%s", strValue);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstStartAddr", TRUE))
    {
       if(inet_pton(AF_INET6,strValue,buf))
       {
               _ansc_snprintf(pFwIpFilter->DstStartIPAddress, sizeof(pFwIpFilter->DstStartIPAddress), "%s", strValue);
               return TRUE;
       }

    }

    return FALSE;
}

BOOL FW_V6_IpFilter_SetParamUlongValue ( ANSC_HANDLE hInsContext, char *ParamName, ULONG ulValue )
{
    PCOSA_CONTEXT_LINK_OBJECT     pLinkObj    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER         *pFwIpFilter   = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "SrcPortStart", TRUE))
    {
        pFwIpFilter->SrcStartPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "SrcPortEnd", TRUE))
    {
        pFwIpFilter->SrcEndPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortStart", TRUE))
    {
        pFwIpFilter->DstStartPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DstPortEnd", TRUE))
    {
        pFwIpFilter->DstEndPort = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "IPv6SrcPrefixLen", TRUE))
    {
        if((ulValue == 0) || (ulValue >= 64 && ulValue <= 128))
        {
            pFwIpFilter->IPv6SrcPrefixLen = ulValue;
            return TRUE;
        }
    }
    if (AnscEqualString(ParamName, "IPv6DstPrefixLen", TRUE))
    {
        pFwIpFilter->IPv6DstPrefixLen = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Protocol", TRUE))
    {
        pFwIpFilter->ProtocolType= ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Action", TRUE))
    {
        pFwIpFilter->FilterAction = ulValue;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "Direction", TRUE))
    {
        pFwIpFilter->FilterDirec = ulValue;
        return TRUE;
    }

    return FALSE;
}

BOOL FW_V6_IpFilter_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength )
{
    return TRUE;
}

ULONG FW_V6_IpFilter_Commit ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT      pLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter  = (COSA_DML_FW_IPFILTER*)pLinkObj->hContext;
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;

    if (pLinkObj->bNew)
    {
        if (CosaDmlFW_V6_IPFilter_AddEntry(pFwIpFilter) != ANSC_STATUS_SUCCESS)
            return -1;
        CosaFwReg_V6_IpFilterDelInfo((ANSC_HANDLE)pCosaDMFirewall, (ANSC_HANDLE)pLinkObj);
        pLinkObj->bNew = FALSE;
    }
    else
    {
        if (CosaDmlFW_V6_IPFilter_SetConf(pFwIpFilter->InstanceNumber, pFwIpFilter) != ANSC_STATUS_SUCCESS)
        {
            CosaDmlFW_V6_IPFilter_GetConf(pFwIpFilter->InstanceNumber, pFwIpFilter);
            return -1;
        }
    }
    return 0;
}

ULONG FW_V6_IpFilter_Rollback ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj       = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_IPFILTER           *pFwIpFilter    = (COSA_DML_FW_IPFILTER *)pLinkObj->hContext;

    if (CosaDmlFW_V6_IPFilter_GetConf(pFwIpFilter->InstanceNumber, pFwIpFilter) != ANSC_STATUS_SUCCESS)
        return -1;

    return 0;
}

ULONG V6_IPFilter_DayOfWeek_GetEntryCount ( ANSC_HANDLE hInsContext )
{
    COSA_DATAMODEL_FIREWALL   *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    return AnscSListQueryDepth(&pCosaDMFirewall->V6DayOfWeekList);
}

ANSC_HANDLE V6_IPFilter_DayOfWeek_GetEntry ( ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber )
{
    COSA_DATAMODEL_FIREWALL    *pCosaDMFirewall = (COSA_DATAMODEL_FIREWALL*)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj    = NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry = NULL;

    pSLinkEntry = AnscQueueGetEntryByIndex((ANSC_HANDLE)&pCosaDMFirewall->V6DayOfWeekList, nIndex);
    if (pSLinkEntry)
    {
        pLinkObj = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        *pInsNumber = pLinkObj->InstanceNumber;
    }

    return pLinkObj;
}

ULONG V6_IPFilter_DayOfWeek_GetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pUlSize )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V6_DAYOFWEEK       *pV6DayOfWeek    = (COSA_DML_FW_V6_DAYOFWEEK*)pLinkObj->hContext;

    if (AnscEqualString(ParamName, "BlockTimeBitMask", TRUE)) {
        AnscCopyString(pValue, pV6DayOfWeek->V6DayOfWeek_BlockTimeBitMask);
        return 0;
    }
    return 1;
}

BOOL V6_IPFilter_DayOfWeek_SetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* strValue )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V6_DAYOFWEEK       *pV6DayOfWeek    = (COSA_DML_FW_V6_DAYOFWEEK*)pLinkObj->hContext;
    int iMaskLen = 0;
    int i = 0;

    /* check the parameter name and set the corresponding value */
    if (AnscEqualString(ParamName, "BlockTimeBitMask", TRUE))
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
        _ansc_snprintf(pV6DayOfWeek->V6DayOfWeek_BlockTimeBitMask, sizeof(pV6DayOfWeek->V6DayOfWeek_BlockTimeBitMask), "%s", strValue);
        return TRUE;
    }
    return FALSE;
}

BOOL V6_IPFilter_DayOfWeek_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength )
{
    PCOSA_DATAMODEL_FIREWALL        pCosaDMFirewall = (PCOSA_DATAMODEL_FIREWALL)g_pCosaBEManager->hFirewall;
    PCOSA_CONTEXT_LINK_OBJECT      pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V6_DAYOFWEEK       *pV6DayOfWeek   = (COSA_DML_FW_V6_DAYOFWEEK*)pLinkObj->hContext;

    int iMaskLen = 0;
    int i = 0;

    if(BTMASK_ALWAYS != pCosaDMFirewall->V6DayOfWeekBlockTimeBitMaskType)
    {
        // Ensure Mask Len is 24 characters
        iMaskLen = strlen(pV6DayOfWeek->V6DayOfWeek_BlockTimeBitMask);
        if(iMaskLen != (DAYOFWEEK_BT_MASK_LEN - 1))
    {
            return FALSE;
        }
        // Ensure Mask is a valid Binary String
        for(i = 0; i < iMaskLen; i++)
    {
            if((int)pV6DayOfWeek->V6DayOfWeek_BlockTimeBitMask[i] < 48 || (int)pV6DayOfWeek->V6DayOfWeek_BlockTimeBitMask[i] > 49)
            {
                return FALSE; // Not a Binary character
            }
        }
    }

    return TRUE;
}

ULONG V6_IPFilter_DayOfWeek_Commit ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V6_DAYOFWEEK       *pV6DayOfWeek = (COSA_DML_FW_V6_DAYOFWEEK*)pLinkObj->hContext;

    if (CosaDmlFW_V6DayOfWeek_SetConf(pV6DayOfWeek->InstanceNumber, pV6DayOfWeek) != ANSC_STATUS_SUCCESS)
    {
        CosaDmlFW_V6DayOfWeek_GetConf(pV6DayOfWeek->InstanceNumber, pV6DayOfWeek);
        return -1;
    }

    return 0;
}

ULONG V6_IPFilter_DayOfWeek_Rollback ( ANSC_HANDLE hInsContext )
{
    PCOSA_CONTEXT_LINK_OBJECT       pLinkObj        = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    COSA_DML_FW_V6_DAYOFWEEK       *pV6DayOfWeek    = (COSA_DML_FW_V6_DAYOFWEEK*)pLinkObj->hContext;

    if (CosaDmlFW_V6DayOfWeek_GetConf(pV6DayOfWeek->InstanceNumber, pV6DayOfWeek) != ANSC_STATUS_SUCCESS)
        return -1;
    return 0;
}

