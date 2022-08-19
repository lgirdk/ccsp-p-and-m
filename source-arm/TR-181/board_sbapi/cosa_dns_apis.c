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

/************************************************************************* 
    module: cosa_dns_apis.c

        For COSA Data Model Library Development

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

**************************************************************************/

#include "cosa_dns_internal.h"
#include "safec_lib_common.h"

#define  SYSCFG_DNS_SERVER_ENABLE_KEY           "dhcp_dns_server_enable_%lu"  //LGI ADD

#if (defined(_COSA_SIM_))

COSA_DML_DNS_CLIENT_SERVER  g_DnsServerFull[10] =
{
    {1, "Server1", TRUE, COSA_DML_DNS_STATUS_Enabled,
        "\x40\x40\x40\x08", "wan8", COSA_DML_DNS_ADDR_SRC_DHCP}
};

COSA_DML_DNS_RELAY_ENTRY  g_DnsRelayFull[10] =
{
    {1, "Forwarding1", TRUE, COSA_DML_DNS_STATUS_Enabled, 
        "\x40\x40\x40\x08", "wan8", COSA_DML_DNS_ADDR_SRC_DHCP}
};

ULONG g_NumOfDnsServers = 1;

ULONG g_NumOfDnsRelays = 1;

BOOLEAN g_ClientEnable = TRUE; 

BOOLEAN g_ReplyEnable = TRUE;

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsInit
            (
                ANSC_HANDLE                 hDml,
                PANSC_HANDLE                phContext
            )


    description:

       This function enables the DNS client.

    argument:  
            ANSC_HANDLE                  hDml,

            PANSC_HANDLE                phContext

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaDmlDnsInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{

    UNREFERENCED_PARAMETER(hDml);
    UNREFERENCED_PARAMETER(phContext);
    return ANSC_STATUS_SUCCESS;
}

/*
 *  DNS Client
 */
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsEnableClient
            (
                ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled
            )

    description:

       This function enables the DNS client.

    argument:   ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsEnableClient
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bEnabled
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;

    g_ClientEnable = bEnabled;
    
    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlIpDnsGetClientStatus
            (
                ANSC_HANDLE                 hContext
            );

    description:

        This function gets the status of the DNS client.

    argument:   ANSC_HANDLE                 hContext
            

    return:     operation status.

**********************************************************************/
COSA_DML_DNS_STATUS
CosaDmlIpDnsGetClientStatus
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    COSA_DML_DNS_STATUS             DnsStatus = COSA_DML_DNS_STATUS_Disabled;

    if ( g_ClientEnable )
    {
          DnsStatus = COSA_DML_DNS_STATUS_Enabled;
    }
    
    return DnsStatus;
}
/*
 *  DNS Client Server
 */
/**********************************************************************

    caller:     self

    prototype:

        PCOSA_DML_DNS_CLIENT_SERVER
        CosaDmlDnsClientGetServers
            (
                ANSC_HANDLE                 hContext,
                PULONG                      pulCount
            );

    description:

        This routine is to retrieve the complete list of DNS servers, which is a dynamic table.

    argument:   ANSC_HANDLE                 hContext,
                PULONG                      pulCount
                To receive the actual number of entries.

    return:     The pointer to the array of DNS servers, allocated by callee. If no entry is found, NULL is returned.

**********************************************************************/
PCOSA_DML_DNS_CLIENT_SERVER
CosaDmlDnsClientGetServers
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount
    )
{
    UNREFERENCED_PARAMETER(hContext);
    PCOSA_DML_DNS_CLIENT_SERVER     pDnsServer = (PCOSA_DML_DNS_CLIENT_SERVER)NULL;
    ULONG                           ulSize     = 0;

    ulSize = sizeof(COSA_DML_DNS_CLIENT_SERVER) * g_NumOfDnsServers;
    
    pDnsServer = AnscAllocateMemory(ulSize);

    if ( !pDnsServer )
    {
        *pulCount = 0;
    } else
    {
        *pulCount = g_NumOfDnsServers;
        AnscCopyMemory(pDnsServer, g_DnsServerFull, ulSize);
    }
    
    return pDnsServer;
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientSetServerValues
            (
                ANSC_HANDLE             hContext,
                ULONG                       ulIndex,
                ULONG                       ulInstanceNumber,
                char*                        pAlias
            )


    description:

        The API sets DNS server values. 

    argument:   
            ANSC_HANDLE             hContext,

            ULONG                       ulIndex,

            ULONG                       ulInstanceNumber,

            char*                        pAlias
    
    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsClientSetServerValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    errno_t                         rc           = -1;

    g_DnsServerFull[ulIndex].InstanceNumber = ulInstanceNumber;
    rc = strcpy_s(g_DnsServerFull[ulIndex].Alias,sizeof(g_DnsServerFull[ulIndex].Alias), pAlias);
    ERR_CHK(rc);        
    return returnStatus;
}



/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientAddServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry
            );

    description:

        The API adds one DNS server entry into DNS client. 

    argument:   ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry
                Caller does not need to fill in Status or Alias fields. Field Type has to be COSA_DML_DNS_ADDR_SRC_Static. Caller does not have to fill in field Interface. Upon return, callee fills in the generated Alias.
    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsClientAddServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_CLIENT_SERVER pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( g_NumOfDnsServers >= 10 )
    {
        return ANSC_STATUS_FAILURE;
    }

    g_DnsServerFull[g_NumOfDnsServers].InstanceNumber  = pEntry->InstanceNumber;  
    g_DnsServerFull[g_NumOfDnsServers].bEnabled        = pEntry->bEnabled;        
    g_DnsServerFull[g_NumOfDnsServers].Status          = pEntry->Status;        
    g_DnsServerFull[g_NumOfDnsServers].Type            = pEntry->Type;      
    g_DnsServerFull[g_NumOfDnsServers].DNSServer.Value = pEntry->DNSServer.Value;             
    errno_t rc = -1;
    rc = strcpy_s(g_DnsServerFull[g_NumOfDnsServers].Alias,sizeof(g_DnsServerFull[g_NumOfDnsServers].Alias), pEntry->Alias);
    ERR_CHK(rc);
    rc = strcpy_s(g_DnsServerFull[g_NumOfDnsServers].Interface,sizeof(g_DnsServerFull[g_NumOfDnsServers].Interface), pEntry->Interface);
    ERR_CHK(rc);
    g_NumOfDnsServers++;

    return ANSC_STATUS_SUCCESS;
}



