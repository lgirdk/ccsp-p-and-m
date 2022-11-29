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
#include "cosa_apis.h"
#include "cosa_dhcpv6_apis.h"
#include "cosa_dhcpv6_internal.h"
#include "cosa_x_cisco_com_devicecontrol_apis.h"
#include "plugin_main_apis.h"
#include "autoconf.h"
#include "secure_wrapper.h"
#include "safec_lib_common.h"
#include "cosa_common_util.h"
#include <ccsp_psm_helper.h>
#include <sys/stat.h>

extern void* g_pDslhDmlAgent;
extern ANSC_HANDLE bus_handle;
extern char g_Subsystem[32];

extern int executeCmd(char *cmd);

void _get_shell_output (FILE *fp, char *buf, size_t len);
int _get_shell_output2 (FILE *fp, char *needle);

#include "ipc_msg.h"
#if defined SUCCESS
#undef SUCCESS
#endif
#define SYSEVENT_FIELD_IPV6_DNS_SERVER    "wan6_ns"
#if defined(FEATURE_RDKB_WAN_MANAGER)
/**
 * @brief API to send dhcpv6 configuration data to RdkWanmanager over nanomsg socket
 * @param Pointer to ipc_dhcpv6_data_t holds the IPv6 configurations
 * @return 0 on success else returned -1
 */
static int send_dhcp_data_to_wanmanager (ipc_dhcpv6_data_t *dhcpv6_data); /* Send data to wanmanager using nanomsg. */
#ifdef WAN_FAILOVER_SUPPORTED
pthread_t Ipv6Handle_tid;
void *Ipv6ModeHandler_thrd(void *data);
#endif
#endif//FEATURE_RDKB_WAN_MANAGER
#if defined(_HUB4_PRODUCT_REQ_) || defined(FEATURE_RDKB_WAN_MANAGER)
#define SYSEVENT_FIELD_IPV6_PREFIXVLTIME  "ipv6_prefix_vldtime"
#define SYSEVENT_FIELD_IPV6_PREFIXPLTIME  "ipv6_prefix_prdtime"
#define SYSEVENT_FIELD_IPV6_ULA_ADDRESS   "ula_address"
#endif

#if defined (_HUB4_PRODUCT_REQ_)
#define _DHCPV6_DEFAULT_STATELESS_
#endif

#if defined(RDKB_EXTENDER_ENABLED) || defined(WAN_FAILOVER_SUPPORTED)
typedef enum deviceMode
{
    DEVICE_MODE_ROUTER,
    DEVICE_MODE_EXTENDER
}deviceMode;
#endif


#if defined (INTEL_PUMA7)
//Intel Proposed RDKB Generic Bug Fix from XB6 SDK
#define NO_OF_RETRY 90  /*No of times the file will poll for TLV config file*/
#endif

#if defined _COSA_SIM_

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
    errno_t                         rc              = -1;

    if ( ulIndex >= sizeof(g_dhcpv6_client)/sizeof(COSA_DML_DHCPCV6_FULL) )
        return ANSC_STATUS_SUCCESS;

    g_dhcpv6_client[ulIndex].Cfg.InstanceNumber  = ulInstanceNumber;
    rc = STRCPY_S_NOCLOBBER(g_dhcpv6_client[ulIndex].Cfg.Alias, sizeof(g_dhcpv6_client[ulIndex].Cfg.Alias), pAlias);
    if ( rc != EOK )
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }
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
    errno_t                         rc              = -1;
    if ( ulIndex+1 > sizeof(g_dhcpv6_client_sent)/sizeof(COSA_DML_DHCPCV6_SENT) )
        return ANSC_STATUS_SUCCESS;

    g_dhcpv6_client_sent[ulIndex].InstanceNumber  = ulInstanceNumber;

    rc = STRCPY_S_NOCLOBBER(g_dhcpv6_client_sent[ulIndex].Alias, sizeof(g_dhcpv6_client_sent[ulIndex].Alias), pAlias);
    if ( rc != EOK )
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }
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
    errno_t                         rc              = -1;
    if ( ulIndex+1 > sizeof(g_dhcpv6_server_pool)/sizeof(COSA_DML_DHCPSV6_POOL_FULL) )
        return ANSC_STATUS_FAILURE;

    g_dhcpv6_server_pool[ulIndex].Cfg.InstanceNumber  = ulInstanceNumber;
    rc = STRCPY_S_NOCLOBBER(g_dhcpv6_server_pool[ulIndex].Cfg.Alias, sizeof(g_dhcpv6_server_pool[ulIndex].Cfg.Alias), pAlias);
    if ( rc != EOK )
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }
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
    errno_t                         rc              = -1;
    if ( ulIndex+1 > sizeof(g_dhcpv6_server_pool_option)/sizeof(COSA_DML_DHCPSV6_POOL_OPTION) )
        return ANSC_STATUS_FAILURE;

    g_dhcpv6_server_pool_option[ulIndex].InstanceNumber  = ulInstanceNumber;

    rc = STRCPY_S_NOCLOBBER(g_dhcpv6_server_pool_option[ulIndex].Alias, sizeof(g_dhcpv6_server_pool_option[ulIndex].Alias), pAlias);
    if ( rc != EOK )
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }
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
#include <utctx/utctx_api.h>
#include "syscfg/syscfg.h"
#include "cosa_drg_common.h"
#include "cosa_ip_apis.h"
#include "cosa_common_util.h"
#if defined (WIFI_MANAGE_SUPPORTED)
#include "cosa_managedwifi_webconfig_apis.h"
#endif /*WIFI_MANAGE_SUPPORTED*/

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
    pthread_t          dbgthrdc;
    pthread_t          dbgthrds;
}g_be_ctx;

static void * dhcpv6c_dbg_thrd(void * in);
static void * dhcpv6s_dbg_thrd(void * in);

extern COSARepopulateTableProc            g_COSARepopulateTable;

#define UTOPIA_SIMULATOR                0

enum {
    DHCPV6_SERVER_TYPE_STATEFUL  =1,
    DHCPV6_SERVER_TYPE_STATELESS = 2,
    DHCPV6_SERVER_TYPE_CONCURRENT = 3,
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

char *safe_strcpy (char *dst, char *src, size_t dst_size)
{
    size_t len;

    if (dst_size == 0)
        return dst;

    len = strlen (src);

    if (len >= dst_size)
    {
        dst[dst_size - 1] = 0;
        return memcpy (dst, src, dst_size - 1);
    }

    return memcpy (dst, src, len + 1);
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
        fprintf(fileHandle, " %s %s \n", pLink->name, pLink->value);
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
    errno_t      rc      = -1;
    
    if ( !pHead )
        utopia_init();

    UserAcquireLock(&gPsmLock);

    rc = sprintf_s(fullname, sizeof(fullname), "%s%s", domainname, pRecordName);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }

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
    errno_t  rc          = -1;
    
    if ( !pHead )
        utopia_init();

    UserAcquireLock(&gPsmLock);

    rc = sprintf_s(fullname, sizeof(fullname), "%s%s", domainname, pRecordName);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }

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
    errno_t      rc      = -1;

    if ( !pHead )
        utopia_init();

    UserAcquireLock(&gPsmLock);

    rc = sprintf_s(fullname, sizeof(fullname), "%s%s", domainname, pRecordName);
    if(rc < EOK)
    {
       ERR_CHK(rc);
    }

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
static COSA_DML_DHCPSV6_POOL_FULL          sDhcpv6ServerPool[DHCPV6S_POOL_NUM]                   = {};
static ULONG                               uDhcpv6ServerPoolOptionNum[DHCPV6S_POOL_NUM]          = {0};
static COSA_DML_DHCPSV6_POOL_OPTION        sDhcpv6ServerPoolOption[DHCPV6S_POOL_NUM][DHCPV6S_POOL_OPTION_NUM] = {};
#if defined(MULTILAN_FEATURE)
static char v6addr_prev[IPV6_PREF_MAXLEN] = {0};
#endif

BOOL  g_dhcpv6_server      = TRUE;
ULONG g_dhcpv6_server_type = DHCPV6_SERVER_TYPE_STATELESS;

BOOL g_lan_ready = FALSE;
BOOL g_dhcpv6_server_prefix_ready = FALSE;
ULONG g_dhcpv6s_restart_count = 0;
ULONG g_dhcpv6s_refresh_count = 0;

static BOOL isRaFlagSet = FALSE;

#if defined (_HUB4_PRODUCT_REQ_)
    #define DHCPV6S_SERVER_PID_FILE   "/etc/dibbler/server.pid"
#else
    #define DHCPV6S_SERVER_PID_FILE   "/tmp/dibbler/server.pid"
#endif

#define DHCPVS_DEBUG_PRINT \
fprintf(stderr,"%s -- %d !!!!!!!!!!!!!!!!!!\n", __FUNCTION__, __LINE__);

#define SETS_INTO_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, value ) \
{ \
    char Namespace[128]; \
    snprintf(Namespace, sizeof(Namespace), "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    Utopia_RawSet(&utctx, Namespace, parameter, (char *) value); \
} \

#define SETI_INTO_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, value ) \
{ \
    char Namespace[128]; \
    char Value[12]; \
    snprintf(Namespace, sizeof(Namespace), "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    snprintf(Value, sizeof(Value), "%lu", (ULONG) value ); \
    Utopia_RawSet(&utctx, Namespace, parameter, Value); \
} \

#define UNSET_INTO_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter ) \
{ \
    char Namespace[128]; \
    snprintf(Namespace, sizeof(Namespace), "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    syscfg_unset(Namespace, parameter); \
} \

#define GETS_FROM_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, out ) \
{ \
    char Namespace[128]; \
    snprintf(Namespace, sizeof(Namespace), "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    Utopia_RawGet(&utctx, Namespace, parameter, (char *) out, sizeof(out)); \
} \

#define GETI_FROM_UTOPIA( uniqueName, table1Name, table1Index, table2Name, table2Index, parameter, out ) \
{ \
    char Namespace[128]; \
    char Value[12] = {0}; \
    snprintf(Namespace, sizeof(Namespace), "%s%s%lu%s%lu", uniqueName, table1Name, (ULONG)table1Index, table2Name, (ULONG)table2Index ); \
    Utopia_RawGet(&utctx, Namespace, parameter, Value, sizeof(Value)); \
    if (strcmp(Value, "4294967295") == 0) out = -1; \
    else if (Value[0]) out = atoi(Value); \
} \


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
    CcspTraceInfo(("%s table1Name: %s, table1Index: %lu, get IANAInterfacePrefixes: %s\n",
                  __func__, table1Name, table1Index, pEntry->Info.IANAPrefixes));

    if ( _ansc_strcmp((const char*)table1Name, "pool") == 0 && table1Index == 0 &&
        _ansc_strcmp((const char*)pEntry->Info.IANAPrefixes, INVALID_IANAInterfacePrefixes) == 0)
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
    char l_cSecWebUI_Enabled[8] = {0};
    syscfg_get(NULL, "SecureWebUI_Enable", l_cSecWebUI_Enabled, sizeof(l_cSecWebUI_Enabled));
    if ( '\0' == pEntry->Cfg.X_RDKCENTRAL_COM_DNSServers[ 0 ] )
    {
        if (!strncmp(l_cSecWebUI_Enabled, "true", 4)){
            pEntry->Cfg.X_RDKCENTRAL_COM_DNSServersEnabled = 1;
        }
        else {
            pEntry->Cfg.X_RDKCENTRAL_COM_DNSServersEnabled = 0;
        }
    }


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

static COSA_DML_DHCPCV6_FULL  g_dhcpv6_client;

static int _dibbler_server_operation(char * arg);
void _cosa_dhcpsv6_refresh_config();
int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger);
#define DHCPS6V_SERVER_RESTART_FIFO "/tmp/ccsp-dhcpv6-server-restart-fifo.txt"

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(_CBR_PRODUCT_REQ_) && ! defined(_BWG_PRODUCT_REQ_) && ! defined(_BCI_FEATURE_REQ)

#else
ANSC_STATUS
CosaDmlDhcpv6SMsgHandler
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
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
    if ( !mkfifo(CCSP_COMMON_FIFO, 0666) || errno == EEXIST )
    {
        if (pthread_create(&g_be_ctx.dbgthrdc, NULL, dhcpv6c_dbg_thrd, NULL)  || pthread_detach(g_be_ctx.dbgthrdc)) 
            CcspTraceWarning(("%s error in creating dhcpv6c_dbg_thrd\n", __FUNCTION__));
    }

    /*we start a thread to hear dhcpv6 server messages */
    if ( !mkfifo(DHCPS6V_SERVER_RESTART_FIFO, 0666) || errno == EEXIST )
    {
        if (pthread_create(&g_be_ctx.dbgthrds, NULL, dhcpv6s_dbg_thrd, NULL)  || pthread_detach(g_be_ctx.dbgthrds))
            CcspTraceWarning(("%s error in creating dhcpv6s_dbg_thrd\n", __FUNCTION__));
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
    UNREFERENCED_PARAMETER(arg);
    CcspTraceWarning(("%s -- lan status is started. \n", __FUNCTION__));
    g_lan_ready = TRUE;

#if defined(_CBR_PRODUCT_REQ_)
    // TCCBR-3353: dibbler-server is failing because brlan's IP address is "tentative".
    // Add a delay so the IP address has time to become "prefered".
    sleep(2);
#endif

#ifdef RDKB_EXTENDER_ENABLED
    {
        char buf[8] ={0};
        int deviceMode = 0;
        
        memset(buf,0,sizeof(buf));
        if (0 == syscfg_get(NULL, "Device_Mode", buf, sizeof(buf)))
        {
            deviceMode = atoi(buf);
        }

        CcspTraceWarning(("%s -- %d Device Mode %d \n", __FUNCTION__, __LINE__,deviceMode));
        //dont start dibbler server in extender mode.
        if (deviceMode == DEVICE_MODE_EXTENDER)
        {
            return 0;
        }
    }
#endif

#if defined(_HUB4_PRODUCT_REQ_)
    g_dhcpv6_server_prefix_ready = TRUE; // To start dibbler server while lan-statues value is 'started'
    /* we have to start the dibbler client on Box bootup even without wan-connection
     * This needs to call dibbler-stop action, before dibbler-start */
    CosaDmlDHCPv6sTriggerRestart(FALSE);
#else
    CosaDmlDHCPv6sTriggerRestart(TRUE);
#endif

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
    UNREFERENCED_PARAMETER(hDml);
    UNREFERENCED_PARAMETER(phContext);
    UtopiaContext utctx = {0};
    ULONG         Index = 0;
    ULONG         Index2 = 0;
    DSLHDMAGNT_CALLBACK *  pEntry = NULL;
    char         value[32] = {0};
    BOOLEAN		 bIsChangesHappened = FALSE;
    errno_t     rc = -1;


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
    v_secure_system("/lib/rdk/dibbler-init.sh");
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

    if (g_dhcpv6_server == 1)
    {
        if (g_dhcpv6_server_type == DHCPV6_SERVER_TYPE_STATEFUL)
        {   
            /* DHCPv6 is used for both addresses and other configuration settings.
            This combination is known as DHCPv6 stateful, in which DHCPv6 is assigning stateful addresses to IPv6 hosts. */
            Utopia_RawSet(&utctx, NULL, "router_managed_flag", "1");
            Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "1");
            Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "0");
        }
        else if (g_dhcpv6_server_type == DHCPV6_SERVER_TYPE_STATELESS)
        {
            /* DHCPv6 is not used to assign addresses, only to assign other configuration settings.
            Neighboring routers are configured to advertise non-link-local address prefixes from which IPv6 hosts derive stateless addresses.
            This combination is known as DHCPv6 stateless: DHCPv6 is not assigning stateful addresses to IPv6 hosts, but stateless configuration settings.*/
            Utopia_RawSet(&utctx, NULL, "router_managed_flag", "0");
            Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "0");
            Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
        }
        else
        {
            /* Hosts use both stateless and stateful address configuration concurrently. */
            Utopia_RawSet(&utctx, NULL, "router_managed_flag", "1");
            Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "1");
            Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
        }
        Utopia_RawSet(&utctx, NULL, "router_other_flag", "1");
        Utopia_RawSet(&utctx, NULL, "router_other_flag_2", "1");
    }
    else
    {
        /* This combination corresponds to a network without a DHCPv6 infrastructure.
        Hosts use router advertisements for non-link-local addresses and other methods (such as manual configuration) to configure other settings. */
        Utopia_RawSet(&utctx, NULL, "router_managed_flag", "0");
        Utopia_RawSet(&utctx, NULL, "router_other_flag", "0");
        Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "0");
        Utopia_RawSet(&utctx, NULL, "router_other_flag_2", "0");
        Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
    }

    if (uDhcpv6ServerPoolNum > DHCPV6S_POOL_NUM) {
        uDhcpv6ServerPoolNum = DHCPV6S_POOL_NUM;
		bIsChangesHappened = TRUE;
    }

    /*
       The _DHCPV6_DEFAULT_STATELESS_ macro should be defined on platforms
       such as HUB4 where the default mode should be Stateless.
    */

#if !defined(_DHCPV6_DEFAULT_STATELESS_)
    /*This logic code is used to change default behavior to stateful dhcpv6 server */
    Utopia_RawGet(&utctx,NULL, "router_statefuldhcpv6_DoOnce",value,sizeof(value));
    if ( value[0]!= '1'  && g_dhcpv6_server_type == 2 )
    {
        g_dhcpv6_server_type = 1;
        Utopia_RawSet(&utctx,NULL,"router_other_flag","1");
        Utopia_RawSet(&utctx,NULL,"router_managed_flag","1");
        Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "0");
        SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "servertype", g_dhcpv6_server_type)

        v_secure_system("sysevent set zebra-restart");
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
        getpool_from_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", Index, &sDhcpv6ServerPool[Index]);

        if (!Utopia_Init(&utctx))
            return ANSC_STATUS_FAILURE;
        GETI_FROM_UTOPIA(DHCPV6S_NAME,  "pool", Index, "", 0, "optionnumber", uDhcpv6ServerPoolOptionNum[Index])
        Utopia_Free(&utctx,0);

        for( Index2 = 0; (Index2 < uDhcpv6ServerPoolOptionNum[Index]) && (Index2 < DHCPV6S_POOL_OPTION_NUM); Index2++ )
        {
            getpooloption_from_utopia((PUCHAR)DHCPV6S_NAME,(PUCHAR)"pool",Index,(PUCHAR)"option",Index2,&sDhcpv6ServerPoolOption[Index][Index2]);
        }
    }

    if ( uDhcpv6ServerPoolNum == 0 )
    {
        uDhcpv6ServerPoolNum = 1;

        AnscZeroMemory( &sDhcpv6ServerPool[0], sizeof(sDhcpv6ServerPool[0]));
        sDhcpv6ServerPool[0].Cfg.InstanceNumber = 1;
        rc = sprintf_s((char*)sDhcpv6ServerPool[0].Cfg.Alias, sizeof(sDhcpv6ServerPool[0].Cfg.Alias), "Pool%d", 1);
        if(rc < EOK)
        {
            ERR_CHK(rc);
        }
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

        setpool_into_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", 0, &sDhcpv6ServerPool[0]);
    }

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "serverenable", g_dhcpv6_server)
    Utopia_Free(&utctx,1);

#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(_CBR_PRODUCT_REQ_) && ! defined(_BWG_PRODUCT_REQ_) && ! defined(_BCI_FEATURE_REQ) 

#else

    /*register callback function to handle message from wan dchcp6 client */
    pEntry = (PDSLHDMAGNT_CALLBACK)AnscAllocateMemory(sizeof(*pEntry));
    if(pEntry == NULL)
    {
	CcspTraceWarning(("%s -- %d pEntry memory allocation error \n", __FUNCTION__, __LINE__));
        return ANSC_STATUS_FAILURE;
    }
    pEntry->func = CosaDmlDhcpv6SMsgHandler;
    g_RegisterCallBackAfterInitDml(g_pDslhDmlAgent, pEntry);

    /*register callback function to restart dibbler-server at right time*/
    CcspTraceWarning(("%s -- %d register lan-status to event dispatcher \n", __FUNCTION__, __LINE__));
    EvtDispterRgstCallbackForEvent("lan-status", CosaDmlDhcpv6sRestartOnLanStarted, NULL);
#ifdef _HUB4_PRODUCT_REQ_
    CcspTraceWarning(("%s -- %d register dibblerServer-restart to event dispatcher \n", __FUNCTION__, __LINE__));
    EvtDispterRgstCallbackForEvent("dibblerServer-restart", CosaDmlDhcpv6sRestartOnLanStarted, NULL);
#endif

#endif

#if defined(FEATURE_RDKB_WAN_MANAGER)
#if defined (WAN_FAILOVER_SUPPORTED) && !defined (RDKB_EXTENDER_ENABLED)
     pthread_create(&Ipv6Handle_tid, NULL, Ipv6ModeHandler_thrd, NULL);
#endif
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
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext); 
    UtopiaContext utctx = {0};
    char buf[256] = {0};
    char out[256] = {0};
    errno_t rc = -1;

    if (ulIndex)
        return ANSC_STATUS_FAILURE;
    
    pEntry->Cfg.InstanceNumber = 1;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    /*Cfg members*/
    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_alias");
    ERR_CHK(rc);
    memset(pEntry->Cfg.Alias, 0, sizeof(pEntry->Cfg.Alias));
    Utopia_RawGet(&utctx,NULL,buf,(char*)pEntry->Cfg.Alias,sizeof(pEntry->Cfg.Alias));

    pEntry->Cfg.SuggestedT1 = pEntry->Cfg.SuggestedT2 = 0;
    
    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_t1");
    ERR_CHK(rc);
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    sscanf(out, "%lu", &pEntry->Cfg.SuggestedT1);

    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_t2");
    ERR_CHK(rc);
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    sscanf(out, "%lu", &pEntry->Cfg.SuggestedT2);

    /*pEntry->Cfg.Interface stores interface name, dml will calculate the full path name*/
    rc = strcpy_s((char*)pEntry->Cfg.Interface, sizeof(pEntry->Cfg.Interface), COSA_DML_DHCPV6_CLIENT_IFNAME);
    ERR_CHK(rc);
    
    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_requested_options");
    ERR_CHK(rc);
    memset(pEntry->Cfg.RequestedOptions, 0, sizeof(pEntry->Cfg.RequestedOptions));
    Utopia_RawGet(&utctx,NULL,buf,(char*)pEntry->Cfg.RequestedOptions,sizeof(pEntry->Cfg.RequestedOptions));

    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_enabled");
    ERR_CHK(rc);
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    pEntry->Cfg.bEnabled = (out[0] == '1') ? TRUE:FALSE;

    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_iana_enabled");
    ERR_CHK(rc);
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    pEntry->Cfg.RequestAddresses = (out[0] == '1') ? TRUE:FALSE;
    
    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_iapd_enabled");
    ERR_CHK(rc);
    memset(out, 0, sizeof(out));
    Utopia_RawGet(&utctx,NULL,buf,out,sizeof(out));
    pEntry->Cfg.RequestPrefixes = (out[0] == '1') ? TRUE:FALSE;

    rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_rapidcommit_enabled");
    ERR_CHK(rc);
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
           
            rc = STRCPY_S_NOCLOBBER((char*)pEntry->Cfg.Alias, sizeof(pEntry->Cfg.Alias), "Client1");
            ERR_CHK(rc);
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    UtopiaContext utctx = {0};
    errno_t rc = -1;

    if (ulIndex)
        return ANSC_STATUS_FAILURE;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    


    rc = strcpy_s((char*)g_dhcpv6_client.Cfg.Alias, sizeof(g_dhcpv6_client.Cfg.Alias), pAlias);
    ERR_CHK(rc);
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
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
                snprintf(line, sizeof(line), "    t1 %lu\n", pCfg->SuggestedT1);
                fprintf(fp, "%s", line);
            }

            if (pCfg->SuggestedT2)
            {
                snprintf(line, sizeof(line), "    t2 %lu\n", pCfg->SuggestedT2);
                fprintf(fp, "%s", line);
            }

            fprintf(fp, "    }\n");
        }

        if (pCfg->RequestPrefixes)
        {
            fprintf(fp, "    pd {\n");

            if (pCfg->SuggestedT1)
            {
                snprintf(line, sizeof(line), "    t1 %lu\n", pCfg->SuggestedT1);
                fprintf(fp, "%s", line);
            }

            if (pCfg->SuggestedT2)
            {
                snprintf(line, sizeof(line), "    t2 %lu\n", pCfg->SuggestedT2);
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

static int _dibbler_client_operation(char * arg)
{
#if defined(INTEL_PUMA7) || defined(_COSA_BCM_ARM_)
    FILE *fp = NULL;
    char out[256];
#endif
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
#elif defined(CORE_NET_LIB)
        v_secure_system("/usr/bin/service_dhcpv6_client dhcpv6_client_service_disable");
        CcspTraceInfo(("%s  Calling service_dhcpv6_client.c with dhcpv6_client_service_disable from cosa_dhcpv6_apis.c\n", __func__));
#else
        v_secure_system("/etc/utopia/service.d/service_dhcpv6_client.sh disable");
#endif
      
#ifdef _COSA_BCM_ARM_
        v_secure_system("killall " CLIENT_BIN);
        sleep(2);
        v_secure_system("killall -9 " CLIENT_BIN);
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
         fp = v_secure_popen("r", "sysevent get TLV202-status");
         _get_shell_output(fp, out, sizeof(out));
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
        fp = v_secure_popen("r","syscfg get last_erouter_mode");
        _get_shell_output(fp, out, sizeof(out));
	/* TODO: To be fixed by Comcast
	         IPv6 address assigned to erouter0 gets deleted when erouter_mode=3(IPV4 and IPV6 both)
	         Don't start v6 service in parallel. Wait for wan-status to be set to 'started' by IPv4 DHCP client.
	*/
        if (strstr(out, "3"))// If last_erouter_mode is both IPV4/IPV6
	{
	     	do{
                        fp = v_secure_popen("r","sysevent get wan-status");
                        _get_shell_output(fp, out, sizeof(out));
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
#elif defined(CORE_NET_LIB)
    v_secure_system("/usr/bin/service_dhcpv6_client dhcpv6_client_service_enable");
    CcspTraceInfo(("%s  Calling service_dhcpv6_client.c with dhcpv6_client_service_enable from cosa_dhcpv6_apis.c\n", __func__));
#else
    v_secure_system("/etc/utopia/service.d/service_dhcpv6_client.sh enable");
#endif
#ifdef _COSA_BCM_ARM_
        /* Dibbler-init is called to set the pre-configuration for dibbler */            
        CcspTraceInfo(("%s dibbler-init.sh Called \n", __func__));
        v_secure_system("/lib/rdk/dibbler-init.sh");
        /*Start Dibber client for tchxb6*/
        CcspTraceInfo(("%s Dibbler Client Started \n", __func__));
        v_secure_system(CLIENT_BIN " start");
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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    char buf[256] = {0};
    char out[256] = {0};
    int  need_to_restart_service = 0;
    errno_t rc = -1;

    if (pCfg->InstanceNumber != 1)
        return ANSC_STATUS_FAILURE;

    /*handle syscfg*/
    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;


    if (!AnscEqualString((char*)pCfg->Alias, (char*)g_dhcpv6_client.Cfg.Alias, TRUE))
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_alias");
        ERR_CHK(rc);
        Utopia_RawSet(&utctx,NULL,buf,(char*)pCfg->Alias);
    }

    if (pCfg->SuggestedT1 != g_dhcpv6_client.Cfg.SuggestedT1)
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_t1");
        ERR_CHK(rc);
        rc = sprintf_s(out, sizeof(out), "%lu", pCfg->SuggestedT1);
        if(rc < EOK)
        {
            ERR_CHK(rc);
        }
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }

    if (pCfg->SuggestedT2 != g_dhcpv6_client.Cfg.SuggestedT2)
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_t2");
        ERR_CHK(rc);
        rc = sprintf_s(out, sizeof(out), "%lu", pCfg->SuggestedT2);
        if(rc < EOK)
        {
            ERR_CHK(rc);
        }
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }

    if (!AnscEqualString((char*)pCfg->RequestedOptions, (char*)g_dhcpv6_client.Cfg.RequestedOptions, TRUE))
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_requested_options");
        ERR_CHK(rc);
        Utopia_RawSet(&utctx,NULL,buf,(char*)pCfg->RequestedOptions);
        need_to_restart_service = 1;
    }

    if (pCfg->bEnabled != g_dhcpv6_client.Cfg.bEnabled)
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_enabled");
        ERR_CHK(rc);
        out[0] = pCfg->bEnabled ? '1':'0';
        out[1] = 0;
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }

    if (pCfg->RequestAddresses != g_dhcpv6_client.Cfg.RequestAddresses)
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_iana_enabled");
        ERR_CHK(rc);
        out[0] = pCfg->RequestAddresses ? '1':'0';
        out[1] = 0;
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }    

    if (pCfg->RequestPrefixes != g_dhcpv6_client.Cfg.RequestPrefixes)
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_iapd_enabled");
        ERR_CHK(rc);
        out[0] = pCfg->RequestPrefixes ? '1':'0';
        out[1] = 0;
        Utopia_RawSet(&utctx,NULL,buf,out);
        need_to_restart_service = 1;
    }    

    if (pCfg->RapidCommit != g_dhcpv6_client.Cfg.RapidCommit)
    {
        rc = strcpy_s(buf, sizeof(buf), SYSCFG_FORMAT_DHCP6C"_rapidcommit_enabled");
        ERR_CHK(rc);
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
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext);
    BOOL bEnabled = FALSE;
    char out[256] = {0};

