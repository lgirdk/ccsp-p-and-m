/*********************************************************************
 * Copyright 2019 ARRIS Enterprises, LLC.
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

#include "cosa_lgi_cloudui_apis.h"
#include <syscfg/syscfg.h>

ANSC_STATUS
CosaDmlGetDhcpLanChangeHide
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pBool
    )
{
    char buf[8];
    memset(buf, sizeof(buf), 0);

    if(syscfg_init() == 0)
    {
        syscfg_get(NULL, "dhcp_lan_change_hide", buf, sizeof(buf));
    }

    if ( pBool != NULL )
    {
        *pBool = (BOOL)atoi(buf);
    }
    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlSetDhcpLanChangeHide
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    char buf[8] = {0};
    if(syscfg_init() == 0)
    {
        snprintf(buf, sizeof(buf), "%d", bValue);
        syscfg_set(NULL, "dhcp_lan_change_hide", buf);
        syscfg_commit();
    }
    return ANSC_STATUS_SUCCESS;
}