/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientDelServer
            (
                ANSC_HANDLE                 hContext,
                ULONG                       ulInstanceNumber
            );

    description:

        The API delete one DNS server entry from DNS client.

    argument:   ANSC_HANDLE                 hContext,
                ULONG                       ulInstanceNumber

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsClientDelServer
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ULONG                           i = 0;
    ULONG                           j = 0;

     for ( i = 0; i < g_NumOfDnsServers; i++ )
    {
        if ( ulInstanceNumber == g_DnsServerFull[i].InstanceNumber )
        {
            for ( j = i; j < g_NumOfDnsServers; j++ )
            {
                AnscCopyMemory
                (
                    &g_DnsServerFull[j], 
                    &g_DnsServerFull[j+1],
                    sizeof(COSA_DML_DNS_CLIENT_SERVER)
                );
            }

            g_NumOfDnsServers--;

            return ANSC_STATUS_SUCCESS;
        }
    }
             
    return ANSC_STATUS_CANT_FIND;
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientSetServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry
            );

    description:

        This function sets the server of the DNS client.

    argument:   ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsClientSetServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_CLIENT_SERVER pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ULONG                           i = 0;
    errno_t                         rc = -1;
    for ( i = 0; i < g_NumOfDnsServers; i++)
    {
        if ( g_DnsServerFull[i].InstanceNumber == pEntry->InstanceNumber )
        {
            g_DnsServerFull[i].InstanceNumber  = pEntry->InstanceNumber;  
            g_DnsServerFull[i].bEnabled        = pEntry->bEnabled;        
            g_DnsServerFull[i].Status          = pEntry->Status;        
            g_DnsServerFull[i].Type            = pEntry->Type;      
            g_DnsServerFull[i].DNSServer.Value = pEntry->DNSServer.Value;             
            
            rc = strcpy_s(g_DnsServerFull[i].Alias,sizeof(g_DnsServerFull[i].Alias), pEntry->Alias);
            ERR_CHK(rc);
            rc = strcpy_s(g_DnsServerFull[i].Interface,sizeof(g_DnsServerFull[i].Interface), pEntry->Interface);
            ERR_CHK(rc);
 
            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_CANT_FIND;   
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientGetServer
            (
                ANSC_HANDLE                                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER      pEntry
             )


    description:

        This routine is to retrieve the config of DNS servers, which is a dynamic table.

    argument:  
                    ANSC_HANDLE                 hContext,

                    PCOSA_DML_DNS_CLIENT_SERVER pEntry

    return:    operation status.  

**********************************************************************/

ANSC_STATUS
CosaDmlDnsClientGetServer
    (
        ANSC_HANDLE hContext,
        PCOSA_DML_DNS_CLIENT_SERVER pEntry
     )

{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    

    return returnStatus;
}



/*
 *  DNS Relay
 */
 
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsEnableRelay
            (
                ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled
            )

    description:

        This function enables the DNS relay.

    argument:   ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsEnableRelay
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bEnabled
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    
    g_ReplyEnable = bEnabled;

    return returnStatus;
}


/**********************************************************************

    caller:     self

    prototype:

        COSA_DML_DNS_STATUS
        CosaDmlIpDnsGetRelayStatus
            (
                ANSC_HANDLE                 hContext
            )

    description:

        This function initiate  cosa nat object and return handle.

    argument:   ANSC_HANDLE                 hContext
          

    return:     operation status.

**********************************************************************/
COSA_DML_DNS_STATUS
CosaDmlIpDnsGetRelayStatus
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    COSA_DML_DNS_STATUS             RelayStatus = COSA_DML_DNS_STATUS_Disabled;

    if ( g_ReplyEnable )
    {
        RelayStatus = COSA_DML_DNS_STATUS_Enabled;
    }
    
    return RelayStatus;
}

/*
 *  DNS Relay Forwarding (server)
 */
/**********************************************************************

    caller:     self

    prototype:

        PCOSA_DML_DNS_RELAY_ENTRY
        CosaDmlDnsRelayGetServers
            (
                ANSC_HANDLE                 hContext,
                PULONG                      pulCount
            );

    description:

        This function Get the DNS relay server.

    argument:   ANSC_HANDLE                 hContext,
                PULONG                      pulCount
                To receive the actual number of entries.

    return:     The pointer to the array of DNS servers, allocated by callee. If no entry is found, NULL is returned.

**********************************************************************/
PCOSA_DML_DNS_RELAY_ENTRY
CosaDmlDnsRelayGetServers
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount
    )
{
    UNREFERENCED_PARAMETER(hContext);
    PCOSA_DML_DNS_RELAY_ENTRY       pForward = (PCOSA_DML_DNS_RELAY_ENTRY)NULL;
    ULONG                           ulSize     = 0;

    ulSize = sizeof(COSA_DML_DNS_RELAY_ENTRY) * g_NumOfDnsRelays;

    pForward = AnscAllocateMemory(ulSize);

    if ( !pForward )
    {
        *pulCount = 0;
    } else
    {
        *pulCount = g_NumOfDnsRelays;
        AnscCopyMemory(pForward, g_DnsRelayFull, ulSize);
    }
    
    return pForward;
}