#if defined (FEATURE_RDKB_WAN_MANAGER)
    FILE *fp = popen("busybox ps |grep -i dibbler-client | grep -v grep", "r");
#elif defined (_HUB4_PRODUCT_REQ_)
    FILE *fp = popen("busybox ps |grep -i dhcp6c | grep -v grep", "r");
#elif defined (_COSA_BCM_ARM_)
    FILE *fp = popen("busybox ps |grep -i dibbler-client | grep -v grep", "r");
#elif defined (_XF3_PRODUCT_REQ_)
    FILE *fp = popen("/usr/sbin/dibbler-client status | grep client", "r");
#else
    FILE *fp = popen("busybox ps |grep -i dibbler-client | grep -v grep", "r");
#endif

    if ( fp != NULL){
        if ( fgets(out, sizeof(out), fp) != NULL ){
#if defined (FEATURE_RDKB_WAN_MANAGER)
            if (strstr(out, "dibbler-client"))
                bEnabled = TRUE;
#elif defined (_HUB4_PRODUCT_REQ_)
            if (strstr(out, "dhcp6c"))
                bEnabled = TRUE;
#elif defined (_COSA_BCM_ARM_)
            if (strstr(out, "dibbler-client"))
                bEnabled = TRUE;
#elif defined (_XF3_PRODUCT_REQ_)
            if (strstr(out, "RUNNING,"))
                bEnabled = TRUE;
#else
            if (strstr(out, "dibbler-client"))
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
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    PCOSA_DML_DHCPCV6_FULL            pDhcpc            = (PCOSA_DML_DHCPCV6_FULL)hContext;

    _get_client_duid(g_dhcpv6_client.Info.DUID, sizeof(pInfo->DUID));

    if (pDhcpc)
    {
        /* We are reading the current value of dhcpv6 enable and updating the Status value */
        if (CosaDmlDhcpv6cGetEnabled(NULL))
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulClientInstanceNumber);
    FILE * fp = fopen(CLIENT_SERVER_INFO_FILE, "r+");

    *pSize = 0;
    if (fp)
    {
        char buf[1024];
        char val[1024];
        int  entry_count = 0;
        errno_t rc = -1;

        /*we only support one entry of ServerCfg*/
        *ppCfg = (PCOSA_DML_DHCPCV6_SVR)AnscAllocateMemory( sizeof(COSA_DML_DHCPCV6_SVR) );
        
        /*InformationRefreshTime not supported*/
        rc = strcpy_s((char*)(*ppCfg)->InformationRefreshTime, sizeof((*ppCfg)->InformationRefreshTime), "0001-01-01T00:00:00Z");
        ERR_CHK(rc);
        
        while(fgets(buf, sizeof(buf), fp))
        {
            if (sscanf(buf, "addr %s", val) == 1)
            {
                rc = strncpy_s((char*)(*ppCfg)->SourceAddress, sizeof((*ppCfg)->SourceAddress), val, sizeof((*ppCfg)->SourceAddress) - 1);
                ERR_CHK(rc);
                entry_count |= 1;
            }
            else if (sscanf(buf, "duid %s", val) == 1)
            {
                unsigned int i = 0, j = 0;
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    
    v_secure_system("killall -SIGUSR2 " CLIENT_BIN);

    return ANSC_STATUS_SUCCESS;
}

/*
 *  DHCP Client Send/Req Option
 *
 *  The options are managed on top of a DHCP client,
 *  which is identified through pClientAlias
 */
#define SYSCFG_DHCP6C_SENT_OPTION_FORMAT "tr_dhcp6c_sent_option_%lu"
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulClientInstanceNumber);
    UtopiaContext utctx = {0};
    char out[256];
    
    if (!Utopia_Init(&utctx))
        return 0;
    
    out[0] = 0;
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
                fprintf(fp, "%lu:%d:%s\n", 
                        g_sent_options[i].Tag,
                        strlen((const char*)g_sent_options[i].Value)/2,
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulClientInstanceNumber);
    UtopiaContext utctx = {0};
    char out[256];
    char namespace[256];
    errno_t rc = -1;

    if ( (int)ulIndex > g_sent_option_num - 1  || !g_sent_options)
        return ANSC_STATUS_FAILURE;
    
    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    /*note in syscfg, sent_options start from 1*/
    rc = sprintf_s(namespace, sizeof(namespace), SYSCFG_DHCP6C_SENT_OPTION_FORMAT, ulIndex+1);
    if(rc < EOK)
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }

    out[0] = 0;
    Utopia_RawGet(&utctx, namespace, "inst_num" ,out,sizeof(out));
    sscanf(out, "%lu", &pEntry->InstanceNumber);

    Utopia_RawGet(&utctx, namespace, "alias" ,(char*)pEntry->Alias, sizeof(pEntry->Alias));

    out[0] = 0;
    Utopia_RawGet(&utctx, namespace, "enabled" ,out,sizeof(out));
    pEntry->bEnabled = (out[0] == '1') ? TRUE:FALSE;

    out[0] = 0;
    Utopia_RawGet(&utctx, namespace, "tag" ,out,sizeof(out));
    sscanf(out, "%lu", &pEntry->Tag);

    Utopia_RawGet(&utctx, namespace, "value" ,(char*)pEntry->Value, sizeof(pEntry->Value));
    
    Utopia_Free(&utctx, 0);

    AnscCopyMemory( &g_sent_options[ulIndex], pEntry, sizeof(COSA_DML_DHCPCV6_SENT));

    /*we only wirte to file when obtaining the last entry*/
    if ((int)ulIndex == g_sent_option_num-1)
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulClientInstanceNumber); 
    int                           index  = 0;

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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    char out[16];
    char namespace[256];
    errno_t rc = -1;

    if (ulClientInstanceNumber != g_dhcpv6_client.Cfg.InstanceNumber)
        return ANSC_STATUS_FAILURE;

    if ( (int)ulIndex+1 > g_sent_option_num )
        return ANSC_STATUS_SUCCESS;

    g_sent_options[ulIndex].InstanceNumber  = ulInstanceNumber;

    rc = STRCPY_S_NOCLOBBER((char*)g_sent_options[ulIndex].Alias, sizeof(g_sent_options[ulIndex].Alias), pAlias);
    if ( rc != EOK )
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }
    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    rc = sprintf_s(namespace, sizeof(namespace), SYSCFG_DHCP6C_SENT_OPTION_FORMAT, ulIndex+1);
    if(rc < EOK)
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }
    snprintf(out, sizeof(out), "%lu", ulInstanceNumber);
    Utopia_RawSet(&utctx, namespace, "inst_num", out);

    Utopia_RawSet(&utctx, namespace, "alias" ,pAlias);

    Utopia_Free(&utctx, 1);    

    return ANSC_STATUS_SUCCESS;
}

static int _syscfg_add_sent_option(PCOSA_DML_DHCPCV6_SENT      pEntry, int index)
{
    UtopiaContext utctx = {0};
    char out[16];
    char namespace[256];
    errno_t rc = -1;

    if (!pEntry)
        return -1;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;

    rc = sprintf_s(namespace, sizeof(namespace), SYSCFG_DHCP6C_SENT_OPTION_FORMAT, (ULONG)index);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }
    snprintf(out, sizeof(out), "%lu", pEntry->InstanceNumber);
    Utopia_RawSet(&utctx, namespace, "inst_num", out );

    Utopia_RawSet(&utctx, namespace, "alias" ,(char*)pEntry->Alias);

    rc = strcpy_s(out, sizeof(out), ((pEntry->bEnabled) ? "1" : "0"));
    ERR_CHK(rc);
    Utopia_RawSet(&utctx, namespace, "enabled" ,out);

    snprintf(out, sizeof(out), "%lu", pEntry->Tag);
    Utopia_RawSet(&utctx, namespace, "tag" , out);

    Utopia_RawSet(&utctx, namespace, "value" ,(char*)pEntry->Value);
    
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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    char out[16];
    struct _COSA_DML_DHCPCV6_SENT *realloc_tmp;

    /*we only have one client*/
    if (ulClientInstanceNumber != g_dhcpv6_client.Cfg.InstanceNumber)
        return ANSC_STATUS_FAILURE;

    realloc_tmp = realloc(g_sent_options, (++g_sent_option_num)* sizeof(COSA_DML_DHCPCV6_SENT));
    if (!realloc_tmp)
    {
        return ANSC_STATUS_FAILURE;
    }
    else
    {
        g_sent_options = realloc_tmp;
    }

    _syscfg_add_sent_option(pEntry, g_sent_option_num);    

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    snprintf(out, sizeof(out), "%d", g_sent_option_num);
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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    char out[16];
    char namespace[256];
    int  i = 0;
    int  j = 0;
    int  saved_enable = 0;
    errno_t rc = -1;

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
    rc = sprintf_s(namespace, sizeof(namespace), SYSCFG_DHCP6C_SENT_OPTION_FORMAT, (ULONG)(g_sent_option_num+1));
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }
    syscfg_unset(namespace, "inst_num");
    syscfg_unset(namespace, "alias");
    syscfg_unset(namespace, "enabled");
    syscfg_unset(namespace, "tag");
    syscfg_unset(namespace, "value");

    snprintf(out, sizeof(out), "%d", g_sent_option_num);
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulClientInstanceNumber);
    ULONG                           index  = 0;
    UtopiaContext                   utctx = {0};
    char                            out[16];
    char                            namespace[256];
    int                             need_restart_service = 0;
    PCOSA_DML_DHCPCV6_SENT          p_old_entry = NULL;
    errno_t                         rc = -1;

    for( index=0;  (int)index<g_sent_option_num; index++)
    {
        if ( pEntry->InstanceNumber == g_sent_options[index].InstanceNumber )
        {
            p_old_entry = &g_sent_options[index];

            if (!Utopia_Init(&utctx))
                return ANSC_STATUS_FAILURE;

            /*handle syscfg*/
            rc = sprintf_s(namespace, sizeof(namespace), SYSCFG_DHCP6C_SENT_OPTION_FORMAT, index+1);
            if(rc < EOK)
            {
                ERR_CHK(rc);
            }
    
            if (!AnscEqualString((char*)pEntry->Alias, (char*)p_old_entry->Alias, TRUE))
                Utopia_RawSet(&utctx, namespace, "alias" ,(char*)pEntry->Alias);                

            if (pEntry->bEnabled != p_old_entry->bEnabled)
            {
                rc = strcpy_s(out, sizeof(out), ((pEntry->bEnabled) ? "1" : "0"));
                ERR_CHK(rc);
                Utopia_RawSet(&utctx, namespace, "enabled" ,out);                

                need_restart_service = 1;
            }
                

            if (pEntry->Tag != p_old_entry->Tag)
            {
                snprintf(out, sizeof(out), "%lu", pEntry->Tag);
                Utopia_RawSet(&utctx, namespace, "tag" , out);

                need_restart_service = 1;
            }

            if (!AnscEqualString((char*)pEntry->Value, (char*)p_old_entry->Value, TRUE))
            {
                Utopia_RawSet(&utctx, namespace, "value" ,(char*)pEntry->Value);

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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulClientInstanceNumber); 
    FILE *                          fp = fopen(CLIENT_RCVED_OPTIONS_FILE, "r+");
    SLIST_HEADER                    option_list;
    COSA_DML_DHCPCV6_RECV *         p_rcv = NULL;
    char                            buf[1024];
    PSINGLE_LINK_ENTRY              pSLinkEntry       = NULL;
    ULONG                           ulIndex = 0;

    AnscSListInitializeHeader( &option_list );
    
    if (fp)
    {
        while (fgets(buf, sizeof(buf), fp))
        {
            int  opt_len = 0;
            char * p = NULL;
            
            p_rcv = (COSA_DML_DHCPCV6_RECV * )AnscAllocateMemory(sizeof(*p_rcv));
            if (!p_rcv)
                break;

            if (sscanf(buf, "%d %lu", &opt_len, &p_rcv->Tag) != 2)
            {
                AnscFreeMemory(p_rcv); /*RDKB-6780, CID-33399, free unused resource*/
                p_rcv = NULL;
                continue;
            }

            p_rcv->Opt_len = opt_len;
            /*don't trust opt_len record in file, calculate by self*/
            if ((p = strrchr(buf, ' ')))
            {
                p++;
                if ((unsigned int)opt_len*2 < (strlen(buf)- (p-buf)))
                    opt_len = (strlen(buf)- (p-buf))/2;
            }
            else 
            {
                AnscFreeMemory(p_rcv); /*RDKB-6780, CID-33399, free unused resource*/
                p_rcv = NULL;
                continue;
            }

            if ((unsigned int)opt_len*2 >= sizeof(p_rcv->Value))
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
            safe_strcpy((char*)p_rcv->Server, "Device.DHCPv6.Client.1.Server.1", sizeof(p_rcv->Server));
            AnscSListPushEntryAtBack(&option_list, &p_rcv->Link);
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

	/* CID 75064 fix */
        AnscCopyMemory( *ppEntry + ulIndex, p_rcv, (sizeof(*p_rcv)-1) );
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

int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger)
{
    int fd = 0;
    char str[32] = "restart";
    UtopiaContext utctx = {0};
    
    DHCPVS_DEBUG_PRINT
  #if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && ! defined(DHCPV6_PREFIX_FIX) 
    UNREFERENCED_PARAMETER(OnlyTrigger);
    commonSyseventSet("dhcpv6_server-restart", "");
  #else

    if (isRaFlagSet == FALSE)
    {
        if (Utopia_Init(&utctx))
        {
            // update managed flag based on the type
            if (g_dhcpv6_server_type == DHCPV6_SERVER_TYPE_STATEFUL)
            {
                Utopia_RawSet(&utctx, NULL, "router_managed_flag", "1");
                Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "1");
                Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "0");
            }
            else if (g_dhcpv6_server_type == DHCPV6_SERVER_TYPE_STATELESS)
            {
                Utopia_RawSet(&utctx, NULL, "router_managed_flag", "0");
                Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "0");
                Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
            }
            else
            {
                Utopia_RawSet(&utctx, NULL, "router_managed_flag", "1");
                Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "1");
                Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
            }
            Utopia_RawSet(&utctx, NULL, "router_other_flag", "1");
            Utopia_RawSet(&utctx, NULL, "router_other_flag_2", "1");
            Utopia_Free(&utctx, 1);
         }
    }

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

