/*********************************************************************************
 * Copyright 2022 Liberty Global B.V.
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

#ifndef  _COSA_LGI_IPERF_DML_H
#define  _COSA_LGI_IPERF_DML_H

#include "cosa_lgi_iperf_apis.h"

BOOL LgiIperf_GetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG *pUlong);
ULONG LgiIperf_GetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize);

BOOL LgiIperf_SetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG uValuepUlong);
BOOL LgiIperf_SetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *strValue);
BOOL LgiIperf_Validate(ANSC_HANDLE hInsContext, char *pReturnParamName, ULONG *puLength);

ULONG LgiIperf_Commit(ANSC_HANDLE hInsContext);
ULONG LgiIperf_Rollback(ANSC_HANDLE hInsContext);

#endif