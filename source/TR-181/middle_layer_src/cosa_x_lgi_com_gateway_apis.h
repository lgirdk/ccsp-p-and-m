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

#ifndef _COSA_LGI_GATEWAY_APIS_H
#define _COSA_LGI_GATEWAY_APIS_H

#include "../middle_layer_src/cosa_apis.h"

#define IF_NAMESIZE	16
#define INET6_ADDRLEN   16

#define RTF_DEFAULT	0x00010000	/* default - learned via ND	*/
#define RTF_ALLONLINK	0x00020000	/* (deprecated and will be removed)
					   fallback, no routers on link */
#define RTF_ADDRCONF	0x00040000	/* addrconf route - RA		*/
#define RTF_PREFIX_RT	0x00080000	/* A prefix only route - RA	*/
#define RTF_ANYCAST	0x00100000	/* Anycast			*/
#define RTF_NONEXTHOP	0x00200000	/* route with no nexthop	*/
#define RTF_EXPIRES	0x00400000
#define RTF_ROUTEINFO	0x00800000	/* route information - RA	*/
#define RTF_CACHE	0x01000000	/* cache entry			*/
#define RTF_FLOW	0x02000000	/* flow significant route	*/
#define RTF_POLICY	0x04000000	/* policy route			*/

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ANSC_STATUS CosaDmlLgiGwGetIpv6LanMode ( ANSC_HANDLE hContext, ULONG *pValue );
ULONG CosaDmlLgiGwSetIpv6LanMode ( ANSC_HANDLE hContext, ULONG ulValue );

int CosaDmlLgiGwGetDnsOverride ( BOOL *pValue );
int CosaDmlLgiGwSetDnsOverride ( BOOL bValue );

int CosaDmlLgiGwGetDnsIpv4Preferred ( char *pValue, ULONG *pUlSize );
int CosaDmlLgiGwGetDnsIpv4Alternate ( char *pValue, ULONG *pUlSize );
int CosaDmlLgiGwGetDnsIpv6Preferred ( char *pValue, ULONG *pUlSize );
int CosaDmlLgiGwGetDnsIpv6Alternate ( char *pValue, ULONG *pUlSize );

ANSC_STATUS CosaDml_Gateway_GetErouterInitMode(ULONG *pInitMode);
ANSC_STATUS CosaDml_Gateway_SetErouterInitMode(ULONG initMode);
ANSC_STATUS CosaDml_Gateway_GetIPv6LeaseTimeRemaining(ULONG *pValue);
ANSC_STATUS CosaDml_Gateway_GetIPv6Router ( ANSC_HANDLE hContext, char* pValue, ULONG* pulSize );

#endif
