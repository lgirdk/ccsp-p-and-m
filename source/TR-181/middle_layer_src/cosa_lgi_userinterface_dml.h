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

#ifndef  _COSA_LGI_USERINTERFACE_DML_H
#define  _COSA_LGI_USERINTERFACE_DML_H

#include "cosa_userinterface_dml.h"

/***********************************************************************

 APIs for Object:

    UserInterface.RemoteAccess.

    *  Std_RemoteAccess_GetParamBoolValue
    *  Std_RemoteAccess_GetParamIntValue
    *  Std_RemoteAccess_GetParamUlongValue
    *  Std_RemoteAccess_GetParamStringValue
    *  Std_RemoteAccess_SetParamBoolValue
    *  Std_RemoteAccess_SetParamIntValue
    *  Std_RemoteAccess_SetParamUlongValue
    *  Std_RemoteAccess_SetParamStringValue
    *  Std_RemoteAccess_Validate
    *  Std_RemoteAccess_Commit
    *  Std_RemoteAccess_Rollback

***********************************************************************/
BOOL
Std_RemoteAccess_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
Std_RemoteAccess_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
Std_RemoteAccess_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
Std_RemoteAccess_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
Std_RemoteAccess_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
Std_RemoteAccess_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    );

BOOL
Std_RemoteAccess_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    );

BOOL
Std_RemoteAccess_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
Std_RemoteAccess_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
Std_RemoteAccess_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
Std_RemoteAccess_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

#endif
