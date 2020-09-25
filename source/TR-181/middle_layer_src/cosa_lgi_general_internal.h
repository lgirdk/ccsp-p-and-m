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

#ifndef  _COSA_ARRIS_GENERAL_INTERNAL_H
#define  _COSA_ARRIS_GENERAL_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_lgi_general_apis.h"

#define  COSA_DATAMODEL_LGI_GENERAL_CLASS_CONTENT    \
    /* duplication of the base object class content */ \
    COSA_BASE_CONTENT                                  \

typedef  struct
_COSA_DATAMODEL_LGI_GENERAL_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_GENERAL_CLASS_CONTENT
    /* start of LgiGeneral object class content */
    ULONG                           CustomerId;
    char                            CurrentLanguage[6];
    char                            CAppName[128];
    char                            WebsiteHelpURL[128];
    BOOL                            FirstInstallState;
    BOOL                            TroubleshootWizardEnable;
    char                            DataModelVersion[32];
    BOOL                            FirstInstallWizardEnable;
}
COSA_DATAMODEL_LGI_GENERAL, *PCOSA_DATAMODEL_LGI_GENERAL;

/*
    Standard function declaration
*/
ANSC_HANDLE
CosaLgiGeneralCreate
    (
        VOID
    );

ANSC_STATUS
CosaLgiGeneralInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaLgiGeneralRemove
    (
        ANSC_HANDLE                 hThisObject
    );

#endif
