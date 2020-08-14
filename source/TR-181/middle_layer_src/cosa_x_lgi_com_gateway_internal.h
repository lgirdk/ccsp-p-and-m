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

#ifndef  _COSA_LGI_IPV6LAN_MODE_INTERNAL_H
#define  _COSA_LGI_IPV6LAN_MODE_INTERNAL_H 

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_x_lgi_com_gateway_apis.h"

#define  COSA_DATAMODEL_LGI_IPV6LAN_MODE_CLASS_CONTENT    \
    /* duplication of the base object class content */ \
    COSA_BASE_CONTENT                                  \

typedef  struct
_COSA_DATAMODEL_LGI_IPV6LAN_MODE_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_IPV6LAN_MODE_CLASS_CONTENT
    /* start of LGI Gateway object class content */
    ULONG                           ipv6LanMode;
}
COSA_DATAMODEL_LGI_IPV6LAN_MODE, *PCOSA_DATAMODEL_LGI_IPV6LAN_MODE;

/*
    Standard function declaration
*/
ANSC_HANDLE
CosaLgiIPv6LANModeCreate
    (
        VOID
    );

ANSC_STATUS
CosaLgiIPv6LANModeInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaLgiIPv6LANModeRemove
    (
        ANSC_HANDLE                 hThisObject
    );

#endif