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

#include "cosa_lgi_general_apis.h"
#include <syscfg/syscfg.h>

ULONG
CosaDmlGiGetFirstInstallWizardEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[64];
    memset(buf, 0, sizeof(buf));
    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "FirstInstallWizard_Enable", buf, sizeof(buf));
    }
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
    if(syscfg_init() == 0)
    {
        syscfg_set(NULL, "FirstInstallWizard_Enable", bValue ? "true" : "false");
    }
    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiSaveSettings()
{
    if(syscfg_init() == 0)
    {
        return syscfg_commit();
    }
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlGiGetCAppName
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    char buf[128];
    memset(buf, 0, sizeof(buf));
    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "CApp_Name", buf, sizeof(buf));
    }
    if( buf[0] != '\0')
       AnscCopyString(pValue, buf);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetWebsiteHelpURL
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    char buf[128];
    memset(buf, 0, sizeof(buf));
    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "Website_HelpURL", buf, sizeof(buf));
    }
    if( buf[0] != '\0')
       AnscCopyString(pValue, buf);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetFirstInstallState
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    )
{
    char buf[64];
    char first_install_enable_buf[64];
    int islgi_customer = 0;
    memset(buf, 0, sizeof(buf));
    if(syscfg_init() == 0)
    {
       syscfg_get( NULL, "FirstInstall_State", buf, sizeof(buf));
    }
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
    char buf[64];
    memset(buf, 0, sizeof(buf));
    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "troubleshoot_wizard_enable", buf, sizeof(buf));
    }
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
      //FIXME : it is just a palce holder for customer index retrival 
      return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGiSetCustomerId
(
    ANSC_HANDLE                 hContext,
    ULONG                       ulValue
)
{
    
     //FIXME : it is just a palce holder for customer index set logic  
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetCAppName
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    if(syscfg_init() == 0)
    {
        syscfg_set(NULL, "CApp_Name", pValue);
    }
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetWebsiteHelpURL
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    if(syscfg_init() == 0)
    {
        syscfg_set(NULL, "Website_HelpURL", pValue);
    }
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetFirstInstallState
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    if(syscfg_init() == 0)
    {
        syscfg_set(NULL, "FirstInstall_State", bValue ? "true" : "false");
    }
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiSetTroubleshootWizardEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    )
{
    if(syscfg_init() == 0)
    {
        syscfg_set(NULL, "troubleshoot_wizard_enable", bValue ? "1" : "0");
    }
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGiGetCurrentLanguage
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    char buf[8];
    memset(buf, 0, sizeof(buf));
    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "Current_Language", buf, sizeof(buf));
    }
    AnscCopyString(pValue, buf);
    return ANSC_STATUS_SUCCESS;
}

#define MAX_HOSTNAME_SIZE 128
ULONG
CosaDmlGiGetLanHostname
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue,
        ULONG                       *pUlSize
    )
{
    char hostname[MAX_HOSTNAME_SIZE];
    memset(hostname, 0, sizeof(hostname));
    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "hostname", hostname, sizeof(hostname));
        if(AnscSizeOfString(hostname) < *pUlSize)
        {
            AnscCopyString(pValue, hostname);
            return 0;
        }
        else
        {
            *pUlSize = AnscSizeOfString(hostname);
            return 1;
        }
    }
    return -1;
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

    memset(buf, 0, sizeof(buf));
    if(syscfg_init() == 0)
    {
        syscfg_get( NULL, "Available_Languages", buf, sizeof(buf));
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
    return -1;
}

ULONG
CosaDmlGiSetCurrentLanguage
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    if(syscfg_init() == 0)
    {
        syscfg_set(NULL, "Current_Language", pValue);
    }
    return ANSC_STATUS_SUCCESS;
}

