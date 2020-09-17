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

#ifndef  _COSA_LGI_GENERAL_DML_H
#define  _COSA_LGI_GENERAL_DML_H

#include "cosa_lgi_general_apis.h"


/***********************************************************************

 APIs for Object:

    GeneralInfo.

    *  LgiGeneral_GetParamBoolValue
    *  LgiGeneral_GetParamUlongValue
    *  LgiGeneral_GetParamStringValue
    *  LgiGeneral_SetParamBoolValue
    *  LgiGeneral_SetParamUlongValue
    *  LgiGeneral_SetParamStringValue
    *  LgiGeneral_Validate
    *  LgiGeneral_Commit

***********************************************************************/
BOOL
LgiGeneral_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
LgiGeneral_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
LgiGeneral_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
LgiGeneral_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
LgiGeneral_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    );

BOOL
LgiGeneral_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
LgiGeneral_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
  );


ULONG
LgiGeneral_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
LgiGeneral_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

#endif
