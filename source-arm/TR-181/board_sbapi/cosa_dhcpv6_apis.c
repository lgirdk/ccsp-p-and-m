/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
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
*/

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

/**************************************************************************

    module: cosa_dhcpv6_apis.c

        For COSA Data Model Library Development.

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.
        03/15/2011    Added for backend Dhcp4 clients.
        04/08/2011    Added Dhcp4 clients Sent Options and Req Options.
 

**************************************************************************/
#include <stdint.h>
#include "cosa_apis.h"
#include "cosa_dhcpv6_apis.h"
#include "cosa_dhcpv6_internal.h"
#include "plugin_main_apis.h"
#include "autoconf.h"


extern void* g_pDslhDmlAgent;
extern ANSC_HANDLE bus_handle;
extern char g_Subsystem[32];

#ifdef _HUB4_PRODUCT_REQ_
#include "wan_manager_ipc_msg.h"
#if defined SUCCESS
#undef SUCCESS
#endif
#define SYSEVENT_FIELD_IPV6_DNS_SERVER    "wan6_ns"
#define SYSEVENT_FIELD_IPV6_ULA_ADDRESS   "ula_address"

/**
 * @brief API to send dhcpv6 configuration data to RdkWanmanager over nanomsg socket
 * @param Pointer to dhcpv6_data_t holds the IPv6 configurations
 * @return 0 on success else returned -1
 */
static int send_dhcp_data_to_wanmanager (dhcpv6_data_t *dhcpv6_data); /* Send data to wanmanager using nanomsg. */
#endif

#if defined (INTEL_PUMA7)
//Intel Proposed RDKB Generic Bug Fix from XB6 SDK
#define NO_OF_RETRY 90  /*No of times the file will poll for TLV config file*/
#endif

#if defined _COSA_SIM_  || defined _COSA_DRG_TPG_

COSA_DML_DHCPCV6_FULL  g_dhcpv6_client[] =
    {
        {
            {
                1,
                "Client1",
                1,
                5,
                "Layer3Interface1",
                "RequestedOption1,RequestedOption2",
                TRUE,
                TRUE,
                TRUE,
                TRUE,
                FALSE
            },
            {
                COSA_DML_DHCP_STATUS_Enabled,
                "SupportedOptions1,SupportedOptions2",
                "duid1,duid2"
            }
        }
    };


COSA_DML_DHCPCV6_SENT    g_dhcpv6_client_sent[] = 
    {
        {
            1,
            "SentOption1",
            TRUE,
            11,
            "Value1"
        },
        {
            2,
            "SentOption2",
            TRUE,
            12,
            "Value2"
        }
    };

COSA_DML_DHCPCV6_SVR g_dhcpv6_client_server[] =
    {
        {
            "2002::1",
            "Interface1",
            "2002:02:03",
        },
        {
            "2002::2",
            "Interface2",
            "2002:02:04",
        }
    };

COSA_DML_DHCPCV6_RECV g_dhcpv6_client_recv[] =
    {
        {
            23,
            "Server1",
            "Value1"
        },
        {
            24,
            "Server2",
            "Value2"
        }
    };

BOOL  g_dhcpv6_server = TRUE;

COSA_DML_DHCPSV6_POOL_FULL  g_dhcpv6_server_pool[]=
{
    {
        {
            1,
            "Pool1",
            3,
            "Layer3Interface",
            "vendorid",                       /*VendorClassID*/
            "usrerid",
            "2002:3:4",
            "ffff:ffff:ffff:0",
            "IANAManualPrefixes",  /*IANAManualPrefixes*/
            "IAPDManualPrefixes",  /*IAPDManualPrefixes*/
            1999,
            "duid1",                        /*DUID*/
            TRUE,
            TRUE,                           /*DUIDExclude*/
            TRUE,
            TRUE,                           /*UserClassIDExclude*/
            TRUE,
            TRUE,  /*IANA enable*/
            TRUE
        },
        {
            COSA_DML_DHCP_STATUS_Enabled,
            "IANAPrefixes",  /*IANAPrefixes*/
            "IAPDPrefixes"   /*IAPDPrefixes*/
        }
    }
};

COSA_DML_DHCPSV6_CLIENT g_dhcpv6_server_client[] = 
{
    "client1",
    "2002::01",
    TRUE
};

COSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS g_dhcpv6_server_client_ipv6address[] = 
{
   {
        "2002::01",
        "1000",
        "1200"
    }
};

COSA_DML_DHCPSV6_CLIENT_IPV6PREFIX g_dhcpv6_server_client_ipv6prefix[] = 
{
    {
        "2002::03",
        "1100",
        "1300"
    }
};

COSA_DML_DHCPSV6_CLIENT_OPTION g_dhcpv6_server_client_option[] = 
{
  
    {
        23,
        "Value1"
    }
};

COSA_DML_DHCPSV6_POOL_OPTION g_dhcpv6_server_pool_option[] =
{
    {
        1,
        "Option1",
        10023,
        "PassthroughClient",
        "Value1",
        TRUE
    }
};


ANSC_STATUS
CosaDmlDhcpv6Init
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API retrieves the number of DHCP clients in the system.
 */
ULONG
CosaDmlDhcpv6cGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{
    PCOSA_DATAMODEL_DHCPV6          pDhcpv6           = (PCOSA_DATAMODEL_DHCPV6)g_pCosaBEManager->hDhcpv6;
    
    return 1;
}

/*
    Description:
        The API retrieves the complete info of the DHCP clients designated by index. The usual process is the caller gets the total number of entries, then iterate through those by calling this API.
    Arguments:
        ulIndex        Indicates the index number of the entry.
        pEntry        To receive the complete info of the entry.
*/
ANSC_STATUS
CosaDmlDhcpv6cGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPCV6_FULL      pEntry
    )
{  
    
   if ( ulIndex >= sizeof(g_dhcpv6_client)/sizeof(COSA_DML_DHCPCV6_FULL) )
        return ANSC_STATUS_FAILURE;
    
    AnscCopyMemory(pEntry, &g_dhcpv6_client[ulIndex], sizeof(COSA_DML_DHCPCV6_FULL));
        
        return ANSC_STATUS_SUCCESS;
    }

ANSC_STATUS
CosaDmlDhcpv6cSetValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    if ( ulIndex >= sizeof(g_dhcpv6_client)/sizeof(COSA_DML_DHCPCV6_FULL) )
        return ANSC_STATUS_SUCCESS;

    g_dhcpv6_client[ulIndex].Cfg.InstanceNumber  = ulInstanceNumber;
    AnscCopyString(g_dhcpv6_client[ulIndex].Cfg.Alias, pAlias);
    
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API adds DHCP client. 
    Arguments:
        pEntry        Caller fills in pEntry->Cfg, except Alias field. Upon return, callee fills pEntry->Cfg.Alias field and as many as possible fields in pEntry->Info.
*/
ANSC_STATUS
CosaDmlDhcpv6cAddEntry
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPCV6_FULL        pEntry
    )
{
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API removes the designated DHCP client entry. 
    Arguments:
        pAlias        The entry is identified through Alias.
*/
ANSC_STATUS
CosaDmlDhcpv6cDelEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
            return ANSC_STATUS_SUCCESS;
}

/*
Description:
    The API re-configures the designated DHCP client entry. 
Arguments:
    pAlias        The entry is identified through Alias.
    pEntry        The new configuration is passed through this argument, even Alias field can be changed.
*/
ANSC_STATUS
CosaDmlDhcpv6cSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPCV6_CFG       pCfg
    )
{
    ULONG                           index  = 0;

    for( index = 0 ; index < sizeof (g_dhcpv6_client)/sizeof(COSA_DML_DHCPCV6_FULL); index++)
    {
        if ( pCfg->InstanceNumber == g_dhcpv6_client[index].Cfg.InstanceNumber )
        {
            AnscCopyMemory(&g_dhcpv6_client[index].Cfg, pCfg, sizeof(COSA_DML_DHCPCV6_CFG));
            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPCV6_CFG       pCfg
    )
{
    ULONG                           index  = 0;
  
    for( index = 0 ; index < sizeof (g_dhcpv6_client)/sizeof(COSA_DML_DHCPCV6_FULL); index++)
    {
        if ( pCfg->InstanceNumber == g_dhcpv6_client[index].Cfg.InstanceNumber )
        {
            AnscCopyMemory(pCfg,  &g_dhcpv6_client[index].Cfg, sizeof(COSA_DML_DHCPCV6_CFG));
            return ANSC_STATUS_SUCCESS;
        }
    }
      
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlDhcpv6cGetInfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_DHCPCV6_INFO      pInfo
    )
{
    ULONG                           index  = 0;
   
    for( index = 0 ; index < sizeof (g_dhcpv6_client)/sizeof(COSA_DML_DHCPCV6_FULL); index++)
    {
        if ( ulInstanceNumber == g_dhcpv6_client[index].Cfg.InstanceNumber )
        {
            AnscCopyMemory(pInfo,  &g_dhcpv6_client[index].Info, sizeof(COSA_DML_DHCPCV6_INFO));
            return ANSC_STATUS_SUCCESS;
        }
    }
    
    return ANSC_STATUS_FAILURE;
}

/* this memory need to be freed by caller */
ANSC_STATUS
CosaDmlDhcpv6cGetServerCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SVR      *ppCfg,
        PULONG                      pSize
    )
{
    *ppCfg = (PCOSA_DML_DHCPCV6_SVR)AnscAllocateMemory( sizeof(g_dhcpv6_client_server ) );
    AnscCopyMemory( *ppCfg, &g_dhcpv6_client_server[0], sizeof(g_dhcpv6_client_server) );
    *pSize = sizeof(g_dhcpv6_client_server) / sizeof(COSA_DML_DHCPCV6_SVR);
        
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API initiates a DHCP client renewal. 
    Arguments:
        pAlias        The entry is identified through Alias.
*/
ANSC_STATUS
CosaDmlDhcpv6cRenew
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{  
    CcspTraceInfo((" CosaDmlDhcpv6cRenew -- ulInstanceNumber:%d.\n", ulInstanceNumber)); 

    return ANSC_STATUS_SUCCESS;
}

/*
 *  DHCP Client Send/Req Option
 *
 *  The options are managed on top of a DHCP client,
 *  which is identified through pClientAlias
 */
ULONG
CosaDmlDhcpv6cGetNumberOfSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber
    )
{
    return sizeof(g_dhcpv6_client_sent)/sizeof(COSA_DML_DHCPCV6_SENT);
}

ANSC_STATUS
CosaDmlDhcpv6cGetSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{
    PCOSA_DML_DHCPCV6_SENT          pDhcpSendOption      = pEntry;

    if ( ulIndex+1 > sizeof(g_dhcpv6_client_sent)/sizeof(COSA_DML_DHCPCV6_SENT) )
        return ANSC_STATUS_FAILURE;

    AnscCopyMemory( pEntry, &g_dhcpv6_client_sent[ulIndex], sizeof(COSA_DML_DHCPCV6_SENT));

    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlDhcpv6cGetSentOptionbyInsNum
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{   
    ULONG                           index  = 0;

    for( index=0;  index<sizeof(g_dhcpv6_client_sent)/sizeof(COSA_DML_DHCPCV6_SENT); index++)
    {
        if ( pEntry->InstanceNumber == g_dhcpv6_client_sent[index].InstanceNumber )
        {
            AnscCopyMemory( pEntry, &g_dhcpv6_client_sent[index], sizeof(COSA_DML_DHCPCV6_SENT));
             return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_FAILURE;
}


ANSC_STATUS
CosaDmlDhcpv6cSetSentOptionValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    if ( ulIndex+1 > sizeof(g_dhcpv6_client_sent)/sizeof(COSA_DML_DHCPCV6_SENT) )
        return ANSC_STATUS_SUCCESS;

    g_dhcpv6_client_sent[ulIndex].InstanceNumber  = ulInstanceNumber;
    AnscCopyString( g_dhcpv6_client_sent[ulIndex].Alias, pAlias);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cAddSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cDelSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        ULONG                       ulInstanceNumber
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cSetSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{
    ULONG                           index  = 0;

    for( index=0;  index<sizeof(g_dhcpv6_client_sent)/sizeof(COSA_DML_DHCPCV6_SENT); index++)
    {
        if ( pEntry->InstanceNumber == g_dhcpv6_client_sent[index].InstanceNumber )
        {
            AnscCopyMemory( &g_dhcpv6_client_sent[index], pEntry, sizeof(COSA_DML_DHCPCV6_SENT));
            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

/* This is to get all 
 */
ANSC_STATUS
CosaDmlDhcpv6cGetReceivedOptionCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_RECV     *ppEntry,
        PULONG                      pSize
    )
{   
    *ppEntry = (PCOSA_DML_DHCPCV6_RECV)AnscAllocateMemory( sizeof(g_dhcpv6_client_recv) );
    AnscCopyMemory( *ppEntry, &g_dhcpv6_client_recv[0], sizeof(g_dhcpv6_client_recv) );
    *pSize = sizeof(g_dhcpv6_client_recv) / sizeof(COSA_DML_DHCPCV6_RECV);
        
    return ANSC_STATUS_SUCCESS;
}

/*
 *  DHCP Server
 */
ANSC_STATUS
CosaDmlDhcpv6sEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bEnable
    )
{
    g_dhcpv6_server = bEnable;
    
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API retrieves the current state of DHCP server: Enabled or Disabled. 
*/
BOOLEAN
CosaDmlDhcpv6sGetState
    (
        ANSC_HANDLE                 hContext
    )
{
    return g_dhcpv6_server;
}

/*
 *  DHCP Server Pool
 */
ULONG
CosaDmlDhcpv6sGetNumberOfPools
    (
        ANSC_HANDLE                 hContext
    )
{
    return sizeof(g_dhcpv6_server_pool)/sizeof(COSA_DML_DHCPSV6_POOL_FULL);
}

ANSC_STATUS
CosaDmlDhcpv6sGetPool
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPSV6_POOL_FULL pEntry
    )
{
    if ( ulIndex+1 > sizeof(g_dhcpv6_server_pool)/sizeof(COSA_DML_DHCPSV6_POOL_FULL) )
        return ANSC_STATUS_FAILURE;

    AnscCopyMemory(pEntry, &g_dhcpv6_server_pool[ulIndex], sizeof(COSA_DML_DHCPSV6_POOL_FULL));
    
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sSetPoolValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    if ( ulIndex+1 > sizeof(g_dhcpv6_server_pool)/sizeof(COSA_DML_DHCPSV6_POOL_FULL) )
        return ANSC_STATUS_FAILURE;

    g_dhcpv6_server_pool[ulIndex].Cfg.InstanceNumber  = ulInstanceNumber;
    AnscCopyString(g_dhcpv6_server_pool[ulIndex].Cfg.Alias, pAlias);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sAddPool
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPSV6_POOL_FULL   pEntry
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sDelPool
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sSetPoolCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPSV6_POOL_CFG  pCfg
    )
{
    ULONG                          index = 0;

    for(index = 0; index <sizeof(g_dhcpv6_server_pool)/sizeof(COSA_DML_DHCPSV6_POOL_FULL); index++)
    {
        if ( pCfg->InstanceNumber == g_dhcpv6_server_pool[index].Cfg.InstanceNumber )
        {
            AnscCopyMemory( &g_dhcpv6_server_pool[index].Cfg, pCfg, sizeof(COSA_DML_DHCPSV6_POOL_FULL) );
            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sGetPoolCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPSV6_POOL_CFG    pCfg
    )
{
    ULONG                          index = 0;

    for(index = 0; index <sizeof(g_dhcpv6_server_pool)/sizeof(COSA_DML_DHCPSV6_POOL_FULL); index++)
    {
        if ( pCfg->InstanceNumber == g_dhcpv6_server_pool[index].Cfg.InstanceNumber )
        {
            AnscCopyMemory( pCfg, &g_dhcpv6_server_pool[index].Cfg, sizeof(COSA_DML_DHCPSV6_POOL_FULL) );
            return ANSC_STATUS_FAILURE;
        }
    }

    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlDhcpv6sGetPoolInfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_INFO pInfo
    )
{
    ULONG                          index = 0;

    for(index = 0; index <sizeof(g_dhcpv6_server_pool)/sizeof(COSA_DML_DHCPSV6_POOL_FULL); index++)
    {
        if ( ulInstanceNumber == g_dhcpv6_server_pool[index].Cfg.InstanceNumber )
        {
            AnscCopyMemory( pInfo, &g_dhcpv6_server_pool[index].Info, sizeof(COSA_DML_DHCPSV6_POOL_INFO) );
            return ANSC_STATUS_FAILURE;
        }
    }

    return ANSC_STATUS_FAILURE;
}


ANSC_STATUS
CosaDmlDhcpv6sPing
    (
        PCOSA_DML_DHCPSV6_CLIENT    pDhcpsClient
    )
{
    return ANSC_STATUS_SUCCESS;
}


/*
 *   This is for 
 *     Pool.{i}.Client.{i}.
 *
 */
ANSC_STATUS
CosaDmlDhcpv6sGetClient
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_CLIENT   *ppEntry,
        PULONG                      pSize
    )
{        
    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT)AnscAllocateMemory( sizeof(g_dhcpv6_server_client) );
    AnscCopyMemory( *ppEntry, &g_dhcpv6_server_client[0], sizeof(g_dhcpv6_server_client) );
    *pSize = sizeof(g_dhcpv6_server_client) / sizeof(COSA_DML_DHCPSV6_CLIENT);
        
    return ANSC_STATUS_SUCCESS;
}

/*
  *   This is for 
        Pool.{i}.Client.{i}.IPv6Address.{i}.
  *
  */
ANSC_STATUS
CosaDmlDhcpv6sGetIPv6Address
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulClientIndex,
        PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS   *ppEntry,
        PULONG                      pSize
    )
{        
    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS)AnscAllocateMemory( sizeof(g_dhcpv6_server_client_ipv6address) );
    AnscCopyMemory( *ppEntry, &g_dhcpv6_server_client_ipv6address[0], sizeof(g_dhcpv6_server_client_ipv6address) );
    *pSize = sizeof(g_dhcpv6_server_client_ipv6address) / sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS);
        
    return ANSC_STATUS_SUCCESS;
}

/*
  *   This is for 
        Pool.{i}.Client.{i}.IPv6Prefix.{i}.
  *
  */
ANSC_STATUS
CosaDmlDhcpv6sGetIPv6Prefix
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulClientIndex,
        PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX   *ppEntry,
        PULONG                      pSize
    )
{        
    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX)AnscAllocateMemory( sizeof(g_dhcpv6_server_client_ipv6prefix) );
    AnscCopyMemory( *ppEntry, &g_dhcpv6_server_client_ipv6prefix[0], sizeof(g_dhcpv6_server_client_ipv6prefix) );
    *pSize = sizeof(g_dhcpv6_server_client_ipv6prefix) / sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6PREFIX);
        
    return ANSC_STATUS_SUCCESS;
}

/*
  *   This is for 
        Pool.{i}.Client.{i}.IPv6Option.{i}.
  *
  */
ANSC_STATUS
CosaDmlDhcpv6sGetIPv6Option
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulClientIndex,
        PCOSA_DML_DHCPSV6_CLIENT_OPTION   *ppEntry,
        PULONG                      pSize
    )
{        
    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT_OPTION)AnscAllocateMemory( sizeof(g_dhcpv6_server_client_option) );
    AnscCopyMemory( *ppEntry, &g_dhcpv6_server_client_option[0], sizeof(g_dhcpv6_server_client_option) );
    *pSize = sizeof(g_dhcpv6_server_client_option) / sizeof(COSA_DML_DHCPSV6_CLIENT_OPTION);

    return ANSC_STATUS_SUCCESS;
}

/*
 *  DHCP Server Pool Option
 *
 *  The options are managed on top of a DHCP server pool,
 *  which is identified through pPoolAlias
 */
ULONG
CosaDmlDhcpv6sGetNumberOfOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber
    )
{
    return sizeof(g_dhcpv6_server_pool_option) / sizeof(COSA_DML_DHCPSV6_POOL_OPTION);
}

ANSC_STATUS
CosaDmlDhcpv6sGetOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    if ( ulIndex+1 > sizeof(g_dhcpv6_server_pool_option)/sizeof(COSA_DML_DHCPSV6_POOL_OPTION) )
        return ANSC_STATUS_FAILURE;

    AnscCopyMemory(pEntry, &g_dhcpv6_server_pool_option[ulIndex], sizeof(COSA_DML_DHCPSV6_POOL_OPTION));
    
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sGetOptionbyInsNum
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    ULONG                           index = 0;

    for(index = 0; index < sizeof(g_dhcpv6_server_pool_option[index])/sizeof(COSA_DML_DHCPSV6_POOL_OPTION); index++)
    {
        if ( pEntry->InstanceNumber == g_dhcpv6_server_pool_option[index].InstanceNumber )
        {
            AnscCopyMemory(pEntry, &g_dhcpv6_server_pool_option[index], sizeof(COSA_DML_DHCPSV6_POOL_OPTION));
            return ANSC_STATUS_SUCCESS;
        }
    }
    
    return ANSC_STATUS_FAILURE;
}


ANSC_STATUS
CosaDmlDhcpv6sSetOptionValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    if ( ulIndex+1 > sizeof(g_dhcpv6_server_pool_option)/sizeof(COSA_DML_DHCPSV6_POOL_OPTION) )
        return ANSC_STATUS_FAILURE;

    g_dhcpv6_server_pool_option[ulIndex].InstanceNumber  = ulInstanceNumber;
    AnscCopyString(g_dhcpv6_server_pool_option[ulIndex].Alias, pAlias);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sAddOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS
CosaDmlDhcpv6sDelOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulInstanceNumber
    )
{
    return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS
CosaDmlDhcpv6sSetOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    ULONG                          index = 0;

    for(index = 0; index <sizeof(g_dhcpv6_server_pool_option)/sizeof(COSA_DML_DHCPSV6_POOL_OPTION); index++)
    {
        if ( pEntry->InstanceNumber == g_dhcpv6_server_pool_option[index].InstanceNumber )
        {
            AnscCopyMemory( &g_dhcpv6_server_pool_option[index], pEntry, sizeof(COSA_DML_DHCPSV6_POOL_OPTION) );
            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_SUCCESS;    
}

void
CosaDmlDhcpv6Remove(ANSC_HANDLE hContext)
{
    return;
}

int  
CosaDmlStartDHCP6Client()
{
   return 0;
}

BOOL tagPermitted(int tag)
{
    return TRUE;
}

#elif (defined _COSA_INTEL_USG_ARM_) || (defined _COSA_BCM_MIPS_)

#include <net/if.h>
#include <sys/ioctl.h>
#include <ctype.h>
//#include <libgen.h>
#include <utapi.h>
#include <utapi_util.h>
#include "utctx/utctx_api.h"
#include "syscfg/syscfg.h"
#include "cosa_drg_common.h"
#include "cosa_ip_apis.h"
#include "cosa_common_util.h"

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_COSA_BCM_MIPS_)
#include <netinet/in.h>
#endif

#define SYSCFG_FORMAT_DHCP6C "tr_dhcpv6c"
#define CLIENT_DUID_FILE "/var/lib/dibbler/client-duid"
#define SERVER_DUID_FILE "/var/lib/dibbler/server-duid"

//for test
#define CLIENT_BIN     "dibbler-client"
#define SERVER_BIN     "dibbler-server"
//#define SERVER_BIN     "/usr/local/sbin//dibbler-server"

/* Server global variable  Begin*/

#if defined (MULTILAN_FEATURE)
#define IPV6_PREF_MAXLEN               128
#define DHCPV6S_POOL_NUM               64  /* this pool means interface. We just supported such number interfaces. Each interface can include many pools*/
#else
#define DHCPV6S_POOL_NUM               1
#endif
#define DHCPV6S_POOL_OPTION_NUM        16 /* each interface supports this number options */
#if defined (MULTILAN_FEATURE)
#define CMD_BUFF_SIZE                  256
#endif
#define DHCPV6S_NAME                   "dhcpv6s"

static struct {
    pthread_t          dbgthrd;
}g_be_ctx;

static void * dhcpv6c_dbg_thrd(void * in);
extern COSARepopulateTableProc            g_COSARepopulateTable;

#define UTOPIA_SIMULATOR                0

enum {
    DHCPV6_SERVER_TYPE_STATEFUL  =1,
    DHCPV6_SERVER_TYPE_STATELESS
};

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_COSA_BCM_MIPS_)
#define MAX_LAN_IF_NUM              3

/*erouter topology mode*/
enum tp_mod {
    TPMOD_UNKNOWN,
    FAVOR_DEPTH,
    FAVOR_WIDTH,
};

typedef struct pd_pool {
    char start[INET6_ADDRSTRLEN];
    char end[INET6_ADDRSTRLEN];
    int  prefix_length;
    int  pd_length;
} pd_pool_t;

typedef struct ia_info {
    union {
        char v6addr[INET6_ADDRSTRLEN];
        char v6pref[INET6_ADDRSTRLEN];
    } value;

    char t1[32], t2[32], iaid[32], pretm[32], vldtm[32];
    int len;
} ia_pd_t;

typedef struct ipv6_prefix {
    char value[INET6_ADDRSTRLEN];
    int  len;
    //int  b_used;
} ipv6_prefix_t;

#endif

#if 0

/*This is test part  Begin */


#define CONFIGFILENAMEYAN "/var/lib/dibbler/config_yan.txt"

typedef struct _UtopiaContext
{
    ULONG c;
}UtopiaContext;


struct _COUPLE_LINK
{
    char * name;
    char * value;
    struct _COUPLE_LINK * next;
};

typedef struct _COUPLE_LINK COUPLE_LINK, *PCOUPL_LINK;

static PCOUPL_LINK pHead = NULL;
static ULONG       lastTime  = 0;
static  USER_LOCK                   gPsmLock;

int safe_strcpy(char * dst, char * src, int dst_size)
{
    if (!dst || !src) return -1;

    memset(dst, 0, dst_size);
    _ansc_strncpy(dst, src, _ansc_strlen(src)<=dst_size-1 ? _ansc_strlen(src):dst_size-1 );

    return 0;
}

void _get_shell_output(char * cmd, char * out, int len)
{
    FILE * fp;
    char   buf[256];
    char * p;

    fp = popen(cmd, "r");

    if (fp)
    {
        fgets(buf, sizeof(buf), fp);
        
        /*we need to remove the \n char in buf*/
        if ((p = strchr(buf, '\n'))) *p = 0;

        strncpy(out, buf, len-1);

        pclose(fp);        
    }

}

static int Utopia_Init(UtopiaContext *ctx)
{
    return 1;
}

static int utopia_init()
{
    FILE    *fileHandle  = NULL;
    char    oneLine[256] = {0};
    char    *pOneLine    = NULL;
    char    *pOneLine2   = NULL;
    PCOUPL_LINK  pLink   = NULL;
    PCOUPL_LINK  pLink2  = NULL;
    static   BOOL    bInitiated = false;
    INT     count        = 0;

    if ( !bInitiated )
    {
        UserInitializeLock(&gPsmLock);
        bInitiated = true;
    }
    
    if ( pHead )
        return 0;

    UserAcquireLock(&gPsmLock);

    //open file 
    fileHandle = fopen(CONFIGFILENAMEYAN, "r+" );
    if ( !fileHandle )
    {
        UserReleaseLock(&gPsmLock);
        return 0;
    }


    //get file
    pLink2 = pHead;    
    while ( (pOneLine = fgets(oneLine, sizeof(oneLine), fileHandle )) )
    {
        pLink = AnscAllocateMemory( sizeof(COUPLE_LINK) );
        pLink->next = NULL;
        if ( !pHead )
        {
            pHead = pLink;
            pLink2 = pHead;
        }
        else
        {
            pLink2->next = pLink;
            pLink2 = pLink;
        }

        // skip non-space in one new line 
        while ( *pOneLine != ' ' ) pOneLine++;
        pOneLine++;
        pOneLine2 = pOneLine;

        //name 
        while ( *pOneLine != ' ' ) pOneLine++;
        *pOneLine = 0;

        pLink->name = AnscCloneString(pOneLine2);

        pOneLine++;
        
        pOneLine2 = pOneLine;

        // value 
        while ( *pOneLine != ' ' ) pOneLine++;
        *pOneLine = 0;
        
        pLink->value = AnscCloneString(pOneLine2);

    }

    pLink = pHead;
    while( pLink )
    {
        count++;
        printf("%3d %s %s  \n", count, pLink->name, pLink->value );
        pLink = pLink->next;
    }

    fclose(fileHandle);

    UserReleaseLock(&gPsmLock);

    return ANSC_STATUS_SUCCESS;
}


static int utopia_save()
{
    FILE    *fileHandle  = NULL;
    char    oneLine[256] = {0};
    PCOUPL_LINK  pLink   = NULL;

    if ( ( AnscGetTickInSeconds() - lastTime ) < 0 )
    {
        return 0;
    }
    else
    {
        lastTime = AnscGetTickInSeconds();
    }

    if ( !pHead )
    {
        UserReleaseLock(&gPsmLock);
        return 1;
    }
    
    UserAcquireLock(&gPsmLock);

    //open file 
    fileHandle = fopen(CONFIGFILENAMEYAN, "w+" );
    if ( !fileHandle )
        return 0;

    pLink = pHead;
    while ( pLink )
    {
        sprintf(oneLine, " %s %s \n\0", pLink->name, pLink->value);
        fputs( oneLine, fileHandle);
        pLink = pLink->next;
    }

    fclose(fileHandle);

    UserReleaseLock(&gPsmLock);

    return ANSC_STATUS_SUCCESS;
}

