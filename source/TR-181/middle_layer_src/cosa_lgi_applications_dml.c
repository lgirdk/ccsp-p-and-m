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
#include "cosa_lgi_applications_apis.h"
#include "cosa_lgi_applications_dml.h"
#include "cosa_lgi_applications_internal.h"
#include "ccsp/platform_hal.h"
#include "cosa_drg_common.h"
#include <syscfg/syscfg.h>

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply
 the changes to all of the specified Parameters atomically. That is, either
 all of the value changes are applied together, or none of the changes are
 applied at all. In the latter case, the CPE MUST return a fault response
 indicating the reason for the failure to apply the changes.

 The CPE MUST NOT apply any of the specified changes without applying all
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }

***********************************************************************/


/***********************************************************************

 APIs for Object:

   X_LGI_COM_Applications.Speedtest.SamKnows.

    *  LgiApplications_SamKnows_GetParamBoolValue
    *  LgiApplications_SamKnows_SetParamBoolValue
    *  LgiApplications_SamKnows_GetParamStringValue
    *  LgiApplications_SamKnows_SetParamStringValue
    *  LgiApplications_SamKnows_Validate
    *  LgiApplications_SamKnows_Commit
    *  LgiApplications_SamKnows_Rollback

***********************************************************************/

BOOL LgiApplications_SamKnows_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool )
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        CosaDmlApplicationsSamKnowsGetEnabled(NULL, pBool);
        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

BOOL LgiApplications_SamKnows_SetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue )
{
    PCOSA_DATAMODEL_LGI_APPLICATIONS  pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)g_pCosaBEManager->hLgiApplications;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        pMyObject->SamKnowsEnable = bValue;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

ULONG LgiApplications_SamKnows_GetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pulSize )
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Property") == 0)
    {
        return CosaDmlApplicationsSamKnowsGetProperty(NULL, pValue, pulSize);
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

BOOL LgiApplications_SamKnows_SetParamStringValue ( ANSC_HANDLE hInsContext, char* ParamName, char* strValue )
{
    PCOSA_DATAMODEL_LGI_APPLICATIONS  pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)g_pCosaBEManager->hLgiApplications;

    if (strcmp(ParamName, "Property") == 0)
    {
        AnscCopyString(pMyObject->SamKnowsProperty, strValue);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

BOOL LgiApplications_SamKnows_Validate ( ANSC_HANDLE hInsContext, char* pReturnParamName, ULONG* puLength )
{
    PCOSA_DATAMODEL_LGI_APPLICATIONS  pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)g_pCosaBEManager->hLgiApplications;
    if(CosaDmlApplicationsSamKnowsValidateProperty(NULL, pMyObject->SamKnowsProperty)  == FALSE)
    {
        _ansc_strcpy(pReturnParamName, "Property");
        return FALSE;
    }
    return TRUE;
}

ULONG LgiApplications_SamKnows_Commit ( ANSC_HANDLE hInsContext )
{
    PCOSA_DATAMODEL_LGI_APPLICATIONS  pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)g_pCosaBEManager->hLgiApplications;

    CosaDmlApplicationsSamKnowsSetEnabled(NULL, pMyObject->SamKnowsEnable);
    CosaDmlApplicationsSamKnowsSetProperty(NULL, pMyObject->SamKnowsProperty);
    return 0;
}

ULONG LgiApplications_SamKnows_Rollback ( ANSC_HANDLE hInsContext )
{
    PCOSA_DATAMODEL_LGI_APPLICATIONS  pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)g_pCosaBEManager->hLgiApplications;
    ULONG ulSize = sizeof(pMyObject->SamKnowsProperty);
    CosaDmlApplicationsSamKnowsGetProperty(NULL, pMyObject->SamKnowsProperty, &ulSize);
    return 0;
}
