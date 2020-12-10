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

#include "cosa_x_lgi_com_gateway_apis.h"
#include <syscfg/syscfg.h>
#include "cosa_dhcpv6_apis.h"

extern ULONG CosaDmlDhcpv6sGetType(ANSC_HANDLE hContext);
extern ANSC_STATUS CosaDmlDhcpv6sSetType(ANSC_HANDLE hContext, ULONG type);
extern int CosaDmlDHCPv6sTriggerRestart(BOOL OnlyTrigger);

#define PATH_PROC_UPTIME           "/proc/uptime"
#define IPV6_LEASE_TIME            "ipv6_"COSA_DML_DHCPV6_CLIENT_IFNAME"_pref_lifetime"
#define IPV6_LEASE_START_TIME      "ipv6_"COSA_DML_DHCPV6_CLIENT_IFNAME"_start_time"

enum {
    DHCPV6_SERVER_TYPE_STATEFUL  =1,
    DHCPV6_SERVER_TYPE_STATELESS
};

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

    syscfg_get(NULL, "dns_override", buf, sizeof(buf));

    *pValue = (strcmp(buf, "true") == 0);

    return 0;
}

int CosaDmlLgiGwSetDnsOverride ( BOOL bValue )
{
    char customer_db_dns_enabled[6];
    int retVal = ANSC_STATUS_FAILURE;

    // Public DNS allows to configure a set of preferred DNS servers different than the ones offered by your internet service provider.
    // This is part of CR039. Allow updates only if already defined in syscfg DB(), ie if it's not already present then it can't be set.

    if (syscfg_get(NULL, "dns_override", customer_db_dns_enabled, sizeof(customer_db_dns_enabled)) == 0)
    {
        pthread_t tid;
        char *nv = bValue ? "true" : "false";

        if (strcmp(customer_db_dns_enabled, nv) != 0)
        {
            syscfg_set_commit(NULL, "dns_override", nv);
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
    char buf[64];

    syscfg_get(NULL, "dns_ipv4_preferred", buf, sizeof(buf));

    if (AnscSizeOfString(buf) == 0)
    {
        AnscCopyString(pValue, "0.0.0.0");
        return 0;
    }

    if (AnscSizeOfString(buf) < *pUlSize)
    {
        AnscCopyString(pValue, buf);
        return 0;
    }
    else
    {
        *pUlSize = AnscSizeOfString(buf);
        return 1;
    }
}

int CosaDmlLgiGwGetDnsIpv4Alternate ( char *pValue, ULONG *pUlSize )
{
    char buf[64];

    syscfg_get(NULL, "dns_ipv4_alternate", buf, sizeof(buf));

    if (AnscSizeOfString(buf) == 0)
    {
        AnscCopyString(pValue, "0.0.0.0");
        return 0;
    }

    if (AnscSizeOfString(buf) < *pUlSize)
    {
        AnscCopyString(pValue, buf);
        return 0;
    }
    else
    {
        *pUlSize = AnscSizeOfString(buf);
        return 1;
    }
}

int CosaDmlLgiGwGetDnsIpv6Preferred ( char *pValue, ULONG *pUlSize )
{
    char buf[64];

    syscfg_get(NULL, "dns_ipv6_preferred", buf, sizeof(buf));

    if (AnscSizeOfString(buf) == 0)
    {
        AnscCopyString(pValue, "0:0:0:0:0:0:0:0");
        return 0;
    }

    if (AnscSizeOfString(buf) < *pUlSize)
    {
        AnscCopyString(pValue, buf);
        return 0;
    }
    else
    {
        *pUlSize = AnscSizeOfString(buf);
        return 1;
    }
}

int CosaDmlLgiGwGetDnsIpv6Alternate ( char *pValue, ULONG *pUlSize )
{
    char buf[64];

    syscfg_get( NULL, "dns_ipv6_alternate", buf, sizeof(buf));

    if (AnscSizeOfString(buf) == 0)
    {
        AnscCopyString(pValue, "0:0:0:0:0:0:0:0");
        return 0;
    }

    if (AnscSizeOfString(buf) < *pUlSize)
    {
        AnscCopyString(pValue, buf);
        return 0;
    }
    else
    {
        *pUlSize = AnscSizeOfString(buf);
        return 1;
    }
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
    ANSC_STATUS retVal = ANSC_STATUS_SUCCESS;
    char out[256]= "";
    long start_time = 0, now = 0, prefered_lft = 0, tmp;

    if (pValue != NULL)
    {
        int ipv6_mode = CosaDmlDhcpv6sGetType(NULL);

        /* The value of preferred life time MUST be 0 if the AddressSource is not DHCP.*/
        if (ipv6_mode == DHCPV6_SERVER_TYPE_STATEFUL)
        {
            if (!commonSyseventGet(IPV6_LEASE_TIME, &out, sizeof(out)))
            {
                long leaseTime = atol(out);
                if (leaseTime)
                {
                    FILE *fp = fopen(PATH_PROC_UPTIME, "r");
                    if (fp != NULL)
                    {
                        fscanf(fp, "%ld, %ld", &now, &tmp);
                        fclose(fp);

                        memset(out,0,sizeof(out));
                        if( !commonSyseventGet(IPV6_LEASE_START_TIME, &out, sizeof(out)))
                        {
                            start_time = atol(out);
                            prefered_lft = leaseTime - (now - start_time);
                        }
                        else
                        {
                            CcspTraceError(("commonSyseventGet failed in %s to get %s\n",__FUNCTION__,IPV6_LEASE_START_TIME));
                            retVal = ANSC_STATUS_FAILURE;
                        }
                    }
                    else
                    {
                        CcspTraceError(("%s: Failed to open %s\n",__FUNCTION__,PATH_PROC_UPTIME));
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
                CcspTraceError(("commonSyseventGet failed in %s to get %s\n",__FUNCTION__,IPV6_LEASE_TIME));
                retVal = ANSC_STATUS_FAILURE;
            }
        }
        if (retVal == ANSC_STATUS_SUCCESS)
            *pValue = prefered_lft;
    }
    else
    {
        CcspTraceError(("%s: Invalid memory %d\n",__FUNCTION__,__LINE__));
        retVal = ANSC_STATUS_FAILURE;
    }

    return retVal;
}

