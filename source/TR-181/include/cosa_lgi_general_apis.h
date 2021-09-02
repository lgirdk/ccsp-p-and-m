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

#ifndef  _COSA_LGI_GENERAL_APIS_H
#define  _COSA_LGI_GENERAL_APIS_H

#include "../middle_layer_src/cosa_apis.h"

#define CUSTOMER_SYSCFG_FILE "/etc/utopia/defaults/lg_syscfg_cust_%d.db"

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/
ULONG
CosaDmlGiGetFirstInstallWizardEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);

ULONG
CosaDmlGiSetFirstInstallWizardEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        bValue
);

ANSC_STATUS
CosaDmlGiGetCustomerId
(
    ANSC_HANDLE                 hContext,
    ULONG                       *pValue
);

ULONG
CosaDmlGiSetCustomerId
(
    ANSC_HANDLE                 hContext,
    ULONG                       ulValue
);

ANSC_STATUS
CosaDmlGiGetCAppName
(
    ANSC_HANDLE                 hContext,
    char                        *pValue,
    size_t                      len
);

ANSC_STATUS
CosaDmlGiGetWebsiteHelpURL
(
    ANSC_HANDLE                 hContext,
    char                        *pValue,
    size_t                      len
);

ANSC_STATUS
CosaDmlGiGetFirstInstallState
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);

ANSC_STATUS
CosaDmlGiGetTroubleshootWizardEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);

ANSC_STATUS
CosaDmlGiSetCAppName
(
    ANSC_HANDLE                 hContext,
    char                        *pValue
);

ANSC_STATUS
CosaDmlGiSetWebsiteHelpURL
(
    ANSC_HANDLE                 hContext,
    char                        *pValue
);

ANSC_STATUS
CosaDmlGiSetFirstInstallState
(
    ANSC_HANDLE                 hContext,
    BOOL                        bValue
);

ANSC_STATUS
CosaDmlGiSetTroubleshootWizardEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        bValue
);

ANSC_STATUS
CosaDmlGiGetCurrentLanguage
(
    ANSC_HANDLE                 hContext,
    char                        *pValue,
    size_t                      len
);

ULONG
CosaDmlGiGetWebUISkin
(
    ANSC_HANDLE                 hContext,
    char                        *pValue,
    ULONG                       *pUlSize
);

ULONG
CosaDmlGiSetWebUISkin
(
    ANSC_HANDLE                 hContext,
    char                        *pValue
);

ULONG
CosaDmlLGiSetUiHashPassword
(
    char                        *pValue
);

ULONG
CosaDmlGiGetAvailableLanguages
(
    ANSC_HANDLE                 hContext,
    char                        *pValue,
    ULONG                       *pUlSize
);

ULONG
CosaDmlGiSetCurrentLanguage
(   
    ANSC_HANDLE                 hContext,
    char                        *pValue
);

ULONG
CosaDmlGiGetDefaultAdminPassword
(
    ANSC_HANDLE                 hContext,
    char                        *pValue,
    ULONG                       *pUlSize
);

ULONG
CosaDmlGiGetUserBridgeModeAllowed
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);

ULONG
CosaDmlGiSetUserBridgeModeAllowed
(
    ANSC_HANDLE                 hContext,
    BOOL                        bValue
);

ANSC_STATUS CosaDmlGiGetSKU (ANSC_HANDLE hContext, char *pValue, ULONG ulSize);

ANSC_STATUS CosaDmlGiGetLedDSErrorTimer (ANSC_HANDLE hContext, int *pValue);
ANSC_STATUS CosaDmlGiSetLedDSErrorTimer (ANSC_HANDLE hContext, int value);
ANSC_STATUS CosaDmlGiGetLedUSErrorTimer (ANSC_HANDLE hContext, int *pValue);
ANSC_STATUS CosaDmlGiSetLedUSErrorTimer (ANSC_HANDLE hContext, int value);
ANSC_STATUS CosaDmlGiGetLedRegistrationErrorTimer (ANSC_HANDLE hContext, int *pValue);
ANSC_STATUS CosaDmlGiSetLedRegistrationErrorTimer (ANSC_HANDLE hContext, int value);

#endif
