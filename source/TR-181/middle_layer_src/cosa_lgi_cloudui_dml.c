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

#include "cosa_lgi_cloudui_dml.h"
#include "cosa_lgi_cloudui_apis.h"
#include "cosa_lgi_cloudui_internal.h"

/***********************************************************************
 *
 *  APIs for Object:
 *
 *      X_LGI-COM_CloudUI.
 *
 *          *  LgiCloudUi_GetParamBoolValue
 *          *  LgiCloudUi_SetParamBoolValue
 *          *  LgiCloudUi_Validate
 *          *  LgiCloudUi_Commit
 *          *  LgiCloudUi_Rollback
 *
 ************************************************************************/

BOOL
LgiCloudUi_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "HideCustomerDhcpLanChange") == 0)
    {
        CosaDmlGetDhcpLanChangeHide(NULL, pBool);
        return TRUE;
    }

    return FALSE;
}

BOOL
LgiCloudUi_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_LGI_CLOUDUI  pMyObject = (PCOSA_DATAMODEL_LGI_CLOUDUI)g_pCosaBEManager->hLgiCloudUi;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "HideCustomerDhcpLanChange") == 0)
    {
        pMyObject->hideDhcpLanChange = bValue;
        return TRUE;
    }

    return FALSE;
}

BOOL
LgiCloudUi_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
  )
{
    return TRUE;
}


ULONG
LgiCloudUi_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_CLOUDUI  pMyObject = (PCOSA_DATAMODEL_LGI_CLOUDUI)g_pCosaBEManager->hLgiCloudUi;
    CosaDmlSetDhcpLanChangeHide(NULL, pMyObject->hideDhcpLanChange);
    return 0;
}

ULONG
LgiCloudUi_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_CLOUDUI  pMyObject = (PCOSA_DATAMODEL_LGI_CLOUDUI)g_pCosaBEManager->hLgiCloudUi;
    CosaDmlGetDhcpLanChangeHide(NULL, &pMyObject->hideDhcpLanChange);
    return 0;
}

