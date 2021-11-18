/*********************************************************************************
 * Copyright 2021 Liberty Global B.V.
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

#include "cosa_lgi_eventlog_internal.h"

ANSC_HANDLE CosaLgiEventLogCreate ( VOID )
{
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE  pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_EVENTLOGTABLE));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_EVENTLOG_OID;
    pMyObject->Create            = CosaLgiEventLogCreate;
    pMyObject->Remove            = CosaLgiEventLogRemove;
    pMyObject->Initialize        = CosaLgiEventLogInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

ANSC_STATUS CosaLgiEventLogInitialize ( ANSC_HANDLE hThisObject )
{

    ANSC_STATUS                   returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)hThisObject;

    /*Nothing to initialize now, May need later if more logs are added*/
    return returnStatus;
}

ANSC_STATUS CosaLgiEventLogRemove ( ANSC_HANDLE hThisObject )
{
    ANSC_STATUS                   returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_EVENTLOGTABLE pMyObject = (PCOSA_DATAMODEL_LGI_EVENTLOGTABLE)hThisObject;

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}
