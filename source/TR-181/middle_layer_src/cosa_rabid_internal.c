/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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
*/

/**************************************************************************

    module: cosa_rabid_internal.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implements back-end apis for the COSA Data Model Library

**************************************************************************/

#include "cosa_rabid_internal.h"
#include "cosa_rabid_dml.h"
#include "ccsp_psm_helper.h"
#include "cosa_advsec_utils.h"

extern ANSC_HANDLE bus_handle;
extern char g_Subsystem[32];

static char *g_RabidEnabled = "Advsecurity_RabidEnable";

ANSC_STATUS CosaGetSysCfgUlong(char* setting, ULONG *value);
ANSC_STATUS CosaSetSysCfgUlong(char* setting, ULONG value);
ANSC_STATUS CosaGetSysCfgString(char* setting, char *value, PULONG pulSize);
ANSC_STATUS CosaSetSysCfgString(char* setting, char *pValue);

ANSC_HANDLE
CosaRabidCreate
    (
        VOID
    )
{
    PCOSA_DATAMODEL_RABID       pMyObject    = (PCOSA_DATAMODEL_RABID)NULL;
    ULONG                   syscfgValue = 0;
    int                     retGet  = CCSP_SUCCESS;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_RABID)AnscAllocateMemory(sizeof(COSA_DATAMODEL_RABID));

    if ( !pMyObject )
    {
    	CcspTraceInfo(("%s exit ERROR \n", __FUNCTION__));
        return  (ANSC_HANDLE)NULL;
    }

    retGet = CosaGetSysCfgUlong(g_RabidEnabled, &syscfgValue);

    pMyObject->bEnable = syscfgValue;

    return  (ANSC_HANDLE)pMyObject;
}

ANSC_STATUS
CosaRabidRemove
    (
        ANSC_HANDLE                 hThisObject
    )
{
    ANSC_STATUS                     returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_RABID            pMyObject    = (PCOSA_DATAMODEL_RABID)hThisObject;

    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);
    CcspTraceInfo(("%s EXIT \n", __FUNCTION__));

    return returnStatus;
}

ANSC_STATUS CosaRabidInit(ANSC_HANDLE hThisObject)
{
    ANSC_STATUS                 returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_RABID     pMyObject    = (PCOSA_DATAMODEL_RABID)hThisObject;
    char cmd[128];

    memset(cmd, 0, sizeof(cmd));

    returnStatus = CosaSetSysCfgUlong(g_RabidEnabled, 1);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscCopyString(cmd, "/usr/ccsp/pam/launch_adv_security.sh -rabidOn &");
        system(cmd);
        pMyObject->bEnable = TRUE;
    }
    return returnStatus;
}

ANSC_STATUS CosaRabidDeInit(ANSC_HANDLE hThisObject)
{
    ANSC_STATUS                 returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_RABID     pMyObject    = (PCOSA_DATAMODEL_RABID)hThisObject;
    char cmd[128];

    memset(cmd, 0, sizeof(cmd));

    returnStatus = CosaSetSysCfgUlong(g_RabidEnabled, 0);
    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscCopyString(cmd, "/usr/ccsp/pam/launch_adv_security.sh -rabidOff &");
        system(cmd);
        pMyObject->bEnable = FALSE;
    }
    return returnStatus;
}