static int Utopia_RawSet
(
    UtopiaContext*              ctx,
    char*                       domainname,
    char*                       pRecordName,
    char*                       pValue
)
{
    char    fullname[512] = {0};
    char    *pOneLine    = NULL;
    char    *pOneLine2   = NULL;
    PCOUPL_LINK  pLink   = NULL;
    PCOUPL_LINK  pLink2  = NULL;
    
    if ( !pHead )
        utopia_init();

    UserAcquireLock(&gPsmLock);

    _ansc_sprintf(fullname, "%s%s", domainname, pRecordName);

    pLink = pHead;
    while( pLink )
    {
        if ( AnscEqualString( fullname, pLink->name , false ) )
        {
            break;
        }
        pLink2 = pLink;
        pLink = pLink->next;
    }

    if ( !pLink )
    {
        pLink = AnscAllocateMemory( sizeof(COUPLE_LINK) );
        pLink->next = NULL;

        pLink->name = AnscCloneString(fullname);
        
        if ( !pHead )
        {
            pHead = pLink;
        }
        else
        {
            pLink2->next = pLink;
        }
    }
    
    pLink->value = AnscCloneString(pValue);

    UserReleaseLock(&gPsmLock);

    utopia_save();
    
    return ANSC_STATUS_SUCCESS;
    
}


static int syscfg_unset
(
    char*                       domainname,
    char*                       pRecordName
)
{
    char    oneLine[256] = {0};
    char    fullname[512] = {0};
    char    *pOneLine    = NULL;
    char    *pOneLine2   = NULL;
    PCOUPL_LINK  pLink   = NULL;
    PCOUPL_LINK  pLink2  = NULL;
    
    if ( !pHead )
        utopia_init();

    UserAcquireLock(&gPsmLock);

    _ansc_sprintf(fullname, "%s%s", domainname, pRecordName);

    pLink = pHead;
    pLink2 = pLink;
    while( pLink )
    {
        if ( AnscEqualString( fullname, pLink->name , false ) )
        {
            break;
        }

        pLink2 = pLink;
        pLink = pLink->next;
    }


    if ( !pLink )
    {
        UserReleaseLock(&gPsmLock);

        return ANSC_STATUS_FAILURE;
    }
    else
    {
        if ( pLink2 == pLink )
        {
            pHead = pHead->next;
        }
        else
        {
            pLink2->next = pLink->next;
        }

        AnscFreeMemory( pLink->name);
        AnscFreeMemory( pLink->value);
        AnscFreeMemory( pLink);
        
    }
    UserReleaseLock(&gPsmLock);

    utopia_save();

    return ANSC_STATUS_SUCCESS;

}


static int Utopia_RawGet
(
    UtopiaContext*              ctx,
    char*                       domainname,
    char*                       pRecordName,
    char*                       pValue,
    ULONG                       size
)
{
    char    oneLine[256] = {0};
    char    fullname[512] = {0};
    char    *pOneLine    = NULL;
    char    *pOneLine2   = NULL;
    PCOUPL_LINK  pLink   = NULL;
    PCOUPL_LINK  pLink2  = NULL;

    if ( !pHead )
        utopia_init();

    UserAcquireLock(&gPsmLock);

    _ansc_sprintf(fullname, "%s%s", domainname, pRecordName);

    pLink = pHead;
    while( pLink )
    {
        if ( AnscEqualString( fullname, pLink->name , false ) )
        {
            break;
        }

        pLink = pLink->next;

    }

    UserReleaseLock(&gPsmLock);

    if ( pLink  )
    {
        _ansc_snprintf(pValue, size, "%s", pLink->value);
        return ANSC_STATUS_SUCCESS;
    }
    else
        return ANSC_STATUS_FAILURE;

}

static void Utopia_Free(UtopiaContext * ctx, ULONG commit)
{
    return;
}

#endif

/* this is test part End*/


static ULONG                               uDhcpv6ServerPoolNum                                  = 0;
static COSA_DML_DHCPSV6_POOL_FULL          sDhcpv6ServerPool[DHCPV6S_POOL_NUM]                   = {0};
static ULONG                               uDhcpv6ServerPoolOptionNum[DHCPV6S_POOL_NUM]          = {0};
static COSA_DML_DHCPSV6_POOL_OPTION        sDhcpv6ServerPoolOption[DHCPV6S_POOL_NUM][DHCPV6S_POOL_OPTION_NUM] = {{0}};
#if defined(MULTILAN_FEATURE)
static char v6addr_prev[IPV6_PREF_MAXLEN] = {0};
#endif

BOOL  g_dhcpv6_server      = TRUE;
ULONG g_dhcpv6_server_type = DHCPV6_SERVER_TYPE_STATELESS;

BOOL g_lan_ready = FALSE;
BOOL g_dhcpv6_server_prefix_ready = FALSE;
ULONG g_dhcpv6s_restart_count = 0;
ULONG g_dhcpv6s_refresh_count = 0;

#if defined (_HUB4_PRODUCT_REQ_)
    #define DHCPV6S_SERVER_PID_FILE   "/etc/dibbler/server.pid"
#else
    #define DHCPV6S_SERVER_PID_FILE   "/tmp/dibbler/server.pid"
#endif

#define DHCPVS_DEBUG_PRINT \
fprintf(stderr,"%s -- %d !!!!!!!!!!!!!!!!!!\n", __FUNCTION__, __LINE__);

#define SETS_INTO_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, value ) \
{  \
    UCHAR  Namespace[128] = {0}; \
    _ansc_snprintf(Namespace, sizeof(Namespace)-1, "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    Utopia_RawSet(&utctx,Namespace,parameter,value);  \
} \

#define SETI_INTO_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, value ) \
{  \
    UCHAR  Namespace[128]  = {0}; \
    UCHAR  Value[12]  = {0}; \
    _ansc_snprintf(Namespace, sizeof(Namespace)-1, "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    _ansc_snprintf(Value, sizeof(Value) -1, "%lu", (ULONG)value ); \
    Utopia_RawSet(&utctx,Namespace,parameter,Value);  \
} \

#define UNSET_INTO_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter ) \
{  \
    UCHAR  Namespace[128]  = {0}; \
    _ansc_snprintf(Namespace, sizeof(Namespace)-1, "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    syscfg_unset(Namespace,parameter);  \
} \

#define GETS_FROM_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, out ) \
{  \
    UCHAR  Namespace[128] = {0}; \
    _ansc_snprintf(Namespace, sizeof(Namespace)-1, "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    Utopia_RawGet(&utctx,Namespace, parameter, out,sizeof(out));  \
} \

#define GETI_FROM_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, out ) \
{  \
    UCHAR  Namespace[128] = {0}; \
    UCHAR  Value[12]  = {0}; \
    _ansc_snprintf(Namespace, sizeof(Namespace)-1, "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    Utopia_RawGet(&utctx,Namespace, parameter,Value,sizeof(Value));  \
    if ( strcmp(Value,"4294967295") == 0) out = -1; \
    else if ( Value[0] ) out = atoi(Value); \
}  \


/* set a ulong type value into utopia */
void setpool_into_utopia( PUCHAR uniqueName, PUCHAR table1Name, ULONG table1Index, PCOSA_DML_DHCPSV6_POOL_FULL pEntry )
{
    UtopiaContext utctx = {0};

    if (!Utopia_Init(&utctx))
        return;

    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "instancenumber", pEntry->Cfg.InstanceNumber)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "alias", pEntry->Cfg.Alias)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Order", pEntry->Cfg.Order)
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAInterface", pEntry->Cfg.Interface)
#else
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Interface", pEntry->Cfg.Interface)
#endif
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "VendorClassID", pEntry->Cfg.VendorClassID)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UserClassID", pEntry->Cfg.UserClassID)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddress", pEntry->Cfg.SourceAddress)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddressMask", pEntry->Cfg.SourceAddressMask)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAManualPrefixes", pEntry->Cfg.IANAManualPrefixes)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDManualPrefixes", pEntry->Cfg.IAPDManualPrefixes)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "PrefixRangeBegin", pEntry->Cfg.PrefixRangeBegin)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "PrefixRangeEnd", pEntry->Cfg.PrefixRangeEnd)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "LeaseTime", pEntry->Cfg.LeaseTime)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDAddLength", pEntry->Cfg.IAPDAddLength)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "DUID", pEntry->Cfg.DUID)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDEnable", pEntry->Cfg.IAPDEnable)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddressExclude", pEntry->Cfg.SourceAddressExclude)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAEnable", pEntry->Cfg.IANAEnable)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UserClassIDExclude", pEntry->Cfg.UserClassIDExclude)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "VendorClassIDExclude", pEntry->Cfg.VendorClassIDExclude)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "DUIDExclude", pEntry->Cfg.DUIDExclude)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "bEnabled", pEntry->Cfg.bEnabled)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Status", pEntry->Info.Status)
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAInterfacePrefixes", pEntry->Info.IANAPrefixes)
#else
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAPrefixes", pEntry->Info.IANAPrefixes)
#endif
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDPrefixes", pEntry->Info.IAPDPrefixes)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "RapidEnable", pEntry->Cfg.RapidEnable)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UnicastEnable", pEntry->Cfg.UnicastEnable)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "EUI64Enable", pEntry->Cfg.EUI64Enable)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAAmount", pEntry->Cfg.IANAAmount)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "StartAddress", pEntry->Cfg.StartAddress)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "X_RDKCENTRAL_COM_DNSServersEnabled", pEntry->Cfg.X_RDKCENTRAL_COM_DNSServersEnabled)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "X_RDKCENTRAL_COM_DNSServers", pEntry->Cfg.X_RDKCENTRAL_COM_DNSServers)

    Utopia_Free(&utctx,1);

    return;
}

void unsetpool_from_utopia( PUCHAR uniqueName, PUCHAR table1Name, ULONG table1Index )
{
    UtopiaContext utctx = {0};

    if (!Utopia_Init(&utctx))
        return;

    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "instancenumber" )
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "alias")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Order")
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAInterface")
#else
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Interface")
#endif
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "VendorClassID")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UserClassID")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddress")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddressMask")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAManualPrefixes")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDManualPrefixes")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "PrefixRangeBegin")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "PrefixRangeEnd")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "LeaseTime")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDAddLength")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "DUID")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDEnable")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddressExclude")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAEnable")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UserClassIDExclude")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "VendorClassIDExclude")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "DUIDExclude")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "bEnabled")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Status")
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAInterfacePrefixes")
#else
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAPrefixes")
#endif
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDPrefixes")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "RapidEnable")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UnicastEnable")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "EUI64Enable")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAAmount")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "StartAddress")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "X_RDKCENTRAL_COM_DNSServersEnabled")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "X_RDKCENTRAL_COM_DNSServers")

    Utopia_Free(&utctx,1); 

    return;
}

void getpool_from_utopia( PUCHAR uniqueName, PUCHAR table1Name, ULONG table1Index, PCOSA_DML_DHCPSV6_POOL_FULL pEntry )
{
    UtopiaContext utctx = {0};
 #if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_BCI_FEATURE_REQ)
    char *INVALID_IANAInterfacePrefixes = "Device.IP.Interface.4.IPv6Prefix.1.";
    char *FIXED_IANAInterfacePrefixes   = "Device.IP.Interface.1.IPv6Prefix.1.";
#endif

    if (!Utopia_Init(&utctx))
        return;

    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "instancenumber", pEntry->Cfg.InstanceNumber)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "alias", pEntry->Cfg.Alias)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Order", pEntry->Cfg.Order)
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAInterface", pEntry->Cfg.Interface)
#else
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Interface", pEntry->Cfg.Interface)
#endif
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "VendorClassID", pEntry->Cfg.VendorClassID)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UserClassID", pEntry->Cfg.UserClassID)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddress", pEntry->Cfg.SourceAddress)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddressMask", pEntry->Cfg.SourceAddressMask)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAManualPrefixes", pEntry->Cfg.IANAManualPrefixes)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDManualPrefixes", pEntry->Cfg.IAPDManualPrefixes)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "PrefixRangeBegin", pEntry->Cfg.PrefixRangeBegin)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "PrefixRangeEnd", pEntry->Cfg.PrefixRangeEnd)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "LeaseTime", pEntry->Cfg.LeaseTime)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDAddLength", pEntry->Cfg.IAPDAddLength)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "DUID", pEntry->Cfg.DUID)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDEnable", pEntry->Cfg.IAPDEnable)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "SourceAddressExclude", pEntry->Cfg.SourceAddressExclude)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAEnable", pEntry->Cfg.IANAEnable)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UserClassIDExclude", pEntry->Cfg.UserClassIDExclude)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "VendorClassIDExclude", pEntry->Cfg.VendorClassIDExclude)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "DUIDExclude", pEntry->Cfg.DUIDExclude)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "bEnabled", pEntry->Cfg.bEnabled)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "Status", pEntry->Info.Status)
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAInterfacePrefixes", pEntry->Info.IANAPrefixes)
 #if defined(_BCI_FEATURE_REQ)
    CcspTraceInfo(("%s table1Name: %s, table1Index: %d, get IANAInterfacePrefixes: %s\n",
                  __func__, table1Name, table1Index, pEntry->Info.IANAPrefixes));

    if ( _ansc_strcmp(table1Name, "pool") == 0 && table1Index == 0 &&
        _ansc_strcmp(pEntry->Info.IANAPrefixes, INVALID_IANAInterfacePrefixes) == 0)
      {
       CcspTraceInfo(("%s Fix invalid IANAInterfacePrefixes by setting it to new default: %s\n", __func__, FIXED_IANAInterfacePrefixes));
       SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAInterfacePrefixes", FIXED_IANAInterfacePrefixes)
       GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAInterfacePrefixes", pEntry->Info.IANAPrefixes)
       CcspTraceInfo(("%s Try again to get IANAInterfacePrefixes: %s\n", __func__, pEntry->Info.IANAPrefixes));
      }
 #endif 
#else
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAPrefixes", pEntry->Info.IANAPrefixes)
#endif
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IAPDPrefixes", pEntry->Info.IAPDPrefixes)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "RapidEnable", pEntry->Cfg.RapidEnable)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "UnicastEnable", pEntry->Cfg.UnicastEnable)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "EUI64Enable", pEntry->Cfg.EUI64Enable)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "IANAAmount", pEntry->Cfg.IANAAmount)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "StartAddress", pEntry->Cfg.StartAddress)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "X_RDKCENTRAL_COM_DNSServersEnabled", pEntry->Cfg.X_RDKCENTRAL_COM_DNSServersEnabled)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, "", 0, "X_RDKCENTRAL_COM_DNSServers", pEntry->Cfg.X_RDKCENTRAL_COM_DNSServers)

    Utopia_Free(&utctx,0);

    return;
}


void setpooloption_into_utopia( PUCHAR uniqueName, PUCHAR table1Name, ULONG table1Index, PUCHAR table2Name, ULONG table2Index, PCOSA_DML_DHCPSV6_POOL_OPTION pEntry )
{
    UtopiaContext utctx = {0};

    if (!Utopia_Init(&utctx))
        return;

    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "InstanceNumber", pEntry->InstanceNumber)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "alias", pEntry->Alias)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "Tag", pEntry->Tag)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "PassthroughClient", pEntry->PassthroughClient)
    SETS_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "Value", pEntry->Value)
    SETI_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "bEnabled", pEntry->bEnabled)

    Utopia_Free(&utctx,1);

    return;
}

void unsetpooloption_from_utopia( PUCHAR uniqueName, PUCHAR table1Name, ULONG table1Index, PUCHAR table2Name, ULONG table2Index )
{
    UtopiaContext utctx = {0};

    if (!Utopia_Init(&utctx))
        return;

    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "InstanceNumber" )
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "alias")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "Tag")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "PassthroughClient")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "Value")
    UNSET_INTO_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "bEnabled")

    Utopia_Free(&utctx,1); 

    return;
}

void getpooloption_from_utopia( PUCHAR uniqueName, PUCHAR table1Name, ULONG table1Index, PUCHAR table2Name, ULONG table2Index, PCOSA_DML_DHCPSV6_POOL_OPTION pEntry )

{
    UtopiaContext utctx = {0};

    if (!Utopia_Init(&utctx))
        return;

    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "InstanceNumber", pEntry->InstanceNumber)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "alias", pEntry->Alias)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "Tag", pEntry->Tag)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "PassthroughClient", pEntry->PassthroughClient)
    GETS_FROM_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "Value", pEntry->Value)
    GETI_FROM_UTOPIA(uniqueName, table1Name, table1Index, table2Name, table2Index, "bEnabled", pEntry->bEnabled)

    Utopia_Free(&utctx,0);

    return;
}

/* Server global variable  End*/

static int _prepare_client_conf(PCOSA_DML_DHCPCV6_CFG       pCfg);
static int _dibbler_client_operation(char * arg);

int safe_strcpy(char * dst, char * src, int dst_size);

static COSA_DML_DHCPCV6_FULL  g_dhcpv6_client;

static int _dibbler_server_operation(char * arg);
void _cosa_dhcpsv6_refresh_config();
static int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger);
#define DHCPS6V_SERVER_RESTART_FIFO "/tmp/ccsp-dhcpv6-server-restart-fifo.txt"

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(_CBR_PRODUCT_REQ_) && ! defined(_BCI_FEATURE_REQ)

#else
ANSC_STATUS
CosaDmlDhcpv6SMsgHandler
    (
        ANSC_HANDLE                 hContext
    )
{
    char ret[16] = {0};
    
    /*We may restart by DM manager when pam crashed. We need to get current two status values */
    commonSyseventGet("lan-status", ret, sizeof(ret));
    if ( !strncmp(ret, "started", strlen("started") ) ) {
        DHCPVS_DEBUG_PRINT
        g_lan_ready = TRUE;
    }

    commonSyseventGet("ipv6_prefix", ret, sizeof(ret));
    if (strlen(ret) > 3) {
        DHCPVS_DEBUG_PRINT
        g_dhcpv6_server_prefix_ready = TRUE;
    }

    /*we start a thread to hear dhcpv6 client message about prefix/address */
    if ( ( !mkfifo(CCSP_COMMON_FIFO, 0666) || errno == EEXIST ) &&
         ( !mkfifo(DHCPS6V_SERVER_RESTART_FIFO, 0666) || errno == EEXIST ) )
    {
        if (pthread_create(&g_be_ctx.dbgthrd, NULL, dhcpv6c_dbg_thrd, NULL)  || pthread_detach(g_be_ctx.dbgthrd)) 
            CcspTraceWarning(("%s error in creating dhcpv6c_dbg_thrd\n", __FUNCTION__));
    }

    //CosaDmlStartDHCP6Client();
//    dhcp v6 client is now initialized in service_wan, no need to initialize from PandM
    #if 0
    pthread_t dibblerthread;
    pthread_create(&dibblerthread, NULL, &CosaDmlStartDHCP6Client, NULL);
    #endif
    return 0;
}

int CosaDmlDhcpv6sRestartOnLanStarted(void * arg)
{
    CcspTraceWarning(("%s -- lan status is started. \n", __FUNCTION__));
    g_lan_ready = TRUE;

#if defined(_CBR_PRODUCT_REQ_)
    // TCCBR-3353: dibbler-server is failing because brlan's IP address is "tentative".
    // Add a delay so the IP address has time to become "prefered".
    sleep(2);
#endif

    CosaDmlDHCPv6sTriggerRestart(TRUE);

    //the thread will start dibbler if need

    return 0;
}
#endif

ANSC_STATUS
CosaDmlDhcpv6Init
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    UtopiaContext utctx = {0};
    ULONG         Index = 0;
    ULONG         Index2 = 0;
    DSLHDMAGNT_CALLBACK *  pEntry = NULL;
    char         value[32] = {0};
	BOOLEAN		 bIsChangesHappened = FALSE;


#if 0
    /* This is for test Begin */
    utopia_init();
    /* This is for test End */
#endif


    /* Server Part Beginning */

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
#ifdef _HUB4_PRODUCT_REQ_
    /* Dibbler-init is called to set the pre-configuration for dibbler */
    CcspTraceInfo(("%s dibbler-init.sh Called \n", __func__));
    system("/etc/dibbler/dibbler-init.sh");
#endif
    GETI_FROM_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "serverenable", g_dhcpv6_server)

    /*We need enable dhcpv6 for 1.5.1 and above by default*/
    Utopia_RawGet(&utctx,NULL, "router_enabledhcpv6_DoOnce",value,sizeof(value));
    if ( !g_dhcpv6_server && value[0]!= '1' )
    {
        g_dhcpv6_server = 1;
        SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "serverenable", g_dhcpv6_server)
    }
   
    if ( value[0] != '1' )
    {
        Utopia_RawSet(&utctx,NULL,"router_enabledhcpv6_DoOnce","1");
    }

    GETI_FROM_UTOPIA(DHCPV6S_NAME,    "", 0, "", 0, "servertype",   g_dhcpv6_server_type)
    GETI_FROM_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "poolnumber", uDhcpv6ServerPoolNum)

    if (uDhcpv6ServerPoolNum > DHCPV6S_POOL_NUM) {
        uDhcpv6ServerPoolNum = DHCPV6S_POOL_NUM;
		bIsChangesHappened = TRUE;
    }

#ifndef _HUB4_PRODUCT_REQ_
    /*This logic code is used to change default behavior to stateful dhcpv6 server */
    Utopia_RawGet(&utctx,NULL, "router_statefuldhcpv6_DoOnce",value,sizeof(value));
    if ( value[0]!= '1'  && g_dhcpv6_server_type == 2 )
    {
        g_dhcpv6_server_type = 1;
        Utopia_RawSet(&utctx,NULL,"router_other_flag","1");
        Utopia_RawSet(&utctx,NULL,"router_managed_flag","1");
        SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "servertype", g_dhcpv6_server_type)

        system("sysevent set zebra-restart");
    }
#endif

    if ( value[0]!= '1' )
    {
        Utopia_RawSet(&utctx,NULL,"router_statefuldhcpv6_DoOnce","1");
    }

    Utopia_Free(&utctx,1);

	/*  Set server pool when changed case */
	if( TRUE == bIsChangesHappened )
	{
		if (!Utopia_Init(&utctx))
			return ANSC_STATUS_FAILURE;
        SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "poolnumber", uDhcpv6ServerPoolNum)
		Utopia_Free(&utctx,1);
	}

    /* RDKB-6780, CID-33283, Out-of-bounds read
    ** Maximum size of Dhcp Server Pool and DHCP Server Pool Option Number 
    ** is defined as DHCPV6S_POOL_NUM. Limiting max number w.r.t global variables defined.
    */
    for ( Index = 0; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM); Index++ )
    {
        getpool_from_utopia(DHCPV6S_NAME, "pool", Index, &sDhcpv6ServerPool[Index]);

        if (!Utopia_Init(&utctx))
            return ANSC_STATUS_FAILURE;
        GETI_FROM_UTOPIA(DHCPV6S_NAME,  "pool", Index, "", 0, "optionnumber", uDhcpv6ServerPoolOptionNum[Index])
        Utopia_Free(&utctx,0);

        for( Index2 = 0; (Index2 < uDhcpv6ServerPoolOptionNum[Index]) && (Index2 < DHCPV6S_POOL_OPTION_NUM); Index2++ )
        {
            getpooloption_from_utopia(DHCPV6S_NAME,"pool",Index,"option",Index2,&sDhcpv6ServerPoolOption[Index][Index2]);
        }
    }

    if ( uDhcpv6ServerPoolNum == 0 )
    {
        uDhcpv6ServerPoolNum = 1;

        AnscZeroMemory( &sDhcpv6ServerPool[0], sizeof(sDhcpv6ServerPool[0]));
        sDhcpv6ServerPool[0].Cfg.InstanceNumber = 1;
        _ansc_sprintf(sDhcpv6ServerPool[0].Cfg.Alias, "%s%d", "Pool",1);
        sDhcpv6ServerPool[0].Cfg.bEnabled = TRUE; //By default it's TRUE because we will use stateless DHCPv6 server at lease.
        sDhcpv6ServerPool[0].Cfg.Order = 1;
        sDhcpv6ServerPool[0].Info.Status  = COSA_DML_DHCP_STATUS_Disabled;
        sDhcpv6ServerPool[0].Cfg.IANAEnable = FALSE;
        sDhcpv6ServerPool[0].Cfg.IAPDEnable = FALSE;
        sDhcpv6ServerPool[0].Cfg.IAPDAddLength = 64; // actually, this is subnet mask
        sDhcpv6ServerPool[0].Cfg.SourceAddressExclude = FALSE;
        sDhcpv6ServerPool[0].Cfg.DUIDExclude = FALSE;
        sDhcpv6ServerPool[0].Cfg.VendorClassIDExclude = FALSE;
        sDhcpv6ServerPool[0].Cfg.UserClassIDExclude= FALSE;
        sDhcpv6ServerPool[0].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled = FALSE;

        if (!Utopia_Init(&utctx))
            return ANSC_STATUS_FAILURE;
        SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "poolnumber", uDhcpv6ServerPoolNum)
        Utopia_Free(&utctx,1);

        setpool_into_utopia(DHCPV6S_NAME, "pool", 0, &sDhcpv6ServerPool[0]);
    }

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "serverenable", g_dhcpv6_server)
    Utopia_Free(&utctx,1);

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(_CBR_PRODUCT_REQ_) && ! defined(_BCI_FEATURE_REQ) 

#else

    /*register callback function to handle message from wan dchcp6 client */
    pEntry = (PDSLHDMAGNT_CALLBACK)AnscAllocateMemory(sizeof(*pEntry));
    pEntry->func = CosaDmlDhcpv6SMsgHandler;
    g_RegisterCallBackAfterInitDml(g_pDslhDmlAgent, pEntry);

    /*register callback function to restart dibbler-server at right time*/
    CcspTraceWarning(("%s -- %d register lan-status to event dispatcher \n", __FUNCTION__, __LINE__));
    EvtDispterRgstCallbackForEvent("lan-status", CosaDmlDhcpv6sRestartOnLanStarted, NULL);


#endif


    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API retrieves the number of DHCP clients in the system.
 */
ULONG
CosaDmlDhcpv6cGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{
    PCOSA_DATAMODEL_DHCPV6          pDhcpv6           = (PCOSA_DATAMODEL_DHCPV6)g_pCosaBEManager->hDhcpv6;
    
    return 1;
}

/*
    Description:
        The API retrieves the complete info of the DHCP clients designated by index. The usual process is the caller gets the total number of entries, then iterate through those by calling this API.
    Arguments:
        ulIndex        Indicates the index number of the entry.
        pEntry        To receive the complete info of the entry.
*/
static int _get_client_duid(UCHAR * out, int len)
{
    char buf[256] = {0};
    FILE * fp = fopen(CLIENT_DUID_FILE, "r+");
    int  i = 0;
    int  j = 0;

    if(fp)
    {
        fgets(buf, sizeof(buf), fp);
        
        if(buf[0])
        {
            while(j<len && buf[i])
            {
                if (buf[i] != ':')
                    out[j++] = buf[i];

                i++;
            }
        }
        fclose(fp);
    }

    return 0;
}


ANSC_STATUS
CosaDmlDhcpv6cGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPCV6_FULL      pEntry
    )
{  
    UtopiaContext utctx = {0};
    char buf[256] = {0};
    char out[256] = {0};
    int  has_set  = 0;

    if (ulIndex)
        return ANSC_STATUS_FAILURE;
    
    pEntry->Cfg.InstanceNumber = 1;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    /*Cfg members*/
    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_alias", sizeof(buf));
    memset(pEntry->Cfg.Alias, 0, sizeof(pEntry->Cfg.Alias));
    Utopia_RawGet(&utctx,NULL,buf,pEntry->Cfg.Alias,sizeof(pEntry->Cfg.Alias));

    pEntry->Cfg.SuggestedT1 = pEntry->Cfg.SuggestedT2 = 0;
    
    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_t1", sizeof(buf));
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    sscanf(out, "%d", &pEntry->Cfg.SuggestedT1);

    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_t2", sizeof(buf));
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    sscanf(out, "%d", &pEntry->Cfg.SuggestedT2);

    /*pEntry->Cfg.Interface stores interface name, dml will calculate the full path name*/
    safe_strcpy(pEntry->Cfg.Interface, COSA_DML_DHCPV6_CLIENT_IFNAME, sizeof(pEntry->Cfg.Interface));
    
    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_requested_options", sizeof(buf));
    memset(pEntry->Cfg.RequestedOptions, 0, sizeof(pEntry->Cfg.RequestedOptions));
    Utopia_RawGet(&utctx,NULL,buf,pEntry->Cfg.RequestedOptions,sizeof(pEntry->Cfg.RequestedOptions));

    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_enabled", sizeof(buf));
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    pEntry->Cfg.bEnabled = (out[0] == '1') ? TRUE:FALSE;

    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_iana_enabled", sizeof(buf));
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    pEntry->Cfg.RequestAddresses = (out[0] == '1') ? TRUE:FALSE;
    
    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_iapd_enabled", sizeof(buf));
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    pEntry->Cfg.RequestPrefixes = (out[0] == '1') ? TRUE:FALSE;

    safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_rapidcommit_enabled", sizeof(buf));
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    pEntry->Cfg.RapidCommit = (out[0] == '1') ? TRUE:FALSE;

    /*Info members*/
    if (pEntry->Cfg.bEnabled)
        pEntry->Info.Status = COSA_DML_DHCP_STATUS_Enabled;
    else 
        pEntry->Info.Status = COSA_DML_DHCP_STATUS_Disabled;

    /*TODO: supported options*/

    _get_client_duid(pEntry->Info.DUID, sizeof(pEntry->Info.DUID));

    Utopia_Free(&utctx,0);                

    /*if we don't have alias, set a default one*/
    if (!pEntry->Cfg.Alias[0])
    {
        if (Utopia_Init(&utctx))
        {
            Utopia_RawSet(&utctx,NULL,SYSCFG_FORMAT_DHCP6C"_alias","Client1");
            Utopia_Free(&utctx,1);                
           
            strcpy(pEntry->Cfg.Alias, "Client1");
        }
    }

    AnscCopyMemory(&g_dhcpv6_client, pEntry, sizeof(g_dhcpv6_client));
    
    if (pEntry->Cfg.bEnabled)
        _prepare_client_conf(&pEntry->Cfg);
        
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cSetValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UtopiaContext utctx = {0};

    if (ulIndex)
        return ANSC_STATUS_FAILURE;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    


    safe_strcpy(g_dhcpv6_client.Cfg.Alias, pAlias, sizeof(g_dhcpv6_client.Cfg.Alias));
    Utopia_RawSet(&utctx,NULL,SYSCFG_FORMAT_DHCP6C"_alias",pAlias);

    Utopia_Free(&utctx,1);
    
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API adds DHCP client. 
    Arguments:
        pEntry        Caller fills in pEntry->Cfg, except Alias field. Upon return, callee fills pEntry->Cfg.Alias field and as many as possible fields in pEntry->Info.
*/
ANSC_STATUS
CosaDmlDhcpv6cAddEntry
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPCV6_FULL        pEntry
    )
{
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API removes the designated DHCP client entry. 
    Arguments:
        pAlias        The entry is identified through Alias.
*/
ANSC_STATUS
CosaDmlDhcpv6cDelEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    return ANSC_STATUS_SUCCESS;
}

