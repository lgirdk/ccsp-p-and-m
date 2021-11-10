/*********************************************************************************
* Copyright 2019 Liberty Global B.V.
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
*********************************************************************************/

#ifndef  _COSA_LGI_MULTICAST_INTERNAL_H
#define  _COSA_LGI_MULTICAST_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_lgi_multicast_apis.h"

#define  COSA_DATAMODEL_LGI_MULTICAST_CLASS_CONTENT    \
    /* duplication of the base object class content */ \
    COSA_BASE_CONTENT                                  \

typedef struct _COSA_DATAMODEL_LGI_MULTICAST_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_MULTICAST_CLASS_CONTENT
    /* start of LgiMulticast object class content */
    BOOL Enable;
    BOOL SnoopingEnable;
}
COSA_DATAMODEL_LGI_MULTICAST, *PCOSA_DATAMODEL_LGI_MULTICAST;

ANSC_HANDLE CosaLgiMulticastCreate ( VOID );
ANSC_STATUS CosaLgiMulticastInitialize ( ANSC_HANDLE hThisObject );
ANSC_STATUS CosaLgiMulticastRemove ( ANSC_HANDLE hThisObject );

#endif
