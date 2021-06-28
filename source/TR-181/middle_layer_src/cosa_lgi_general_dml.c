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

#include <unistd.h>

#include "ansc_platform.h"
#include "cosa_lgi_general_apis.h"
#include "cosa_lgi_general_dml.h"
#include "cosa_lgi_general_internal.h"
#include "ccsp/platform_hal.h"
#include <syscfg/syscfg.h>

#define MAX_HOSTNAME_SIZE 64

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

    GeneralInfo.

    *  LgiGeneral_GetParamBoolValue
    *  LgiGeneral_GetParamStringValue

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        LgiGeneral_GetParamBoolValue
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
LgiGeneral_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    BOOL dsliteEnabled;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "FirstInstallWizardEnable") == 0)
    {
        CosaDmlGiGetFirstInstallWizardEnable(NULL, pBool);
        return TRUE;
    }

    if (strcmp(ParamName, "FirstInstallState") == 0)
    {
        CosaDmlGiGetFirstInstallState(NULL, pBool);
        return TRUE;
    }

    if (strcmp(ParamName, "TroubleshootWizardEnable") == 0)
    {
        CosaDmlGiGetTroubleshootWizardEnable(NULL, pBool);
        return TRUE;
    }
    if (strcmp(ParamName, "UserBridgeModeAllowed") == 0)
    {
        CosaDmlGetDsliteEnable(NULL, &dsliteEnabled);
        if (TRUE == dsliteEnabled) {
            *pBool = FALSE;
        } else {
            CosaDmlGiGetUserBridgeModeAllowed(NULL, pBool);
        }
        return TRUE;
    }
    // LGI ADD END

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        LgiGeneral_GetParamUlongValue
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
LgiGeneral_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "CustomerId") == 0)
    {
        CosaDmlGiGetCustomerId(NULL, puLong);
        return TRUE;
    }
    
    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        LgiGeneral_GetParamStringValue
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
LgiGeneral_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{

    PCOSA_DATAMODEL_LGI_GENERAL  pMyObject = (PCOSA_DATAMODEL_LGI_GENERAL)g_pCosaBEManager->hLgiGeneral;

    if (strcmp(ParamName, "AvailableLanguages") == 0)
    {
        return CosaDmlGiGetAvailableLanguages(NULL, pValue, pulSize);
    }

    if (strcmp(ParamName, "CurrentLanguage") == 0)
    {
        if (AnscSizeOfString(pMyObject->CurrentLanguage) < *pulSize){
          AnscCopyString(pValue, pMyObject->CurrentLanguage);
          return 0;
        }
        else
        {
          *pulSize = AnscSizeOfString(pMyObject->CurrentLanguage);
          return 1;
        }
    }

    if (strcmp(ParamName, "LanHostname") == 0)
    {
        if (*pulSize <= MAX_HOSTNAME_SIZE) {
            *pulSize = MAX_HOSTNAME_SIZE + 1;
            return 1;
        }

        gethostname(pValue, *pulSize);

        return 0;
    }

    if (strcmp(ParamName, "CAppName") == 0)
    {
        if (AnscSizeOfString(pMyObject->CAppName) < *pulSize){
          AnscCopyString(pValue, pMyObject->CAppName);
          return 0;
        }
        else
        {
          *pulSize = AnscSizeOfString(pMyObject->CAppName);
          return 1;
        }
    }

    if (strcmp(ParamName, "WebsiteHelpURL") == 0)
    {
        if (AnscSizeOfString(pMyObject->WebsiteHelpURL) < *pulSize){
          AnscCopyString(pValue, pMyObject->WebsiteHelpURL);
          return 0;
        }
        else
        {
          *pulSize = AnscSizeOfString(pMyObject->WebsiteHelpURL);
          return 1;
        }
    }
    if (strcmp(ParamName, "DataModelVersion") == 0)
    {
        if (AnscSizeOfString(pMyObject->DataModelVersion) < *pulSize){
          AnscCopyString(pValue, pMyObject->DataModelVersion);
          return 0;
        }
        else
        {
          *pulSize = AnscSizeOfString(pMyObject->DataModelVersion);
          return 1;
        }
    }
    if (strcmp(ParamName, "DataModelSpecification") == 0)
    {
        if (AnscSizeOfString(pMyObject->DataModelSpecification) < *pulSize){
          AnscCopyString(pValue, pMyObject->DataModelSpecification);
          return 0;
        }
        else
        {
          *pulSize = AnscSizeOfString(pMyObject->DataModelSpecification);
          return 1;
        }
    }
    if (strcmp(ParamName, "WebUISkin") == 0)
    {
        if (AnscSizeOfString(pMyObject->WebUISkin) < *pulSize){
          AnscCopyString(pValue, pMyObject->WebUISkin);
          return 0;
        }
        else
        {
          *pulSize = AnscSizeOfString(pMyObject->WebUISkin);
          return 1;
        }
    }
    if (strcmp(ParamName, "DefaultAdminPassword") == 0)
    {
        if (AnscSizeOfString(pMyObject->DefaultAdminPassword) < *pulSize){
          AnscCopyString(pValue, pMyObject->DefaultAdminPassword);
          return 0;
        }
        else
        {
          *pulSize = AnscSizeOfString(pMyObject->DefaultAdminPassword);
          return 1;
        }
    }
    // LGI ADD END
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

BOOL
LgiGeneral_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_LGI_GENERAL  pMyObject = (PCOSA_DATAMODEL_LGI_GENERAL)g_pCosaBEManager->hLgiGeneral;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "FirstInstallWizardEnable") == 0)
    {
        pMyObject->FirstInstallWizardEnable = bValue;
        return TRUE;
    }
    if (strcmp(ParamName, "FirstInstallState") == 0)
    {
        pMyObject->FirstInstallState = bValue;
        return TRUE;
    }
    if (strcmp(ParamName, "TroubleshootWizardEnable") == 0)
    {
        pMyObject->TroubleshootWizardEnable = bValue;
        return TRUE;
    }
    if (strcmp(ParamName, "UserBridgeModeAllowed") == 0)
    {
        pMyObject->UserBridgeModeAllowed = bValue;
        return TRUE;
    }
    return FALSE;
}

