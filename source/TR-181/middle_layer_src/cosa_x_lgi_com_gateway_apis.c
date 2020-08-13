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

