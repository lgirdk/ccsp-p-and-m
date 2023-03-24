/*****************************************************************************
 * Copyright 2022 Liberty Global B.V.
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
 ****************************************************************************/
#ifdef FEATURE_NETWORK_LOGS

#include "cosa_networklogs_internal.h"

/**********************************************************************

    Caller:     Owner of the object

    Prototype:

        ANSC_HANDLE
        CosaNetworkLogsCreate
            (
                VOID
            );

    Description:

        This function creates NetworkLogs object and return handle.

    Argument:  

    Return:    created NetworkLogs object.

**********************************************************************/
ANSC_HANDLE
CosaNetworkLogsCreate
    (
        VOID
    )
{
    PCOSA_DML_NETWORKLOGS pMyObject = (PCOSA_DML_NETWORKLOGS)NULL;

    pMyObject = (PCOSA_DML_NETWORKLOGS)AnscAllocateMemory(sizeof(COSA_DML_NETWORKLOGS));

    if(pMyObject == NULL)
    {
        return (ANSC_HANDLE)NULL;
    }

    pMyObject->Oid        = COSA_DATAMODEL_LGI_NETWORKLOGS_OID;
    pMyObject->Create     = CosaNetworkLogsCreate;
    pMyObject->Remove     = CosaNetworkLogsRemove;
    pMyObject->Initialize = CosaNetworkLogsInitialize;

    pMyObject->Initialize((ANSC_HANDLE)pMyObject);

    return (ANSC_HANDLE)pMyObject;     
}

/**********************************************************************

    Caller:     Self

    Prototype:

        ANSC_STATUS
        CosaNetworkLogsInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    Description:

        This function initiate  NetworkLogs object and return status.

    Argument:	ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object itself.

    Return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaNetworkLogsInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    UNREFERENCED_PARAMETER(hThisObject);
    PCOSA_DML_NETWORKLOGS pMyObject = (PCOSA_DML_NETWORKLOGS) hThisObject;
    
    //Initialization of NetworkLogs.Logs. object take place during Logs_Synchronize

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    Caller:     self

    Prototype:

        ANSC_STATUS
        CosaNetworkLogsRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    Description:

        This function free memory of NetworkLogs object and return the status

    Argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object itself.

    Return:     operation status.

**********************************************************************/
ANSC_STATUS
CosaNetworkLogsRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    PCOSA_DML_NETWORKLOGS pMyObject = (PCOSA_DML_NETWORKLOGS)hThisObject;

    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return ANSC_STATUS_SUCCESS;
}

#endif
