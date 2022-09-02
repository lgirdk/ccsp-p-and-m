/*****************************************************************************
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
 ****************************************************************************/

#ifndef _COSA_LGI_TUNNELEDSTATICIP_DML_H
#define _COSA_LGI_TUNNELEDSTATICIP_DML_H

#include "cosa_lgi_tunneledstaticip_apis.h"

BOOL X_LGI_COM_TunneledStaticIPService_GetParamBoolValue (ANSC_HANDLE hInsContext, char *ParamName, BOOL* pBool);
BOOL X_LGI_COM_TunneledStaticIPService_SetParamBoolValue (ANSC_HANDLE hInsContext, char *ParamName, BOOL bValue);
BOOL X_LGI_COM_TunneledStaticIPService_GetParamIntValue (ANSC_HANDLE hInsContext, char *ParamName, int *pInt);
BOOL X_LGI_COM_TunneledStaticIPService_SetParamIntValue (ANSC_HANDLE hInsContext, char *ParamName, int value);
ULONG X_LGI_COM_TunneledStaticIPService_GetParamStringValue (ANSC_HANDLE hInsContext, char *ParamName, char *pValue, ULONG *pUlSize);
BOOL X_LGI_COM_TunneledStaticIPService_SetParamStringValue (ANSC_HANDLE hInsContext, char *ParamName, char *strValue);
BOOL X_LGI_COM_TunneledStaticIPService_GetParamUlongValue (ANSC_HANDLE hInsContext, char *ParamName, ULONG *puLong);
BOOL X_LGI_COM_TunneledStaticIPService_SetParamUlongValue (ANSC_HANDLE hInsContext, char *ParamName, ULONG uValuepUlong);
BOOL X_LGI_COM_TunneledStaticIPService_Validate (ANSC_HANDLE hInsContext, char *pReturnParamName, ULONG *puLength);
ULONG X_LGI_COM_TunneledStaticIPService_Commit (ANSC_HANDLE hInsContext);
ULONG X_LGI_COM_TunneledStaticIPService_Rollback (ANSC_HANDLE hInsContext);

#endif
