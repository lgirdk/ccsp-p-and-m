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


#include "cosa_userinterface_apis.h"
#include "cosa_lgi_userinterface_apis.h"
#include <syscfg/syscfg.h>

/**************************************************************************

    module: cosa_lgi_userinterface_apis.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implements back-end apis for the COSA Data Model Library

        *  CosaDmlStdRaInit
        *  CosaDmlStdRaSetCfg
        *  CosaDmlStdRaGetCfg
    -------------------------------------------------------------------

**************************************************************************/

ANSC_STATUS
CosaDmlStdRaInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlStdRaSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_STD_RA_CFG        pCfg
    )
{
#if 0
/************************************************************************************************************
* According to the latest update to the CSR Login requirements, we are supposed to enable the CSR UI login
* only when the Password is set, not when the remote access is enabled. So the setting of syscfg variable
* and firewall restart is to be done after the Device.Users.User.{i}.Password is set
***********************************************************************************************************/
    const char* gui_session;
    gui_session = "/var/tmp/gui/";
    struct stat stats;

    /* HTTP/HTTPS enable */
    if (pCfg->bEnabled)
    {
       /* enable WAN access */
       if (syscfg_set(NULL, "mgmt_wan_access", "1") != 0)
       {
           fprintf(stderr, "Failed to enable WAN access\n");
           return ANSC_STATUS_FAILURE;
       }
    }
    else
    {
       /* enable WAN access */
       if (syscfg_set(NULL, "mgmt_wan_access", "0") != 0)
       {
           fprintf(stderr, "Failed to enable WAN access\n");
           return ANSC_STATUS_FAILURE;
       }
    }

    /* remove the GUI session file when Enabling or Disabling Remote Access*/
    if (stat(gui_session, &stats) == 0 && S_ISDIR(stats.st_mode)) {
        system("rm -rf /var/tmp/gui/session_*");
    }

    /* commit the change to persistent storage */
    syscfg_commit();

    /* Restarting Firewall */
    if (system("sysevent set firewall-restart") != 0)
    {
        fprintf(stderr, "%s: fail to restart firewall\n", __FUNCTION__);
        return ANSC_STATUS_FAILURE;
    }

    /* Restarting Lighttpd */
    system("/bin/sh /etc/start_lighttpd.sh restart");

    fprintf(stderr, "%s: OK !!!!\n", __FUNCTION__);
#endif
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlStdRaGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_STD_RA_CFG        pCfg
    )
{
    /* Enable flag - default to FALSE */
    pCfg->bEnabled = FALSE;

    /* Supported Protocols */
    snprintf(pCfg->SupportedProtocols, sizeof(pCfg->SupportedProtocols), "HTTP");

    /* Port - default to HTTPS */
    pCfg->Port = HTTP_INTERNAL_PORT;

    /* Protocol - default to HTTPS */
    snprintf(pCfg->Protocol, sizeof(pCfg->Protocol), "HTTP");

    return ANSC_STATUS_SUCCESS;
}