void CosaDmlDhcpv6sRestartOnRaChanged(ULONG dhcpv6ServerType)
{
    g_dhcpv6_server_type = dhcpv6ServerType;
    UtopiaContext utctx  = {0};

    /*Set the dhcpv6 server type*/
    if(!Utopia_Init(&utctx))
        return;

    SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "servertype", g_dhcpv6_server_type)
    Utopia_Free(&utctx,1);

    isRaFlagSet = TRUE;
    CosaDmlDHCPv6sTriggerRestart(FALSE);
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
    ULONG Index  = 0;
    int fd = 0;
    struct stat filestat;
    UtopiaContext utctx = {0};
    
    CcspTraceInfo(("%s:%d\n",__FUNCTION__, __LINE__));
    if (!strncmp(arg, "stop", 4))
    {
        /*stop the process only if it is started*/
        #if defined (_HUB4_PRODUCT_REQ_)
            if ( !g_dhcpv6_server_started )
                goto EXIT;
        #endif
        fd = open(DHCPV6S_SERVER_PID_FILE, O_RDONLY);
        if (fd >= 0) {
            CcspTraceInfo(("%s:%d stop dibbler.\n",__FUNCTION__, __LINE__));
            //fprintf(stderr, "%s -- %d stop\n", __FUNCTION__, __LINE__);
	    //dibbler stop seems to be hanging intermittently with v_secure_system
	    //replacing with system call
            //v_secure_system(SERVER_BIN " stop >/dev/null");
	    snprintf(cmd,sizeof(cmd), "%s stop >/dev/null", SERVER_BIN);
	    executeCmd(cmd);

            if (isRaFlagSet == TRUE)
            {
                /* RA Managed/OtherConfig flags are already set, just restart zebra service. */
                system("sysevent set zebra-restart");
            }
            else
            {
                if (Utopia_Init(&utctx))
                {
                    Utopia_RawSet(&utctx, NULL, "router_managed_flag", "0");
                    Utopia_RawSet(&utctx, NULL, "router_other_flag", "0");
                    Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
                    Utopia_RawSet(&utctx, NULL, "router_managed_flag_2", "0");
                    Utopia_RawSet(&utctx, NULL, "router_other_flag_2", "0");
                    Utopia_Free(&utctx, 1);

                    system("sysevent set zebra-restart");
                }
            }

            close(fd);
        }else{
            //this should not happen.
            CcspTraceInfo(("%s:%d No PID server is not running.\n",__FUNCTION__, __LINE__));
            //fprintf(stderr, "%s -- %d server is not running. \n", __FUNCTION__, __LINE__);
        }
    }
    else if (!strncmp(arg, "start", 5))
    {
        /* We need judge current config file is right or not.
                    * There is not interface enabled. Not start
                    * There is not valid pool. Not start. 
                */
        CcspTraceInfo(("Dibbler Server Start %s Line (%d)\n", __FUNCTION__, __LINE__));
        if ( !g_dhcpv6_server )
        {
            goto EXIT;
        }

        for ( Index = 0; Index < uDhcpv6ServerPoolNum; Index++ )
        {
            if ( sDhcpv6ServerPool[Index].Cfg.bEnabled ) 
                if ( ( !sDhcpv6ServerPool[Index].Info.IANAPrefixes[0] ) && ( !sDhcpv6ServerPool[Index].Info.IAPDPrefixes[0] ) )
                    break;
        }
        if ( Index < uDhcpv6ServerPoolNum )
        {
            goto EXIT;
        }
#ifdef FEATURE_RDKB_WAN_MANAGER
        char prefix[64] = {0};
        commonSyseventGet("ipv6_prefix", prefix, sizeof(prefix));
        if (strlen(prefix) > 3)
        {
            g_dhcpv6_server_prefix_ready = TRUE;
        }
#endif	
        if (g_dhcpv6_server_prefix_ready && g_lan_ready)
        {
            CcspTraceInfo(("%s:%d start dibbler %d\n",__FUNCTION__, __LINE__,g_dhcpv6_server));
            //fprintf(stderr, "%s -- %d start %d\n", __FUNCTION__, __LINE__, g_dhcpv6_server);

            if (stat("/tmp/dibbler", &filestat) < 0)
            {
                /* If the path doesn't exist, create it */
                if (errno == ENOENT)
                    mkdir("/tmp/dibbler", 0755);
            }

            #if defined (_HUB4_PRODUCT_REQ_)
                g_dhcpv6_server_started = TRUE;
            #endif

            //v_secure_system(SERVER_BIN " start");
	    snprintf(cmd,sizeof(cmd), "%s start", SERVER_BIN);
	    executeCmd(cmd);

            commonSyseventSet("lan-restart", "1");/*Trigger the lan_handler.sh to configure the global IPv6 addres for brlan0*/
            if (isRaFlagSet == TRUE)
            {
                /* RA Managed/OtherConfig flags are already set, just restart zebra service. */
                system("sysevent set zebra-restart");
            }
            else
            {
                if (Utopia_Init(&utctx))
                {
                    if (g_dhcpv6_server_type == DHCPV6_SERVER_TYPE_STATEFUL)
                    {
                        Utopia_RawSet(&utctx, NULL, "router_managed_flag", "1");
                        Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "0");
                    }
                    else if (g_dhcpv6_server_type == DHCPV6_SERVER_TYPE_STATELESS)
                    {
                        Utopia_RawSet(&utctx, NULL, "router_managed_flag", "0");
                        Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
                    }
                    else
                    {
                        Utopia_RawSet(&utctx, NULL, "router_managed_flag", "1");
                        Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");
                    }
                    Utopia_RawSet(&utctx, NULL, "router_other_flag", "1");
                    Utopia_Free(&utctx, 1);
                    system("sysevent set zebra-restart");
                }
            }
        }
    }
    else if (!strncmp(arg, "restart", 7))
    {
        CcspTraceInfo(("%s:%d restart dibbler.\n",__FUNCTION__, __LINE__));
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
    static char     newString[256];
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

static int format_dibbler_option(char *option)
{
    if (option == NULL)
        return -1;

    unsigned int i;
    size_t len = strlen(option);

    for (i = 0; i < len; i++) {
        if(option[i] == ' ')
            option[i] = ',';
    }

    return 0;
}

int remove_single_quote (char *buf)
{
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



#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_COSA_BCM_MIPS_)
// adding new logics to handle pd-class
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
    char active_insts[32];
    char lan_pd_if[128];
    char *p = NULL;
    int i = 0;
    char if_name[16];
    char buf[64];
    char *st = NULL;
    syscfg_get(NULL, "lan_pd_interfaces", lan_pd_if, sizeof(lan_pd_if));

    if (lan_pd_if[0] == '\0') {
        *num = 0;
        return *num;
    }

    active_insts[0] = 0;
    commonSyseventGet("multinet-instances", active_insts, sizeof(active_insts));

    p = strtok_r(active_insts, " ", &st);

    while (p != NULL) {
        snprintf(buf, sizeof(buf), "multinet_%s-name", p);
        if_name[0] = 0;
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
    unsigned int        i;
    ipv6_prefix_t       *p_prefix = NULL;
    int                 bit_boundary = 0;
    unsigned long long  sub_prefix, tmp_prefix; //64 bits
    char                iface_prefix[INET6_ADDRSTRLEN]; //for iface prefix str
    char                evt_name[64];
    char                evt_val[64];
    char                iface_name[64];
    ULONG               used_sub_prefix_num = 0;
    char                ipv6_prefix[64] = {0};
    int                 tpmod;

    commonSyseventSet("ipv6_prefix-divided", "");
    commonSyseventGet(COSA_DML_DHCPV6C_PREF_SYSEVENT_NAME, ipv6_prefix, sizeof(ipv6_prefix));

    if (get_prefix_info(ipv6_prefix, mso_prefix.value, sizeof(mso_prefix.value), (unsigned int*)&mso_prefix.len) != 0) {
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

    if (enabled_iface_num > (ULONG)(1 << delta_bits)) {
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
        CcspTraceInfo(("[%s] added one to used_sub_prefix_num: %lu\n",
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
    errno_t rc = -1;

    commonSyseventGet("ipv6_subprefix-start", evt_val, sizeof(evt_val));
    CcspTraceInfo(("[%s] commonSyseventGet ipv6_subprefix-start: %s\n", __FUNCTION__, evt_val));
    if (evt_val[0] != '\0') {
      remove_single_quote(evt_val);
      rc = STRCPY_S_NOCLOBBER(pool->start, sizeof(pool->start), evt_val);
      ERR_CHK(rc);
      CcspTraceInfo(("[%s] pool->start: %s\n", __FUNCTION__, pool->start));
    }
    else  {
      CcspTraceWarning(("[%s] sysevent_get for ipv6_subprefix-start is NULL\n",  __FUNCTION__));
      return -1;
    }

    commonSyseventGet("ipv6_subprefix-end", evt_val, sizeof(evt_val));
    CcspTraceInfo(("[%s] commonSyseventGet ipv6_subprefix-end: %s\n", __FUNCTION__, evt_val));
    if (evt_val[0] != '\0') {
      remove_single_quote(evt_val);
      rc = STRCPY_S_NOCLOBBER(pool->end, sizeof(pool->end), evt_val);
      ERR_CHK(rc);
      CcspTraceInfo(("[%s] pool->end: %s\n", __FUNCTION__, pool->end));
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
    errno_t rc = -1;
    if(iapd == NULL)
      return -1;

    commonSyseventGet(COSA_DML_DHCPV6C_PREF_T1_SYSEVENT_NAME, evt_val, sizeof(evt_val));
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
      {
        rc = STRCPY_S_NOCLOBBER(iapd->t1, sizeof(iapd->t1), "0");
        ERR_CHK(rc);
      }
      else
      {
        rc = STRCPY_S_NOCLOBBER(iapd->t1, sizeof(iapd->t1), strtok_r (evt_val,"'", &st));
        ERR_CHK(rc);
      }
    }
    CcspTraceInfo(("[%s] iapd->t1: %s\n", __FUNCTION__, iapd->t1));

    commonSyseventGet(COSA_DML_DHCPV6C_PREF_T2_SYSEVENT_NAME, evt_val, sizeof(evt_val));
    st = NULL;
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
      {
        rc = STRCPY_S_NOCLOBBER(iapd->t2, sizeof(iapd->t2), "0");
        ERR_CHK(rc);
      }
      else
      {
        rc = STRCPY_S_NOCLOBBER(iapd->t2, sizeof(iapd->t2), strtok_r (evt_val,"'", &st));
        ERR_CHK(rc);
      }
    }
    CcspTraceInfo(("[%s] iapd->t2: %s\n", __FUNCTION__, iapd->t2));

    commonSyseventGet(COSA_DML_DHCPV6C_PREF_PRETM_SYSEVENT_NAME, evt_val, sizeof(evt_val));
    st = NULL;
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
      {
        rc = STRCPY_S_NOCLOBBER(iapd->pretm, sizeof(iapd->pretm), "0");
        ERR_CHK(rc);
      }
      else
      {
        rc = STRCPY_S_NOCLOBBER(iapd->pretm, sizeof(iapd->pretm), strtok_r (evt_val,"'", &st));
        ERR_CHK(rc);
      }
    }
    CcspTraceInfo(("[%s] iapd->pretm: %s\n", __FUNCTION__, iapd->pretm));

    st = NULL;
    if(evt_val[0]!='\0')
    {
      if(!strcmp(evt_val,"'\\0'"))
      {
        rc = STRCPY_S_NOCLOBBER(iapd->vldtm, sizeof(iapd->vldtm), "0");
        ERR_CHK(rc);
      }
      else
      {
        rc = STRCPY_S_NOCLOBBER(iapd->vldtm, sizeof(iapd->vldtm), strtok_r (evt_val,"'", &st));
        ERR_CHK(rc);
      }
    }
    CcspTraceInfo(("[%s] iapd->vldtm: %s\n", __FUNCTION__, iapd->vldtm));

    return 0;
}

#endif

#ifdef _HUB4_PRODUCT_REQ_
static int getLanUlaInfo(int *ula_enable)
{
    char  *pUla_enable=NULL;

    if (PSM_Get_Record_Value2(bus_handle,g_Subsystem,"dmsb.lanmanagemententry.lanulaenable", NULL, &pUla_enable) != CCSP_SUCCESS )
    {
        return -1;
    }

    if ( strncmp(pUla_enable, "TRUE", 4 ) == 0) {
        *ula_enable = 1;
    }
    else {
        *ula_enable = 0;
    }

    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(pUla_enable);

    return 0;
}
#endif

static int sysevent_fd_global = 0;
static token_t sysevent_token_global;

#ifdef _COSA_INTEL_USG_ARM_
void __cosa_dhcpsv6_refresh_config()
{
    FILE * fp = fopen(SERVER_CONF_LOCATION, "w+");
    PCHAR pTmp1 = NULL;
    PCHAR pTmp2 = NULL;
    PCHAR pTmp3 = NULL;
    CHAR prefixValue[64] = {0};
    CHAR prefixFullName[64] = {0};
    ULONG Index  = 0;
    ULONG Index2 = 0;
    ULONG Index3 = 0;
    int Index4 = 0;
    ULONG uSize = 0;
    ULONG preferedTime = 3600;
    ULONG validTime = 7200;
    int   returnValue = 0;
    ULONG ret = 0;
    char * saveptr = NULL;
    char *pServerOption = NULL;
    FILE *responsefd=NULL;
    char *networkResponse = "/var/tmp/networkresponse.txt";
    int iresCode = 0;
    char responseCode[10];
    BOOL  isInCaptivePortal = FALSE;
    int inWifiCp=0;
    char buf[6];
    ULONG  T1 = 0;
    ULONG  T2 = 0;
    struct stat check_ConfigFile;
    UtopiaContext utctx = {0};
    char relayStr[1024];
    int  Cnt = 0;
    errno_t rc = -1;

    if (!fp)
        return;

    if (!Utopia_Init(&utctx)) {
        fclose(fp);
        return;
    }

    /*Begin write configuration */
    {
        char buf[12];
        int log_level = 4;

        if (syscfg_get( NULL, "dibbler_log_level", buf, sizeof(buf)) == 0)
        {
            log_level = atoi(buf);
        }
        if (log_level < 1)
        {
            log_level = 1;
        }
        else if (log_level > 8)
        {
            log_level = 4;
        }

        /*
            1 : Emergency (not used - logging will be disabled)
            2 : Alert (not used - logging will be disabled)
            3 : Critical
            4 : Error
            5 : Warning
            6 : Notice
            7 : Info
            8 : Debug
        */
        fprintf(fp, "log-level %d\n", log_level);
    }

    fprintf(fp, "log-mode syslog\n");

    /*
       Enable inactive mode: When server begins operation and it detects that
       required interfaces are not ready, error message is printed and server
       exits. However, if inactive mode is enabled, server sleeps instead and
       wait for required interfaces to become operational.
    */
    fprintf(fp, "inactive-mode\n");

    //strict RFC compliance rfc3315 Section 13
    fprintf(fp, "drop-unicast\n");

    //Intel Proposed RDKB Generic Bug Fix from XB6 SDK
    fprintf(fp, "reconfigure-enabled 1\n");

    //support relay
    fprintf(fp, "guess-mode\n");

    for ( Index = 0; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        /* We need get interface name according to Interface field*/
        if ( !sDhcpv6ServerPool[Index].Cfg.bEnabled )
            continue;

        fprintf(fp, "iface %s {\n", COSA_DML_DHCPV6_SERVER_IFNAME);
        Cnt += sprintf(relayStr+Cnt,"iface  relay1 {\n");
        Cnt += sprintf(relayStr+Cnt,"   relay %s\n",  COSA_DML_DHCPV6_SERVER_IFNAME);

        if ( sDhcpv6ServerPool[Index].Cfg.RapidEnable ){
            fprintf(fp, "   rapid-commit yes\n");
            Cnt += sprintf(relayStr+Cnt,"   rapid-commit yes\n");
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
        Cnt += sprintf(relayStr+Cnt,"   preference %d\n", 255);

        /*begin class
                    fc00:1:0:0:4::/80,fc00:1:0:0:5::/80,fc00:1:0:0:6::/80
                */
        if ( sDhcpv6ServerPool[Index].Cfg.IANAEnable && sDhcpv6ServerPool[Index].Info.IANAPrefixes[0] )
        {
            pTmp1 = AnscCloneString((char*)sDhcpv6ServerPool[Index].Info.IANAPrefixes);
            pTmp3 = pTmp1;
            pTmp2 = pTmp1;

            for (; ; pTmp2 = NULL)
            {
                pTmp1 = strtok_r(pTmp2, ",", &saveptr);
                if (pTmp1 == NULL)
                    break;

                /* This pTmp1 is IP.Interface.{i}.IPv6Prefix.{i}., we need ipv6 address(eg:2000:1:0:0:6::/64) according to it*/
                rc = sprintf_s(prefixFullName, sizeof(prefixFullName), "%sPrefix",pTmp1);
                if(rc < EOK)
                {
                    ERR_CHK(rc);
                }
                uSize = sizeof(prefixValue);
                returnValue = g_GetParamValueString(g_pDslhDmlAgent, prefixFullName, prefixValue, &uSize);
                if ( returnValue != 0 )
                {
                    CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- g_GetParamValueString for iana:%d\n", returnValue));
                }

                fprintf(fp, "   class {\n");
                Cnt += sprintf(relayStr+Cnt,"   class {\n");

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
                    int i = _ansc_strlen((const char*)prefixValue);
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
                Cnt += sprintf(relayStr+Cnt,"       pool %s%s - %s%s\n", prefixValue, sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin, prefixValue, sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd );
#endif

                /*we need get two time values */
                {
				char s_iapd_pretm[32] = {0};
				char s_iapd_vldtm[32] = {0};
				ULONG  iapd_pretm, iapd_vldtm =0;
			
#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
                /* This pTmp1 is IP.Interface.{i}.IPv6Prefix.{i}., Fetch Interface Name from it*/
                int index, prefixIndex;
                char dmName[256] ={0};
                char dmValue[64] ={0};
                char sysEventName[256] ={0};

                sscanf (pTmp1,"Device.IP.Interface.%d.IPv6Prefix.%d.", &index, &prefixIndex);

                rc = sprintf_s((char*)dmName, sizeof(dmName), "Device.IP.Interface.%d.Name",index);
                if(rc < EOK)
                {
                    ERR_CHK(rc);
                }

                uSize = sizeof(dmValue);
                returnValue = g_GetParamValueString(g_pDslhDmlAgent, dmName, dmValue, &uSize);
                if ( returnValue != 0 )
                {
                    CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- g_GetParamValueString for iana:%d\n", returnValue));
                }

                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_PRETM_SYSEVENT_NAME, dmValue);
                commonSyseventGet(sysEventName, s_iapd_pretm, sizeof(s_iapd_pretm));

                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_VLDTM_SYSEVENT_NAME, dmValue);
                commonSyseventGet(sysEventName, s_iapd_vldtm, sizeof(s_iapd_vldtm));
#else
                commonSyseventGet(COSA_DML_DHCPV6C_PREF_PRETM_SYSEVENT_NAME, s_iapd_pretm, sizeof(s_iapd_pretm));
                commonSyseventGet(COSA_DML_DHCPV6C_PREF_VLDTM_SYSEVENT_NAME, s_iapd_vldtm, sizeof(s_iapd_vldtm));
#endif

				sscanf(s_iapd_pretm, "%lu", &iapd_pretm);
				sscanf(s_iapd_vldtm, "%lu", &iapd_vldtm);
				
				if (sDhcpv6ServerPool[Index].Cfg.LeaseTime <= 0) {
					token_t se_token;
					int se_fd = -1;
					char preferred_lft[32] = {0};
					char valid_lft[32] = {0};

					se_fd = s_sysevent_connect(&se_token);

					if (se_fd < 0)
					{
						AnscTraceFlow(("%s: preferred_lft = syseventerror\n", __FUNCTION__));
						AnscCopyString(preferred_lft, "syseventError");
						preferedTime = 0;
						validTime = 0;
					}
					else
					{
						/* Get the lease time, valid time of the prefix delegation */
						sysevent_get(se_fd, se_token, "ipv6_prefix_prdtime", preferred_lft, sizeof(preferred_lft));
						sysevent_get(se_fd, se_token, "ipv6_prefix_vldtime", valid_lft, sizeof(valid_lft));
						preferedTime = atoi(preferred_lft);
						validTime = atoi(valid_lft);
					}

					T1 = preferedTime / 2;
					T2 = preferedTime * 8 / 10;
				}else{
					T1           = iapd_pretm/2;
					T2           = (ULONG)(iapd_pretm*80.0/100);
					preferedTime = iapd_pretm;
					validTime    = iapd_vldtm;
				}

				fprintf(fp, "       T1 %lu\n", T1);
				fprintf(fp, "       T2 %lu\n", T2);
				fprintf(fp, "       prefered-lifetime %lu\n", iapd_pretm);
				fprintf(fp, "       valid-lifetime %lu\n", iapd_vldtm);			
				Cnt += sprintf(relayStr+Cnt, "		 T1 %lu\n", T1);
				Cnt += sprintf(relayStr+Cnt, "		 T2 %lu\n", T2);
				Cnt += sprintf(relayStr+Cnt, "		 prefered-lifetime %lu\n", iapd_pretm);
				Cnt += sprintf(relayStr+Cnt, "		 valid-lifetime %lu\n", iapd_vldtm);
			}	              
                fprintf(fp, "   }\n");
                Cnt += sprintf(relayStr+Cnt, "   }\n");
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
                if ( tagList[Index3].tag != (int)sDhcpv6ServerPoolOption[Index][Index2].Tag )
                    continue;
                else
                    break;
            }

            if ( Index3 >= sizeof(tagList)/sizeof(struct DHCP_TAG) )
                continue;

            // During captive portal no need to pass DNS
            // Check the reponse code received from Web Service

            iresCode = 0;

            if ( ( responsefd = fopen( networkResponse, "r" ) ) != NULL )
            {
                if (fgets( responseCode, sizeof(responseCode), responsefd ) != NULL )
                {
                    iresCode = atoi( responseCode );
                }

                /* RDKB-6780, CID-33149, free unused resources before return */
                fclose(responsefd);
                responsefd = NULL;
            }

            // Get value of redirection_flag
            /* CID: 55578 Array compared against 0*/
            if(!syscfg_get( NULL, "redirection_flag", buf, sizeof(buf)))
            {
                if ((strcmp(buf, "true") == 0) && (iresCode == 204))
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
        char rfCpEnable[6] = {0};
        char rfCpMode[6] = {0};
        int inRfCaptivePortal = 0;
        /* CID: 55578 Array compared against 0*/
        if(!syscfg_get(NULL, "enableRFCaptivePortal", rfCpEnable, sizeof(rfCpEnable)))
        {
          if (strncmp(rfCpEnable,"true",4) == 0)
          {
              /* CID: 55578 Array compared against 0*/
              if(!syscfg_get(NULL, "rf_captive_portal", rfCpMode,sizeof(rfCpMode)))
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
					   rc = strcpy_s( dns_str, sizeof(dns_str), (char*)sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
					   ERR_CHK(rc);
					   CosaDmlDhcpv6s_format_DNSoption( dns_str );
					
					   // Check device is in captive portal mode or not
					   if( 1 == isInCaptivePortal )
					   {
                                                  if (!strncmp(l_cSecWebUI_Enabled, "true", 4))
                                                  {
                                                      char static_dns[256] = {0};
                                                      commonSyseventGet("lan_ipaddr_v6", static_dns, sizeof(static_dns));
                                                      if ( '\0' != dns_str[ 0 ] )
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
						              fprintf(fp, "#	 option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dns_str);
                                                          }
                                                          else
                                                          {
                                                              fprintf(fp, "#	 option %s %s\n", tagList[Index3].cmdstring, dns_str);
                                                          }
                                                      }
                                                      else
                                                      {
                                                          char dyn_dns[256] = {0};
                                                          sysevent_get(sysevent_fd_global, sysevent_token_global, "wan6_ns", dyn_dns, sizeof(dyn_dns));
                                                          if ( '\0' != dyn_dns[ 0 ] )
                                                          {
                                                              format_dibbler_option(dyn_dns);
                                                              if ( '\0' != static_dns[ 0 ] )
                                                              {
                                                                  fprintf(fp, "#    option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dyn_dns);
                                                              }
                                                              else
                                                              {
                                                                  fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dyn_dns);
                                                              }
                                                          }
                                                          else
                                                          {
                                                              if ( '\0' != static_dns[ 0 ] )
                                                              {
                                                                  fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, static_dns);
                                                              }
                                                          }
                                                      }
                                                          
                                                  }
                                                  else
                                                  {
                                                      fprintf(fp, "#	 option %s %s\n", tagList[Index3].cmdstring, dns_str);
                                                  }
                                                   
					   }
					   else
					   {
                                                   if (!strncmp(l_cSecWebUI_Enabled, "true", 4)) 
                                                   {
                                                       char static_dns[256] = {0};
                                                       char dyn_dns[256] = {0};
                                                       sysevent_get(sysevent_fd_global, sysevent_token_global, "wan6_ns", dyn_dns, sizeof(dyn_dns));
                                                       if ( '\0' != dyn_dns[ 0 ] )
                                                       {
                                                           format_dibbler_option(dyn_dns);
                                                       }
                                                       commonSyseventGet("lan_ipaddr_v6", static_dns, sizeof(static_dns));
                                                       if ( '\0' != dns_str[ 0 ] )
                                                       {
                                                            
                                                            if ( '\0' != static_dns[ 0 ] )
                                                            {
                                                                if ( '\0' != dyn_dns[ 0 ] )
                                                                {
						                    fprintf(fp, "	option %s %s,%s,%s\n", tagList[Index3].cmdstring, static_dns, dns_str, dyn_dns);
                                                                }
                                                                else
                                                                {
                                                                    fprintf(fp, "	option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dns_str);
                                                                }
                                                            }
                                                            else
                                                            {
                                                                if ( '\0' != dyn_dns[ 0 ] )
                                                                {
                                                                     fprintf(fp, "	option %s %s,%s\n", tagList[Index3].cmdstring, dyn_dns, dns_str);
                                                                }
                                                                else
                                                                {
                                                                     fprintf(fp, "	option %s %s\n", tagList[Index3].cmdstring, dns_str);
                                                                }
                                                            }
                                                       }
                                                       else
                                                       {
                                                            if ( '\0' != static_dns[ 0 ] )
                                                            {
                                                                if ( '\0' != dyn_dns[ 0 ] )
                                                                {
                                                                    fprintf(fp, "	option %s %s,%s\n", tagList[Index3].cmdstring, static_dns,dyn_dns);
                                                                }
                                                                else
                                                                {
                                                                    fprintf(fp, "	option %s %s\n", tagList[Index3].cmdstring, static_dns);
                                                                }
                                                            }
                                                            else
                                                            {
                                                                if ( '\0' != dyn_dns[ 0 ] )
                                                                {
                                                                    fprintf(fp, "	option %s %s\n", tagList[Index3].cmdstring, dyn_dns);
                                                                }
                                                            }
                                                       }
                                                   }
                                                   else
                                                   {
                                                       if ( '\0' != dns_str[ 0 ] ){
							      fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, dns_str);
						       } 
                                                   }
					   }
					
					   CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__, 
																						 __LINE__,
																						 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
																						 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
					}
					else
					{
						sysevent_get(sysevent_fd_global, sysevent_token_global, "wan6_ns", dns_str, sizeof(dns_str));
						if (dns_str[0] != '\0') {
							format_dibbler_option(dns_str);
							if( isInCaptivePortal == TRUE )
							{
								fprintf(fp, "#	  option %s %s\n", tagList[Index3].cmdstring, dns_str);
							}
							else
							{
								//By default isInCaptivePortal is false
								fprintf(fp, "	option %s %s\n", tagList[Index3].cmdstring, dns_str);
							}
						}
					}
                }
                else if (sDhcpv6ServerPoolOption[Index][Index2].Tag == 24)
                {//domain
                    char domain_str[256] = {0};
                        CcspTraceWarning(("_cosa_dhcpsv6_refresh_config -- Tag is 24 \n"));
                    sysevent_get(sysevent_fd_global, sysevent_token_global, "ipv6_dnssl", domain_str, sizeof(domain_str));
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
				/* Static DNS Servers */
                if ( sDhcpv6ServerPoolOption[Index][Index2].Tag == 23 ) {
                    char dnsServer[ 256 ] = { 0 };
		    int ula_enable = 0;
		    int result = 0;
		    result = getLanUlaInfo(&ula_enable);
		    if(result != 0) {
			    fprintf(stderr, "getLanIpv6Info failed");
			    return;
		    }
				    if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled ) {
					   //CcspTraceWarning(("Cfg.X_RDKCENTRAL_COM_DNSServersEnabled is 1 \n"));
					   /* RDKB-50535 send ULA address as DNS address only when lan UNA is enabled */
					    if (ula_enable)
					    {
					          rc = strcpy_s( dnsServer, sizeof( dnsServer ), (const char * restrict)sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
						  ERR_CHK(rc);
					    }
					    CosaDmlDhcpv6s_format_DNSoption( dnsServer );
                                            char l_cSecWebUI_Enabled[8] = {0};
                                            syscfg_get(NULL, "SecureWebUI_Enable", l_cSecWebUI_Enabled, sizeof(l_cSecWebUI_Enabled));
                                            char static_dns[256] = {0};
                                            commonSyseventGet("lan_ipaddr_v6", static_dns, sizeof(static_dns));

					    // Check device is in captive portal mode or not
					    if( 1 == isInCaptivePortal )
					    {
                                                  if (!strncmp(l_cSecWebUI_Enabled, "true", 4) && (!ula_enable)) 
                                                  {
                                                      if ( '\0' != dnsServer[ 0 ] )
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
                                                              fprintf(fp, "#    option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dnsServer);
                                                          }
                                                          else
                                                          {
                                                              fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsServer);
                                                          }
                                                      }
                                                      else
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
                                                              fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, static_dns);
                                                          }
                                                      }
                                                  }
                                                  else
                                                  {
						      fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsServer);
                                                  }
					    }
					    else
					    {
                                                  if (!strncmp(l_cSecWebUI_Enabled, "true", 4) && (!ula_enable)) 
                                                  {
                                                      if ( '\0' != dnsServer[ 0 ] )
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
                                                              fprintf(fp, "     option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dnsServer);
                                                          }
                                                          else
                                                          {
                                                              fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, dnsServer);
                                                          }
                                                      }
                                                      else
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
                                                              fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, static_dns);
                                                          }
                                                      }
                                                  }
                                                  else
                                                  {
						      fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, dnsServer);
                                                  }
					    }

				    }
                }
                else if (sDhcpv6ServerPoolOption[Index][Index2].Tag == 24) {
                    char domain_str[256] = {0};
                    sysevent_get(sysevent_fd_global, sysevent_token_global, "ipv6_domain_name", domain_str, sizeof(domain_str));
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
                    sysevent_get(sysevent_fd_global, sysevent_token_global, "vendor_spec", vendor_spec, sizeof(vendor_spec));
                    if (vendor_spec[0] != '\0') {
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, vendor_spec);
                    }
                    else
                    {
                        CcspTraceWarning(("vendor_spec sysevent failed to get, so not updating vendor_spec information. \n"));
                    }
                }
