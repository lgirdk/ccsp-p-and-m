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

#include "cosa_lgi_wol_internal.h"
#include "cosa_lgi_wol_apis.h"

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiWoLCreate
            (
            );

    description:

        This function constructs cosa WoL object and return handle.

    argument:

    return:     newly created WoL object.

**********************************************************************/

ANSC_HANDLE
CosaLgiWoLCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_LGI_WOL  pMyObject    = (PCOSA_DATAMODEL_LGI_WOL)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_WOL)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_WOL));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_WOL_OID;
    pMyObject->Create            = CosaLgiWoLCreate;
    pMyObject->Remove            = CosaLgiWoLRemove;
    pMyObject->Initialize        = CosaLgiWoLInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiWoLInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa WoL object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiWoLInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_WOL            pMyObject    = (PCOSA_DATAMODEL_LGI_WOL)hThisObject;

    /* Initiation all functions */
    initMac();
    CosaDmlGetInterval(NULL, &pMyObject->Interval);
    CosaDmlGetRetries(NULL, &pMyObject->Retries);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiWoLRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa WoL object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiWoLRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_WOL            pMyObject    = (PCOSA_DATAMODEL_LGI_WOL)hThisObject;

    /* Remove necessary resources */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}
