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

#ifndef  _COSA_LGI_PLUME_APIS_H
#define  _COSA_LGI_PLUME_APIS_H

#include "../middle_layer_src/cosa_apis.h"

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/
ANSC_STATUS
CosaDmlGetPlumeUrl
(
    ANSC_HANDLE                 hContext,
    char                        *pValue,
    ULONG                       *pUlSize
);

ULONG
CosaDmlSetPlumeUrl
(
    ANSC_HANDLE                 hContext,
    char                        *pValue
);

BOOL
CosaDmlGetPlumeAdminStatus
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);

BOOL
CosaDmlSetPlumeAdminStatus
(
    ANSC_HANDLE                 hContext,
    BOOL                        value
);

BOOL
CosaDmlGetPlumeOperationalStatus
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);

BOOL
CosaDmlSetPlumeOperationalStatus
(
    ANSC_HANDLE                 hContext,
    BOOL                        value
);
BOOL
CosaDmlGetPlumeDFSEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);
BOOL
CosaDmlSetPlumeDFSEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        value
);
BOOL
CosaDmlGetPlumeNativeAtmBsControl
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);
BOOL
CosaDmlSetPlumeNativeAtmBsControl
(
    ANSC_HANDLE                 hContext,
    BOOL                        value
);
BOOL
CosaDmlGetPlumeLogpullEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        *pValue
);
BOOL
CosaDmlSetPlumeLogpullEnable
(
    ANSC_HANDLE                 hContext,
    BOOL                        value
);
#endif
