/*********************************************************************************
 * Copyright 2023 Liberty Global B.V.
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
#ifdef FEATURE_STATIC_IPV4

#include "cosa_lgi_staticip_apis.h"
#include "cosa_lgi_staticip_internal.h"

ANSC_HANDLE CosaLgiStaticIPCreate (void)
{
    PCOSA_DATAMODEL_LGI_STATICIP  pMyObject = (PCOSA_DATAMODEL_LGI_STATICIP)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_STATICIP)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_STATICIP));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_STATICIP_OID;
    pMyObject->Create            = CosaLgiStaticIPCreate;
    pMyObject->Remove            = CosaLgiStaticIPRemove;
    pMyObject->Initialize        = CosaLgiStaticIPInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

ANSC_STATUS CosaLgiStaticIPInitialize ( ANSC_HANDLE hThisObject )
{

    PCOSA_DATAMODEL_LGI_STATICIP pMyObject = (PCOSA_DATAMODEL_LGI_STATICIP)hThisObject;

    CosaDmlStaticIPGetAdministrativeStatus(NULL, &pMyObject->AdministrativeStatus);
  
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaLgiStaticIPRemove ( ANSC_HANDLE hThisObject )
{
    PCOSA_DATAMODEL_LGI_STATICIP pMyObject = (PCOSA_DATAMODEL_LGI_STATICIP)hThisObject;

    /* Remove necessary resource */
    
    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return ANSC_STATUS_SUCCESS;
}

#endif