/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelaySetServerValues
            (
                ANSC_HANDLE                 hContext,
                ULONG                           ulIndex,
                ULONG                           ulInstanceNumber,
                char*                            pAlias
            )


    description:

        This function Get the DNS relay server.

    argument:  
                 ANSC_HANDLE                 hContext,

                 ULONG                           ulIndex,

                 ULONG                           ulInstanceNumber,

                 char*                            pAlias

    return:     The pointer to the array of DNS servers, allocated by callee. If no entry is found, NULL is returned.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelaySetServerValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    errno_t                         rc           = -1;
    g_DnsRelayFull[ulIndex].InstanceNumber = ulInstanceNumber;
    rc = strcpy_s(g_DnsRelayFull[ulIndex].Alias,sizeof(g_DnsRelayFull[ulIndex].Alias), pAlias);
    ERR_CHK(rc);
        
    return returnStatus;
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelayAddServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry
            );

    description:

        The API adds one DNS server entry into DNS relay. 

    argument:   ANSC_HANDLE                 hContext,
    
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry
                Caller does not need to fill in Status or Alias fields. Field Type has to be COSA_DML_DNS_ADDR_SRC_Static. Caller does not have to fill in field Interface. Upon return, callee fills in the generated Alias.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelayAddServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_RELAY_ENTRY   pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( g_NumOfDnsRelays >= 10 )
    {
        return ANSC_STATUS_FAILURE;
    }

    g_DnsRelayFull[g_NumOfDnsRelays].InstanceNumber  = pEntry->InstanceNumber;  
    g_DnsRelayFull[g_NumOfDnsRelays].bEnabled        = pEntry->bEnabled;        
    g_DnsRelayFull[g_NumOfDnsRelays].Status          = pEntry->Status;        
    g_DnsRelayFull[g_NumOfDnsRelays].Type            = pEntry->Type;      
    g_DnsRelayFull[g_NumOfDnsRelays].DNSServer.Value = pEntry->DNSServer.Value;             
    errno_t                                          = -1;
    rc = strcpy_s(g_DnsRelayFull[g_NumOfDnsRelays].Alias,sizeof(g_DnsRelayFull[g_NumOfDnsRelays].Alias), pEntry->Alias);
    ERR_CHK(rc);
    rc = strcpy_s(g_DnsRelayFull[g_NumOfDnsRelays].Interface,sizeof(g_DnsRelayFull[g_NumOfDnsRelays].Interface), pEntry->Interface);
    ERR_CHK(rc);

    g_NumOfDnsRelays++;

    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelayDelServer
        (
            ANSC_HANDLE                 hContext,
            ULONG                       ulInstanceNumber
        );

    description:

        The API delete one DNS server entry from DNS client. 

    argument:   ANSC_HANDLE                 hContext,
    
                ULONG                       ulInstanceNumber

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelayDelServer
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ULONG                           i = 0;
    ULONG                           j = 0;

    for ( i = 0; i < g_NumOfDnsRelays; i++ )
    {
        if ( ulInstanceNumber == g_DnsRelayFull[i].InstanceNumber )
        {
            for ( j = i; j < g_NumOfDnsRelays; j++ )
            {
                AnscCopyMemory
                (
                    &g_DnsRelayFull[j], 
                    &g_DnsRelayFull[j+1],
                    sizeof(COSA_DML_DNS_RELAY_ENTRY)
                );
            }

            g_NumOfDnsRelays--;

            return ANSC_STATUS_SUCCESS;
        }
    }
             
    return ANSC_STATUS_CANT_FIND;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelaySetServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry
            );

    description:

        This function set the relay server.

    argument:   ANSC_HANDLE                 hContext,
    
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelaySetServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_RELAY_ENTRY   pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    ULONG                           i = 0;
    errno_t                         rc = -1;

    for ( i = 0; i < g_NumOfDnsRelays; i++)
    {
        if ( g_DnsRelayFull[i].InstanceNumber == pEntry->InstanceNumber )
        {
            g_DnsRelayFull[i].InstanceNumber  = pEntry->InstanceNumber;  
            g_DnsRelayFull[i].bEnabled        = pEntry->bEnabled;        
            g_DnsRelayFull[i].Status          = pEntry->Status;        
            g_DnsRelayFull[i].Type            = pEntry->Type;      
            g_DnsRelayFull[i].DNSServer.Value = pEntry->DNSServer.Value;             
            
            rc = strcpy_s(g_DnsRelayFull[i].Alias,sizeof(g_DnsRelayFull[i].Alias), pEntry->Alias);
            ERR_CHK(rc);
            rc = strcpy_s(g_DnsRelayFull[i].Interface,sizeof(g_DnsRelayFull[i].Interface), pEntry->Interface);
            ERR_CHK(rc);
 
            return ANSC_STATUS_SUCCESS;
        }
    }

    return ANSC_STATUS_CANT_FIND;   
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelayGetServer
            (
                ANSC_HANDLE                           hContext,
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry     
            )



    description:

        This routine is to retrieve the config of DNS servers, which is a dynamic table.

    argument:  
                    ANSC_HANDLE                          hContext,

                    PCOSA_DML_DNS_RELAY_ENTRY  pEntry

    return:    operation status.  

**********************************************************************/

ANSC_STATUS
CosaDmlDnsRelayGetServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_RELAY_ENTRY   pEntry     
    )


{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    
    return returnStatus;
}


#elif ( defined(_COSA_INTEL_USG_ARM_) || defined(_COSA_BCM_MIPS_) )

#include <utctx_api.h>
#include <utapi.h>
#include <utapi_util.h>
#include <ulog.h>
#include <syscfg/syscfg.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "cosa_x_cisco_com_devicecontrol_apis.h"
#include <utapi/utapi_dns.h>
#include <utapi/utapi_dslite.h>
#include "cosa_dns_internal.h"

static const char *DEFAULT_WAN_INTERFACE = "Device.IP.Interface.1";
static const char *DEFAULT_LAN_INTERFACE = "Device.IP.Interface.3";
static const char *DNS_RESOLV_CONF = "/etc/resolv.conf";
static const char *DNS_RELAY_RESOLV_CONF = "/var/dnsrelay-resolv.conf";
static const char *DHCP_OPTIONS = "/var/dhcp_options";
static const char *DHCPV6_OPTIONS = "/etc/dibbler/server.conf";
static const char *DNS_KEY_RESOLVCONF = "nameserver";
static const char *DNS_KEY_DHCPOPS = "option:dns-server";
static const char *DNS_KEY_DHCPV6OPS = "option dns-server";
static const char *UPDATE_RESOLV_CMD = "/bin/sh /etc/utopia/service.d/set_resolv_conf.sh";
static const int DHCP_DNS_NUMBER = 3;
static boolean_t g_DnsRelayEnabled = FALSE;
extern int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger);

