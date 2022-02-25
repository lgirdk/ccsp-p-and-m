/*********************************************************************************
* Copyright 2019 Liberty Global B.V.
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
*********************************************************************************/

#include "cosa_lgi_multicast_internal.h"
#include "cosa_lgi_multicast_apis.h"
#include <syscfg/syscfg.h>
#include "cosa_drg_common.h"

ULONG CosaDmlMulticastGetEnable ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[8];

    syscfg_get (NULL, "multicast_enable", buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlMulticastSetEnable ( ANSC_HANDLE hContext, BOOL bValue )
{
    syscfg_set_commit (NULL, "multicast_enable", bValue ? "1" : "0");

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlMulticastGetSnoopingEnable ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[8];

    syscfg_get (NULL, "multicast_snooping_enable", buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlMulticastSetSnoopingEnable ( ANSC_HANDLE hContext, BOOL bValue )
{
    syscfg_set_commit (NULL, "multicast_snooping_enable", bValue ? "1" : "0");

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlMulticastGetIGMPv3ProxyEnable ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[8];

    syscfg_get (NULL, "igmpproxy_enabled", buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlMulticastSetIGMPv3ProxyEnable ( ANSC_HANDLE hContext, BOOL bValue )
{
    if (syscfg_set (NULL, "igmpproxy_enabled", bValue ? "1" : "0") != 0)
        return ANSC_STATUS_FAILURE;

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlMulticastGetMLDv2ProxyEnable ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[8];

    syscfg_get (NULL, "mldproxy_enabled", buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlMulticastSetMLDv2ProxyEnable ( ANSC_HANDLE hContext, BOOL bValue )
{
    if (syscfg_set (NULL, "mldproxy_enabled", bValue ? "1" : "0") != 0)
        return ANSC_STATUS_FAILURE;

    return ANSC_STATUS_SUCCESS;
}
