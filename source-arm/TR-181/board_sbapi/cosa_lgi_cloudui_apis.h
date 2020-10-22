/*********************************************************************
 * Copyright 2019 ARRIS Enterprises, LLC.
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

#ifndef  _COSA_LGI_CLOUDUI_APIS_H
#define  _COSA_LGI_CLOUDUI_APIS_H

#include "../middle_layer_src/cosa_apis.h"

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ANSC_STATUS CosaDmlGetDhcpLanChangeHide ( ANSC_HANDLE hContext, BOOL *pBool );
ULONG CosaDmlSetDhcpLanChangeHide ( ANSC_HANDLE hContext, BOOL bValue );

ANSC_STATUS CosaDmlGetSmartWifiSectionHide ( ANSC_HANDLE hContext, BOOL *pBool );
ULONG CosaDmlSetSmartWifiSectionHide ( ANSC_HANDLE hContext, BOOL bValue );

#endif
