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

#include "cosa_lgi_plume_dml.h"
#include "cosa_lgi_plume_apis.h"
#include "cosa_lgi_plume_internal.h"

/***********************************************************************
 *
 *  APIs for Object:
 *
 *      X_LGI-COM_SON
 *
 *          *  LgiPlume_GetParamBoolValue
 *          *  LgiPlume_SetParamBoolValue
 *          *  LgiPlume_GetParamStringValue
 *          *  LgiPlume_SetParamStringValue
 *          *  LgiPlume_Validate
 *          *  LgiPlume_Commit
 *          *  LgiPlume_Rollback
 *
 ************************************************************************/

BOOL
LgiPlume_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pValue
    )
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "SONAdminStatus") == 0)
    {
        return CosaDmlGetPlumeAdminStatus(NULL, pValue);
    }

    if (strcmp(ParamName, "SONOperationalStatus") == 0)
    {
        return CosaDmlGetPlumeOperationalStatus(NULL, pValue);
    }
    if (strcmp(ParamName, "SONDFSEnable") == 0)
    {
        return CosaDmlGetPlumeDFSEnable(NULL, pValue);
    }
    if (strcmp(ParamName, "NativeAtmBsControl") == 0)
    {
        return CosaDmlGetPlumeNativeAtmBsControl(NULL, pValue);
    }
    if (strcmp(ParamName, "SONLogpullEnable") == 0)
    {
        return CosaDmlGetPlumeLogpullEnable(NULL, pValue);
    }
    if (strcmp(ParamName, "SONMeshEnabled") == 0)
    {
        return CosaDmlGetPlumeMeshEnabled(NULL, pValue);
    }

    return -1;
}

BOOL
LgiPlume_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        value
    )
{
    PCOSA_DATAMODEL_LGI_PLUME  pMyObject = (PCOSA_DATAMODEL_LGI_PLUME)g_pCosaBEManager->hLgiPlume;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "SONAdminStatus") == 0)
    {
        if (pMyObject->plumeAdminStatus != value) {
            pMyObject->plumeAdminStatus = value;
            pMyObject->plumeAdminStatusChanged = TRUE;
            pMyObject->bNeedPlumeServiceRestart = 1;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "SONOperationalStatus") == 0)
    {
        if (pMyObject->plumeOperationalStatus != value) {
            pMyObject->plumeOperationalStatus = value;
            pMyObject->plumeOperationalStatusChanged = TRUE;
            pMyObject->bNeedPlumeServiceRestart = 1;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "SONDFSEnable") == 0)
    {
        if (pMyObject->plumeDFSEnable != value) {
            pMyObject->plumeDFSEnable = value;
            pMyObject->bNeedPlumeServiceRestart = 1;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "NativeAtmBsControl") == 0)
    {
        pMyObject->plumeNativeAtmBsControl = value;
        pMyObject->bPlumeNativeAtmBsControlChanged = TRUE;
        return TRUE;
    }
    if (strcmp(ParamName, "SONLogpullEnable") == 0)
    {
        pMyObject->plumeLogpullEnable = value;
        return TRUE;
    }

    return FALSE;
}

ULONG
LgiPlume_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "SONURL") == 0)
    {
        return CosaDmlGetPlumeUrl(NULL, pValue, pulSize);
    }

    return -1;
}

BOOL
LgiPlume_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue
    )
{
    PCOSA_DATAMODEL_LGI_PLUME  pMyObject = (PCOSA_DATAMODEL_LGI_PLUME)g_pCosaBEManager->hLgiPlume;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "SONURL") == 0)
    {
        if(0 != strcmp(pMyObject->plumeUrl, pValue))
        {
            pMyObject->bPlumeUrlChanged = 1;
            /* workaround to avoid plume getting stuck with a wrong URL */
            pMyObject->bNeedPlumeServiceRestart = 1;
        }
        _ansc_snprintf(pMyObject->plumeUrl, sizeof(pMyObject->plumeUrl), "%s", pValue);
        return TRUE;
    }

    return FALSE;
}

BOOL
LgiPlume_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
  )
{
    return TRUE;
}


