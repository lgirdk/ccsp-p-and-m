/*********************************************************************
 * Copyright 2017-2019 ARRIS Enterprises, LLC.
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

#include <syscfg/syscfg.h>
#include "cosa_lgi_general_internal.h"
/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiGeneralCreate
            (
            );

    description:

        This function constructs cosa LgiGeneral object and return handle.

    argument:

    return:     newly created LgiGeneral object.

**********************************************************************/

ANSC_HANDLE
CosaLgiGeneralCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_LGI_GENERAL  pMyObject    = (PCOSA_DATAMODEL_LGI_GENERAL)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_GENERAL)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_GENERAL));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_GENERAL_OID;
    pMyObject->Create            = CosaLgiGeneralCreate;
    pMyObject->Remove            = CosaLgiGeneralRemove;
    pMyObject->Initialize        = CosaLgiGeneralInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiGeneralInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiGeneral object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiGeneralInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_GENERAL      pMyObject    = (PCOSA_DATAMODEL_LGI_GENERAL)hThisObject;

    /* Initiation all functions */

    ULONG size_webuiskin = sizeof(pMyObject->WebUISkin);
    memset(pMyObject->WebUISkin, 0, size_webuiskin);
    CosaDmlGiGetWebUISkin(NULL, pMyObject->WebUISkin, &size_webuiskin);

    CosaDmlGiGetCustomerId(NULL, &pMyObject->CustomerId);

    CosaDmlGiGetFirstInstallWizardEnable(NULL, &pMyObject->FirstInstallWizardEnable);

    memset(pMyObject->CAppName, 0, sizeof(pMyObject->CAppName));
    CosaDmlGiGetCAppName(NULL, pMyObject->CAppName, sizeof(pMyObject->CAppName));

    memset(pMyObject->WebsiteHelpURL, 0, sizeof(pMyObject->WebsiteHelpURL));
    CosaDmlGiGetWebsiteHelpURL(NULL, pMyObject->WebsiteHelpURL, sizeof(pMyObject->WebsiteHelpURL));

    CosaDmlGiGetFirstInstallState(NULL, &pMyObject->FirstInstallState);
    CosaDmlGiGetTroubleshootWizardEnable(NULL, &pMyObject->TroubleshootWizardEnable);

    CosaDmlGiGetCurrentLanguage(NULL, pMyObject->CurrentLanguage, sizeof(pMyObject->CurrentLanguage));

    ULONG size_DefaultAdminPassword = sizeof(pMyObject->DefaultAdminPassword);
    memset(pMyObject->DefaultAdminPassword, 0, size_DefaultAdminPassword);
    CosaDmlGiGetDefaultAdminPassword(NULL, pMyObject->DefaultAdminPassword, &size_DefaultAdminPassword);

    /*Initializing the DataModelVersion with default value 1.01.01.
    Further implementation of this parameter is still under design discussion*/
    AnscCopyString(pMyObject->DataModelVersion, "1.01.01");

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiGeneralRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiGeneral object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiGeneralRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_GENERAL      pMyObject    = (PCOSA_DATAMODEL_LGI_GENERAL)hThisObject;


    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}