/* 
A sample content of client.conf
iface wan0 {
    rapid-commit yes
    ia {
    t1 1000
    t2 2000
    }
    pd {
    t1 1000
    t2 2000
    }
}
*/
#define TMP_CLIENT_CONF "/tmp/.dibbler_client_conf"
#define CLIENT_CONF_LOCATION  "/etc/dibbler/client.conf"
#define TMP_SERVER_CONF "/tmp/.dibbler_server_conf"
#define SERVER_CONF_LOCATION  "/etc/dibbler/server.conf"

static int _prepare_client_conf(PCOSA_DML_DHCPCV6_CFG       pCfg)
{
    FILE * fp = fopen(TMP_CLIENT_CONF, "w+");
    char line[256] = {0};

    if (fp)
    {
        /*we need this to get IANA IAPD info from dibbler*/
        fprintf(fp, "notify-scripts\n");

        fprintf(fp, "iface %s {\n", pCfg->Interface);
        
        if (pCfg->RapidCommit)
            fprintf(fp, "    rapid-commit yes\n");
        
        if (pCfg->RequestAddresses)
        {
            fprintf(fp, "    ia {\n");

            if (pCfg->SuggestedT1)
            {
                memset(line, 0, sizeof(line));
                snprintf(line, sizeof(line)-1, "    t1 %d\n", pCfg->SuggestedT1);
                fprintf(fp, "%s", line);
            }

            if (pCfg->SuggestedT2)
            {
                memset(line, 0, sizeof(line));
                snprintf(line, sizeof(line)-1, "    t2 %d\n", pCfg->SuggestedT2);
                fprintf(fp, "%s", line);
            }

            fprintf(fp, "    }\n");
        }

        if (pCfg->RequestPrefixes)
        {
            fprintf(fp, "    pd {\n");

            if (pCfg->SuggestedT1)
            {
                memset(line, 0, sizeof(line));
                snprintf(line, sizeof(line)-1, "    t1 %d\n", pCfg->SuggestedT1);
                fprintf(fp, "%s", line);
            }

            if (pCfg->SuggestedT2)
            {
                memset(line, 0, sizeof(line));
                snprintf(line, sizeof(line)-1, "    t2 %d\n", pCfg->SuggestedT2);
                fprintf(fp, "%s", line);
            }

            fprintf(fp, "    }\n");
        }
        
        fprintf(fp, "}\n");
        
        fclose(fp);
    }

    /*we will copy the updated conf file at once*/
    if (rename(TMP_CLIENT_CONF, CLIENT_CONF_LOCATION))
        CcspTraceWarning(("%s rename failed %s\n", __FUNCTION__, strerror(errno)));

    return 0;
}

void _get_shell_output(char * cmd, char * out, int len);
int _get_shell_output2(char * cmd, char * dststr);

static int _dibbler_client_operation(char * arg)
{
    char cmd[256] = {0};
    char out[256] = {0};
#if defined (INTEL_PUMA7)
    int watchdog = NO_OF_RETRY;
#endif

    if (!strncmp(arg, "stop", 4))
    {
        CcspTraceInfo(("%s stop\n", __func__));
        /*TCXB6 is also calling service_dhcpv6_client.sh but the actuall script is installed from meta-rdk-oem layer as the intel specific code
                   had to be removed */
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(DHCPV6_PREFIX_FIX)
        commonSyseventSet("dhcpv6_client-stop", "");
#else
        system("/etc/utopia/service.d/service_dhcpv6_client.sh disable");
#endif
      
#ifdef _COSA_BCM_ARM_
        sprintf(cmd, "killall %s", CLIENT_BIN);
        system(cmd);
        sleep(2);
        sprintf(cmd, "ps -A|grep %s", CLIENT_BIN);
        _get_shell_output(cmd, out, sizeof(out));
         if (strstr(out, CLIENT_BIN))
         {
            sprintf(cmd, "killall -9 %s", CLIENT_BIN);
            system(cmd);
          }
#endif
    }
    else if (!strncmp(arg, "start", 5))
    {
        CcspTraceInfo(("%s start\n", __func__));

#if defined(INTEL_PUMA7) || defined(_COSA_BCM_ARM_)

#if defined(INTEL_PUMA7)
        //Intel Proposed RDKB Generic Bug Fix from XB6 SDK      
        /* Waiting for the TLV file to be parsed correctly so that the right erouter mode can be used in the code below.
        For ANYWAN please extend the below code to support the case of TLV config file not being there. */
      do{
         sprintf(cmd, "sysevent get TLV202-status");
         _get_shell_output(cmd, out, sizeof(out));
         fprintf( stderr, "\n%s:%s(): Waiting for CcspGwProvApp to parse TLV config file\n", __FILE__, __FUNCTION__);
         sleep(1);//sleep(1) is to avoid lots of trace msgs when there is latency
         watchdog--;
        }while((!strstr(out,"success")) && (watchdog != 0));

        if(watchdog == 0)
        {
            //When 60s have passed and the file is still not configured by CcspGwprov module
            fprintf(stderr, "\n%s()%s(): TLV data has not been initialized by CcspGwProvApp.Continuing with the previous configuration\n",__FILE__, __FUNCTION__);  
        }
#endif
		
        sprintf(cmd, "syscfg get last_erouter_mode");
        _get_shell_output(cmd, out, sizeof(out));
	/* TODO: To be fixed by Comcast
	         IPv6 address assigned to erouter0 gets deleted when erouter_mode=3(IPV4 and IPV6 both)
	         Don't start v6 service in parallel. Wait for wan-status to be set to 'started' by IPv4 DHCP client.
	*/
        if (strstr(out, "3"))// If last_erouter_mode is both IPV4/IPV6
	{
	     	do{
            		sprintf(cmd, "sysevent get wan-status");
        		_get_shell_output(cmd, out, sizeof(out));
        		CcspTraceInfo(("%s waiting for wan-status to started\n", __func__));
			sleep(1);//sleep(1) is to avoid lots of trace msgs when there is latency
		}while(!strstr(out,"started"));
	}
#endif
        /*This is for ArrisXB6 */
        /*TCXB6 is also calling service_dhcpv6_client.sh but the actuall script is installed from meta-rdk-oem layer as the intel specific code
         had to be removed */
        CcspTraceInfo(("%s  Callin service_dhcpv6_client.sh enable \n", __func__));
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(DHCPV6_PREFIX_FIX)
      commonSyseventSet("dhcpv6_client-start", "");
#else
        system("/etc/utopia/service.d/service_dhcpv6_client.sh enable");
#endif
      
#ifdef _COSA_BCM_ARM_
        /* Dibbler-init is called to set the pre-configuration for dibbler */            
        CcspTraceInfo(("%s dibbler-init.sh Called \n", __func__));
        system("/etc/dibbler/dibbler-init.sh");
        /*Start Dibber client for tchxb6*/
        CcspTraceInfo(("%s Dibbler Client Started \n", __func__));
        sprintf(cmd, "%s start", CLIENT_BIN);
        system(cmd);
#endif
    }
    else if (!strncmp(arg, "restart", 7))
    {
        _dibbler_client_operation("stop");
        _dibbler_client_operation("start");
    }

    return 0;
}
/*internal.c will call this when obtained Client and SentOptions. so we don't need to start service in getEntry for Client and SentOptions.*/
int  CosaDmlStartDHCP6Client()
{
    pthread_detach(pthread_self());
#if defined(_COSA_INTEL_XB3_ARM_)
    CcspTraceInfo(("Not restarting ti_dhcp6c for XB3 case\n"));
#else
   // _dibbler_client_operation("restart");    
#endif
    return 0;
}
/*
Description:
    The API re-configures the designated DHCP client entry. 
Arguments:
    pAlias        The entry is identified through Alias.
    pEntry        The new configuration is passed through this argument, even Alias field can be changed.
*/
ANSC_STATUS
CosaDmlDhcpv6cSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPCV6_CFG       pCfg
    )
{
    UtopiaContext utctx = {0};
    char buf[256] = {0};
    char out[256] = {0};
    int  need_to_restart_service = 0;

    if (pCfg->InstanceNumber != 1)
        return ANSC_STATUS_FAILURE;

    /*handle syscfg*/
    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;


    if (!AnscEqualString(pCfg->Alias, g_dhcpv6_client.Cfg.Alias, TRUE))
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_alias", sizeof(buf));
        Utopia_RawSet(&utctx,NULL,buf,pCfg->Alias);
    }

    if (pCfg->SuggestedT1 != g_dhcpv6_client.Cfg.SuggestedT1)
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_t1", sizeof(buf));
        sprintf(out, "%d", pCfg->SuggestedT1);
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }

    if (pCfg->SuggestedT2 != g_dhcpv6_client.Cfg.SuggestedT2)
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_t2", sizeof(buf));
        sprintf(out, "%d", pCfg->SuggestedT2);
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }

    if (!AnscEqualString(pCfg->RequestedOptions, g_dhcpv6_client.Cfg.RequestedOptions, TRUE))
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_requested_options", sizeof(buf));
        Utopia_RawSet(&utctx,NULL,buf,pCfg->RequestedOptions);
        need_to_restart_service = 1;
    }

    if (pCfg->bEnabled != g_dhcpv6_client.Cfg.bEnabled)
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_enabled", sizeof(buf));
        out[0] = pCfg->bEnabled ? '1':'0';
        out[1] = 0;
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }

    if (pCfg->RequestAddresses != g_dhcpv6_client.Cfg.RequestAddresses)
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_iana_enabled", sizeof(buf));
        out[0] = pCfg->RequestAddresses ? '1':'0';
        out[1] = 0;
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }    

    if (pCfg->RequestPrefixes != g_dhcpv6_client.Cfg.RequestPrefixes)
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_iapd_enabled", sizeof(buf));
        out[0] = pCfg->RequestPrefixes ? '1':'0';
        out[1] = 0;
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }    

    if (pCfg->RapidCommit != g_dhcpv6_client.Cfg.RapidCommit)
    {
        safe_strcpy(buf, SYSCFG_FORMAT_DHCP6C"_rapidcommit_enabled", sizeof(buf));
        out[0] = pCfg->RapidCommit ? '1':'0';
        out[1] = 0;
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }    

    Utopia_Free(&utctx,1);

    /*update dibbler-client service if necessary*/
    if (need_to_restart_service)
    {
        if ( pCfg->bEnabled != g_dhcpv6_client.Cfg.bEnabled &&  !pCfg->bEnabled )
            _dibbler_client_operation("stop");
        else if (pCfg->bEnabled != g_dhcpv6_client.Cfg.bEnabled &&  pCfg->bEnabled )
        {
            _prepare_client_conf(pCfg);
            _dibbler_client_operation("restart");
        }
        else if (pCfg->bEnabled == g_dhcpv6_client.Cfg.bEnabled && !pCfg->bEnabled)
        {
            /*do nothing*/
        }
        else if (pCfg->bEnabled == g_dhcpv6_client.Cfg.bEnabled && pCfg->bEnabled)
        {
            _prepare_client_conf(pCfg);
            _dibbler_client_operation("restart");
        }
    }
    
    AnscCopyMemory(&g_dhcpv6_client.Cfg, pCfg, sizeof(COSA_DML_DHCPCV6_CFG));

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPCV6_CFG       pCfg
    )
{
    /*for rollback*/
    if (pCfg->InstanceNumber != 1)
        return ANSC_STATUS_FAILURE;
    
    AnscCopyMemory(pCfg,  &g_dhcpv6_client.Cfg, sizeof(COSA_DML_DHCPCV6_CFG));
  
    return ANSC_STATUS_SUCCESS;
}

BOOL
CosaDmlDhcpv6cGetEnabled
    (
        ANSC_HANDLE                 hContext
    )
{
    BOOL bEnabled = FALSE;
    char out[256] = {0};
    BOOL dibblerEnabled = FALSE;

// For XB3, AXB6 if dibbler flag enabled, check dibbler-client process status
#if defined(_COSA_INTEL_XB3_ARM_) || defined(INTEL_PUMA7)
        char buf[8];
        if(( syscfg_get( NULL, "dibbler_client_enable", buf, sizeof(buf))==0) && (strcmp(buf, "true") == 0))
	{
		dibblerEnabled = TRUE;
	}
#endif

#if defined (_HUB4_PRODUCT_REQ_)
    // For HUB4, check dhcp6c process status
    FILE *fp = popen("ps |grep -i dhcp6c | grep -v grep", "r");

#elif defined (_COSA_BCM_ARM_)
    FILE *fp = popen("ps |grep -i dibbler-client | grep -v grep", "r");

#elif defined (_XF3_PRODUCT_REQ_)
   FILE *fp = popen("/usr/sbin/dibbler-client status |grep  client", "r");

#else
	FILE *fp;
	// For XB3, AXB6 if dibbler flag enabled, check dibbler-client process status
	if(dibblerEnabled)
		fp = popen("ps |grep -i dibbler-client | grep -v grep", "r");
	else
    		fp = popen("ps |grep -i ti_dhcp6c | grep erouter0 | grep -v grep", "r");
#endif

    if ( fp != NULL){
        if ( fgets(out, sizeof(out), fp) != NULL ){
#if defined (_HUB4_PRODUCT_REQ_)
            if ( _ansc_strstr(out, "dhcp6c") )
                bEnabled = TRUE;
#elif defined (_COSA_BCM_ARM_)
            if ( _ansc_strstr(out, "dibbler-client") )
                bEnabled = TRUE;
#elif defined (_XF3_PRODUCT_REQ_)
            if ( strstr(out, "RUNNING,") )
                bEnabled = TRUE;
#else
	// For XB3, AXB6 if dibbler flag enabled, check dibbler-client process status
	if ( dibblerEnabled && _ansc_strstr(out, "dibbler-client") )
                bEnabled = TRUE;
        if ( _ansc_strstr(out, "erouter_dhcp6c") )
                bEnabled = TRUE;
#endif
       }
        pclose(fp);
    }

    return bEnabled;
}

ANSC_STATUS
CosaDmlDhcpv6cGetInfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_DHCPCV6_INFO      pInfo
    )
{
    PCOSA_DML_DHCPCV6_FULL            pDhcpc            = (PCOSA_DML_DHCPCV6_FULL)hContext;

    _get_client_duid(g_dhcpv6_client.Info.DUID, sizeof(pInfo->DUID));

    if (pDhcpc)
    {
        if (pDhcpc->Cfg.bEnabled)
            g_dhcpv6_client.Info.Status = pDhcpc->Info.Status = COSA_DML_DHCP_STATUS_Enabled;
        else 
            g_dhcpv6_client.Info.Status = pDhcpc->Info.Status = COSA_DML_DHCP_STATUS_Disabled;
    }   

    AnscCopyMemory(pInfo,  &g_dhcpv6_client.Info, sizeof(COSA_DML_DHCPCV6_INFO));
    
    return ANSC_STATUS_SUCCESS;
}

#define CLIENT_SERVER_INFO_FILE "/tmp/.dibbler-info/client_server"
/*this file's format:
 line 1: addr server_ipv6_addr
 line 2: duid server_duid*/
/* this memory need to be freed by caller */
ANSC_STATUS
CosaDmlDhcpv6cGetServerCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SVR      *ppCfg,
        PULONG                      pSize
    )
{
    FILE * fp = fopen(CLIENT_SERVER_INFO_FILE, "r+");

    *pSize = 0;
    if (fp)
    {
        char buf[1024] = {0};
        char val[1024] = {0};
        int  entry_count = 0;

        /*we only support one entry of ServerCfg*/
        *ppCfg = (PCOSA_DML_DHCPCV6_SVR)AnscAllocateMemory( sizeof(COSA_DML_DHCPCV6_SVR) );
        memset(*ppCfg, 0, sizeof(COSA_DML_DHCPCV6_SVR));
        
        /*InformationRefreshTime not supported*/
        safe_strcpy((*ppCfg)->InformationRefreshTime, "0001-01-01T00:00:00Z", sizeof((*ppCfg)->InformationRefreshTime));
        
        while(fgets(buf, sizeof(buf)-1, fp))
        {
            memset(val, 0, sizeof(val));
            if (sscanf(buf, "addr %s", val))
            {
                safe_strcpy((*ppCfg)->SourceAddress, val, sizeof((*ppCfg)->SourceAddress));
                entry_count |= 1;
            }
            else if (sscanf(buf, "duid %s", val))
            {
                int i = 0, j = 0;
                /*the file stores duid in this format 00:01:..., we need to transfer it to continuous hex*/
                
                for (i=0; i<strlen(val) && j < sizeof((*ppCfg)->DUID)-1; i++)
                {
                    if (val[i] != ':')
                        ((*ppCfg)->DUID)[j++] = val[i];
                }

                entry_count |= 2;
            }

            /*only we have both addr and  duid we think we have a whole server-info*/
            if (entry_count == 3)
                *pSize = 1;

            memset(buf, 0, sizeof(buf));
        }
        fclose(fp);
    }
        
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API initiates a DHCP client renewal. 
    Arguments:
        pAlias        The entry is identified through Alias.
*/
ANSC_STATUS
CosaDmlDhcpv6cRenew
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{  
    char cmd[256] = {0};
    
    sprintf(cmd, "killall -SIGUSR2 %s", CLIENT_BIN);
    system(cmd);

    return ANSC_STATUS_SUCCESS;
}

/*
 *  DHCP Client Send/Req Option
 *
 *  The options are managed on top of a DHCP client,
 *  which is identified through pClientAlias
 */
#define SYSCFG_DHCP6C_SENT_OPTION_FORMAT "tr_dhcp6c_sent_option_%d"
static int g_sent_option_num;
static COSA_DML_DHCPCV6_SENT * g_sent_options;
static int g_recv_option_num   = 0;
static COSA_DML_DHCPCV6_RECV * g_recv_options = NULL;

struct DHCP_TAG tagList[] =
{
    {17, "vendor-spec"},
    {21, "sip-domain"},
    {22, "sip-server"},
    {23, "dns-server"},
    {24, "domain"},
    {27, "nis-server"},
    {28, "nis+-server"},
    {29, "nis-domain"},
    {30, "nis+-domain"},
/*    {39, "OPTION_FQDN"},*/
    {31, "ntp-server"},
    {42, "time-zone"}
};

ULONG
CosaDmlDhcpv6cGetNumberOfSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber
    )
{
    UtopiaContext utctx = {0};
    char out[256] = {0};
    
    if (!Utopia_Init(&utctx))
        return 0;
    
    Utopia_RawGet(&utctx,NULL,"tr_dhcp6c_sent_option_num",out,sizeof(out));
    
    Utopia_Free(&utctx, 0);
    
    g_sent_option_num = atoi(out);
    
    if (g_sent_option_num)
        g_sent_options = (COSA_DML_DHCPCV6_SENT *)AnscAllocateMemory(sizeof(COSA_DML_DHCPCV6_SENT)* g_sent_option_num);

    return g_sent_option_num;

}

#define CLIENT_SENT_OPTIONS_FILE "/tmp/.dibbler-info/client_sent_options"
/*this function will generate sent_option info file to dibbler-client,
 the format of CLIENT_SENT_OPTIONS_FILE:
    option-type:option-len:option-data*/
static int _write_dibbler_sent_option_file(void)
{
    FILE * fp = fopen(CLIENT_SENT_OPTIONS_FILE, "w+");
    int  i = 0;
    
    if (fp)
    {
        for (i=0; i<g_sent_option_num; i++)
        {
            if (g_sent_options[i].bEnabled)
                fprintf(fp, "%d:%d:%s\n", 
                        g_sent_options[i].Tag,
                        strlen(g_sent_options[i].Value)/2,
                        g_sent_options[i].Value);
        }

        fclose(fp);
    }

    return 0;
}

