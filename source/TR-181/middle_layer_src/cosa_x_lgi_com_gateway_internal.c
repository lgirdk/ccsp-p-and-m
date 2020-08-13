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


#include "cosa_x_lgi_com_gateway_internal.h"

/**********************************************************************

    caller:     owner of the object

    prototype:

        ANSC_HANDLE
        CosaLgiGatewayCreate
            (
            );

    description:

        This function constructs cosa LgiGateway object and return handle.

    argument:

    return:     newly created LgiGateway object.

**********************************************************************/

ANSC_HANDLE
CosaLgiGatewayCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_LGI_GATEWAY  pMyObject    = (PCOSA_DATAMODEL_LGI_GATEWAY)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_GATEWAY)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_GATEWAY));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_GATEWAY_OID;
    pMyObject->Create            = CosaLgiGatewayCreate;
    pMyObject->Remove            = CosaLgiGatewayRemove;
    pMyObject->Initialize        = CosaLgiGatewayInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiGatewayInitialize
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiGateway object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiGatewayInitialize
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_GATEWAY        pMyObject    = (PCOSA_DATAMODEL_LGI_GATEWAY)hThisObject;
    ULONG                              size;

    /* Initiation all functions */
    CosaDmlLgiGwGetIpv6LanMode(NULL, &pMyObject->ipv6LanMode);

	size = sizeof(pMyObject->dns_ipv4_preferred);
    CosaDmlLgiGwGetDnsIpv4Preferred(pMyObject->dns_ipv4_preferred, &size);
    size = sizeof(pMyObject->dns_ipv4_alternate);
    CosaDmlLgiGwGetDnsIpv4Alternate(pMyObject->dns_ipv4_alternate, &size);
    size = sizeof(pMyObject->dns_ipv6_preferred);
    CosaDmlLgiGwGetDnsIpv6Preferred(pMyObject->dns_ipv6_preferred, &size);
    size = sizeof(pMyObject->dns_ipv6_alternate);
    CosaDmlLgiGwGetDnsIpv6Alternate(pMyObject->dns_ipv6_alternate, &size);
    CosaDmlLgiGwGetDnsOverride(&(pMyObject->dns_override));

    return returnStatus;
}

/**********************************************************************

    caller:     self

    prototype:

        ANSC_STATUS
        CosaLgiGatewayRemove
            (
                ANSC_HANDLE                 hThisObject
            );

    description:

        This function initiate  cosa LgiGateway object and return handle.

    argument:   ANSC_HANDLE                 hThisObject
            This handle is actually the pointer of this object
            itself.

    return:     operation status.

**********************************************************************/

ANSC_STATUS
CosaLgiGatewayRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                        returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_GATEWAY        pMyObject    = (PCOSA_DATAMODEL_LGI_GATEWAY)hThisObject;

    /* Remove necessary resources */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}