ULONG
LgiPlume_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_PLUME  pMyObject = (PCOSA_DATAMODEL_LGI_PLUME)g_pCosaBEManager->hLgiPlume;
    PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths;

    pWiFiDataPaths = (PCOSA_LGI_PLUME_DATAPATHS)AnscAllocateMemory(sizeof(*pWiFiDataPaths));
    if (pWiFiDataPaths == NULL) {
        CcspTraceError(("Failed to allocate WiFi data paths\n"));
        return ANSC_STATUS_FAILURE;
    }

    CosaDmlSetPlumeUrl(NULL, pMyObject->plumeUrl);
    CosaDmlSetPlumeAdminStatus(NULL, pMyObject->plumeAdminStatus);
    CosaDmlSetPlumeOperationalStatus(NULL, pMyObject->plumeOperationalStatus);
    CosaDmlSetPlumeDFSEnable(NULL, pMyObject->plumeDFSEnable);
    CosaDmlSetPlumeLogpullEnable(NULL, pMyObject->plumeLogpullEnable);
    if (pMyObject->bPlumeNativeAtmBsControlChanged) {
        CosaDmlSetPlumeNativeAtmBsControl((PANSC_HANDLE)pWiFiDataPaths, pMyObject->plumeNativeAtmBsControl);
    }
    /* Only handle disable case as the enable case is handled by the OpenSync */
    if ((pMyObject->plumeOperationalStatusChanged && pMyObject->plumeOperationalStatus == FALSE) ||
        (pMyObject->plumeAdminStatusChanged && pMyObject->plumeAdminStatus == FALSE))
    {
        CosaDmlSetPlumeBackhaulSSIDsState((PANSC_HANDLE)pWiFiDataPaths, FALSE);
        CosaDmlSetRRMState((PANSC_HANDLE)pWiFiDataPaths, FALSE);
    }

    if(pMyObject->bNeedPlumeServiceRestart)
    {
#ifdef _PUMA6_ARM_
        /* Use WiFi lock when running the plume agent to avoid running it when WiFi is down*/
        system("rpcclient2 '(flock 200; (exec 200>&-; /etc/plume_init.sh restart;) ) 200> /var/run/lock/resetlock-wlan'");
#else
        /* Use WiFi lock when running the plume agent to avoid running it when WiFi is down*/
        system("(flock 200; (exec 200>&-; /etc/plume_init.sh restart;) ) 200> /var/run/lock/resetlock-wlan");
#endif
    }
    if(pMyObject->bPlumeUrlChanged)
    {
        // Send URL change event to RDK Mesh
        char cmd[256];
#ifdef _PUMA6_ARM_
        snprintf(cmd, sizeof(cmd), "rpcclient2 '/usr/bin/sysevent set mesh_url \"RDK|%s\"'", pMyObject->plumeUrl);
#else
        snprintf(cmd, sizeof(cmd), "/usr/bin/sysevent set mesh_url \"RDK|%s\"", pMyObject->plumeUrl);
#endif
        system(cmd);
    }

    CosaDmlApplyPlumeWiFiChanges((PANSC_HANDLE)pWiFiDataPaths, pWiFiDataPaths->applyToRadio > 0);

    pMyObject->bNeedPlumeServiceRestart = FALSE;
    pMyObject->plumeAdminStatusChanged = FALSE;
    pMyObject->plumeOperationalStatusChanged = FALSE;
    pMyObject->bPlumeUrlChanged = FALSE;
    pMyObject->bPlumeNativeAtmBsControlChanged = FALSE;
    return 0;
}

ULONG
LgiPlume_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_PLUME  pMyObject = (PCOSA_DATAMODEL_LGI_PLUME)g_pCosaBEManager->hLgiPlume;
    ULONG ulSize = URL_LEN;
    CosaDmlGetPlumeUrl(NULL, pMyObject->plumeUrl, &ulSize);
    CosaDmlGetPlumeAdminStatus(NULL, &pMyObject->plumeAdminStatus);
    CosaDmlGetPlumeOperationalStatus(NULL, &pMyObject->plumeOperationalStatus);
    CosaDmlGetPlumeDFSEnable(NULL, &pMyObject->plumeDFSEnable);
    CosaDmlGetPlumeNativeAtmBsControl(NULL, &pMyObject->plumeNativeAtmBsControl);
    pMyObject->bNeedPlumeServiceRestart = FALSE;
    pMyObject->plumeAdminStatusChanged = FALSE;
    pMyObject->plumeOperationalStatusChanged = FALSE;
    pMyObject->bPlumeUrlChanged = FALSE;
    pMyObject->bPlumeNativeAtmBsControlChanged = FALSE;
    return 0;
}


