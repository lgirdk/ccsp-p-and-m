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
    pthread_t tid;

    syscfg_set_commit(NULL, "dns_override", bValue ? "true" : "false");

    pthread_create(&tid, NULL, &CosaDmlDNSOverride, NULL); 
 
    return 0;
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
    ipv6_addr_info_t * tmp_v6addr = NULL;
    ipv6_addr_info_t * p_v6addr = NULL;
    ipv6_addr_info_t * orig_p_v6addr = NULL;
    int  v6addr_num = 0;
    int i = 0;
    char cmd[SYS_CMD_STR_LEN] = {0};
    char dhcpv6_addr[IPV6_ADDR_STR_LEN] = {0};
    int  prefered_lft = 0;
    ANSC_STATUS retVal = ANSC_STATUS_FAILURE;

    if (CosaUtilGetIpv6AddrInfo(EROUTER_IF_NAME, &tmp_v6addr, &v6addr_num) != ANSC_STATUS_SUCCESS)
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Get erouter0 IPv6 address. */
    commonSyseventGet(COSA_DML_DHCPV6C_ADDR_SYSEVENT_NAME, dhcpv6_addr, sizeof(dhcpv6_addr));
    if (tmp_v6addr != NULL)
    {
        /* Save for free. */
        orig_p_v6addr = tmp_v6addr;
        for (i=0; i<v6addr_num; i++,tmp_v6addr++)
        {
            if ((tmp_v6addr->scope == IPV6_ADDR_SCOPE_GLOBAL) && (strncmp(tmp_v6addr->v6addr, dhcpv6_addr, sizeof(tmp_v6addr->v6addr)) == 0))
            {
                p_v6addr = tmp_v6addr;
                break;
            }
        }
    }

    /* Porting from X_CISCO_COM_PreferredLifetime implementated in cosa_ip_apis.c */
    sprintf(cmd, IPV6_SHOW_CMD, EROUTER_IF_NAME, TMP_IP_CMD_OUTPUT);
    system(cmd);
    retVal = getIPv6PreferredLifetime(TMP_IP_CMD_OUTPUT, &prefered_lft, p_v6addr);

    /* Free memory */
    if (orig_p_v6addr)
    {
        free(orig_p_v6addr);
    }

    if ( (pValue != NULL) && (retVal == ANSC_STATUS_SUCCESS) )
    {
        *pValue = prefered_lft;
    }

    return retVal;
}


#define RTF_UP 0x0001
#define RTF_GATEWAY 0x0002
#define PATH_ROUTE6  "/proc/net/ipv6_route"

#define ADDR_ISNONZERO( ipaddr, len ) ( ((char*)ipaddr)[0] | ((char*)ipaddr)[1] | \
                                        ((char*)ipaddr)[(len)-2] | ((char*)ipaddr)[(len)-1] )

#define IP_TO_STR_GEN( address, str, family ) \
    inet_ntop( family, address, str, INET6_ADDRSTRLEN )

static unsigned int parseHex(const char *strAddr, unsigned char  *inetAddr)
{
    unsigned int len=0;
    const char *p = NULL;
    if(NULL == (p = strAddr) )
    {
        return 0;
    }
    while (*p)
    {
        int tmp;
        if (p[1] == 0)
        {
            return 0;
        }
        if (sscanf(p, "%02x", &tmp) != 1)
        {
            return 0;
        }
        inetAddr[len] = tmp;
        len++;
        p += 2;
    }
    return len;
}

static ANSC_STATUS getInterfaceGWIAddr(const char *ifname, unsigned char  *defGwAddr)
{
    FILE *fp ;
    int  prefix_len, slen;
    int  metric, refcnt, use, ifflag;
    signed char  buff[4*INET6_ADDRSTRLEN];
    char iface[IF_NAMESIZE];
    char dstNet[INET6_ADDRSTRLEN];
    char srcNetSrc[INET6_ADDRSTRLEN];
    char nextHopSrc[INET6_ADDRSTRLEN];
    unsigned char  inet_dst[INET6_ADDRLEN];

    if ( (defGwAddr == NULL) || (ifname == NULL) )
    {
        return ANSC_STATUS_FAILURE;
    }

    if ( NULL == (fp = fopen(PATH_ROUTE6, "r")) )
    {
        return ANSC_STATUS_FAILURE;
    }

    while ( fgets( (char*)buff, 4*INET6_ADDRSTRLEN, fp) )
    {
        sscanf( (const char*)buff, "%s %02x %s %02x %s %x %x %x %x %s\n",
               dstNet, &prefix_len, srcNetSrc, &slen, nextHopSrc, &metric, &use, &refcnt, &ifflag, iface);

        if ( strlen(iface) != strlen(ifname) || strncmp(iface, ifname, strlen(iface)) )
        {
            continue;
        }
        if ( prefix_len )
        {
            continue;
        }
        parseHex(dstNet, inet_dst);
        if ( ADDR_ISNONZERO( inet_dst, INET6_ADDRLEN ) )
        {
            continue;
        }
        if ( !(ifflag & (RTF_UP | RTF_GATEWAY | RTF_ADDRCONF | RTF_DEFAULT | RTF_EXPIRES)) )
        {
            continue;
        }
        fclose(fp);
        parseHex(nextHopSrc, inet_dst);
        memcpy(defGwAddr, inet_dst, INET6_ADDRLEN);
        return ANSC_STATUS_SUCCESS;
    }

    fclose(fp);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS CosaDml_Gateway_GetIPv6Router ( ANSC_HANDLE hContext, char* pValue, ULONG* pulSize )
{
    unsigned char gwAddr6[INET6_ADDRLEN] = {0};
    char ipStr[INET6_ADDRSTRLEN] = {0};

    if (pValue == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }
    if(ANSC_STATUS_SUCCESS == getInterfaceGWIAddr(EROUTER_IF_NAME, gwAddr6))
    {
        IP_TO_STR_GEN(gwAddr6, ipStr, AF_INET6);
        AnscCopyString(pValue, ipStr);
    }
    else
    {
        AnscCopyString(pValue, "::");
    }
    return ANSC_STATUS_SUCCESS;
}
