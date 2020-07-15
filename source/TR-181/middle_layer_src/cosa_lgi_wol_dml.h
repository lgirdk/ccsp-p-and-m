/*********************************************************************
 * Copyright 2020 ARRIS Enterprises, LLC.
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

#ifndef  _COSA_LGI_WOL_DML_H
#define  _COSA_LGI_WOL_DML_H

#include "cosa_lgi_wol_apis.h"

/***********************************************************************

 APIs for Object:

    Device.X_LGI-COM_WoL.

    *  LgiWoL_GetParamStringValue
    *  LgiWoL_SetParamStringValue
    *  LgiWoL_GetParamUlongValue
    *  LgiWoL_SetParamUlongValue
    *  LgiWoL_Validate
    *  LgiWoL_Commit
    *  LgiWoL_Rollback


***********************************************************************/
ULONG
LgiWoL_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
LgiWoL_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );


BOOL
LgiWoL_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

BOOL
LgiWoL_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    );

BOOL
LgiWoL_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
  );


ULONG
LgiWoL_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
LgiWoL_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

#endif
