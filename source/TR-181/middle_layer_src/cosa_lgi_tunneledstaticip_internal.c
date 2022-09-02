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

#include "cosa_lgi_tunneledstaticip_apis.h"
#include "cosa_lgi_tunneledstaticip_internal.h"


ANSC_HANDLE CosaLgiTunneledStaticIPCreate (void)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_TUNNELEDSTATICIP));

    if (!pMyObject)
    {
        return (ANSC_HANDLE) NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid = COSA_DATAMODEL_LGI_TUNNELEDSTATICIP_OID;
    pMyObject->Create = CosaLgiTunneledStaticIPCreate;
    pMyObject->Remove = CosaLgiTunneledStaticIPRemove;
    pMyObject->Initialize = CosaLgiTunneledStaticIPInitialize;

    pMyObject->Initialize ((ANSC_HANDLE) pMyObject);

    return (ANSC_HANDLE) pMyObject;
}

ANSC_STATUS CosaLgiTunneledStaticIPInitialize (ANSC_HANDLE hThisObject)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) hThisObject;

    CosaDmlTunneledStaticIPGetEnable(NULL, &pMyObject->Enable);
    CosaDmlTunneledStaticIPGetUsername(NULL, &pMyObject->Username);
    CosaDmlTunneledStaticIPGetPassword(NULL, &pMyObject->Password);
    CosaDmlTunneledStaticIPGetRadiusInterface(NULL, &pMyObject->RadiusInterface);
    CosaDmlTunneledStaticIPGetRadiusAuthServerIPAddr(NULL, &pMyObject->RadiusAuthServerIPAddr);
    CosaDmlTunneledStaticIPGetRadiusAuthSecret(NULL, &pMyObject->RadiusAuthSecret);
    CosaDmlTunneledStaticIPGetRadiusAuthServerPort(NULL, &pMyObject->RadiusAuthServerPort);
    CosaDmlTunneledStaticIPGetNASIdentifier(NULL, &pMyObject->NASIdentifier);
    CosaDmlTunneledStaticIPGetRadiusAccServerIPAddr(NULL, &pMyObject->RadiusAccServerIPAddr);
    CosaDmlTunneledStaticIPGetRadiusAccSecret(NULL, &pMyObject->RadiusAccSecret);
    CosaDmlTunneledStaticIPGetRadiusAccServerPort(NULL, &pMyObject->RadiusAccServerPort);

    return returnStatus;
}

ANSC_STATUS CosaLgiTunneledStaticIPRemove (ANSC_HANDLE hThisObject)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) hThisObject;

    /* Remove necessary resource */
    
    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}    
