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

#include "cosa_lgi_general_apis.h"

#include <platform_hal.h>

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
    char buf[12];
    char cust_file[45];
    ULONG id = 0;

    CosaDmlGiGetCustomerId(NULL, &id);
    
    /* compare the new value with the current customer ID */
    if (id != ulValue)
    {
        snprintf(cust_file, sizeof(cust_file), CUSTOMER_SYSCFG_FILE, (int) ulValue);

        /* Load customer specific syscfg file to memory */
        if (syscfg_load_from_file(cust_file) == 0)
        {
            /* Set customer-index-changed as true in syscfg. This will be used to
            remove bbhm files from nvram on next boot. */
            if (syscfg_set(NULL, "customer-index-changed", "true") == 0)
            {
                /* If all previous steps are successful then change the Customer_Index in syscfg
                and reboot the box */
                snprintf (buf, sizeof(buf), "%d", (int) ulValue);
                if (syscfg_set (NULL, "Customer_Index", buf) == 0){
                    if (syscfg_commit() == 0){
                        CosaDmlDcSetRebootDevice(NULL, "Device");
                    }
                    else{
                        CcspTraceError(("syscfg_commit Customer_Index failed\n"));
                    }
                }
                else{
                    CcspTraceError(("syscfg_set Customer_Index failed\n"));
                }
            }
            else{
                CcspTraceError(("syscfg_set customer-index-changed failed\n"));
            }
        }
        else{
            CcspTraceError(("Unable to load customer specific file - %s\n", cust_file));
        }
    }
    else{
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
