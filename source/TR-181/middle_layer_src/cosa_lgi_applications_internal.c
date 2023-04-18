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
#include "cosa_lgi_applications_internal.h"

ANSC_HANDLE CosaLgiApplicationsCreate ( VOID )
{
    PCOSA_DATAMODEL_LGI_APPLICATIONS  pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_APPLICATIONS));

    if ( !pMyObject )
    {
        return  (ANSC_HANDLE)NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid               = COSA_DATAMODEL_LGI_APPLICATIONS_OID;
    pMyObject->Create            = CosaLgiApplicationsCreate;
    pMyObject->Remove            = CosaLgiApplicationsRemove;
    pMyObject->Initialize        = CosaLgiApplicationsInitialize;

    pMyObject->Initialize   ((ANSC_HANDLE)pMyObject);

    return  (ANSC_HANDLE)pMyObject;
}

ANSC_STATUS CosaLgiApplicationsInitialize ( ANSC_HANDLE hThisObject )
{
    ANSC_STATUS                   returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_APPLICATIONS pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)hThisObject;

    CosaDmlApplicationsSamKnowsGetEnabled(NULL, &pMyObject->SamKnowsEnable);

    ULONG propertyStrLen = SAMKNOWS_PROPERTY_STRING_LEN;
    CosaDmlApplicationsSamKnowsGetProperty(NULL, pMyObject->SamKnowsProperty, &propertyStrLen);
#ifdef _PUMA6_ARM_
    v_secure_system("rpcclient2 'echo \"%s\" > /tmp/sk_property.txt &'", pMyObject->SamKnowsProperty);
#endif

#if !defined (FEATURE_GPON)
    /*
       Start SK here for DOCSIS platforms only (it's too early for non-DOCSIS).
       Note also that unitid path may change depending on SK release. Here we only
       support the older path, needed for the older SK releases which are used
       on DOCSIS platforms. The newer path added for reference but commented out.
    */
    if ((pMyObject->SamKnowsEnable == TRUE) &&
        (access("/tmp/samknows/unitid", F_OK) != 0) /* &&
        (access("/var/run/opt/samknows/router_agent/unitid", F_OK) != 0) */ )
    {
        CcspTraceInfo(("%s : Starting Samknows\n", __FUNCTION__));	    
        system("/etc/init.d/samknows_ispmon start &");
    }
#endif

    return returnStatus;
}

ANSC_STATUS CosaLgiApplicationsRemove ( ANSC_HANDLE hThisObject )
{
    ANSC_STATUS                   returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_APPLICATIONS pMyObject = (PCOSA_DATAMODEL_LGI_APPLICATIONS)hThisObject;

    /* Remove necessary resource */

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}
