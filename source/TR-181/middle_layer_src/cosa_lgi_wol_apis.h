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

#ifndef  _COSA_LGI_WOL_APIS_H
#define  _COSA_LGI_WOL_APIS_H

#include "../middle_layer_src/cosa_apis.h"

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ULONG CosaDmlSetMACAddress ( ANSC_HANDLE hContext, char *pValue );
ANSC_STATUS CosaDmlGetInterval ( ANSC_HANDLE hContext, ULONG *puLong );
ANSC_STATUS CosaDmlSetInterval ( ANSC_HANDLE hContext, ULONG uLong );
ANSC_STATUS CosaDmlGetRetries ( ANSC_HANDLE hContext, ULONG *puLong );
ANSC_STATUS CosaDmlSetRetries ( ANSC_HANDLE hContext, ULONG uLong );
ANSC_STATUS isMacValid (char *MacAddress);
void initMac (void);

#endif
