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

#include <ctype.h>
#include "ansc_platform.h"
#include "cosa_lgi_multicast_apis.h"
#include "cosa_lgi_multicast_dml.h"
#include "cosa_lgi_multicast_internal.h"
#include "ccsp/platform_hal.h"
#include "cosa_drg_common.h"
#include <syscfg/syscfg.h>

BOOL LgiMulticast_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool )
{
    if (strcmp (ParamName, "Enable") == 0)
    {
        CosaDmlMulticastGetEnable(NULL, pBool);
        return TRUE;
    }

    if (strcmp(ParamName, "SnoopingEnable") == 0)
    {
        CosaDmlMulticastGetSnoopingEnable(NULL, pBool);
        return TRUE;
    }

    return FALSE;
}