#else

                //default basic options if there is no recv. options/config available
                if ( (g_recv_option_num == 0) && (Index2 == 0) )
                { 
                    char tmpstr[256];
                    token_t se_token;
                    int se_fd;

                    se_fd = s_sysevent_connect(&se_token);

                    if (se_fd >= 0)
                    {
                        tmpstr[0] = 0;
                        sysevent_get(se_fd, se_token, "ipv6_nameserver", tmpstr, sizeof(tmpstr));
                        if (tmpstr[0] != '\0') 
                        { 
                          format_dibbler_option(tmpstr);
                          fprintf(fp, "option dns-server %s\n", tmpstr);
                          Cnt += sprintf(relayStr+Cnt, "option dns-server %s\n", tmpstr);
                        }

                        tmpstr[0] = 0;
                        sysevent_get(se_fd, se_token, "wan6_domain", tmpstr, sizeof(tmpstr));
                        if (tmpstr[0] != '\0') 
                        { 
                          format_dibbler_option(tmpstr);
                          fprintf(fp, "option domain %s\n", tmpstr);
                          Cnt += sprintf(relayStr+Cnt, "option domain %s\n", tmpstr);
                        }

                        tmpstr[0] = 0;
                        sysevent_get(se_fd, se_token, "wan6_ntp_srv", tmpstr, sizeof(tmpstr));
                        if (tmpstr[0] != '\0') 
                        {
                          format_dibbler_option(tmpstr);
                          fprintf(fp, "option ntp-server %s\n", tmpstr);
                          Cnt += sprintf(relayStr+Cnt, "option ntp-server %s\n", tmpstr);
                        }

                        fprintf(fp, "\n");
                        Cnt += sprintf(relayStr+Cnt, "\n");
                    }
                }

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
                   char l_cSecWebUI_Enabled[8] = {0};
                   syscfg_get(NULL, "SecureWebUI_Enable", l_cSecWebUI_Enabled, sizeof(l_cSecWebUI_Enabled));

				   /* Static DNS Servers */
				   if( 1 == sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled )
				   {
					  rc = strcpy_s( dnsStr, sizeof(dnsStr), (const char * __restrict__)sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
					  ERR_CHK(rc);
					  CosaDmlDhcpv6s_format_DNSoption( dnsStr );

					  // Check device is in captive portal mode or not
					  if( 1 == isInCaptivePortal )
					  {
                                                  if (!strncmp(l_cSecWebUI_Enabled, "true", 4))
                                                  {
                                                      char static_dns[256] = {0};
                                                      commonSyseventGet("lan_ipaddr_v6", static_dns, sizeof(static_dns));
                                                      if ( '\0' != dnsStr[ 0 ] )
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
                                                              fprintf(fp, "#    option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dnsStr);
                                                          }
                                                          else
                                                          {
                                                              fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                                          }
                                                      }
                                                      else
                                                      {
                                                          char dyn_dns[256] = {0};
                                                          ret = CosaDmlDHCPv6sGetDNS((char*)g_recv_options[Index4].Value, dyn_dns, sizeof(dyn_dns) );
                                                          if ( '\0' != dyn_dns[ 0 ] )
                                                          {
                                                              if ( '\0' != static_dns[ 0 ] )
                                                              {
                                                                  fprintf(fp, "#    option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dyn_dns);
                                                              }
                                                              else
                                                              {
                                                                  fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dyn_dns);
                                                              }
                                                          }
                                                          else
                                                          {
                                                              if ( '\0' != static_dns[ 0 ] )
                                                              {
                                                                  fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, static_dns);
                                                              }
                                                          }
                                                      }
                                                  }
                                                  else
                                                  {
						      fprintf(fp, "#    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                                  }
					  }
					  else
					  {
                                                  if (!strncmp(l_cSecWebUI_Enabled, "true", 4))
                                                  {
                                                      char static_dns[256] = {0};
                                                      commonSyseventGet("lan_ipaddr_v6", static_dns, sizeof(static_dns));
                                                      char dyn_dns[256] = {0};
                                                      ret = CosaDmlDHCPv6sGetDNS((char*)g_recv_options[Index4].Value, dyn_dns, sizeof(dyn_dns) );
                                                      if ( '\0' != dnsStr[ 0 ] )
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
                                                              if ( '\0' != dyn_dns[ 0 ] )
                                                              {
						                  fprintf(fp, "    option %s %s,%s,%s\n", tagList[Index3].cmdstring, static_dns, dnsStr, dyn_dns);
                                                              }
                                                              else
                                                              {
                                                                  fprintf(fp, "    option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dnsStr);
                                                              }
                                                          }
                                                          else
                                                          {
                                                              if ( '\0' != dyn_dns[ 0 ] )
                                                              {
                                                                  fprintf(fp, "    option %s %s,%s\n", tagList[Index3].cmdstring, dyn_dns, dnsStr);
                                                              }
                                                              else
                                                              {
                                                                  fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                                              }
                                                          }
                                                               
                                                      }
                                                      else
                                                      {
                                                          if ( '\0' != static_dns[ 0 ] )
                                                          {
                                                              if ( '\0' != dyn_dns[ 0 ] )
                                                              {
                                                                  fprintf(fp, "    option %s %s,%s\n", tagList[Index3].cmdstring, static_dns, dyn_dns);
                                                              }
                                                              else
                                                              {
                                                                  fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, static_dns);
                                                              }
                                                          }
                                                          else
                                                          {
                                                              if ( '\0' != dyn_dns[ 0 ] )
                                                              {
                                                                  fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, dyn_dns);
                                                              }
                                                          }
                                                           
                                                      }
                                                  }
                                                  else
                                                  {
						      fprintf(fp, "     option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                                  }
					  }

					  CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__, 
					  																	__LINE__,
					  																	sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
					  																	sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
				   }
				   else
				   {
#if defined(_COSA_BCM_ARM_)
					   ret=commonSyseventGet("wan6_ns", dnsStr, sizeof(dnsStr));
#else
                       ret = CosaDmlDHCPv6sGetDNS((char *)g_recv_options[Index4].Value, dnsStr, sizeof(dnsStr) );
#endif
					   if ( !ret )
                       {
                           if ( '\0' != dnsStr[ 0 ] )
                           {
#if defined(_COSA_BCM_ARM_)
                               format_dibbler_option(dnsStr);
#endif
                               // Check device is in captive portal mode or not
                               if ( 1 == isInCaptivePortal )
                               {
                                   fprintf(fp, "#	 option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                               }
                               else
                               {
                                   fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                               }
                           }
                       }
                   }
               }
                else if ( g_recv_options[Index4].Tag == 24 )
                { //domain
                    pServerOption =    CosaDmlDhcpv6sGetStringFromHex((char*)g_recv_options[Index4].Value);
                    if ( pServerOption )
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                }else{
                    if ( pServerOption )
                        fprintf(fp, "    option %s 0x%s\n", tagList[Index3].cmdstring, pServerOption);
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
					   rc = strcpy_s( dns_str, sizeof(dns_str), (char*)sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
					   ERR_CHK(rc);
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
						if ( _ansc_strstr((const char*)sDhcpv6ServerPoolOption[Index][Index2].Value, ":") )
							pServerOption = (char*)sDhcpv6ServerPoolOption[Index][Index2].Value;
						else
							pServerOption = CosaDmlDhcpv6sGetAddressFromString((char*)sDhcpv6ServerPoolOption[Index][Index2].Value);
	
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
                    pServerOption = CosaDmlDhcpv6sGetStringFromHex((char *)g_recv_options[Index4].Value);

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
        Cnt += sprintf(relayStr+Cnt, "}\n");
    }
    
    fprintf(fp, "\n\n");
    fprintf(fp, "%s",relayStr);

    fclose(fp);

    Utopia_Free(&utctx,1);

#if (!defined _COSA_INTEL_USG_ARM_) && (!defined _COSA_BCM_MIPS_)
    /*we will copy the updated conf file at once*/
    if (rename(TMP_SERVER_CONF, SERVER_CONF_LOCATION))
        CcspTraceWarning(("%s rename failed %s\n", __FUNCTION__, strerror(errno)));
#endif

    if (stat(SERVER_CONF_LOCATION, &check_ConfigFile) == -1) {
        commonSyseventSet("dibbler_server_conf-status","");
    }
    else if (check_ConfigFile.st_size == 0) {
        commonSyseventSet("dibbler_server_conf-status","empty");
    }
    else {
        commonSyseventSet("dibbler_server_conf-status","ready");
    }

    system("/bin/sh /etc/utopia/service.d/set_ipv6_dns.sh dibbler");
}
#endif

#ifdef _COSA_BCM_MIPS_
void __cosa_dhcpsv6_refresh_config()
{
    FILE * fp = fopen(SERVER_CONF_LOCATION, "w+");
    PCHAR pTmp1 = NULL;
    PCHAR pTmp2 = NULL;
    PCHAR pTmp3 = NULL;
    CHAR prefixValue[64] = {0};
    CHAR prefixFullName[64] = {0};
    ULONG Index  = 0;
    ULONG Index2 = 0;
    ULONG Index3 = 0;
    int Index4 = 0;
    ULONG uSize = 0;
    ULONG preferedTime = 3600;
    ULONG validTime = 7200;
    int   returnValue = 0;
    ULONG ret = 0;
    char * saveptr = NULL;
    char *pServerOption = NULL;
    FILE *responsefd=NULL;
    char *networkResponse = "/var/tmp/networkresponse.txt";
    int iresCode = 0;
    int IsCaptivePortalMode = 0;
    char responseCode[10];
    char buf[6];
    ULONG  T1 = 0;
    ULONG  T2 = 0;
    struct stat check_ConfigFile;
#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
    pd_pool_t           pd_pool;
    ia_pd_t             ia_pd;
#endif
    BOOL bBadPrefixFormat = FALSE;
    errno_t rc = -1;

    if (!fp)
        return;

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
        return;
    }
#endif

    /*Begin write configuration */
    fprintf(fp, "log-level 8\n");

    //strict RFC compliance rfc3315 Section 13
    fprintf(fp, "drop-unicast\n");

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
           CcspTraceInfo(("%s Skip Index: %lu, iface: %s, entry. Invalid PrefixRangeBegin: %s,  PrefixRangeEnd: %s, LeaseTime: %lu\n",
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
            pTmp1 = AnscCloneString((char*)sDhcpv6ServerPool[Index].Info.IANAPrefixes);
            pTmp3 = pTmp1;
            pTmp2 = pTmp1;

            for (; ; pTmp2 = NULL)
            {
                pTmp1 = strtok_r(pTmp2, ",", &saveptr);
                if (pTmp1 == NULL)
                    break;

                /* This pTmp1 is IP.Interface.{i}.IPv6Prefix.{i}., we need ipv6 address(eg:2000:1:0:0:6::/64) according to it*/
                rc = sprintf_s(prefixFullName, sizeof(prefixFullName), "%sPrefix",pTmp1);
                if(rc < EOK)
                {
                    ERR_CHK(rc);
                }
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
                    ULONG i = _ansc_strlen((const char*)prefixValue);
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
                       ULONG k;
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
                                               rc = strcpy_s( &prefixValue[k+1], sizeof(prefixValue) - k, "0:0:0:" );
                                               ERR_CHK(rc);
                                           break;

                                           case 2:
                                               // A:B:: -> A:B:0:0:
                                               rc = strcpy_s( &prefixValue[k+1], sizeof(prefixValue) - k, "0:0:" );
                                               ERR_CHK(rc);
                                           break;

                                           case 3:
                                               // A:B:C:: -> A:B:C:0:
                                               rc = strcpy_s( &prefixValue[k+1], sizeof(prefixValue) - k,  "0:" );
                                               ERR_CHK(rc);
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
                        validTime    = (sDhcpv6ServerPool[Index].Cfg.ValidLeaseTime);
                    }

                    fprintf(fp, "       T1 %lu\n", T1);
                    fprintf(fp, "       T2 %lu\n", T2);
                    fprintf(fp, "       prefered-lifetime %lu\n", preferedTime);
                    fprintf(fp, "       valid-lifetime %lu\n", validTime);
                }

                fprintf(fp, "   }\n");
            }

            AnscFreeMemory(pTmp3);

#ifdef CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION
            CcspTraceInfo(("[%s]  %d - See if need to emit pd-class, sDhcpv6ServerPool[Index].Cfg.IAPDEnable: %d, Index: %lu\n",
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
                if ( tagList[Index3].tag != (int)sDhcpv6ServerPoolOption[Index][Index2].Tag )
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

            // Get value of redirection_flag
            /* CID: 55578 Array compared against 0*/
            if(!syscfg_get( NULL, "redirection_flag", buf, sizeof(buf)))
            {
                if ((strcmp(buf, "true") == 0) && (iresCode == 204))
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
                                          rc = strcpy_s( dnsStr, sizeof(dnsStr), (const char*)sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
                                          ERR_CHK(rc);
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
                                           ret = CosaDmlDHCPv6sGetDNS((char*)g_recv_options[Index4].Value, dnsStr, sizeof(dnsStr) );

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
                    pServerOption =    CosaDmlDhcpv6sGetStringFromHex((char*)g_recv_options[Index4].Value);
                    if ( pServerOption )
                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                }else{
                    if ( pServerOption )
                        fprintf(fp, "    option %s 0x%s\n", tagList[Index3].cmdstring, pServerOption);
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
                                           rc = strcpy_s( dnsStr, sizeof(dnsStr), (const char*)sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers );
                                           ERR_CHK(rc);
                                           CosaDmlDhcpv6s_format_DNSoption( dnsStr );

                                           // Check device is in captive portal mode or not
                                           if( 1 == IsCaptivePortalMode )
                                           {
                                                   fprintf(fp, "#        option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                           }
                                           else
                                           {
                                                   fprintf(fp, "       option %s %s\n", tagList[Index3].cmdstring, dnsStr);
                                           } 

                                           CcspTraceWarning(("%s %d - DNSServersEnabled:%d DNSServers:%s\n", __FUNCTION__,
                                                                                                                                                                                 __LINE__,
                                                                                                                                                                                 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled,
                                                                                                                                                                                 sDhcpv6ServerPool[Index].Cfg.X_RDKCENTRAL_COM_DNSServers ));
                                        }
                                        else
                                        {
                                                if ( _ansc_strstr((const char*)sDhcpv6ServerPoolOption[Index][Index2].Value, ":") )
                                                        pServerOption = (char*)sDhcpv6ServerPoolOption[Index][Index2].Value;
                                                else
                                                        pServerOption = CosaDmlDhcpv6sGetAddressFromString((char*)sDhcpv6ServerPoolOption[Index][Index2].Value);

                                                if ( pServerOption )
                                                        fprintf(fp, "    option %s %s\n", tagList[Index3].cmdstring, pServerOption);
                                        }

                }
                else if ( g_recv_options[Index4].Tag == 24 )
                { //domain
                    pServerOption = CosaDmlDhcpv6sGetStringFromHex((char*)g_recv_options[Index4].Value);

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

    if (fp != NULL)
        fclose(fp);

    if (stat(SERVER_CONF_LOCATION, &check_ConfigFile) == -1) {
        commonSyseventSet("dibbler_server_conf-status","");
    }
    else if (check_ConfigFile.st_size == 0) {
        commonSyseventSet("dibbler_server_conf-status","empty");
    }
    else {
        commonSyseventSet("dibbler_server_conf-status","ready");
    }
}

#endif

int CosaDmlDhcpv6s_format_DNSoption( char *option )
{
    if (option == NULL)
        return -1;

    unsigned int i;

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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    
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
    v_secure_system("sysevent set zebra-restart");
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
    UNREFERENCED_PARAMETER(hContext);

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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};

    if ( type == g_dhcpv6_server_type )
        return ANSC_STATUS_SUCCESS;
    
    g_dhcpv6_server_type = type;

    if (!Utopia_Init(&utctx))
        return ANSC_STATUS_FAILURE;
    
    SETI_INTO_UTOPIA(DHCPV6S_NAME,  "", 0, "", 0, "servertype", g_dhcpv6_server_type)

    // lets update managed flag based on the type
    isRaFlagSet = FALSE;

    Utopia_Free(&utctx,1);

    if ( g_dhcpv6_server )
    {
        /* We need enable server */
        CosaDmlDHCPv6sTriggerRestart(FALSE);
#ifdef _HUB4_PRODUCT_REQ_
        v_secure_system("sysevent set zebra-restart");
#endif
        /* Stateful address is not effecting for clients whenever pool range is changed.
         * Added gw_lan_refresh to effect the stateful address to the clients according
         * to the configured poll range.
         */
        v_secure_system("gw_lan_refresh &");
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
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext);
    if ( ulIndex+1 > uDhcpv6ServerPoolNum )
        return ANSC_STATUS_FAILURE;

    /* CID 72229 fix */
    AnscCopyMemory(pEntry, &sDhcpv6ServerPool[ulIndex], sizeof(sDhcpv6ServerPool[ulIndex]));
    
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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    errno_t                         rc              = -1;

    if ( ulIndex+1 > uDhcpv6ServerPoolNum )
        return ANSC_STATUS_FAILURE;

    sDhcpv6ServerPool[ulIndex].Cfg.InstanceNumber  = ulInstanceNumber;

    rc = STRCPY_S_NOCLOBBER((char*)sDhcpv6ServerPool[ulIndex].Cfg.Alias, sizeof(sDhcpv6ServerPool[ulIndex].Cfg.Alias), pAlias);
    if ( rc != EOK )
    {
        ERR_CHK(rc);
        return ANSC_STATUS_FAILURE;
    }
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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    ULONG         Index = 0;
#if defined (MULTILAN_FEATURE)
    CHAR         param_val[CMD_BUFF_SIZE] = {0};
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
        snprintf((char*)pEntry->Info.IANAPrefixes, sizeof(pEntry->Info.IANAPrefixes), "%s%s", pEntry->Cfg.Interface, "IPv6Prefix.1.");
    }
#endif
    sDhcpv6ServerPool[Index] = *pEntry;
    sDhcpv6ServerPool[Index].Info.Status = COSA_DML_DHCP_STATUS_Enabled; /* We set this to be enabled all time */
  
    /* Add this entry into utopia */
    setpool_into_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", Index, &sDhcpv6ServerPool[Index]);
    
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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx  = {0};
    ULONG         Index  = 0;

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
    ULONG         Index2 = 0;
    for ( Index2 = Index+1; (Index2 < uDhcpv6ServerPoolNum) && (Index2 < DHCPV6S_POOL_NUM); Index2++ )
    {
        setpool_into_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", Index2-1, &sDhcpv6ServerPool[Index2]);
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

    unsetpool_from_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", Index );
    AnscZeroMemory(&sDhcpv6ServerPool[Index], sizeof(sDhcpv6ServerPool[Index]));

    /* do this lastly */
    /* limiting lower boundary of the DHCP Server Pool Num to avoid -ve values*/
    /*CID: 65892 Unsigned compared against 0*/
    uDhcpv6ServerPoolNum = (0 == (uDhcpv6ServerPoolNum)? 0 : (uDhcpv6ServerPoolNum-1) );


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
    errno_t rc = -1;

    if (!p_len || !pp_pathname)
        return -1;

    *p_len = 1; /*RDKB-6780, CID-33107, use after null check*/

    p_ipif_path = (char*)CosaUtilGetFullPathNameByKeyword
        (
            (PUCHAR)"Device.IP.Interface.",
            (PUCHAR)"Name",
            (PUCHAR)COSA_DML_DHCPV6_CLIENT_IFNAME
            );

    if (!p_ipif_path)
        return -2;

    rc = sprintf_s(name, sizeof(name), "%sIPv6PrefixNumberOfEntries", p_ipif_path);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }
            
    num = g_GetParamValueUlong(g_pDslhDmlAgent, name);

    for (i=0; i<num; i++)
    {
        rc = sprintf_s(name, sizeof(name), "%sIPv6Prefix.", p_ipif_path);
        if(rc < EOK)
        {
            ERR_CHK(rc);
        }
        inst2 = g_GetInstanceNumberByIndex(g_pDslhDmlAgent, name, i);

        if ( inst2 )
        {
            rc = sprintf_s(name,  sizeof(name), "%sIPv6Prefix.%lu.Origin", p_ipif_path, inst2);
            if(rc < EOK)
            {
                ERR_CHK(rc);
            }

            val_len = sizeof(param_val);               
            if ( ( 0 == g_GetParamValueString(g_pDslhDmlAgent, name, param_val, &val_len)) &&
                 AnscEqualString(param_val, "PrefixDelegation", TRUE ) )
            {
                snprintf(path, sizeof(path)-1, "%sIPv6Prefix.%lu.", p_ipif_path, inst2);
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
                
                rc = sprintf_s(*pp_pathname+strlen(*pp_pathname), *p_len - strlen(*pp_pathname), "%s,",path);
                if(rc < EOK)
                {
                    ERR_CHK(rc);
                }

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
    char * p1 = NULL;
    int need_append_comma = 0;
    char * p_pathname = NULL;
    int pathname_len = 0;
    int ret = 0;
    errno_t rc = -1;

    size = strlen((const char*)pCfg->IAPDManualPrefixes);

    p1 = (char*)pCfg->IAPDManualPrefixes;
    if ( strlen(p1) && (p1[strlen(p1)-1] != ',') )
    {
        need_append_comma = 1;
        size++;
    }
    
    _get_iapd_prefix_pathname(&p_pathname, &pathname_len);
    size += pathname_len; 

    if ((unsigned int)size > *pSize)
    {
        *pSize = size+1;
        ret = 1;
    }
    else
    {
        /*pValue is big enough*/
        rc = STRCPY_S_NOCLOBBER(pValue, *pSize, (const char*)pCfg->IAPDManualPrefixes);
        ERR_CHK(rc);

        if (need_append_comma)
        {
            rc = sprintf_s(pValue+strlen(pValue), *pSize - strlen(pValue), ",");
            if(rc < EOK)
            {
                ERR_CHK(rc);
            }
        }

        if (p_pathname)
        {
            rc = sprintf_s(pValue+strlen(pValue), *pSize - strlen(pValue), "%s", p_pathname);
            if(rc < EOK)
            {
                ERR_CHK(rc);
            }
        }
        
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
    ULONG size = 0;
    int ret = 0;
    char pd_pool[128] = {0};
    char start[64], end[64], pd_len[4];
    UNREFERENCED_PARAMETER(pCfg);
    errno_t rc = -1;

    commonSyseventGet("ipv6_subprefix-start", start, sizeof(start));
    commonSyseventGet("ipv6_subprefix-end", end, sizeof(end));
    commonSyseventGet("ipv6_pd-length", pd_len, sizeof(pd_len));

    if(start[0] != '\0' && end[0] != '\0' && pd_len[0] != '\0'){
        rc = sprintf_s(pd_pool, sizeof(pd_pool), "%s-%s/%s", start, end, pd_len);
        if(rc < EOK) ERR_CHK(rc);
    }
    
    size = strlen(pd_pool); 

    if (size > *pSize)
    {
        *pSize = size+1;
        ret = 1;
    }
    else
    {
        /*pValue is big enough*/
        rc = STRCPY_S_NOCLOBBER(pValue, *pSize, pd_pool);
        ERR_CHK(rc);

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
    UNREFERENCED_PARAMETER(hContext);
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
			  ( 0 != strcmp( (const char*)pCfg->X_RDKCENTRAL_COM_DNSServers, (const char*)sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.X_RDKCENTRAL_COM_DNSServers ) )
			 )
		   )
		{
			bNeedZebraRestart = TRUE;
		}
//#ifdef _HUB4_PRODUCT_REQ_
        /* Stateful address is not effecting for clients whenever pool range is changed.
         * Added gw_lan_refresh to effect the stateful address to the clients according
         * to the configured poll range.
         */
        if ( ( 0 != strcmp( (const char*)pCfg->PrefixRangeBegin, (const char*)sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin ) ) || \
             ( 0 != strcmp( (const char*)pCfg->PrefixRangeEnd, (const char*)sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd ) ) )
        {
            v_secure_system("gw_lan_refresh &");
        }
//#endif

        sDhcpv6ServerPool[Index].Cfg = *pCfg;
    }
    else
    {
		// Zebra restart based on below criteria
		if( ( pCfg->X_RDKCENTRAL_COM_DNSServersEnabled != sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.X_RDKCENTRAL_COM_DNSServersEnabled ) || \
			( ( '\0' != pCfg->X_RDKCENTRAL_COM_DNSServers[ 0 ] ) && \
			  ( 0 != strcmp( (const char*)pCfg->X_RDKCENTRAL_COM_DNSServers, (const char*)sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.X_RDKCENTRAL_COM_DNSServers ) )
			 )
		   )
		{
			bNeedZebraRestart = TRUE;
		}
//#ifdef _HUB4_PRODUCT_REQ_
        /* Stateful address is not effecting for clients whenever pool range is changed.
         * Added gw_lan_refresh to effect the stateful address to the clients according
         * to the configured poll range.
         */
        if ( ( 0 != strcmp((const char*)pCfg->PrefixRangeBegin, (const char*)sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.PrefixRangeBegin)) ||
             ( 0 != strcmp((const char*)pCfg->PrefixRangeEnd, (const char*)sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg.PrefixRangeEnd ) ) )
        {
            v_secure_system("gw_lan_refresh &");
        }
//#endif

        sDhcpv6ServerPool[DHCPV6S_POOL_NUM -1].Cfg = *pCfg;
        Index = DHCPV6S_POOL_NUM - 1;
    }    
#if defined (MULTILAN_FEATURE)
    if(pCfg->Interface[0] != '\0')
    {
        snprintf((char*)sDhcpv6ServerPool[Index].Info.IANAPrefixes, sizeof(sDhcpv6ServerPool[Index].Info.IANAPrefixes), "%s%s", pCfg->Interface, "IPv6Prefix.1.");
    }
#endif

    /* We do this synchronization here*/
    if ( ( _ansc_strlen((const char*)sDhcpv6ServerPool[Index].Cfg.IANAManualPrefixes) > 0 ) && 
         !_ansc_strstr((const char*)sDhcpv6ServerPool[Index].Info.IANAPrefixes, (const char*)sDhcpv6ServerPool[Index].Cfg.IANAManualPrefixes) ){
        _ansc_strcat((char*)sDhcpv6ServerPool[Index].Info.IANAPrefixes, "," );
        _ansc_strcat((char*)sDhcpv6ServerPool[Index].Info.IANAPrefixes, (const char*)sDhcpv6ServerPool[Index].Cfg.IANAManualPrefixes );
    }
   
    setpool_into_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", Index, &sDhcpv6ServerPool[Index]);

    CosaDmlDHCPv6sTriggerRestart(FALSE);

	// Check whether static DNS got enabled or not. If enabled then we need to restart the zebra process
	if( bNeedZebraRestart )
	{
        CcspTraceWarning(("%s Restarting Zebra Process\n", __FUNCTION__));
        v_secure_system("killall zebra && sysevent set zebra-restart");
	}

    return ANSC_STATUS_SUCCESS;
}

static char **string_split(char *string_to_split, char *delim_to_split)
{
    PANSC_TOKEN_CHAIN pTokenChain;
    PANSC_STRING_TOKEN pToken;
    char **result = NULL;
    int token_count;
    int i, idx = 0;

    pTokenChain = (PANSC_TOKEN_CHAIN) AnscTcAllocate(string_to_split, delim_to_split);
    if (pTokenChain == NULL)
    {
        return NULL;
    }

    token_count = AnscTcGetTokenCount(pTokenChain);
    if (token_count > 0)
    {
        result = AnscAllocateMemory(sizeof(char *) * (token_count + 1));

        for (i = 0; i < token_count; i++)
        {
            pToken = AnscTcUnlinkToken(pTokenChain);
            if (pToken)
            {
                result[idx++] = strdup(pToken->Name);
                AnscFreeMemory(pToken);
                pToken = NULL;
            }
        }

        result[idx] = NULL;
    }

    AnscTcFree((ANSC_HANDLE)pTokenChain);

    return result;
}

ANSC_STATUS
CosaDmlDhcpv6sMaxClient
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        ULONG                       *puLong
    )
{
    ULONG Index;
    ANSC_STATUS ret_Val = ANSC_STATUS_SUCCESS;
    char **prefixrangebegin, **prefixrangeend;
    int i;

    for (Index = 0; (Index < uDhcpv6ServerPoolNum) && (Index < DHCPV6S_POOL_NUM); Index++)
    {
        if (sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulInstanceNumber)
        {
            break;
        }
    }

    if ((Index >= DHCPV6S_POOL_NUM) || (Index >= uDhcpv6ServerPoolNum))
    {
        return ANSC_STATUS_FAILURE;
    }

    prefixrangebegin = string_split(sDhcpv6ServerPool[Index].Cfg.PrefixRangeBegin, ":");
    prefixrangeend = string_split(sDhcpv6ServerPool[Index].Cfg.PrefixRangeEnd, ":");

    if (prefixrangebegin && prefixrangeend)
    {
        unsigned long count = 0;

        for (i = 0; prefixrangebegin[i] && prefixrangeend[i]; i++)
        {
            unsigned long begin = strtoul(prefixrangebegin[i], NULL, 16);
            unsigned long end = strtoul(prefixrangeend[i], NULL, 16);

            count += (end - begin);
        }

        *puLong = count + 1;
    }
    else
    {
        ret_Val = ANSC_STATUS_FAILURE;
    }

    if (prefixrangebegin)
    {
        for (i = 0; prefixrangebegin[i]; i++)
        {
            AnscFreeMemory(prefixrangebegin[i]);
        }
        AnscFreeMemory(prefixrangebegin);
    }

    if (prefixrangeend)
    {
        for (i = 0; prefixrangeend[i]; i++)
        {
            AnscFreeMemory(prefixrangeend[i]);
        }
        AnscFreeMemory(prefixrangeend);
    }

    return ret_Val;
}

ANSC_STATUS
CosaDmlDhcpv6sGetPoolCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DHCPSV6_POOL_CFG    pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext);
    FILE *fp = NULL;
    char out[256];

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

    fp = v_secure_popen("r","busybox ps |grep %s|grep -v grep", SERVER_BIN);
    _get_shell_output(fp, out, sizeof(out));
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

#define DHCPSV6_SERVER_FILE "/tmp/dibbler/server-AddrMgr.xml"
#define DHCPSV6_CLIENT_FILE "/tmp/dibbler/server-client.txt"
#define DHCPSV6_CLIENT_FILE2 "/tmp/dibbler/client-AddrMgr.xml"
ULONG   g_dhcps6v_client_num  = 0;
PCOSA_DML_DHCPSV6_CLIENT        g_dhcps6v_client        = NULL;
PCOSA_DML_DHCPSV6_CLIENTCONTENT g_dhcps6v_clientcontent = NULL;

BOOL tagPermitted(int tag)
{
    unsigned int i = 0;

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
The file:/var/lib/dibbler/server-client.txt was not generated by default
Hence we could not use the existing _cosa_dhcpsv6_get_client api for DHCPv6 clients.

The below API is for parsing /tmp/dibbler/server-AddrMgr.xml
to get the DHCPv6 client information and forming '/tmp/dibbler/server-client.txt' file.
*/

#define DUID_STR_LEN 17

BOOL get_interface_name(char *paramNames, char *interface)
{
    char name[64] = {0};
    char param_val[64] = {0};
    ULONG val_len = 0;

    sprintf(name,  "%sName", paramNames);
    val_len = sizeof(param_val);
    if( 0 == g_GetParamValueString(g_pDslhDmlAgent, name, param_val, &val_len))
    {
        strcpy(interface, param_val);
        return TRUE;
    }
    return FALSE;
}

int cosa_dhcpv6_client_info(char *pInterface)
{
    char *buffer = 0;
    char *start_buf = 0;
    char *ptr = 0;
    int  i, mac_index = 0;
    int  client_num = 0;
    int  s_len;
    int  count, tag;
    long buffer_len = 0;
    char optionVal[256];
    char duid[64], ip6_addr[40], time_stamp[32], preferred_lifetime[32], unicast[128], prefix[32], valid_lifetime[32], ifacename[32];
    BOOL ret = 0;
    BOOL isClientOnInterface;
    FILE * fp;
    FILE * serverclient = NULL;
    char *interface = NULL;
    int counter = 1;

    serverclient = fopen(DHCPSV6_CLIENT_FILE,"w");

    if(serverclient)
    {	    
        while(counter <= 2)
        {
            if(counter == 1)
            {	    
                fp = fopen(DHCPSV6_SERVER_FILE, "r");
            }
            else
            {
                fp = fopen(DHCPSV6_CLIENT_FILE2, "r");	
            }	

            if (fp)
            {
                fseek(fp, 0, SEEK_END);
                buffer_len = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                buffer = malloc(buffer_len + 1);

                if (buffer)
                {
                    start_buf = buffer; //Storing the buffer starting address to free in the end
                    i = fread(buffer, 1, buffer_len, fp);
                    buffer[i] = 0;
                }
                else
                {
                    fclose(fp);
                    fclose(serverclient);
                    return ANSC_STATUS_FAILURE;
                }
                fclose(fp);
            }
            else
            {
                fclose(serverclient);		    
                return ANSC_STATUS_FAILURE;
            }

        
            // Added the client number text with reserved spaces which will be replaced later with actual client number
            if(counter == 1)
            {	
                fprintf(serverclient, "ClientNum:0  \n");
            }

            while(buffer)
            {
                isClientOnInterface = FALSE;
                buffer = strstr(buffer, "<AddrClient");
                if (buffer != NULL)
                {
                    buffer += strlen("<AddrClient");

                    ptr = strstr(buffer, "<IACount>");
                    if (ptr == NULL)
                    {
                        continue;
                    }
                    ptr += strlen("<IACount>");

                    count = 0;
                    while (*ptr != '<')
                    {
                        count = count * 10 + (*ptr++ - '0');
                    }

                    for (int cnt=0; cnt<count; cnt++)
                    {
                        buffer = strstr(buffer, "<AddrIA");
                        if (buffer != NULL)
                        {
                            buffer += strlen("<AddrIA");
                            interface = (char *) malloc(16);
                            memset(interface, '\0', 16);

                            if(counter == 1)
                            {	    

                                //read Interface
                                ptr = strstr(buffer, "ifacename=\"");
                                if(ptr !=NULL)
                                {
                                    s_len = strlen("ifacename=\"");
                                    ptr += s_len;
                                    i = 0;
                                    while(*ptr != '"')
                                    {
                                        ifacename[i++] = *ptr++;
                                    }
                                    ifacename[i] = '\0';
                                    ret = get_interface_name(pInterface, interface);
                                }

                                if (cnt == 0)
                                {
                                    // Since a client can have multiple addresses, increment client count only for the first instance.
                                    // Also DUID and unicast address will be added once for each client in server-client.txt file.
                                    isClientOnInterface = TRUE;

                                    // read DUID
                                    ptr = strstr(buffer, "duid length=\"");
                                    if(ptr !=NULL)
                                    {
                                        ptr += DUID_STR_LEN;
                                        i = 0;
                                        while(*ptr != '<')
                                        {
                                            duid[i++] = *ptr++;
                                        }
                                        duid[i] = '\0';
                                        fprintf(serverclient, "DUID:%s\n", duid);
                                    }
     
                                    // read Unicast
                                    ptr = strstr(buffer, "unicast=\"");
                                    if(ptr !=NULL)
                                    {
                                        s_len = strlen("unicast=\"");
                                        ptr += s_len;
                                        i = 0;
                                        while(*ptr != '"')
                                        {
                                            unicast[i++] = *ptr++;
                                        }
                                        unicast[i] = '\0';
                                        fprintf(serverclient, "Unicast:%s\n", unicast);
                                    }
                                }
                            }   

                            // read the prefix
                            ip6_addr[0] = 0;
                            if(counter == 1)
                            {		    
                                ptr = strstr(buffer, "prefix=\"");
                            }
                            else
                            {
                                ptr = strstr(buffer, "<AddrPrefix");
                                if (ptr != NULL)
                                {                     
                                    ptr += strlen("<AddrPrefix");    
                                    ptr = strstr(ptr, "length=\"");
                                }
                            }	
                            if(ptr !=NULL)
                            {
                                if(counter == 1)
                                {    	
                                    s_len = strlen("prefix=\"");
                                }
                                else
                                {
                                    s_len = strlen("length=\"");	
                                }   
                                ptr += s_len;
                                i = 0;
                                while(*ptr != '"')
                                {
                                    prefix[i++] = *ptr++;
                                }
                                prefix[i] = '\0';
          
                                // read IPv6Addr
                                s_len = strlen(">\"");
                                ptr += s_len;
                                i = 0;
                                while(*ptr != '<')
                                {
                                    ip6_addr[i++] = *ptr++;
                                }
                                ip6_addr[i] = '\0';
                                if(counter == 1)
                                {		
                                    fprintf(serverclient, "Addr:%s\n", ip6_addr);
                                }
                                else
                                {
                                    fprintf(serverclient, "pdPrefix:%s/%s\n", ip6_addr, prefix);
                                }    
                            }

                            // read the timestamp
                            ptr = strstr(buffer, "timestamp=\"");
                            if(ptr !=NULL)
                            {
                                s_len = strlen("timestamp=\"");
                                ptr += s_len;
                                i = 0;
                                while(*ptr != '"')
                                {
                                    time_stamp[i++] = *ptr++;
                                }
                                time_stamp[i] = '\0';
                                if(counter == 1)
                                {		
                                    fprintf(serverclient, "Timestamp:%s\n", time_stamp);
                                }
                                else
                                {
                                    fprintf(serverclient, "pdTimestamp:%s\n", time_stamp);
                                }	    
                            }
          
                            // read the PreferredLifetime
                            ptr = strstr(buffer, "pref=\"");
                            if(ptr !=NULL)
                            {
                                s_len = strlen("pref=\"");
                                ptr += s_len;
                                i = 0;
                                while(*ptr != '"')
                                {
                                    preferred_lifetime[i++] = *ptr++;
                                }
                                preferred_lifetime[i] = '\0';
                                if(counter == 1)
                                {
                                    fprintf(serverclient, "Prefered:%s\n", preferred_lifetime);
                                }
                                else
                                {
                                    fprintf(serverclient, "pdPrefered:%s\n", preferred_lifetime);
                                }	
                            }
          
                            //read ValidLifetime
                            ptr = strstr(buffer, "valid=\"");
                            if(ptr !=NULL)
                            {
                                s_len = strlen("valid=\"");
                                ptr += s_len;
                                i = 0;
                                while(*ptr != '"')
                                {
                                    valid_lifetime[i++] = *ptr++;
                                }
                                valid_lifetime[i] = '\0';
                                if(counter == 1)
                                {	
                                    fprintf(serverclient, "Valid:%s\n", valid_lifetime);
                                }
                                else
                                {
                                    fprintf(serverclient, "pdValid:%s\n", valid_lifetime);
                                }
                            }
                        
                            if (interface != NULL)
                            {
                                free(interface);
                                interface = NULL;
                            }
                        }
                    }
                    if (isClientOnInterface)
                    {
                        ptr = strstr(buffer, "<optionCount>");
                        ptr += strlen("<optionCount>");
                        count = 0;
                        while (*ptr != '<')
                        {
                            count = count * 10 + (*ptr++ - '0');
                        }

                        for (int cnt=0; cnt<count; cnt++)
                        {
                            buffer = strstr(buffer, "<option ");
                            buffer += strlen("<option ");
                            tag = 0;
                            ptr = buffer;
                            while (*ptr != '>')
                            {
                                tag = tag * 10 + (*ptr++ - '0');
                            }

                            ++ptr;
                            i = 0;
                            while (*ptr != '<')
                            {
                                optionVal[i++] = *ptr++;
                            }
                            optionVal[i] = '\0';
                            fprintf(serverclient, "option:%d:%s\n", tag, optionVal);
                        }
                        ++client_num;
                    }
                }
            }

            //setting back the starting postion of File for writing the Client Number
            if(counter == 1)
            {	
                fseek(serverclient, 0, SEEK_SET);
                fprintf(serverclient, "ClientNum:%d", client_num);
                fseek(serverclient, 0, SEEK_END);
            }
            counter++;

            buffer = start_buf; //Moving the buffer to the starting address
            if (buffer != NULL)
            {
                free(buffer);
            }
        }
        fclose(serverclient);
    }
    return 0;
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
    FILE * fp = fopen(DHCPSV6_CLIENT_FILE, "r");
    CHAR  oneLine[256] = {0};
    PCHAR pTmp1 = NULL;
    PCHAR pTmp2 = NULL;
    PCHAR pTmp3 = NULL;
    ULONG  Index = 0xFFFFFFFF;
    time_t t1    = 0;
    struct tm t2 = {0};
    CHAR  buf[128] = {0};
    ULONG  timeStamp = 0;    
    errno_t rc = -1;
    PCOSA_DML_DHCPSV6_CLIENT_OPTION          pOption1 = NULL;
    PCOSA_DML_DHCPSV6_CLIENT_IPV6ADDRESS     pIPv6Address1 = NULL;
    PCOSA_DML_DHCPSV6_CLIENT_IPV6PREFIX      pIPv6Prefix1  = NULL;
    struct timespec monotonic_time;
    time_t monotonic_offset;

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

    clock_gettime(CLOCK_MONOTONIC, &monotonic_time);
    monotonic_offset = time(NULL) - monotonic_time.tv_sec;

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
                    rc = sprintf_s(g_dhcps6v_client[i].Alias, sizeof(g_dhcps6v_client[i].Alias), "Client%lu",i+1);
                    if(rc < EOK)
                    {
                        ERR_CHK(rc);
                    }
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

            // commenting out for warning "value computed is not used [-Werror=unused-value]".
            pTmp1++;

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

            rc = STRCPY_S_NOCLOBBER( (char*)g_dhcps6v_clientcontent[Index].pOption[g_dhcps6v_clientcontent[Index].NumberofOption-1].Value, sizeof(g_dhcps6v_clientcontent[Index].pOption[g_dhcps6v_clientcontent[Index].NumberofOption-1].Value), pTmp1);
            ERR_CHK(rc);
        }else if ( _ansc_strcmp(pTmp1, "Unicast") == 0 )
        {
          /* CID: 61618 Explicit null dereferenced*/
          if (g_dhcps6v_client) {
            rc = STRCPY_S_NOCLOBBER((char*)g_dhcps6v_client[Index].SourceAddress, sizeof(g_dhcps6v_client[Index].SourceAddress), pTmp3);
            ERR_CHK(rc);
          }
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
            

            rc = STRCPY_S_NOCLOBBER((char*)g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].IPAddress, sizeof(g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].IPAddress), pTmp3);
            ERR_CHK(rc);
        }else if ( _ansc_strcmp(pTmp1, "Timestamp") == 0 )
        {
            timeStamp = atoi(pTmp3);
        }else if ( _ansc_strcmp(pTmp1, "Prefered") == 0 )
        {
            /* CID: 73916 Explicit null dereferenced*/
            if ( g_dhcps6v_clientcontent && g_dhcps6v_clientcontent[Index].NumberofIPv6Address )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if ((unsigned int)t1 == 0xffffffff)
                {
                    rc = strcpy_s(buf, sizeof(buf), "9999-12-31T23:59:59Z");
                    ERR_CHK(rc);
                }
                else
                {
                    t1 += monotonic_offset;
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ%c", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec, '\0');
                }


                rc = STRCPY_S_NOCLOBBER((char*)g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].PreferredLifetime, sizeof(g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].PreferredLifetime), buf);
                ERR_CHK(rc);
            }
        }else if ( _ansc_strcmp(pTmp1, "Valid") == 0 )
        {
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Address )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if ((unsigned int)t1 == 0xffffffff)
                {
                    rc = strcpy_s(buf, sizeof(buf), "9999-12-31T23:59:59Z");
                    ERR_CHK(rc);
                }
                else
                {
                    t1 += monotonic_offset;
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ%c", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec, '\0');
                }


                rc = STRCPY_S_NOCLOBBER((char*)g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].ValidLifetime, sizeof(g_dhcps6v_clientcontent[Index].pIPv6Address[g_dhcps6v_clientcontent[Index].NumberofIPv6Address-1].ValidLifetime), buf);
                ERR_CHK(rc);
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
            
            rc = STRCPY_S_NOCLOBBER((char*)g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].Prefix, sizeof(g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].Prefix), pTmp3);
            ERR_CHK(rc);

        }else if ( _ansc_strcmp(pTmp1, "pdTimestamp") == 0 )
        {
            timeStamp = atoi(pTmp3);
        }else if ( _ansc_strcmp(pTmp1, "pdPrefered") == 0 )
        {
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if ((unsigned int)t1 == 0xffffffff)
                {
                    rc = strcpy_s(buf, sizeof(buf), "9999-12-31T23:59:59Z");
                    ERR_CHK(rc);
                }
                else
                {
                    t1 += monotonic_offset;
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ%c", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec, '\0');
                }

                rc = STRCPY_S_NOCLOBBER((char*)g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].PreferredLifetime, sizeof(g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].PreferredLifetime), buf);
                ERR_CHK(rc);
            }

        }else if ( _ansc_strcmp(pTmp1, "pdValid") == 0 )
        {
            if ( g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix )
            {
                t1 = timeStamp;
                t1 += atoi(pTmp3);

                if ((unsigned int)t1 == 0xffffffff)
                {
                    rc = strcpy_s(buf, sizeof(buf), "9999-12-31T23:59:59Z");
                    ERR_CHK(rc);
                }
                else
                {
                    t1 += monotonic_offset;
                    localtime_r(&t1, &t2);
                    snprintf(buf, sizeof(buf), "%d-%d-%dT%02d:%02d:%02dZ%c", 
                         1900+t2.tm_year, t2.tm_mon+1, t2.tm_mday,
                         t2.tm_hour, t2.tm_min, t2.tm_sec, '\0');
                }

                rc = STRCPY_S_NOCLOBBER((char*)g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].ValidLifetime, sizeof(g_dhcps6v_clientcontent[Index].pIPv6Prefix[g_dhcps6v_clientcontent[Index].NumberofIPv6Prefix-1].ValidLifetime), buf);
                ERR_CHK(rc);
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
    FILE *fp = NULL;
    ULONG     i        = 0;

    for( i =0; i<g_dhcps6v_client_num; i++ )
    {
        if ( _ansc_strcmp((const char*)g_dhcps6v_client[i].SourceAddress, (const char*)pDhcpsClient->SourceAddress) == 0 )
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
    fp = v_secure_popen("r",  "ping6 -w 2 -c 1 %s", g_dhcps6v_clientcontent[i].pIPv6Address[i].IPAddress );
    if (_get_shell_output2(fp, "0 packets received"))
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
        PULONG                      pSize,
        char                       *pInterface
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulPoolInstanceNumber); 

    cosa_dhcpv6_client_info(pInterface);
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulPoolInstanceNumber);
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulPoolInstanceNumber); 
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
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulPoolInstanceNumber); 
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
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext);
    char globalAddress[64] = {0};
    commonSyseventGet(COSA_DML_DHCPV6C_PREF_SYSEVENT_NAME, globalAddress, sizeof(globalAddress));

    strncpy(addr, globalAddress, (unsigned int)len>strlen(globalAddress)?strlen(globalAddress):(unsigned int)len);

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
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext);
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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx             = {0};
    ULONG                           Index   =  0;
    errno_t                         rc      = -1;

    for( Index = 0 ; Index < uDhcpv6ServerPoolNum; Index++ )
    {
        if ( sDhcpv6ServerPool[Index].Cfg.InstanceNumber == ulPoolInstanceNumber )
        {
            sDhcpv6ServerPoolOption[Index][ulIndex].InstanceNumber = ulInstanceNumber;

            rc = STRCPY_S_NOCLOBBER((char*)sDhcpv6ServerPoolOption[Index][ulIndex].Alias, sizeof(sDhcpv6ServerPoolOption[Index][ulIndex].Alias), pAlias);
            ERR_CHK(rc);

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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx             = {0};
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
            setpooloption_into_utopia((PUCHAR)DHCPV6S_NAME,(PUCHAR)"pool",Index,(PUCHAR)"option",Index2,&sDhcpv6ServerPoolOption[Index][Index2]);

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
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx             = {0};
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
	    unsetpooloption_from_utopia((PUCHAR)DHCPV6S_NAME,(PUCHAR)"pool",Index,(PUCHAR)"option",Index2);
#endif
            for( Index2++; Index2  < uDhcpv6ServerPoolOptionNum[Index]; Index2++ )
            {
#if defined (MULTILAN_FEATURE)
                setpooloption_into_utopia((PUCHAR)DHCPV6S_NAME,(PUCHAR)"pool",Index,(PUCHAR)"option",Index2-1,&sDhcpv6ServerPoolOption[Index][Index2]);
#endif
                sDhcpv6ServerPoolOption[Index][Index2-1] =  sDhcpv6ServerPoolOption[Index][Index2];
            }
#if defined (MULTILAN_FEATURE)
            unsetpooloption_from_utopia((PUCHAR)DHCPV6S_NAME,(PUCHAR)"pool",Index,(PUCHAR)"option",Index2-1);
#endif
            uDhcpv6ServerPoolOptionNum[Index]--;
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
    UNREFERENCED_PARAMETER(hContext);
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
                        ( _ansc_strcmp((const char*)sDhcpv6ServerPoolOption[Index][Index2].PassthroughClient, (const char*)pEntry->PassthroughClient ) ||
                        ( _ansc_strcmp((const char*)sDhcpv6ServerPoolOption[Index][Index2].Value, (const char*)pEntry->Value) &&
                                  !_ansc_strlen((const char*)pEntry->PassthroughClient) ) ) )
                    {
                        v_secure_system("sysevent set zebra-restart");
                    }

                    sDhcpv6ServerPoolOption[Index][Index2] = *pEntry;

                    setpooloption_into_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", Index, (PUCHAR)"option", Index2, &sDhcpv6ServerPoolOption[Index][Index2]);
                    
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
    UNREFERENCED_PARAMETER(hContext);
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
    char            out[256];
    char            tmp[8]           = {0};
    FILE *fp = NULL;
    errno_t         rc               = -1;

    rc = strcpy_s( globalIP, sizeof(globalIP), prefix);
    if(rc != EOK)
    {
        ERR_CHK(rc);
        return 1;
    }

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

#ifdef _HUB4_PRODUCT_REQ_
    if(strncmp(intfName, COSA_DML_DHCPV6_SERVER_IFNAME, strlen(intfName)) == 0)
    {
        snprintf(ipAddr, 128, "%s1", globalIP);
        return 0;
    }
#endif

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
    fp = v_secure_popen("r", "ifconfig %s | grep HWaddr", intfName );
    _get_shell_output(fp, out, sizeof(out));
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

    rc = STRCPY_S_NOCLOBBER(ipAddr, 128, globalIP);
    ERR_CHK(rc);
    
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
    char event_value[64] = {0};
#ifdef FEATURE_RDKB_WAN_MANAGER
    commonSyseventGet("ipv6_prefix", event_value, sizeof(event_value));
    if (strlen(event_value) > 3)
    {
        g_dhcpv6_server_prefix_ready = TRUE;
    }
    memset(event_value,0,sizeof(event_value));
#endif
    commonSyseventGet("lan-status", event_value, sizeof(event_value));
    if ( !strncmp(event_value, "started", strlen("started") ) )
    {
        g_lan_ready = TRUE;
    }
    if (!g_dhcpv6_server_prefix_ready || !g_lan_ready)
        return;
#if defined (MULTILAN_FEATURE)
    commonSyseventSet("dhcpv6s-restart", "");
    commonSyseventSet("zebra-restart", "");
#else
    FILE *fp = NULL;
    int fd = 0;
#if !defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
    if (g_dhcpv6s_restart_count) 
#endif
    {
        g_dhcpv6s_restart_count=0;
        //when need stop, it's supposed the configuration file need to be updated.
        _cosa_dhcpsv6_refresh_config();
        CcspTraceInfo(("%s - Call _dibbler_server_operation stop\n",__FUNCTION__));
        _dibbler_server_operation("stop");
    }

    fd = open(DHCPV6S_SERVER_PID_FILE, O_RDONLY);
/* dibbler-server process start fix for HUB4 and ADA */
#if defined (_HUB4_PRODUCT_REQ_)
    FILE *fp_bin = NULL;
    char binbuff[64] = {0};
    fp_bin = v_secure_popen("r","ps|grep %s|grep -v grep", SERVER_BIN);
    _get_shell_output(fp_bin, binbuff, sizeof(binbuff));
    if ((fd < 0) || (!strstr(binbuff, SERVER_BIN))) {
#else
    if (fd < 0) {
#endif
        BOOL isBridgeMode = FALSE;
        char out[128];

/* dibbler-server process start fix for HUB4 and ADA */
#if defined (_HUB4_PRODUCT_REQ_)
        if(fd >= 0)
            close(fd);
#endif
        /*CID: 66130 Unchecked return value*/
        if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&isBridgeMode)) &&
           ( TRUE == isBridgeMode ))
            return;

        //make sure it's not in a bad status
        fp = v_secure_popen("r","busybox ps|grep %s|grep -v grep", SERVER_BIN);
        _get_shell_output(fp, out, sizeof(out));
        if (strstr(out, SERVER_BIN))
        {
            v_secure_system("kill -15 `pidof " SERVER_BIN "`");
	    sleep(1);
        }
        CcspTraceInfo(("%s - Call _dibbler_server_operation start\n",__FUNCTION__));
        _dibbler_server_operation("start");
    } else{
        close(fd);
    }