static int GetWanDhcpDns(DNS_Client_t *dns)
{
    int dns_count = 0;
    token_t se_token;
    char dns_list[512] = {0};
    int se_fd = -1;
    char * ip = NULL;

    se_fd = s_sysevent_connect(&se_token);
    if (se_fd < 0)
    {
        return 0;
    }

    // Get IPv4 DNS IPs.
    sysevent_get(se_fd, se_token, "wan_dhcp_dns", dns_list, sizeof(dns_list) - 1);
    ip = strtok(dns_list, " ");
    while (dns_count < DHCP_DNS_NUMBER)
    {
        if (ip)
        {
            AnscCopyString(dns->dns_server[dns_count], ip);
            ip = strtok(NULL, " ");
        }
        else
        {
            AnscCopyString(dns->dns_server[dns_count], "0.0.0.0");
        }
        dns_count++;
    }

    // Get IPv6 DNS IPs.
    sysevent_get(se_fd, se_token, "ipv6_nameserver", dns_list, sizeof(dns_list) - 1);
    ip = strtok(dns_list, " ");
    while (dns_count < DHCP_DNS_NUMBER * 2)
    {
        if (ip)
        {
            AnscCopyString(dns->dns_server[dns_count], ip);
            ip = strtok(NULL, " ");
        }
        else
        {
            AnscCopyString(dns->dns_server[dns_count], "::");
        }
        dns_count++;
    }

    return dns_count;
}

static int RestartPlatform(boolean_t doIpv6Restart)
{
    int rc = system(UPDATE_RESOLV_CMD);
    if ((rc == 0) && (doIpv6Restart == TRUE))
    {
        rc = CosaDmlDHCPv6sTriggerRestart(FALSE);
    }
    return rc;
}
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsInit
            (
                ANSC_HANDLE                 hDml,
                PANSC_HANDLE                phContext
            )


    description:

       This function enables the DNS client.

    argument:  
            ANSC_HANDLE                  hDml,

            PANSC_HANDLE                phContext

    return:     operation status.

**********************************************************************/


ANSC_STATUS
CosaDmlDnsInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    UNREFERENCED_PARAMETER(hDml);
    UNREFERENCED_PARAMETER(phContext);
    return ANSC_STATUS_SUCCESS;
}

/*
 *  DNS Client
 */
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsEnableClient
            (
                ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled
            )

    description:

       This function enables the DNS client.

    argument:   ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsEnableClient
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bEnabled
    )
{
    /* USGv2 not support Disable DNS Client */
    UNREFERENCED_PARAMETER(hContext);
    if( TRUE == bEnabled ){
        return ANSC_STATUS_SUCCESS;
    }else{
        return ANSC_STATUS_FAILURE;
    }
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlIpDnsGetClientStatus
            (
                ANSC_HANDLE                 hContext
            );

    description:

        This function gets the status of the DNS client.

    argument:   ANSC_HANDLE                 hContext
            

    return:     operation status.

**********************************************************************/
COSA_DML_DNS_STATUS
CosaDmlIpDnsGetClientStatus
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    return COSA_DML_DNS_STATUS_Enabled;    
}

/*
 *  DNS Client Server
 */
typedef enum dns_family{
    DNS_FAMILY_NONE = 0,
    DNS_FAMILY_IPV4,
    DNS_FAMILY_IPV6
}dns_family_t;

static int CHECK_V4_V6(char *str){
    if( NULL != strchr(str, ':'))
        return DNS_FAMILY_IPV6;
    if( NULL != strchr(str, '.'))
        return DNS_FAMILY_IPV4;

    return DNS_FAMILY_NONE;
}

static COSA_DML_DNS_STATUS GetDnsServerStatus(const char *ip, const char *interface)
{
    COSA_DML_DNS_STATUS status = COSA_DML_DNS_STATUS_Disabled;
    FILE *fd = NULL;
    char buff[256] = {};
    struct stat fs = {};
    const char *resolvConf = NULL;
    const char *dnsKey = DNS_KEY_RESOLVCONF;

    // Evaluate active config file for the specific interface.
    if (strcmp(interface, DEFAULT_WAN_INTERFACE) == 0)
    {
        if ((stat(DNS_RESOLV_CONF, &fs) == 0) && (fs.st_size > 0))
        {
            resolvConf = DNS_RESOLV_CONF;
        }
    }
    else if (strcmp(interface, DEFAULT_LAN_INTERFACE) == 0)
    {
        if ((stat(DNS_RELAY_RESOLV_CONF, &fs) == 0) && (fs.st_size > 0))
        {
            resolvConf = DNS_RELAY_RESOLV_CONF;
        }
        else if ((stat(DHCP_OPTIONS, &fs) == 0) && (fs.st_size > 0) && (CHECK_V4_V6(ip) == DNS_FAMILY_IPV4))
        {
            resolvConf = DHCP_OPTIONS;
            dnsKey = DNS_KEY_DHCPOPS;
        }
        else if ((stat(DHCPV6_OPTIONS, &fs) == 0) && (fs.st_size > 0) && (CHECK_V4_V6(ip) == DNS_FAMILY_IPV6))
        {
            resolvConf = DHCPV6_OPTIONS;
            dnsKey = DNS_KEY_DHCPV6OPS;
        }
    }

    if (resolvConf)
    {
        snprintf(buff, sizeof(buff) - 1, "cat %s | grep '%s' | grep -c -E '( |,)%s(,|$)' >&1", resolvConf, dnsKey, ip);
        fd = popen(buff, "r");
        if (fd)
        {
            if ((EOF != fgets(buff, sizeof(buff), fd)) && (0 != strncmp(buff, "0", 1)))
            {
                status = COSA_DML_DNS_STATUS_Enabled;
            }
            pclose(fd);
        }
    }
    return status;
}