ANSC_STATUS
CosaDmlDhcpv6cGetSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{
    UtopiaContext utctx = {0};
    char out[256] = {0};
    char namespace[256] = {0};

    if ( ulIndex > g_sent_option_num - 1  || !g_sent_options)
        return ANSC_STATUS_FAILURE;
    
    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    /*note in syscfg, sent_options start from 1*/
    sprintf(namespace, SYSCFG_DHCP6C_SENT_OPTION_FORMAT, ulIndex+1);
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx, namespace, "inst_num" ,out,sizeof(out));
    sscanf(out, "%d", &pEntry->InstanceNumber);

    Utopia_RawGet(&utctx, namespace, "alias" ,pEntry->Alias, sizeof(pEntry->Alias));

    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx, namespace, "enabled" ,out,sizeof(out));
    pEntry->bEnabled = (out[0] == '1') ? TRUE:FALSE;

    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx, namespace, "tag" ,out,sizeof(out));
    sscanf(out, "%d", &pEntry->Tag);

    Utopia_RawGet(&utctx, namespace, "value" ,pEntry->Value, sizeof(pEntry->Value));
    
    Utopia_Free(&utctx, 0);

    AnscCopyMemory( &g_sent_options[ulIndex], pEntry, sizeof(COSA_DML_DHCPCV6_SENT));

    /*we only wirte to file when obtaining the last entry*/
    if (ulIndex == g_sent_option_num-1)
        _write_dibbler_sent_option_file();


    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlDhcpv6cGetSentOptionbyInsNum
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{   
    ULONG                           index  = 0;

    for( index=0;  index<g_sent_option_num; index++)
    {
        if ( pEntry->InstanceNumber == g_sent_options[index].InstanceNumber )
        {
            AnscCopyMemory( pEntry, &g_sent_options[index], sizeof(COSA_DML_DHCPCV6_SENT));
             return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_FAILURE;
}


ANSC_STATUS
CosaDmlDhcpv6cSetSentOptionValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UtopiaContext utctx = {0};
    char out[256] = {0};
    char namespace[256] = {0};

    if (ulClientInstanceNumber != g_dhcpv6_client.Cfg.InstanceNumber)
        return ANSC_STATUS_FAILURE;

    if ( ulIndex+1 > g_sent_option_num )
        return ANSC_STATUS_SUCCESS;

    g_sent_options[ulIndex].InstanceNumber  = ulInstanceNumber;
    AnscCopyString( g_sent_options[ulIndex].Alias, pAlias);

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    sprintf(namespace, SYSCFG_DHCP6C_SENT_OPTION_FORMAT, ulIndex+1);
    snprintf(out, sizeof(out)-1, "%d", ulInstanceNumber);
    Utopia_RawSet(&utctx, namespace, "inst_num", out);

    Utopia_RawSet(&utctx, namespace, "alias" ,pAlias);

    Utopia_Free(&utctx, 1);    

    return ANSC_STATUS_SUCCESS;
}

static int _syscfg_add_sent_option(PCOSA_DML_DHCPCV6_SENT      pEntry, int index)
{
    UtopiaContext utctx = {0};
    char out[256] = {0};
    char namespace[256] = {0};

    if (!pEntry)
        return -1;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    sprintf(namespace, SYSCFG_DHCP6C_SENT_OPTION_FORMAT, index);
    snprintf(out, sizeof(out)-1, "%d", pEntry->InstanceNumber);
    Utopia_RawSet(&utctx, namespace, "inst_num", out );

    Utopia_RawSet(&utctx, namespace, "alias" ,pEntry->Alias);

    if (pEntry->bEnabled)
        sprintf(out, "1");
    else 
        sprintf(out, "0");
    Utopia_RawSet(&utctx, namespace, "enabled" ,out);

    snprintf(out, sizeof(out)-1, "%d", pEntry->Tag);
    Utopia_RawSet(&utctx, namespace, "tag" , out);

    Utopia_RawSet(&utctx, namespace, "value" ,pEntry->Value);
    
    Utopia_Free(&utctx, 1);        

    return 0;
}

ANSC_STATUS
CosaDmlDhcpv6cAddSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{
    UtopiaContext utctx = {0};
    char out[256] = {0};
    char namespace[256] = {0};

    /*we only have one client*/
    if (ulClientInstanceNumber != g_dhcpv6_client.Cfg.InstanceNumber)
        return ANSC_STATUS_FAILURE;

    g_sent_options = realloc(g_sent_options, (++g_sent_option_num)* sizeof(COSA_DML_DHCPCV6_SENT));
    if (!g_sent_options)
        return ANSC_STATUS_FAILURE;

    _syscfg_add_sent_option(pEntry, g_sent_option_num);    

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    snprintf(out, sizeof(out)-1, "%d", g_sent_option_num);
    Utopia_RawSet(&utctx, NULL, "tr_dhcp6c_sent_option_num" ,out);
    Utopia_Free(&utctx, 1);        

    g_sent_options[g_sent_option_num-1] = *pEntry;

    /*if the added entry is disabled, don't update dibbler-info file*/
    if (pEntry->bEnabled)
    {
        _write_dibbler_sent_option_file();
        _dibbler_client_operation("restart");
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cDelSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        ULONG                       ulInstanceNumber
    )
{
    UtopiaContext utctx = {0};
    char out[256] = {0};
    char namespace[256] = {0};
    int  i = 0;
    int  j = 0;
    int  saved_enable = 0;

    /*we only have one client*/
    if (ulClientInstanceNumber != g_dhcpv6_client.Cfg.InstanceNumber)
        return ANSC_STATUS_FAILURE;


    for (i=0; i<g_sent_option_num; i++)
    {
        if (g_sent_options[i].InstanceNumber == ulInstanceNumber)
            break;
    }
    
    if (i == g_sent_option_num)
        return ANSC_STATUS_CANT_FIND;

    saved_enable = g_sent_options[i].bEnabled;

    for (j=i; j<g_sent_option_num-1; j++)
    {
        g_sent_options[j] = g_sent_options[j+1];
        
        /*move syscfg ahead to fill in the deleted one*/
        _syscfg_add_sent_option(g_sent_options+j+1, j+1);
    }

    g_sent_option_num--;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    /*syscfg unset the last one, since we move the syscfg table ahead*/
    sprintf(namespace, SYSCFG_DHCP6C_SENT_OPTION_FORMAT, g_sent_option_num+1);
    syscfg_unset(namespace, "inst_num");
    syscfg_unset(namespace, "alias");
    syscfg_unset(namespace, "enabled");
    syscfg_unset(namespace, "tag");
    syscfg_unset(namespace, "value");

    snprintf(out, sizeof(out)-1, "%d", g_sent_option_num);
    Utopia_RawSet(&utctx, NULL, "tr_dhcp6c_sent_option_num" ,out);

    Utopia_Free(&utctx, 1);        

    /*only take effect when the deleted entry was enabled*/
    if (saved_enable)
    {
        _write_dibbler_sent_option_file();
        _dibbler_client_operation("restart");
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6cSetSentOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_SENT      pEntry
    )
{
    ULONG                           index  = 0;
    UtopiaContext                   utctx = {0};
    char                            out[256] = {0};
    char                            namespace[256] = {0};
    int                             i = 0;
    int                             j = 0;
    int                             need_restart_service = 0;
    PCOSA_DML_DHCPCV6_SENT          p_old_entry = NULL;

    for( index=0;  index<g_sent_option_num; index++)
    {
        if ( pEntry->InstanceNumber == g_sent_options[index].InstanceNumber )
        {
            p_old_entry = &g_sent_options[index];

            if (!Utopia_Init(&utctx))
                return ANSC_STATUS_FAILURE;

            /*handle syscfg*/
            sprintf(namespace, SYSCFG_DHCP6C_SENT_OPTION_FORMAT, index+1);
    
            if (!AnscEqualString(pEntry->Alias, p_old_entry->Alias, TRUE))
                Utopia_RawSet(&utctx, namespace, "alias" ,pEntry->Alias);                

            if (pEntry->bEnabled != p_old_entry->bEnabled)
            {
                if (pEntry->bEnabled)
                    sprintf(out, "1");
                else 
                    sprintf(out, "0");
                Utopia_RawSet(&utctx, namespace, "enabled" ,out);                

                need_restart_service = 1;
            }
                

            if (pEntry->Tag != p_old_entry->Tag)
            {
                snprintf(out, sizeof(out)-1, "%d", pEntry->Tag);
                Utopia_RawSet(&utctx, namespace, "tag" , out);

                need_restart_service = 1;
            }

            if (!AnscEqualString(pEntry->Value, p_old_entry->Value, TRUE))
            {
                Utopia_RawSet(&utctx, namespace, "value" ,pEntry->Value);

                need_restart_service = 1;
            }

            Utopia_Free(&utctx, 1);        

            AnscCopyMemory( &g_sent_options[index], pEntry, sizeof(COSA_DML_DHCPCV6_SENT));

            if (need_restart_service)
            {
                _write_dibbler_sent_option_file();
                _dibbler_client_operation("restart");
            }

            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

/* This is to get all 
 */
#define CLIENT_RCVED_OPTIONS_FILE "/tmp/.dibbler-info/client_received_options"
/*this file has the following format:
 ......
 line n : opt-len opt-type opt-data(HexBinary)
 ......
*/
ANSC_STATUS
CosaDmlDhcpv6cGetReceivedOptionCfg
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulClientInstanceNumber,
        PCOSA_DML_DHCPCV6_RECV     *ppEntry,
        PULONG                      pSize
    )
{   
    FILE *                          fp = fopen(CLIENT_RCVED_OPTIONS_FILE, "r+");
    SLIST_HEADER                    option_list;
    COSA_DML_DHCPCV6_RECV *         p_rcv = NULL;
    char                            buf[1024] = {0};
    PSINGLE_LINK_ENTRY              pSLinkEntry       = NULL;
    ULONG                           ulIndex = 0;

    AnscSListInitializeHeader( &option_list );
    
    if (fp)
    {
        while (fgets(buf, sizeof(buf)-1, fp))
        {
            int  opt_len = 0;
            char opt_data[1024] = {0};
            char * p = NULL;
            
            p_rcv = (COSA_DML_DHCPCV6_RECV * )AnscAllocateMemory(sizeof(*p_rcv));
            memset(p_rcv, 0, sizeof(*p_rcv));
            if (!p_rcv)
                break;

            if (sscanf(buf, "%d %d", &opt_len, &p_rcv->Tag) != 2)
            {
                AnscFreeMemory(p_rcv); /*RDKB-6780, CID-33399, free unused resource*/
                p_rcv = NULL;
                continue;
            }

            /*don't trust opt_len record in file, calculate by self*/
            if ((p = strrchr(buf, ' ')))
            {
                p++;
                if (opt_len*2 < (strlen(buf)- (p-buf)))
                    opt_len = (strlen(buf)- (p-buf))/2;
            }
            else 
            {
                AnscFreeMemory(p_rcv); /*RDKB-6780, CID-33399, free unused resource*/
                p_rcv = NULL;
                continue;
            }

            if (opt_len*2 >= sizeof(p_rcv->Value))
            {
                AnscFreeMemory(p_rcv); /*RDKB-6780, CID-33399, free unused resource*/
                p_rcv = NULL;
                continue;
            }

            /*finally we are safe, copy string into Value*/
            if (sscanf(buf, "%*d %*d %s", p_rcv->Value) != 1)
            {
                AnscFreeMemory(p_rcv); /*RDKB-6780, CID-33399, free unused resource*/
                p_rcv = NULL;
                continue;
            }
            /*we only support one server, hardcode it*/
            safe_strcpy(p_rcv->Server, "Device.DHCPv6.Client.1.Server.1", sizeof(p_rcv->Server));
            AnscSListPushEntryAtBack(&option_list, &p_rcv->Link);

            memset(buf, 0, sizeof(buf));
        }
        fclose(fp);
    }

    if (!AnscSListGetFirstEntry(&option_list))
    {
        *pSize = 0;
        return ANSC_STATUS_SUCCESS;
    }

    *ppEntry = (PCOSA_DML_DHCPCV6_RECV)AnscAllocateMemory( AnscSListQueryDepth(&option_list) *sizeof(COSA_DML_DHCPCV6_RECV) );
    
    pSLinkEntry = AnscSListGetFirstEntry(&option_list);

    for ( ulIndex = 0; ulIndex < option_list.Depth; ulIndex++ )
    {
        p_rcv = ACCESS_DHCPV6_RECV_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry = AnscSListGetNextEntry(pSLinkEntry);

        AnscCopyMemory( *ppEntry + ulIndex, p_rcv, sizeof(*p_rcv) );
        AnscFreeMemory(p_rcv);
    }

    *pSize = AnscSListQueryDepth(&option_list);

    /* we need keep this for reference in server*/
    if ( g_recv_options )
        AnscFreeMemory(g_recv_options);
    
    g_recv_option_num = *pSize;
    g_recv_options = (COSA_DML_DHCPCV6_RECV *)AnscAllocateMemory( sizeof(COSA_DML_DHCPCV6_RECV) * g_recv_option_num );
    AnscCopyMemory(g_recv_options, *ppEntry, sizeof(COSA_DML_DHCPCV6_RECV) * g_recv_option_num);

    return ANSC_STATUS_SUCCESS;
}

static int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger)
{
    int fd = 0;
    char str[32] = "restart";
    
    DHCPVS_DEBUG_PRINT
  #if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(DHCPV6_PREFIX_FIX) 
    commonSyseventSet("dhcpv6_server-restart", "");
  #else
  
    //not restart really.we only need trigger pthread to check whether there is pending action.
    if ( !OnlyTrigger ) {
        g_dhcpv6s_restart_count++;
    }

    fd= open(DHCPS6V_SERVER_RESTART_FIFO, O_RDWR);

    if (fd < 0) 
    {
        DHCPVS_DEBUG_PRINT
        fprintf(stderr, "open dhcpv6 server restart fifo when writing.\n");
        return 1;
    }
    write( fd, str, sizeof(str) );
    close(fd);

  #endif
    return 0;
}

/*SKYH4-3227 : variable to check if process is started already*/
#if defined (_HUB4_PRODUCT_REQ_)
    BOOL  g_dhcpv6_server_started = FALSE;
#endif

/*
 *  DHCP Server
 */
static int _dibbler_server_operation(char * arg)
{
    char cmd[256] = {0};
    char out[256] = {0};
    ULONG Index  = 0;
    int fd = 0;

    if (!strncmp(arg, "stop", 4))
    {
        /*stop the process only if it is started*/
        #if defined (_HUB4_PRODUCT_REQ_)
            if ( !g_dhcpv6_server_started )
                goto EXIT;
        #endif
        fd = open(DHCPV6S_SERVER_PID_FILE, O_RDONLY);
        if (fd >= 0) {
            fprintf(stderr, "%s -- %d stop\n", __FUNCTION__, __LINE__);
            sprintf(cmd, "%s stop >/dev/null", SERVER_BIN);
            system(cmd);
            close(fd);
        }else{
            //this should not happen.
            fprintf(stderr, "%s -- %d server is not running. \n", __FUNCTION__, __LINE__);
        }
    }
    else if (!strncmp(arg, "start", 5))
    {
        /* We need judge current config file is right or not.
                    * There is not interface enabled. Not start
                    * There is not valid pool. Not start. 
                */
        if ( !g_dhcpv6_server )
            goto EXIT;

        for ( Index = 0; Index < uDhcpv6ServerPoolNum; Index++ )
        {
            if ( sDhcpv6ServerPool[Index].Cfg.bEnabled ) 
                if ( ( !sDhcpv6ServerPool[Index].Info.IANAPrefixes[0] ) && ( !sDhcpv6ServerPool[Index].Info.IAPDPrefixes[0] ) )
                    break;
        }
        if ( Index < uDhcpv6ServerPoolNum )
            goto EXIT;
    
        if (g_dhcpv6_server_prefix_ready && g_lan_ready)
        {
            fprintf(stderr, "%s -- %d start %d\n", __FUNCTION__, __LINE__, g_dhcpv6_server);

            #if defined (_HUB4_PRODUCT_REQ_)
                g_dhcpv6_server_started = TRUE;
            #endif

            sprintf(cmd, "%s start", SERVER_BIN);
            system(cmd);
        }
    }
    else if (!strncmp(arg, "restart", 7))
    {
        _dibbler_server_operation("stop");
	_dibbler_server_operation("start");
    }

EXIT:

    return 0;
}
/*
log-level 8
log-mode short

#Client-class TelephoneClass1{
#  match-if ( (client.vendor-spec.en == 1234567) and (client.vendor-spec.data contain CISCO) )
#}

iface "eth0" {

     t1 1800-2000
     t2 2700-3000
     prefered-lifetime 3600
     valid-lifetime 7200
     rapid-commit yes

     preference 100

     class {
        pool fc00:1:0:0:2::/80

        reject-clients 0x00010001c792d8b700252e7d05b5,fe80::225:2eff:fe7d:5b5,fe80::200:39ff:fe4b:1abe
#    deny TelephoneClass1
     }

     class {
        pool fc00:1:0:0:3::1/80
        reject-clients 0x00010001c792d8b700252e7d05b5
     }

      pd-class {
         pd-pool fc00:1:0:0:4::/80
         pd-pool fc00:1:0:0:5::/80
         pd-length 96
     }

     option dns-server 2000::ff,2000::fe 
     option domain example.com
     option vendor-spec 5678-1-0x3031323334,1556-2-0x393837363534
# option ntp-server 2000::200,2000::201,2000::202
# option time-zone  CET
# option sip-server 2000::300,2000::302,2000::303,2000::304
# option sip-domain sip1.example.com,sip2.example.com
# option nis-server 2000::400,2000::401,2000::404,2000::405,2000::405
# option nis-domain nis.example.com
# option nis+-server 2000::501,2000::502
# option nis+-domain nisplus.example.com
}

*/

/*return : seperated ipv6 address 
    2018CAFE00000000020C29FFFE97FCCC ==> 
    2018:CAFE:0000:0000:020C:29FF:FE97:FCCC
*/
char * CosaDmlDhcpv6sGetAddressFromString(char * address){
    static char     ipv6Address[256] = {0};
    ULONG   i =0;
    ULONG   j =0;
    ULONG   k =0;

    while( address[i] && (i< sizeof(ipv6Address)) )
    {
        ipv6Address[j++] = address[i++];
        if ( i%4 == 0 ) 
            ipv6Address[j++] = ':';
    }
    /*remove last ":"*/
    if ( j == 0 )
        ipv6Address[j] = '\0';
    else
        ipv6Address[j-1] = '\0';
    
    CcspTraceWarning(("New ipv6 address is %s from %s .\n", ipv6Address, address));

    return ipv6Address;
}

char * CosaDmlDhcpv6sGetStringFromHex(char * hexString){
    static char     newString[256] = {0};
    char    buff[8] = {0};
    ULONG   i =0;
    ULONG   j =0;
    ULONG   k =0;

    memset(newString,0, sizeof(newString));
    while( hexString[i] && (i< sizeof(newString)-1) )
    {
        buff[k++]        = hexString[i++];
        if ( k%2 == 0 ) {
             char c =  (char)strtol(buff, (char **)NULL, 16);
             if( !iscntrl(c) )
                 newString[j++] = c;
             memset(buff, 0, sizeof(buff));
             k = 0;
        }
    }
    CcspTraceWarning(("New normal string is %s from %s .\n", newString, hexString));

    return newString;
}


/*now we have 2 threads to access __cosa_dhcpsv6_refresh_config(), one is the big thread to process datamodel, the other is dhcpv6c_dbg_thrd(void * in),
 add a lock*/
void __cosa_dhcpsv6_refresh_config();
void _cosa_dhcpsv6_refresh_config()
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    
    pthread_mutex_lock(&mutex);
    __cosa_dhcpsv6_refresh_config();
    pthread_mutex_unlock(&mutex);
}

/*
    This function will translate DNS from flat string to dns strings by comma.

    length: 32*N 
    2018CAFE00000000020C29FFFE97FCCC2222CAFE00000000020C29FFFE97FCCC
    will translate to
    2018:CAFE:0000:0000:020C:29FF:FE97:FCCC,2222:CAFE:0000:0000:020C:29FF:FE97:FCCC

    Fail when return 1
    Succeed when return 0
*/
int CosaDmlDHCPv6sGetDNS(char* Dns, char* output, int outputLen)
{
    char oneDns[64]  = {0};
    int  len         = _ansc_strlen(Dns);
    int  count       = len/32;
    int  i           = 0;
    char *pStr       = NULL;

    if ( outputLen < (count*(32+8)) )
    {
        count = outputLen/(32+8);
        if (!count)
            return 1;
    }

    output[0] = '\0';
    while(i < count){
        _ansc_strncpy(oneDns, &Dns[i*32], 32);                       
        pStr = CosaDmlDhcpv6sGetAddressFromString(oneDns);
        _ansc_strcat(output, pStr);
        _ansc_strcat(output, ",");
        i++;
    }

    fprintf(stderr, "%s -- !!!!!!! %d %d %s %s\n", __FUNCTION__, outputLen, count, Dns, output);

    output[_ansc_strlen(output)-1] = '\0';

    return 0;
}
#if defined(_XB6_PRODUCT_REQ_) && defined(_COSA_BCM_ARM_)
static int format_dibbler_option(char *option)
{
    if (option == NULL)
        return -1;

    int i;

    for (i = 0; i < strlen(option); i++) {
        if(option[i] == ' ')
            option[i] = ',';
    }

    return 0;
}
#endif

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_COSA_BCM_MIPS_)
// adding new logics to handle pd-class
static int remove_single_quote (char *buf)
{
  int  mode;
  int i = 0;
  int j = 0;

  while (buf[i] != '\0' && i < 255) {
    if (buf[i] != '\'') {
      buf[j++] = buf[i];
    }
    i++;
  }
  buf[j] = '\0';
  return 0;
}


static int get_ipv6_tpmode (int *tpmod)
{
  char buf[16] = { 0 };
  char fixed_buf[16] = { 0 };
  int  mode;
  int i = 0;
  int j = 0;

  // sysevent_get(si6->sefd, si6->setok, "erouter_topology-mode", buf, sizeof(buf));
  commonSyseventGet(COSA_DML_DHCPV6C_PREF_IAID_SYSEVENT_NAME, buf, sizeof(buf));
  while (buf[i] != '\0') {
    if (buf[i] != '\'') {
        fixed_buf[j++] = buf[i];
      }
      i++;
  }

  mode = atoi(fixed_buf);

  switch(mode) {
  case 1:
    *tpmod = FAVOR_DEPTH;
    break;
  case 2:
    *tpmod = FAVOR_WIDTH;
    break;
  default:
    CcspTraceInfo(("%s: unknown erouter topology mode, buf: %s\n", __FUNCTION__, buf));
    *tpmod = TPMOD_UNKNOWN;
    break;
  }

  return 0;
}
static int get_prefix_info(const char *prefix,  char *value, unsigned int val_len, unsigned int *prefix_len)
{
    int i;

    i = strlen(prefix);

    while((prefix[i-1] != '/') && (i > 0)) i--;

    if (i == 0) {
        CcspTraceError(("[%s] ERROR, there is not '/' in prefix:%s\n", __FUNCTION__, prefix));

        return -1;
    }

    if (prefix_len != NULL)
        *prefix_len = atoi(&prefix[i]);

    if (value != NULL) {
        memset(value, 0, val_len);
        strncpy(value, prefix, i-1);
    }

    //fprintf(stderr, "[%s] prefix:%s length: %d.\n",__FUNCTION__, value != NULL ? value : "null", *prefix_len);
    CcspTraceInfo(("[%s] output value: %s prefix_len: %d.\n", __FUNCTION__, value != NULL ? value : "null", *prefix_len));

    return 0;
}
/* get the interfaces which need to assign /64 interface-prefix
 * suppose we currently use syscfg "lan_pd_interfaces" to represent the interfaces need to prefix delegation
 */
static int get_active_lanif(unsigned int insts[], unsigned int *num)
{
    char active_insts[32] = {0};
    char lan_pd_if[128] = {0};
    char *p = NULL;
    int i = 0;
    char if_name[16] = {0};
    char buf[64] = {0};
    char *st = NULL;
    syscfg_get(NULL, "lan_pd_interfaces", lan_pd_if, sizeof(lan_pd_if));

    if (lan_pd_if[0] == '\0') {
        *num = 0;
        return *num;
    }

    commonSyseventGet("multinet-instances", active_insts, sizeof(active_insts));

    p = strtok_r(active_insts, " ", &st);

    while (p != NULL) {
        snprintf(buf, sizeof(buf), "multinet_%s-name", p);
        commonSyseventGet(buf, if_name, sizeof(if_name));

        if (strstr(lan_pd_if, if_name)) { /*active interface and need prefix delegation*/
            insts[i] = atoi(p);
            i++;
        }

        p = strtok_r(NULL, " ", &st);
    }

    *num = i;

    return *num;
}

/*
 * Break the prefix provisoned from wan to sub-prefixes based on favor width/depth and topology mode
 */
static int divide_ipv6_prefix()
{
    ipv6_prefix_t       mso_prefix;
    ipv6_prefix_t       *sub_prefixes = NULL;
    unsigned int        enabled_iface_num = 0;
    unsigned int        l2_insts[MAX_LAN_IF_NUM] = {0};
    unsigned char       prefix[sizeof(struct in6_addr)];
    unsigned char       buf[sizeof(struct in6_addr)];
    int                 delta_bits = 0;
    unsigned int        sub_prefix_num = 0;
    unsigned int        iface_prefix_num = 0;
    int                 i;
    ipv6_prefix_t       *p_prefix = NULL;
    int                 bit_boundary = 0;
    unsigned long long  sub_prefix, tmp_prefix; //64 bits
    char                iface_prefix[INET6_ADDRSTRLEN]; //for iface prefix str
    char                evt_name[64];
    char                evt_val[64];
    char                iface_name[64];
    int                 used_sub_prefix_num = 0;
    char                ipv6_prefix[64] = {0};
    int                 tpmod;

    commonSyseventSet("ipv6_prefix-divided", "");
    commonSyseventGet(COSA_DML_DHCPV6C_PREF_SYSEVENT_NAME, ipv6_prefix, sizeof(ipv6_prefix));

    if (get_prefix_info(ipv6_prefix, mso_prefix.value, sizeof(mso_prefix.value), &mso_prefix.len) != 0) {
      CcspTraceError(("[%s] ERROR return -1 (i.e. error due to get_prefix_info() return -1)\n", __FUNCTION__));
      return -1;
    }

    if ((delta_bits = 64 - mso_prefix.len) < 0) {
      CcspTraceError(("[%s] ERROR invalid prefix. mso_prefix.len: %d greater than 64, delta_bits: %d\n",
                      __FUNCTION__, mso_prefix.len, delta_bits));
      return -1;
    }

    if (inet_pton(AF_INET6, mso_prefix.value, prefix) <= 0) {
      CcspTraceError(("[%s] ERROR prefix inet_pton error!\n",  __FUNCTION__));
      return -1;
    }

    get_active_lanif(l2_insts, &enabled_iface_num);
    CcspTraceInfo(("[%s] got enabled_iface_num: %d, delta_bits: %d.\n",
                    __FUNCTION__, enabled_iface_num, delta_bits));

    if (enabled_iface_num == 0) {
      CcspTraceError(("[%s] ERROR no enabled lan interfaces. return -1\n", __FUNCTION__));
      return -1;
    }

    if (enabled_iface_num > (1 << delta_bits)) {
      CcspTraceError(("[%s] ERROR delta_bits: %d  is too small to address all of its interfaces.\n",
                      __FUNCTION__, delta_bits));
      return -1;
    }
    
        //printf("mso_prefix.value %s \n",mso_prefix.value);
    //  printf("mso_prefix.len %d \n",mso_prefix.len);
    //  printf("si6->tpmod %d \n",tpmod);
    get_ipv6_tpmode (&tpmod);
    CcspTraceInfo(("[%s] got mso_prefix.value: %s, mso_prefix.len: %d, tpmod: %d\n",
                   __FUNCTION__, mso_prefix.value, mso_prefix.len, tpmod));

    /* divide base on mso prefix len and topology mode
     *  1) prefix len > 56 && topology mode = "favor depth", divide on 2 bit boundaries to 4 sub-prefixes.
     *  2) prefix len > 56 && topology mode = "favor width", divide on 3 bit boundaries to 8 sub-prefixes.
     *  3) prefix len <= 56 && topology mode = "favor depth", divide on 3 bit boundaries to 8 sub-prefixes.
     *  4) prefix len <= 56 && topology mode = "favor width", divide on 4 bit boundaries to 16 sub-prefixes.
     *  5) if prefix is to small to divide in the manner described, divided into as many /64 sub-prefixes as possible and log a message.
     * */
    /*get boundary*/
    if (mso_prefix.len > 56) {
        if (tpmod == FAVOR_DEPTH) {
            bit_boundary = (delta_bits < 2) ? delta_bits : 2;
        } else if (tpmod == FAVOR_WIDTH) {
            bit_boundary = (delta_bits < 3) ? delta_bits : 3;
        }
    }
    else {
        if (tpmod == FAVOR_DEPTH) {
            bit_boundary = (delta_bits < 3) ? delta_bits : 3;
        } else if(tpmod == FAVOR_WIDTH) {
            bit_boundary = (delta_bits < 4) ? delta_bits : 4;
        }
    }

    /*divide to sub-prefixes*/
    sub_prefix_num = 1 << bit_boundary;
    CcspTraceInfo(("[%s] set sub_prefix_num: %d by left shifting 1 with calculated bit_boundary: %d\n",
                   __FUNCTION__, sub_prefix_num, bit_boundary));

    sub_prefixes = (ipv6_prefix_t *)calloc(sub_prefix_num, sizeof(ipv6_prefix_t));
    if (sub_prefixes == NULL) {
      CcspTraceError(("[%s] ERROR calloc mem for sub-prefixes failed.\n", __FUNCTION__));
      return -1;
    }

    p_prefix = sub_prefixes;

    memcpy((void *)&tmp_prefix, (void *)prefix, 8); // the first 64 bits of mso prefix value
#ifdef _CBR_PRODUCT_REQ_
        tmp_prefix = helper_ntoh64(&tmp_prefix); // The memcpy is copying in reverse order due to LEndianess
#endif
    tmp_prefix &= ((~0) << delta_bits);
    for (i = 0; i < sub_prefix_num; i ++) {
        sub_prefix = tmp_prefix | (i << (delta_bits - bit_boundary));
        memset(buf, 0, sizeof(buf));
#ifdef _CBR_PRODUCT_REQ_        
                sub_prefix = helper_hton64(&sub_prefix);// The memcpy is copying in reverse order due to LEndianess
#endif
     memcpy((void *)buf, (void *)&sub_prefix, 8);
        inet_ntop(AF_INET6, buf, p_prefix->value, INET6_ADDRSTRLEN);
        p_prefix->len = mso_prefix.len + bit_boundary;
        //p_prefix->b_used = 0;

        fprintf(stderr, "sub-prefix:%s/%d\n", p_prefix->value, p_prefix->len);

        p_prefix++;
    }

    /*break the first sub-prefix to interface prefix for lan interface*/
    iface_prefix_num = (1 << delta_bits) / (sub_prefix_num); /*determine the iface prefix num for each sub-prefix*/

    p_prefix = sub_prefixes;
    inet_pton(AF_INET6, p_prefix->value, prefix);
    memcpy((void *)&tmp_prefix, (void *)prefix, 8); //the first 64 bits of the first sub-prefix
#ifdef _CBR_PRODUCT_REQ_
        tmp_prefix = helper_ntoh64(&tmp_prefix); // The memcpy is copying in reverse order due to LEndianess
#endif
    for (i = 0; i < enabled_iface_num; i++) {
        //p_prefix->b_used = 1;
        memset(buf, 0, sizeof(buf));
#ifdef _CBR_PRODUCT_REQ_
                tmp_prefix = helper_hton64(&tmp_prefix);// The memcpy is copying in reverse order due to LEndianess
#endif
        memcpy((void *)buf, (void *)&tmp_prefix, 8);
        inet_ntop(AF_INET6, buf, iface_prefix, INET6_ADDRSTRLEN);
        strcat(iface_prefix, "/64");

        /*set related sysevent*/
        snprintf(evt_name, sizeof(evt_name), "multinet_%d-name", l2_insts[i]);
        commonSyseventGet(evt_name, iface_name, sizeof(iface_name));
        snprintf(evt_name, sizeof(evt_name), "ipv6_%s-prefix", iface_name);
        commonSyseventSet(evt_name, iface_prefix);

        CcspTraceInfo(("[%s] interface-prefix %s:%s\n", __FUNCTION__, iface_name, iface_prefix));

        tmp_prefix++;
    }

    /*last set sub-prefix related sysevent*/
    used_sub_prefix_num = enabled_iface_num / iface_prefix_num;

    if ((enabled_iface_num % iface_prefix_num) != 0 ) {
        used_sub_prefix_num += 1;
        CcspTraceInfo(("[%s] added one to used_sub_prefix_num: %d\n",
                       __FUNCTION__, used_sub_prefix_num ));
    }
    if (used_sub_prefix_num < sub_prefix_num) {
        commonSyseventSet("ipv6_subprefix-start", sub_prefixes[used_sub_prefix_num].value);
        commonSyseventSet("ipv6_subprefix-end", sub_prefixes[sub_prefix_num-1].value);
    } else {
        commonSyseventSet("ipv6_subprefix-start", "");
        commonSyseventSet("ipv6_subprefix-end", "");
    }

    CcspTraceInfo(("[%s] set ipv6_prefix-length to: %d from mso_prefix.len\n", __FUNCTION__, mso_prefix.len));
    CcspTraceInfo(("[%s] set ipv6_pd-length to: %d mso_prefix.len: %d + bit_boundary: %d\n",
                   __FUNCTION__, mso_prefix.len, mso_prefix.len, bit_boundary));

    snprintf(evt_val, sizeof(evt_val), "%d", mso_prefix.len);
    commonSyseventSet("ipv6_prefix-length", evt_val);
    snprintf(evt_val, sizeof(evt_val), "%d", mso_prefix.len + bit_boundary);
    commonSyseventSet("ipv6_pd-length", evt_val);

    commonSyseventSet("ipv6_prefix-divided", "ready");

    if (sub_prefixes != NULL)
        free(sub_prefixes);

    return 0;
}

static int get_pd_pool(pd_pool_t *pool)
{
    char evt_val[256] = {0};

    commonSyseventGet("ipv6_subprefix-start", evt_val, sizeof(evt_val));
    CcspTraceInfo(("[%s] commonSyseventGet ipv6_subprefix-start: %s\n", __FUNCTION__, evt_val));
    if (evt_val[0] != '\0') {
      remove_single_quote(evt_val);
      strcpy(pool->start, evt_val);
      CcspTraceInfo(("[%s] pool->start: %d\n", __FUNCTION__, pool->start));
    }
    else  {
      CcspTraceWarning(("[%s] sysevent_get for ipv6_subprefix-start is NULL\n",  __FUNCTION__));
      return -1;
    }

    commonSyseventGet("ipv6_subprefix-end", evt_val, sizeof(evt_val));
    CcspTraceInfo(("[%s] commonSyseventGet ipv6_subprefix-end: %s\n", __FUNCTION__, evt_val));
    if (evt_val[0] != '\0') {
      remove_single_quote(evt_val);
      strcpy(pool->end, evt_val);
      CcspTraceInfo(("[%s] pool->end: %d\n", __FUNCTION__, pool->end));
    }
    else {
      CcspTraceWarning(("[%s] sysevent_get for ipv6_subprefix-end is NULL\n",  __FUNCTION__));
      return -1;
    }

    commonSyseventGet("ipv6_prefix-length", evt_val, sizeof(evt_val));
    CcspTraceInfo(("[%s] commonSyseventGet ipv6_prefix-length: %s\n", __FUNCTION__, evt_val));
    if (evt_val[0] != '\0') {
      remove_single_quote(evt_val);
      pool->prefix_length = atoi(evt_val);
      CcspTraceInfo(("[%s] pool->prefix_length : %d\n", __FUNCTION__, pool->prefix_length));
    }
    else {
      CcspTraceWarning(("[%s] sysevent_get for ipv6_prefix-length is NULL\n",  __FUNCTION__));
      return -1;
    }

    commonSyseventGet("ipv6_pd-length", evt_val, sizeof(evt_val));
    CcspTraceInfo(("[%s] commonSyseventGet ipv6_pd-length: %s\n", __FUNCTION__, evt_val));
    if (evt_val[0] != '\0') {
      remove_single_quote(evt_val);
      pool->pd_length = atoi(evt_val);
      CcspTraceInfo(("[%s] pool->pd_length: %d\n", __FUNCTION__, pool->pd_length));

    }
    else {
      CcspTraceWarning(("[%s] sysevent_get for ipv6_pd-length is NULL\n",  __FUNCTION__));
      return -1;
    }

    return 0;
}

static int get_iapd_info(ia_pd_t *iapd)
{
    char evt_val[256] = {0};
    char *st = NULL;
    if(iapd == NULL)
      return -1;

    commonSyseventGet(COSA_DML_DHCPV6C_PREF_T1_SYSEVENT_NAME, evt_val, sizeof(evt_val));
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
        strcpy(iapd->t1,"0");
      else
        strcpy(iapd->t1,strtok_r (evt_val,"'", &st));
    }
    CcspTraceInfo(("[%s] iapd->t1: %s\n", __FUNCTION__, iapd->t1));

    commonSyseventGet(COSA_DML_DHCPV6C_PREF_T2_SYSEVENT_NAME, evt_val, sizeof(evt_val));
    st = NULL;
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
        strcpy(iapd->t2,"0");
      else
        strcpy(iapd->t2,strtok_r (evt_val,"'", &st));
    }
    CcspTraceInfo(("[%s] iapd->t2: %s\n", __FUNCTION__, iapd->t2));

    commonSyseventGet(COSA_DML_DHCPV6C_PREF_PRETM_SYSEVENT_NAME, evt_val, sizeof(evt_val));
    st = NULL;
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
        strcpy(iapd->pretm,"0");
      else
        strcpy(iapd->pretm,strtok_r (evt_val,"'", &st));
    }
    CcspTraceInfo(("[%s] iapd->pretm: %s\n", __FUNCTION__, iapd->pretm));

    st = NULL;
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
        strcpy(iapd->vldtm,"0");
      else
        strcpy(iapd->vldtm,strtok_r (evt_val,"'", &st));
    }
    CcspTraceInfo(("[%s] iapd->vldtm: %s\n", __FUNCTION__, iapd->vldtm));

    return 0;
}

#endif