#endif

    // refresh lan if we were asked to
#if !defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
    if (g_dhcpv6s_refresh_count) 
#endif
    {
        g_dhcpv6s_refresh_count = 0;
        CcspTraceWarning(("%s: DBG calling  gw_lan_refresh\n", __func__));
        v_secure_system("gw_lan_refresh");
    }

    return;
}
/*int calcPrefixNumber(int prefixLen, int req_prefixLen)
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
    errno_t rc = -1;
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
    rc = STRCPY_S_NOCLOBBER(pref, 100, str);
    if(rc != EOK)
    {
        ERR_CHK(rc);
        return 0;
    }
    return 1;
}

int GenIPv6Prefix(char *ifName,char *GlobalPref, char *pref)
{
int index = 0;
char cmd[100] = {0};
char out[100] = {0};
errno_t rc = -1;
static int interface_num = 4; // Reserving first 4 /64s for dhcp configurations
	if(ifName == NULL)
	return 0;

	rc = sprintf_s(cmd, sizeof(cmd), "%s_ipv6_index", ifName);
	if(rc < EOK)
	{
		ERR_CHK(rc);
		return 0;
	}
	commonSyseventGet(cmd, out, sizeof(out));
	if(strlen(out) != 0)
	{
		index = atoi(out);
	}

	rc = STRCPY_S_NOCLOBBER(pref, 100, GlobalPref);
	if(rc != EOK)
	{
		ERR_CHK(rc);
		return 0;
	}
	if(index == 0)
	{
		if(CalcIPv6Prefix(GlobalPref,pref,interface_num)== 0)
		return 0;
		rc = sprintf_s(cmd, sizeof(cmd), "%s_ipv6_index", ifName);
		if(rc < EOK)
		{
			ERR_CHK(rc);
			return 0;
		}
		rc = sprintf_s(out, sizeof(out), "%d",interface_num);
		if(rc < EOK)
		{
			ERR_CHK(rc);
			return 0;
		}
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
int remove_interface(char* Inf_name)
{
	char *token = NULL;char *pt;
	char OutBuff[128],buf[128] ;
	
	memset(OutBuff,0,sizeof(OutBuff));
	
	syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));
	// interface is present in the list, we need to remove interface to disable IPv6 PD
	pt = buf;
   while((token = strtok_r(pt, ",", &pt))) {
	if(strncmp(Inf_name,token,strlen(Inf_name)))
		{
			strcat(OutBuff,token);
			strcat(OutBuff,",");
		}
   }
	syscfg_set_commit(NULL, "IPv6_Interface", OutBuff);
	return 0;
}

int append_interface(char* Inf_name)
{
	/* CID 174287 fix */
	char OutBuff[129],buf[128] ;
	
	memset(OutBuff,0,sizeof(OutBuff));
	
	syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));
	
	strncpy(OutBuff, buf, (sizeof(OutBuff)-1));
	strcat(OutBuff,Inf_name);
	strcat(OutBuff,",");
	syscfg_set_commit(NULL, "IPv6_Interface", OutBuff);
	return 0;
}

int Get_Device_Mode()
{
    int deviceMode = 0;
    char buf[8]= {0};
    memset(buf,0,sizeof(buf));
    if ( 0 == syscfg_get(NULL, "Device_Mode", buf, sizeof(buf)))
    {
        if (buf[0] != '\0' && strlen(buf) != 0 )
            deviceMode = atoi(buf);
    }
    return deviceMode;

}

/* This thread is added to handle the LnF interface IPv6 rule, because LnF is coming up late in XB6 devices. 
This thread can be generic to handle the operations depending on the interfaces. Other interface and their events can be register here later based on requirement */
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_CBR_PRODUCT_REQ_)
#else
static pthread_t InfEvtHandle_tid;
static int sysevent_fd_1;
static token_t sysevent_token_1;

#ifdef RDKB_EXTENDER_ENABLED
#include <sys/stat.h>
#define EXT_MODE_ROUTE_TABLE_NUM 12
#define ULA_ROUTE_SET "/tmp/.ula_route_set"
#define DEF_ULA_PREF_LEN 64

void enable_Ula_IPv6(char* ifname)
{
    CcspTraceInfo(("%s : ENTRY\n",__FUNCTION__));
    char *token_pref = NULL ;
    char buf[128] = {0}, cmd[128] = {0} ;
    char pref_rx[16];
    char ipv6_addr[128]={0};
    memset(buf,0,sizeof(buf));
    commonSyseventGet("ula_ipv6_enabled", buf, sizeof(buf));
    if ( 1 == atoi(buf) )
    {
        CcspTraceInfo(("%s : Enabling ula ipv6 on iface %s\n",__FUNCTION__,ifname));
        memset(cmd,0,sizeof(cmd));
        memset(buf,0,sizeof(buf));
        int pref_len = DEF_ULA_PREF_LEN;
        memset(pref_rx,0,sizeof(pref_rx));
        commonSyseventGet("backup_wan_prefix_v6_len", pref_rx, sizeof(pref_rx));

        if ( strlen(pref_rx) != 0 )
                pref_len = atoi(pref_rx);

        snprintf(cmd,sizeof(cmd),"%s_ipaddr_v6_ula",ifname);
        commonSyseventGet(cmd, buf, sizeof(buf));
        if (buf[0] != '\0' && strlen(buf) != 0 )
        {
            SetV6Route(ifname,buf,0);
            token_pref = strtok(buf,"/");
            memset(ipv6_addr,0,sizeof(ipv6_addr));
            snprintf(ipv6_addr,sizeof(ipv6_addr),"%s:1/%d",token_pref,pref_len);
            AssignIpv6Addr(ifname,ipv6_addr);
        }
    }
}
#endif

void enable_IPv6(char* if_name)
{
        FILE *fp = NULL;

    	CcspTraceInfo(("%s : Enabling ipv6 on iface %s\n",__FUNCTION__,if_name));

    	char tbuff[100], ipv6_addr[128] = {0} , cmd[128] = {0} ;
    	errno_t rc = -1;

	fp = v_secure_popen("r","sysctl net.ipv6.conf.%s.autoconf",if_name);
	_get_shell_output(fp, tbuff, sizeof(tbuff));

    	if(tbuff[strlen(tbuff)-1] == '0')
    	{
            v_secure_system("sysctl -w net.ipv6.conf.%s.autoconf=1",if_name);
            v_secure_system("ifconfig %s down;ifconfig %s up",if_name,if_name);
    	}
        rc = sprintf_s(cmd, sizeof(cmd), "%s_ipaddr_v6",if_name);
        if(rc < EOK)
        {
            ERR_CHK(rc);
        }
    	commonSyseventGet(cmd, ipv6_addr, sizeof(ipv6_addr));
    	v_secure_system("ip -6 route add %s dev %s", ipv6_addr, if_name);
    	#ifdef _COSA_INTEL_XB3_ARM_
        v_secure_system("ip -6 route add %s dev %s table erouter", ipv6_addr, if_name);
    	#endif
    	v_secure_system("ip -6 rule add iif %s lookup erouter",if_name);

        #ifdef RDKB_EXTENDER_ENABLED
            if ( DEVICE_MODE_ROUTER == Get_Device_Mode() && access(ULA_ROUTE_SET, R_OK) == 0 )
            {
                enable_Ula_IPv6(if_name);
            }
        #endif

}

