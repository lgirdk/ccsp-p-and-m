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


#ifndef  _COSA_LGI_APPLICATIONS_APIS_H
#define  _COSA_LGI_APPLICATIONS_APIS_H

#include "cosa_apis.h"

#define SAMKNOWS_PROPERTY_STRING_LEN 64

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ULONG CosaDmlApplicationsSamKnowsGetEnabled ( ANSC_HANDLE hContext, BOOL *pValue );
ULONG CosaDmlApplicationsSamKnowsSetEnabled ( ANSC_HANDLE hContext, BOOL bValue );
ULONG CosaDmlApplicationsSamKnowsGetProperty ( ANSC_HANDLE hContext, char *pValue, ULONG *pUlSize );
ULONG CosaDmlApplicationsSamKnowsSetProperty ( ANSC_HANDLE hContext, char *pValue );
BOOL CosaDmlApplicationsSamKnowsValidateProperty ( ANSC_HANDLE hContext, char *pValue );

#endif
