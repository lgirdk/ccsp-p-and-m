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

#include <syscfg/syscfg.h>

#include "cosa_x_cisco_com_devicecontrol_apis.h"
#include "cosa_deviceinfo_apis.h"
#include "cosa_lgi_general_apis.h"
#include "plugin_main_apis.h"

#include <platform_hal.h>

extern void* g_pDslhDmlAgent;

ULONG
CosaDmlGiGetFirstInstallWizardEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[16];

    syscfg_get (NULL, "FirstInstallWizard_Enable", buf, sizeof(buf));

    *pValue = (strcmp(buf, "true") == 0);

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiSetFirstInstallWizardEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    syscfg_set (NULL, "FirstInstallWizard_Enable", bValue ? "true" : "false");

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetSTPEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[8];

    syscfg_get (NULL, "stp_enable", buf, sizeof(buf));

    *pValue = (strcmp(buf, "true") == 0);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetSTPEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    syscfg_set_commit (NULL, "stp_enable", bValue ? "true" : "false");

#ifdef _LG_MV2_PLUS_
    if (access("/sys/class/net/brlan0/", F_OK) != 0)
    {
        return ANSC_STATUS_SUCCESS;
    }
    else
    {	
        if (bValue)
        {
            if (access("/sys/devices/virtual/net/ovs-system/", F_OK) == 0)
            {
                system("ovs-vsctl set bridge brlan0 stp_enable=true");
            }
            else
            {
                system("brctl stp brlan0 on");
            }
        }
        else
        {
            if (access("/sys/devices/virtual/net/ovs-system/", F_OK) == 0)
            {
                system("ovs-vsctl set bridge brlan0 stp_enable=false");
            }
            else
            {
                system("brctl stp brlan0 off");
            }
        }
    }
#endif

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiSaveSettings()
{
    return syscfg_commit();
}