#ifdef _COSA_INTEL_USG_ARM_
void __cosa_dhcpsv6_refresh_config()
{
#if defined _COSA_INTEL_USG_ARM_ || _COSA_BCM_MIPS_
    FILE * fp = fopen(SERVER_CONF_LOCATION, "w+");
#else
    FILE * fp = fopen(TMP_SERVER_CONF, "w+");
#endif
    char   line[256] = {0};
    PUCHAR pTmp1 = NULL;
    PUCHAR pTmp2 = NULL;
    PUCHAR pTmp3 = NULL;
    UCHAR accessLimit[1024] = {0};
    UCHAR prefixValue[64] = {0};
    UCHAR prefixFullName[64] = {0};
    UCHAR SourceAddress[40] = {0};
    UCHAR SourceAddressMask[40] = {0};
    ULONG Index  = 0;
    ULONG Index2 = 0;
    ULONG Index3 = 0;
    ULONG Index4 = 0;
    ULONG uSize = 0;
    ULONG val1  = 0;
    UCHAR pVal1[16]  = {0};
    ULONG preferedTime = 3600;
    ULONG validTime = 7200;    
    int   returnValue = 0;
    UCHAR ipv6Addr[16] = {0};
    UCHAR ipv6Mask[16] = {0};
    UCHAR ipv6AddrStr[INET6_ADDRSTRLEN] = {0};
    ULONG ret = 0;
    int   ret2 = 0;
    BOOL  bFound = FALSE;
    BOOL  isInCaptivePortal = FALSE;
    char * saveptr = NULL;
    UCHAR IAPDPrefixes[320] = {0};
    ULONG IAPDPrefixes_Len = 0;
    char  ServerOption[256] = {0};
    char *pServerOption = NULL;
    FILE *responsefd=NULL;
    char *networkResponse = "/var/tmp/networkresponse.txt";
    int iresCode = 0;
    char responseCode[10];
    int inRfCaptivePortal=0,inWifiCp=0;
    char buf[20]={0};
    char rfCpEnable[6] = {0};
    char rfCpMode[6] = {0};
    ULONG  T1 = 0;
    ULONG  T2 = 0;

    if (!fp)
        goto EXIT;

    /*Begin write configuration */
    fprintf(fp, "log-level 4\n");
	
#if defined (INTEL_PUMA7)
    //Intel Proposed RDKB Generic Bug Fix from XB6 SDK
    fprintf(fp, "reconfigure-enabled 1\n");
#endif

    //Intel Proposed RDKB Generic Bug Fix from XB6 SDK
    fprintf(fp, "reconfigure-enabled 1\n");

    if ( g_dhcpv6_server_type != DHCPV6_SERVER_TYPE_STATEFUL )
        fprintf(fp, "stateless\n");

    for ( Index = 0; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        /* We need get interface name according to Interface field*/
        if ( !sDhcpv6ServerPool[Index].Cfg.bEnabled )
            continue;

        fprintf(fp, "iface %s {\n", COSA_DML_DHCPV6_SERVER_IFNAME);

        if (g_dhcpv6_server_type != DHCPV6_SERVER_TYPE_STATEFUL ) 
            goto OPTIONS;

        if ( sDhcpv6ServerPool[Index].Cfg.RapidEnable ){
            fprintf(fp, "   rapid-commit yes\n");
        }

#ifdef CONFIG_CISCO_DHCP6S_REQUIREMENT_FROM_DPC3825
        if ( sDhcpv6ServerPool[Index].Cfg.UnicastEnable ){
            char globalAddress[64] = {0};
            commonSyseventGet("lan_ipaddr_v6", globalAddress, sizeof(globalAddress));
            if ( strlen(globalAddress) > 0 )
                fprintf(fp, "   unicast %s\n", globalAddress);
        }

        fprintf(fp, "   iface-max-lease %d\n", sDhcpv6ServerPool[Index].Cfg.IANAAmount );
#endif

        /* on GUI, we will limit the order to be [1-256]*/
        fprintf(fp, "   preference %d\n", 255); /*256-(sDhcpv6ServerPool[Index].Cfg.Order%257));*/

        /*begin class
                    fc00:1:0:0:4::/80,fc00:1:0:0:5::/80,fc00:1:0:0:6::/80
                */
        if ( sDhcpv6ServerPool[Index].Cfg.IANAEnable && sDhcpv6ServerPool[Index].Info.IANAPrefixes[0] )
        {
            pTmp1 = AnscCloneString(sDhcpv6ServerPool[Index].Info.IANAPrefixes);              
            pTmp3 = pTmp1;
            pTmp2 = pTmp1;

            for (pTmp2; ; pTmp2 = NULL) 
            {
                pTmp1 = strtok_r(pTmp2, ",", &saveptr);
                if (pTmp1 == NULL)
                    break;

                /* This pTmp1 is IP.Interface.{i}.IPv6Prefix.{i}., we need ipv6 address(eg:2000:1:0:0:6::/64) according to it*/
                _ansc_sprintf(prefixFullName, "%sPrefix",pTmp1);
                uSize = sizeof(prefixValue);
                returnValue = g_GetParamValueString(g_pDslhDmlAgent, prefixFullName, prefixValue, &uSize);
                if ( returnValue != 0 )
                {
                    CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- g_GetParamValueString for iana:%d\n", returnValue));
                }

                fprintf(fp, "   class {\n");

#ifdef CONFIG_CISCO_DHCP6S_REQUIREMENT_FROM_DPC3825
                /*When enable EUI64, the pool prefix value must use xxx/64 format*/
                if ( sDhcpv6ServerPool[Index].Cfg.EUI64Enable){
                    fprintf(fp, "       share 1000\n");
                }

                /*prefix value should be: xxxx/64 */
                fprintf(fp, "       pool %s\n", prefixValue );
#else
                /*prefix value May be: 2001:01:02::/64 or 2001:0001:0001:001::/64 
                                    ::/ is necessary.
                                    We need translate them to valid value. 
                                 */
                {
                    int i = _ansc_strlen(prefixValue);
                    int j = 0;
                    while( (prefixValue[i-1] != '/') && ( i > 0 ) )
                        i--;

                    if ( i == 0 ){
                        CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- iana:%s is error\n", prefixValue));
                    }

                    if ( ( prefixValue[i-2] != ':' ) || ( prefixValue[i-3] != ':' ) ){
                        CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- iana:%s is error\n", prefixValue));
                    }

                    /* We just delete last '/' here */
                    prefixValue[i-1] = '\0';

                    /* skip '::/'  */
                    i = i-3;
                    while(  i > 0 ) {
                        if ( prefixValue[i-1] == ':' )
                            j++;
                        i--;
                    }

                    /* delete one last ':' becaues there are 4 parts in this prefix*/
                    if ( j == 3 )
                        prefixValue[_ansc_strlen(prefixValue)-1] = '\0';

                }

                fprintf(fp, "       pool %s%s - %s%s\n", prefixValue, sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin, prefixValue, sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd );
#endif

                /*we need get two time values */
                {
                    if (sDhcpv6ServerPool[Index].Cfg.LeaseTime <= -1 ) {
                        T1 = T2 = preferedTime = validTime = 0xFFFFFFFF;
                    }else{
                        T1           = (sDhcpv6ServerPool[Index].Cfg.LeaseTime)/2;
                        T2           = (ULONG)((sDhcpv6ServerPool[Index].Cfg.LeaseTime)*80.0/100);
                        preferedTime = (sDhcpv6ServerPool[Index].Cfg.LeaseTime);
                        validTime    = (sDhcpv6ServerPool[Index].Cfg.LeaseTime);
                    }

                    fprintf(fp, "       T1 %u\n", T1);
                    fprintf(fp, "       T2 %u\n", T2);
                    fprintf(fp, "       prefered-lifetime %u\n", preferedTime);
                    fprintf(fp, "       valid-lifetime %u\n", validTime);
                }
                
                fprintf(fp, "   }\n");
            }

            AnscFreeMemory(pTmp3);
        }

OPTIONS:
        /* For options */
        for ( Index2 = 0 ; Index2 < uDhcpv6ServerPoolOptionNum[Index]; Index2++ )
        {
            if ( !sDhcpv6ServerPoolOption[Index][Index2].bEnabled )
                continue;
            
            for ( Index3 = 0; Index3 < sizeof(tagList)/sizeof(struct DHCP_TAG);Index3++ )
            {
                if ( tagList[Index3].tag != sDhcpv6ServerPoolOption[Index][Index2].Tag )
                    continue;
                else
                    break;
            }

            if ( Index3 >= sizeof(tagList)/sizeof(struct DHCP_TAG) )
                continue;

            // During captive portal no need to pass DNS
            // Check the reponse code received from Web Service

            iresCode = 0;

            if((responsefd = fopen(networkResponse, "r")) != NULL)
            {
                if(fgets(responseCode, sizeof(responseCode), responsefd) != NULL)
                {
                    iresCode = atoi(responseCode);
                }

                /* RDKB-6780, CID-33149, free unused resources before return */
                //ARRISXB6-7321
                fclose(responsefd);
                responsefd = NULL;
            }

            // Get value of redirection_flag
            syscfg_get( NULL, "redirection_flag", buf, sizeof(buf));
            if( buf != NULL )
            {
                if ((strncmp(buf,"true",4) == 0) && iresCode == 204)
                {
                        inWifiCp = 1;
                        CcspTraceWarning((" _cosa_dhcpsv6_refresh_config -- Box is in captive portal mode \n"));
                }
                else
                {
                        //By default isInCaptivePortal is false
                        CcspTraceWarning((" _cosa_dhcpsv6_refresh_config -- Box is not in captive portal mode \n"));
                }
            }
#if defined (_XB6_PRODUCT_REQ_)
        syscfg_get(NULL, "enableRFCaptivePortal", rfCpEnable, sizeof(rfCpEnable));
        if(rfCpEnable != NULL)
        {
          if (strncmp(rfCpEnable,"true",4) == 0)
          {
              syscfg_get(NULL, "rf_captive_portal", rfCpMode,sizeof(rfCpMode));
              if(rfCpMode != NULL)
              {
                 if (strncmp(rfCpMode,"true",4) == 0)
                 {
                    inRfCaptivePortal = 1;
                    CcspTraceWarning((" _cosa_dhcpsv6_refresh_config -- Box is in RF captive portal mode \n"));
                 }
              }
          }
        }
        if((inWifiCp == 1) || (inRfCaptivePortal == 1))
        {
            isInCaptivePortal = TRUE;
        }
#else
        if(inWifiCp == 1)
           isInCaptivePortal = TRUE;
#endif

            if ( sDhcpv6ServerPoolOption[Index][Index2].PassthroughClient[0] )
            {
                /* this content get from v6 client directly. If there is problem, change to 
                                get from data model tree
                            */
#if defined(_XB6_PRODUCT_REQ_) && defined(_COSA_BCM_ARM_)

                static int sysevent_fd_gs;
                static token_t sysevent_token_gs;
                if ((sysevent_fd_gs = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT,
                    SE_VERSION, "SERVICE-IPV6", &sysevent_token_gs)) < 0) {
                fprintf(stderr, "%s: fail to open sysevent\n", __FUNCTION__);
                return -1;
                }
                char l_cSecWebUI_Enabled[8] = {0};
                syscfg_get(NULL, "SecureWebUI_Enable", l_cSecWebUI_Enabled, sizeof(l_cSecWebUI_Enabled));
                if (!strncmp(l_cSecWebUI_Enabled, "true", 4)) {
                  sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled = 1;
                }
                if ( sDhcpv6ServerPoolOption[Index][Index2].Tag == 23 )
                {
                    char dns_str[256] = {0};

                    CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- Tag is 23 \n"));
					/* Static DNS Servers */
					if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled )
					{
					   memset( dns_str, 0, sizeof( dns_str ) );	 
					   strcpy( dns_str, sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
					   CosaDmlDhcpv6s_format_DNSoption( dns_str );
					
					   // Check device is in captive portal mode or not
					   if( 1 == isInCaptivePortal )
					   {
						   fprintf(fp, "#	 option %s %s\n", tagList[Index3].cmdstring, dns_str);
					   }
					   else
					   {
						   fprintf(fp, "	option %s %s\n", tagList[Index3].cmdstring, dns_str);
					   }
					
					   CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__, 
																						 __LINE__,
																						 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
																						 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
					}
					else
					{
						sysevent_get(sysevent_fd_gs, sysevent_token_gs, "wan6_ns", dns_str, sizeof(dns_str));
						if (dns_str[0] != '\0') {
							format_dibbler_option(dns_str);
							if( isInCaptivePortal == TRUE )
							{
								fprintf(fp, "#	  option %s %s\n", tagList[Index3].cmdstring, dns_str);
							}
							else
							{
								//By default isInCaptivePortal is false
								fprintf(fp, "	 option %s %s\n", tagList[Index3].cmdstring, dns_str);
							}
						}
					}
                }
                else if (sDhcpv6ServerPoolOption[Index][Index2].Tag == 24)
                {//domain
                    char domain_str[256] = {0};
                        CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- Tag is 24 \n"));
                    sysevent_get(sysevent_fd_gs, sysevent_token_gs, "ipv6_dnssl", domain_str, sizeof(domain_str));
                    if (domain_str[0] != '\0') {
                        format_dibbler_option(domain_str);
                        if( isInCaptivePortal == TRUE )
                        {
                            fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, domain_str);
                        }
                        else
                        {
                            fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, domain_str);
                        }
                    }
                }
#elif defined _HUB4_PRODUCT_REQ_
                static int sysevent_fd_gs;
                static token_t sysevent_token_gs;
                if ((sysevent_fd_gs = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT,
                    SE_VERSION, "SERVICE-IPV6", &sysevent_token_gs)) < 0) {
                    fprintf(stderr, "%s: fail to open sysevent\n", __FUNCTION__);
                    return -1;
                }
				/* Static DNS Servers */
                if ( sDhcpv6ServerPoolOption[Index][Index2].Tag == 23 ) {
                    char dnsServer[ 256 ] = { 0 };
				    if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled ) {
                       //CcspTraceWarning(("Cfg.X_RDKCENTRAL_COM_DNSServersEnabled is 1 \n"));
					    memset( dnsServer, 0, sizeof( dnsServer ) );
				   	    strcpy( dnsServer, sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
					    CosaDmlDhcpv6s_format_DNSoption( dnsServer );

					    // Check device is in captive portal mode or not
					    if( 1 == isInCaptivePortal )
					    {
						  fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsServer);
					    }
					    else
					    {
						  fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, dnsServer);
					    }

				    }
                }
                else if (sDhcpv6ServerPoolOption[Index][Index2].Tag == 24) {
                    char domain_str[256] = {0};
                    sysevent_get(sysevent_fd_gs, sysevent_token_gs, "ipv6_domain_name", domain_str, sizeof(domain_str));
                    if (domain_str[0] != '\0') {
                        if( isInCaptivePortal == TRUE )
                        {
                            fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, domain_str);
                        }
                        else
                        {
                            fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, domain_str);
                        }
                    }
                }
                else if (sDhcpv6ServerPoolOption[Index][Index2].Tag == 17) {
                    char vendor_spec[512] = {0};
                    sysevent_get(sysevent_fd_gs, sysevent_token_gs, "vendor_spec", vendor_spec, sizeof(vendor_spec));
                    if (vendor_spec[0] != '\0') {
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, vendor_spec);
                    }
                    else
                    {
                        CcspTraceWarning(("vendor_spec sysevent failed to get, so not updating vendor_spec information. \n"));
                    }
                }
#else
                for ( Index4 = 0; Index4 < g_recv_option_num; Index4++ )
                {
                    if ( g_recv_options[Index4].Tag != sDhcpv6ServerPoolOption[Index][Index2].Tag  )
                        continue;
                    else
                        break;
                }

                if ( Index4 >= g_recv_option_num )
                    continue;

                /* We need to translate hex to normal string */
                if ( g_recv_options[Index4].Tag == 23 )
                { //dns
                   char dnsStr[ 256 ] = { 0 };
				   
				   /* Static DNS Servers */
				   if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled )
				   {
					  memset( dnsStr, 0, sizeof( dnsStr ) );  	
				   	  strcpy( dnsStr, sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
					  CosaDmlDhcpv6s_format_DNSoption( dnsStr );

					  // Check device is in captive portal mode or not
					  if( 1 == isInCaptivePortal )
					  {
						  fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
					  }
					  else
					  {
						  fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
					  }

					  CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__, 
					  																	__LINE__,
					  																	sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
					  																	sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
				   }
				   else
				   {
					   ret = CosaDmlDHCPv6sGetDNS(g_recv_options[Index4].Value, dnsStr, sizeof(dnsStr) );
					   
					   if ( !ret )
					   {
							// Check device is in captive portal mode or not
						   if ( 1 == isInCaptivePortal )
						   {
				   
							   fprintf(fp, "#	 option %s %s\n", tagList[Index3].cmdstring, dnsStr);
						   }
						   else
						   {
				   
							   fprintf(fp, "	option %s %s\n", tagList[Index3].cmdstring, dnsStr);
						   }
				   		}
					}
                }
                else if ( g_recv_options[Index4].Tag == 24 )
                { //domain
                    pServerOption =    CosaDmlDhcpv6sGetStringFromHex(g_recv_options[Index4].Value);                    
                    if ( pServerOption )
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                }else{
                    if ( pServerOption )
                        fprintf(fp, "   option %s 0x%s\n", tagList[Index3].cmdstring, pServerOption);
                }
#endif
            }
            else
            {
                /* We need to translate hex to normal string */
                if ( sDhcpv6ServerPoolOption[Index][Index2].Tag == 23 )
                { //dns
                    char dns_str[256] = {0};

					/* Static DNS Servers */
					if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled )
					{
					   memset( dns_str, 0, sizeof( dns_str ) );	 
					   strcpy( dns_str, sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
					   CosaDmlDhcpv6s_format_DNSoption( dns_str );
					
					   // Check device is in captive portal mode or not
					   if( 1 == isInCaptivePortal )
					   {
						   fprintf(fp, "#	 option %s %s\n", tagList[Index3].cmdstring, dns_str);
					   }
					   else
					   {
						   fprintf(fp, "	option %s %s\n", tagList[Index3].cmdstring, dns_str);
					   }
					
					   CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__, 
																						 __LINE__,
																						 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
																						 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
					}
					else
					{
						if ( _ansc_strstr(sDhcpv6ServerPoolOption[Index][Index2].Value, ":") )
							pServerOption = sDhcpv6ServerPoolOption[Index][Index2].Value;
						else
							pServerOption = CosaDmlDhcpv6sGetAddressFromString(sDhcpv6ServerPoolOption[Index][Index2].Value);
	
						if ( pServerOption ){
#if defined(_XB6_PRODUCT_REQ_) && defined(_COSA_BCM_ARM_)
					    if( isInCaptivePortal == TRUE ) {
									fprintf(fp, "#	  option %s %s\n", tagList[Index3].cmdstring, pServerOption);
							}
#else
							fprintf(fp, "	 option %s %s\n", tagList[Index3].cmdstring, pServerOption);
#endif
						}
					}
                }
                else if ( g_recv_options[Index4].Tag == 24 )
                { //domain
                    pServerOption = CosaDmlDhcpv6sGetStringFromHex(g_recv_options[Index4].Value);

                    if ( pServerOption ){
#if defined(_XB6_PRODUCT_REQ_) && defined(_COSA_BCM_ARM_)
			 if( isInCaptivePortal == TRUE ) {
                                fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                        }
#else
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
#endif
                    } 
                }else

                    if ( pServerOption ){
#if defined(_XB6_PRODUCT_REQ_) && defined(_COSA_BCM_ARM_)
			 if( isInCaptivePortal == TRUE ) {
                            fprintf(fp, "#    option %s 0x%s\n", tagList[Index3].cmdstring, pServerOption);
                        }
#else
                        fprintf(fp, "    option %s 0x%s\n", tagList[Index3].cmdstring, pServerOption);
#endif
                }
            }

        }     

        fprintf(fp, "}\n");
    }
    
    if(fp != NULL)
      fclose(fp);


#if (!defined _COSA_INTEL_USG_ARM_) && (!defined _COSA_BCM_MIPS_)
    /*we will copy the updated conf file at once*/
    if (rename(TMP_SERVER_CONF, SERVER_CONF_LOCATION))
        CcspTraceWarning(("%s rename failed %s\n", __FUNCTION__, strerror(errno)));
#endif

EXIT:

    return;
}
#endif

#ifdef _COSA_BCM_MIPS_
void __cosa_dhcpsv6_refresh_config()
{

    FILE * fp = fopen(SERVER_CONF_LOCATION, "w+");

    char   line[256] = {0};
    PUCHAR pTmp1 = NULL;
    PUCHAR pTmp2 = NULL;
    PUCHAR pTmp3 = NULL;
    UCHAR accessLimit[1024] = {0};
    UCHAR prefixValue[64] = {0};
    UCHAR prefixFullName[64] = {0};
    UCHAR SourceAddress[40] = {0};
    UCHAR SourceAddressMask[40] = {0};
    ULONG Index  = 0;
    ULONG Index2 = 0;
    ULONG Index3 = 0;
    ULONG Index4 = 0;
    ULONG uSize = 0;
    ULONG val1  = 0;
    UCHAR pVal1[16]  = {0};
    ULONG preferedTime = 3600;
    ULONG validTime = 7200;
    int   returnValue = 0;
    UCHAR ipv6Addr[16] = {0};
    UCHAR ipv6Mask[16] = {0};
    UCHAR ipv6AddrStr[INET6_ADDRSTRLEN] = {0};
    ULONG ret = 0;
    int   ret2 = 0;
    BOOL  bFound = FALSE;
    char * saveptr = NULL;
    UCHAR IAPDPrefixes[320] = {0};
    ULONG IAPDPrefixes_Len = 0;
    char  ServerOption[256] = {0};
    char *pServerOption = NULL;
    FILE *responsefd=NULL;
    char *networkResponse = "/var/tmp/networkresponse.txt";
    int iresCode = 0;
    char responseCode[10];
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    pd_pool_t           pd_pool;
    ia_pd_t             ia_pd;
#endif
    ULONG  T1 = 0;
    ULONG  T2 = 0;
        char buf[ 6 ];
        int IsCaptivePortalMode = 0;
    BOOL  bBadPrefixFormat = FALSE;

    if (!fp)
        goto EXIT;

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION)
    /* handle logic:
     *  1) divide the Operator-delegated prefix to sub-prefixes
     *  2) further break the first of these sub-prefixes into /64 interface-prefixes for lan interface
     *  3) assign IPv6 address from the corresponding interface-prefix for lan interfaces
     *  4) the remaining sub-prefixes are delegated via DHCPv6 to directly downstream routers
     *  5) Send RA, start DHCPv6 server
     */

    if (divide_ipv6_prefix() != 0) {
      // CcspTraceError(("[%s] ERROR divide the operator-delegated prefix to sub-prefix error.\n",  __FUNCTION__));
        fprintf(stderr, "divide the operator-delegated prefix to sub-prefix error.\n");
        // sysevent_set(si6->sefd, si6->setok, "service_ipv6-status", "error", 0);
        commonSyseventSet("service_ipv6-status", "error");
        return -1;
    }

