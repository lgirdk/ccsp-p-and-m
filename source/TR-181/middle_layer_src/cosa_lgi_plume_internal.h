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

#ifndef  _COSA_LGI_PLUME_INTERNAL_H
#define  _COSA_LGI_PLUME_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"

#define  COSA_DATAMODEL_LGI_PLUME_CLASS_CONTENT      \
    /* duplication of the base object class content */ \
    COSA_BASE_CONTENT                                  \

#define URL_LEN 128

typedef  struct
_COSA_DATAMODEL_LGI_PLUME_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_PLUME_CLASS_CONTENT
    /* start of LGI CloudUi object class content */
    BOOL plumeAdminStatus;
    BOOL plumeOperationalStatus;
    char plumeUrl[URL_LEN];
    BOOL plumeDFSEnable;
    BOOL plumeNativeAtmBsControl;
    BOOL bNeedPlumeServiceRestart;
    BOOL bPlumeUrlChanged;
    BOOL bPlumeNativeAtmBsControlChanged;
    BOOL plumeLogpullEnable;
}
COSA_DATAMODEL_LGI_PLUME, *PCOSA_DATAMODEL_LGI_PLUME;

/*
    Standard function declaration
*/
ANSC_HANDLE
CosaLgiPlumeCreate
    (
        VOID
    );

ANSC_STATUS
CosaLgiPlumeInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaLgiPlumeRemove
    (
        ANSC_HANDLE                 hThisObject
    );

#endif

