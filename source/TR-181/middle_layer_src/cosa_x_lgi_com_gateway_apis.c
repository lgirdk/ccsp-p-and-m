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
static const char *UPDATE_RESOLV_CMD = "/bin/sh /etc/utopia/service.d/set_resolv_conf.sh";
#define CUSTOM_DATA_MODEL_ENABLED "custom_data_model_enabled"
#define PATH_PROC_UPTIME           "/proc/uptime"
#define IPV6_LEASE_TIME            "ipv6_"COSA_DML_DHCPV6_CLIENT_IFNAME"_pref_lifetime"
#define IPV6_LEASE_START_TIME      "ipv6_"COSA_DML_DHCPV6_CLIENT_IFNAME"_start_time"

enum {
    DHCPV6_SERVER_TYPE_STATEFUL  =1,
    DHCPV6_SERVER_TYPE_STATELESS
};

/* Static func */
static ANSC_STATUS getIPv6PreferredLifetime(char * fn, int * p_prefer, ipv6_addr_info_t * p_addr);

ANSC_STATUS
CosaDmlLgiGwGetIpv6LanMode
    (
        ANSC_HANDLE                 hContext,
        ULONG                       *pValue
    )
{
    if ( pValue != NULL )
    {
        *pValue = CosaDmlDhcpv6sGetType(NULL);
    }
    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlLgiGwSetIpv6LanMode
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulValue
    )
{
    return CosaDmlDhcpv6sSetType((ANSC_HANDLE)NULL, ulValue);
}

int
CosaDmlLgiGwGetDnsOverride
    (
        BOOL                        *pValue
    )
{
    char buf[8];
    memset(buf, 0, sizeof(buf));

    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "dns_override", buf, sizeof(buf));
    }
    else
    {
        CcspTraceError(("syscfg_init failed in %s\n",__FUNCTION__));
    }
    *pValue = (strcmp(buf, "true") == 0);
    return 0;
}

int
CosaDmlLgiGwSetDnsOverride
    (
        BOOL                        bValue
    )
{
    char buffer[6] = "true";
    pthread_t tid;
    int retVal = 0;

    if (syscfg_init() == 0)
    {
        if (!bValue)
        {
            strcpy(buffer,"false");
        }
        syscfg_set(NULL, "dns_override", buffer);
        syscfg_commit();

        pthread_create(&tid, NULL, &CosaDmlDNSOverride, NULL); 
    }
    else
    {
        CcspTraceError(("syscfg_init failed in %s\n",__FUNCTION__));
        retVal = -1;
    }
 
    return retVal;
}

int
CosaDmlLgiGwGetDnsIpv4Preferred
    (
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char buf[64];
    memset(buf, 0, sizeof(buf));

    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "dns_ipv4_preferred", buf, sizeof(buf));
        if(AnscSizeOfString(buf) == 0)
        {
            AnscCopyString(pValue, "0.0.0.0");
            return 0;
        }
        else if(AnscSizeOfString(buf) < *pUlSize)
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
    else
    {
        CcspTraceError(("syscfg_init failed in %s\n",__FUNCTION__));
    }

    return -1;
}

int
CosaDmlLgiGwGetDnsIpv4Alternate
    (
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char buf[64];
    memset(buf, 0, sizeof(buf));

    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "dns_ipv4_alternate", buf, sizeof(buf));
        if(AnscSizeOfString(buf) == 0)
        {
            AnscCopyString(pValue, "0.0.0.0");
            return 0;
        }
        else if(AnscSizeOfString(buf) < *pUlSize)
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
    else
    {
        CcspTraceError(("syscfg_init failed in %s\n",__FUNCTION__));
    }
    return -1;
}

int
CosaDmlLgiGwGetDnsIpv6Preferred
    (
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char buf[64];
    memset(buf, 0, sizeof(buf));

    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "dns_ipv6_preferred", buf, sizeof(buf));
        if(AnscSizeOfString(buf) == 0)
        {
            AnscCopyString(pValue, "0:0:0:0:0:0:0:0");
            return 0;
        }
        else if(AnscSizeOfString(buf) < *pUlSize)
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
    else
    {
        CcspTraceError(("syscfg_init failed in %s\n",__FUNCTION__));
    }
    return -1;
}

int
CosaDmlLgiGwGetDnsIpv6Alternate
    (
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char buf[64];
    memset(buf, 0, sizeof(buf));

    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "dns_ipv6_alternate", buf, sizeof(buf));
        if(AnscSizeOfString(buf) == 0)
        {
            AnscCopyString(pValue, "0:0:0:0:0:0:0:0");
            return 0;
        }
        else if(AnscSizeOfString(buf) < *pUlSize)
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
    else
    {
        CcspTraceError(("syscfg_init failed in %s\n",__FUNCTION__));
    }
    return -1;
}

void CosaDmlDNSOverride()
{
    pthread_detach(pthread_self());

    system(UPDATE_RESOLV_CMD);

    CosaDmlDHCPv6sTriggerRestart(TRUE);
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

static ANSC_STATUS
getIPv6PreferredLifetime(char * fn, int * p_prefer, ipv6_addr_info_t * p_addr)
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

ANSC_STATUS
CosaDml_Gateway_GetIPv6LeaseTimeRemaining(ULONG *pValue)
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

ULONG
CosaDmlGiGetCustomDataModelEnabled
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[16];

    syscfg_get (NULL, CUSTOM_DATA_MODEL_ENABLED, buf, sizeof(buf));

    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiSetCustomDataModelEnabled
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    syscfg_set (NULL, CUSTOM_DATA_MODEL_ENABLED, bValue ? "1" : "0");

    return ANSC_STATUS_SUCCESS;
}

