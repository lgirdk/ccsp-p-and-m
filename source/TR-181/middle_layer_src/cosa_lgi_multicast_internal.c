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

#include <syscfg/syscfg.h>
#include "cosa_lgi_multicast_internal.h" 

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiMulticastCreate
            (
            );

    description:

        This function constructs cosa LgiMulticast object and return handle.

    argument:

    return:     newly created LgiMulticast object.

**********************************************************************/

ANSC_HANDLE
CosaLgiMulticastCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_LGI_MULTICAST  pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_MULTICAST));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_MULTICAST_OID;
    pMyObject->Create            = CosaLgiMulticastCreate;
    pMyObject->Remove            = CosaLgiMulticastRemove;
    pMyObject->Initialize        = CosaLgiMulticastInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiMulticastInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiMulticast object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiMulticastInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{

    ANSC_STATUS                   returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST)hThisObject;

    CosaDmlMulticastGetEnable(NULL, &pMyObject->Enable);

    CosaDmlMulticastGetSnoopingEnable(NULL, &pMyObject->SnoopingEnable);

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiMulticastRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function remove cosa LgiMulticast object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiMulticastRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                   returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST)hThisObject;

    /* Remove necessary resource */
    

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}
