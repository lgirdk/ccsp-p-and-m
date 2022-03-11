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

#ifndef  _COSA_LGI_MULTICAST_APIS_H
#define  _COSA_LGI_MULTICAST_APIS_H

#include "cosa_apis.h"

ULONG CosaDmlMulticastGetEnable ( ANSC_HANDLE hContext, BOOL *pValue );
ULONG CosaDmlMulticastSetEnable ( ANSC_HANDLE hContext, BOOL bValue );
ULONG CosaDmlMulticastGetSnoopingEnable ( ANSC_HANDLE hContext, BOOL *pValue );
ULONG CosaDmlMulticastSetSnoopingEnable ( ANSC_HANDLE hContext, BOOL bValue );
ULONG CosaDmlMulticastGetIGMPv3ProxyEnable ( ANSC_HANDLE hContext, BOOL *pValue );
ULONG CosaDmlMulticastSetIGMPv3ProxyEnable ( ANSC_HANDLE hContext, BOOL bValue );
ULONG CosaDmlMulticastGetMLDv2ProxyEnable ( ANSC_HANDLE hContext, BOOL *pValue );
ULONG CosaDmlMulticastSetMLDv2ProxyEnable ( ANSC_HANDLE hContext, BOOL bValue );
ULONG CosaDmlMulticastGetSSMForwardingEnable ( ANSC_HANDLE hContext, BOOL *pValue );
ULONG CosaDmlMulticastSetSSMForwardingEnable ( ANSC_HANDLE hContext, BOOL bValue );
ULONG CosaDmlMulticastGetMaxSSMSessions ( ANSC_HANDLE hContext, ULONG* puLong );
ULONG CosaDmlMulticastSetMaxSSMSessions ( ANSC_HANDLE hContext, ULONG uValue );
ULONG CosaDmlMulticastGetM2UMaxSessions ( ANSC_HANDLE hContext, ULONG* puLong );
ULONG CosaDmlMulticastSetM2UMaxSessions ( ANSC_HANDLE hContext, ULONG uValue );

#endif