#endif

    /*Begin write configuration */
    fprintf(fp, "log-level 8\n");

    //Intel Proposed RDKB Generic Bug Fix from XB6 SDK
    fprintf(fp, "reconfigure-enabled 1\n");

    if ( g_dhcpv6_server_type != DHCPV6_SERVER_TYPE_STATEFUL )
        fprintf(fp, "stateless\n");

    for ( Index = 0; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        /* We need get interface name according to Interface field*/
        if ( !sDhcpv6ServerPool[Index].Cfg.bEnabled )
            continue;

       // Skip emit any invalid iface entry to the dibbler server.conf file 
       if (sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin[0] == '\0'  &&
           sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd[0] == '\0')
         {
           CcspTraceInfo(("%s Skip Index: %d, iface: %s, entry. Invalid PrefixRangeBegin: %s,  PrefixRangeEnd: %s, LeaseTime: %d\n",
                          __func__, Index, COSA_DML_DHCPV6_SERVER_IFNAME, sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin,
                          sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd, sDhcpv6ServerPool[Index].Cfg.LeaseTime));
            continue;
         }

        fprintf(fp, "iface %s {\n", COSA_DML_DHCPV6_SERVER_IFNAME);

        if (g_dhcpv6_server_type != DHCPV6_SERVER_TYPE_STATEFUL )
            goto OPTIONS;

        if ( sDhcpv6ServerPool[Index].Cfg.RapidEnable ){
            fprintf(fp, "   rapid-commit yes\n");
        }

#ifdef CONFIG_CISCO_DHCP6S_REQUIREMENT_FROM_DPC3825
        if ( sDhcpv6ServerPool[Index].Cfg.UnicastEnable ){
            char globalAddress[64] = {0};
            commonSyseventGet("lan_ipaddr_v6", globalAddress, sizeof(globalAddress));
            if ( strlen(globalAddress) > 0 )
                fprintf(fp, "   unicast %s\n", globalAddress);
        }

        fprintf(fp, "   iface-max-lease %d\n", sDhcpv6ServerPool[Index].Cfg.IANAAmount );
#endif

        /* on GUI, we will limit the order to be [1-256]*/
        fprintf(fp, "   preference %d\n", 255); /*256-(sDhcpv6ServerPool[Index].Cfg.Order%257));*/

        /*begin class
                    fc00:1:0:0:4::/80,fc00:1:0:0:5::/80,fc00:1:0:0:6::/80
                */
        if ( sDhcpv6ServerPool[Index].Cfg.IANAEnable && sDhcpv6ServerPool[Index].Info.IANAPrefixes[0] )
        {
            pTmp1 = AnscCloneString(sDhcpv6ServerPool[Index].Info.IANAPrefixes);
            pTmp3 = pTmp1;
            pTmp2 = pTmp1;

            for (pTmp2; ; pTmp2 = NULL)
            {
                pTmp1 = strtok_r(pTmp2, ",", &saveptr);
                if (pTmp1 == NULL)
                    break;

                /* This pTmp1 is IP.Interface.{i}.IPv6Prefix.{i}., we need ipv6 address(eg:2000:1:0:0:6::/64) according to it*/
                _ansc_sprintf(prefixFullName, "%sPrefix",pTmp1);
                uSize = sizeof(prefixValue);
                returnValue = g_GetParamValueString(g_pDslhDmlAgent, prefixFullName, prefixValue, &uSize);
                if ( returnValue != 0 )
                {
                    CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- g_GetParamValueString for iana:%d\n", returnValue));
                }

                fprintf(fp, "   class {\n");

#ifdef CONFIG_CISCO_DHCP6S_REQUIREMENT_FROM_DPC3825
                /*When enable EUI64, the pool prefix value must use xxx/64 format*/
                if ( sDhcpv6ServerPool[Index].Cfg.EUI64Enable){
                    fprintf(fp, "       share 1000\n");
                }

                /*prefix value should be: xxxx/64 */
                fprintf(fp, "       pool %s\n", prefixValue );
#else
                /*prefix value May be: 2001:01:02::/64 or 2001:0001:0001:001::/64 
                                    ::/ is necessary.
                                    We need translate them to valid value. 
                                 */
                {
                    int i = _ansc_strlen(prefixValue);
                    int j = 0;
                    while( (prefixValue[i-1] != '/') && ( i > 0 ) )
                        i--;

                    if ( i == 0 ){
                        CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- iana:%s is error\n", prefixValue));
                       bBadPrefixFormat = TRUE;
                    }

                    if ( ( prefixValue[i-2] != ':' ) || ( prefixValue[i-3] != ':' ) ){
                        CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- iana:%s is error\n", prefixValue));
                       bBadPrefixFormat = TRUE;
                    }

#if 0
                    /* We just delete last '/' here */
                    prefixValue[i-1] = '\0';

                    /* skip '::/'  */
                    i = i-3;
                    while(  i > 0 ) {
                        if ( prefixValue[i-1] == ':' )
                            j++;
                        i--;
                    }

                    /* delete one last ':' becaues there are 4 parts in this prefix*/
                    if ( j == 3 )
                        prefixValue[_ansc_strlen(prefixValue)-1] = '\0';
#endif
                    if (bBadPrefixFormat == FALSE && i > 2)
                     {

                       /* Need to convert A....::/xx to W:X:Y:Z: where X, Y or Z could be zero */
                       /* We just delete last ':/' here */
                       int k;
                       for ( k = 0; k < i; k++ )
                       {
                           if ( prefixValue[k] == ':' )
                           {
                               j++;
                               if ( k && (k < (sizeof(prefixValue) - 6)) )
                               {
                                   if ( prefixValue[k+1] == ':' )
                                   {
                                       switch (j)
                                       {
                                           case 1:
                                               // A:: -> A:0:0:0:
                                               strcpy( &prefixValue[k+1], "0:0:0:" );
                                           break;

                                           case 2:
                                               // A:B:: -> A:B:0:0:
                                               strcpy( &prefixValue[k+1], "0:0:" );
                                           break;

                                           case 3:
                                               // A:B:C:: -> A:B:C:0:
                                               strcpy( &prefixValue[k+1], "0:" );
                                           break;

                                           case 4:
                                               // A:B:C:D:: -> A:B:C:D:
                                               prefixValue[k+1] = 0;
                                           break;
                                       }
                                       break;
                                   }
                               }
                           }
                       }

                       CcspTraceInfo(("%s Fixed prefixValue: %s\n", __func__, prefixValue));

                     }
                }

                fprintf(fp, "       pool %s%s - %s%s\n", prefixValue, sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin, prefixValue, sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd );
#endif

                /*we need get two time values */
                {
                    if (sDhcpv6ServerPool[Index].Cfg.LeaseTime <= -1 ) {
                        T1 = T2 = preferedTime = validTime = 0xFFFFFFFF;
                    }else{
                        T1           = (sDhcpv6ServerPool[Index].Cfg.LeaseTime)/2;
                        T2           = (ULONG)((sDhcpv6ServerPool[Index].Cfg.LeaseTime)*80.0/100);
                        preferedTime = (sDhcpv6ServerPool[Index].Cfg.LeaseTime);
                        validTime    = (sDhcpv6ServerPool[Index].Cfg.LeaseTime);
                    }

                    fprintf(fp, "       T1 %u\n", T1);
                    fprintf(fp, "       T2 %u\n", T2);
                    fprintf(fp, "       prefered-lifetime %u\n", preferedTime);
                    fprintf(fp, "       valid-lifetime %u\n", validTime);
                }

                fprintf(fp, "   }\n");
            }

            AnscFreeMemory(pTmp3);

#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
            CcspTraceInfo(("[%s]  %d - See if need to emit pd-class, sDhcpv6ServerPool[Index].Cfg.IAPDEnable: %d, Index: %d\n",
                           __FUNCTION__, __LINE__, sDhcpv6ServerPool[Index].Cfg.IAPDEnable, Index));

            if (sDhcpv6ServerPool[Index].Cfg.IAPDEnable) {
              /*pd pool*/

              if(get_pd_pool(&pd_pool) == 0) {

                CcspTraceInfo(("[%s]  %d emit pd-class { pd-pool pd_pool.start: %s, pd_pool.prefix_length: %d\n",
                               __FUNCTION__, __LINE__, pd_pool.start, pd_pool.prefix_length));
                CcspTraceInfo(("[%s]  %d emit            pd-length pd_pool.pd_length: %d\n",
                               __FUNCTION__, __LINE__, pd_pool.pd_length));

                fprintf(fp, "   pd-class {\n");
#if defined (_CBR_PRODUCT_REQ_) || defined (CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION)
                fprintf(fp, "       pd-pool %s /%d\n", pd_pool.start, pd_pool.prefix_length);
#else
                fprintf(fp, "       pd-pool %s - %s /%d\n", pd_pool.start, pd_pool.end, pd_pool.prefix_length);
#endif
                fprintf(fp, "       pd-length %d\n", pd_pool.pd_length);

                if(get_iapd_info(&ia_pd) == 0) {
                  fprintf(fp, "       T1 %s\n", ia_pd.t1);
                  fprintf(fp, "       T2 %s\n", ia_pd.t2);
                  fprintf(fp, "       prefered-lifetime %s\n", ia_pd.pretm);
                  fprintf(fp, "       valid-lifetime %s\n", ia_pd.vldtm);
                }

                fprintf(fp, "   }\n");
              }
            }
#endif

        }

OPTIONS:
        /* For options */
        for ( Index2 = 0 ; Index2 < uDhcpv6ServerPoolOptionNum[Index]; Index2++ )
        {
            if ( !sDhcpv6ServerPoolOption[Index][Index2].bEnabled )
                continue;

            for ( Index3 = 0; Index3 < sizeof(tagList)/sizeof(struct DHCP_TAG);Index3++ )
            {
                if ( tagList[Index3].tag != sDhcpv6ServerPoolOption[Index][Index2].Tag )
                    continue;
                else
                    break;
            }

            if ( Index3 >= sizeof(tagList)/sizeof(struct DHCP_TAG) )
                continue;

                        // During captive portal no need to pass DNS
                        // Check the reponse code received from Web Service

                        iresCode = 0;

                        if( ( responsefd = fopen( networkResponse, "r" ) ) != NULL )
                        {
                                if( fgets( responseCode, sizeof( responseCode ), responsefd ) != NULL )
                                {
                                        iresCode = atoi( responseCode );
                                }

                                /* RDKB-6780, CID-33149, free unused resources before return */
                                fclose(responsefd);
                                responsefd = NULL;
                        }

                        syscfg_get( NULL, "redirection_flag", buf, sizeof(buf));
                        if( buf != NULL )
                        {
                                if ( ( strncmp( buf,"true",4 ) == 0 ) && iresCode == 204 )
                                {
                                         IsCaptivePortalMode = 1;
                                }
                        }

            if ( sDhcpv6ServerPoolOption[Index][Index2].PassthroughClient[0] )
            {
                /* this content get from v6 client directly. If there is problem, change to 
                                get from data model tree
                            */
                for ( Index4 = 0; Index4 < g_recv_option_num; Index4++ )
                {
                    if ( g_recv_options[Index4].Tag != sDhcpv6ServerPoolOption[Index][Index2].Tag  )
                        continue;
                    else
                        break;
                }

                if ( Index4 >= g_recv_option_num )
                    continue;

                /* We need to translate hex to normal string */
                if ( g_recv_options[Index4].Tag == 23 )
                { //dns
                   char dnsStr[256] = {0};

                                   /* Static DNS Servers */
                                   if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled )
                                   {
                                          memset( dnsStr, 0, sizeof( dnsStr ) );
                                          strcpy( dnsStr, sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
                                          CosaDmlDhcpv6s_format_DNSoption( dnsStr );

                                          // Check device is in captive portal mode or not
                                          if( 1 == IsCaptivePortalMode )
                                          {
                                                  fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                          }
                                          else
                                          {
                                                  fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                          }

                                          CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__,
                                                                                                                                                                                __LINE__,
                                                                                                                                                                                sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
                                                                                                                                                                                sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
                                   }
                                   else
                                   {
                                           ret = CosaDmlDHCPv6sGetDNS(g_recv_options[Index4].Value, dnsStr, sizeof(dnsStr) );

                                                if ( !ret )
                                                {
                                                         // Check device is in captive portal mode or not
                                                        if ( 1 == IsCaptivePortalMode )
                                                        {

                                                                fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                                        }
                                                        else
                                                        {

                                                                fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                                        }
                                                }
                                   }
                }
                else if ( g_recv_options[Index4].Tag == 24 )
                { //domain
                    pServerOption =    CosaDmlDhcpv6sGetStringFromHex(g_recv_options[Index4].Value);
                    if ( pServerOption )
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                }else{
                    if ( pServerOption )
                        fprintf(fp, "   option %s 0x%s\n", tagList[Index3].cmdstring, pServerOption);
                }
            }
            else
            {
                /* We need to translate hex to normal string */
                if ( sDhcpv6ServerPoolOption[Index][Index2].Tag == 23 )
                { //dns
                                        char dnsStr[256] = {0};

                                        /* Static DNS Servers */
                                        if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled )
                                        {
                                           memset( dnsStr, 0, sizeof( dnsStr ) );
                                           strcpy( dnsStr, sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
                                           CosaDmlDhcpv6s_format_DNSoption( dnsStr );

                                           // Check device is in captive portal mode or not
                                           if( 1 == IsCaptivePortalMode )
                                           {
                                                   fprintf(fp, "#        option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                           }
                                           else
                                           {
                                                   fprintf(fp, "        option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                           }

                                           CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__,
                                                                                                                                                                                 __LINE__,
                                                                                                                                                                                 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
                                                                                                                                                                                 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
                                        }
                                        else
                                        {
                                                if ( _ansc_strstr(sDhcpv6ServerPoolOption[Index][Index2].Value, ":") )
                                                        pServerOption = sDhcpv6ServerPoolOption[Index][Index2].Value;
                                                else
                                                        pServerOption = CosaDmlDhcpv6sGetAddressFromString(sDhcpv6ServerPoolOption[Index][Index2].Value);

                                                if ( pServerOption )
                                                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                                        }

                }
                else if ( g_recv_options[Index4].Tag == 24 )
                { //domain
                    pServerOption = CosaDmlDhcpv6sGetStringFromHex(g_recv_options[Index4].Value);

                    if ( pServerOption )
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                }
                else
                {
                    if ( pServerOption )
                        fprintf(fp, "    option %s 0x%s\n", tagList[Index3].cmdstring, pServerOption);
                }
            }
        }
        fprintf(fp, "}\n");
    }
    if(fp != NULL)
      fclose(fp);



EXIT:

    return;
}

#endif

int CosaDmlDhcpv6s_format_DNSoption( char *option )
{
    if (option == NULL)
        return -1;

    int i;

    for (i = 0; i < strlen(option); i++) {
        if(option[i] == ' ')
            option[i] = ',';
    }

    return 0;
}

ANSC_STATUS
CosaDmlDhcpv6sEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bEnable
    )
{
    UtopiaContext utctx = {0};
    BOOL              bActualEnable = FALSE;
    
    if (g_dhcpv6_server == bEnable) {
        fprintf(stderr, "%s -- %d server is :%d. bEnable:%d\n", __FUNCTION__, __LINE__, g_dhcpv6_server, bEnable);
        return ANSC_STATUS_SUCCESS;
    }

    g_dhcpv6_server = bEnable;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "serverenable", g_dhcpv6_server)
    Utopia_Free(&utctx,1);

    #if defined(_BCI_FEATURE_REQ)
    system("sysevent set zebra-restart");
    #endif

    if ( bEnable )
    {
       #if defined(_BCI_FEATURE_REQ)
       CcspTraceInfo(("Enable DHCPv6. Starting Dibbler-Server and Zebra Process\n"));
       #endif    
        /* We need enable server */
        CosaDmlDHCPv6sTriggerRestart(FALSE);
    }
    else if ( !bEnable  )
    {
       #if defined(_BCI_FEATURE_REQ)
       CcspTraceInfo(("Disable DHCPv6. Stopping Dibbler-Server and Zebra Process\n "));
       #endif
      /* we need disable server. */
        
       #if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(DHCPV6_PREFIX_FIX) 
        commonSyseventSet("dhcpv6_server-stop", "");
       #else
        _dibbler_server_operation("stop");
       #endif
    }
    
    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API retrieves the current state of DHCP server: Enabled or Disabled. 
*/
BOOLEAN
CosaDmlDhcpv6sGetState
    (
        ANSC_HANDLE                 hContext
    )
{
    char cmd[256] = {0};
    char out[256] = {0};

    /*
    sprintf(cmd, "ps -A|grep %s", SERVER_BIN);
    _get_shell_output(cmd, out, sizeof(out));
    if (strstr(out, SERVER_BIN))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }*/

    return g_dhcpv6_server;
}

/*
    Description:
        The API retrieves the current type of DHCP server: STATELESS or STATEFUL. 
*/
ANSC_STATUS
CosaDmlDhcpv6sSetType
    (
        ANSC_HANDLE                 hContext,
        ULONG                        type        
    )
{
    BOOL                            bApply = FALSE;
    UtopiaContext utctx = {0};

    if ( type != g_dhcpv6_server_type )
        bApply = TRUE;
    
    g_dhcpv6_server_type = type;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    
    SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "servertype", g_dhcpv6_server_type)

    Utopia_Free(&utctx,1);

#ifdef _HUB4_PRODUCT_REQ_
    /* Stateful address is not effecting for clients whenever pool range is changed.
     * Added gw_lan_refresh to effect the stateful address to the clients according
     * to the configured poll range.
     */
    system("gw_lan_refresh");
#endif
    if ( g_dhcpv6_server && bApply )
    {
        /* We need enable server */
        CosaDmlDHCPv6sTriggerRestart(FALSE);
#ifdef _HUB4_PRODUCT_REQ_
        system("sysevent set zebra-restart");
#endif
    }

    return ANSC_STATUS_SUCCESS;
}

/*
    Description:
        The API retrieves the current state of DHCP server: Enabled or Disabled. 
*/
ULONG
CosaDmlDhcpv6sGetType
    (
        ANSC_HANDLE                 hContext
    )
{
    return g_dhcpv6_server_type;
}

/*
 *  DHCP Server Pool
 */
ULONG
CosaDmlDhcpv6sGetNumberOfPools
    (
        ANSC_HANDLE                 hContext
    )
{
    return uDhcpv6ServerPoolNum;
}

ANSC_STATUS
CosaDmlDhcpv6sGetPool
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPSV6_POOL_FULL pEntry
    )
{
    if ( ulIndex+1 > uDhcpv6ServerPoolNum )
        return ANSC_STATUS_FAILURE;

    AnscCopyMemory(pEntry, &sDhcpv6ServerPool[ulIndex], sizeof(COSA_DML_DHCPSV6_POOL_FULL));
    
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sSetPoolValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UtopiaContext utctx = {0};
    char out[256] = {0};    

    if ( ulIndex+1 > uDhcpv6ServerPoolNum )
        return ANSC_STATUS_FAILURE;

    sDhcpv6ServerPool[ulIndex].Cfg.InstanceNumber  = ulInstanceNumber;
    AnscCopyString(sDhcpv6ServerPool[ulIndex].Cfg.Alias, pAlias);

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    SETI_INTO_UTOPIA(DHCPV6S_NAME, "pool", ulIndex, "", 0, "instancenumber", ulInstanceNumber)
    SETS_INTO_UTOPIA(DHCPV6S_NAME, "pool", ulIndex, "", 0, "alias", pAlias)
    Utopia_Free(&utctx,1);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sAddPool
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPSV6_POOL_FULL   pEntry
    )
{
    UtopiaContext utctx = {0};
    ULONG         Index = 0;
#if defined (MULTILAN_FEATURE)
    UCHAR         param_val[CMD_BUFF_SIZE] = {0};
#endif

    if ( uDhcpv6ServerPoolNum >= DHCPV6S_POOL_NUM )
        return ANSC_STATUS_NOT_SUPPORTED;

    Index = uDhcpv6ServerPoolNum;
#if defined (MULTILAN_FEATURE)    
    if(!commonSyseventGet(COSA_DML_DHCPV6C_PREF_PRETM_SYSEVENT_NAME, param_val, sizeof(param_val)))
    {
        if(param_val[0] != '\0')
            pEntry->Cfg.LeaseTime = atoi(param_val);
    }

    if(pEntry->Cfg.Interface[0] != '\0')
    {
        snprintf(pEntry->Info.IANAPrefixes, sizeof(pEntry->Info.IANAPrefixes), "%s%s", pEntry->Cfg.Interface, "IPv6Prefix.1.");
    }
#endif
    sDhcpv6ServerPool[Index] = *pEntry;
    sDhcpv6ServerPool[Index].Info.Status = COSA_DML_DHCP_STATUS_Enabled; /* We set this to be enabled all time */
  
    /* Add this entry into utopia */
    setpool_into_utopia(DHCPV6S_NAME, "pool", Index, &sDhcpv6ServerPool[Index]);
    
    /* do this lastly */
    uDhcpv6ServerPoolNum++;    

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    SETI_INTO_UTOPIA(DHCPV6S_NAME, "", 0, "", 0, "poolnumber", uDhcpv6ServerPoolNum)
    Utopia_Free(&utctx,1);

    CosaDmlDHCPv6sTriggerRestart(FALSE);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sDelPool
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    UtopiaContext utctx  = {0};
    ULONG         Index  = 0;
    ULONG         Index2 = 0;

    /* RDKB-6780, CID-33220, Out-of-bounds read
    ** Global array defined is of size DHCPV6S_POOL_NUM
    ** Hence limiting upper boundary of index to avoid out of boud access.
    */
    for( Index = 0; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM); Index++)
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulInstanceNumber )
            break;
    }
#if defined (MULTILAN_FEATURE)
    for ( Index2 = Index+1; (Index2 < uDhcpv6ServerPoolNum) && (Index2 < DHCPV6S_POOL_NUM); Index2++ )
    {
        setpool_into_utopia(DHCPV6S_NAME, "pool", Index2-1, &sDhcpv6ServerPool[Index2]);
        sDhcpv6ServerPool[Index2-1] =  sDhcpv6ServerPool[Index2];
    }
#else
#if 0
    for ( Index2 = Index+1; (Index2 < uDhcpv6ServerPoolNum) && (Index2 < DHCPV6S_POOL_NUM); Index2++ )
    {
        setpool_into_utopia(DHCPV6S_NAME, "pool", Index2-1, &sDhcpv6ServerPool[Index2]);
        sDhcpv6ServerPool[Index2-1] =  sDhcpv6ServerPool[Index2];
    }
#endif
#endif

    /* unset last one in utopia  RDKB-6780, CID-33220, limiting upper bounday*/
    if (uDhcpv6ServerPoolNum == 0)
    {
        Index = DHCPV6S_POOL_NUM - 1;
    }
    else
    {
        Index =((uDhcpv6ServerPoolNum < DHCPV6S_POOL_NUM)? (uDhcpv6ServerPoolNum - 1):(DHCPV6S_POOL_NUM-1));
    }

    unsetpool_from_utopia(DHCPV6S_NAME, "pool", Index );
    AnscZeroMemory(&sDhcpv6ServerPool[Index], sizeof(sDhcpv6ServerPool[Index]));

    /* do this lastly */
    /* limiting lower boundary of the DHCP Server Pool Num to avoid -ve values*/
    uDhcpv6ServerPoolNum = (0 >(uDhcpv6ServerPoolNum-1)? 0 : (uDhcpv6ServerPoolNum-1) );


    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    SETI_INTO_UTOPIA(DHCPV6S_NAME, "", 0, "", 0, "poolnumber", uDhcpv6ServerPoolNum)
    Utopia_Free(&utctx,1);

    CosaDmlDHCPv6sTriggerRestart(FALSE);

    return ANSC_STATUS_SUCCESS;

}

static int
_get_iapd_prefix_pathname(char ** pp_pathname, int * p_len)
{
    char * p_ipif_path = NULL;
    int i = 0;
    int num = 0;
    char name[64] = {0};
    char path[256] = {0};
    char param_val[64] = {0};
    ULONG inst2 = 0;
    ULONG val_len = 0;

    if (!p_len || !pp_pathname)
        return -1;

    *p_len = 1; /*RDKB-6780, CID-33107, use after null check*/

    p_ipif_path = CosaUtilGetFullPathNameByKeyword
        (
            "Device.IP.Interface.",
            "Name",
            COSA_DML_DHCPV6_CLIENT_IFNAME
            );

    if (!p_ipif_path)
        return -2;

    sprintf(name, "%sIPv6PrefixNumberOfEntries", p_ipif_path);
            
    num = g_GetParamValueUlong(g_pDslhDmlAgent, name);

    for (i=0; i<num; i++)
    {
        sprintf(name, "%sIPv6Prefix.", p_ipif_path);
        inst2 = g_GetInstanceNumberByIndex(g_pDslhDmlAgent, name, i);

        if ( inst2 )
        {
            sprintf(name,  "%sIPv6Prefix.%d.Origin", p_ipif_path, inst2);

            val_len = sizeof(param_val);               
            if ( ( 0 == g_GetParamValueString(g_pDslhDmlAgent, name, param_val, &val_len)) &&
                 AnscEqualString(param_val, "PrefixDelegation", TRUE ) )
            {
                snprintf(path, sizeof(path)-1, "%sIPv6Prefix.%d.", p_ipif_path, inst2);
                *p_len += strlen(path)+1;
                
                if (!*pp_pathname)
                    *pp_pathname = calloc(1, *p_len);
                else
                    *pp_pathname = realloc(*pp_pathname, *p_len);
                
                if (!*pp_pathname)
                {
                    AnscFreeMemory(p_ipif_path); /*RDKB-6780, CID-33072, free unused resource before return*/
                    return -3;
                }
                
                sprintf(*pp_pathname+strlen(*pp_pathname), "%s,",path);

                break;
            }
        }
    }

    AnscFreeMemory(p_ipif_path);
    
    return 0;
}

int 
CosaDmlDhcpv6sGetIAPDPrefixes
    (
        PCOSA_DML_DHCPSV6_POOL_CFG  pCfg,
        char*                       pValue,
        ULONG*                      pSize
    )
{
    int size = 0;
    char * p = NULL;
    char * p1 = NULL;
    int need_append_comma = 0;
    char * p_pathname = NULL;
    int pathname_len = 0;
    int ret = 0;

    size = strlen(pCfg->IAPDManualPrefixes);

    p1 = pCfg->IAPDManualPrefixes;
    if ( strlen(p1) && (p1[strlen(p1)-1] != ',') )
    {
        need_append_comma = 1;
        size++;
    }
    
    _get_iapd_prefix_pathname(&p_pathname, &pathname_len);
    size += pathname_len; 

    if (size > *pSize)
    {
        *pSize = size+1;
        ret = 1;
    }
    else
    {
        /*pValue is big enough*/
        strcpy(pValue, pCfg->IAPDManualPrefixes);

        if (need_append_comma)
            sprintf(pValue+strlen(pValue), ",");

        if (p_pathname)
            sprintf(pValue+strlen(pValue), "%s", p_pathname);
        
        ret = 0;
    }

    if (p_pathname)
        free(p_pathname);

    return ret;
}
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
/*this function gets IAPD prefixes from sysevent, the value is PD prefix range*/
int 
CosaDmlDhcpv6sGetIAPDPrefixes2
    (
        PCOSA_DML_DHCPSV6_POOL_CFG  pCfg,
        char*                       pValue,
        ULONG*                      pSize
    )
{
    int size = 0;
    int ret = 0;
    char pd_pool[128] = {0};
    char evt_val[64] = {0};
    char start[64], end[64], pd_len[4];

    commonSyseventGet("ipv6_subprefix-start", start, sizeof(start));
    commonSyseventGet("ipv6_subprefix-end", end, sizeof(end));
    commonSyseventGet("ipv6_pd-length", pd_len, sizeof(pd_len));

    if(start[0] != '\0' && end[0] != '\0' && pd_len[0] != '\0')
        snprintf(pd_pool, sizeof(pd_pool), "%s-%s/%s", start, end, pd_len);
    
    size = strlen(pd_pool); 

    if (size > *pSize)
    {
        *pSize = size+1;
        ret = 1;
    }
    else
    {
        /*pValue is big enough*/
        strcpy(pValue, pd_pool);

        ret = 0;
    }

    return ret;
}
#endif
ANSC_STATUS
CosaDmlDhcpv6sSetPoolCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPSV6_POOL_CFG  pCfg
    )
{
    ULONG                           Index = 0;
	BOOLEAN  						bNeedZebraRestart = FALSE;

    /* RDKB-6780, CID-33383, Out-of-bounds write
    ** Maximum size of Dhcp Server Pool is DHCPV6S_POOL_NUM.
    */
    for( Index = 0; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM); Index++)
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == pCfg->InstanceNumber)
            break;
    }

    if(Index < DHCPV6S_POOL_NUM)
    {
		// Zebra restart based on below criteria
		if( ( pCfg->X_RDKCENTRAL_COM_DNSServersEnabled != sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled ) || \
			( ( '\0' != pCfg->X_RDKCENTRAL_COM_DNSServers[ 0 ] ) && \
			  ( 0 != strcmp( pCfg->X_RDKCENTRAL_COM_DNSServers, sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.X_RDKCENTRAL_COM_DNSServers ) )
			 )
		   )
		{
			bNeedZebraRestart = TRUE;
		}
#ifdef _HUB4_PRODUCT_REQ_
        /* Stateful address is not effecting for clients whenever pool range is changed.
         * Added gw_lan_refresh to effect the stateful address to the clients according
         * to the configured poll range.
         */
        if ( ( 0 != strcmp( pCfg->PrefixRangeBegin, sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin ) ) || \
             ( 0 != strcmp( pCfg->PrefixRangeEnd, sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd ) ) )
        {
            system("gw_lan_refresh");
        }
#endif

        sDhcpv6ServerPool[Index].Cfg = *pCfg;
    }
    else
    {
		// Zebra restart based on below criteria
		if( ( pCfg->X_RDKCENTRAL_COM_DNSServersEnabled != sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled ) || \
			( ( '\0' != pCfg->X_RDKCENTRAL_COM_DNSServers[ 0 ] ) && \
			  ( 0 != strcmp( pCfg->X_RDKCENTRAL_COM_DNSServers, sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.X_RDKCENTRAL_COM_DNSServers ) )
			 )
		   )
		{
			bNeedZebraRestart = TRUE;
		}
#ifdef _HUB4_PRODUCT_REQ_
        /* Stateful address is not effecting for clients whenever pool range is changed.
         * Added gw_lan_refresh to effect the stateful address to the clients according
         * to the configured poll range.
         */
        if ( ( 0 != strcmp( pCfg->PrefixRangeBegin, sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.PrefixRangeBegin ) ) || \
             ( 0 != strcmp( pCfg->PrefixRangeEnd, sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.PrefixRangeEnd ) ) )
        {
            system("gw_lan_refresh");
        }
#endif

        sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg = *pCfg;
        Index = DHCPV6S_POOL_NUM - 1;
    }    
#if defined (MULTILAN_FEATURE)
    if(pCfg->Interface[0] != '\0')
    {
        snprintf(sDhcpv6ServerPool[Index].Info.IANAPrefixes, sizeof(sDhcpv6ServerPool[Index].Info.IANAPrefixes), "%s%s", pCfg->Interface, "IPv6Prefix.1.");
    }
#endif

    /* We do this synchronization here*/
    if ( ( _ansc_strlen(sDhcpv6ServerPool[Index].Cfg.IANAManualPrefixes) > 0 ) && 
         !_ansc_strstr(sDhcpv6ServerPool[Index].Info.IANAPrefixes, sDhcpv6ServerPool[Index].Cfg.IANAManualPrefixes) ){
        _ansc_strcat(sDhcpv6ServerPool[Index].Info.IANAPrefixes, "," );
        _ansc_strcat(sDhcpv6ServerPool[Index].Info.IANAPrefixes, sDhcpv6ServerPool[Index].Cfg.IANAManualPrefixes );
    }
   
    setpool_into_utopia(DHCPV6S_NAME, "pool", Index, &sDhcpv6ServerPool[Index]);

    CosaDmlDHCPv6sTriggerRestart(FALSE);

	// Check whether static DNS got enabled or not. If enabled then we need to restart the zebra process
	if( bNeedZebraRestart )
	{
        CcspTraceWarning(("%s Restarting Zebra Process\n", __FUNCTION__));
        system("sysevent set zebra-restart");
	}

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sGetPoolCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPSV6_POOL_CFG    pCfg
    )
{
    ULONG                           Index = 0;

    /* RDKB-6780, CID-33390, out of bound read
    ** Index must be less than the max array size
    */
    for( Index = 0; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM); Index++)
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == pCfg->InstanceNumber)
            break;
    }

    if(Index < DHCPV6S_POOL_NUM)
    {
        *pCfg = sDhcpv6ServerPool[Index].Cfg;
    }
    else
    {
        *pCfg = sDhcpv6ServerPool[DHCPV6S_POOL_NUM-1].Cfg;
    }
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sGetPoolInfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_INFO pInfo
    )
{
    char cmd[256] = {0};
    char out[256] = {0};

    ULONG                           Index = 0;

    /* RDKB-6780, CID-33131, Out-of-bounds read
    ** Global array defined is of size DHCPV6S_POOL_NUM
    ** Hence limiting upper boundary of index to avoid out of boud access.
    */
    for( Index = 0; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM); Index++)
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulInstanceNumber )
            break;
    }

    if (Index >= DHCPV6S_POOL_NUM)
    {
        return ANSC_STATUS_FAILURE;
    }

    sprintf(cmd, "ps -A|grep %s", SERVER_BIN);
    _get_shell_output(cmd, out, sizeof(out));

    if ( (strstr(out, SERVER_BIN)) && sDhcpv6ServerPool[Index].Cfg.bEnabled )
    {
        sDhcpv6ServerPool[0].Info.Status  = COSA_DML_DHCP_STATUS_Enabled;
    }
    else
    {
        sDhcpv6ServerPool[0].Info.Status  = COSA_DML_DHCP_STATUS_Disabled;
    }
    
    *pInfo = sDhcpv6ServerPool[Index].Info;

    return ANSC_STATUS_SUCCESS;
}


#define DHCPS6V_CLIENT_FILE "/var/lib/dibbler/server-client.txt"
ULONG   g_dhcps6v_client_num  = 0;
PCOSA_DML_DHCPSV6_CLIENT        g_dhcps6v_client        = NULL;
PCOSA_DML_DHCPSV6_CLIENTCONTENT g_dhcps6v_clientcontent = NULL;

BOOL tagPermitted(int tag)
{
    int i = 0;

    for ( i =0; i < sizeof(tagList)/sizeof(struct DHCP_TAG); i++ )
    {
        if ( tag == tagList[i].tag )
            break;
    }

    if ( i < sizeof(tagList)/sizeof(struct DHCP_TAG) )
        return true;
    else
        return false;
}

