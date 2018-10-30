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

    module: cosa_nat_apis_custom.c

    For Data Model Implementation,
    Common Component Software Platform (CCSP)

    ---------------------------------------------------------------

    description:

        Custom data model APIs are implemented in this file

    ---------------------------------------------------------------

    environment:

        Maybe platform independent, but customer specific

    ---------------------------------------------------------------

    author:

        Ding Hua

    ---------------------------------------------------------------

    revision:

        05/08/2014  initial revision.

**************************************************************************/

#include "ansc_platform.h"
#include "dml_tr181_custom_cfg.h" 
#include "cosa_nat_dml.h"
#include "cosa_nat_dml_custom.h"

#include "plugin_main_apis.h"
#include "cosa_nat_apis.h"        
#include "cosa_nat_internal.h"


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        NAT_GetParamBoolValue_Custom
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
NAT_GetParamBoolValue_Custom
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_NAT             pMyObject    = (PCOSA_DATAMODEL_NAT)g_pCosaBEManager->hNat;
    PCOSA_DML_NAT                   pNat         = &pMyObject->Nat;

    CosaDmlNatGet(NULL, pNat);

    if (AnscEqualString(ParamName, "X_Comcast_com_EnablePortMapping", TRUE))
    {
        *pBool = COSA_DML_NAT_CUSTOM_GET_ENABLEPORTMAPPING(pNat);
        return TRUE;
    }
    if (AnscEqualString(ParamName, "X_Comcast_com_EnableHSPortMapping", TRUE))
    {
        *pBool = COSA_DML_NAT_CUSTOM_GET_ENABLEHSPORTMAPPING(pNat);
        return TRUE;
    }
    if( AnscEqualString(ParamName, "X_Comcast_com_EnableNATMapping", TRUE))
    {
        *pBool = COSA_DML_NAT_CUSTOM_GET_ENABLENATMAPPING(pNat);
        return TRUE;
    }

    return FALSE;
}


BOOL
NAT_SetParamBoolValue_Custom
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_NAT             pMyObject    = (PCOSA_DATAMODEL_NAT)g_pCosaBEManager->hNat;
    PCOSA_DML_NAT                   pNat         = &pMyObject->Nat;
    BOOL                                      bridgeMode;

    if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&bridgeMode)) &&
       (TRUE == bridgeMode))
        return FALSE;

    if( AnscEqualString(ParamName, "X_Comcast_com_EnablePortMapping", TRUE))
    {
        COSA_DML_NAT_CUSTOM_SET_ENABLEPORTMAPPING(pNat, bValue);
        return TRUE;
    }
    if( AnscEqualString(ParamName, "X_Comcast_com_EnableHSPortMapping", TRUE))
    {
        COSA_DML_NAT_CUSTOM_SET_ENABLEHSPORTMAPPING(pNat, bValue);
        return TRUE;
    }
    if( AnscEqualString(ParamName, "X_Comcast_com_EnableNATMapping", TRUE))
    {
        COSA_DML_NAT_CUSTOM_SET_ENABLENATMAPPING(pNat, bValue);
        return TRUE;
    }

    return FALSE;    
}

BOOL
PortMapping_GetParamUlongValue_Custom
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_PMAPPING_LINK_OBJECT        pCxtLink      = (PCOSA_CONTEXT_PMAPPING_LINK_OBJECT)hInsContext;
    PCOSA_DML_NAT_PMAPPING                    pNatPMapping  = (PCOSA_DML_NAT_PMAPPING)pCxtLink->hContext;

    if( AnscEqualString(ParamName, "X_Comcast_com_PublicIP", TRUE))
    {
        /* collect value */
        *puLong = pNatPMapping->PublicIP.Value;

        return TRUE;
    }
    else
    {
        return  FALSE;
    }
}


BOOL
PortMapping_SetParamUlongValue_Custom
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_PMAPPING_LINK_OBJECT        pCxtLink      = (PCOSA_CONTEXT_PMAPPING_LINK_OBJECT)hInsContext;
    PCOSA_DML_NAT_PMAPPING                    pNatPMapping  = (PCOSA_DML_NAT_PMAPPING)pCxtLink->hContext;

    if( AnscEqualString(ParamName, "X_Comcast_com_PublicIP", TRUE))
    {
        /* save update to backup */
        pNatPMapping->PublicIP.Value = uValue;

        return  TRUE;
    }
    else
    {
        return  FALSE;
    }
}
