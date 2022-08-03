/*****************************************************************************
 * Copyright 2022 Liberty Global B.V.
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
 ****************************************************************************/
#ifdef FEATURE_RDKB_WAN_MANAGER

#ifndef _COSA_NETWORKLOGS_APIS_H_
#define _COSA_NETWORKLOGS_APIS_H_

#include "cosa_networklogs_internal.h"

ANSC_STATUS
CosaDmlGetNetworkLogs
    (
        ANSC_HANDLE                 hContext,
        PULONG                      pulCount,
        PCOSA_DML_NETWORKLOGS_LOG    *ppConf
    );


#endif

#endif