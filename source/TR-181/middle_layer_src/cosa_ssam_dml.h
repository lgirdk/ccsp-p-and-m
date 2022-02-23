/*****************************************************************************
 * Copyright 2021 Liberty Global B.V.
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
 ****************************************************************************/

#ifndef _COSA_SSAM_DML_H
#define _COSA_SSAM_DML_H

BOOL X_LGI_COM_DigitalSecurity_GetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG *puLong);
BOOL X_LGI_COM_DigitalSecurity_SetParamUlongValue(ANSC_HANDLE hInsContext, char *ParamName, ULONG uValue);
BOOL X_LGI_COM_DigitalSecurity_SetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pString);
ULONG X_LGI_COM_DigitalSecurity_GetParamStringValue(ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize);
BOOL X_LGI_COM_DigitalSecurity_GetParamBoolValue(ANSC_HANDLE hInsContext, char *ParamName, BOOL *pBool);
BOOL X_LGI_COM_DigitalSecurity_SetParamBoolValue(ANSC_HANDLE hInsContext, char *ParamName, BOOL bValue);
ULONG X_LGI_COM_DigitalSecurity_Commit(ANSC_HANDLE hInsContext);

#endif