int getprefixinfo(const char *prefix,  char *value, unsigned int val_len, unsigned int *prefix_len)
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
int GenAndUpdateIpv6PrefixIntoSysevent(char *pInfName)
{
    char out1[128] = {0};
    char cmd[256];
    char ipv6_prefix[64] = {0};
    char prefixvalue[INET6_ADDRSTRLEN] = {0};
    int  len =0;
    errno_t rc = -1;

    if (!pInfName)
        return -1;
    commonSyseventGet(COSA_DML_DHCPV6C_PREF_SYSEVENT_NAME, ipv6_prefix, sizeof(ipv6_prefix));

    if (getprefixinfo(ipv6_prefix, prefixvalue, sizeof(prefixvalue), (unsigned int*)&len) != 0) 
    {
      CcspTraceError(("[%s] ERROR return -1 (i.e. error due to get_prefix_info() return -1)\n", __FUNCTION__));
      return -1;
    }
    if(GenIPv6Prefix(pInfName,prefixvalue,out1))
    {
        rc = sprintf_s(cmd, sizeof(cmd), "%s_ipaddr_v6", pInfName);
        if(rc < EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        commonSyseventSet(cmd, out1);
    }
    return 0;
}
int handle_MocaIpv6(char *status)
{
    FILE *fp = NULL;
    char *Inf_name = NULL;
    int retPsmGet, retPsmGet1 = CCSP_SUCCESS;
	char *str = NULL;
	int HomeIsolationEnable = 0;
    char tbuff[100];
    char ipv6If[128], mbuf[128];
    int restart_zebra = 0;
	
    if (!status)
            return -1;

	//checking Homeisolation is enabled and ipv6_moca_bridge is true
	retPsmGet1 = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.HomeNetworkIsolation", NULL, &str);
    if(retPsmGet1 == CCSP_SUCCESS) {
        HomeIsolationEnable = _ansc_atoi(str);
    }
    else
    {
        CcspTraceError(("%s PSM_Get_Record_Value2 failed for dmsb.l2net.HomeNetworkIsolation \n",__FUNCTION__));
        return -1;
    }
    
	syscfg_get( NULL, "ipv6_moca_bridge", mbuf, sizeof(mbuf));
	syscfg_get( NULL, "IPv6_Interface", ipv6If, sizeof(ipv6If));

    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(str);
    retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.9.Name", NULL, &Inf_name);
    if(retPsmGet != CCSP_SUCCESS )
    {
        CcspTraceError(("%s PSM_Get_Record_Value2 failed for dmsb.l2net.9.Name \n",__FUNCTION__));
        return -1;
    }
    if(!retPsmGet)
    {
        retPsmGet = CCSP_SUCCESS;
		if(NULL == Inf_name){
			Inf_name = (char *)AnscAllocateMemory( (strlen("brlan10") + 1) );
			strncpy(Inf_name, "brlan10", strlen("brlan10") +1);
		}
    }
    if(strcmp((const char*)status, "ready") == 0)
    {
        if(( mbuf != NULL ) && ( ipv6If != NULL ) && ( Inf_name != NULL ))
        {
            if( (strcmp(mbuf, "true") == 0) && (HomeIsolationEnable == 1))
            {
                if (!strstr(ipv6If, Inf_name)) {
                    append_interface(Inf_name);
                    GenAndUpdateIpv6PrefixIntoSysevent(Inf_name);
                    restart_zebra = 1;
                }
            }
            else if ( (strcmp(mbuf, "false") == 0) || (HomeIsolationEnable == 0))
            {
                if (strstr(ipv6If, Inf_name)){
                    remove_interface(Inf_name);
                    restart_zebra = 1;
                }
            }
        }
        if (retPsmGet == CCSP_SUCCESS)
        {                      
            memset(tbuff,0,sizeof(tbuff));
            fp = v_secure_popen("r","sysctl net.ipv6.conf.%s.autoconf",Inf_name);
            _get_shell_output(fp, tbuff, sizeof(tbuff));
            if(tbuff[strlen(tbuff)-1] == '0')
            {
                enable_IPv6(Inf_name);
            }

	    /* CID 180991 fix */
	   if(NULL != Inf_name){
		((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
		Inf_name = NULL;
	   }
        }

    }
    if(strcmp((const char*)status, "stopped") == 0)
    {
        if ( (strcmp(mbuf, "false") == 0) || (HomeIsolationEnable == 0))
        {
            if (strstr(ipv6If, Inf_name)) {
                remove_interface(Inf_name);
                restart_zebra = 1;
            }
        }
    }
    if (restart_zebra)
    {
        v_secure_system("sysevent set zebra-restart");
    }
	if(NULL != Inf_name){
    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
	Inf_name = NULL;
	}
    return 0;

}


static void *InterfaceEventHandler_thrd(void *data)
{
    UNREFERENCED_PARAMETER(data);
    async_id_t interface_asyncid;
    async_id_t interface_XHS_asyncid;
    async_id_t interface_POD_asyncid;
    async_id_t interface_MoCA_asyncid;
#if defined (WIFI_MANAGE_SUPPORTED)
    async_id_t interface_WiFi_asyncid;
    char aMultiNetStatus[BUFF_LEN_64] = {0};
    char index[BUFF_LEN_8] = {0};
#endif /*WIFI_MANAGE_SUPPORTED*/
    CcspTraceWarning(("%s started\n",__FUNCTION__));
    sysevent_fd_1 = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "Interface_evt_handler", &sysevent_token_1);

    sysevent_set_options(sysevent_fd_1, sysevent_token_1, "multinet_6-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_1, sysevent_token_1, "multinet_6-status",  &interface_asyncid);
    sysevent_set_options(sysevent_fd_1, sysevent_token_1, "multinet_2-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_1, sysevent_token_1, "multinet_2-status",  &interface_XHS_asyncid);
	sysevent_set_options(sysevent_fd_1, sysevent_token_1, "multinet_10-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_1, sysevent_token_1, "multinet_10-status",  &interface_POD_asyncid);
    sysevent_set_options(sysevent_fd_1, sysevent_token_1, "multinet_9-status", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd_1, sysevent_token_1, "multinet_9-status",  &interface_MoCA_asyncid);

#if defined (WIFI_MANAGE_SUPPORTED)
    psmGet(MANAGE_WIFI_BRIDGE_INDEX, index, BUFF_LEN_8);
    if ('\0' != index[0])
    {
        snprintf (aMultiNetStatus,BUFF_LEN_64,"multinet_%s-status",index);
        CcspTraceInfo(("%s:%d,aMultiNetStatus:%s\n",__FUNCTION__,__LINE__, aMultiNetStatus));
        sysevent_set_options(sysevent_fd_1, sysevent_token_1, aMultiNetStatus, TUPLE_FLAG_EVENT);
        sysevent_setnotification(sysevent_fd_1, sysevent_token_1,aMultiNetStatus,  &interface_WiFi_asyncid);
    }
#endif /*WIFI_MANAGE_SUPPORTED*/

    FILE *fp = NULL;
    char *Inf_name = NULL;
    int retPsmGet = CCSP_SUCCESS;
    char tbuff[100];
    int err;
    char name[25] = {0}, val[42] = {0}, buf[128], cmd[128];
    errno_t rc = -1;
	
    rc = strcpy_s(cmd, sizeof(cmd), "multinet_9-status");
    ERR_CHK(rc);

    buf[0] = 0;
    commonSyseventGet(cmd, buf, sizeof(buf));
            
    CcspTraceWarning(("%s multinet_9-status is %s\n",__FUNCTION__,buf));

    handle_MocaIpv6(buf);

    rc = strcpy_s(cmd, sizeof(cmd), "multinet_10-status");
    ERR_CHK(rc);

    buf[0] = 0;
    commonSyseventGet(cmd, buf, sizeof(buf));
            
    CcspTraceWarning(("%s multinet_10-status is %s\n",__FUNCTION__,buf));

    if(strcmp((const char*)buf, "ready") == 0)
    {
            retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.10.Name", NULL, &Inf_name);
			if(!retPsmGet)
			{
				retPsmGet = CCSP_SUCCESS;
				Inf_name = "br403";		
			}
            if (retPsmGet == CCSP_SUCCESS)
            {                      
                memset(tbuff,0,sizeof(tbuff));
                fp = v_secure_popen("r","sysctl net.ipv6.conf.%s.autoconf",Inf_name);
		_get_shell_output(fp, tbuff, sizeof(tbuff));
                if(tbuff[strlen(tbuff)-1] == '0')
                {
                    enable_IPv6(Inf_name);
                }

                ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
                Inf_name = NULL;
            }
    
    }

    rc = strcpy_s(cmd, sizeof(cmd), "multinet_2-status");
    ERR_CHK(rc);

    buf[0] = 0;
    commonSyseventGet(cmd, buf, sizeof(buf));

    CcspTraceWarning(("%s multinet_2-status is %s\n",__FUNCTION__,buf));

    if(strcmp((const char*)buf, "ready") == 0)
    {
        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.2.Port.1.Name", NULL, &Inf_name);
        if (retPsmGet == CCSP_SUCCESS)
        {
            fp = v_secure_popen("r","sysctl net.ipv6.conf.%s.autoconf",Inf_name);
            _get_shell_output(fp, tbuff, sizeof(tbuff));
            if(tbuff[strlen(tbuff)-1] == '0')
            {
                enable_IPv6(Inf_name);
            }
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
            Inf_name = NULL;
        }

    }

    rc = strcpy_s(cmd, sizeof(cmd), "multinet_6-status");
    ERR_CHK(rc);

    buf[0] = 0;
    commonSyseventGet(cmd, buf, sizeof(buf));

    CcspTraceWarning(("%s multinet_6-status is %s\n",__FUNCTION__,buf));

    if(strcmp((const char*)buf, "ready") == 0)
    {
        fp = v_secure_popen("r","sysctl net.ipv6.conf.br106.autoconf");
        _get_shell_output(fp, tbuff, sizeof(tbuff));
        if(tbuff[strlen(tbuff)-1] == '0')
        {
            enable_IPv6("br106");
        }

    }

    while(1)
    {
        async_id_t getnotification_asyncid;
        memset(name,0,sizeof(name));
        memset(val,0,sizeof(val));
        memset(cmd,0,sizeof(cmd));
        memset(buf,0,sizeof(buf));

        int namelen = sizeof(name);
        int vallen  = sizeof(val);
        err = sysevent_getnotification(sysevent_fd_1, sysevent_token_1, name, &namelen,  val, &vallen, &getnotification_asyncid);

        if (err)
        {
            CcspTraceWarning(("sysevent_getnotification failed with error: %d %s\n", err,__FUNCTION__));
            CcspTraceWarning(("sysevent_getnotification failed name: %s val : %s\n", name,val));
            if ( 0 != v_secure_system("pidof syseventd")) {

                CcspTraceWarning(("%s syseventd not running ,breaking the receive notification loop \n",__FUNCTION__));
                break;
            }	
        }
        else
        {

            CcspTraceWarning(("%s Recieved notification event  %s\n",__FUNCTION__,name));
            if(strcmp((const char*)name,"multinet_6-status") == 0)
            {
                if(strcmp((const char*)val, "ready") == 0)
                {
                    enable_IPv6("br106");
                }
            }

            if(strcmp((const char*)name,"multinet_2-status") == 0)
            {
                if(strcmp((const char*)val, "ready") == 0)
                {
                    Inf_name = NULL ;
                    retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.2.Port.1.Name", NULL, &Inf_name);
                    if (retPsmGet == CCSP_SUCCESS)
                    {               
                        enable_IPv6(Inf_name);
                        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
                    }
                    else
                    {
                        CcspTraceWarning(("%s PSM get failed for interface name\n", __FUNCTION__));
                    }
                }
            }
			
			if(strcmp((const char*)name,"multinet_10-status") == 0)
			{
				if(strcmp((const char*)val, "ready") == 0)
				{
                    Inf_name = NULL ;
					retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.10.Name", NULL, &Inf_name);
					if(!retPsmGet)
					{
						retPsmGet = CCSP_SUCCESS;
						Inf_name = "br403";		
					}
            		if (retPsmGet == CCSP_SUCCESS)
					{               
                        enable_IPv6(Inf_name);
						((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
					}
					else
					{
						CcspTraceWarning(("%s PSM get failed for interface name\n", __FUNCTION__));
					}
				}

			}	

            if(strcmp((const char*)name,"multinet_9-status") == 0)
            {
                handle_MocaIpv6(val);

            }
#if defined (WIFI_MANAGE_SUPPORTED)
            if((0 == strcmp((const char*)name, aMultiNetStatus)) && (0 == strcmp((const char*)val, "ready")))
            {
                char aParamName[BUFF_LEN_64] = {0};
                char aParamVal[BUFF_LEN_64] = {0};
                char aBridgeName[BUFF_LEN_64] = {0};
                snprintf(aParamName, BUFF_LEN_64, "dmsb.l2net.%s.Name", index);
                psmGet(aParamName, aParamVal, BUFF_LEN_64);
                CcspTraceInfo(("BridgeName:%s\n", aParamVal));
                if ('\0' != aParamVal[0])
                {
                    strcpy(aBridgeName,aParamVal);
                }
                else
                {
                    strcpy(aBridgeName,"brlan15");
                }
                snprintf(aParamName, BUFF_LEN_64, "dmsb.l3net.%s.IPv6Enable", index);
                psmGet(aParamName, aParamVal, BUFF_LEN_64);
                if (('\0' != aParamVal[0]) && (!strncmp(aParamVal, "true", 4)))
                {
                    if (!GenAndUpdateIpv6PrefixIntoSysevent(aBridgeName))
                    {
                        enable_IPv6(aBridgeName);
                    }
                }
            }
#endif /*WIFI_MANAGE_SUPPORTED*/
        }
    }
    return NULL;
}
#endif

#if defined (RDKB_EXTENDER_ENABLED) || defined (WAN_FAILOVER_SUPPORTED)

#define CELLULAR_IFNAME "cellular_ifname"
#define SYSEVENT_CM_WAN_V6_IPADDR_PREV "cellular_wan_v6_ip_prev"
#define SYSEVENT_CM_WAN_V6_GWADDR   "cellular_wan_v6_gw"
#define SYSEVENT_CM_WAN_V6_GWADDR_PREV   "cellular_wan_v6_gw_prev"

void AssignIpv6Addr(char* ifname , char* ipv6Addr)
{      
    CcspTraceInfo(("%s : interface name=%s , ipv6 address=%s\n", __FUNCTION__, ifname,ipv6Addr));
    v_secure_system("ip -6 addr add %s dev %s", ipv6Addr,ifname);
}

void DelIpv6Addr(char* ifname , char* ipv6Addr)
{
    CcspTraceInfo(("%s : interface name=%s , ipv6 address=%s\n", __FUNCTION__, ifname,ipv6Addr));
    v_secure_system("ip -6 addr del %s dev %s", ipv6Addr,ifname);
}

void SetV6Route(char* ifname , char* route_addr,int metric_val)
{
    CcspTraceInfo(("%s : interface name=%s ,route_addr=%s, metric_val=%d\n", __FUNCTION__, ifname,route_addr,metric_val));
    if ( 0 == metric_val )
        v_secure_system("ip -6 route add default via %s dev %s ", route_addr,ifname);
    else
        v_secure_system("ip -6 route add default via %s dev %s metric %d", route_addr,ifname,metric_val);

}
void UnSetV6Route(char* ifname , char* route_addr,int metric_val)
{
    CcspTraceInfo(("%s : interface name=%s ,route_addr=%s, metric_val=%d\n", __FUNCTION__, ifname,route_addr,metric_val));
    if ( 0 == metric_val )
        v_secure_system("ip -6 route del default via %s dev %s", route_addr,ifname);
    else
        v_secure_system("ip -6 route del default via %s dev %s metric %d", route_addr,ifname,metric_val);

}

void SetV6RouteTable(char* ifname , char* route_addr,int metric_val,int table_num)
{
    CcspTraceInfo(("%s: interface name=%s ,route_addr=%s,metric_val=%d, table_num=%d\n", __FUNCTION__, ifname,route_addr,metric_val,table_num));
    if ( 0 == metric_val )
        v_secure_system("ip -6 route add default via %s dev %s table %d", route_addr,ifname,table_num);
    else
        v_secure_system("ip -6 route add default via %s dev %s metric %d table %d", route_addr,ifname,metric_val,table_num);

}
void UnSetV6RouteFromTable(char* ifname , char* route_addr,int metric_val, int table_num)
{
    CcspTraceInfo(("%s: interface name=%s ,route_addr=%s,metric_val=%d, table_num=%d\n", __FUNCTION__, ifname,route_addr,metric_val,table_num));
    if ( 0 == metric_val )
        v_secure_system("ip -6 route del default via %s dev %s table %d", route_addr,ifname,table_num);
    else
        v_secure_system("ip -6 route del default via %s dev %s metric %d table %d", route_addr,ifname,metric_val,table_num);
}

#endif

#ifdef WAN_FAILOVER_SUPPORTED

#define MESH_WAN_IFNAME "dmsb.Mesh.WAN.Interface.Name"
#define MESH_WAN_WAN_IPV6ADDR "MeshWANInterface_UlaAddr"
#define MESH_REMWAN_WAN_IPV6ADDR "MeshRemoteWANInterface_UlaAddr"

void getMeshWanIfName(char *mesh_wan_ifname,int size)
{
    char* val=NULL;
    if (PSM_Get_Record_Value2(bus_handle, g_Subsystem,MESH_WAN_IFNAME , NULL, &val) != CCSP_SUCCESS )
    {
        return;
    }
    if (val)
    {
        snprintf(mesh_wan_ifname,size,"%s",val);
        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(val);

    }
    CcspTraceWarning(("%s :Mesh WAN IfName is (%s)\n", __FUNCTION__,mesh_wan_ifname));   
    return;      
}

void addRemoteWanIpv6Route()
{
    if (DEVICE_MODE_ROUTER == Get_Device_Mode())
    {
        char mesh_wan_status[8] = {0};

        commonSyseventGet("mesh_wan_linkstatus", mesh_wan_status, sizeof(mesh_wan_status));

            if(strncmp(mesh_wan_status,"up",2) == 0 )
            {
                char mesh_wan_ifname[32] = {0};
                char ipv6_address[128] = {0};
                getMeshWanIfName(mesh_wan_ifname,sizeof(mesh_wan_ifname));
                if (mesh_wan_ifname[0] != '\0') 
                {
                    memset(ipv6_address,0,sizeof(ipv6_address));

                    commonSyseventGet(MESH_WAN_WAN_IPV6ADDR, ipv6_address, sizeof(ipv6_address));
                    if( '\0' != ipv6_address[0] ) 
                    {
                        v_secure_system("ip -6 route del default");
                        SetV6Route(mesh_wan_ifname,strtok(ipv6_address,"/"),1025);
                        commonSyseventSet("remotewan_routeset", "true");
                    }
                }
            }

    }
}
void addIpv6toRemoteWan()
{                    
    if (DEVICE_MODE_ROUTER == Get_Device_Mode())
    {
        char mesh_wan_ifname[32] = {0};
        char ipv6_address[128] = {0};
        getMeshWanIfName(mesh_wan_ifname,sizeof(mesh_wan_ifname));
        if (mesh_wan_ifname[0] != '\0') 
        {
                memset(ipv6_address,0,sizeof(ipv6_address));
                commonSyseventGet(MESH_REMWAN_WAN_IPV6ADDR, ipv6_address, sizeof(ipv6_address));
                if( '\0' != ipv6_address[0] ) 
                {
                    AssignIpv6Addr(mesh_wan_ifname,ipv6_address); 
                }
        }
    }
}

void delIpv6toRemoteWan()
{
    if (DEVICE_MODE_ROUTER == Get_Device_Mode())
    {
        char mesh_wan_ifname[32] = {0};
        char ipv6_address[128] = {0};
        getMeshWanIfName(mesh_wan_ifname,sizeof(mesh_wan_ifname));
        if (mesh_wan_ifname[0] != '\0') 
        {
                memset(ipv6_address,0,sizeof(ipv6_address));
                commonSyseventGet(MESH_REMWAN_WAN_IPV6ADDR, ipv6_address, sizeof(ipv6_address));
                if( '\0' != ipv6_address[0] ) 
                {
                    DelIpv6Addr(mesh_wan_ifname,ipv6_address); 
                } 
            }
    }
}
#endif


#if defined (RDKB_EXTENDER_ENABLED)

void  deleteIpv6Route(char* ifname)
{
    char addr[128]={0};
    memset(addr,0,sizeof(addr));
    commonSyseventGet(SYSEVENT_CM_WAN_V6_GWADDR_PREV, addr, sizeof(addr));
    UnSetV6RouteFromTable(ifname,strtok(addr,"/"),0,EXT_MODE_ROUTE_TABLE_NUM);  
    UnSetV6Route(ifname,strtok(addr,"/"),0);  
}

void  addIpv6Route(char* ifname,uint32_t DeviceMode)
{
    char addr[128]={0};
    char buf[16] = {0};
    memset(addr,0,sizeof(addr));
    commonSyseventGet(SYSEVENT_CM_WAN_V6_GWADDR, addr, sizeof(addr));
    commonSyseventSet(SYSEVENT_CM_WAN_V6_GWADDR_PREV, addr);
    if ( DEVICE_MODE_EXTENDER == DeviceMode )
        SetV6RouteTable(ifname,strtok(addr,"/"),0,EXT_MODE_ROUTE_TABLE_NUM);
    else
    {
        SetV6Route(ifname,strtok(addr,"/"),0);
        memset(buf,0,sizeof(buf));
        commonSyseventGet("ula_ipv6_enabled", buf, sizeof(buf));  
        if ((access(ULA_ROUTE_SET, R_OK)) != 0 && 1 == atoi(buf) )
        {
            CcspTraceInfo(("%s: ula enabled,creating ula ipv6 configuration in router mode of EXT device\n", __FUNCTION__));
            commonSyseventSet("routeset-ula","");
            creat(ULA_ROUTE_SET,S_IRUSR |S_IWUSR | S_IRGRP | S_IROTH);
        }
    }
}
void configureIpv6Route(uint32_t DeviceMode)
{
    CcspTraceInfo(("%s: Configuring Ipv6 route:\n", __FUNCTION__));
    char cellular_ifname[32]={0};
    memset(cellular_ifname,0,sizeof(cellular_ifname));
    commonSyseventGet(CELLULAR_IFNAME, cellular_ifname, sizeof(cellular_ifname));

    deleteIpv6Route(cellular_ifname);
    addIpv6Route(cellular_ifname,DeviceMode);
}

void configureLTEIpv6(char* v6addr)
{
    CcspTraceInfo(("%s: Configuring LTE Ipv6:\n", __FUNCTION__));

    char cellular_ifname_val[32]={0};
    char addr[128]={0};

    CcspTraceInfo(("%s : v6addr: %s\n", __FUNCTION__, v6addr));
    memset(cellular_ifname_val,0,sizeof(cellular_ifname_val));
    commonSyseventGet(CELLULAR_IFNAME, cellular_ifname_val, sizeof(cellular_ifname_val));
    CcspTraceInfo(("%s : cellular_ifname_val: %s\n", __FUNCTION__, cellular_ifname_val));

    memset(addr,0,sizeof(addr));
    commonSyseventGet(SYSEVENT_CM_WAN_V6_IPADDR_PREV, addr, sizeof(addr));
    DelIpv6Addr(cellular_ifname_val , addr);
    int deviceMode = -1 ;
    deviceMode = Get_Device_Mode();
    // Delete ipv6 route
    deleteIpv6Route(cellular_ifname_val);
    memset(addr,0,sizeof(addr));
    AssignIpv6Addr(cellular_ifname_val,v6addr);
    commonSyseventSet(SYSEVENT_CM_WAN_V6_IPADDR_PREV, v6addr);
    // setIpv6 route
    addIpv6Route(cellular_ifname_val,(uint32_t)deviceMode);
    CcspTraceInfo(("%s -- EXIT:\n", __FUNCTION__));
}
#endif

static void * 
dhcpv6s_dbg_thrd(void * in)
{
    UNREFERENCED_PARAMETER(in);
    int v6_srvr_fifo_file_dscrptr=0;
    char msg[1024] = {0};
    fd_set rfds;
    struct timeval tm;

    v6_srvr_fifo_file_dscrptr = open(DHCPS6V_SERVER_RESTART_FIFO, O_RDWR);

    if (v6_srvr_fifo_file_dscrptr< 0)
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
        FD_SET(v6_srvr_fifo_file_dscrptr, &rfds);

        retCode = select(v6_srvr_fifo_file_dscrptr+1, &rfds, NULL, NULL, &tm);
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

        /* We need consume the data.
	 * It's possible more than one triggering events are consumed in one time, which is expected.*/
        if (FD_ISSET(v6_srvr_fifo_file_dscrptr, &rfds)) {
            /* This sleep help do two things:
             * When GUI operate too fast, it gurantees more operations combine into one;
             * Not frequent dibbler start/stop. When do two start fast, dibbler will in bad status.
             */
            sleep(3);
            memset(msg, 0, sizeof(msg));
            read(v6_srvr_fifo_file_dscrptr, msg, sizeof(msg));
#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
#if !(defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_CBR_PRODUCT_REQ_))
            CcspTraceInfo(("%s %d check IPv6subPrefix  \n", __FUNCTION__, __LINE__));
            char IPv6pref[256] = {0};
            char InterfaceList[128] = {0};
            char interface_name[32] = {0};
            char sysEventOut[64] = {0};
            char *token = NULL ,*pt = NULL;
            FILE *fp = NULL;
            int pref_len = 0;
            static bool InterfaceEventHandleStarted = false;
            errno_t rc = -1;

            commonSyseventGet("ipv6_prefix", IPv6pref, sizeof(IPv6pref));
            /* Remove Prefix length from IPv6pref */
            char *ptr = strchr(IPv6pref, '/');
            if (ptr != NULL) {
                *ptr = '\0';
            }

            commonSyseventGet("lan_prefix_v6", sysEventOut, sizeof(sysEventOut));
            pref_len = atoi(sysEventOut);

            if(pref_len < 64)
            {
                fp = v_secure_popen("r","syscfg get IPv6subPrefix");
                _get_shell_output(fp, sysEventOut, sizeof(sysEventOut));
                if(!strcmp(sysEventOut,"true"))
                {
                    fp = v_secure_popen("r","syscfg get IPv6_Interface");
                    _get_shell_output(fp, InterfaceList, sizeof(InterfaceList));
                    pt = InterfaceList;

                    while((token = strtok_r(pt, ",", &pt)))
                    {
                        char InterfacePrefix[256] ={0};
                        if(GenIPv6Prefix(token,IPv6pref,InterfacePrefix))
                        {
                            memset(interface_name,0,sizeof(interface_name));
                            strncpy(interface_name,token,sizeof(interface_name)-1);
#ifdef _COSA_INTEL_XB3_ARM_
                            char LnFIfName[32] = {0} , LnFBrName[32] = {0} ;
                            syscfg_get( NULL, "iot_ifname", LnFIfName, sizeof(LnFIfName));
                            syscfg_get( NULL, "iot_brname", LnFBrName, sizeof(LnFBrName));
                            if (strcmp((const char*)token,LnFIfName) == 0 && (LnFBrName[0] != '\0' ) && ( strlen(LnFBrName) != 0 ))
                            {
                                memset(interface_name,0,sizeof(interface_name));
                                strncpy(interface_name,LnFBrName,sizeof(interface_name)-1);
                            }
#endif
                            memset(sysEventOut,0,sizeof(sysEventOut));
                            rc = sprintf_s(sysEventOut, sizeof(sysEventOut), "%s_ipaddr_v6", interface_name);
                            if(rc < EOK)
                            {
                                ERR_CHK(rc);
                            }
                            commonSyseventSet(sysEventOut, InterfacePrefix);
                            enable_IPv6(interface_name);
                            commonSyseventSet("zebra-restart","");
                        }
                    }
                    if(!InterfaceEventHandleStarted)
                    {
                        InterfaceEventHandleStarted = true;
                        pthread_create(&InfEvtHandle_tid, NULL, InterfaceEventHandler_thrd, NULL);
                    }
                }
            }
#endif
#endif /* FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE */

            CosaDmlDhcpv6sRebootServer();
            continue;
        }
    }

EXIT:
    if(v6_srvr_fifo_file_dscrptr>=0) {
        close(v6_srvr_fifo_file_dscrptr);
    }

    return NULL;
}

/*******************************************************
* Function Name : isDropbearRunningWithIpv6 (char *pIpv6Addr)
*      It will verify, dropbear process is running with provided IPv6 address or not
*      if dropbear process is running with different IPv6 address other than provided
*      pIpv6Addr then it will restart the sshd-service
*
* Parameter[in] : pIpv6Addr
*      New IPv6Address of erouter0 interface
*
* @retval : void
*
*******************************************************/
static void isDropbearRunningWithIpv6(char * pIpv6Addr)
{
#define LINE_LENGTH 1024
    char arryOfDropbearPids[LINE_LENGTH] = {0};
    char pathToCmdline[LINE_LENGTH] = {0};
    char cmdLineArgs[LINE_LENGTH] = {0};
    char *pDropbearPid = NULL;
    int indexOfCmdLineArg=0, indexOfeRouterIpv6Str = 0;
    bool DropbearPidWithIpv6 = false;
    int dropbearPidNo = 0;
    FILE *fpToDropbearPids = NULL;

    if (NULL == pIpv6Addr)
    {
        CcspTraceWarning(("%s -- NULL parameter passed", __FUNCTION__));
        return;
    }

    fpToDropbearPids = v_secure_popen("r", "ps ww | grep 'dropbear -E -s -b /etc/sshbanner.txt' | grep -v grep");

    if (NULL == fpToDropbearPids)
    {
        CcspTraceWarning(("%s -- Failed to open the pipe for dropbear pids: %s", __FUNCTION__, strerror(errno)));
        return;
    }

    if (NULL != fgets(arryOfDropbearPids,LINE_LENGTH,fpToDropbearPids))
    {
        pDropbearPid = strtok(arryOfDropbearPids," ");

        while((NULL != pDropbearPid) && (true != DropbearPidWithIpv6))
        {
            dropbearPidNo = atoi(pDropbearPid);
            sprintf(pathToCmdline,"/proc/%d/cmdline",dropbearPidNo);
            int fileDescriptor = open (pathToCmdline, O_RDONLY);
            if (-1 != fileDescriptor)
            {
                int noOfBytesRead = read(fileDescriptor, cmdLineArgs, LINE_LENGTH);
                close(fileDescriptor);
                for (indexOfCmdLineArg = 0, indexOfeRouterIpv6Str = 0; indexOfCmdLineArg < noOfBytesRead; indexOfCmdLineArg++)
                {
                    if (pIpv6Addr[indexOfeRouterIpv6Str] == cmdLineArgs[indexOfCmdLineArg])
                    {
                        indexOfeRouterIpv6Str++;
                        if (pIpv6Addr[indexOfeRouterIpv6Str] == '\0')
                        {
                            DropbearPidWithIpv6 = true;
                            break;
                        }
                    }
                    else
                    {
                        indexOfeRouterIpv6Str=0;
                    }
                }
            }
            memset(pathToCmdline, 0, LINE_LENGTH);
            memset(cmdLineArgs,0, LINE_LENGTH);
            pDropbearPid = strtok (NULL , " ");
        }

        if (true != DropbearPidWithIpv6)
        {
            CcspTraceInfo(("%s --Dropbear process is NOT running with %s, sshd , Hence restart the dropbear process\n",__FUNCTION__, pIpv6Addr));
            v_secure_system("sh /etc/utopia/service.d/service_sshd.sh sshd-restart");
        }
        else
        {
                CcspTraceInfo (("%s - Dropbear process is running with eRouter0Ipv6:%s\n", __FUNCTION__, pIpv6Addr));
        }
    }
    v_secure_pclose(fpToDropbearPids);
    fpToDropbearPids = NULL;
}

static void * 
dhcpv6c_dbg_thrd(void * in)
{
    UNREFERENCED_PARAMETER(in);
    int fd=0 ;
    char msg[1024];
    char * p = NULL;
    char globalIP2[128] = {0};
    char *pntr = NULL;
    char *lastentry = NULL;
    //When PaM restart, this is to get previous addr.
    commonSyseventGet("lan_ipaddr_v6", globalIP2, sizeof(globalIP2));
    if ( globalIP2[0] )
        CcspTraceWarning(("%s  It seems there is old value(%s)\n", __FUNCTION__, globalIP2));

    sysevent_fd_global = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "sysevent dhcpv6", &sysevent_token_global);
   
    CcspTraceWarning(("%s sysevent_fd_global is %d\n", __FUNCTION__, sysevent_fd_global));
 
    char lan_multinet_state[16] ;
    
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

    while (1) 
    {
	int retCode = 0;
        tm.tv_sec  = 60;
        tm.tv_usec = 0;
    
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

	retCode = select(fd+1, &rfds, NULL, NULL, &tm);
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

        if ( FD_ISSET(fd, &rfds) )
        {
             msg[0] = 0;
             read(fd, msg, sizeof(msg));
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

            char action[64] = {0};
#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
            char IfaceName[64] = {0};
#endif
            char * pString = NULL;
            char objName[128] = {0};
            errno_t rc = -1;

            int  ret = 0;
            char globalIP[128] = {0};
            BOOL bRestartLan = FALSE;

#if defined(FEATURE_RDKB_WAN_MANAGER)
            int hub4_valid_lft = 0;
            int hub4_preferred_lft = 0;
#else
#if defined(_HUB4_PRODUCT_REQ_) 
            char ula_address[64] = {0};
            char hub4_valid_lft[64] = {0};
            char hub4_preferred_lft[64] = {0};
#endif
#endif

#if defined(FEATURE_MAPT) && defined(FEATURE_RDKB_WAN_MANAGER)
            int dataLen = 0;
            char preflen[12] = {0};
            char brIPv6Prefix[128] = {0};
            char ruleIPv4Prefix[32] = {0};
            char ruleIPv6Prefix[128] = {0};
            char pdIPv6Prefix[128] = {0};
            char mapAssigned[8] = {0};
            char v4Len[8] = {0};
            char v6Len[8] = {0};
            char eaLen[8] = {0};
            char psidOffset[8] = {0};
            char psidLen[8] = {0};
            char psid[8] = {0};
            char isFMR[8] = {0};
#endif
#if defined (FEATURE_SUPPORT_MAPT_NAT46)
            unsigned char  opt95_dBuf[BUFLEN_256] = {0};
            char  pdV6pref[BUFLEN_64] = {0};
#endif
            /*the format is :
             add 2000::ba7a:1ed4:99ea:cd9f :: 0 t1
             action, address, prefix, pref_len 3600
            now action only supports "add", "del"*/

            pntr = msg;
            /*dibbler-client may have multiple ipv6 addresses. So parse the latest ipv6 address*/
            while((pntr = strstr(pntr, "dibbler-client")) != NULL )
            {
               lastentry = pntr;
               pntr++;
            }
            p = lastentry;
            p = p+strlen("dibbler-client");

            while(isblank(*p)) p++;

            fprintf(stderr, "%s -- %d !!! get event from v6 client: %s \n", __FUNCTION__, __LINE__,p);

#if defined(FEATURE_MAPT) && defined(FEATURE_RDKB_WAN_MANAGER)
#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
            dataLen = sscanf(p, "%63s %63s %63s %s %s %s %s %s %63s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                       action, IfaceName, v6addr,    iana_iaid, iana_t1, iana_t2, iana_pretm, iana_vldtm,
                       v6pref, preflen, iapd_iaid, iapd_t1, iapd_t2, iapd_pretm, iapd_vldtm,
                       mapAssigned, ruleIPv6Prefix, ruleIPv4Prefix, brIPv6Prefix, v6Len, isFMR, eaLen, v4Len,
                       psidOffset, psidLen, psid );

            /* dataLen = 25 : MAPT 16:1
             * dataLen = 22 : MAPT 1:1
             * dataLen = 14 : NON-MAPT */
            if((dataLen == 26) || (dataLen == 23) || (dataLen == 15))
#else
            dataLen = sscanf(p, "%63s %63s %s %s %s %s %s %63s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                       action, v6addr,    iana_iaid, iana_t1, iana_t2, iana_pretm, iana_vldtm,
                       v6pref, preflen, iapd_iaid, iapd_t1, iapd_t2, iapd_pretm, iapd_vldtm,
                       mapAssigned, ruleIPv6Prefix, ruleIPv4Prefix, brIPv6Prefix, v6Len, isFMR, eaLen, v4Len,
                       psidOffset, psidLen, psid );

            /* dataLen = 25 : MAPT 16:1
             * dataLen = 22 : MAPT 1:1
             * dataLen = 14 : NON-MAPT */
            if((dataLen == 25) || (dataLen == 22) || (dataLen == 14))
#endif
#elif defined (FEATURE_SUPPORT_MAPT_NAT46)
#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
            if (sscanf(p, "%63s %63s %63s %s %s %s %s %s %63s %d %s %s %s %s %s %s",
                       action, IfaceName, v6addr,    iana_iaid, iana_t1, iana_t2, iana_pretm, iana_vldtm,
                       v6pref, &pref_len, iapd_iaid, iapd_t1, iapd_t2, iapd_pretm, iapd_vldtm,
                       opt95_dBuf ) == 16)
#else
            if (sscanf(p, "%63s %63s %s %s %s %s %s %63s %d %s %s %s %s %s %s",
                       action, v6addr,    iana_iaid, iana_t1, iana_t2, iana_pretm, iana_vldtm,
                       v6pref, &pref_len, iapd_iaid, iapd_t1, iapd_t2, iapd_pretm, iapd_vldtm,
                       opt95_dBuf ) == 15)
#endif
#else // FEATURE_MAPT
#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
            if (sscanf(p, "%63s %63s %63s %s %s %s %s %s %63s %d %s %s %s %s %s", 
                       action, IfaceName, v6addr,    iana_iaid, iana_t1, iana_t2, iana_pretm, iana_vldtm,
                       v6pref, &pref_len, iapd_iaid, iapd_t1, iapd_t2, iapd_pretm, iapd_vldtm ) == 15)
#else
            if (sscanf(p, "%63s %63s %s %s %s %s %s %63s %d %s %s %s %s %s", 
                       action, v6addr,    iana_iaid, iana_t1, iana_t2, iana_pretm, iana_vldtm,
                       v6pref, &pref_len, iapd_iaid, iapd_t1, iapd_t2, iapd_pretm, iapd_vldtm ) == 14)
#endif
#endif
            {
                pString = (char*)CosaUtilGetFullPathNameByKeyword
                    (
                        (PUCHAR)"Device.IP.Interface.",
                        (PUCHAR)"Name",
                        (PUCHAR)COSA_DML_DHCPV6_CLIENT_IFNAME
                        ); 
                if (!strncmp(action, "add", 3))
                {
                    CcspTraceInfo(("%s: add\n", __func__));

// Waiting until private lan interface is ready , so that we can assign global ipv6 address and also start dhcp server.
			while (1)
			{

			        memset(lan_multinet_state,0,sizeof(lan_multinet_state));
                    		return_val=sysevent_get(sysevent_fd_global, sysevent_token_global, "multinet_1-status", lan_multinet_state, sizeof(lan_multinet_state));
			
                        	CcspTraceWarning(("%s multinet_1-status is %s, ret val is %d\n",__FUNCTION__,lan_multinet_state,return_val));

				if(strcmp((const char*)lan_multinet_state, "ready") == 0)
				{
					break;
				}
			
				sleep(5);
			  }
#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
            /*Move IPv6 handle to WanManager*/
#if defined(FEATURE_RDKB_WAN_MANAGER)
            /*
             * Send data to wanmanager.
             */

            char dns_server[256] = {'\0'};
            char sysEventName[256];
            remove_single_quote(iapd_pretm);
            remove_single_quote(iapd_vldtm);
            sscanf(iapd_pretm, "%d", &hub4_preferred_lft);
            sscanf(iapd_vldtm, "%d", &hub4_valid_lft);
            ipc_dhcpv6_data_t dhcpv6_data;
            memset(&dhcpv6_data, 0, sizeof(ipc_dhcpv6_data_t));

            strncpy(dhcpv6_data.ifname, IfaceName, sizeof(dhcpv6_data.ifname));
            if(strlen(v6pref) == 0) {
                dhcpv6_data.isExpired = TRUE;
            } else {
                dhcpv6_data.isExpired = FALSE;
                dhcpv6_data.prefixAssigned = TRUE;
#if defined(FEATURE_MAPT)
                    remove_single_quote(v6pref);
                    strcpy(pdIPv6Prefix, v6pref);
                    remove_single_quote(preflen);
                    pref_len=atoi(preflen);
#endif
                rc = sprintf_s(dhcpv6_data.sitePrefix, sizeof(dhcpv6_data.sitePrefix), "%s/%d", v6pref, pref_len);
                if(rc < EOK)
                {
                    ERR_CHK(rc);
                }
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName,  dhcpv6_data.sitePrefix);

                strncpy(dhcpv6_data.pdIfAddress, "", sizeof(dhcpv6_data.pdIfAddress));
                /** DNS servers. **/
                commonSyseventGet(SYSEVENT_FIELD_IPV6_DNS_SERVER, dns_server, sizeof(dns_server));
                if (strlen(dns_server) != 0)
                {
                    dhcpv6_data.dnsAssigned = TRUE;
                    sscanf (dns_server, "%s %s", dhcpv6_data.nameserver, dhcpv6_data.nameserver1);
                }
                dhcpv6_data.prefixPltime = hub4_preferred_lft;
                dhcpv6_data.prefixVltime = hub4_valid_lft;
                dhcpv6_data.maptAssigned = FALSE;
                dhcpv6_data.mapeAssigned = FALSE;
                dhcpv6_data.prefixCmd = 0;
            }
#ifdef FEATURE_MAPT
            remove_single_quote(brIPv6Prefix);
            remove_single_quote(ruleIPv4Prefix);
            remove_single_quote(ruleIPv6Prefix);
            remove_single_quote(mapAssigned);
            remove_single_quote(v4Len);
            remove_single_quote(v6Len);
            remove_single_quote(eaLen);
            remove_single_quote(psidOffset);
            remove_single_quote(psidLen);
            remove_single_quote(psid);
            remove_single_quote(isFMR);

            if(!strncmp(mapAssigned, "MAPT", 4))
                dhcpv6_data.maptAssigned = TRUE;
            else if(!strncmp(mapAssigned, "MAPE", 4))
                dhcpv6_data.mapeAssigned = TRUE;

            if((dhcpv6_data.maptAssigned == TRUE) || (dhcpv6_data.mapeAssigned == TRUE))
            {
                strncpy(dhcpv6_data.mapt.pdIPv6Prefix, pdIPv6Prefix, sizeof(pdIPv6Prefix));

                strncpy(dhcpv6_data.mapt.ruleIPv6Prefix, ruleIPv6Prefix, sizeof(ruleIPv6Prefix));
                if(strlen(ruleIPv6Prefix) == 0) {
                    CcspTraceError(("[%s-%d] MAPT Rule_V6_Prefix is Empty \n", __FUNCTION__, __LINE__));
                }

                strncpy(dhcpv6_data.mapt.brIPv6Prefix, brIPv6Prefix, sizeof(brIPv6Prefix));
                if(strlen(brIPv6Prefix) == 0) {
                    CcspTraceError(("[%s-%d] MAPT Br_V6_Prefix is Empty \n", __FUNCTION__, __LINE__));
                }

                strncpy(dhcpv6_data.mapt.ruleIPv4Prefix, ruleIPv4Prefix, sizeof(ruleIPv4Prefix));
                if(strlen(ruleIPv4Prefix) == 0) {
                    CcspTraceError(("[%s-%d] MAPT Rule_V4_Prefix is Empty \n", __FUNCTION__, __LINE__));
                }

                dhcpv6_data.mapt.iapdPrefixLen = pref_len;
                dhcpv6_data.mapt.v6Len = atoi(v6Len);
                dhcpv6_data.mapt.isFMR = atoi(isFMR);
                dhcpv6_data.mapt.eaLen = atoi(eaLen);
                dhcpv6_data.mapt.v4Len = atoi(v4Len);
                dhcpv6_data.mapt.psidOffset = atoi(psidOffset);
                dhcpv6_data.mapt.psidLen = atoi(psidLen);
                dhcpv6_data.mapt.psid = atoi(psid);
                dhcpv6_data.mapt.ratio = 1 << (dhcpv6_data.mapt.eaLen - (32 - dhcpv6_data.mapt.v4Len));
            }

#endif //FEATURE_MAPT
            if (send_dhcp_data_to_wanmanager(&dhcpv6_data) != ANSC_STATUS_SUCCESS) {
                CcspTraceError(("[%s-%d] Failed to send dhcpv6 data to wanmanager!!! \n", __FUNCTION__, __LINE__));
            }
            g_dhcpv6_server_prefix_ready = TRUE;

            /* Set Interface specific sysevnts. This is used for Ip interface DM */
            if (iana_iaid[0] != '\0') {
                remove_single_quote(iana_iaid);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_ADDR_IAID_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName,  iana_iaid);
            }
            if (iana_t1[0] != '\0') {
                remove_single_quote(iana_t1);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_ADDR_T1_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName,    iana_t1);
            }
            if (iana_t2[0] != '\0') {
                remove_single_quote(iana_t2);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_ADDR_T2_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName,    iana_t2);
            }
            if (iana_pretm[0] != '\0') {
                remove_single_quote(iana_pretm);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_ADDR_PRETM_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName, iana_pretm);
            }
            if (iana_vldtm[0] != '\0') {
                remove_single_quote(iana_vldtm);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_ADDR_VLDTM_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName, iana_vldtm);
            }

            if (iapd_iaid[0] != '\0') {
                remove_single_quote(iapd_iaid);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_IAID_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName,  iapd_iaid);
            }
            if (iapd_t1[0] != '\0') {
                remove_single_quote(iapd_t1);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_T1_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName,    iapd_t1);
            }
            if (iapd_t2[0] != '\0') {
                remove_single_quote(iapd_t2);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_T2_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName,    iapd_t2);
            }
            if (iapd_pretm[0] != '\0') {
                remove_single_quote(iapd_pretm);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_PRETM_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName, iapd_pretm);
            }
            if (iapd_vldtm[0] != '\0') {
                remove_single_quote(iapd_vldtm);
                memset( sysEventName, 0, sizeof(sysEventName));
                snprintf(sysEventName, sizeof(sysEventName), COSA_DML_WANIface_PREF_VLDTM_SYSEVENT_NAME, IfaceName);
                commonSyseventSet(sysEventName, iapd_vldtm);
            }

            continue;  
            /* 
             * Sysevent and Interface configuration will be done by WanManager for FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE builds.
             * Skiping below code. 
             */