/**********************************************************************

    caller:     self

    prototype:

        PCOSA_DML_DNS_CLIENT_SERVER
        CosaDmlDnsClientGetServers
            (
                ANSC_HANDLE                 hContext,
                PULONG                      pulCount
            );

    description:

        This routine is to retrieve the complete list of DNS servers, which is a dynamic table.

    argument:   ANSC_HANDLE                 hContext,
                PULONG                      pulCount
                To receive the actual number of entries.

    return:     The pointer to the array of DNS servers, allocated by callee. If no entry is found, NULL is returned.

**********************************************************************/
PCOSA_DML_DNS_CLIENT_SERVER
CosaDmlDnsClientGetServers
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount
    )
{
#if 0
    DNS_Client_t dns;
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext ctx;
    int i;
    ULONG j;    
    PCOSA_DML_DNS_CLIENT_SERVER pServer = NULL;
    *pulCount = 0;
    errno_t rc = -1;
    if (Utopia_Init(&ctx)){
        if(SUCCESS == Utopia_GetDNSServer(&ctx, &dns)){
            for(i = DNS_CLIENT_NAMESERVER_CNT -1; i >= 0 && (dns.dns_server[i][0] == 0) ; i--);
            *pulCount = i + 1;
            if(*pulCount > 0)
                 pServer = (PCOSA_DML_DNS_CLIENT_SERVER)AnscAllocateMemory(*pulCount * sizeof(COSA_DML_DNS_CLIENT_SERVER));
            if(pServer == NULL){
                *pulCount = 0;
            }else{
                for(i = 0, j = 0; j < *pulCount; j++ ){
                    int af = CHECK_V4_V6(dns.dns_server[j]);
                    CcspTraceWarning(("CosaDmlDnsClientGetServers -- af:%d,i:%d,j:%lu, server:%s \n", af, i, j, dns.dns_server[j]));
                    switch (af){
                    case DNS_FAMILY_IPV4:
                            /*inet_pton(AF_INET, dns.dns_server[i], &(pServer[i].DNSServer));*/
                            rc = strcpy_s(pServer[i].DNSServer,sizeof(pServer[i].DNSServer), dns.dns_server[j]);
                            ERR_CHK(rc);
                            pServer[i].Order          = 1 + i;
                            pServer[i].InstanceNumber = 1 + i;
                            pServer[i].bEnabled       = TRUE;
                            pServer[i].Type           = COSA_DML_DNS_ADDR_SRC_DHCPV4;
                            i++;
                            break;
                    case DNS_FAMILY_IPV6:
                            rc = strcpy_s(pServer[i].DNSServer,sizeof(pServer[i].DNSServer), dns.dns_server[j]);
                            ERR_CHK(rc);
                            pServer[i].Order          = 1 + i;
                            pServer[i].InstanceNumber = 1 + i;
                            pServer[i].bEnabled       = TRUE;
                            pServer[i].Type           = COSA_DML_DNS_ADDR_SRC_DHCPV6;
                            i++;
                            break;
                    default:
                            AnscFreeMemory(pServer);
                            pServer = NULL;
                            *pulCount = 0;
                            break;

                    }
                }
                *pulCount = i;
            }
        }
        Utopia_Free(&ctx, 0);
    }
    return pServer;
#else
    PCOSA_DML_DNS_CLIENT_SERVER pServer = NULL;
    DNS_Client_t dhcpcDns = {0};
    UtopiaContext ctx = {};
    int i = 0;
    int dnsCount = 0;
    int dhcpDnsCount = 0;
    int staticDnsCount = 0;
    *pulCount = 0;
    char server_enable[sizeof(SYSCFG_DNS_SERVER_ENABLE_KEY) + 1] = {0};
    char buf[3] = {0};

    if (!Utopia_Init(&ctx))
    {
        return NULL;
    }

    dhcpDnsCount = GetWanDhcpDns(&dhcpcDns);
    staticDnsCount = Utopia_GetNumberOfDnsServers(&ctx);

    dnsCount = dhcpDnsCount + staticDnsCount;
    if (dnsCount == 0)
    {
        Utopia_Free(&ctx, 0);
        return NULL;
    }

    pServer = (PCOSA_DML_DNS_CLIENT_SERVER)AnscAllocateMemory(dnsCount * sizeof(COSA_DML_DNS_CLIENT_SERVER));
    if (!pServer)
    {
        Utopia_Free(&ctx, 0);
        return NULL;
    }
    *pulCount = dnsCount;
    int af = DNS_FAMILY_NONE;
    for (i = 0; i < dhcpDnsCount; i++)
    {
        pServer[i].InstanceNumber = i + 1;
        pServer[i].Order = pServer[i].InstanceNumber;
        sprintf(pServer[i].Alias, "Server%d", pServer[i].InstanceNumber);
        AnscCopyString(pServer[i].DNSServer, dhcpcDns.dns_server[i]);
        AnscCopyString(pServer[i].Interface, DEFAULT_WAN_INTERFACE);
        snprintf(server_enable, sizeof(server_enable), SYSCFG_DNS_SERVER_ENABLE_KEY, pServer[i].InstanceNumber);
        syscfg_get(NULL, server_enable, buf, sizeof(buf));
        pServer[i].bEnabled = ((strcmp(pServer[i].DNSServer, "0.0.0.0") == 0) || (strcmp(pServer[i].DNSServer, "::") == 0) ? FALSE :
                                buf[0] != '\0' ? atoi(buf) : TRUE );
        /*If the DNS IP is 0.0.0.0 or if the user sets it to FALSE, the enable will be FALSE Otherwise the Enable will be TRUE   */

        if (pServer[i].bEnabled == TRUE)
        {
            pServer[i].Status = GetDnsServerStatus(pServer[i].DNSServer, pServer[i].Interface);
        }
        else
        {
            pServer[i].Status = COSA_DML_DNS_STATUS_Disabled;
        }

        af = CHECK_V4_V6(dhcpcDns.dns_server[i]);
        switch (af)
        {
        case DNS_FAMILY_IPV4:
            pServer[i].Type = COSA_DML_DNS_ADDR_SRC_DHCPV4;
            break;
        case DNS_FAMILY_IPV6:
            pServer[i].Type = COSA_DML_DNS_ADDR_SRC_DHCPV6;
            break;
        default:
            break;
        }
    }

    PCOSA_DML_DNS_CLIENT_SERVER pStaticServer = pServer + dhcpDnsCount;
    for (i = 0; i < staticDnsCount; i++)
    {
        dns_server_t dns = {};
        Utopia_GetDnsServerByIndex(&ctx, i, &dns);

        pStaticServer[i].InstanceNumber = dns.ins_num;
        pStaticServer[i].Order = pServer[i].InstanceNumber;
        AnscCopyString(pStaticServer[i].Alias, dns.alias);
        AnscCopyString(pStaticServer[i].DNSServer, dns.ip_address);
        AnscCopyString(pStaticServer[i].Interface, dns.interface);
        pStaticServer[i].bEnabled = dns.enable;
        if (pStaticServer[i].bEnabled == TRUE)
        {
            pStaticServer[i].Status = GetDnsServerStatus(pStaticServer[i].DNSServer, pStaticServer[i].Interface);
        }
        else
        {
            pStaticServer[i].Status = COSA_DML_DNS_STATUS_Disabled;
        }
        pStaticServer[i].Type = COSA_DML_DNS_ADDR_SRC_Static;
    }

    Utopia_Free(&ctx, 0);
    return pServer;
#endif
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientSetServerValues
            (
                ANSC_HANDLE             hContext,
                ULONG                       ulIndex,
                ULONG                       ulInstanceNumber,
                char*                        pAlias
            )


    description:

        The API sets DNS server values. 

    argument:   
            ANSC_HANDLE             hContext,

            ULONG                       ulIndex,

            ULONG                       ulInstanceNumber,

            char*                        pAlias
    
    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsClientSetServerValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pAlias);

