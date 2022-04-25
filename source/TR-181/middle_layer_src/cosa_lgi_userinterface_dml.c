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

#include "cosa_lgi_userinterface_dml.h"
#include <syscfg/syscfg.h>

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply
 the changes to all of the specified Parameters atomically. That is, either
 all of the value changes are applied together, or none of the changes are
 applied at all. In the latter case, the CPE MUST return a fault response
 indicating the reason for the failure to apply the changes.

 The CPE MUST NOT apply any of the specified changes without applying all
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }

***********************************************************************/
/***********************************************************************

 APIs for Object:

    UserInterface.RemoteAccess.

    *  Std_RemoteAccess_GetParamBoolValue
    *  Std_RemoteAccess_GetParamIntValue
    *  Std_RemoteAccess_GetParamUlongValue
    *  Std_RemoteAccess_GetParamStringValue
    *  Std_RemoteAccess_SetParamBoolValue
    *  Std_RemoteAccess_SetParamIntValue
    *  Std_RemoteAccess_SetParamUlongValue
    *  Std_RemoteAccess_SetParamStringValue
    *  Std_RemoteAccess_Validate
    *  Std_RemoteAccess_Commit
    *  Std_RemoteAccess_Rollback

***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        int
        RemoteAccess_GetHttpWanValue
            (
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve HTTP WAN access parameter
        value from the syscfg db;

    argument:   BOOL*                       pBool
                The buffer of returned boolean value;

    return:     0 if succeeded, -1 if failed.

**********************************************************************/
static int
RemoteAccess_GetHttpWanValue
   (
       BOOL*                       pBool
   )
{
    char query[16] = {0};
    int rc = 0;
    rc = syscfg_get(NULL, "mgmt_wan_access", query, sizeof(query));
    if (rc != 0)
    {
        fprintf(stderr, "Failed to get WAN access status\n");
        return -1;
    }
    *pBool =  atoi(query);
    return 0;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_GetParamBoolValue
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
Std_RemoteAccess_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    /* CosaDmlRaGetCfg(NULL, &pMyObject->RaCfg); */

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        *pBool = pMyObject->StdRaCfg.bEnabled;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Std_RemoteAccess_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Std_RemoteAccess_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    if (strcmp(ParamName, "Port") == 0)
    {
        *puLong = pMyObject->StdRaCfg.Port;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        Std_RemoteAccess_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Std_RemoteAccess_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    if (strcmp(ParamName, "SupportedProtocols") == 0)
    {
        AnscCopyString(pValue, pMyObject->StdRaCfg.SupportedProtocols);
        return 0;
    }

    if (strcmp(ParamName, "Protocol") == 0)
    {
        AnscCopyString(pValue, pMyObject->StdRaCfg.Protocol);
        return 0;
    }

    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_SetParamBoolValue
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
Std_RemoteAccess_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    if (strcmp(ParamName, "Enable") == 0)
    {
        char buffer[5];

        if ((syscfg_get(NULL, "wan_password_set", buffer, sizeof(buffer)) == 0) && (strcmp(buffer, "1") == 0))
        {
            pMyObject->StdRaCfg.bEnabled = bValue;
            return TRUE;
        }

        fprintf(stderr, "%s: Remote UI password not set\n", __FUNCTION__);

        return FALSE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Std_RemoteAccess_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Std_RemoteAccess_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
   PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    if (strcmp(ParamName, "Port") == 0)
    {
        pMyObject->StdRaCfg.Port = uValue;
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Std_RemoteAccess_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    /* check the parameter name and set the corresponding value */
    PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    if (strcmp(ParamName, "Protocol") == 0)
    {
        if ((strcmp(pString, "HTTP") == 0) || (strcmp(pString, "HTTPS") == 0))
        {
            AnscCopyString(pMyObject->StdRaCfg.Protocol, pString);
            return TRUE;
        }
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Std_RemoteAccess_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation.

                ULONG*                      puLength
                The output length of the param name.

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
Std_RemoteAccess_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        Std_RemoteAccess_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Std_RemoteAccess_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    CosaDmlStdRaSetCfg(NULL, &pMyObject->StdRaCfg);

    return 0;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        Std_RemoteAccess_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Std_RemoteAccess_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_USERINTERFACE   pMyObject = (PCOSA_DATAMODEL_USERINTERFACE)g_pCosaBEManager->hUserinterface;

    /* Rollback to the defaults */
    CosaDmlStdRaGetCfg(NULL, &pMyObject->StdRaCfg);
    /* Get the actual RemoteAccess.Enable value from the proc file system */
    BOOL bEnable = FALSE;
    int status = RemoteAccess_GetHttpWanValue(&bEnable);
    /* Update the current Enable value. In case of error retrieving the proc file system value, don't do anything */
    if (status == 0)
    {
        pMyObject->StdRaCfg.bEnabled = bEnable;
        fprintf(stderr, "%s: Rolled back to last Enable value %d!!!!\n", __FUNCTION__, (int)pMyObject->StdRaCfg.bEnabled);
    }

    return 0;
}
