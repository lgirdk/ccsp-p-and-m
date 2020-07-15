/*********************************************************************
 * Copyright 2020 ARRIS Enterprises, LLC.
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

#include "cosa_lgi_wol_dml.h"
#include "cosa_lgi_wol_apis.h"
#include "cosa_lgi_wol_internal.h"

static bool isMACAddressSet = FALSE;

/***********************************************************************
 *
 *  APIs for Object:
 *
 *      X_LGI-COM_WoL.
 *
 *          *  LgiWoL_GetParamUlongValue
 *          *  LgiWoL_SetParamUlongValue
 *          *  LgiWoL_GetParamStringValue
 *          *  LgiWoL_SetParamStringValue
 *          *  LgiWoL_Validate
 *          *  LgiWoL_Commit
 *          *  LgiWoL_Rollback
 *
 ************************************************************************/
ULONG
LgiWoL_GetParamStringValue (ANSC_HANDLE hInsContext,
                                char*       ParamName,
                                char*       pValue,
                                ULONG*      pUlSize)
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "MACAddress") == 0)
    {
        //This parameter defined to always return empty string when read
        AnscCopyString(pValue, "");
        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

BOOL
LgiWoL_SetParamStringValue (ANSC_HANDLE hInsContext,
                                char*       ParamName,
                                char*       pValue)
{
    PCOSA_DATAMODEL_LGI_WOL pWoL = (PCOSA_DATAMODEL_LGI_WOL)g_pCosaBEManager->hLgiWoL;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "MACAddress") == 0) {
       snprintf(pWoL->MACAddress, sizeof(pWoL->MACAddress), "%s", pValue);
       isMACAddressSet = TRUE;
       return TRUE;
    }
    return FALSE;
}

BOOL
LgiWoL_GetParamUlongValue (ANSC_HANDLE hInsContext,
                           char*       ParamName,
                           ULONG*      puLong)
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Interval") == 0) {
        if(ANSC_STATUS_SUCCESS == CosaDmlGetInterval(NULL, puLong)) {
            return TRUE;
        }
    }
    if (strcmp(ParamName, "Retries") == 0) {
        if(ANSC_STATUS_SUCCESS == CosaDmlGetRetries(NULL, puLong)) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL
LgiWoL_SetParamUlongValue (ANSC_HANDLE hInsContext,
                           char*       ParamName,
                           ULONG       uLong)
{
    PCOSA_DATAMODEL_LGI_WOL pWoL = (PCOSA_DATAMODEL_LGI_WOL)g_pCosaBEManager->hLgiWoL;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "Interval") == 0) {
          pWoL->Interval = uLong;
          return TRUE;
    }
    if (strcmp(ParamName, "Retries") == 0) {
          pWoL->Retries = uLong;
          return TRUE;
    }
    return FALSE;
}

BOOL
LgiWoL_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
  )
{
    PCOSA_DATAMODEL_LGI_WOL pWoL = (PCOSA_DATAMODEL_LGI_WOL)g_pCosaBEManager->hLgiWoL;

    if( isMACAddressSet != TRUE )
    {
        return TRUE;
    }
    /* Do parameter validation */
    if (!isMacValid(pWoL->MACAddress))
    {
        return TRUE;
    }

    return FALSE;
}


ULONG
LgiWoL_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_WOL pWoL = (PCOSA_DATAMODEL_LGI_WOL)g_pCosaBEManager->hLgiWoL;

    /* Send WoL packets only if received an SPV of MACAddress */
    if ( isMACAddressSet == TRUE )
    {
        CosaDmlSetMACAddress(NULL,pWoL->MACAddress);
        isMACAddressSet = FALSE;
    }

    CosaDmlSetInterval(NULL,pWoL->Interval);
    CosaDmlSetRetries(NULL,pWoL->Retries);
    return 0;
}

ULONG
LgiWoL_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_WOL pWoL = (PCOSA_DATAMODEL_LGI_WOL)g_pCosaBEManager->hLgiWoL;
    /* isMACAdressSet value is always TRUE in LgiWol_Rollback. Need to reset it to FALSE */
    isMACAddressSet = FALSE;
    CosaDmlGetInterval(NULL, &pWoL->Interval);
    CosaDmlGetRetries(NULL, &pWoL->Retries);

    return 0;
}