#if 0
    char inst_str[10];
    char inst_num[32];
    errno_t safec_rc = -1;
    UtopiaContext ctx;
   
    if (Utopia_Init(&ctx))
    {

        safec_rc = sprintf_s(inst_num, sizeof(inst_num), "dns_client_server_instance_%lu", ulIndex);
        if(safec_rc < EOK)
        {
            ERR_CHK(safec_rc);
        }
        safec_rc = sprintf_s(inst_str, sizeof(inst_str), "%lu", ulInstanceNumber);
        if(safec_rc < EOK)
        {
            ERR_CHK(safec_rc);
        }
        Utopia_RawSet(&ctx, NULL, inst_num, inst_str);
        Utopia_Free(&ctx, 1); 
    }
#endif

    return SUCCESS;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientAddServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry
            );

    description:

        The API adds one DNS server entry into DNS client. 

    argument:   ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry
                Caller does not need to fill in Status or Alias fields. Field Type has to be COSA_DML_DNS_ADDR_SRC_Static. Caller does not have to fill in field Interface. Upon return, callee fills in the generated Alias.
    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaDmlDnsClientAddServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_CLIENT_SERVER pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
    int rc = -1;
    UtopiaContext ctx = {};

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    dns_server_t dns = {};
    dns.ins_num = pEntry->InstanceNumber;
    dns.enable = pEntry->bEnabled;
    AnscCopyString(dns.alias, pEntry->Alias);
    AnscCopyString(dns.ip_address, pEntry->DNSServer);
    AnscCopyString(dns.interface, (strlen(pEntry->Interface) > 0) ? pEntry->Interface : DEFAULT_WAN_INTERFACE);
    AnscCopyString(dns.type, "Static");

    rc = Utopia_AddDnsServer(&ctx, &dns);
    Utopia_Free(&ctx, !rc);

    if (rc == 0)
    {
        rc = RestartPlatform((CHECK_V4_V6(dns.ip_address) == DNS_FAMILY_IPV6) ? TRUE : FALSE);
    }
    return (rc == 0) ? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientDelServer
            (
                ANSC_HANDLE                 hContext,
                ULONG                       ulInstanceNumber
            );

    description:

        The API delete one DNS server entry from DNS client.

    argument:   ANSC_HANDLE                 hContext,
                ULONG                       ulInstanceNumber

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsClientDelServer
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    int rc = -1;
    UtopiaContext ctx = {};

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    int index = 0;
    dns_server_t dns = {};
    Utopia_GetDnsServerIndexByInsNum(&ctx, ulInstanceNumber, &index);
    Utopia_GetDnsServerByIndex(&ctx, index, &dns);
    boolean_t doIpv6Restart = (CHECK_V4_V6(dns.ip_address) == DNS_FAMILY_IPV6) ? TRUE : FALSE;

    rc = Utopia_RemoveDnsServer(&ctx, ulInstanceNumber);
    Utopia_Free(&ctx, !rc);

    if (rc == 0)
    {
        rc = RestartPlatform(doIpv6Restart);
    }
    return (rc == 0) ? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientSetServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry
            );

    description:

        This function sets the server of the DNS client.

    argument:   ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER pEntry

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsClientSetServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_CLIENT_SERVER pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
#if 0
    ULONG ipAddr;

    inet_pton(AF_INET, pEntry->DNSServer, &ipAddr);
	
    return CosaDmlDcSetWanNameServer(NULL, ipAddr, pEntry->InstanceNumber);
#else
    int rc = -1;
    UtopiaContext ctx = {};
    dns_server_t dns = {};
    int index = 0;
    boolean_t doIpv6Restart = FALSE;
    char server_enable[sizeof(SYSCFG_DNS_SERVER_ENABLE_KEY) + 1] = {0};
    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }
    if (pEntry->InstanceNumber <= DHCP_DNS_NUMBER * 2) //This checking is done to set the value for type DHCP separately
    {
         snprintf(server_enable, sizeof(server_enable), SYSCFG_DNS_SERVER_ENABLE_KEY, pEntry->InstanceNumber);
         syscfg_set(NULL, server_enable, pEntry->bEnabled ? "1" : "0");
         return ANSC_STATUS_SUCCESS;        
    }

    Utopia_GetDnsServerIndexByInsNum(&ctx, pEntry->InstanceNumber, &index);
    rc = Utopia_GetDnsServerByIndex(&ctx, index, &dns);

    if (rc == 0)
    {
        doIpv6Restart = ((CHECK_V4_V6(dns.ip_address) == DNS_FAMILY_IPV6) || (CHECK_V4_V6(pEntry->DNSServer) == DNS_FAMILY_IPV6)) ? TRUE : FALSE;
        dns.enable = pEntry->bEnabled;
        AnscCopyString(dns.alias, pEntry->Alias);
        AnscCopyString(dns.ip_address, pEntry->DNSServer);
        AnscCopyString(dns.interface, pEntry->Interface);
        rc = Utopia_SetDnsServerByIndex(&ctx, index, &dns);
    }
    Utopia_Free(&ctx, !rc);

    if (rc == 0)
    {
        rc = RestartPlatform(doIpv6Restart);
    }
    return (rc == 0) ? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;
#endif
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsClientGetServer
            (
                ANSC_HANDLE                                 hContext,
                PCOSA_DML_DNS_CLIENT_SERVER      pEntry
             )


    description:

        This routine is to retrieve the config of DNS servers, which is a dynamic table.

    argument:  
                    ANSC_HANDLE                 hContext,

                    PCOSA_DML_DNS_CLIENT_SERVER pEntry

    return:    operation status.  

**********************************************************************/

ANSC_STATUS
CosaDmlDnsClientGetServer
    (
        ANSC_HANDLE hContext,
        PCOSA_DML_DNS_CLIENT_SERVER pEntry
     )