ANSC_STATUS
CosaDmlGiGetCAppName
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue,
        size_t                      len
    )
{
    syscfg_get (NULL, "CApp_Name", pValue, len);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetWebsiteHelpURL
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue,
        size_t                      len
    )
{
    syscfg_get (NULL, "Website_HelpURL", pValue, len);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetFirstInstallState
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[16];

    syscfg_get (NULL, "FirstInstall_State", buf, sizeof(buf));

    *pValue = (strcmp(buf, "true") == 0);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetTroubleshootWizardEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[16];

    syscfg_get( NULL, "troubleshoot_wizard_enable", buf, sizeof(buf));

    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiGetSKU (ANSC_HANDLE hContext, char *pValue, ULONG ulSize)
{
    char buf[20];
    char *sku = NULL;
    int customerId = 0;

    if (syscfg_get (NULL, "Customer_Index", buf, sizeof(buf)) == 0)
    {
        customerId = atoi(buf);
    }

#if defined (_PUMA6_ARM_)

    ULONG len;

    buf[0] = 0;
    len = sizeof(buf);
    CosaDmlDiGetProductClass (hContext, buf, &len);

    if (strcmp (buf, "CH7465LG") == 0)
    {
        if (customerId == 6)
        {
            sku = "Nextgen WiFi";
        }
        else if (customerId == 8)
        {
            sku = "Hub 3";
        }
    }

#elif defined (_LG_MV2_PLUS_)

    if (customerId == 8)
    {
        sku = "Hub 5";
    }
    else if (customerId == 20)
    {
        sku = "SmartWifi modem";
    }
    else if (customerId == 41)
    {
        sku = "Virgin Media Hub 6";
    }
    else if (customerId == 50)
    {
        sku = "yallo Home Cable Box 2";
    }
    else if (customerId == 51)
    {
        sku = "Connect Box 3";
    }
    else if (customerId == 53)
    {
        sku = "Giga Connect Box 6";
    }

#elif defined (_LG_MV3_)

    if (customerId == 8)
    {
        sku = "Hub 5x";
    }
    else if (customerId == 20)
    {
        sku = "SmartWifi modem";
    }
    else if (customerId == 41)
    {
        sku = "Fibre Hub";
    }
    else if (customerId == 50)
    {
        sku = "Home Fiber Box 2";
    }
    else if (customerId == 51)
    {
        sku = "Connect Box 3 Fiber";
    }
    else if (customerId == 53)
    {
        sku = "Giga Connect Box 6 Fiber";
    }

#endif

    if (sku == NULL)
    {
        sku = "Connect Box";
    }

    snprintf (pValue, ulSize, sku);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetCustomerId
    (
        ANSC_HANDLE                 hContext,
        ULONG                       *pValue
    )
{
    char buf[12];

    syscfg_get (NULL, "Customer_Index", buf, sizeof(buf));

    *pValue = (ULONG)atoi(buf);

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiSetCustomerId
(
    ANSC_HANDLE                 hContext,
    ULONG                       ulValue
)
{
    ULONG id = 0;

    CosaDmlGiGetCustomerId(NULL, &id);
    
    /* compare the new value with the current customer ID */
    if (id != ulValue)
    {
        /*
            This logic is only for testing multiple customer indexes.
            In the field customer index won't change.
            Customer specific file will be loaded in syscfg db from
            syscfg_create on the next boot.
        */
        FILE *fpt = fopen("/nvram/bootconfig_custindex", "w");
        if(fpt)
        {
            fprintf(fpt, "%d\n", (int)ulValue);
            fclose(fpt);

#if defined(_LG_MV2_PLUS_)
            fpt = fopen("/nvram/sagemCmCustomerId", "w");
            if (fpt)
            {
                fprintf(fpt, "%d\n", (int)ulValue);
                fclose(fpt);
            }
#endif

            CcspTraceInfo(("Customer id Changed from %d to %d\n", id, (int)ulValue));

#ifdef _PUMA6_ARM_
            {
                char rpcCmd[64];
                /* set the customer index value in atom for wifi module */
                sprintf (rpcCmd, "rpcclient2 'apply_customer_index.sh %u'", (unsigned int) ulValue);
                system (rpcCmd);
            }
#endif
            CosaDmlDcSetRebootDevice(NULL, "Device");
        }
        else
        {
            CcspTraceError(("Not able to open file in %s\n", __func__));
        }
    }
    else
    {
        CcspTraceInfo(("Customer id didn't change. Customer_Index - %d\n",(int)ulValue));
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetCAppName
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    syscfg_set (NULL, "CApp_Name", pValue);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetWebsiteHelpURL
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    syscfg_set (NULL, "Website_HelpURL", pValue);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetFirstInstallState
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    syscfg_set (NULL, "FirstInstall_State", bValue ? "true" : "false");

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetTroubleshootWizardEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    syscfg_set (NULL, "troubleshoot_wizard_enable", bValue ? "1" : "0");

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetRipIpAddress
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char rip_status[8];
    char staticErouterEnable[8];
    char staticBrlanEnable[8];
    char rip_static_ip[20];

    rip_static_ip[0] = 0;
    syscfg_get(NULL, "rip_enabled", rip_status, sizeof(rip_status));

    if (strcmp(rip_status, "1") == 0)
    {
        syscfg_get(NULL, "erouter_static_ip_enable", staticErouterEnable, sizeof(staticErouterEnable));
        syscfg_get(NULL, "brlan_static_ip_enable", staticBrlanEnable, sizeof(staticBrlanEnable));

        if (strcmp(staticErouterEnable, "true") == 0)
        {
            syscfg_get( NULL, "erouter_static_ip_address",rip_static_ip, sizeof(rip_static_ip));
        }
        else if (strcmp(staticBrlanEnable, "true") == 0)
        {
            syscfg_get( NULL, "brlan_static_lan_ipaddr",rip_static_ip, sizeof(rip_static_ip));
        }

        if (AnscSizeOfString(rip_static_ip) >= *pUlSize)
        {
            return ANSC_STATUS_FAILURE;
        }
    }

    AnscCopyString(pValue, rip_static_ip);
    return ANSC_STATUS_SUCCESS;

}

ANSC_STATUS
CosaDmlGiGetRipSubnetMask
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char rip_status[8];
    char staticErouterEnable[8];
    char staticBrlanEnable[8];
    char rip_static_mask[20];

    rip_static_mask[0] = 0;
    syscfg_get(NULL, "rip_enabled", rip_status, sizeof(rip_status));

    if (strcmp(rip_status, "1") == 0)
    {
        syscfg_get(NULL, "erouter_static_ip_enable", staticErouterEnable, sizeof(staticErouterEnable));
        syscfg_get(NULL, "brlan_static_ip_enable", staticBrlanEnable, sizeof(staticBrlanEnable));

        if (strcmp(staticErouterEnable, "true") == 0)
        {
            syscfg_get( NULL, "erouter_static_ip_mask",rip_static_mask, sizeof(rip_static_mask));
        }
        else if (strcmp(staticBrlanEnable, "true") == 0)
        {
            syscfg_get( NULL, "brlan_static_lan_netmask",rip_static_mask, sizeof(rip_static_mask));
        }

        if (AnscSizeOfString(rip_static_mask) >= *pUlSize)
        {
            return ANSC_STATUS_FAILURE;
        }
    }

    AnscCopyString(pValue, rip_static_mask);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetCurrentLanguage
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue,
        size_t                      len
    )
{
    syscfg_get (NULL, "Current_Language", pValue, len);

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiGetAvailableLanguages
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char buf[128];
    size_t len;

    syscfg_get (NULL, "Available_Languages", buf, sizeof(buf));

    len = strlen (buf);

    if (len < *pUlSize)
    {
        memcpy (pValue, buf, len + 1);
        return 0;
    }

    *pUlSize = len + 1;

    return 1;
}

ULONG
CosaDmlGiSetCurrentLanguage
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    syscfg_set (NULL, "Current_Language", pValue);

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlGiGetWebUISkin ( ANSC_HANDLE hContext, char *pValue, ULONG *pUlSize )
{
    char buf[16];

    syscfg_get (NULL, "Web_UI_Skin", buf, sizeof(buf));

    if(AnscSizeOfString(buf) < *pUlSize)
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

ULONG CosaDmlGiSetWebUISkin ( ANSC_HANDLE hContext, char *pValue )
{
    syscfg_set (NULL, "Web_UI_Skin", pValue);

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlLGiSetUiHashPassword
    (
        char                        *pValue
    )
{
    char name[64];
    ULONG size = sizeof(name);
    syscfg_set(NULL, "hash_password_3", pValue);
    syscfg_commit();
    //Get the datamodel to update the structure
    g_GetParamValueString(g_pDslhDmlAgent, "Device.Users.User.3.X_CISCO_COM_Password", name, &size);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetBrightness
    (
        ANSC_HANDLE                 hContext,
        ULONG                       *pValue
    )
{
    char buf[ 12 ] = { 0 };

    if (0 == syscfg_get (NULL, "led_brightness", buf, sizeof(buf)))
    {
        *pValue = (ULONG)atoi(buf);
        return ANSC_STATUS_SUCCESS;
    }
    return ANSC_STATUS_FAILURE;
}

ULONG
CosaDmlGiSetBrightness
    (
        ANSC_HANDLE                 hContext,
        ULONG                       pValue
    )
{
    if (platform_hal_SetLedBrightness(pValue) != RETURN_OK)
    {
        return ANSC_STATUS_FAILURE;
    }

    syscfg_set_u (NULL, "led_brightness", pValue);

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlGiGetDefaultAdminPassword ( ANSC_HANDLE hContext, char *pValue, ULONG *pUlSize )
{
    if (platform_hal_getUIDefaultPassword (pValue, *pUlSize) == RETURN_OK)
    {
        /*
           If the HAL returns an empty string it's bug in the HAL (it
           should return an error instead), but try to handle it anyway.
        */
        if (strlen (pValue) > 0)
        {
            return ANSC_STATUS_SUCCESS;
        }
    }

    strcpy (pValue, "password");

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiGetUserBridgeModeAllowed
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[8];

    syscfg_get(NULL, "user_bridge_mode_allowed", buf, sizeof(buf));

    *pValue = (strcmp(buf, "true") == 0);

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiSetUserBridgeModeAllowed
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    syscfg_set(NULL, "user_bridge_mode_allowed", bValue ? "true" : "false");

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiGetLedDSErrorTimer ( ANSC_HANDLE hContext, int *pValue )
{
    char buf[12];

    syscfg_get(NULL, "LedDSErrorTimer", buf, sizeof(buf));

    *pValue = atoi(buf);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiSetLedDSErrorTimer ( ANSC_HANDLE hContext, int value )
{
    if (platform_hal_SetLedDSErrorTimer(value) != RETURN_OK)
    {
        return ANSC_STATUS_FAILURE;
    }

    syscfg_set_u(NULL, "LedDSErrorTimer", (unsigned long) value);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiGetLedUSErrorTimer ( ANSC_HANDLE hContext, int *pValue )
{
    char buf[12];

    syscfg_get(NULL, "LedUSErrorTimer", buf, sizeof(buf));

    *pValue = atoi(buf);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiSetLedUSErrorTimer ( ANSC_HANDLE hContext, int value )
{
    if (platform_hal_SetLedUSErrorTimer(value) != RETURN_OK)
    {
        return ANSC_STATUS_FAILURE;
    }

    syscfg_set_u(NULL, "LedUSErrorTimer", (unsigned long) value);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiGetLedRegistrationErrorTimer ( ANSC_HANDLE hContext, int *pValue )
{
    char buf[12];

    syscfg_get(NULL, "LedRegistrationErrorTimer", buf, sizeof(buf));

    *pValue = atoi(buf);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiSetLedRegistrationErrorTimer ( ANSC_HANDLE hContext, int value )
{
    if (platform_hal_SetLedRegistrationErrorTimer(value) != RETURN_OK)
    {
        return ANSC_STATUS_FAILURE;
    }

    syscfg_set_u(NULL, "LedRegistrationErrorTimer", (unsigned long) value);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiGetLedPONRegistrationErrorTimer ( ANSC_HANDLE hContext, int *pValue )
{
    *pValue = platform_hal_GetLedPONRegistrationErrorTimer();
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiSetLedPONRegistrationErrorTimer ( ANSC_HANDLE hContext, int value )
{
    return platform_hal_SetLedPONRegistrationErrorTimer(value);
}

ANSC_STATUS CosaDmlGiGetLedOLTProvisioningErrorTimer ( ANSC_HANDLE hContext, int *pValue )
{
    *pValue = platform_hal_GetLedOLTProvisioningErrorTimer();
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiSetLedOLTProvisioningErrorTimer ( ANSC_HANDLE hContext, int value )
{
    return platform_hal_SetLedOLTProvisioningErrorTimer(value);
}

ANSC_STATUS CosaDmlGiGetLedWanDhcpErrorTimer ( ANSC_HANDLE hContext, int *pValue )
{
    *pValue = platform_hal_GetLedWanDhcpErrorTimer();
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGiSetLedWanDhcpErrorTimer ( ANSC_HANDLE hContext, int value )
{
    return platform_hal_SetLedWanDhcpErrorTimer(value);
}

ULONG CosaDmlGiGetLocalUIonStaticIPEnabled ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[8];
    syscfg_get(NULL, "static_ui_enable", buf, sizeof(buf));
    *pValue = (strcmp(buf, "true") == 0);
    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlGiSetLocalUIonStaticIPEnabled ( ANSC_HANDLE hContext, BOOL bValue )
{
    syscfg_set(NULL, "static_ui_enable", bValue ? "true" : "false");
    v_secure_system("sysevent set firewall-restart");
    return ANSC_STATUS_SUCCESS;
}
