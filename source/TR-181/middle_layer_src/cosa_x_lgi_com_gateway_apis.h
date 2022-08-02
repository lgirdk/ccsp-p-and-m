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
#include "cosa_x_lgi_com_gateway_internal.h"


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

ANSC_STATUS CosaDmlDNS_Rebind_SetConf( BOOL value );
ANSC_STATUS CosaDmlDNS_Rebind_GetConf( BOOL* value );
ULONG CosaDmlDNS_Whitelist_GetNumberOfEntries( void );
ANSC_STATUS CosaDmlDNS_Whitelist_GetEntryByIndex( ULONG index, COSA_DML_DNS_WHITELIST *entry );
ANSC_STATUS CosaDmlDNS_Whitelist_AddEntry( COSA_DML_DNS_WHITELIST *entry, BOOL dns_rebind_protection_enable );
ANSC_STATUS CosaDmlDNS_Whitelist_DelEntry( ULONG ins, BOOL dns_rebind_protection_enable );
ANSC_STATUS CosaDmlDNS_Whitelist_GetConf( ULONG ins, COSA_DML_DNS_WHITELIST *entry );
ANSC_STATUS CosaDmlDNS_Whitelist_SetConf( ULONG ins, COSA_DML_DNS_WHITELIST *entry, BOOL dns_rebind_protection_enable );

#endif
