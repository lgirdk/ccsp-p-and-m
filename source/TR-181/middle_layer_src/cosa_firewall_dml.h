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

    module: cosa_firewall_dml.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/14/2011    initial revision.

**************************************************************************/


#ifndef  _COSA_FIREWALL_DML_H
#define  _COSA_FIREWALL_DML_H

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
BOOL
Firewall_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
Firewall_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
Firewall_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
Firewall_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
Firewall_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
Firewall_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    );

BOOL
Firewall_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    );

BOOL
Firewall_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
Firewall_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
Firewall_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
Firewall_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

BOOL
V4_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
V4_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bBool
    );

// LGI ADD START
BOOL
V4_GetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char*       ParamName,
        ULONG*      puLong
    );

BOOL
V4_SetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char*       ParamName,
        ULONG       ulValue
    );
// LGI ADD END

BOOL
V4_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
V4_Commit
   (
       ANSC_HANDLE                  hInsContext
   );

ULONG
V4_Rollback
   (
       ANSC_HANDLE                  hInsContext
   );

// LGI ADD START
//V4 IP Filter----------------------------------------------------------------
ULONG
FW_V4_IpFilter_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
FW_V4_IpFilter_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

ANSC_HANDLE
FW_V4_IpFilter_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    );

ULONG
FW_V4_IpFilter_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    );

BOOL
FW_V4_IpFilter_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

ULONG
FW_V4_IpFilter_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
FW_V4_IpFilter_GetParamUlongValue(
        ANSC_HANDLE hInsContext,
        char *ParamName,
        ULONG *pUlong);

BOOL
FW_V4_IpFilter_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
FW_V4_IpFilter_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
FW_V4_IpFilter_SetParamUlongValue(
        ANSC_HANDLE hInsContext,
        char        *ParamName,
        ULONG       ulValue
        );

BOOL
FW_V4_IpFilter_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
FW_V4_IpFilter_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
FW_V4_IpFilter_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

/***********************************************************************

 APIs for Object:

    Device.Firewall.X_RDKCENTRAL-COM_Security.V4.IPFiltering.DayOfWeek.{i}.

    * V4_IPFilter_DayOfWeek_GetEntryCount
    * V4_IPFilter_DayOfWeek_GetEntry
    * V4_IPFilter_DayOfWeek_GetParamStringValue
    * V4_IPFilter_DayOfWeek_SetParamStringValue
    * V4_IPFilter_DayOfWeek_Validate
    * V4_IPFilter_DayOfWeek_Commit
    * V4_IPFilter_DayOfWeek_Rollback

***********************************************************************/
ULONG
V4_IPFilter_DayOfWeek_GetEntryCount
    (
         ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
V4_IPFilter_DayOfWeek_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

ULONG
V4_IPFilter_DayOfWeek_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
V4_IPFilter_DayOfWeek_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
V4_IPFilter_DayOfWeek_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
V4_IPFilter_DayOfWeek_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
V4_IPFilter_DayOfWeek_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );
// LGI ADD END

BOOL
V6_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
V6_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bBool
    );

// LGI ADD START
BOOL
V6_GetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char*       ParamName,
        ULONG*      puLong
    );

BOOL
V6_SetParamUlongValue
    (
        ANSC_HANDLE hInsContext,
        char*       ParamName,
        ULONG       ulValue
    );
// LGI ADD END

BOOL
V6_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
V6_Commit
   (
       ANSC_HANDLE                  hInsContext
   );

ULONG
V6_Rollback
   (
       ANSC_HANDLE                  hInsContext
   );

// LGI ADD START
//V6 IP Filter----------------------------------------------------------------
ULONG
FW_V6_IpFilter_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
FW_V6_IpFilter_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

ANSC_HANDLE
FW_V6_IpFilter_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    );

ULONG
FW_V6_IpFilter_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    );

BOOL
FW_V6_IpFilter_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

ULONG
FW_V6_IpFilter_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
FW_V6_IpFilter_GetParamUlongValue(
        ANSC_HANDLE hInsContext,
        char *ParamName,
        ULONG *pUlong);

BOOL
FW_V6_IpFilter_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
FW_V6_IpFilter_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
FW_V6_IpFilter_SetParamUlongValue(
        ANSC_HANDLE hInsContext,
        char        *ParamName,
        ULONG       ulValue
        );

BOOL
FW_V6_IpFilter_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
FW_V6_IpFilter_Commit
    (
        ANSC_HANDLE                 hInsContext
    );
ULONG
FW_V6_IpFilter_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

/***********************************************************************
 APIs for Object:

    Device.Firewall.X_RDKCENTRAL-COM_Security.V6.IPFiltering.DayOfWeek.{i}.

    * V6_IPFilter_DayOfWeek_GetEntryCount
    * V6_IPFilter_DayOfWeek_GetEntry
    * V6_IPFilter_DayOfWeek_GetParamStringValue
    * V6_IPFilter_DayOfWeek_SetParamStringValue
    * V6_IPFilter_DayOfWeek_Validate
    * V6_IPFilter_DayOfWeek_Commit
    * V6_IPFilter_DayOfWeek_Rollback
***********************************************************************/
ULONG
V6_IPFilter_DayOfWeek_GetEntryCount
    (
         ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
V6_IPFilter_DayOfWeek_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

ULONG
V6_IPFilter_DayOfWeek_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
V6_IPFilter_DayOfWeek_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
V6_IPFilter_DayOfWeek_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
V6_IPFilter_DayOfWeek_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
V6_IPFilter_DayOfWeek_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );
// LGI ADD END
#endif