{
    UNREFERENCED_PARAMETER(hContext);
#if 0
    ULONG count;
    PCOSA_DML_DNS_CLIENT_SERVER pTable;
    pTable = CosaDmlDnsClientGetServers(hContext, &count);
    ANSC_STATUS ret = ANSC_STATUS_FAILURE;
    ULONG i;
    
    if(pTable == NULL)
        return ret;
    for(i = 0; i < count ;i++){
        if( pTable[i].InstanceNumber == pEntry->InstanceNumber)
            memcpy(pEntry, &pTable[i], sizeof(COSA_DML_DNS_CLIENT_SERVER));
        ret = ANSC_STATUS_SUCCESS;
    }

    AnscFreeMemory(pTable);/*RDKB-6837, CID-33471, free unused resource before exit*/
#else
    UtopiaContext ctx = {};
    dns_server_t dns = {};
    int index = 0;

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    Utopia_GetDnsServerIndexByInsNum(&ctx, pEntry->InstanceNumber, &index);
    Utopia_GetDnsServerByIndex(&ctx, index, &dns);

    pEntry->bEnabled = dns.enable;
    AnscCopyString(pEntry->Alias, dns.alias);
    AnscCopyString(pEntry->DNSServer, dns.ip_address);
    AnscCopyString(pEntry->Interface, dns.interface);
    pEntry->Type = COSA_DML_DNS_ADDR_SRC_Static;
    pEntry->Status = (dns.enable == TRUE) ? COSA_DML_DNS_STATUS_Enabled : COSA_DML_DNS_STATUS_Disabled;

    Utopia_Free(&ctx, 0);
#endif
    return ANSC_STATUS_SUCCESS;
}

/*
 *  DNS Relay
 */
 
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsEnableRelay
            (
                ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled
            )

    description:

        This function enables the DNS relay.

    argument:   ANSC_HANDLE                 hContext,
                BOOLEAN                     bEnabled

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsEnableRelay
    (
        ANSC_HANDLE                 hContext,
        BOOLEAN                     bEnabled
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(bEnabled);
    UtopiaContext ctx = {};

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    boolean_t doRestart = (g_DnsRelayEnabled == bEnabled) ? FALSE : TRUE;
    g_DnsRelayEnabled = bEnabled;
    int rc = Utopia_SetDnsRelayEnabled(&ctx, g_DnsRelayEnabled);
    Utopia_Free(&ctx, !rc);

    if ((rc == 0) && (doRestart == TRUE))
    {
        rc = RestartPlatform(TRUE);

        if (rc == 0)
        {
            PCOSA_DATAMODEL_DNS    pDns = (PCOSA_DATAMODEL_DNS)g_pCosaBEManager->hDNS;

            if (pDns != NULL)
            {
                PCOSA_DML_DNS_RELAY    pRelay = &pDns->Relay;

                //update the status based on enabled state
                pRelay->Status = (g_DnsRelayEnabled) ? COSA_DML_DNS_STATUS_Enabled : COSA_DML_DNS_STATUS_Disabled;
            }
        }
 
    }
    return (rc == 0) ? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;
}


/**********************************************************************

    caller:     self

    prototype:

        COSA_DML_DNS_STATUS
        CosaDmlIpDnsGetRelayStatus
            (
                ANSC_HANDLE                 hContext
            )

    description:

        This function initiate  cosa nat object and return handle.

    argument:   ANSC_HANDLE                 hContext
          

    return:     operation status.

**********************************************************************/
COSA_DML_DNS_STATUS
CosaDmlIpDnsGetRelayStatus
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_RELAY         pRelay
    )
{
    COSA_DML_DNS_STATUS status = COSA_DML_DNS_STATUS_Error;
    boolean_t dslite_enable = false;
    UtopiaContext ctx = {};
    if (!Utopia_Init(&ctx))
    {
        return status;
    }

    int rc = Utopia_GetDnsRelayEnabled(&ctx, &g_DnsRelayEnabled);
    if (rc == 0)
    {
        Utopia_GetDsliteEnable(&ctx, &dslite_enable);
        status = ((g_DnsRelayEnabled | dslite_enable) == TRUE) ? COSA_DML_DNS_STATUS_Enabled : COSA_DML_DNS_STATUS_Disabled;
        pRelay->Status = status;
        pRelay->bEnabled = g_DnsRelayEnabled;
    }
    Utopia_Free(&ctx, 0);
    return status;
}
COSA_DML_DNS_STATUS
CosaDmlIpDnsGetRelayEnable
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    char buf[64];

    syscfg_get( NULL, "dns_relay_enable", buf, sizeof(buf));

    return strcmp(buf, "1") == 0 ? COSA_DML_DNS_STATUS_Enabled : COSA_DML_DNS_STATUS_Disabled;
}

/*
 *  DNS Relay Forwarding (server)
 */
/**********************************************************************

    caller:     self

    prototype:

        PCOSA_DML_DNS_RELAY_ENTRY
        CosaDmlDnsRelayGetServers
            (
                ANSC_HANDLE                 hContext,
                PULONG                      pulCount
            );

    description:

        This function Get the DNS relay server.

    argument:   ANSC_HANDLE                 hContext,
                PULONG                      pulCount
                To receive the actual number of entries.

    return:     The pointer to the array of DNS servers, allocated by callee. If no entry is found, NULL is returned.

**********************************************************************/
PCOSA_DML_DNS_RELAY_ENTRY
CosaDmlDnsRelayGetServers
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pulCount);
    PCOSA_DML_DNS_RELAY_ENTRY pForward = NULL;
    UtopiaContext ctx = {};
    int forwardCount = 0;
    *pulCount = 0;

    if (!Utopia_Init(&ctx))
    {
        return NULL;
    }

    forwardCount = Utopia_GetNumberOfDnsForwards(&ctx);

    pForward = (PCOSA_DML_DNS_CLIENT_SERVER)AnscAllocateMemory(forwardCount * sizeof(COSA_DML_DNS_CLIENT_SERVER));
    if (!pForward)
    {
        Utopia_Free(&ctx, 0);
        return NULL;
    }

    *pulCount = forwardCount;

    int i = 0;
    for (; i < forwardCount; i++)
    {
        relay_forward_t forward = {};
        Utopia_GetDnsForwardByIndex(&ctx, i, &forward);

        pForward[i].InstanceNumber = forward.ins_num;
        AnscCopyString(pForward[i].Alias, forward.alias);
        AnscCopyString(pForward[i].DNSServer, forward.ip_address);
        AnscCopyString(pForward[i].Interface, forward.interface);
        pForward[i].bEnabled = forward.enable;
        if (pForward[i].bEnabled == TRUE)
        {
            pForward[i].Status = GetDnsServerStatus(pForward[i].DNSServer, pForward[i].Interface);
        }
        else
        {
            pForward[i].Status = COSA_DML_DNS_STATUS_Disabled;
        }
        pForward[i].Type = COSA_DML_DNS_ADDR_SRC_Static;
    }

    Utopia_Free(&ctx, 0);
    return pForward;
}



