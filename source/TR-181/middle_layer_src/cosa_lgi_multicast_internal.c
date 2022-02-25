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

ANSC_HANDLE CosaLgiMulticastCreate ( VOID )
{
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) NULL;

    pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_MULTICAST));

    if (!pMyObject)
    {
        return NULL;
    }

    pMyObject->Oid               = COSA_DATAMODEL_LGI_MULTICAST_OID;
    pMyObject->Create            = CosaLgiMulticastCreate;
    pMyObject->Remove            = CosaLgiMulticastRemove;
    pMyObject->Initialize        = CosaLgiMulticastInitialize;

    pMyObject->Initialize ((ANSC_HANDLE) pMyObject);

    return (ANSC_HANDLE) pMyObject;
}

ANSC_STATUS CosaLgiMulticastInitialize ( ANSC_HANDLE hThisObject )
{
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) hThisObject;

    CosaDmlMulticastGetEnable(NULL, &pMyObject->Cfg.bEnable);
    CosaDmlMulticastGetSnoopingEnable(NULL, &pMyObject->Cfg.bSnoopingEnable);
    CosaDmlMulticastGetIGMPv3ProxyEnable(NULL, &pMyObject->Cfg.bIGMPv3ProxyEnable);
    CosaDmlMulticastGetMLDv2ProxyEnable(NULL, &pMyObject->Cfg.bMLDv2ProxyEnable);

    CACHE_CFG(pMyObject);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaLgiMulticastRemove ( ANSC_HANDLE hThisObject )
{
    PCOSA_DATAMODEL_LGI_MULTICAST pMyObject = (PCOSA_DATAMODEL_LGI_MULTICAST) hThisObject;

    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return ANSC_STATUS_SUCCESS;
}