#endif
#endif /* FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE */
                    /*for now we only support one address, one prefix notify, if need multiple addr/prefix, must modify dibbler-client code*/
                    if (strncmp(v6addr, "::", 2) != 0) 
                    {
                        if (strncmp(v6addr, "''", 2) == 0)
                        {
                            commonSyseventSet(COSA_DML_DHCPV6C_ADDR_SYSEVENT_NAME, "");
                        }
                        else
                        {
                            #define STRING_LENGTH 64
                            char dropbearInterface[STRING_LENGTH] = {0};

                            CcspTraceInfo(("%s: v6addr is %s ,pref_len is %d\n", __func__,v6addr,pref_len));

                            remove_single_quote(v6addr);
			    commonSyseventSet(COSA_DML_DHCPV6C_ADDR_SYSEVENT_NAME,v6addr);

                            if ((0 == CheckAndGetDevicePropertiesEntry(dropbearInterface, STRING_LENGTH, "DROPBEAR_INTERFACE")) && (0 == strncmp(dropbearInterface, "erouter0", strlen(dropbearInterface))))
                            {
                                CcspTraceInfo(("%s: Dropbear interface is erouter0 \n", __func__));
                                isDropbearRunningWithIpv6(v6addr);
                            }
                            else
                            {
                                CcspTraceInfo(("%s: Dropbear interface is NOT eRouter0\n", __func__));
                            }

                             #ifdef RDKB_EXTENDER_ENABLED

                                if(pref_len >= 64)
                                    commonSyseventSet("ula_ipv6_enabled","1");
                                else
                                    commonSyseventSet("ula_ipv6_enabled","0");
                                
                                configureLTEIpv6(v6addr);
                             #endif
                        }
						if (iana_iaid[0] != '\0') {
							remove_single_quote(iana_iaid);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_IAID_SYSEVENT_NAME,  iana_iaid);
						}
						if (iana_t1[0] != '\0') {
							remove_single_quote(iana_t1);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_T1_SYSEVENT_NAME,    iana_t1);
						}
						if (iana_t2[0] != '\0') {
							remove_single_quote(iana_t2);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_T2_SYSEVENT_NAME,    iana_t2);
						}
						if (iana_pretm[0] != '\0') {
							remove_single_quote(iana_pretm);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_PRETM_SYSEVENT_NAME, iana_pretm);
						}
						if (iana_vldtm[0] != '\0') {
							remove_single_quote(iana_vldtm);
                        commonSyseventSet(COSA_DML_DHCPV6C_ADDR_VLDTM_SYSEVENT_NAME, iana_vldtm);
						}

                        if(pString)
                        {
                            snprintf(objName, sizeof(objName)-1, "%sIPv6Address.", pString);
                            g_COSARepopulateTable(g_pDslhDmlAgent, objName);
                        }
                    }

#if defined(FEATURE_MAPT) && defined(FEATURE_RDKB_WAN_MANAGER)
                    remove_single_quote(v6pref);
                    strcpy(pdIPv6Prefix, v6pref);
                    remove_single_quote(preflen);
                    pref_len=atoi(preflen);
#endif
                    if (strncmp(v6pref, "::", 2) != 0)
                    {
#if defined (FEATURE_SUPPORT_MAPT_NAT46)
                    rc = sprintf_s(pdV6pref, BUFLEN_64, "%s/%d", v6pref, pref_len);
                    if(rc < EOK)
		     {
			 ERR_CHK(rc);
		     }
#endif
			memset(v6Tpref,0,sizeof(v6Tpref));
			strncpy(v6Tpref,v6pref,sizeof(v6Tpref));
                        /*We just delegate longer and equal 64bits. Use zero to fill in the slot naturally. */
#if defined (MULTILAN_FEATURE)
                        rc = sprintf_s(v6pref+strlen(v6pref), sizeof(v6pref) - strlen(v6pref), "/%d", pref_len);
                        if(rc < EOK)
                        {
                            ERR_CHK(rc);
                        }
#else
                        if ( pref_len >= 64 )
                        {
                            rc = sprintf_s(v6pref+strlen(v6pref),  sizeof(v6pref) - strlen(v6pref), "/%d", pref_len);
                            if(rc < EOK)
                            {
                                ERR_CHK(rc);
                            }
                        }
                        else
						{
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION)
                            rc = sprintf_s(v6pref+strlen(v6pref), sizeof(v6pref) - strlen(v6pref), "/%d", pref_len);
                            if(rc < EOK)
                            {
                                ERR_CHK(rc);
                            }
#else
                            rc = sprintf_s(v6pref+strlen(v6pref), sizeof(v6pref) - strlen(v6pref), "/%d", 64);
                            if(rc < EOK)
                            {
                                ERR_CHK(rc);
                            }
#endif
						}
#endif
	char cmd[100];

    	char lan_v6_pref[100];
        memset(lan_v6_pref,0,sizeof(lan_v6_pref));
        rc = sprintf_s(lan_v6_pref,sizeof(lan_v6_pref),"%s/64",v6Tpref);
        if(rc < EOK){
            ERR_CHK(rc);
        }
        commonSyseventSet("lan_prefix",lan_v6_pref);
/* The below sysevent is set in wanmanager interface state machine so
 * not setting again. Once Interface state machine is used by all customers
 * need to change the flag to FEATURE_RDKB_WAN_MANAGER.
*/
#ifndef _HUB4_PRODUCT_REQ_
    commonSyseventSet(COSA_DML_DHCPV6C_PREF_SYSEVENT_NAME,       v6pref);
#endif
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && defined(_CBR_PRODUCT_REQ_)
#else
			char out1[100]; 
			char *token = NULL;char *pt;
            		char interface_name[32] = {0};	
                        char out[128] = {0};
                        FILE *fp = NULL; 

                            
			if(pref_len < 64)
			{
			    memset(out1,0,sizeof(out1));
                            fp = v_secure_popen("r","syscfg get IPv6subPrefix");
                            _get_shell_output(fp, out, sizeof(out));
			    if(!strcmp(out,"true"))
				{
                                static int first = 0;

                                fp = v_secure_popen("r","syscfg get IPv6_Interface");
                                _get_shell_output(fp, out, sizeof(out));
				pt = out;


				while((token = strtok_r(pt, ",", &pt)))
				 {
			 
					if(GenIPv6Prefix(token,v6Tpref,out1))
					{
						memset(cmd,0,sizeof(cmd));
                        			memset(interface_name,0,sizeof(interface_name));

                        			#ifdef _COSA_INTEL_XB3_ARM_
                                                char LnFIfName[32] = {0} , LnFBrName[32] = {0} ;
                            			syscfg_get( NULL, "iot_ifname", LnFIfName, sizeof(LnFIfName));
                            			if( (LnFIfName[0] != '\0' ) && ( strlen(LnFIfName) != 0 ) )
                            			{
                                			if (strcmp((const char*)token,LnFIfName) == 0 )
                                			{
                                    				syscfg_get( NULL, "iot_brname", LnFBrName, sizeof(LnFBrName));
                                    				if( (LnFBrName[0] != '\0' ) && ( strlen(LnFBrName) != 0 ) )
                                    				{
                                        				strncpy(interface_name,LnFBrName,sizeof(interface_name)-1);
                                    				}
                                    				else
                                    				{
                                        				strncpy(interface_name,token,sizeof(interface_name)-1);
                                    				}
                                			}
                                			else
                                			{
                                    				strncpy(interface_name,token,sizeof(interface_name)-1);
                                			}
                            			}
                            			else
                            			{
                                    			strncpy(interface_name,token,sizeof(interface_name)-1);

                            			}
                        			#else
                            				strncpy(interface_name,token,sizeof(interface_name)-1);
						#endif
						rc = sprintf_s(cmd, sizeof(cmd), "%s_ipaddr_v6", interface_name);
						if(rc < EOK)
						{
							ERR_CHK(rc);
						}
						commonSyseventSet(cmd, out1);

                        			enable_IPv6(interface_name);
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
                        if (iapd_iaid[0] != '\0') {
							remove_single_quote(iapd_iaid);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_IAID_SYSEVENT_NAME,  iapd_iaid);
						}
						if (iapd_t1[0] != '\0') {
							remove_single_quote(iapd_t1);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_T1_SYSEVENT_NAME,    iapd_t1);
						}
						if (iapd_t2[0] != '\0') {
							remove_single_quote(iapd_t2);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_T2_SYSEVENT_NAME,    iapd_t2);
						}
						if (iapd_pretm[0] != '\0') {
							remove_single_quote(iapd_pretm);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_PRETM_SYSEVENT_NAME, iapd_pretm);
						}
						if (iapd_vldtm[0] != '\0') {
							remove_single_quote(iapd_vldtm);
                        commonSyseventSet(COSA_DML_DHCPV6C_PREF_VLDTM_SYSEVENT_NAME, iapd_vldtm);
						}

                        ULONG idx = 0;
                        for ( idx=0; idx<uDhcpv6ServerPoolNum; idx++)
                        {
                              if(sDhcpv6ServerPool[idx].Cfg.LeaseTime != atoi(iapd_pretm))
                              {
                                  sDhcpv6ServerPool[idx].Cfg.LeaseTime = atoi(iapd_pretm);
                                  setpool_into_utopia((PUCHAR)DHCPV6S_NAME, (PUCHAR)"pool", idx, &sDhcpv6ServerPool[idx]);
                              }
                              sDhcpv6ServerPool[idx].Cfg.ValidLeaseTime = atoi(iapd_vldtm);
                        }

                        if(pString)
                        {
                            snprintf(objName, sizeof(objName)-1, "%sIPv6Prefix.", pString);
                            g_COSARepopulateTable(g_pDslhDmlAgent, objName);
                        }

                        g_COSARepopulateTable(g_pDslhDmlAgent, "Device.DHCPv6.Client.1.ReceivedOption.");

                        g_COSARepopulateTable(g_pDslhDmlAgent, "Device.DHCPv6.Server.Pool.1.Option.");

#if defined (MULTILAN_FEATURE)
                        /* we need save this for zebra to send RA 
                           ipv6_prefix           // xx:xx::/yy
                         */
                        v_secure_system("sysevent set ipv6_prefix %s ",v6pref);

                        g_dhcpv6_server_prefix_ready = TRUE;

                        if ((v6addr_prev[0] == '\0') || ( _ansc_strcmp(v6addr_prev, v6pref ) !=0))
                        {
                            _ansc_strncpy( v6addr_prev, v6pref, sizeof(v6pref));
			    /* In case of change in prefix, sysevent set on 'ipv6_prefix' will always trigger ipv6-restart */
                            //commonSyseventSet("ipv6-restart", "1");
                        }
                        else
                        {
                            commonSyseventSet("ipv6_addr-renew", "");
                        }
