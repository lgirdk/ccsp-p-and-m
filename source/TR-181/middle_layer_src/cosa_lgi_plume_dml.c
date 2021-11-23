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

#define ATOM_IP_ADDRESS      "192.168.254.254"

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
    if( AnscEqualString(ParamName, "SONAdminStatus", TRUE))
    {
        return CosaDmlGetPlumeAdminStatus(NULL, pValue);
    }

    if( AnscEqualString(ParamName, "SONOperationalStatus", TRUE))
    {
        return CosaDmlGetPlumeOperationalStatus(NULL, pValue);
    }
    if( AnscEqualString(ParamName, "SONDFSEnable", TRUE))
    {
        return CosaDmlGetPlumeDFSEnable(NULL, pValue);
    }
    if( AnscEqualString(ParamName, "NativeAtmBsControl", TRUE))
    {
        return CosaDmlGetPlumeNativeAtmBsControl(NULL, pValue);
    }
    if( AnscEqualString(ParamName, "SONLogpullEnable", TRUE))
    {
        return CosaDmlGetPlumeLogpullEnable(NULL, pValue);
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
    if( AnscEqualString(ParamName, "SONAdminStatus", TRUE))
    {
        if (pMyObject->plumeAdminStatus != value) {
            pMyObject->plumeAdminStatus = value;
            pMyObject->bNeedPlumeServiceRestart = 1;
        }
        return TRUE;
    }

    if( AnscEqualString(ParamName, "SONOperationalStatus", TRUE))
    {
        if (pMyObject->plumeOperationalStatus != value) {
            pMyObject->plumeOperationalStatus = value;
            pMyObject->bNeedPlumeServiceRestart = 1;
        }
        return TRUE;
    }

    if( AnscEqualString(ParamName, "SONDFSEnable", TRUE))
    {
        if (pMyObject->plumeDFSEnable != value) {
            pMyObject->plumeDFSEnable = value;
            pMyObject->bNeedPlumeServiceRestart = 1;
        }
        return TRUE;
    }

    if( AnscEqualString(ParamName, "NativeAtmBsControl", TRUE))
    {
        pMyObject->plumeNativeAtmBsControl = value;
        pMyObject->bPlumeNativeAtmBsControlChanged = 1;
        return TRUE;
    }
    if( AnscEqualString(ParamName, "SONLogpullEnable", TRUE))
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
    if( AnscEqualString(ParamName, "SONURL", TRUE))
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
    if( AnscEqualString(ParamName, "SONURL", TRUE))
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
    CosaDmlSetPlumeUrl(NULL, pMyObject->plumeUrl);
    CosaDmlSetPlumeAdminStatus(NULL, pMyObject->plumeAdminStatus);
    CosaDmlSetPlumeOperationalStatus(NULL, pMyObject->plumeOperationalStatus);
    CosaDmlSetPlumeDFSEnable(NULL, pMyObject->plumeDFSEnable);
    CosaDmlSetPlumeLogpullEnable(NULL, pMyObject->plumeLogpullEnable);
    if (pMyObject->bPlumeNativeAtmBsControlChanged) {
        CosaDmlSetPlumeNativeAtmBsControl(NULL, pMyObject->plumeNativeAtmBsControl);
    }
    if(pMyObject->bNeedPlumeServiceRestart)
    {
        /* Use WiFi lock when running the plume agent to avoid running it when WiFi is down*/
        system("rpcclient " ATOM_IP_ADDRESS " '(flock 200; (exec 200>&-; /etc/plume_init.sh restart;) ) 200> /var/run/lock/resetlock-wlan'");
    }
    if(pMyObject->bPlumeUrlChanged)
    {
        // Send URL change event to RDK Mesh
        char cmd[256] = {0};
        snprintf(cmd, sizeof(cmd), "rpcclient " ATOM_IP_ADDRESS " '/usr/bin/sysevent set mesh_url \"RDK|%s\"'", pMyObject->plumeUrl);
        system(cmd);
    }
    pMyObject->bNeedPlumeServiceRestart = 0;
    pMyObject->bPlumeUrlChanged = 0;
    pMyObject->bPlumeNativeAtmBsControlChanged = 0;
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
    pMyObject->bNeedPlumeServiceRestart = 0;
    pMyObject->bPlumeUrlChanged = 0;
    pMyObject->bPlumeNativeAtmBsControlChanged = 0;
    return 0;
}