BOOL
LgiGeneral_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    )
{
    /* check the parameter name and return the corresponding value */
    PCOSA_DATAMODEL_LGI_GENERAL  pMyObject = (PCOSA_DATAMODEL_LGI_GENERAL)g_pCosaBEManager->hLgiGeneral;
    if (strcmp(ParamName, "CustomerId") == 0)
    {
        /* Add support for customer index zero */
        if ( (int)uValuepUlong == 0 )
        {
            pMyObject->CustomerId = uValuepUlong;
            return TRUE;
        }
        /*
        Check that a config file matching the customer ID is present in the filesystem.
        */
        char cust_file[45];
        snprintf(cust_file,sizeof(cust_file), CUSTOMER_SYSCFG_FILE, (int)uValuepUlong);
        if (access(cust_file, F_OK) == 0)
        {
            pMyObject->CustomerId = uValuepUlong;
            return TRUE;
        }
    }
    return FALSE;
}
BOOL
LgiGeneral_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{

    PCOSA_DATAMODEL_LGI_GENERAL  pMyObject = (PCOSA_DATAMODEL_LGI_GENERAL)g_pCosaBEManager->hLgiGeneral;

    if (strcmp(ParamName, "CurrentLanguage") == 0)
    {
        /*Before SPV CurrentLanguage should be checked with the Available_Languages List present.*/
        char buf[128];
        char *token;

        syscfg_get (NULL, "Available_Languages", buf, sizeof(buf));

        token = strtok (buf, ",");

        while (token != NULL)
        {
            if(!strcmp (token, strValue))
            {
                AnscCopyString(pMyObject->CurrentLanguage, strValue);
                return TRUE;
            }
            token = strtok (NULL, ",");
        }
        return FALSE;
    }

    if (strcmp(ParamName, "CAppName") == 0)
    {
        AnscCopyString(pMyObject->CAppName, strValue);
        return TRUE;
    }
    if (strcmp(ParamName, "WebsiteHelpURL") == 0)
    {
        AnscCopyString(pMyObject->WebsiteHelpURL, strValue);
        return TRUE;
    }
    if (strcmp(ParamName, "WebUISkin") == 0)
    {
        if( AnscSizeOfString(strValue) < sizeof(pMyObject->WebUISkin))
        {
            AnscCopyString(pMyObject->WebUISkin, strValue);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    return FALSE;
}

BOOL
LgiGeneral_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
  )
{
    return true;
}


ULONG
LgiGeneral_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_GENERAL  pMyObject = (PCOSA_DATAMODEL_LGI_GENERAL)g_pCosaBEManager->hLgiGeneral;

    CosaDmlGiSetCurrentLanguage(NULL, pMyObject->CurrentLanguage);
    CosaDmlGiSetFirstInstallWizardEnable(NULL, pMyObject->FirstInstallWizardEnable);
    CosaDmlGiSetCAppName(NULL, pMyObject->CAppName);
    CosaDmlGiSetWebsiteHelpURL(NULL, pMyObject->WebsiteHelpURL);
    CosaDmlGiSetFirstInstallState(NULL, pMyObject->FirstInstallState);
    CosaDmlGiSetTroubleshootWizardEnable(NULL, pMyObject->TroubleshootWizardEnable);
    CosaDmlGiSetWebUISkin(NULL, pMyObject->WebUISkin);
    CosaDmlGiSetCustomerId(NULL, pMyObject->CustomerId);
    CosaDmlGiSetUserBridgeModeAllowed(NULL, pMyObject->UserBridgeModeAllowed);
    CosaDmlGiSaveSettings();
    return 0;
}

ULONG
LgiGeneral_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_LGI_GENERAL  pMyObject = (PCOSA_DATAMODEL_LGI_GENERAL)g_pCosaBEManager->hLgiGeneral;

    /* restore any parameters that can fail in Validate function */
    CosaDmlGiGetCustomerId(NULL, &pMyObject->CustomerId);
    return 0;
}

