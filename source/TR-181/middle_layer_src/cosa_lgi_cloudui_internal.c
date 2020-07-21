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

#include "cosa_lgi_cloudui_internal.h"
#include "cosa_lgi_cloudui_apis.h"

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiCloudUiCreate
            (
            );

    description:

        This function constructs cosa LgiCloudUi object and return handle.

    argument:

    return:     newly created LgiCloudUi object.

**********************************************************************/

ANSC_HANDLE
CosaLgiCloudUiCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_LGI_CLOUDUI  pMyObject    = (PCOSA_DATAMODEL_LGI_CLOUDUI)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_CLOUDUI)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_CLOUDUI));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_CLOUDUI_OID;
    pMyObject->Create            = CosaLgiCloudUiCreate;
    pMyObject->Remove            = CosaLgiCloudUiRemove;
    pMyObject->Initialize        = CosaLgiCloudUiInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiCloudUiInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiCloudUi object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiCloudUiInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_CLOUDUI        pMyObject    = (PCOSA_DATAMODEL_LGI_CLOUDUI)hThisObject;

    /* Initiation all functions */
    CosaDmlGetDhcpLanChangeHide(NULL, &pMyObject->hideDhcpLanChange);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiCloudUiRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiCloudUi object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiCloudUiRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_CLOUDUI        pMyObject    = (PCOSA_DATAMODEL_LGI_CLOUDUI)hThisObject;

    /* Remove necessary resources */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}
