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

#include "cosa_lgi_plume_internal.h"
#include "cosa_lgi_plume_apis.h"

//#define URL_LEN 256

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiPlumeCreate
            (
            );

    description:

        This function constructs cosaLgiPlume object and return handle.

    argument:

    return:     newly created LgiPlume object.

**********************************************************************/

ANSC_HANDLE
CosaLgiPlumeCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_LGI_PLUME  pMyObject    = (PCOSA_DATAMODEL_LGI_PLUME)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_PLUME)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_PLUME));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_PLUME_OID;
    pMyObject->Create            = CosaLgiPlumeCreate;
    pMyObject->Remove            = CosaLgiPlumeRemove;
    pMyObject->Initialize        = CosaLgiPlumeInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiPlumeInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiPlume object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiPlumeInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_PLUME       pMyObject    = (PCOSA_DATAMODEL_LGI_PLUME)hThisObject;
    ULONG ulSize = URL_LEN;

    /* Initiation all functions */
    CosaDmlGetPlumeUrl(NULL, pMyObject->plumeUrl, &ulSize);
    CosaDmlGetPlumeAdminStatus(NULL, &pMyObject->plumeAdminStatus);
    CosaDmlGetPlumeOperationalStatus(NULL, &pMyObject->plumeOperationalStatus);
    CosaDmlGetPlumeDFSEnable(NULL, &pMyObject->plumeDFSEnable);
    CosaDmlGetPlumeNativeAtmBsControl(NULL, &pMyObject->plumeNativeAtmBsControl);
    CosaDmlGetPlumeLogpullEnable(NULL, &pMyObject->plumeLogpullEnable);
    pMyObject->bNeedPlumeServiceRestart = FALSE;
    pMyObject->plumeAdminStatusChanged = FALSE;
    pMyObject->plumeOperationalStatusChanged = FALSE;
    pMyObject->bPlumeUrlChanged = FALSE;
    pMyObject->bPlumeNativeAtmBsControlChanged = FALSE;

#ifdef _PUMA6_ARM_
    {
        char rpc_cmd[80];

        /* Initialize the DFS enabled status on ATOM */
        snprintf(rpc_cmd, sizeof(rpc_cmd), "rpcclient2 'echo %s > /tmp/.syscfg_son_dfs_enable'", pMyObject->plumeDFSEnable ? "1" : "0");
        system(rpc_cmd);

        /* Initialize the operational status on ATOM */
        snprintf(rpc_cmd, sizeof(rpc_cmd), "rpcclient2 'echo %s > /tmp/.syscfg_son_operational_status'", pMyObject->plumeOperationalStatus ? "1" : "0");
        system(rpc_cmd);

        /* Initialize the admin status on ATOM */
        snprintf(rpc_cmd, sizeof(rpc_cmd), "rpcclient2 'echo %s > /tmp/.syscfg_son_admin_status'", pMyObject->plumeAdminStatus ? "1" : "0");
        system(rpc_cmd);

        if (pMyObject->plumeLogpullEnable)
        {
            // Start icu if not already running
            system("pidof -x /usr/sbin/icu > /dev/null || nice -n 10 /usr/sbin/icu -R -B -p 192.168.254.253:2222");
        }
    }
#endif

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiPlumeRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiPlumeRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_PLUME       pMyObject    = (PCOSA_DATAMODEL_LGI_PLUME)hThisObject;

    /* Remove necessary resources */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}

