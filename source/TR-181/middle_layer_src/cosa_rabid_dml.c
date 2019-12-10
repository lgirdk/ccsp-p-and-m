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
   
#include "cosa_rabid_dml.h"
#include "cosa_rabid_internal.h"
#include "ansc_platform.h"
#include "syslog.h"
#include "ccsp_trace.h"

#define MIN_RABID_MEMORY_LIMIT 5

static char *g_RabidEnable = "Advsecurity_RabidEnable";

/***********************************************************************

 APIs for Object:

	X_RDKCENTRAL-COM_RabidFramework.

    *  RabidFramework_GetParamBoolValue
    *  RabidFramework_SetParamBoolValue
    *  RabidFramework_GetParamUlongValue
    *  RabidFramework_SetParamUlongValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        RabidFramework_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RabidFramework_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    /* check the parameter name and return the corresponding value */
    PCOSA_DATAMODEL_RABID       pMyObject     = (PCOSA_DATAMODEL_RABID)g_pCosaBEManager->hRabid;

    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        *pBool = pMyObject->bEnable;
        return TRUE;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        RabidFramework_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RabidFramework_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{

    /* check the parameter name and return the corresponding value */
    PCOSA_DATAMODEL_RABID       pMyObject     = (PCOSA_DATAMODEL_RABID)g_pCosaBEManager->hRabid;
    ANSC_STATUS  returnStatus = ANSC_STATUS_SUCCESS;

    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        if(bValue == pMyObject->bEnable)
                return TRUE;
        if( bValue )
                returnStatus = CosaRabidInit(pMyObject);
        else
                returnStatus = CosaRabidDeInit(pMyObject);

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            CcspTraceInfo(("%s EXIT Error\n", __FUNCTION__));
            return  returnStatus;
        }

        return TRUE;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        RabidFramework_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      pUlong
            );

    description:

        This function is called to retrieve unsigned long parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                       pUlong
                The buffer of returned unsigned long value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RabidFramework_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    )
{
    /* check the parameter name and return the corresponding value */
    PCOSA_DATAMODEL_RABID       pMyObject     = (PCOSA_DATAMODEL_RABID)g_pCosaBEManager->hRabid;

    if( AnscEqualString(ParamName, "MemoryLimit", TRUE))
    {
        *pUlong = pMyObject->uMemoryLimit;
        return TRUE;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        RabidFramework_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set unsigned long parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                        uValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RabidFramework_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{

    /* check the parameter name and return the corresponding value */
    PCOSA_DATAMODEL_RABID       pMyObject     = (PCOSA_DATAMODEL_RABID)g_pCosaBEManager->hRabid;
    ANSC_STATUS  returnStatus = ANSC_STATUS_SUCCESS;

    if( AnscEqualString(ParamName, "MemoryLimit", TRUE))
    {
        if(uValue == pMyObject->uMemoryLimit)
                return TRUE;

        if (uValue <= MIN_RABID_MEMORY_LIMIT)
                return FALSE;

        returnStatus = CosaRabidSetMemoryLimit(pMyObject, uValue);

        if ( returnStatus != ANSC_STATUS_SUCCESS )
        {
            CcspTraceInfo(("%s EXIT Error\n", __FUNCTION__));
            return  returnStatus;
        }

        return TRUE;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}