/* 
file: /var/lib/dibbler/server-client.txt 

ClientNum:1
DUID:00:01:00:01:c7:92:bf:d0:00:25:2e:7d:05:b5
Unicast:fe80::225:2eff:fe7d:5b4
Addr:fc00:1::2:fdd1:a9ab:bbc7
Timestamp:1318388831
Prefered:3600
Valid:7200
Addr:fc00:1::2:8fd8:da4f:a598
Timestamp:1318388831
Prefered:3600
Prefered:7200
option:1:00010001C793092300252E7D05B4
option:3:00000001FFFFFFFFFFFFFFFF00050018FC000002000000000003968D133EC4A800000E1000001C20
option:3:00000002FFFFFFFFFFFFFFFF00050018FC000002000000000003678E7E6D42CF00000E1000001C20
option:3:00000003FFFFFFFFFFFFFFFF00050018FC000002000000000003B19B73D106E500000E1000001C20
option:6:0017
option:2:000100011654D9A4000C299B4E7A
option:8:0000

*/
void _cosa_dhcpsv6_get_client()
{
    ULONG i = 0;
    FILE * fp = fopen(DHCPS6V_CLIENT_FILE, "r");
    UCHAR  oneLine[256] = {0};
    PUCHAR pTmp1 = NULL;
    PUCHAR pTmp2 = NULL;
    PUCHAR pTmp3 = NULL;
    ULONG  Index = 0xFFFFFFFF;
    time_t t1    = 0;
    struct tm t2 = {0};
    UCHAR  buf[128] = {0};
    ULONG  timeStamp = 0;    
    PCOSA_DML_DHCPSV6_CLIENT_OPTION          pOption1 = NULL;
    PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS     pIPv6Address1 = NULL;
    PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX      pIPv6Prefix1  = NULL;

    /* When read error, we keep current configuration */
    if ( fp == NULL )
        return;
    
    /* First free all */
    if ( g_dhcps6v_client ) 
    {
        for(i=0; i<g_dhcps6v_client_num; i++)
        {
            if ( g_dhcps6v_clientcontent[i].pIPv6Address )
                AnscFreeMemory(g_dhcps6v_clientcontent[i].pIPv6Address);

            if ( g_dhcps6v_clientcontent[i].pIPv6Prefix )
                AnscFreeMemory(g_dhcps6v_clientcontent[i].pIPv6Prefix);

            if ( g_dhcps6v_clientcontent[i].pOption )
                AnscFreeMemory(g_dhcps6v_clientcontent[i].pOption);
        }

        AnscFreeMemory( g_dhcps6v_client );
        AnscFreeMemory( g_dhcps6v_clientcontent );

        g_dhcps6v_client = NULL;
        g_dhcps6v_clientcontent = NULL;
        
        g_dhcps6v_client_num = 0;
    }

    /* Get all from file again */
    while ( fgets(oneLine, sizeof(oneLine), fp ) != NULL )
    {
        pTmp1 = &oneLine[0];
        pTmp2 = pTmp1;
        while( (*pTmp2 != ':') && ( *pTmp2 != 0) ) pTmp2++;

        if ( *pTmp2 == 0 )
        {
            continue;
        }

        *pTmp2 = 0;
        pTmp2++;

        pTmp3 = pTmp2;

        while( (*pTmp2 != 10) && (*pTmp2 != 13) && ( *pTmp2 != 0) ) pTmp2++;

        if ( *pTmp2 != 0)
        {
            *pTmp2 = 0;
            pTmp2++;
        }
        
        if ( (ULONG)pTmp2 <= (ULONG)pTmp3 )
        {
            /* Case: XXX:      */
            continue;
        }

        /*
                    pTmp1  name
                    pTmp3: value
                */

        if ( _ansc_strcmp(pTmp1, "ClientNum") == 0 )
        {
            // this case must be first one
            g_dhcps6v_client_num = atoi(pTmp3);

            if ( g_dhcps6v_client_num ) 
            {
                g_dhcps6v_client = (PCOSA_DML_DHCPSV6_CLIENT)AnscAllocateMemory(sizeof(COSA_DML_DHCPSV6_CLIENT)*g_dhcps6v_client_num);
                 if ( !g_dhcps6v_client )
                {
                    g_dhcps6v_client_num = 0;
                    goto EXIT;
                }

                for ( i=0; i < g_dhcps6v_client_num; i++ )
                {
                    _ansc_sprintf(g_dhcps6v_client[i].Alias, "Client%d",i+1);
                }

                g_dhcps6v_clientcontent = (PCOSA_DML_DHCPSV6_CLIENTCONTENT)AnscAllocateMemory(sizeof(COSA_DML_DHCPSV6_CLIENTCONTENT)*g_dhcps6v_client_num);

                if ( !g_dhcps6v_clientcontent )
                {
                    AnscFreeMemory(g_dhcps6v_client);
                    g_dhcps6v_client = NULL;

                    g_dhcps6v_client_num = 0;
                    goto EXIT;
                }
                
                Index = 0xFFFFFFFF;
            }
            else
            {
                goto EXIT;
            }

        }else if ( _ansc_strcmp(pTmp1, "DUID") == 0 )
        {
            if ( Index == 0xFFFFFFFF )
            {
                Index = 0;
            }
            else
            {
                Index++;
            }
            /* DUID will be in dump files */
                
        } else if ( _ansc_strcmp(pTmp1, "option") == 0 )
        {
              //  pTmp1  name //option
              //  pTmp3: value //1:00010001C793092300252E7D05B4
            pTmp1 = pTmp3;
            while((*pTmp1 != ':') && (*pTmp1 != 0)) pTmp1++;

            if ( *pTmp1 == 0 )
                continue;

            *pTmp1 = 0;
            *pTmp1++;

            // pTmp3 is 1
            // pTmp1 is 00010001C793092300252E7D05B4

            // allocate new one array 
            g_dhcps6v_clientcontent[Index].NumberofOption++;
            if ( g_dhcps6v_clientcontent[Index].NumberofOption == 1 )
            {
                g_dhcps6v_clientcontent[Index].pOption = (PCOSA_DML_DHCPSV6_CLIENT_OPTION)AnscAllocateMemory(sizeof(COSA_DML_DHCPSV6_CLIENT_OPTION));
            }
            else
            {
                pOption1 = (PCOSA_DML_DHCPSV6_CLIENT_OPTION)AnscAllocateMemory(sizeof(COSA_DML_DHCPSV6_CLIENT_OPTION)*g_dhcps6v_clientcontent[Index].NumberofOption );
                if ( !pOption1 )
                    continue;

                
                AnscCopyMemory(pOption1, g_dhcps6v_clientcontent[Index].pOption, sizeof(COSA_DML_DHCPSV6_CLIENT_OPTION)*(g_dhcps6v_clientcontent[Index].NumberofOption-1) ); 
                AnscFreeMemory(g_dhcps6v_clientcontent[Index].pOption);
                g_dhcps6v_clientcontent[Index].pOption = pOption1;
                pOption1 = NULL;
            }

            if (  g_dhcps6v_clientcontent[Index].pOption == NULL )
            {
                g_dhcps6v_clientcontent[Index].NumberofOption = 0;
                continue;
            }
            
            g_dhcps6v_clientcontent[Index].pOption[g_dhcps6v_clientcontent[Index].NumberofOption-1].Tag = _ansc_atoi(pTmp3);
            AnscCopyString( g_dhcps6v_clientcontent[Index].pOption[g_dhcps6v_clientcontent[Index].NumberofOption-1].Value, pTmp1 );
        }else if ( _ansc_strcmp(pTmp1, "Unicast") == 0 )
        {
            AnscCopyString(g_dhcps6v_client[Index].SourceAddress, pTmp3);
        }else if ( _ansc_strcmp(pTmp1, "Addr") == 0 )
        {
            g_dhcps6v_clientcontent[Index].NumberofIPv6Address++;
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Address == 1 )
            {
                g_dhcps6v_clientcontent[Index].pIPv6Address = (PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS)AnscAllocateMemory(sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS));
            }
            else
            {
                pIPv6Address1 = (PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS)AnscAllocateMemory( sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS)*g_dhcps6v_clientcontent[Index].NumberofIPv6Address );
                if ( !pIPv6Address1 )
                    continue;

                AnscCopyMemory(pIPv6Address1, g_dhcps6v_clientcontent[Index].pIPv6Address, sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS)*(g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1) ); 
                AnscFreeMemory(g_dhcps6v_clientcontent[Index].pIPv6Address);
                g_dhcps6v_clientcontent[Index].pIPv6Address = pIPv6Address1;
                pIPv6Address1 = NULL;
            }

            if ( g_dhcps6v_clientcontent[Index].pIPv6Address == NULL )
            {
                g_dhcps6v_clientcontent[Index].NumberofIPv6Address = 0;
                continue;
            }
            
            AnscCopyString (g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].IPAddress, pTmp3);
        }else if ( _ansc_strcmp(pTmp1, "Timestamp") == 0 )
        {
            timeStamp = atoi(pTmp3);
        }else if ( _ansc_strcmp(pTmp1, "Prefered") == 0 )
        {
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Address )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if (t1 == 0xffffffff)
                {
                    strcpy(buf, "9999-12-31T23:59:59Z");
                }
                else
                {
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ\0", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec);
                }

                AnscCopyString (g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].PreferredLifetime, buf);               
            }
        }else if ( _ansc_strcmp(pTmp1, "Valid") == 0 )
        {
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Address )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if (t1 == 0xffffffff)
                {
                    strcpy(buf, "9999-12-31T23:59:59Z");
                }
                else
                {
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ\0", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec);
                }

                AnscCopyString (g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].ValidLifetime, buf);               
            }
        }else if ( _ansc_strcmp(pTmp1, "pdPrefix") == 0 )
        {
            g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix++;
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix == 1 )
            {
                g_dhcps6v_clientcontent[Index].pIPv6Prefix = (PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX)AnscAllocateMemory(sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6PREFIX));
            }
            else
            {
                pIPv6Prefix1 = (PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX)AnscAllocateMemory( sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6PREFIX)*g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix );
                if ( !pIPv6Prefix1 )
                    continue;

                AnscCopyMemory(pIPv6Prefix1, g_dhcps6v_clientcontent[Index].pIPv6Prefix, sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6PREFIX)*(g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1) ); 
                AnscFreeMemory(g_dhcps6v_clientcontent[Index].pIPv6Prefix);
                g_dhcps6v_clientcontent[Index].pIPv6Prefix= pIPv6Prefix1;
                pIPv6Prefix1 = NULL;
            }

            if ( g_dhcps6v_clientcontent[Index].pIPv6Prefix== NULL )
            {
                g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix= 0;
                continue;
            }
            
            AnscCopyString (g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].Prefix, pTmp3);

        }else if ( _ansc_strcmp(pTmp1, "pdTimestamp") == 0 )
        {
            timeStamp = atoi(pTmp3);
        }else if ( _ansc_strcmp(pTmp1, "pdPrefered") == 0 )
        {
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if (t1 == 0xffffffff)
                {
                    strcpy(buf, "9999-12-31T23:59:59Z");
                }
                else
                {
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ\0", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec);
                }

                AnscCopyString (g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].PreferredLifetime, buf);               
            }

        }else if ( _ansc_strcmp(pTmp1, "pdValid") == 0 )
        {
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if (t1 == 0xffffffff)
                {
                    strcpy(buf, "9999-12-31T23:59:59Z");
                }
                else
                {
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ\0", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec);
                }

                AnscCopyString (g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].ValidLifetime, buf);               
            }

        }else
        {
            continue;
        }        

        AnscZeroMemory( oneLine, sizeof(oneLine) );
    }

EXIT:
    
    fclose(fp);

    return;
}


ANSC_STATUS
CosaDmlDhcpv6sPing( PCOSA_DML_DHCPSV6_CLIENT        pDhcpsClient )
{
    UCHAR     cmd[256] = {0};
    char      out[256] = {0};
    ULONG     i        = 0;

    for( i =0; i<g_dhcps6v_client_num; i++ )
    {
        if ( _ansc_strcmp(g_dhcps6v_client[i].SourceAddress, pDhcpsClient->SourceAddress) == 0 )
        {
            break;
        }
    }

    if ( i >= g_dhcps6v_client_num )
    {
        return ANSC_STATUS_FAILURE;
    }

    if ( !g_dhcps6v_clientcontent[i].NumberofIPv6Address )
    {
        return ANSC_STATUS_FAILURE;
    }

    if ( g_dhcps6v_clientcontent[i].pIPv6Address == NULL )
    {
        return ANSC_STATUS_FAILURE;
    }

    /*ping -w 2 -c 1 fe80::225:2eff:fe7d:5b5 */
    _ansc_sprintf(cmd, "ping6 -w 2 -c 1 %s\n", g_dhcps6v_clientcontent[i].pIPv6Address[i].IPAddress );

    if (_get_shell_output2(cmd, "0 packets received"))
    {
        /*1 packets transmitted, 0 packets received, 100% packet loss*/
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        /*1 packets transmitted, 1 packets received, 0% packet loss*/
        return ANSC_STATUS_SUCCESS;
    }


}

/*
  *   This is for 
        Pool.{i}.Client.{i}.
  *
  */
ANSC_STATUS
CosaDmlDhcpv6sGetClient
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_CLIENT   *ppEntry,
        PULONG                      pSize
    )
{        
    /* We don't support this table currently */
    _cosa_dhcpsv6_get_client();
    
    *pSize = g_dhcps6v_client_num;
    
    if (!*pSize)
        return ANSC_STATUS_FAILURE;

    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT)AnscAllocateMemory( sizeof(COSA_DML_DHCPSV6_CLIENT) * *pSize);
    if ( *ppEntry == NULL )
    {
        *pSize = 0;
        return ANSC_STATUS_FAILURE;
    }
    
    AnscCopyMemory( *ppEntry, g_dhcps6v_client,  sizeof(COSA_DML_DHCPSV6_CLIENT)*(*pSize) );

    return ANSC_STATUS_SUCCESS;
}

/*
  *   This is for 
        Pool.{i}.Client.{i}.IPv6Address.{i}.
  *
  */
ANSC_STATUS
CosaDmlDhcpv6sGetIPv6Address
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulClientIndex,
        PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS   *ppEntry,
        PULONG                      pSize
    )
{
    *pSize = g_dhcps6v_clientcontent[ulClientIndex].NumberofIPv6Address ;
    
    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS)AnscAllocateMemory( sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS) * (*pSize) );

    if ( *ppEntry == NULL )
    {
        *pSize = 0;
        return ANSC_STATUS_FAILURE;
    }

    AnscCopyMemory( *ppEntry, g_dhcps6v_clientcontent[ulClientIndex].pIPv6Address, sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS) * (*pSize) );

    return ANSC_STATUS_SUCCESS;
}

/*
  *   This is for 
        Pool.{i}.Client.{i}.IPv6Prefix.{i}.
  *
  */
ANSC_STATUS
CosaDmlDhcpv6sGetIPv6Prefix
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulClientIndex,
        PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX   *ppEntry,
        PULONG                      pSize
    )
{        
    *pSize = g_dhcps6v_clientcontent[ulClientIndex].NumberofIPv6Prefix;
    
    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX)AnscAllocateMemory( sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6PREFIX) * (*pSize) );

    if ( *ppEntry == NULL )
    {
        *pSize = 0;
        return ANSC_STATUS_FAILURE;
    }

    AnscCopyMemory( *ppEntry, g_dhcps6v_clientcontent[ulClientIndex].pIPv6Prefix, sizeof(COSA_DML_DHCPSV6_CLIENT_IPV6PREFIX) * (*pSize) );

    return ANSC_STATUS_SUCCESS;
}

/*
  *   This is for 
        Pool.{i}.Client.{i}.IPv6Option.{i}.
  *
  */
ANSC_STATUS
CosaDmlDhcpv6sGetIPv6Option
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulClientIndex,
        PCOSA_DML_DHCPSV6_CLIENT_OPTION   *ppEntry,
        PULONG                      pSize
    )
{        
    *pSize = g_dhcps6v_clientcontent[ulClientIndex].NumberofOption;
    
    *ppEntry = (PCOSA_DML_DHCPSV6_CLIENT_OPTION)AnscAllocateMemory( sizeof(COSA_DML_DHCPSV6_CLIENT_OPTION) * (*pSize) );

    if ( *ppEntry == NULL )
    {
        *pSize = 0;
        return ANSC_STATUS_FAILURE;
    }

    AnscCopyMemory( *ppEntry, g_dhcps6v_clientcontent[ulClientIndex].pOption, sizeof(COSA_DML_DHCPSV6_CLIENT_OPTION) * (*pSize) );

    return ANSC_STATUS_SUCCESS;
}

/*
 *  DHCP Server Pool Option
 *
 *  The options are managed on top of a DHCP server pool,
 *  which is identified through pPoolAlias
 */
ULONG
CosaDmlDhcpv6sGetNumberOfOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber
    )
{
    ULONG                           Index  =  0;
    /*RDKB-6780, CID-33219, boundary check for upper limit*/
    for( Index = 0 ; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM) ; Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
            break;
    }

    /* RDKB-6780, CID-33219, boundary check for upper limit
    ** In case index exceeds the upper limit, return last value.
    */
    if(Index < DHCPV6S_POOL_NUM)
    {
        return uDhcpv6ServerPoolOptionNum[Index];
    }
    else
    {
        return uDhcpv6ServerPoolOptionNum[DHCPV6S_POOL_NUM - 1];
    }
}

ANSC_STATUS
CosaDmlDhcpv6sGetStartAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       addr,
        int                         len
    )
{
    char globalAddress[64] = {0};
    commonSyseventGet(COSA_DML_DHCPV6C_PREF_SYSEVENT_NAME, globalAddress, sizeof(globalAddress));

    strncpy(addr, globalAddress, len>strlen(globalAddress)?strlen(globalAddress):len);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sGetOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulIndex,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    ULONG                           Index  =  0;

    /* RDKB-6780, CID-33221, out of bound read
    ** Index must be less than the max array size
    */
    for( Index = 0 ; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM); Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
            break;
    }

    if(Index < DHCPV6S_POOL_NUM)
    {
        *pEntry = sDhcpv6ServerPoolOption[Index][ulIndex];
    }
    else
    {
        *pEntry = sDhcpv6ServerPoolOption[DHCPV6S_POOL_NUM - 1][ulIndex];
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sGetOptionbyInsNum
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    ULONG                           Index   =  0;
    ULONG                           Index2  =  0;

    for( Index = 0 ; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
        {
            for( Index2 = 0; Index2 < uDhcpv6ServerPoolOptionNum[Index]; Index2++ )
            {
                if ( sDhcpv6ServerPoolOption[Index][Index2].InstanceNumber == pEntry->InstanceNumber )
                {
                    *pEntry = sDhcpv6ServerPoolOption[Index][Index2];
                    return ANSC_STATUS_SUCCESS;
                }
            }
        }
    }
    
    return ANSC_STATUS_FAILURE;
}


ANSC_STATUS
CosaDmlDhcpv6sSetOptionValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UtopiaContext utctx             = {0};
    char out[256]                   = {0};    
    ULONG                           Index   =  0;
    ULONG                           Index2  =  0;

    for( Index = 0 ; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
        {
            sDhcpv6ServerPoolOption[Index][ulIndex].InstanceNumber = ulInstanceNumber;
            AnscCopyString( sDhcpv6ServerPoolOption[Index][ulIndex].Alias, pAlias);

            if (!Utopia_Init(&utctx))
                return ANSC_STATUS_FAILURE;
            SETI_INTO_UTOPIA(DHCPV6S_NAME, "pool", Index, "option", ulIndex, "InstanceNumber", ulInstanceNumber)
            SETS_INTO_UTOPIA(DHCPV6S_NAME, "pool", Index, "option", ulIndex, "alias", pAlias)
            Utopia_Free(&utctx,1);

            return ANSC_STATUS_SUCCESS;
        }
    }
    
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlDhcpv6sAddOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    UtopiaContext utctx             = {0};
    char out[256]                   = {0};    
    ULONG                           Index   =  0;
    ULONG                           Index2  =  0;

    for( Index = 0 ; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
        {

            if ( uDhcpv6ServerPoolOptionNum[Index] >= DHCPV6S_POOL_OPTION_NUM )
                return ANSC_STATUS_NOT_SUPPORTED;

            Index2 = uDhcpv6ServerPoolOptionNum[Index];
            uDhcpv6ServerPoolOptionNum[Index]++;
            if (!Utopia_Init(&utctx))
                return ANSC_STATUS_FAILURE;
            SETI_INTO_UTOPIA(DHCPV6S_NAME, "pool", Index, "", 0, "optionnumber", uDhcpv6ServerPoolOptionNum[Index])
            Utopia_Free(&utctx,1);

            sDhcpv6ServerPoolOption[Index][Index2] = *pEntry;
            setpooloption_into_utopia(DHCPV6S_NAME,"pool",Index,"option",Index2,&sDhcpv6ServerPoolOption[Index][Index2]);

            CosaDmlDHCPv6sTriggerRestart(FALSE);

            return ANSC_STATUS_SUCCESS;
        }
    }
    
    return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS
