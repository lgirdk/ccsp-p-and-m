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

#ifndef  _COSA_LGI_CLOUDUI_INTERNAL_H
#define  _COSA_LGI_CLOUDUI_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"

#define  COSA_DATAMODEL_LGI_CLOUDUI_CLASS_CONTENT      \
    /* duplication of the base object class content */ \
    COSA_BASE_CONTENT                                  \

typedef  struct
_COSA_DATAMODEL_LGI_CLOUDUI_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_CLOUDUI_CLASS_CONTENT
    /* start of LGI CloudUi object class content */
    BOOL hideDhcpLanChange;
}
COSA_DATAMODEL_LGI_CLOUDUI, *PCOSA_DATAMODEL_LGI_CLOUDUI;

/*
    Standard function declaration
*/
ANSC_HANDLE
CosaLgiCloudUiCreate
    (
        VOID
    );

ANSC_STATUS
CosaLgiCloudUiInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaLgiCloudUiRemove
    (
        ANSC_HANDLE                 hThisObject
    );

#endif