/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelaySetServerValues
            (
                ANSC_HANDLE                 hContext,
                ULONG                           ulIndex,
                ULONG                           ulInstanceNumber,
                char*                            pAlias
            )


    description:

        This function Get the DNS relay server.

    argument:  
                 ANSC_HANDLE                 hContext,

                 ULONG                           ulIndex,

                 ULONG                           ulInstanceNumber,

                 char*                            pAlias

    return:     The pointer to the array of DNS servers, allocated by callee. If no entry is found, NULL is returned.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelaySetServerValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulIndex);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    UNREFERENCED_PARAMETER(pAlias);
    return ANSC_STATUS_FAILURE;
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelayAddServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry
            );

    description:

        The API adds one DNS server entry into DNS relay. 

    argument:   ANSC_HANDLE                 hContext,
    
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry
                Caller does not need to fill in Status or Alias fields. Field Type has to be COSA_DML_DNS_ADDR_SRC_Static. Caller does not have to fill in field Interface. Upon return, callee fills in the generated Alias.

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelayAddServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_RELAY_ENTRY   pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
    UtopiaContext ctx = {};

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    relay_forward_t forward = {};
    forward.ins_num = pEntry->InstanceNumber;
    forward.enable = pEntry->bEnabled;
    AnscCopyString(forward.alias, pEntry->Alias);
    AnscCopyString(forward.ip_address, pEntry->DNSServer);
    AnscCopyString(forward.interface, (strlen(pEntry->Interface) > 0) ? pEntry->Interface : DEFAULT_LAN_INTERFACE);
    AnscCopyString(forward.type, "Static");

    int rc = Utopia_AddDnsForward(&ctx, &forward);
    Utopia_Free(&ctx, !rc);

    if ((rc == 0) && (g_DnsRelayEnabled == TRUE))
    {
        rc = RestartPlatform((CHECK_V4_V6(forward.ip_address) == DNS_FAMILY_IPV6) ? TRUE : FALSE);
    }
    return (rc == 0) ? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelayDelServer
        (
            ANSC_HANDLE                 hContext,
            ULONG                       ulInstanceNumber
        );

    description:

        The API delete one DNS server entry from DNS client. 

    argument:   ANSC_HANDLE                 hContext,
    
                ULONG                       ulInstanceNumber

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelayDelServer
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    UtopiaContext ctx = {};

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    int index = 0;
    relay_forward_t forward = {};
    Utopia_GetDnsForwardIndexByInsNum(&ctx, ulInstanceNumber, &index);
    Utopia_GetDnsForwardByIndex(&ctx, index, &forward);
    boolean_t doIpv6Restart = (CHECK_V4_V6(forward.ip_address) == DNS_FAMILY_IPV6) ? TRUE : FALSE;

    int rc = Utopia_RemoveDnsForward(&ctx, ulInstanceNumber);
    Utopia_Free(&ctx, !rc);

    if ((rc == 0) && (g_DnsRelayEnabled == TRUE))
    {
        rc = RestartPlatform(doIpv6Restart);
    }
    return (rc == 0) ? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;
}
/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelaySetServer
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry
            );

    description:

        This function set the relay server.

    argument:   ANSC_HANDLE                 hContext,
    
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry

    return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaDmlDnsRelaySetServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_RELAY_ENTRY   pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
    int rc = -1;
    UtopiaContext ctx = {};
    relay_forward_t forward = {};
    int index = 0;
    boolean_t doIpv6Restart = FALSE;

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    Utopia_GetDnsForwardIndexByInsNum(&ctx, pEntry->InstanceNumber, &index);
    rc = Utopia_GetDnsForwardByIndex(&ctx, index, &forward);

    if (rc == 0)
    {
        doIpv6Restart = ((CHECK_V4_V6(forward.ip_address) == DNS_FAMILY_IPV6) || (CHECK_V4_V6(pEntry->DNSServer) == DNS_FAMILY_IPV6)) ? TRUE : FALSE;
        forward.enable = pEntry->bEnabled;
        AnscCopyString(forward.alias, pEntry->Alias);
        AnscCopyString(forward.ip_address, pEntry->DNSServer);
        AnscCopyString(forward.interface, pEntry->Interface);
        rc = Utopia_SetDnsForwardByIndex(&ctx, index, &forward);
    }

    Utopia_Free(&ctx, !rc);

    if ((rc == 0) && (g_DnsRelayEnabled == TRUE))
    {
        rc = RestartPlatform(doIpv6Restart);
    }
    return (rc == 0) ? ANSC_STATUS_SUCCESS : ANSC_STATUS_FAILURE;
}


/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaDmlDnsRelayGetServer
            (
                ANSC_HANDLE                           hContext,
                PCOSA_DML_DNS_RELAY_ENTRY   pEntry     
            )



    description:

        This routine is to retrieve the config of DNS servers, which is a dynamic table.

    argument:  
                    ANSC_HANDLE                          hContext,

                    PCOSA_DML_DNS_RELAY_ENTRY  pEntry

    return:    operation status.  

**********************************************************************/

ANSC_STATUS
CosaDmlDnsRelayGetServer
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_DNS_RELAY_ENTRY   pEntry     
    )


{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
    UtopiaContext ctx = {};
    relay_forward_t forward = {};
    int index = 0;

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    Utopia_GetDnsForwardIndexByInsNum(&ctx, pEntry->InstanceNumber, &index);
    Utopia_GetDnsForwardByIndex(&ctx, index, &forward);

    pEntry->bEnabled = forward.enable;
    AnscCopyString(pEntry->Alias, forward.alias);
    AnscCopyString(pEntry->DNSServer, forward.ip_address);
    AnscCopyString(pEntry->Interface, forward.interface);
    pEntry->Type = COSA_DML_DNS_ADDR_SRC_Static;
    pEntry->Status = (forward.enable == TRUE) ? COSA_DML_DNS_STATUS_Enabled : COSA_DML_DNS_STATUS_Disabled;

    Utopia_Free(&ctx, 0);
    return ANSC_STATUS_SUCCESS;
}
#endif

