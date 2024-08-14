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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include "cosa_x_lgi_com_gateway_apis.h"
#include <syscfg/syscfg.h>
#include <utctx/utctx.h>
#include <utapi/utapi.h>
#include <utapi/utapi_util.h>
#include "cosa_dhcpv6_apis.h"
#include "cosa_deviceinfo_apis_custom.h"
#include "cosa_drg_common.h"
#include "cm_hal.h"

extern ULONG CosaDmlDhcpv6sGetType(ANSC_HANDLE hContext);
extern ANSC_STATUS CosaDmlDhcpv6sSetType(ANSC_HANDLE hContext, ULONG type);
extern int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger);

ANSC_STATUS CosaDmlLgiGwGetIpv6LanMode ( ANSC_HANDLE hContext, ULONG *pValue )
{
    *pValue = CosaDmlDhcpv6sGetType(NULL);

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlLgiGwSetIpv6LanMode ( ANSC_HANDLE hContext, ULONG ulValue )
{
    return CosaDmlDhcpv6sSetType((ANSC_HANDLE)NULL, ulValue);
}

static void CosaDmlDNSOverride (void)
{
    pthread_detach(pthread_self());

    system("/bin/sh /etc/utopia/service.d/set_resolv_conf.sh");

    CosaDmlDHCPv6sTriggerRestart(TRUE);
}

int CosaDmlLgiGwGetDnsOverride ( BOOL *pValue )
{
    char buf[8];
    char override_mode[6];
    char erouter_mode[6];

    syscfg_get(NULL, "dns_override", buf, sizeof(buf));

    if (strcmp(buf, "true") == 0)
    {
        *pValue = TRUE;

        /* return FALSE if dns_override_mode is set and it doesn't match last_erouter_mode */

        if (syscfg_get(NULL, "dns_override_mode", override_mode, sizeof(override_mode)) == 0)
        {
            syscfg_get(NULL, "last_erouter_mode", erouter_mode, sizeof(erouter_mode));

            if (strcmp(override_mode, erouter_mode) != 0)
            {
                *pValue = FALSE;
            }
        }
    }
    else
    {
        *pValue = FALSE;
    }

    return 0;
}

int CosaDmlLgiGwSetDnsOverride ( BOOL bValue )
{
    char customer_db_dns_enabled[6];
    char erouter_mode[6];
    int retVal = ANSC_STATUS_FAILURE;

    // Public DNS allows to configure a set of preferred DNS servers different than the ones offered by your internet service provider.
    // This is part of CR039. Allow updates only if already defined in syscfg DB(), ie if it's not already present then it can't be set.

    if (syscfg_get(NULL, "dns_override", customer_db_dns_enabled, sizeof(customer_db_dns_enabled)) == 0)
    {
        pthread_t tid;
        char *nv = bValue ? "true" : "false";

        if (strcmp(customer_db_dns_enabled, nv) != 0)
        {
            syscfg_set(NULL, "dns_override", nv);
        }

        if (bValue)
        {
            syscfg_get(NULL, "last_erouter_mode", erouter_mode, sizeof(erouter_mode));
            syscfg_set_commit(NULL, "dns_override_mode", erouter_mode);
        }
        else
        {
            syscfg_set_commit(NULL, "dns_override_mode", "-1");
        }

        pthread_create(&tid, NULL, &CosaDmlDNSOverride, NULL);
        retVal = ANSC_STATUS_SUCCESS;
    }
    else
    {
        CcspTraceError(("dns_override not present in syscfg db %s\n",__FUNCTION__));
    }

    return retVal;
}

int CosaDmlLgiGwGetDnsIpv4Preferred ( char *pValue, ULONG *pUlSize )
{
    /*
       Returning an empty string (ie the default behaviour
       since no default syscfg value is defined) ie OK.
    */
    syscfg_get(NULL, "dns_ipv4_preferred", pValue, *pUlSize);

    return 0;
}

int CosaDmlLgiGwGetDnsIpv4Alternate ( char *pValue, ULONG *pUlSize )
{
    /*
       Returning an empty string (ie the default behaviour
       since no default syscfg value is defined) ie OK.
    */
    syscfg_get(NULL, "dns_ipv4_alternate", pValue, *pUlSize);

    return 0;
}

int CosaDmlLgiGwGetDnsIpv6Preferred ( char *pValue, ULONG *pUlSize )
{
    /*
       Returning an empty string (ie the default behaviour
       since no default syscfg value is defined) ie OK.
    */
    syscfg_get(NULL, "dns_ipv6_preferred", pValue, *pUlSize);

    return 0;
}

int CosaDmlLgiGwGetDnsIpv6Alternate ( char *pValue, ULONG *pUlSize )
{
    /*
       Returning an empty string (ie the default behaviour
       since no default syscfg value is defined) ie OK.
    */
    syscfg_get( NULL, "dns_ipv6_alternate", pValue, *pUlSize);

    return 0;
}

ANSC_STATUS CosaDml_Gateway_GetErouterInitMode(ULONG *pInitMode)
{
    if (!pInitMode || cm_hal_Get_ErouterModeControl(pInitMode))
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDml_Gateway_SetErouterInitMode(ULONG initMode)
{
    ANSC_STATUS retVal = ANSC_STATUS_FAILURE;

    retVal = cm_hal_Set_ErouterModeControl(initMode);
        
    return retVal;
}

#define SYS_CMD_STR_LEN 256
#define EROUTER_IF_NAME "erouter0"
#define IPV6_SHOW_CMD "ip -6 addr show dev %s > %s"
#define TMP_IP_CMD_OUTPUT  "/var/.ip_output_tmp"
#define TMP_FILE_BUF 256
#define TMP_FILE_STR_LEN 128
#define TMP_ADDR_STR_LEN 128
#define IPV6_ADDR_STR_LEN 64

static ANSC_STATUS getIPv6PreferredLifetime(char * fn, int * p_prefer, ipv6_addr_info_t * p_addr)
{
    FILE * fp = NULL;
    char buf[TMP_FILE_BUF] = {0};
    struct in6_addr  addr = { };
    char addr_str[TMP_ADDR_STR_LEN] = {0};
    char * p = NULL;
    char file_str[TMP_FILE_STR_LEN] = {0};
    int  pre_len = 0;
    BOOL  found = FALSE;

    if (p_addr == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    if (!(fp = fopen(fn, "r")))
    {
        return ANSC_STATUS_FAILURE;
    }

    while (fgets((char *)buf, sizeof(buf), fp))
    {
        if (strstr((char *)buf, "inet6") && strstr((char *)buf, "scope"))
        {
            inet_pton(AF_INET6, (char *)p_addr->v6addr, &addr);
            inet_ntop(AF_INET6, &addr, addr_str, sizeof(addr_str));

            p = strchr((char *)p_addr->v6pre, '/');
            if (!p)
            {
                continue;
            }

            sscanf(p, "/%d", &pre_len);

            sprintf(addr_str+strlen(addr_str), "/%d", pre_len);

            p = (char *)buf;
            while (isblank(*p))
            {
                p++;
            }

            if (sscanf(p, "inet6 %s", file_str) == 1)
            {
                if (!strncmp((char *)file_str, (char *)addr_str, sizeof(file_str)))
                {
                    found = TRUE;
                }
            }

            if (found != TRUE)
            {
                continue;
            }

            /*the next line is life time info*/
            memset(buf, 0, sizeof(buf));
            fgets((char *)buf, sizeof(buf), fp);

            p = (char *)buf;
            while (isblank(*p))
            {
                p++;
            }

            if ((p = strstr(p, "preferred_lft")) != NULL)
            {
                if (!strncmp(p, "preferred_lft forever", strlen("preferred_lft forever")))
                {
                    *p_prefer = 0xffffffff;
                    fclose(fp);
                    return ANSC_STATUS_SUCCESS;
                }

                if (sscanf(p, "preferred_lft %dsec", p_prefer) == 1)
                {
                    fclose(fp);
                    return ANSC_STATUS_SUCCESS;
                }
            }

        }
        memset(buf, 0, sizeof(buf));
    }
    if (fp != NULL)
    {
        fclose(fp);
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDml_Gateway_GetIPv6LeaseTimeRemaining(ULONG *pValue)
{
    char out[256];
    long prefered_lft = 0;
    ANSC_STATUS retVal = ANSC_STATUS_SUCCESS;

    /* This function returns the lease time for erouter0,
     * so the DHCPV6_SERVER_TYPE_STATEFUL is not relevant here.
     */

    if (commonSyseventGet("ipv6_" COSA_DML_DHCPV6_CLIENT_IFNAME "_pref_lifetime", &out, sizeof(out)) == 0)
    {
        long leaseTime = atol(out);

        if (leaseTime)
        {
            if (commonSyseventGet("ipv6_" COSA_DML_DHCPV6_CLIENT_IFNAME "_start_time", &out, sizeof(out)) == 0)
            {
                struct sysinfo si;
                long start_time = atol(out);

                sysinfo(&si);

                prefered_lft = leaseTime - (si.uptime - start_time);
            }
            else
            {
                CcspTraceError(("commonSyseventGet failed in %s to get %s\n", __FUNCTION__, "ipv6_" COSA_DML_DHCPV6_CLIENT_IFNAME "_start_time"));
                retVal = ANSC_STATUS_FAILURE;
            }
        }
        else
        {
            /* set preferred life time to forever */
            prefered_lft = -1;
        }
    }
    else
    {
        CcspTraceError(("commonSyseventGet failed in %s to get %s\n", __FUNCTION__, "ipv6_" COSA_DML_DHCPV6_CLIENT_IFNAME "_pref_lifetime"));
        retVal = ANSC_STATUS_FAILURE;
    }

    if (retVal == ANSC_STATUS_SUCCESS)
        *pValue = prefered_lft;

    return retVal;
}

static int DNS_Whitelist_InsGetIndex (ULONG ins)
{
    int i, ins_num, ret = -1;
    int total_count = 0;
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfDNSWhitelistedUrl(&ctx, &total_count);
    for (i = 0; i < total_count; i++)
    {
        Utopia_GetDNSWhitelistInsNumByIndex(&ctx, i, &ins_num);
        if (ins_num == ins)
        {
            ret = i;
            break;
        }
    }

    Utopia_Free(&ctx, 0);
    return ret;
}

static void stop_dns_filter(void)
{
    FILE *fd;
    char buff[64];
    int pid = 0;

    fd = fopen("/var/run/dns-filter.pid", "r");
    if (fd == NULL)
    {
        return;
    }

    if (fgets(buff, sizeof(buff), fd) != NULL)
    {
        pid = atoi(buff);
    }
    fclose(fd);
    fd = NULL;

    if (pid > 0)
    {
        sprintf(buff, "/proc/%d/cmdline", pid);
        fd = fopen(buff, "r");
        if (fd)
        {
            if (fgets(buff, sizeof(buff), fd) != NULL)
            {
                if (strstr(buff, "dns_filter"))
                {
                    if (kill(pid, SIGKILL) < 0)
                    {
                        CcspTraceInfo(("stop_dns_filter: kill returns error\n"));
                    }
                }
            }

            fclose(fd);
        }
    }
}

static void restart_dns_filter(void)
{
    stop_dns_filter();
    system("/usr/bin/dns_filter &");
}

ANSC_STATUS CosaDmlDNS_Rebind_GetConf( BOOL* value )
{
    char buf[8];

    syscfg_get(NULL, "dns_rebind_protection_enable", buf, sizeof(buf));

    *value = atoi(buf);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlDNS_Rebind_SetConf( BOOL value )
{
    syscfg_set_commit(NULL, "dns_rebind_protection_enable", value ? "1" : "0");

    if (value)
    {
        system("/usr/bin/dns_filter &");
    }
    else
    {
        stop_dns_filter();
    }

    commonSyseventSet("firewall-restart", "");

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlDNS_Whitelist_GetNumberOfEntries(void)
{
    UtopiaContext ctx;
    int total_count = 0;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfDNSWhitelistedUrl(&ctx, &total_count);
    Utopia_Free(&ctx, 0);

    return total_count;
}

ANSC_STATUS CosaDmlDNS_Whitelist_GetEntryByIndex(ULONG index, COSA_DML_DNS_WHITELIST *entry)
{
    UtopiaContext ctx;
    int total_count = 0;
    dns_whitelist_url_t dns_whitelist_entry;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfDNSWhitelistedUrl(&ctx, &total_count);
    if (index >= total_count)
        return ANSC_STATUS_FAILURE;

    Utopia_GetDNSWhitelistByIndex(&ctx, index, &dns_whitelist_entry);
    entry->InstanceNumber = dns_whitelist_entry.InstanceNumber;
    strncpy(entry->Url, dns_whitelist_entry.Url, sizeof(entry->Url));
    strncpy(entry->Description, dns_whitelist_entry.Description, sizeof(entry->Description));

    Utopia_Free(&ctx, 0);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlDNS_Whitelist_AddEntry(COSA_DML_DNS_WHITELIST *entry, BOOL dns_rebind_protection_enable)
{
    int rc = -1;
    UtopiaContext ctx;
    dns_whitelist_url_t dns_whitelist_entry;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    dns_whitelist_entry.InstanceNumber = entry->InstanceNumber;

    strncpy(dns_whitelist_entry.Url, entry->Url, sizeof(dns_whitelist_entry.Url));
    strncpy(dns_whitelist_entry.Description, entry->Description, sizeof(dns_whitelist_entry.Description));

    rc = Utopia_AddDNSWhitelist(&ctx, &dns_whitelist_entry);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;

    if (dns_rebind_protection_enable)
    {
        restart_dns_filter();
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlDNS_Whitelist_DelEntry(ULONG ins, BOOL dns_rebind_protection_enable)
{
    int rc = -1;
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_DelDNSWhitelist(&ctx, ins);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;

    if (dns_rebind_protection_enable)
    {
        restart_dns_filter();
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlDNS_Whitelist_GetConf(ULONG ins, COSA_DML_DNS_WHITELIST *entry)
{
    int index;

    if ((index = DNS_Whitelist_InsGetIndex(ins)) == -1)
        return ANSC_STATUS_FAILURE;

    return CosaDmlDNS_Whitelist_GetEntryByIndex(index, entry);
}

ANSC_STATUS CosaDmlDNS_Whitelist_SetConf(ULONG ins, COSA_DML_DNS_WHITELIST *entry, BOOL dns_rebind_protection_enable)
{
    int index;
    UtopiaContext ctx;
    dns_whitelist_url_t dns_whitelist_entry;
    int rc = -1;

    index = DNS_Whitelist_InsGetIndex(ins);
    if (index == -1 || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    dns_whitelist_entry.InstanceNumber = entry->InstanceNumber;
    strncpy(dns_whitelist_entry.Url, entry->Url, sizeof(dns_whitelist_entry.Url));
    strncpy(dns_whitelist_entry.Description, entry->Description, sizeof(dns_whitelist_entry.Description));

    rc = Utopia_SetDNSWhitelistByIndex(&ctx, index, &dns_whitelist_entry);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;

    if (dns_rebind_protection_enable)
    {
        restart_dns_filter();
    }

    return ANSC_STATUS_SUCCESS;
}