CosaDmlDhcpv6sDelOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        ULONG                       ulInstanceNumber
    )
{
    UtopiaContext utctx             = {0};
    char out[256]                   = {0};    
    ULONG                           Index   =  0;
    ULONG                           Index2  =  0;

    for( Index = 0 ; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
        {
            for ( Index2 = 0; Index2 < uDhcpv6ServerPoolOptionNum[Index]; Index2++ )
            {
                if ( sDhcpv6ServerPoolOption[Index][Index2].InstanceNumber == ulInstanceNumber )
                {
                    break;
                }
            }
#ifndef MULTILAN_FEATURE
	    unsetpooloption_from_utopia(DHCPV6S_NAME,"pool",Index,"option",Index2);
#endif
            for( Index2++; Index2  < uDhcpv6ServerPoolOptionNum[Index]; Index2++ )
            {
#if defined (MULTILAN_FEATURE)
                setpooloption_into_utopia(DHCPV6S_NAME,"pool",Index,"option",Index2-1,&sDhcpv6ServerPoolOption[Index][Index2]);
#endif
                sDhcpv6ServerPoolOption[Index][Index2-1] =  sDhcpv6ServerPoolOption[Index][Index2];
            }
#if defined (MULTILAN_FEATURE)
            unsetpooloption_from_utopia(DHCPV6S_NAME,"pool",Index,"option",Index2-1);

            uDhcpv6ServerPoolOptionNum[Index] = (0 >(uDhcpv6ServerPoolOptionNum[Index]-1)? 0: (uDhcpv6ServerPoolOptionNum[Index]-1) );
#else
            uDhcpv6ServerPoolOptionNum[Index]--;
#endif
            if (!Utopia_Init(&utctx))
                return ANSC_STATUS_FAILURE;
            SETI_INTO_UTOPIA(DHCPV6S_NAME, "pool", Index, "", 0, "optionnumber", uDhcpv6ServerPoolOptionNum[Index])
            Utopia_Free(&utctx,1);

            CosaDmlDHCPv6sTriggerRestart(FALSE);

            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDhcpv6sSetOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulPoolInstanceNumber,
        PCOSA_DML_DHCPSV6_POOL_OPTION  pEntry
    )
{
    UtopiaContext utctx                     = {0};
    char out[256]                           = {0};    
    ULONG                           Index   =  0;
    ULONG                           Index2  =  0;

    for( Index = 0 ; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
        {
            for ( Index2 = 0; Index2 < uDhcpv6ServerPoolOptionNum[Index]; Index2++ )
            {
                if ( sDhcpv6ServerPoolOption[Index][Index2].InstanceNumber == pEntry->InstanceNumber )
                {
                    if ( pEntry->Tag == 23 &&
                        ( _ansc_strcmp(sDhcpv6ServerPoolOption[Index][Index2].PassthroughClient, pEntry->PassthroughClient ) ||
                        ( _ansc_strcmp(sDhcpv6ServerPoolOption[Index][Index2].Value, pEntry->Value) &&
                                  !_ansc_strlen(pEntry->PassthroughClient) ) ) )
                    {
                        system("sysevent set zebra-restart");
                    }

                    sDhcpv6ServerPoolOption[Index][Index2] = *pEntry;

                    setpooloption_into_utopia(DHCPV6S_NAME, "pool", Index, "option", Index2, &sDhcpv6ServerPoolOption[Index][Index2]);
                    
                    CosaDmlDHCPv6sTriggerRestart(FALSE);
                    
                    return ANSC_STATUS_SUCCESS;
                }
            }
        }
    }
    
    return ANSC_STATUS_FAILURE;
}

void     
CosaDmlDhcpv6Remove(ANSC_HANDLE hContext)
{
    /*remove backend memory*/
    if (g_sent_options)
        AnscFreeMemory(g_sent_options);

    /* empty global variable */
    AnscZeroMemory(&uDhcpv6ServerPoolOptionNum, sizeof(uDhcpv6ServerPoolOptionNum));
    AnscZeroMemory(&sDhcpv6ServerPool, sizeof(sDhcpv6ServerPool));
    AnscZeroMemory(&sDhcpv6ServerPoolOption, sizeof(sDhcpv6ServerPoolOption));
    uDhcpv6ServerPoolNum = 0;

    return;
}


int dhcpv6_assign_global_ip(char * prefix, char * intfName, char * ipAddr)
{
    unsigned int    length           = 0;
    char            globalIP[64]     = {0};
    char           *pMac             = NULL;
    unsigned int    i                = 0;
    unsigned int    j                = 0;
    unsigned int    k                = 0;
    char            cmd[256]         = {0};
    char            out[256]         = {0};
    char            tmp[8]           = {0};


    _ansc_strcpy( globalIP, prefix);

    /* Prepare the first part. */

    i = _ansc_strlen(globalIP);

    while( (globalIP[i-1] != '/') && (i>0) ) i--;

    if ( i == 0 ){
        AnscTrace("error, there is not '/' in prefix:%s\n", prefix);
        return 1;
    }

    length = atoi(&globalIP[i]);

    if ( length > 64 ){
        AnscTrace("error, length is bigger than 64. prefix:%s, length:%d\n", prefix, length);
        return 1;
    }

    globalIP[i-1] = '\0';

    i = i-1;

    if ( (globalIP[i-1]!=':') && (globalIP[i-2]!=':') ){
        AnscTrace("error, there is not '::' in prefix:%s\n", prefix);
        return 1;
    }

    j = i-2;
    k = 0;
    while( j>0 ){
        if ( globalIP[j-1] == ':' )
            k++;
        j--;
    }

    if ( k == 3 )
    {
        globalIP[i-1] = '\0';
        i = i - 1;
    }

    AnscTrace("the first part is:%s\n", globalIP);


    /* prepare second part */
    _ansc_sprintf(cmd, "ifconfig %s | grep HWaddr\n", intfName );
    _get_shell_output(cmd, out, sizeof(out));
    pMac =_ansc_strstr(out, "HWaddr");
    if ( pMac == NULL ){
        AnscTrace("error, this interface has not a mac address .\n");
        return 1;
    }

    pMac += _ansc_strlen("HWaddr");
    while( pMac && (pMac[0] == ' ') )
        pMac++;

    /* switch 7bit to 1*/
    tmp[0] = pMac[1];

    k = strtol(tmp, (char **)NULL, 16);

    k = k ^ 0x2;
    if ( k < 10 )
        k += '0';
    else
        k += 'A' - 10;

    pMac[1] = k;
    pMac[17] = '\0';

    //00:50:56: FF:FE:  92:00:22
    _ansc_strncpy(out, pMac, 9);
    out[9] = '\0';
    _ansc_strcat(out, "FF:FE:");
    _ansc_strcat(out, pMac+9);

    for(k=0,j=0;out[j];j++){
        if ( out[j] == ':' )
            continue;
        globalIP[i++] = out[j];
        if ( ++k == 4 ){
           globalIP[i++] = ':';
           k = 0;
        }
    }

    globalIP[i-1] = '\0';

    AnscTrace("the full part is:%s\n", globalIP);

    _ansc_strcpy(ipAddr, globalIP);
    
    /* This IP should be unique. If not I have no idea. */
    return 0;
}


/* Dibbler reboot need 3 conditions.
    a get a global prefix. 
    b need brlan0 is ready
    c need brlan0 has the link local address(this should be ready when brlan0 is ready)
*/
void CosaDmlDhcpv6sRebootServer()
{
    int fd = 0;
    char cmd[64] = {0};
    char out[128] = {0};
    BOOL isBridgeMode = FALSE;

    if (!g_dhcpv6_server_prefix_ready || !g_lan_ready)
        return;
#if defined (MULTILAN_FEATURE)
    commonSyseventSet("dhcpv6s-restart", "");
#else
    if (g_dhcpv6s_restart_count) {
        g_dhcpv6s_restart_count=0;

        //when need stop, it's supposed the configuration file need to be updated.
        _cosa_dhcpsv6_refresh_config();

        _dibbler_server_operation("stop");
    }

    fd = open(DHCPV6S_SERVER_PID_FILE, O_RDONLY);
    if (fd < 0) {

         is_usg_in_bridge_mode(&isBridgeMode);
         if ( isBridgeMode )
            return;

        //make sure it's not in a bad status
        sprintf(cmd, "ps|grep %s|grep -v grep", SERVER_BIN);
        _get_shell_output(cmd, out, sizeof(out));
        if (strstr(out, SERVER_BIN))
        {
            sprintf(cmd, "kill -15 `pidof %s`", SERVER_BIN);
            system(cmd);
	    sleep(1);
        }
	
        _dibbler_server_operation("start");
    } else{
        close(fd);
    }
#endif

    // refresh lan if we were asked to
    if (g_dhcpv6s_refresh_count) {
        g_dhcpv6s_refresh_count = 0;
        system("gw_lan_refresh");
    }

    return;
}
/*int calcPrefixNumber(int prefixLen, int req_prefixLen)
{
int delta = 0;
int numOfSubnets = 2;
if(req_prefixLen > prefixLen)
{
	delta = req_prefixLen - prefixLen;
	printf("<<< delta  = %d >>>\n",delta);
	numOfSubnets = numOfSubnets << (delta-1);
	
}
else
{
	numOfSubnets = 1;
}
printf("<<<  numOfSubnets = %d >>>\n",numOfSubnets); 
return numOfSubnets;
}*/
#define POS_PREFIX_DELEGATION 7
int CalcIPv6Prefix(char *GlobalPref, char *pref,int index)
{
    unsigned char buf[sizeof(struct in6_addr)];
    int domain, s;
    char str[INET6_ADDRSTRLEN];
   domain = AF_INET6;
   s = inet_pton(domain, GlobalPref, buf);
    if (s <= 0) {
        if (s == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
           return 0;
    }
   buf[POS_PREFIX_DELEGATION] = buf[POS_PREFIX_DELEGATION] + index;
   if (inet_ntop(domain, buf, str, INET6_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
            return 0;
    }
    printf("%s\n", str);
    strcpy(pref,str);
    return 1;
}

int GenIPv6Prefix(char *ifName,char *GlobalPref, char *pref)
{
int len = 0;
int index = 0;
char cmd[100];
char out[100];
static int interface_num = 4; // Reserving first 4 /64s for dhcp configurations
	if(ifName == NULL)
	return 0;
	
	memset(cmd,0,sizeof(cmd));
	memset(out,0,sizeof(out));
	_ansc_sprintf(cmd, "%s%s",ifName,"_ipv6_index");
	commonSyseventGet(cmd, out, sizeof(out));
	if(strlen(out) != 0)
	{
		index = atoi(out);
	}

	len = strlen(GlobalPref);
	strcpy(pref,GlobalPref);
	if(index == 0)
	{
		if(CalcIPv6Prefix(GlobalPref,pref,interface_num)== 0)
		return 0;
		memset(cmd,0,sizeof(cmd));
		_ansc_sprintf(cmd, "%s%s",ifName,"_ipv6_index");
		_ansc_sprintf(out, "%d",interface_num);
		commonSyseventSet(cmd, out);
		interface_num++;
	}
	else
	{
		if(CalcIPv6Prefix(GlobalPref,pref,index)==0 )
		return 0;
	}
	strcat(pref,"/64");
        CcspTraceInfo(("%s: pref %s\n", __func__, pref));
return 1;
	
}
/* This thread is added to handle the LnF interface IPv6 rule, because LnF is coming up late in XB6 devices. 
This thread can be generic to handle the operations depending on the interfaces. Other interface and their events can be register here later based on requirement */
static int sysevent_fd_1;
static token_t sysevent_token_1;
static pthread_t InfEvtHandle_tid;
static void *InterfaceEventHandler_thrd(void *data)
{
    async_id_t interface_asyncid;
    async_id_t interface_XHS_asyncid;
    CcspTraceWarning(("%s started\n",__FUNCTION__));
    sysevent_fd_1 = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "Interface_evt_handler", &sysevent_token_1);

    sysevent_set_options(sysevent_fd_1, sysevent_token_1, "multinet_6-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_1, sysevent_token_1, "multinet_6-status",  &interface_asyncid);
    sysevent_set_options(sysevent_fd_1, sysevent_token_1, "multinet_2-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_1, sysevent_token_1, "multinet_2-status",  &interface_XHS_asyncid);
    while(1)
	{
         async_id_t getnotification_asyncid;
         int err;
         unsigned char name[25], val[42],buf[128],cmd[128];
         int namelen = sizeof(name);
         int vallen  = sizeof(val);
		err = sysevent_getnotification(sysevent_fd_1, sysevent_token_1, name, &namelen,  val, &vallen, &getnotification_asyncid);

        	if (err)
        	{
           		CcspTraceWarning(("sysevent_getnotification failed with error: %d %s\n", err,__FUNCTION__));
           		CcspTraceWarning(("sysevent_getnotification failed name: %s val : %s\n", name,val));
			if ( 0 != system("pidof syseventd")) {

           			CcspTraceWarning(("%s syseventd not running ,breaking the receive notification loop \n",__FUNCTION__));
				break;
			}	
        	}
        	else
        	{

           		CcspTraceWarning(("%s Recieved notification event  %s\n",__FUNCTION__,name));
			if(strcmp(name,"multinet_6-status") == 0)
			{
				if(strcmp(val, "ready") == 0)
				{
    					commonSyseventGet("br106_ipaddr_v6", buf, sizeof(buf));
                                        memset(cmd,0,sizeof(cmd));
                                        _ansc_sprintf(cmd, "ip -6 route add %s dev br106",buf);
					system(cmd);
					#ifdef _COSA_INTEL_XB3_ARM_
                                        memset(cmd,0,sizeof(cmd));
                                        _ansc_sprintf(cmd, "ip -6 route add %s dev br106 table erouter",buf);
					system(cmd);
					#endif
                                        memset(cmd,0,sizeof(cmd));
                                        sprintf(cmd, "ip -6 rule add iif br106 lookup erouter");
                                        system(cmd);
				}

			}
			if(strcmp(name,"multinet_2-status") == 0)
			{
				if(strcmp(val, "ready") == 0)
				{
					char *Inf_name = NULL;
					int retPsmGet = CCSP_SUCCESS;
					retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.2.Port.1.Name", NULL, &Inf_name);
            				if (retPsmGet == CCSP_SUCCESS)
					{
						char tbuff[100];
		                                memset(cmd,0,sizeof(cmd));
						memset(tbuff,0,sizeof(tbuff));
						sprintf(cmd,"sysctl net.ipv6.conf.%s.autoconf",Inf_name);
						_get_shell_output(cmd, tbuff, sizeof(tbuff));
						if(tbuff[strlen(tbuff)-1] == '0')
						{
							memset(cmd,0,sizeof(cmd));
							sprintf(cmd,"sysctl -w net.ipv6.conf.%s.autoconf=1",Inf_name);
							system(cmd);
							memset(cmd,0,sizeof(cmd));
							sprintf(cmd,"ifconfig %s down;ifconfig %s up",Inf_name,Inf_name);
							system(cmd);
						}

		                                memset(cmd,0,sizeof(cmd));
		                                _ansc_sprintf(cmd, "%s_ipaddr_v6",Inf_name);
	    					commonSyseventGet(cmd, buf, sizeof(buf));
		                                memset(cmd,0,sizeof(cmd));
		                                _ansc_sprintf(cmd, "ip -6 route add %s dev %s",buf,Inf_name);
						system(cmd);
						#ifdef _COSA_INTEL_XB3_ARM_
		                                memset(cmd,0,sizeof(cmd));
		                                _ansc_sprintf(cmd, "ip -6 route add %s dev %s table erouter",buf,Inf_name);
						system(cmd);
						#endif
		                                memset(cmd,0,sizeof(cmd));
		                                sprintf(cmd, "ip -6 rule add iif %s lookup erouter",Inf_name);
		                                system(cmd);
						((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
					}
					else
					{
						CcspTraceWarning(("%s PSM get failed for interface name\n", __FUNCTION__));
					}
				}

			}	
	
		}
	}

}

static int sysevent_fd_mnet;
static token_t sysevent_token_mnet;

static void * 
dhcpv6c_dbg_thrd(void * in)
{
    int fd=0 , fd1=0;
    char msg[1024] = {0};
    int i;
    char * p = NULL;
    char globalIP2[128] = {0};
    char out[128] = {0};
    //When PaM restart, this is to get previous addr.
    commonSyseventGet("lan_ipaddr_v6", globalIP2, sizeof(globalIP2));
    if ( globalIP2[0] )
        CcspTraceWarning((stderr,"%s  It seems there is old value(%s)\n", __FUNCTION__, globalIP2));

    sysevent_fd_mnet = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "Multinet Status", &sysevent_token_mnet);
   
    CcspTraceWarning(("%s sysevent_fd_mnet is %d\n", __FUNCTION__, sysevent_fd_mnet));
 
    unsigned char lan_multinet_state[16] ;
    
    int return_val=0;

    fd_set rfds;
    struct timeval tm;

    fd= open(CCSP_COMMON_FIFO, O_RDWR);

    if (fd< 0) 
    {
        DHCPVS_DEBUG_PRINT
        fprintf(stderr,"open common fifo!!!!!!!!!!!!\n");
        goto EXIT;
    }

    fd1= open(DHCPS6V_SERVER_RESTART_FIFO, O_RDWR);

    if (fd1< 0) 
    {
        fprintf(stderr, "open dhcpv6 server restart fifo!!!!!\n");
        goto EXIT;
    }

    while (1) 
    {
	int retCode = 0;
        tm.tv_sec  = 60;
        tm.tv_usec = 0;
    
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        FD_SET(fd1, &rfds);

	retCode = select( (fd>fd1)?(fd+1):(fd1+1), &rfds, NULL, NULL, &tm);
        /* When return -1, it's error.
           When return 0, it's timeout
           When return >0, it's the number of valid fds */
        if (retCode < 0) {
	    fprintf(stderr, "dbg_thrd : select returns error \n" );

            if (errno == EINTR)
                continue;
            
            DHCPVS_DEBUG_PRINT
            CcspTraceWarning(("%s -- select(): %s", __FUNCTION__, strerror(errno)));
            goto EXIT;
        }
	else if(retCode == 0 )
	    continue;

        /*We need consume the data.
          It's possible more than one triggering events are consumed in one time, which is expected.*/
        if (FD_ISSET(fd1, &rfds)) {
            /*this sleep help do two things: 
                * When GUI operate too fast, it gurantees more operations combine into one; 
                * Not frequent dibbler start/stop. When do two start fast, dibbler will in bad status. 
              */
            sleep(3);
            memset(msg, 0, sizeof(msg));
            read(fd1, msg, sizeof(msg));

            CosaDmlDhcpv6sRebootServer();
	    continue;
        }

        if ( FD_ISSET(fd, &rfds) )
        {
	     memset(msg, 0, sizeof(msg));
             read(fd, msg, sizeof(msg));

            /*check dibbler server status*/
//            CosaDmlDhcpv6sRebootServer();
//            continue;
        }
	else
		continue;

        if (msg[0] != 0)
        {
            CcspTraceInfo(("%s: get message %s\n", __func__, msg));
        } else {
	    //Message is empty. Wait 5 sec before trying the select again.
            sleep(5);
            continue;
        }

        if (!strncmp(msg, "dibbler-client", strlen("dibbler-client"))) 
        {
            char v6addr[64] = {0};
            char v6pref[128] = {0};
            char v6pref_addr[128] = {0};
            char v6Tpref[128] = {0};
            int pref_len = 0;
            
            char iana_t1[32]    = {0};
            char iana_t2[32]    = {0};
            char iana_iaid[32]  = {0};
            char iana_pretm[32] = {0};
            char iana_vldtm[32] = {0};
            
            char iapd_t1[32]    = {0};
            char iapd_t2[32]    = {0};
            char iapd_iaid[32]  = {0};
            char iapd_pretm[32] = {0};
            char iapd_vldtm[32] = {0};

            int t1 = 0;
            int idx = 0;
            char action[64] = {0};
            char * pString = NULL;
            char objName[128] = {0};
            char globalIP[128] = {0};
            BOOL bRestartLan = FALSE;
            int  ret = 0;
#ifdef _HUB4_PRODUCT_REQ_
            char ula_address[64] = {0};
#endif
            /*the format is :
             add 2000::ba7a:1ed4:99ea:cd9f :: 0 t1
             action, address, prefix, pref_len 3600
            now action only supports "add", "del"*/
            
            p = msg+strlen("dibbler-client");
            while(isblank(*p)) p++;

            fprintf(stderr, "%s -- %d !!! get event from v6 client: %s \n", __FUNCTION__, __LINE__,p);

            if (sscanf(p, "%63s %63s %s %s %s %s %s %63s %d %s %s %s %s %s", 
                       action, v6addr,    iana_iaid, iana_t1, iana_t2, iana_pretm, iana_vldtm,
                       v6pref, &pref_len, iapd_iaid, iapd_t1, iapd_t2, iapd_pretm, iapd_vldtm ) == 14)
            {
                pString = CosaUtilGetFullPathNameByKeyword
                    (
                        "Device.IP.Interface.",
                        "Name",
                        COSA_DML_DHCPV6_CLIENT_IFNAME
                        );

                if (!strncmp(action, "add", 3))
                {
                    CcspTraceInfo(("%s: add\n", __func__));

// Waiting until private lan interface is ready , so that we can assign global ipv6 address and also start dhcp server.
			while (1)
			{

			        memset(lan_multinet_state,0,sizeof(lan_multinet_state));
                    		return_val=sysevent_get(sysevent_fd_mnet, sysevent_token_mnet, "multinet_1-status", lan_multinet_state, sizeof(lan_multinet_state));
			
                        	CcspTraceWarning(("%s multinet_1-status is %s, ret val is %d\n",__FUNCTION__,lan_multinet_state,return_val));

				if(strcmp(lan_multinet_state, "ready") == 0)
				{
					break;
				}
			
				sleep(5);
			  }
		     	
                    /*for now we only support one address, one prefix notify, if need multiple addr/prefix, must modify dibbler-client code*/
                    if (strncmp(v6addr, "::", 2) != 0) 
                    {
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_SYSEVENT_NAME,       v6addr);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_IAID_SYSEVENT_NAME,  iana_iaid);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_T1_SYSEVENT_NAME,    iana_t1);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_T2_SYSEVENT_NAME,    iana_t2);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_PRETM_SYSEVENT_NAME, iana_pretm);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_VLDTM_SYSEVENT_NAME, iana_vldtm);

                        if(pString)
                        {
                            snprintf(objName, sizeof(objName)-1, "%sIPv6Address.", pString);
                            g_COSARepopulateTable(g_pDslhDmlAgent, objName);
                        }
                    }
                    
                    if (strncmp(v6pref, "::", 2) != 0)
                    {
			memset(v6Tpref,0,sizeof(v6Tpref));
			strncpy(v6Tpref,v6pref,sizeof(v6Tpref));
                        /*We just delegate longer and equal 64bits. Use zero to fill in the slot naturally. */
#if defined (MULTILAN_FEATURE)
                        sprintf(v6pref+strlen(v6pref), "/%d", pref_len);
#else
			if ( pref_len >= 64 )
                            sprintf(v6pref+strlen(v6pref), "/%d", pref_len);
                        else
						{
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION)
							sprintf(v6pref+strlen(v6pref), "/%d", pref_len);
#else
                            sprintf(v6pref+strlen(v6pref), "/%d", 64);
#endif
						}
#endif
	char cmd[100];
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_CBR_PRODUCT_REQ_)
#else
			char out1[100]; 
			char *token = NULL;char *pt;
			char s[2] = ",";	
			if(pref_len < 64)
			{
			    memset(out,0,sizeof(out));
			    memset(cmd,0,sizeof(cmd));
			    memset(out1,0,sizeof(out1));
			    sprintf(cmd, "syscfg get IPv6subPrefix");
      				_get_shell_output(cmd, out, sizeof(out));
			    if(!strcmp(out,"true"))
				{
                                static int first = 0;

				memset(out,0,sizeof(out));
				memset(cmd,0,sizeof(cmd));
				sprintf(cmd, "syscfg get IPv6_Interface");
      				_get_shell_output(cmd, out, sizeof(out));
				pt = out;
				while((token = strtok_r(pt, ",", &pt)))
				 {
			 
					if(GenIPv6Prefix(token,v6Tpref,out1))
					{
						char tbuff[100];
						memset(cmd,0,sizeof(cmd));
						_ansc_sprintf(cmd, "%s%s",token,"_ipaddr_v6");
						commonSyseventSet(cmd, out1);
						memset(cmd,0,sizeof(cmd));
						memset(tbuff,0,sizeof(tbuff));
						sprintf(cmd,"sysctl net.ipv6.conf.%s.autoconf",token);
						_get_shell_output(cmd, tbuff, sizeof(tbuff));
						if(tbuff[strlen(tbuff)-1] == '0')
						{
							memset(cmd,0,sizeof(cmd));
							sprintf(cmd,"sysctl -w net.ipv6.conf.%s.autoconf=1",token);
							system(cmd);
							memset(cmd,0,sizeof(cmd));
							sprintf(cmd,"ifconfig %s down;ifconfig %s up",token,token);
							system(cmd);
						}
						memset(cmd,0,sizeof(cmd));
						sprintf(cmd, "ip -6 route add %s dev %s", out1, token);
                        			system(cmd);
						#ifdef _COSA_INTEL_XB3_ARM_
						memset(cmd,0,sizeof(cmd));
						sprintf(cmd, "ip -6 route add %s dev %s table erouter", out1, token);
                        			system(cmd);
						#endif
						memset(cmd,0,sizeof(cmd));
						sprintf(cmd, "ip -6 rule add iif %s lookup erouter",token);
						system(cmd);
						memset(out1,0,sizeof(out1));
					}
				} 
                                        memset(out,0,sizeof(out));
                                        if(first == 0)
                                        {       first = 1;
                                                pthread_create(&InfEvtHandle_tid, NULL, InterfaceEventHandler_thrd, NULL);
                                        }
				}
			}
#endif
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_SYSEVENT_NAME,       v6pref);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_IAID_SYSEVENT_NAME,  iapd_iaid);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_T1_SYSEVENT_NAME,    iapd_t1);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_T2_SYSEVENT_NAME,    iapd_t2);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_PRETM_SYSEVENT_NAME, iapd_pretm);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_VLDTM_SYSEVENT_NAME, iapd_vldtm);

#if defined (MULTILAN_FEATURE)
                        for ( idx=0; idx<uDhcpv6ServerPoolNum; idx++)
                        {
                              if(sDhcpv6ServerPool[idx].Cfg.LeaseTime != atoi(iapd_pretm))
                              {
                                  sDhcpv6ServerPool[idx].Cfg.LeaseTime = atoi(iapd_pretm);
                                  setpool_into_utopia(DHCPV6S_NAME, "pool", idx, &sDhcpv6ServerPool[idx]);
                              }
                        }
#endif
                        if(pString)
                        {
                            snprintf(objName, sizeof(objName)-1, "%sIPv6Prefix.", pString);
                            g_COSARepopulateTable(g_pDslhDmlAgent, objName);
                        }

                        g_COSARepopulateTable(g_pDslhDmlAgent, "Device.DHCPv6.Client.1.ReceivedOption.");

                        g_COSARepopulateTable(g_pDslhDmlAgent, "Device.DHCPv6.Server.Pool.1.Option.");

#if defined (MULTILAN_FEATURE)
                        g_dhcpv6_server_prefix_ready = TRUE;

                        if ((v6addr_prev[0] == '\0') || ( _ansc_strcmp(v6addr_prev, v6pref ) !=0))
                        {
                            _ansc_strncpy( v6addr_prev, v6pref, sizeof(v6pref));
                            commonSyseventSet("ipv6-restart", "1");
                        }
                        else
                        {
                            commonSyseventSet("ipv6_addr-set", "");
                        }
#endif

#ifdef MULTILAN_FEATURE
/* Service IPv6 will assign IP address and prefix allocation,
   for all lan interfaces.
*/
#if !defined(INTEL_PUMA7) && !defined(_COSA_INTEL_XB3_ARM_)
                        // not the best place to add route, just to make it work
                        // delegated prefix need to route to LAN interface
			memset(cmd,0,sizeof(cmd));
                        sprintf(cmd, "ip -6 route add %s dev %s", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
                        system(cmd);
			#ifdef _COSA_INTEL_XB3_ARM_
			memset(cmd,0,sizeof(cmd));
                        sprintf(cmd, "ip -6 route add %s dev %s table erouter", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
                        system(cmd);
			#endif
			memset(cmd,0,sizeof(cmd));
                        /* we need save this for zebra to send RA 
                           ipv6_prefix           // xx:xx::/yy
                         */
                        sprintf(cmd, "sysevent set ipv6_prefix %s \n",v6pref);
                        system(cmd);
                        CcspTraceWarning(("!run cmd1:%s", cmd));

                        CosaDmlDHCPv6sTriggerRestart(FALSE);
#if defined(_COSA_BCM_ARM_) || defined(INTEL_PUMA7)
                        CcspTraceWarning((" %s dhcpv6_assign_global_ip to brlan0 \n", __FUNCTION__));
                        ret = dhcpv6_assign_global_ip(v6pref, "brlan0", globalIP);
#elif defined _COSA_BCM_MIPS_
                        ret = dhcpv6_assign_global_ip(v6pref, COSA_DML_DHCPV6_SERVER_IFNAME, globalIP);
#else
                        /*We need get a global ip addres */
                        ret = dhcpv6_assign_global_ip(v6pref, "l2sd0", globalIP);
#endif
                        CcspTraceWarning(("%s: globalIP %s globalIP2 %s\n", __func__,
                            globalIP, globalIP2));
                        if ( _ansc_strcmp(globalIP, globalIP2 ) ){
                            bRestartLan = TRUE;

                            //PaM may restart. When this happen, we should not overwrite previous ipv6
                            if ( globalIP2[0] ) 
                               commonSyseventSet("lan_ipaddr_v6_prev", globalIP2);

                            _ansc_strcpy(globalIP2, globalIP);
                        }else{
								char lanrestart[8] = {0};
		    					commonSyseventGet("lan_restarted",lanrestart, sizeof(lanrestart));
								fprintf(stderr,"lan restart staus is %s \n",lanrestart);
   			       				if (strcmp("true",lanrestart) == 0)
							    	bRestartLan = TRUE;
								else
                            		bRestartLan = FALSE;
						}
                        g_dhcpv6s_refresh_count = bRestartLan;
                        CcspTraceWarning(("%s: bRestartLan %d\n", __func__, bRestartLan));

                        fprintf(stderr, "%s -- %d !!! ret:%d bRestartLan:%d %s %s \n", __FUNCTION__, __LINE__,ret,  bRestartLan,  globalIP, globalIP2);

                        if ( ret != 0 )
                        {
                            AnscTrace("error, assign global ip error.\n");
                        }else if ( bRestartLan == FALSE ){
                            AnscTrace("Same global IP, Need not restart.\n");
                        }else{
                            /* This is for IP.Interface.1. use */
                            commonSyseventSet(COSA_DML_DHCPV6S_ADDR_SYSEVENT_NAME, globalIP);

                            /*This is for brlan0 interface */
                            commonSyseventSet("lan_ipaddr_v6", globalIP);
                            _ansc_sprintf(cmd, "%d", pref_len);
                            commonSyseventSet("lan_prefix_v6", cmd);

                            commonSyseventSet("lan-restart", "1");
                        }
#endif
#else
#ifdef _HUB4_PRODUCT_REQ_
                        if ((iapd_vldtm[0]=='\0') || (iapd_pretm[0]=='\0')){
                            strncpy(iapd_pretm, "forever", sizeof(iapd_pretm));
                            strncpy(iapd_vldtm, "forever", sizeof(iapd_vldtm));
                        }
                        commonSyseventGet(SYSEVENT_FIELD_IPV6_ULA_ADDRESS,
                                     ula_address, sizeof(ula_address));
                        if(ula_address[0] != '\0') {
                            sprintf(cmd, "ip -6 addr add %s/64 dev %s", ula_address, COSA_DML_DHCPV6_SERVER_IFNAME);
                            system(cmd);
                        }
                        ret = dhcpv6_assign_global_ip(v6pref, COSA_DML_DHCPV6_SERVER_IFNAME, globalIP);
                        if(ret != 0) {
                            CcspTraceInfo(("Assign global ip error \n"));
                        }
                        else {
                            commonSyseventSet("lan_ipaddr_v6", globalIP);
                            sprintf(cmd, "ip -6 addr add %s/64 dev %s valid_lft %s preferred_lft %s",
                                globalIP, COSA_DML_DHCPV6_SERVER_IFNAME, iapd_vldtm, iapd_pretm);
                            CcspTraceInfo(("Going to execute: %s \n", cmd));
                            system(cmd);
                        }
                        if(strlen(v6pref) > 0) {
                            strncpy(v6pref_addr, v6pref, (strlen(v6pref)-5));
                            CcspTraceInfo(("Going to set ::1 address on brlan0 interface \n"));
                            sprintf(cmd, "ip -6 addr add %s::1/64 dev %s valid_lft %s preferred_lft %s",
                                v6pref_addr, COSA_DML_DHCPV6_SERVER_IFNAME, iapd_vldtm, iapd_pretm);
                            CcspTraceInfo(("Going to execute: %s \n", cmd));
                            system(cmd);
                        }
                        // send an event to Sky-pro app manager that Global-prefix is set
                        commonSyseventSet("lan_prefix_set", globalIP); 
                        /**
                        * Send data to wanmanager.
                        */
                        int ipv6_wan_status = 0;
                        char dns_server[256] = {'\0'};
                        dhcpv6_data_t dhcpv6_data;
                        memset(&dhcpv6_data, 0, sizeof(dhcpv6_data_t));

                        if(strlen(v6pref) == 0) {
                            dhcpv6_data.isExpired = TRUE;
                        } else {
                            dhcpv6_data.isExpired = FALSE;
                            dhcpv6_data.prefixAssigned = TRUE;
                            strncpy(dhcpv6_data.sitePrefix, v6pref, sizeof(dhcpv6_data.sitePrefix));
                            strncpy(dhcpv6_data.pdIfAddress, "", sizeof(dhcpv6_data.pdIfAddress));
                            /** DNS servers. **/
                            commonSyseventGet(SYSEVENT_FIELD_IPV6_DNS_SERVER, dns_server, sizeof(dns_server));
                            if (strlen(dns_server) != 0)
                            {
                                dhcpv6_data.dnsAssigned = TRUE;
                                sscanf (dns_server, "%s %s", dhcpv6_data.nameserver, dhcpv6_data.nameserver1);
                            }
                            dhcpv6_data.prefixPltime = 3600;
                            dhcpv6_data.prefixVltime = 3600;
                            dhcpv6_data.maptAssigned = FALSE;
                            dhcpv6_data.mapeAssigned = FALSE;
                            dhcpv6_data.prefixCmd = 0;
                        }

                        if (send_dhcp_data_to_wanmanager(&dhcpv6_data) != ANSC_STATUS_SUCCESS) {
                            CcspTraceError(("[%s-%d] Failed to send dhcpv6 data to wanmanager!!! \n", __FUNCTION__, __LINE__));
                        }
#endif
                        // not the best place to add route, just to make it work
                        // delegated prefix need to route to LAN interface
                        sprintf(cmd, "ip -6 route add %s dev %s", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
                        system(cmd);
			#ifdef _COSA_INTEL_XB3_ARM_
			memset(cmd,0,sizeof(cmd));
                        sprintf(cmd, "ip -6 route add %s dev %s table erouter", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
                        system(cmd);
			#endif
                        /* we need save this for zebra to send RA 
                           ipv6_prefix           // xx:xx::/yy
                         */
#ifndef _HUB4_PRODUCT_REQ_
                        sprintf(cmd, "sysevent set ipv6_prefix %s \n",v6pref);
                        system(cmd);
#else
                        sprintf(cmd, "sysevent set zebra-restart \n");
                        system(cmd);
#endif
                        g_dhcpv6_server_prefix_ready = TRUE;
                        CcspTraceWarning(("!run cmd1:%s", cmd));

                        CosaDmlDHCPv6sTriggerRestart(FALSE);
                        
                        /*We need get a global ip addres */
#if defined(_COSA_BCM_ARM_) || defined(INTEL_PUMA7)
                        /*this is for tchxb6*/
                        CcspTraceWarning((" %s dhcpv6_assign_global_ip to brlan0 \n", __FUNCTION__));
                        ret = dhcpv6_assign_global_ip(v6pref, "brlan0", globalIP);
#elif defined _COSA_BCM_MIPS_
                        ret = dhcpv6_assign_global_ip(v6pref, COSA_DML_DHCPV6_SERVER_IFNAME, globalIP);
#else
                        ret = dhcpv6_assign_global_ip(v6pref, "l2sd0", globalIP);
#endif
                        CcspTraceWarning(("%s: globalIP %s globalIP2 %s\n", __func__,
                            globalIP, globalIP2));
                        if ( _ansc_strcmp(globalIP, globalIP2 ) ){
                            bRestartLan = TRUE;

                            //PaM may restart. When this happen, we should not overwrite previous ipv6
                            if ( globalIP2[0] ) 
                               commonSyseventSet("lan_ipaddr_v6_prev", globalIP2);

                            _ansc_strcpy(globalIP2, globalIP);
                        }else{
								char lanrestart[8] = {0};
		    					commonSyseventGet("lan_restarted",lanrestart, sizeof(lanrestart));
								fprintf(stderr,"lan restart staus is %s \n",lanrestart);
   			       				if (strcmp("true",lanrestart) == 0)
							    	bRestartLan = TRUE;
								else
                            		bRestartLan = FALSE;
						}
                        g_dhcpv6s_refresh_count = bRestartLan;
                        CcspTraceWarning(("%s: bRestartLan %d\n", __func__, bRestartLan));

                        fprintf(stderr, "%s -- %d !!! ret:%d bRestartLan:%d %s %s \n", __FUNCTION__, __LINE__,ret,  bRestartLan,  globalIP, globalIP2);

                        if ( ret != 0 )
                        {
                            AnscTrace("error, assign global ip error.\n");
                        }else if ( bRestartLan == FALSE ){
                            AnscTrace("Same global IP, Need not restart.\n");
                        }else{
                            /* This is for IP.Interface.1. use */
                            commonSyseventSet(COSA_DML_DHCPV6S_ADDR_SYSEVENT_NAME, globalIP);

                            /*This is for brlan0 interface */
                            commonSyseventSet("lan_ipaddr_v6", globalIP);
                            _ansc_sprintf(cmd, "%d", pref_len);
                            commonSyseventSet("lan_prefix_v6", cmd);

                            commonSyseventSet("lan-restart", "1");
                        }
#endif
                    }
                }
                else if (!strncmp(action, "del", 3))
                {
                    /*todo*/
                }
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && (defined(_CBR_PRODUCT_REQ_) || defined(_BCI_FEATURE_REQ))

#else
		system("sysevent set zebra-restart");
#endif
                if (pString)
                    AnscFreeMemory(pString);                    
            }
            
        }
#ifdef _DEBUG
        else if (!strncmp(msg, "mem", 3)) 
        {
            /*add the test funcs in the run time.*/

            AnscTraceMemoryTable();
        }
#endif
    }

EXIT:
    if(fd>=0) {
        close(fd);
    }

    if(fd1>=0) {
        close(fd1);
    }

    return NULL;
}

#endif

#ifdef _HUB4_PRODUCT_REQ_
static int send_dhcp_data_to_wanmanager (dhcpv6_data_t *dhcpv6_data)
{
    int ret = ANSC_STATUS_SUCCESS;
    if ( NULL == dhcpv6_data)
    {
        printf ("[%s-%d] Invalid argument \n", __FUNCTION__,__LINE__);
        return ANSC_STATUS_FAILURE;
    }

    /*
     * Init nanomsg socket.
     */

    int sock = -1;
    int conn = -1;

    sock = nn_socket(AF_SP, NN_PUSH);
    if (sock < ANSC_STATUS_SUCCESS)
    {
        CcspTraceError(("[%s-%d] Failed to create the nanomsg socket \n", __FUNCTION__, __LINE__));
        return ANSC_STATUS_INTERNAL_ERROR;
    }

    conn = nn_connect(sock, WAN_MANAGER_ADDR);
    if (conn < ANSC_STATUS_SUCCESS)
    {
        CcspTraceError(("[%s-%d] Failed to connect to the wanmanager socket \n", __FUNCTION__, __LINE__));
        nn_close (sock);
        return ANSC_STATUS_INTERNAL_ERROR;
    }

    CcspTraceInfo(("[%s-%d] Established connection to wanmanager \n",__FUNCTION__, __LINE__));

    msg_payload_t msg;
    memset (&msg, 0, sizeof(msg_payload_t));

    /**
     * Copy dhcpv6 data.
     */
    msg.msg_type = DHCP6C_STATE_CHANGED;
    memcpy(&msg.data.dhcpv6, dhcpv6_data, sizeof(dhcpv6_data_t));

    /**
     * Send data to wanmanager.
     */
    int bytes = 0;
    int sz_msg = sizeof(msg_payload_t);


    bytes = nn_send(sock, (char *) &msg, sz_msg, 0);
    if (bytes < 0)
    {
        CcspTraceError(("[%s-%d] Failed to send data to wanmanager  error=[%d][%s] \n", __FUNCTION__, __LINE__,errno, strerror(errno)));
        nn_close (sock);
        return ANSC_STATUS_INTERNAL_ERROR;
    }

    CcspTraceInfo(("[%s-%d] Successfully send %d bytes over nano msg  \n", __FUNCTION__, __LINE__,bytes));
    nn_close (sock);
    return ret;
}
#endif