#endif

#ifdef MULTILAN_FEATURE
/* Service IPv6 will assign IP address and prefix allocation,
   for all lan interfaces.
*/
#if 0 /* Not required for MVx platforms with MULTILAN feature enabled */

#if !defined(_COSA_INTEL_XB3_ARM_)
                        // not the best place to add route, just to make it work
                        // delegated prefix need to route to LAN interface
                        v_secure_system("ip -6 route add %s dev %s", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
			#ifdef _COSA_INTEL_XB3_ARM_
                        v_secure_system("ip -6 route add %s dev %s table erouter", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
			#endif

#if defined(FEATURE_RDKB_WAN_MANAGER)
                        // Disabled sending ipv6 info to wan manager for comcast platforms
                        // Later we should enable if needed.
                        if (0)
                        {
                            /*
                             * Send data to wanmanager.
                             */

                            char dns_server[256] = {'\0'};
                            //char ipv6_lan_prefix[256] = {'\0'};
                            //int prefix_unset_timeout = 100;
                            sscanf(iapd_pretm, "%d", &hub4_preferred_lft);
                            sscanf(iapd_vldtm, "%d", &hub4_valid_lft);
                            ipc_dhcpv6_data_t dhcpv6_data;
                            memset(&dhcpv6_data, 0, sizeof(ipc_dhcpv6_data_t));

#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
                            strncpy(dhcpv6_data.ifname, IfaceName, sizeof(dhcpv6_data.ifname));
#else
                            strncpy(dhcpv6_data.ifname, CFG_TR181_DHCPv6_CLIENT_IfName, sizeof(dhcpv6_data.ifname));
#endif
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
                                dhcpv6_data.prefixPltime = hub4_preferred_lft;
                                dhcpv6_data.prefixVltime = hub4_valid_lft;
                                dhcpv6_data.maptAssigned = FALSE;
                                dhcpv6_data.mapeAssigned = FALSE;
                                dhcpv6_data.prefixCmd = 0;
                            }
                            if (send_dhcp_data_to_wanmanager(&dhcpv6_data) != ANSC_STATUS_SUCCESS) {
                                CcspTraceError(("[%s-%d] Failed to send dhcpv6 data to wanmanager!!! \n", __FUNCTION__, __LINE__));
                            }
                        }
#endif

                        CosaDmlDHCPv6sTriggerRestart(FALSE);
#if defined(_COSA_BCM_ARM_) || defined(INTEL_PUMA7)
                        CcspTraceWarning((" %s dhcpv6_assign_global_ip to brlan0 \n", __FUNCTION__));
                        ret = dhcpv6_assign_global_ip(v6pref, "brlan0", globalIP);
#elif defined _COSA_BCM_MIPS_
                        ret = dhcpv6_assign_global_ip(v6pref, COSA_DML_DHCPV6_SERVER_IFNAME, globalIP);
#else
                        /*We need get a global ip addres */
                        ret = dhcpv6_assign_global_ip(v6pref, "brlan0", globalIP);
#endif
                        CcspTraceWarning(("%s: globalIP %s globalIP2 %s\n", __func__,
                            globalIP, globalIP2));
                        if ( _ansc_strcmp(globalIP, globalIP2 ) ){
                            bRestartLan = TRUE;

                            //PaM may restart. When this happen, we should not overwrite previous ipv6
                            if ( globalIP2[0] ) 
                               commonSyseventSet("lan_ipaddr_v6_prev", globalIP2);

                           // Making sure gw_lan_refresh doesn't get called 1st time
                            if (strlen(globalIP2) != 0 )
                            {
                                g_dhcpv6s_refresh_count = bRestartLan;
                                CcspTraceWarning(("%s: g_dhcpv6s_refresh_count %ld, globalIP2 is %s, strlen is %d\n", __func__, g_dhcpv6s_refresh_count,globalIP2,strlen(globalIP2)));

                            }
                            rc = strcpy_s(globalIP2, sizeof(globalIP2), globalIP);
                            ERR_CHK(rc);
                        }else{
								char lanrestart[8] = {0};
		    					commonSyseventGet("lan_restart_required",lanrestart, sizeof(lanrestart));
								fprintf(stderr,"lan restart staus is %s \n",lanrestart);
   			       				if (strcmp("true",lanrestart) == 0)
								{
							    		bRestartLan = TRUE;
									commonSyseventSet("lan_restart_required","false");
								}
								else
                            		bRestartLan = FALSE;


                                g_dhcpv6s_refresh_count = bRestartLan;

						}

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
                            rc = sprintf_s(cmd, sizeof(cmd), "%d", pref_len);
                            if(rc < EOK)
                            {
                                ERR_CHK(rc);
                            }
                            commonSyseventSet("lan_prefix_v6", cmd);

			    CcspTraceWarning(("%s: setting lan-restart\n", __FUNCTION__));
                            commonSyseventSet("lan-restart", "1");
                        }
#endif
#endif
#else
#if defined(FEATURE_RDKB_WAN_MANAGER)
#if !defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
                       /*
                        * Send data to wanmanager.
                        */

                        char dns_server[256] = {'\0'};
                        char ipv6_lan_prefix[256] = {'\0'};
                        int prefix_unset_timeout = 100;
                        sscanf(iapd_pretm, "%d", &hub4_preferred_lft);
                        sscanf(iapd_vldtm, "%d", &hub4_valid_lft);
                        ipc_dhcpv6_data_t dhcpv6_data;
                        memset(&dhcpv6_data, 0, sizeof(ipc_dhcpv6_data_t));

#if defined(FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE)
                        strncpy(dhcpv6_data.ifname, IfaceName, sizeof(dhcpv6_data.ifname));
#else
                        strncpy(dhcpv6_data.ifname, CFG_TR181_DHCPv6_CLIENT_IfName, sizeof(dhcpv6_data.ifname));
#endif
                        if(strlen(v6pref) == 0) {
                            dhcpv6_data.isExpired = TRUE;
                        } else {
                            dhcpv6_data.isExpired = FALSE;
                            dhcpv6_data.prefixAssigned = TRUE;
                            rc = sprintf_s(dhcpv6_data.sitePrefix, sizeof(dhcpv6_data.sitePrefix), "%s/%d", v6Tpref, pref_len);
                            if(rc < EOK)
                            {
                                ERR_CHK(rc);
                            }
                            strncpy(dhcpv6_data.pdIfAddress, "", sizeof(dhcpv6_data.pdIfAddress));
                            /** DNS servers. **/
                            commonSyseventGet(SYSEVENT_FIELD_IPV6_DNS_SERVER, dns_server, sizeof(dns_server));
                            if (strlen(dns_server) != 0)
                            {
                                dhcpv6_data.dnsAssigned = TRUE;
                                sscanf (dns_server, "%s %s", dhcpv6_data.nameserver, dhcpv6_data.nameserver1);
                            }
                            dhcpv6_data.prefixPltime = hub4_preferred_lft;
                            dhcpv6_data.prefixVltime = hub4_valid_lft;
                            dhcpv6_data.maptAssigned = FALSE;
                            dhcpv6_data.mapeAssigned = FALSE;
                            dhcpv6_data.prefixCmd = 0;
                        }
#ifdef FEATURE_MAPT
                        remove_single_quote(brIPv6Prefix);
                        remove_single_quote(ruleIPv4Prefix);
                        remove_single_quote(ruleIPv6Prefix);
                        remove_single_quote(mapAssigned);
                        remove_single_quote(v4Len);
                        remove_single_quote(v6Len);
                        remove_single_quote(eaLen);
                        remove_single_quote(psidOffset);
                        remove_single_quote(psidLen);
                        remove_single_quote(psid);
                        remove_single_quote(isFMR);

                        if(!strncmp(mapAssigned, "MAPT", 4))
                            dhcpv6_data.maptAssigned = TRUE;
                        else if(!strncmp(mapAssigned, "MAPE", 4))
                            dhcpv6_data.mapeAssigned = TRUE;

                        if((dhcpv6_data.maptAssigned == TRUE) || (dhcpv6_data.mapeAssigned == TRUE))
                        {
                            strncpy(dhcpv6_data.mapt.pdIPv6Prefix, pdIPv6Prefix, sizeof(pdIPv6Prefix));

                            strncpy(dhcpv6_data.mapt.ruleIPv6Prefix, ruleIPv6Prefix, sizeof(ruleIPv6Prefix));
                            if(strlen(ruleIPv6Prefix) == 0) {
                                CcspTraceError(("[%s-%d] MAPT Rule_V6_Prefix is Empty \n", __FUNCTION__, __LINE__));
                            }

                            strncpy(dhcpv6_data.mapt.brIPv6Prefix, brIPv6Prefix, sizeof(brIPv6Prefix));
                            if(strlen(brIPv6Prefix) == 0) {
                                CcspTraceError(("[%s-%d] MAPT Br_V6_Prefix is Empty \n", __FUNCTION__, __LINE__));
                            }

                            strncpy(dhcpv6_data.mapt.ruleIPv4Prefix, ruleIPv4Prefix, sizeof(ruleIPv4Prefix));
                            if(strlen(ruleIPv4Prefix) == 0) {
                                CcspTraceError(("[%s-%d] MAPT Rule_V4_Prefix is Empty \n", __FUNCTION__, __LINE__));
                            }

                            dhcpv6_data.mapt.iapdPrefixLen = pref_len;
                            dhcpv6_data.mapt.v6Len = atoi(v6Len);
                            dhcpv6_data.mapt.isFMR = atoi(isFMR);
                            dhcpv6_data.mapt.eaLen = atoi(eaLen);
                            dhcpv6_data.mapt.v4Len = atoi(v4Len);
                            dhcpv6_data.mapt.psidOffset = atoi(psidOffset);
                            dhcpv6_data.mapt.psidLen = atoi(psidLen);
                            dhcpv6_data.mapt.psid = atoi(psid);
                            dhcpv6_data.mapt.ratio = 1 << (dhcpv6_data.mapt.eaLen - (32 - dhcpv6_data.mapt.v4Len));
                        }

#endif //FEATURE_MAPT
                        if (send_dhcp_data_to_wanmanager(&dhcpv6_data) != ANSC_STATUS_SUCCESS) {
                            CcspTraceError(("[%s-%d] Failed to send dhcpv6 data to wanmanager!!! \n", __FUNCTION__, __LINE__));
                        }

                        /* Wait till interface state machine to unconfigure previous IPv6 configuration */
                        while (--prefix_unset_timeout > 0)
                        {
                            memset(ipv6_lan_prefix, 0, sizeof(ipv6_lan_prefix));
                            sysevent_get(sysevent_fd_global, sysevent_token_global, "lan_prefix_set",
                                         ipv6_lan_prefix, sizeof(ipv6_lan_prefix));
                            if(ipv6_lan_prefix[0] == '\0')
                            {
                                break;
                            }
                            usleep(50000);
                        }

                        if (hub4_preferred_lft != 0 && hub4_valid_lft != 0)
                        {
                            ret = dhcpv6_assign_global_ip(v6pref, COSA_DML_DHCPV6_SERVER_IFNAME, globalIP);
                            if(ret != 0) {
                                CcspTraceInfo(("Assign global ip error \n"));
                            }
#ifdef _HUB4_PRODUCT_REQ_
                            else {
                                CcspTraceInfo(("%s Going to set [%s] address on brlan0 interface \n", __FUNCTION__, globalIP));
                                v_secure_system("ip -6 addr add %s/64 dev %s valid_lft %d preferred_lft %d",
                                                globalIP, COSA_DML_DHCPV6_SERVER_IFNAME, hub4_valid_lft, hub4_preferred_lft);
                                commonSyseventSet("lan_ipaddr_v6", globalIP);
                                // send an event to wanmanager that Global-prefix is set
                                commonSyseventSet("lan_prefix_set", globalIP);
                            }
#else
                            else {
                                commonSyseventSet("lan_ipaddr_v6", globalIP);

                                /*v_secure_system("ip -6 addr add %s/64 dev %s valid_lft %d preferred_lft %d", globalIP, 
                                                COSA_DML_DHCPV6_SERVER_IFNAME, hub4_valid_lft, hub4_preferred_lft);
                            */
			}
                        /*    if(strlen(v6pref) > 0) {
                                char v6pref_addr[128] = {0};
                                strncpy(v6pref_addr, v6pref, (strlen(v6pref)-5));
                                v_secure_system("ip -6 addr add %s::1/64 dev %s valid_lft %d preferred_lft %d",
                                                v6pref_addr, COSA_DML_DHCPV6_SERVER_IFNAME, hub4_valid_lft, hub4_preferred_lft);
                            } */
                            // send an event to wanmanager that Global-prefix is set
                            commonSyseventSet("lan_prefix_set", globalIP);
#endif
                        }
#endif
#else
#if defined(_HUB4_PRODUCT_REQ_) 
                    commonSyseventGet(SYSEVENT_FIELD_IPV6_PREFIXVLTIME,
                                 hub4_valid_lft, sizeof(hub4_valid_lft));
                    commonSyseventGet(SYSEVENT_FIELD_IPV6_PREFIXPLTIME,
                                 hub4_preferred_lft, sizeof(hub4_preferred_lft));
                    if ((hub4_valid_lft[0]=='\0') || (hub4_preferred_lft[0]=='\0')){
                        strncpy(hub4_preferred_lft, "forever", sizeof(hub4_preferred_lft));
                        strncpy(hub4_valid_lft, "forever", sizeof(hub4_valid_lft));
                    }
                    commonSyseventGet(SYSEVENT_FIELD_IPV6_ULA_ADDRESS,
                                 ula_address, sizeof(ula_address));
                    if(ula_address[0] != '\0') {
                        v_secure_system("ip -6 addr add %s/64 dev %s", ula_address, COSA_DML_DHCPV6_SERVER_IFNAME);
                    }
                    ret = dhcpv6_assign_global_ip(v6pref, COSA_DML_DHCPV6_SERVER_IFNAME, globalIP);
                    if(ret != 0) {
                        CcspTraceInfo(("Assign global ip error \n"));
                    }
                    else
                    {
                        CcspTraceInfo(("%s Going to set [%s] address on brlan0 interface \n", __FUNCTION__, globalIP));
                        v_secure_system("ip -6 addr add %s/64 dev %s valid_lft %s preferred_lft %s", globalIP, COSA_DML_DHCPV6_SERVER_IFNAME, hub4_valid_lft, hub4_preferred_lft);
                        commonSyseventSet("lan_ipaddr_v6", globalIP);
                        // send an event to Sky-pro app manager that Global-prefix is set
                        commonSyseventSet("lan_prefix_set", globalIP);
                    }
#endif
#endif // FEATURE_RDKB_WAN_MANAGER

                        /* check for old prefix*/
                        char prevPref[128];

                        prevPref[0] = 0;
                        commonSyseventGet("ipv6_prefix", prevPref, sizeof(prevPref));
                        if ((strlen(prevPref) > 3) && (strcmp(prevPref, v6pref) != 0)) {
                            v_secure_system("ip -6 route del %s dev %s", prevPref, COSA_DML_DHCPV6_SERVER_IFNAME);
                            v_secure_system("ip -6 route del %s dev %s table all_lans", prevPref, COSA_DML_DHCPV6_SERVER_IFNAME);
#ifdef _COSA_INTEL_XB3_ARM_
                            v_secure_system("ip -6 route del %s dev %s table erouter", prevPref, COSA_DML_DHCPV6_SERVER_IFNAME);
#endif
                        }

                        // not the best place to add route, just to make it work
                        // delegated prefix need to route to LAN interface
                        v_secure_system("ip -6 route add %s dev %s", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
                        v_secure_system("ip -6 route add %s dev %s table all_lans", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
			#ifdef _COSA_INTEL_XB3_ARM_
                        v_secure_system("ip -6 route add %s dev %s table erouter", v6pref, COSA_DML_DHCPV6_SERVER_IFNAME);
			#endif
                        /* we need save this for zebra to send RA
                           ipv6_prefix           // xx:xx::/yy
                         */
#ifndef _HUB4_PRODUCT_REQ_
                        v_secure_system("sysevent set ipv6_prefix %s ",v6pref);
#else
                        v_secure_system("sysevent set zebra-restart ");
#endif
                        g_dhcpv6_server_prefix_ready = TRUE;

#if !defined (_SKY_HUB_COMMON_PRODUCT_REQ_)
                        CosaDmlDHCPv6sTriggerRestart(FALSE);
#endif /* _SKY_HUB_COMMON_PRODUCT_REQ_ */

                        /*We need get a global ip addres */
#if defined(_COSA_BCM_ARM_) || defined(INTEL_PUMA7)
#ifndef _HUB4_PRODUCT_REQ_
                        /*this is for tchxb6*/
                        CcspTraceWarning((" %s dhcpv6_assign_global_ip to brlan0 \n", __FUNCTION__));
                        ret = dhcpv6_assign_global_ip(v6pref, "brlan0", globalIP);
#endif
#elif defined _COSA_BCM_MIPS_
                        ret = dhcpv6_assign_global_ip(v6pref, COSA_DML_DHCPV6_SERVER_IFNAME, globalIP);
#else
                        ret = dhcpv6_assign_global_ip(v6pref, "brlan0", globalIP);
#endif
                        CcspTraceWarning(("%s: globalIP %s globalIP2 %s\n", __func__,
                            globalIP, globalIP2));
                        if ( _ansc_strcmp(globalIP, globalIP2 ) ){
                            bRestartLan = TRUE;

                            //PaM may restart. When this happen, we should not overwrite previous ipv6
                            if ( globalIP2[0] ) 
                               commonSyseventSet("lan_ipaddr_v6_prev", globalIP2);

                            // Making sure gw_lan_refresh doesn't get called 1st time
                            if (strlen(globalIP2) != 0 )
                            {
                                g_dhcpv6s_refresh_count = bRestartLan;
                                CcspTraceWarning(("%s: g_dhcpv6s_refresh_count %ld, globalIP2 is %s, strlen is %d\n", __func__, g_dhcpv6s_refresh_count,globalIP2,strlen(globalIP2)));
                            }

                            rc = strcpy_s(globalIP2, sizeof(globalIP2), globalIP);
                            ERR_CHK(rc);
                        }else{
								char lanrestart[8] = {0};
		    					commonSyseventGet("lan_restart_required",lanrestart, sizeof(lanrestart));
								fprintf(stderr,"lan restart staus is %s \n",lanrestart);
   			       				if (strcmp("true",lanrestart) == 0)
								{
							    		bRestartLan = TRUE;
									commonSyseventSet("lan_restart_required","false");
								}
								else
                            		bRestartLan = FALSE;


                                g_dhcpv6s_refresh_count = bRestartLan;
						}
                    
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
                            rc = sprintf_s(cmd, sizeof(cmd), "%d", pref_len);
                            if(rc < EOK)
                            {
                                ERR_CHK(rc);
                            }
                            commonSyseventSet("lan_prefix_v6", cmd);

                            CcspTraceWarning(("%s: setting lan-restart\n", __FUNCTION__));
                            commonSyseventSet("lan-restart", "1");
#if defined (_SKY_HUB_COMMON_PRODUCT_REQ_)
                            CosaDmlDHCPv6sTriggerRestart(FALSE);
#endif /* _SKY_HUB_COMMON_PRODUCT_REQ_ */
                        }
#endif

#ifdef FEATURE_SUPPORT_MAPT_NAT46
                       /*
                        * Parses option-95 response, apply mapt configuration and set
                        * appropriate events.
                        */
                       {
                          char maptEnable[BUFLEN_8] = {0};

                          if (!syscfg_get(NULL, "MAPT_Enable", maptEnable, sizeof(maptEnable)) &&
                              !strncmp(maptEnable, "true", 4))
                          {
                              CcspTraceWarning(("%s: Enabling mapt configuration\n", __FUNCTION__));

                              if (CosaDmlMaptProcessOpt95Response(pdV6pref, opt95_dBuf)
                                                              != ANSC_STATUS_SUCCESS)
                              {
                                  CcspTraceError(("%s: Failed to enable mapt configuration\n",
                                                   __FUNCTION__));
                              }
                          }
                       }
#endif
                   }

#if defined(DSLITE_FEATURE_SUPPORT) && defined(_LG_MV3_)
                   /* Fixme: needs review. Temp fix for Mv3 */
                   //This is workaround fix to force all the device running previously on 1.3 factory image to come with dslite enabled by true for VM-IE
                   //This needs to be reviewed and fixed.

                   char countryindex[8],erouter_mode[8],dsliteforceenable[8];

                   syscfg_get(NULL, "Customer_Index", countryindex, sizeof(countryindex));
                   syscfg_get(NULL, "last_erouter_mode", erouter_mode, sizeof(erouter_mode));
                   syscfg_get(NULL, "dslite_enable_vm_ie", dsliteforceenable, sizeof(dsliteforceenable));

                   if((strcmp(countryindex, "41") == 0) && (strcmp(erouter_mode, "2") == 0) && strcmp(dsliteforceenable, "1"))//VM-IE, 2->IPV6 mode,dslite not force enable
                   {
                       g_SetParamValueBool("Device.DSLite.Enable", TRUE);
                       g_SetParamValueBool("Device.DSLite.InterfaceSetting.1.Enable", TRUE);
                       syscfg_set_commit(NULL, "dslite_enable_vm_ie", "1");
                   }
                   v_secure_system("service_dslite restart &");
#endif
                }
                else if (!strncmp(action, "del", 3))
                {
                    /*todo*/
                }
#if defined(CISCO_CONFIG_DHCPV6_PREFIX_DELEGATION) && (defined(_CBR_PRODUCT_REQ_) || defined(_BCI_FEATURE_REQ))

#else
		v_secure_system("sysevent set zebra-restart");
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

    return NULL;
}
#if defined(FEATURE_RDKB_WAN_MANAGER)
static int send_dhcp_data_to_wanmanager (ipc_dhcpv6_data_t *dhcpv6_data)
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

    ipc_msg_payload_t msg;
    memset (&msg, 0, sizeof(ipc_msg_payload_t));

    /**
     * Copy dhcpv6 data.
     */
    msg.msg_type = DHCP6C_STATE_CHANGED;
    memcpy(&msg.data.dhcpv6, dhcpv6_data, sizeof(ipc_dhcpv6_data_t));

    /**
     * Send data to wanmanager.
     */
    int bytes = 0;
   int sz_msg = sizeof(ipc_msg_payload_t);


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
#if defined (WAN_FAILOVER_SUPPORTED) && !defined (RDKB_EXTENDER_ENABLED)
void SwitchToGlobalIpv6()
{
    CcspTraceInfo(("%s started\n",__FUNCTION__));
    char buf[16] = {0};
    memset(buf,0,sizeof(buf));
    commonSyseventGet("ula_ipv6_enabled", buf, sizeof(buf));
    if ( 1 == atoi(buf) )
    {
        CcspTraceInfo(("%s Switching mode to GLOBAL_IPV6\n",__FUNCTION__));

        commonSyseventSet("routeunset-ula","");
        commonSyseventSet("ula_ipv6_enabled","0");
        commonSyseventSet("mode_switched","GLOBAL_IPV6");
        commonSyseventSet("disable_old_prefix_ra","true");
        commonSyseventSet("zebra-restart","");
        commonSyseventSet("firewall-restart","");   
    }
}

void SwitchToULAIpv6()
{
    CcspTraceInfo(("%s started\n",__FUNCTION__));
    commonSyseventSet("routeset-ula","");
    commonSyseventSet("ula_ipv6_enabled","1");
    commonSyseventSet("mode_switched","ULA_IPV6");
    commonSyseventSet("disable_old_prefix_ra","true");
    commonSyseventSet("zebra-restart","");
    commonSyseventSet("firewall-restart","");
}

/** Switching  between Primary and Secondary Wan for LTE Backup **/
void Switch_ipv6_mode(char *ifname, int length)
{
    char default_wan_ifname[64];
    memset(default_wan_ifname, 0, sizeof(default_wan_ifname));
    commonSyseventGet("wan_ifname", default_wan_ifname, sizeof(default_wan_ifname));
    if (ifname)
    {
#ifdef FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE
        char mesh_wan_ifname[32] = {0};
        getMeshWanIfName(mesh_wan_ifname,sizeof(mesh_wan_ifname));
        if(strncmp(ifname, mesh_wan_ifname,length ) == 0)
#else
        if(strncmp(ifname,default_wan_ifname,length) != 0)
#endif
        {
            SwitchToULAIpv6(); //Secondary Wan
        }
        else
        {
            SwitchToGlobalIpv6(); //Primary Wan
        }
    }
}

void *Ipv6ModeHandler_thrd(void *data)
{
    UNREFERENCED_PARAMETER(data);
    int sysevent_fd;
    token_t sysevent_token;
    async_id_t async_id_wanfailover[3];
    int err;
    char name[64] = {0}, val[64] = {0};
    char tmpBuf[32] ={0};

    CcspTraceWarning(("%s started\n",__FUNCTION__));
    sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "Ipv6ModeHandler", &sysevent_token);
    sysevent_set_options(sysevent_fd, sysevent_token, "current_wan_ifname", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd, sysevent_token, "current_wan_ifname",  &async_id_wanfailover[0]);
    sysevent_set_options(sysevent_fd, sysevent_token, "mesh_wan_linkstatus", TUPLE_FLAG_EVENT);
    sysevent_setnotification(sysevent_fd, sysevent_token, "mesh_wan_linkstatus",  &async_id_wanfailover[1]);
    while(1)
    {
        async_id_t getnotification_asyncid;
        memset(name,0,sizeof(name));
        memset(val,0,sizeof(val));
        int namelen = sizeof(name);
        int vallen  = sizeof(val);
        err = sysevent_getnotification(sysevent_fd, sysevent_token, name, &namelen,  val, &vallen, &getnotification_asyncid);
        if (err)
        {
            CcspTraceWarning(("sysevent_getnotification failed with error: %d %s\n", err,__FUNCTION__));
            CcspTraceWarning(("sysevent_getnotification failed name: %s val : %s\n", name,val));
            if ( 0 != v_secure_system("pidof syseventd")) {
                CcspTraceWarning(("%s syseventd not running ,breaking the receive notification loop \n",__FUNCTION__));
                break;
            }
        }
        else
        {
            CcspTraceWarning(("%s Recieved notification event  %s\n",__FUNCTION__,name));
            if(!strcmp(name, "mesh_wan_linkstatus"))
            {
                if (!strncmp(val, "up", 2))
                {
                    addIpv6toRemoteWan();
                }
                else if (!strncmp(val, "down", 4))
                {
                    delIpv6toRemoteWan();
                }
            }
            else if(!strcmp(name, "current_wan_ifname"))
            {
                memset(tmpBuf,0,sizeof(tmpBuf));
#ifdef FEATURE_RDKB_CONFIGURABLE_WAN_INTERFACE
                getMeshWanIfName(tmpBuf,sizeof(tmpBuf));
                if (strcmp(val,tmpBuf) == 0 )
#else
                sysevent_get(sysevent_fd, sysevent_token, "wan_ifname", tmpBuf, sizeof(tmpBuf));
                if (strcmp(val,tmpBuf) != 0 )
#endif
                {
                    _dibbler_server_operation("stop");
                    addRemoteWanIpv6Route();
                } 
                Switch_ipv6_mode(val,vallen);
            }
        }
    }
    return NULL;
}
#endif
#endif //FEATURE_RDKB_WAN_MANAGER
#endif
