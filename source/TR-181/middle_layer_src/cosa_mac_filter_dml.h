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

#ifndef  _COSA_MAC_FILTER_DML_H
#define  _COSA_MAC_FILTER_DML_H

#include "cosa_mac_filter_apis.h"

BOOL
MacFltTimeMask_GetParamUlongValue (
    ANSC_HANDLE                     hInsContext,
    char*                           ParamName,
    ULONG*                          puLong
);

BOOL
MacFltTimeMask_SetParamUlongValue (
    ANSC_HANDLE                     hInsContext,
    char*                           ParamName,
    ULONG                           ulValue
);

/***********************************************************************

 APIs for Object:

    X_LGI-COM_Gateway.Firewall.MACFiltering.Device.{i}.

    * MACFilter_GetEntryCount
    * MACFilter_GetEntry
    * MACFilter_AddEntry
    * MACFilter_DelEntry
    * MACFilter_GetParamBoolValue
    * MACFilter_GetParamStringValue
    * MACFilter_SetParamBoolValue
    * MACFilter_SetParamStringValue
    * MACFilter_Validate
    * MACFilter_Commit
    * MACFilter_Rollback
***********************************************************************/
ULONG
MACFilter_GetEntryCount
    (
         ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
MACFilter_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

ANSC_HANDLE
MACFilter_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    );

ULONG
MACFilter_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    );

BOOL
MACFilter_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

ULONG
MACFilter_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
MACFilter_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        pBool
    );

BOOL
MACFilter_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
MACFilter_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
MACFilter_Commit
    (
        ANSC_HANDLE                 hInsContext
    );


ULONG
MACFilter_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

/***********************************************************************

 APIs for Object:

    Device.X_ARRIS_COM_Gateway.Firewall.MACFiltering.DayOfWeek.{i}.

    * MacFilter_DayOfWeek_GetEntryCount
    * MacFilter_DayOfWeek_GetEntry
    * MacFilter_DayOfWeek_GetParamStringValue
    * MacFilter_DayOfWeek_SetParamStringValue
    * MacFilter_DayOfWeek_Validate
    * MacFilter_DayOfWeek_Commit
    * MacFilter_DayOfWeek_Rollback

***********************************************************************/
ULONG
MacFilter_DayOfWeek_GetEntryCount
    (
         ANSC_HANDLE                 hInsContext
    );

ANSC_HANDLE
MacFilter_DayOfWeek_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

ULONG
MacFilter_DayOfWeek_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
MacFilter_DayOfWeek_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
MacFilter_DayOfWeek_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
MacFilter_DayOfWeek_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
MacFilter_DayOfWeek_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );
#endif
