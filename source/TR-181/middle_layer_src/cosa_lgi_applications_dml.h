/*********************************************************************************
 * Copyright 2019 Liberty Global B.V.
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
 *********************************************************************************/

#ifndef  _COSA_LGI_APPLICATIONS_DML_H
#define  _COSA_LGI_APPLICATIONS_DML_H

#include "cosa_lgi_applications_apis.h"

/***********************************************************************

 APIs for Object:

   X_LGI_COM_Applications.Speedtest.SamKnows.

    *  LgiApplications_SamKnows_GetParamBoolValue
    *  LgiApplications_SamKnows_SetParamBoolValue
    *  LgiApplications_SamKnows_GetParamStringValue
    *  LgiApplications_SamKnows_SetParamStringValue
    *  LgiApplications_SamKnows_Validate
    *  LgiApplications_SamKnows_Commit
    *  LgiApplications_SamKnows_Rollback

***********************************************************************/

BOOL LgiApplications_SamKnows_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool );
BOOL LgiApplications_SamKnows_SetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue );
ULONG LgiApplications_SamKnows_GetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pulSize );
BOOL LgiApplications_SamKnows_SetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* strValue );
BOOL LgiApplications_SamKnows_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength );
ULONG LgiApplications_SamKnows_Commit ( ANSC_HANDLE hInsContext );
ULONG LgiApplications_SamKnows_Rollback ( ANSC_HANDLE hInsContext );

#endif
