/*********************************************************************************
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
 *********************************************************************************/

#ifndef  _COSA_LGI_IPERF_APIS_H
#define  _COSA_LGI_IPERF_APIS_H

#include "../middle_layer_src/cosa_apis.h"
#include "cosa_lgi_iperf_internal.h"

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

VOID CosaDmlIperfSetDiagnosticsState(PCOSA_DATAMODEL_LGI_IPERF pObj, const char *state, BOOL external);
ANSC_STATUS CosaDmlDiagnosticsStateChangeAction(PCOSA_DATAMODEL_LGI_IPERF pObj);
ANSC_STATUS CosaDmlCreateIperfThread(PCOSA_DATAMODEL_LGI_IPERF pObj);
ANSC_STATUS CosaDmlDestroyIperfThread(PCOSA_DATAMODEL_LGI_IPERF pObj);
ANSC_STATUS CosaDmlIperfGetConnectionTimeout(ULONG *puValue);
ANSC_STATUS CosaDmlIperfSetConnectionTimeout(PCOSA_DATAMODEL_LGI_IPERF pObj, ULONG uValue);
#endif
