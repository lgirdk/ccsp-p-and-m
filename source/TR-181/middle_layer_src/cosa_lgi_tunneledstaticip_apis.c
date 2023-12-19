/*****************************************************************************
 * Copyright 2022 Liberty Global B.V.
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
 ****************************************************************************/

#include "cosa_lgi_tunneledstaticip_apis.h"
#include "cosa_lgi_tunneledstaticip_internal.h"
#include <syscfg/syscfg.h>


ANSC_STATUS CosaDmlTunneledStaticIPGetEnable (ANSC_HANDLE hContext, BOOL *pValue)
{
    char buf[8];
    syscfg_get(NULL, "tunneled_static_ip_enable", buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetEnable (ANSC_HANDLE hContext, BOOL bValue)
{
    if (bValue)
    {
       char buf[8];
       syscfg_get(NULL, "rip_enabled", buf, sizeof(buf));
       if (!strcmp(buf,"1"))
       {
          return ANSC_STATUS_FAILURE;
       }
    }
    if (syscfg_set(NULL, "tunneled_static_ip_enable", bValue ? "1" : "0") != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetRadiusAuthServerPort (ANSC_HANDLE hContext, int *pValue)
{
    char buf[8];
    syscfg_get(NULL, "tunneled_static_ip_auth_port", buf, sizeof(buf));
    *pValue = atoi(buf);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetRadiusAuthServerPort (ANSC_HANDLE hContext, int value)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", value);
    if (syscfg_set(NULL, "tunneled_static_ip_auth_port", buf) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetRadiusAccServerPort (ANSC_HANDLE hContext, int *pValue)
{
    char buf[8];
    syscfg_get(NULL, "tunneled_static_ip_acct_port", buf, sizeof(buf));
    *pValue = atoi(buf);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetRadiusAccServerPort (ANSC_HANDLE hContext, int value)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", value);
    if (syscfg_set(NULL, "tunneled_static_ip_acct_port", buf) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetUsername (ANSC_HANDLE hContext, char *pValue)
{
    syscfg_get(NULL, "tunneled_static_ip_username", pValue, TUNNELEDSTATICIP_CFG_MEMBER_SIZE(Username));
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetUsername (ANSC_HANDLE hContext, char *pValue)
{
    if (syscfg_set(NULL, "tunneled_static_ip_username", pValue) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetPassword (ANSC_HANDLE hContext, char *pValue)
{
    syscfg_get(NULL, "tunneled_static_ip_password", pValue, TUNNELEDSTATICIP_CFG_MEMBER_SIZE(Password));
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetPassword (ANSC_HANDLE hContext, char *pValue)
{
    if (syscfg_set(NULL, "tunneled_static_ip_password", pValue) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetRadiusAuthServerIPAddr (ANSC_HANDLE hContext, char *pValue)
{
    syscfg_get(NULL, "tunneled_static_ip_authserver", pValue, TUNNELEDSTATICIP_CFG_MEMBER_SIZE(RadiusAuthServerIPAddr));
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetRadiusAuthServerIPAddr (ANSC_HANDLE hContext, char *pValue)
{
    if (syscfg_set(NULL, "tunneled_static_ip_authserver", pValue) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetRadiusAuthSecret (ANSC_HANDLE hContext, char *pValue)
{
    syscfg_get(NULL, "tunneled_static_ip_authsecret", pValue, TUNNELEDSTATICIP_CFG_MEMBER_SIZE(RadiusAuthSecret));
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetRadiusAuthSecret (ANSC_HANDLE hContext, char *pValue)
{
    if (syscfg_set(NULL, "tunneled_static_ip_authsecret", pValue) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetNASIdentifier (ANSC_HANDLE hContext, char *pValue)
{
    syscfg_get(NULL, "tunneled_static_ip_nasid", pValue, TUNNELEDSTATICIP_CFG_MEMBER_SIZE(NASIdentifier));
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetNASIdentifier (ANSC_HANDLE hContext, char *pValue)
{
    if (syscfg_set(NULL, "tunneled_static_ip_nasid", pValue) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetRadiusAccServerIPAddr (ANSC_HANDLE hContext, char *pValue)
{
    syscfg_get(NULL, "tunneled_static_ip_acctserver", pValue, TUNNELEDSTATICIP_CFG_MEMBER_SIZE(RadiusAccServerIPAddr));
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetRadiusAccServerIPAddr (ANSC_HANDLE hContext, char *pValue)
{
    if (syscfg_set(NULL, "tunneled_static_ip_acctserver", pValue) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetRadiusAccSecret (ANSC_HANDLE hContext, char *pValue)
{
    syscfg_get(NULL, "tunneled_static_ip_acctsecret", pValue, TUNNELEDSTATICIP_CFG_MEMBER_SIZE(RadiusAccSecret));
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetRadiusAccSecret (ANSC_HANDLE hContext, char *pValue)
{
    if (syscfg_set(NULL, "tunneled_static_ip_acctsecret", pValue) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPGetRadiusInterface (ANSC_HANDLE hContext, int *pValue)
{
    char buf[8];
    syscfg_get(NULL, "tunneled_static_ip_interface", buf, sizeof(buf));
    *pValue = atoi(buf);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlTunneledStaticIPSetRadiusInterface (ANSC_HANDLE hContext, int value)
{
    if (syscfg_set_u(NULL, "tunneled_static_ip_interface", value) != 0)
        return ANSC_STATUS_FAILURE;
    return ANSC_STATUS_SUCCESS;
}
