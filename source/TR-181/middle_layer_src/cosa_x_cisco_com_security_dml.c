/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
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

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/


/**************************************************************************

    module: cosa_x_cisco_com_security_dml.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/14/2011    initial revision.

**************************************************************************/

#include "ansc_platform.h"
#include "cosa_x_cisco_com_security_dml.h"
#include "cosa_x_cisco_com_security_internal.h"
#include "safec_lib_common.h"

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

    X_CISCO_COM_Security.


***********************************************************************/


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

    X_CISCO_COM_Security.

    *  X_CISCO_COM_Security_GetParamBoolValue
    *  X_CISCO_COM_Security_GetParamIntValue
    *  X_CISCO_COM_Security_GetParamUlongValue
    *  X_CISCO_COM_Security_GetParamStringValue
    *  X_CISCO_COM_Security_SetParamBoolValue
    *  X_CISCO_COM_Security_SetParamIntValue
    *  X_CISCO_COM_Security_SetParamUlongValue
    *  X_CISCO_COM_Security_SetParamStringValue
    *  X_CISCO_COM_Security_Validate
    *  X_CISCO_COM_Security_Commit
    *  X_CISCO_COM_Security_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_Security_GetParamBoolValue
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
X_CISCO_COM_Security_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PCOSA_DML_SECURITY_CFG          pSecurityCfg    = &pCosaDMSecurity->SecurityConfig;


    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "ApplyFirewallSettings", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->ApplyFirewallSettings;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "BlockPingMessages", TRUE))
    {
        /* Collect Value */
        char buf[5] = {0};

        syscfg_get( NULL, "block_ping", buf, sizeof(buf));

        if(buf != NULL)
         {
             if (strcmp(buf,"1") == 0)
             {
                 *pBool = TRUE;
                 return TRUE;
             }
         }
         *pBool = FALSE;

         return TRUE;
    }

    if( AnscEqualString(ParamName, "CleanLog", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->CleanLog;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "TriggerEmailLog", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->TriggerEmailLog;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailEnable", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->EmailEnable;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailParentalControlBreach", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->EmailParentalControlBreach;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailAlertsOrWarnings", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->EmailAlertsOrWarnings;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailFirewallBreach", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->EmailFirewallBreach;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailSendLogs", TRUE))
    {
        /* collect value */
        *pBool = pSecurityCfg->EmailSendLogs;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_Security_GetParamIntValue
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
X_CISCO_COM_Security_GetParamIntValue
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
        X_CISCO_COM_Security_GetParamUlongValue
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
X_CISCO_COM_Security_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and get the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_Security_GetParamStringValue
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
X_CISCO_COM_Security_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PCOSA_DML_SECURITY_CFG          pSecurityCfg    = &pCosaDMSecurity->SecurityConfig;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "FilterWebTraffic", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->FilterWebTraffic);

        return 0;
    }

    if( AnscEqualString(ParamName, "TrafficDetect", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->TrafficDetect);

        return 0;
    }

    if( AnscEqualString(ParamName, "FilterLanTraffic", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->FilterLanTraffic);

        return 0;
    }

    if( AnscEqualString(ParamName, "AllowPassthrough", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->AllowPassthrough);

        return 0;
    }

    if( AnscEqualString(ParamName, "EmailSendTo", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->EmailSendTo);

        return 0;
    }

    if( AnscEqualString(ParamName, "EmailServer", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->EmailServer);

        return 0;
    }

    if( AnscEqualString(ParamName, "EmailUserName", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->EmailUserName);

        return 0;
    }

    if( AnscEqualString(ParamName, "EmailPassword", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->EmailPassword);

        return 0;
    }

    if( AnscEqualString(ParamName, "EmailFromAddress", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSecurityCfg->EmailFromAddress);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_Security_SetParamBoolValue
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
X_CISCO_COM_Security_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PCOSA_DML_SECURITY_CFG          pSecurityCfg    = &pCosaDMSecurity->SecurityConfig;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "ApplyFirewallSettings", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->ApplyFirewallSettings = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "BlockPingMessages", TRUE))
    {
        char buf[5];
        int value;

        syscfg_get( NULL, "block_ping" , buf, sizeof(buf));
        value = atoi(buf);
        if(value != bValue)
        {
            syscfg_set(NULL, "block_ping", bValue == TRUE ? "1" : "0");
            syscfg_set(NULL, "block_pingv6", bValue == TRUE ? "1" : "0");

            if (syscfg_commit() != 0)
            {
                AnscTraceWarning(("Firewall - syscfg_commit BlockPingMessages failed!\n"));
            }
            else
            {
                //Restart firewall to apply BlockPingMessages setting
                //commonSyseventSet("firewall-restart", ""); In Current code the sysevent method will not restart firewall , Uncomment once the issue is resolved.
                system("firewall restart");
            }
        }
        return TRUE;
    }

    if( AnscEqualString(ParamName, "CleanLog", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->CleanLog = bValue;

        return TRUE;
    }


    if( AnscEqualString(ParamName, "TriggerEmailLog", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->TriggerEmailLog = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailEnable", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->EmailEnable = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailParentalControlBreach", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->EmailParentalControlBreach = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailAlertsOrWarnings", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->EmailAlertsOrWarnings = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailFirewallBreach", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->EmailFirewallBreach = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EmailSendLogs", TRUE))
    {
        /* save update to backup */
        pSecurityCfg->EmailSendLogs = bValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_Security_SetParamIntValue
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
X_CISCO_COM_Security_SetParamIntValue
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
        X_CISCO_COM_Security_SetParamUlongValue
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
X_CISCO_COM_Security_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
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
        X_CISCO_COM_Security_SetParamStringValue
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
X_CISCO_COM_Security_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    errno_t                         rc              = -1;
    int                             ind             = -1;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PCOSA_DML_SECURITY_CFG          pSecurityCfg    = &pCosaDMSecurity->SecurityConfig;

    if((pString == NULL) || (ParamName == NULL))
    {
        AnscTraceWarning(("RDK_LOG_WARN, %s %s:%d\n",__FILE__,__FUNCTION__,__LINE__));
        return FALSE;
    }

    /* check the parameter name and set the corresponding value */
    if(!(rc = strcmp_s("FilterWebTraffic", strlen("FilterWebTraffic"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->FilterWebTraffic, sizeof(pSecurityCfg->FilterWebTraffic), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
	    return TRUE;
	}
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("TrafficDetect", strlen("TrafficDetect"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->TrafficDetect, sizeof(pSecurityCfg->TrafficDetect), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("FilterLanTraffic", strlen("FilterLanTraffic"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->FilterLanTraffic, sizeof(pSecurityCfg->FilterLanTraffic), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("AllowPassthrough", strlen("AllowPassthrough"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->AllowPassthrough, sizeof(pSecurityCfg->AllowPassthrough), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("EmailSendTo", strlen("EmailSendTo"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
            rc = STRCPY_S_NOCLOBBER(pSecurityCfg->EmailSendTo, sizeof(pSecurityCfg->EmailSendTo), pString);
            if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("EmailServer", strlen("EmailServer"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->EmailServer, sizeof(pSecurityCfg->EmailServer), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("EmailUserName", strlen("EmailUserName"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->EmailUserName, sizeof(pSecurityCfg->EmailUserName), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("EmailPassword", strlen("EmailPassword"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->EmailPassword, sizeof(pSecurityCfg->EmailPassword), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if(!(rc = strcmp_s("EmailFromAddress", strlen("EmailFromAddress"), ParamName, &ind)))
    {
        if(!ind)
        {
            /* collect value */
	    rc = STRCPY_S_NOCLOBBER(pSecurityCfg->EmailFromAddress, sizeof(pSecurityCfg->EmailFromAddress), pString);
	    if(rc != EOK)
            {
                AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                return FALSE;
            }
            return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_CISCO_COM_Security_Validate
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
X_CISCO_COM_Security_Validate
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
        X_CISCO_COM_Security_Commit
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
X_CISCO_COM_Security_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS ret;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PCOSA_DML_SECURITY_CFG          pSecurityCfg    = &pCosaDMSecurity->SecurityConfig;

    ret = CosaDmlSecuritySetConfig(NULL, pSecurityCfg);
    pSecurityCfg->TriggerEmailLog = FALSE;

    return ret;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_CISCO_COM_Security_Rollback
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
X_CISCO_COM_Security_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}


/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.Firewall.

    *  Firewall1_GetParamBoolValue
    *  Firewall1_GetParamIntValue
    *  Firewall1_GetParamUlongValue
    *  Firewall1_GetParamStringValue
    *  Firewall1_SetParamBoolValue
    *  Firewall1_SetParamIntValue
    *  Firewall1_SetParamUlongValue
    *  Firewall1_SetParamStringValue
    *  Firewall1_Validate
    *  Firewall1_Commit
    *  Firewall1_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall1_GetParamBoolValue
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
extern int fwSync;

BOOL
Firewall1_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PCOSA_DML_FIREWALL_CFG2         pFirewallCfg    = &pCosaDMSecurity->FirewallConfig;

    if(fwSync)
    {
        CosaDmlFirewallGetConfig2(NULL, pFirewallCfg);
        fwSync = 0;
    }

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "L2TPPassthrough", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->L2TPPassthrough;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "PPTPPassthrough", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->PPTPPassthrough;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IPSecPassthrough", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->IPSecPassthrough;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterNATRedirection", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterNATRedirection;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterMulticast", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterMulticast;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterMulticastV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterMulticastV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterAnonymousInternetRequests", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterAnonymousInternetRequests;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterAnonymousInternetRequestsV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterAnonymousInternetRequestsV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockCookies", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->WebBlockCookies;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockActiveX", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->WebBlockActiveX;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockJava", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->WebBlockJava;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockProxy", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->WebBlockProxy;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTP", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterHTTP;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTPV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterHTTPV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTPs", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterHTTPs;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTPsV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterHTTPsV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterP2P", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterP2P;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterP2PV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterP2PV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterIdent", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterIdent;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterIdentV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->FilterIdentV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "TrueStaticIpEnable", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->TrueStaticIpEnable;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "TrueStaticIpEnableV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->TrueStaticIpEnableV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SmartPktDectionEnable", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->SmartPktDectionEnable;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SmartPktDectionEnableV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->SmartPktDectionEnableV6;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WanPingEnable", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->WanPingEnable;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WanPingEnableV6", TRUE))
    {
        /* collect value */
        *pBool = pFirewallCfg->WanPingEnableV6;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall1_GetParamIntValue
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
Firewall1_GetParamIntValue
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
        Firewall1_GetParamUlongValue
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
Firewall1_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PCOSA_DML_FIREWALL_CFG2         pFirewallCfg    = &pCosaDMSecurity->FirewallConfig;

    CosaDmlFirewallGetConfig2(NULL, pFirewallCfg); //ARRISXB6-1181 ARRISXB6-7324

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "FirewallLevel", TRUE))
    {
        /* collect value */
        *puLong = pFirewallCfg->FirewallLevel;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FirewallLevelV6", TRUE))
    {
        /* collect value */
        *puLong = pFirewallCfg->FirewallLevelV6;

        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Firewall1_GetParamStringValue
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
Firewall1_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall1_SetParamBoolValue
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
Firewall1_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    BOOL                            pBridgeMode     = FALSE;

    if((ANSC_STATUS_SUCCESS == is_usg_in_bridge_mode(&pBridgeMode)) && (pBridgeMode == TRUE))
        return FALSE;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "L2TPPassthrough", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.L2TPPassthrough = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "PPTPPassthrough", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.PPTPPassthrough = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IPSecPassthrough", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.IPSecPassthrough = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterNATRedirection", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterNATRedirection = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterMulticast", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterMulticast = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterMulticastV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterMulticastV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterAnonymousInternetRequests", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterAnonymousInternetRequests = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterAnonymousInternetRequestsV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterAnonymousInternetRequestsV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockCookies", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.WebBlockCookies = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockActiveX", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.WebBlockActiveX = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockJava", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.WebBlockJava = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WebBlockProxy", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.WebBlockProxy = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTP", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterHTTP = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTPV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterHTTPV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTPs", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterHTTPs = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterHTTPsV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterHTTPsV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterP2P", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterP2P = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterP2PV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterP2PV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterIdent", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterIdent = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "FilterIdentV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FilterIdentV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "TrueStaticIpEnable", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.TrueStaticIpEnable = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "TrueStaticIpEnableV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.TrueStaticIpEnableV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SmartPktDectionEnable", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.SmartPktDectionEnable = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "SmartPktDectionEnableV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.SmartPktDectionEnableV6 = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WanPingEnable", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.WanPingEnable = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "WanPingEnableV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.WanPingEnableV6 = bValue;

        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall1_SetParamIntValue
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
Firewall1_SetParamIntValue
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
        Firewall1_SetParamUlongValue
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
Firewall1_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    BOOL                            pBridgeMode     = FALSE;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "FirewallLevel", TRUE))
    {
        /* save update to backup */
        if(CosaGetParamValueBool("Device.X_RDKCENTRAL-COM_VideoService.Enabled") && uValue == 1)
        {
            CcspTraceWarning(("Firewall Level can not be set to HIGH when VideoService is ENABLED\n"));
            return FALSE;
        }
        pCosaDMSecurity->FirewallConfig.FirewallLevel = uValue;

	/*RDKB-28857 : Restore firewall settings does not restore check boxes in custom security */
	if( pCosaDMSecurity->FirewallConfig.FirewallLevel == COSA_DML_FIREWALL_LEVEL_Low )
	{
		/*block_http*/
		pCosaDMSecurity->FirewallConfig.FilterHTTP = false;
		pCosaDMSecurity->FirewallConfig.FilterHTTPs = false;
		/*block_icmp*/
		pCosaDMSecurity->FirewallConfig.FilterAnonymousInternetRequests = false;
		/*block_multicast*/
		pCosaDMSecurity->FirewallConfig.FilterMulticast = false;
		/*block_peer*/
		pCosaDMSecurity->FirewallConfig.FilterP2P = false;
		/*block_ident*/
		pCosaDMSecurity->FirewallConfig.FilterIdent = false;
	}
        return TRUE;
    }

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "FirewallLevelV6", TRUE))
    {
        /* save update to backup */
        pCosaDMSecurity->FirewallConfig.FirewallLevelV6 = uValue;

	/*RDKB-28857 : Restore firewall settings does not restore check boxes in custom security */
	if( pCosaDMSecurity->FirewallConfig.FirewallLevelV6 == COSA_DML_FIREWALL_LEVEL_High )
	{
		/*block_http*/
		pCosaDMSecurity->FirewallConfig.FilterHTTPV6 = false;
		pCosaDMSecurity->FirewallConfig.FilterHTTPsV6 = false;
		/*block_icmp*/
		pCosaDMSecurity->FirewallConfig.FilterAnonymousInternetRequestsV6 = false;
		/*block_multicast*/
		pCosaDMSecurity->FirewallConfig.FilterMulticastV6 = false;
		/*block_peer*/
		pCosaDMSecurity->FirewallConfig.FilterP2PV6 = false;
		/*block_ident*/
		pCosaDMSecurity->FirewallConfig.FilterIdentV6 = false;
	}
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Firewall1_SetParamStringValue
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
Firewall1_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
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
        Firewall1_Validate
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
Firewall1_Validate
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
        Firewall1_Commit
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
Firewall1_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;

    return CosaDmlFirewallSetConfig2(NULL, &pCosaDMSecurity->FirewallConfig);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Firewall1_Rollback
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
Firewall1_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}

/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.InternetAccess.

    *  InternetAccess_GetParamBoolValue
    *  InternetAccess_GetParamIntValue
    *  InternetAccess_GetParamUlongValue
    *  InternetAccess_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        InternetAccess_GetParamBoolValue
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
InternetAccess_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
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
        InternetAccess_GetParamIntValue
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
InternetAccess_GetParamIntValue
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
        InternetAccess_GetParamUlongValue
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
InternetAccess_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "NumberOfPolicies", TRUE))
    {
        /* collect value */
        /*
        *puLong = CosaDmlIaGetNumberOfPolicies(NULL);
        */
        return TRUE;
        
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        InternetAccess_GetParamStringValue
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
InternetAccess_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "DumpAllFWlog", TRUE))
    {
        ULONG logSize = *pUlSize;
        if(ANSC_STATUS_FAILURE == CosaDmlIaGetALLLogEntries(pValue, &logSize) != ANSC_STATUS_SUCCESS)
        {
            if(logSize > *pUlSize){
                *pUlSize = logSize;
                return 1;
            }
            else
                return -1;
        }
        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.InternetAccess.AccessPolicy.{i}.

    *  AccessPolicy_GetEntryCount
    *  AccessPolicy_GetEntry
    *  AccessPolicy_AddEntry
    *  AccessPolicy_DelEntry
    *  AccessPolicy_GetParamBoolValue
    *  AccessPolicy_GetParamIntValue
    *  AccessPolicy_GetParamUlongValue
    *  AccessPolicy_GetParamStringValue
    *  AccessPolicy_SetParamBoolValue
    *  AccessPolicy_SetParamIntValue
    *  AccessPolicy_SetParamUlongValue
    *  AccessPolicy_SetParamStringValue
    *  AccessPolicy_Validate
    *  AccessPolicy_Commit
    *  AccessPolicy_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AccessPolicy_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
AccessPolicy_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;

    return AnscSListQueryDepth(&pCosaDMSecurity->AccessPolicyList);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        AccessPolicy_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
AccessPolicy_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PSLIST_HEADER                   pPolicyHead       = (PSLIST_HEADER            )&pCosaDMSecurity->AccessPolicyList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext      = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy      = (PCOSA_DML_IA_POLICY2     )NULL;
    PCOSA_DML_IA_POLICY2            pDmlIAEntry       = (PCOSA_DML_IA_POLICY2     )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry       = (PSINGLE_LINK_ENTRY       )NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(pPolicyHead, nIndex);

    if ( pSLinkEntry )
    {
        pCosaContext = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
	if(pCosaContext == NULL)
	{
		CcspTraceWarning(("AccessPolicy_GetEntry - pCosaContext is NULL\n"));
		return NULL;
	}
        *pInsNumber = pCosaContext->InstanceNumber;
    }

    return pCosaContext; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        AccessPolicy_AddEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to add a new entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle of new added entry.

**********************************************************************/
ANSC_HANDLE
AccessPolicy_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PSLIST_HEADER                   pPolicyHead       = (PSLIST_HEADER            )&pCosaDMSecurity->AccessPolicyList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext      = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy      = (PCOSA_DML_IA_POLICY2     )NULL;

    pDmlIAPolicy = (PCOSA_DML_IA_POLICY2)AnscAllocateMemory(sizeof(COSA_DML_IA_POLICY2));

    if ( !pDmlIAPolicy )
    {
        return NULL;
    }

    /* Set the default policy name */
    _ansc_sprintf(pDmlIAPolicy->Alias, "Policy%d", pCosaDMSecurity->ulNextAPInstanceNum);

    AnscSListInitializeHeader(&pDmlIAPolicy->URLList    );
    AnscSListInitializeHeader(&pDmlIAPolicy->KeywordList);
    AnscSListInitializeHeader(&pDmlIAPolicy->AppList    );

    pDmlIAPolicy->ulNextURLInsNum = 1;
    pDmlIAPolicy->ulNextKeyInsNum = 1;
    pDmlIAPolicy->ulNextAppInsNum = 1;
    
    /* CosaDmlIaAddPolicy(NULL, (PCOSA_DML_IA_POLICY)pDmlIAPolicy); */

    /* Update the middle layer data */
    if ( TRUE )
    {
        pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));

        if ( !pCosaContext )
        {
            AnscFreeMemory(pDmlIAPolicy);
            
            return NULL;
        }

        pCosaContext->InstanceNumber = pDmlIAPolicy->InstanceNumber = pCosaDMSecurity->ulNextAPInstanceNum;

        pCosaDMSecurity->ulNextAPInstanceNum++;

        if ( pCosaDMSecurity->ulNextAPInstanceNum == 0 )
        {
            pCosaDMSecurity->ulNextAPInstanceNum = 1;
        }

        pCosaContext->hContext         = (ANSC_HANDLE)pDmlIAPolicy;
        pCosaContext->hParentTable     = NULL;
        pCosaContext->hPoamIrepUpperFo = pCosaDMSecurity->hIrepFolderIAHA;
        pCosaContext->bNew             = TRUE;

        CosaSListPushEntryByInsNum(pPolicyHead, pCosaContext);

        CosaSecurityIARegAddInfo
            (
                (ANSC_HANDLE)pCosaDMSecurity,
                COSA_DML_RR_NAME_IA_NextInsNunmber,
                pCosaDMSecurity->ulNextAPInstanceNum,
                COSA_DML_RR_NAME_IA_Prefix,
                0,
                pDmlIAPolicy->Alias,
                (ANSC_HANDLE)pCosaContext
            );
    }

    *pInsNumber = pCosaContext->InstanceNumber;

    return pCosaContext; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AccessPolicy_DelEntry
            (
                ANSC_HANDLE                 hInsContext,
                ANSC_HANDLE                 hInstance
            );

    description:

        This function is called to delete an exist entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ANSC_HANDLE                 hInstance
                The exist entry handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
AccessPolicy_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext      = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy      = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PSLIST_HEADER                   pPolicyHead       = (PSLIST_HEADER            )&pCosaDMSecurity->AccessPolicyList;

    if ( pCosaContext->bNew )
    {
        returnStatus = CosaSecurityIARegDelInfo((ANSC_HANDLE)pCosaDMSecurity, (ANSC_HANDLE)pCosaContext);
    }
    else
    {
        returnStatus = CosaDmlIaDelPolicy(NULL, pDmlIAPolicy->InstanceNumber);
    }

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscSListPopEntryByLink(pPolicyHead, &pCosaContext->Linkage);
     
        AnscFreeMemory(pDmlIAPolicy);
        AnscFreeMemory(pCosaContext);
    }

    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AccessPolicy_GetParamBoolValue
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
AccessPolicy_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* collect value */
        *pBool = pDmlIAPolicy->bEnabled;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "LanHostsIsEffective", TRUE) )
    {
        /* collect value */
        *pBool = pDmlIAPolicy->LanHost.bUseLanHosts;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "EnableLanHostMACAddresses", TRUE))
    {
        /* collect value */
        *pBool = pDmlIAPolicy->EnableLanHostMACAddresses;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "AllowLanHostMACAddresses", TRUE) )
    {
        /* collect value */
        *pBool = pDmlIAPolicy->AllowLanHostMACAddresses;

        return TRUE;
    }
    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AccessPolicy_GetParamIntValue
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
AccessPolicy_GetParamIntValue
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
        AccessPolicy_GetParamUlongValue
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
AccessPolicy_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    /* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "NumberOfApplications", TRUE))
    {
        /* collect value */
        *puLong = AnscSListQueryDepth(&pDmlIAPolicy->AppList);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "NumberOfKeywords", TRUE))
    {
        /* collect value */
        *puLong = AnscSListQueryDepth(&pDmlIAPolicy->KeywordList);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "NumberOfURLs", TRUE))
    {
        /* collect value */
        *puLong = AnscSListQueryDepth(&pDmlIAPolicy->URLList);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AccessPolicy_GetParamStringValue
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
AccessPolicy_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    ULONG                           i            = 0;
    int                             pre_buf_size = 1023;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pDmlIAPolicy->Alias);

        return 0;
    }

    if( AnscEqualString(ParamName, "LanHostMACAddresses", TRUE) )
    {
        /* collect value */

        if ( pDmlIAPolicy->LanHost.MacCount > pre_buf_size/AnscSizeOfString("AA:AA:AA:AA:AA:AA,") )
        {
            *pUlSize = pDmlIAPolicy->LanHost.MacCount * AnscSizeOfString("AA:AA:AA:AA:AA:AA,");

            return 1;
        }
        else
        {
            /* RDKB-6745, CID-32995, Out-of-bounds read
            ** Adding upper bounday check for MacList to avoid out of bound access.
            */
            for ( i = 0; (i < pDmlIAPolicy->LanHost.MacCount) && (i < COSA_DML_IA_LH_MAX_MAC); i++ )
            {
                _ansc_sprintf
                    (
                        pValue, 
                        "%02X:%02X:%02X:%02X:%02X:%02X",
                        pDmlIAPolicy->LanHost.MacList[i].Mac[0],
                        pDmlIAPolicy->LanHost.MacList[i].Mac[1],
                        pDmlIAPolicy->LanHost.MacList[i].Mac[2],
                        pDmlIAPolicy->LanHost.MacList[i].Mac[3],
                        pDmlIAPolicy->LanHost.MacList[i].Mac[4],
                        pDmlIAPolicy->LanHost.MacList[i].Mac[5]
                    );

                if ( i < (ULONG)pDmlIAPolicy->LanHost.MacCount - 1 )
                {
                    _ansc_strcat(pValue, ",");
                }

                pValue = pValue + AnscSizeOfString(pValue);
            }
        
            return 0;
        }
    }

    if( AnscEqualString(ParamName, "LanHostIPAddresses", TRUE) )
    {
        /* collect value */

        if ( pDmlIAPolicy->LanHost.IpCount > pre_buf_size/AnscSizeOfString("AAA.AAA.AAA.AAA,") )
        {
            *pUlSize = pDmlIAPolicy->LanHost.IpCount * AnscSizeOfString("AAA.AAA.AAA.AAA,");

            return 1;
        }
        else
        {
            /* RDKB-6745, CID-33020,  Out-of-bounds read
            ** Adding upper bounday check for MacList to avoid out of bound access.
            */
            for ( i =0; (i< pDmlIAPolicy->LanHost.IpCount) && (i < COSA_DML_IA_LH_MAX_IP); i++ )
            {
                AnscGetIpAddrString(&pDmlIAPolicy->LanHost.IpList[i].Ip.Value, pValue);

                if ( i < (ULONG)pDmlIAPolicy->LanHost.IpCount - 1 )
                {
                    _ansc_strcat(pValue, ",");
                }

                pValue = pValue + AnscSizeOfString(pValue);
            }

            return 0;
        }
    }

    if( AnscEqualString(ParamName, "LanHostIPRange", TRUE) )
    {
        /* collect value */

        if ( pDmlIAPolicy->LanHost.IprCount > pre_buf_size/AnscSizeOfString("AAA.AAA.AAA.AAA-AAA.AAA.AAA.AAA,") )
        {
            *pUlSize = pDmlIAPolicy->LanHost.IprCount * AnscSizeOfString("AAA.AAA.AAA.AAA-AAA.AAA.AAA.AAA,");

            return 1;
        }
        else
        {
            /* RDKB-6745, CID-33020,  Out-of-bounds read
            ** Adding upper bounday check for MacList to avoid out of bound access.
            */
            for ( i = 0; (i < pDmlIAPolicy->LanHost.IprCount) && (i < COSA_DML_IA_LH_MAX_IP_RANGE); i++ )
            {
                AnscGetIpAddrString(&pDmlIAPolicy->LanHost.IprList[i].StartIp.Value, pValue);

                _ansc_strcat(pValue, "-");

                pValue = pValue + AnscSizeOfString(pValue);

                AnscGetIpAddrString(&pDmlIAPolicy->LanHost.IprList[i].EndIp.Value, pValue);

                if ( i < (ULONG)pDmlIAPolicy->LanHost.IprCount - 1 )
                {
                    _ansc_strcat(pValue, ",");
                }

                pValue = pValue + AnscSizeOfString(pValue);
            }

            return 0;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AccessPolicy_SetParamBoolValue
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
AccessPolicy_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy    = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Enable", TRUE))
    {
        /* save update to backup */
        pDmlIAPolicy->bEnabled = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "LanHostsIsEffective", TRUE) )
    {
        /*the backend doesn't support setting to LanHostsIsEffective to false*/
        if (bValue == TRUE) 
        {
            pDmlIAPolicy->LanHost.bUseLanHosts = bValue;

            return TRUE;
        }
    }
    
    if( AnscEqualString(ParamName, "EnableLanHostMACAddresses", TRUE))
    {
        /* save update to backup */
        pDmlIAPolicy->EnableLanHostMACAddresses = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "AllowLanHostMACAddresses", TRUE) )
    {
        /*the backend doesn't support setting to LanHostsIsEffective to false*/
        if (bValue == TRUE) 
        {
            pDmlIAPolicy->AllowLanHostMACAddresses = bValue;

            return TRUE;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AccessPolicy_SetParamIntValue
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
AccessPolicy_SetParamIntValue
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
        AccessPolicy_SetParamUlongValue
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
AccessPolicy_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
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
        AccessPolicy_SetParamStringValue
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
AccessPolicy_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy    = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    char*                           pNext           = pString;
    USHORT                          ulCount         = 0;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pDmlIAPolicy->Alias, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "LanHostMACAddresses", TRUE) )
    {
        /* save update to backup */

        while ( pString )
        {
            int i = 0;
            int tmp[6] = {0};

            pNext = _ansc_strchr(pString, ',');

            if ( pNext )
            {
                *pNext = '\0';

                pNext++;
            }

            _ansc_sscanf
            (
                pString, 
                "%02X:%02X:%02X:%02X:%02X:%02X", 
                &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4],&tmp[5] 
            );
            
            for (i=0; i<6; i++)
                pDmlIAPolicy->LanHost.MacList[ulCount].Mac[i] = tmp[i];

            pString = pNext;

            ulCount++;
        }

        pDmlIAPolicy->LanHost.MacCount = ulCount;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "LanHostIPAddresses", TRUE) )
    {
        /* save update to backup */

        while ( pString )
        {
            pNext = _ansc_strchr(pString, ',');

            if ( pNext )
            {
                *pNext = '\0';

                pNext++;
            }

            pDmlIAPolicy->LanHost.IpList[ulCount++].Ip.Value = _ansc_inet_addr(pString);

#ifdef _COSA_DRG_CNS_
                /*the ip address should fall in the subnet of lan interface*/
                ANSC_IPV4_ADDRESS  addr = {0};
                addr.Value = CosaUtilGetIfAddr("brlan0");

                if (addr.Dot[0]!= pDmlIAPolicy->LanHost.IpList[ulCount-1].Ip.Dot[0] || 
                    addr.Dot[1]!= pDmlIAPolicy->LanHost.IpList[ulCount-1].Ip.Dot[1] ||
                    addr.Dot[2]!= pDmlIAPolicy->LanHost.IpList[ulCount-1].Ip.Dot[2])
                    return FALSE;

#endif
            pString = pNext;
        }

        pDmlIAPolicy->LanHost.IpCount = ulCount;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "LanHostIPRange", TRUE) )
    {
        /* save update to backup */
        char * str = NULL;
        char * p_token = NULL;
        char * saveptr = NULL;
        char * dup_str = AnscCloneString(pString);

        if (!dup_str)
            return FALSE;
                
        for (str = dup_str; ; str = NULL) 
        {
            p_token = strtok_r(str, ",", &saveptr);
            if (p_token == NULL)
                break;


            pNext = _ansc_strchr(p_token, '-');

            if (!pNext )
                continue;

            *pNext = '\0';

            pDmlIAPolicy->LanHost.IprList[ulCount].StartIp.Value = _ansc_inet_addr(p_token);

            p_token = pNext+1;

            pDmlIAPolicy->LanHost.IprList[ulCount].EndIp.Value = _ansc_inet_addr(p_token);

#ifdef _COSA_DRG_CNS_
            /*the iprange should fall in the subnet of lan interface*/
            ANSC_IPV4_ADDRESS  addr = {0};
            addr.Value = CosaUtilGetIfAddr("brlan0");


            if (addr.Dot[0]!= pDmlIAPolicy->LanHost.IprList[ulCount].StartIp.Dot[0] || 
                addr.Dot[1]!= pDmlIAPolicy->LanHost.IprList[ulCount].StartIp.Dot[1] ||
                addr.Dot[2]!= pDmlIAPolicy->LanHost.IprList[ulCount].StartIp.Dot[2])
            {
                AnscFreeMemory(dup_str);
                return FALSE;
            }

            if (addr.Dot[0]!= pDmlIAPolicy->LanHost.IprList[ulCount].EndIp.Dot[0] || 
                addr.Dot[1]!= pDmlIAPolicy->LanHost.IprList[ulCount].EndIp.Dot[1] ||
                addr.Dot[2]!= pDmlIAPolicy->LanHost.IprList[ulCount].EndIp.Dot[2])
            {
                AnscFreeMemory(dup_str);
                return FALSE;
            }
#endif

            ulCount++;
        }
        
        AnscFreeMemory(dup_str);

        pDmlIAPolicy->LanHost.IprCount = ulCount;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AccessPolicy_Validate
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
AccessPolicy_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY)g_pCosaBEManager->hSecurity;
    PSLIST_HEADER                   pPolicyHead       = (PSLIST_HEADER            )&pCosaDMSecurity->AccessPolicyList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy      = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2     = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_IA_POLICY2            pSecurityIAPolicy = (PCOSA_DML_IA_POLICY2     )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry       = (PSINGLE_LINK_ENTRY       )NULL;

    pSLinkEntry = AnscSListGetFirstEntry(pPolicyHead);

    while ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry   = AnscSListGetNextEntry(pSLinkEntry);

        pSecurityIAPolicy = (PCOSA_DML_IA_POLICY2)pCosaContext2->hContext;

        if ( 
                 pSecurityIAPolicy && 
                 ((ULONG)pSecurityIAPolicy != (ULONG)pDmlIAPolicy) && 
                 AnscEqualString(pSecurityIAPolicy->Alias, pDmlIAPolicy->Alias, TRUE) 
           )
        {
            AnscCopyString(pReturnParamName, "Alias");

            *puLength = AnscSizeOfString("Alias");
             
            return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AccessPolicy_Commit
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
AccessPolicy_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus    = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy    = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    if ( pCosaContext->bNew )
    {
        returnStatus = CosaDmlIaAddPolicy(NULL, (PCOSA_DML_IA_POLICY)pDmlIAPolicy);

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCosaContext->bNew = FALSE;

            CosaSecurityIARegDelInfo(NULL, pCosaContext);
        }
    }
    else
    {
        returnStatus = CosaDmlIaSetPolicy(NULL, pDmlIAPolicy->InstanceNumber, (PCOSA_DML_IA_POLICY)pDmlIAPolicy);
    }
    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        AccessPolicy_Rollback
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
AccessPolicy_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext      = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy      = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    return CosaDmlIaGetPolicyByInsNum
               (
                   NULL, 
                   pDmlIAPolicy->InstanceNumber,
                   (PCOSA_DML_IA_POLICY)pDmlIAPolicy
               );
}

/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.InternetAccess.AccessPolicy.{i}.Schedule.

    *  Schedule_GetParamBoolValue
    *  Schedule_GetParamIntValue
    *  Schedule_GetParamUlongValue
    *  Schedule_GetParamStringValue
    *  Schedule_SetParamBoolValue
    *  Schedule_SetParamIntValue
    *  Schedule_SetParamUlongValue
    *  Schedule_SetParamStringValue
    *  Schedule_Validate
    *  Schedule_Commit
    *  Schedule_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Schedule_GetParamBoolValue
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
Schedule_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_SCH         pSchedule    = (PCOSA_DML_IA_POLICY_SCH  )&pDmlIAPolicy->Schedule;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "IncludeSaturday", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->IncludeSaturday;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeFriday", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->IncludeFriday;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeThursday", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->IncludeThursday;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeWednesday", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->IncludeWednesday;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeTuesday", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->IncludeTuesday;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeMonday", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->IncludeMonday;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeSunday", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->IncludeSunday;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "AllowAccess", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->AllowAccess;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "AllDay", TRUE))
    {
        /* collect value */
        *pBool = pSchedule->AllDay;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Schedule_GetParamIntValue
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
Schedule_GetParamIntValue
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
        Schedule_GetParamUlongValue
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
Schedule_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Schedule_GetParamStringValue
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
Schedule_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_SCH         pSchedule    = (PCOSA_DML_IA_POLICY_SCH  )&pDmlIAPolicy->Schedule;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "EndTime", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSchedule->EndTime);

        return 0;
    }

    if( AnscEqualString(ParamName, "StartTime", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pSchedule->StartTime);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Schedule_SetParamBoolValue
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
Schedule_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_SCH         pSchedule    = (PCOSA_DML_IA_POLICY_SCH  )&pDmlIAPolicy->Schedule;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "IncludeSaturday", TRUE))
    {
        /* save update to backup */
        pSchedule->IncludeSaturday = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeFriday", TRUE))
    {
        /* save update to backup */
        pSchedule->IncludeFriday = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeThursday", TRUE))
    {
        /* save update to backup */
        pSchedule->IncludeThursday = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeWednesday", TRUE))
    {
        /* save update to backup */
        pSchedule->IncludeWednesday = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeTuesday", TRUE))
    {
        /* save update to backup */
        pSchedule->IncludeTuesday = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeMonday", TRUE))
    {
        /* save update to backup */
        pSchedule->IncludeMonday = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "IncludeSunday", TRUE))
    {
        /* save update to backup */
        pSchedule->IncludeSunday = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "AllowAccess", TRUE))
    {
        /* save update to backup */
        pSchedule->AllowAccess = bValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "AllDay", TRUE))
    {
        /* save update to backup */
        pSchedule->AllDay = bValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Schedule_SetParamIntValue
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
Schedule_SetParamIntValue
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
        Schedule_SetParamUlongValue
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
Schedule_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
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
        Schedule_SetParamStringValue
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
Schedule_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_SCH         pSchedule    = (PCOSA_DML_IA_POLICY_SCH  )&pDmlIAPolicy->Schedule;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "EndTime", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pSchedule->EndTime, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "StartTime", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pSchedule->StartTime, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Schedule_Validate
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
Schedule_Validate
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
        Schedule_Commit
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
Schedule_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_SCH         pSchedule    = (PCOSA_DML_IA_POLICY_SCH  )&pDmlIAPolicy->Schedule;

    return CosaDmlIaSetPolicySchedule
               (
                   NULL,
                   pDmlIAPolicy->InstanceNumber,
                   pSchedule
               );

}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Schedule_Rollback
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
Schedule_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    return 0;
}


/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.InternetAccess.AccessPolicy.{i}.BlockedURL.{i}.

    *  BlockedURL_GetEntryCount
    *  BlockedURL_GetEntry
    *  BlockedURL_GetParamBoolValue
    *  BlockedURL_GetParamIntValue
    *  BlockedURL_GetParamUlongValue
    *  BlockedURL_GetParamStringValue
    *  BlockedURL_SetParamBoolValue
    *  BlockedURL_SetParamIntValue
    *  BlockedURL_SetParamUlongValue
    *  BlockedURL_SetParamStringValue
    *  BlockedURL_Validate
    *  BlockedURL_Commit
    *  BlockedURL_Rollbac

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedURL_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
BlockedURL_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    return AnscSListQueryDepth(&pDmlIAPolicy->URLList);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        BlockedURL_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
BlockedURL_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext  = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy  = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead     = (PSLIST_HEADER            )&pDmlIAPolicy->URLList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2 = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry   = (PSINGLE_LINK_ENTRY       )NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(&pDmlIAPolicy->URLList, nIndex);

    if ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);

        *pInsNumber = pCosaContext2->InstanceNumber;
    }

    return pCosaContext2;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        BlockedURL_AddEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to add a new entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle of new added entry.

**********************************************************************/
ANSC_HANDLE
BlockedURL_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy    = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead       = (PSLIST_HEADER            )&pDmlIAPolicy->URLList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2   = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_IA_POLICY_URL         pURL            = (PCOSA_DML_IA_POLICY_URL )NULL;

    pURL = (PCOSA_DML_IA_POLICY_URL)AnscAllocateMemory(sizeof(COSA_DML_IA_POLICY_URL));

    if ( !pURL )
    {
        return NULL;
    }

    _ansc_sprintf(pURL->Alias, "BlockedURL%d", pDmlIAPolicy->ulNextURLInsNum);

    /* Update the middle layer cache */
    if ( TRUE )
    {
        pCosaContext2 = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));

        if ( !pCosaContext2 )
        {
            AnscFreeMemory(pURL);
            
            return NULL;
        }

        pCosaContext2->InstanceNumber = pURL->InstanceNumber = pDmlIAPolicy->ulNextURLInsNum;

        pDmlIAPolicy->ulNextURLInsNum++;

        if ( pDmlIAPolicy->ulNextURLInsNum == 0 )
        {
            pDmlIAPolicy->ulNextURLInsNum = 1;
        }

        pCosaContext2->hContext         = (ANSC_HANDLE)pURL;
        pCosaContext2->hParentTable     = (ANSC_HANDLE)pDmlIAPolicy;
        pCosaContext2->hPoamIrepUpperFo = pCosaDMSecurity->hIrepFolderIAHA;
        pCosaContext2->bNew             = TRUE;

        CosaSListPushEntryByInsNum(pListHead, (ANSC_HANDLE)pCosaContext2);

        CosaSecurityIARegAddInfo
            (
                (ANSC_HANDLE)pCosaDMSecurity,
                COSA_DML_RR_NAME_IA_URL_NextInsNunmber,
                pDmlIAPolicy->ulNextURLInsNum,
                COSA_DML_RR_NAME_IA_URL_Prefix,
                pDmlIAPolicy->InstanceNumber,
                pURL->Alias,
                (ANSC_HANDLE)pCosaContext2
            );
    }

    *pInsNumber = pCosaContext2->InstanceNumber;

    return pCosaContext2; /* return the handle */
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedURL_DelEntry
            (
                ANSC_HANDLE                 hInsContext,
                ANSC_HANDLE                 hInstance
            );

    description:

        This function is called to delete an exist entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ANSC_HANDLE                 hInstance
                The exist entry handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
BlockedURL_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus     = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity  = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead        = (PSLIST_HEADER            )&pDmlIAPolicy->URLList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2    = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_IA_POLICY_URL         pURL             = (PCOSA_DML_IA_POLICY_URL  )pCosaContext2->hContext;

    if ( pCosaContext2->bNew )
    {
        returnStatus = CosaSecurityIARegDelInfo((ANSC_HANDLE)pCosaDMSecurity, (ANSC_HANDLE)pCosaContext2);
    }
    else
    {
        returnStatus = 
            CosaDmlIaPolicyDelUrl
            (
                NULL, 
                pDmlIAPolicy->InstanceNumber, 
                pURL
                );
    }

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscSListPopEntryByLink(pListHead, &pCosaContext2->Linkage);

        AnscFreeMemory(pURL);
        AnscFreeMemory(pCosaContext2);
    }

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedURL_GetParamBoolValue
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
BlockedURL_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
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
        BlockedURL_GetParamIntValue
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
BlockedURL_GetParamIntValue
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
        BlockedURL_GetParamUlongValue
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
BlockedURL_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedURL_GetParamStringValue
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
BlockedURL_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_URL         pURL             = (PCOSA_DML_IA_POLICY_URL  )pCosaContext->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pURL->Alias);

        return 0;
    }

    if( AnscEqualString(ParamName, "URL", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pURL->Url);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedURL_SetParamBoolValue
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
BlockedURL_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
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
        BlockedURL_SetParamIntValue
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
BlockedURL_SetParamIntValue
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
        BlockedURL_SetParamUlongValue
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
BlockedURL_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
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
        BlockedURL_SetParamStringValue
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
BlockedURL_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_URL         pURL             = (PCOSA_DML_IA_POLICY_URL  )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        AnscCopyString(pURL->Alias, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "URL", TRUE))
    {
        /* collect value */
        AnscCopyString(pURL->Url, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedURL_Validate
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
BlockedURL_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hParentTable;
    PCOSA_DML_IA_POLICY_URL         pURL             = (PCOSA_DML_IA_POLICY_URL  )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_URL         pURLEntry        = (PCOSA_DML_IA_POLICY_URL  )NULL;
    PSLIST_HEADER                   pListHead        = (PSLIST_HEADER            )&pDmlIAPolicy->URLList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2    = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry      = (PSINGLE_LINK_ENTRY       )NULL;

    pSLinkEntry = AnscSListGetFirstEntry(pListHead);

    while ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry   = AnscSListGetNextEntry(pSLinkEntry);

        pURLEntry = (PCOSA_DML_IA_POLICY_URL)pCosaContext2->hContext;

        if ( 
                 pURLEntry && 
                 ((ULONG)pURLEntry != (ULONG)pURL) && 
                 AnscEqualString(pURLEntry->Alias, pURL->Alias, TRUE) 
           )
        {
            AnscCopyString(pReturnParamName, "Alias");

            *puLength = AnscSizeOfString("Alias");
             
            return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedURL_Commit
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
BlockedURL_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus     = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity  = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_URL         pURL             = (PCOSA_DML_IA_POLICY_URL  )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hParentTable;

    if ( pCosaContext->bNew )
    {
        returnStatus = CosaDmlIaPolicyAddUrl(NULL, pDmlIAPolicy->InstanceNumber, pURL);

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCosaContext->bNew = FALSE;

            CosaSecurityIARegDelInfo((ANSC_HANDLE)pCosaDMSecurity, (ANSC_HANDLE)pCosaContext);
        }
    }
    else
    {
        returnStatus = CosaDmlIaPolicySetUrl(NULL, pDmlIAPolicy->InstanceNumber, pURL);
    }

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedURL_Rollback
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
BlockedURL_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_URL         pURL             = (PCOSA_DML_IA_POLICY_URL  )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hParentTable;

    return CosaDmlIaPolicyGetUrlByInsNum
               (
                   NULL, 
                   pDmlIAPolicy->InstanceNumber, 
                   pURL->InstanceNumber, 
                   pURL
               );
}

/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.InternetAccess.AccessPolicy.{i}.BlockedKeyword.{i}.

    *  BlockedKeyword_GetEntryCount
    *  BlockedKeyword_GetEntry
    *  BlockedKeyword_GetParamBoolValue
    *  BlockedKeyword_GetParamIntValue
    *  BlockedKeyword_GetParamUlongValue
    *  BlockedKeyword_GetParamStringValue
    *  BlockedKeyword_SetParamBoolValue
    *  BlockedKeyword_SetParamIntValue
    *  BlockedKeyword_SetParamUlongValue
    *  BlockedKeyword_SetParamStringValue
    *  BlockedKeyword_Validate
    *  BlockedKeyword_Commit
    *  BlockedKeyword_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedKeyword_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
BlockedKeyword_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    return AnscSListQueryDepth(&pDmlIAPolicy->KeywordList);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        BlockedKeyword_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
BlockedKeyword_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext  = (PCOSA_CONTEXT_LINK_OBJECT   )hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy  = (PCOSA_DML_IA_POLICY2        )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead     = (PSLIST_HEADER               )&pDmlIAPolicy->KeywordList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2 = (PCOSA_CONTEXT_LINK_OBJECT   )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry   = (PSINGLE_LINK_ENTRY          )NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(&pDmlIAPolicy->KeywordList, nIndex);

    if ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);

        *pInsNumber = pCosaContext2->InstanceNumber;
    }
    
    return pCosaContext2;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        BlockedKeyword_AddEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to add a new entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle of new added entry.

**********************************************************************/
ANSC_HANDLE
BlockedKeyword_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY   )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = (PCOSA_CONTEXT_LINK_OBJECT  )hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy    = (PCOSA_DML_IA_POLICY2       )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead       = (PSLIST_HEADER              )&pDmlIAPolicy->KeywordList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2   = (PCOSA_CONTEXT_LINK_OBJECT  )NULL;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeyword        = (PCOSA_DML_IA_POLICY_KEYWORD)NULL;

    pKeyword = (PCOSA_DML_IA_POLICY_KEYWORD)AnscAllocateMemory(sizeof(COSA_DML_IA_POLICY_KEYWORD));

    if ( !pKeyword )
    {
        return NULL;
    }
    
    _ansc_sprintf(pKeyword->Alias, "Keyword%d", pDmlIAPolicy->ulNextKeyInsNum);

    /* Update the middle layer cache */
    if ( TRUE )
    {
        pCosaContext2 = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));

        if ( !pCosaContext2 )
        {
            AnscFreeMemory(pKeyword);
            
            return NULL;
        }

        pCosaContext2->InstanceNumber = pKeyword->InstanceNumber = pDmlIAPolicy->ulNextKeyInsNum;

        pDmlIAPolicy->ulNextKeyInsNum++;

        if ( pDmlIAPolicy->ulNextKeyInsNum == 0 )
        {
            pDmlIAPolicy->ulNextKeyInsNum = 1;
        }

        pCosaContext2->hContext         = (ANSC_HANDLE)pKeyword;
        pCosaContext2->hParentTable     = (ANSC_HANDLE)pDmlIAPolicy;
        pCosaContext2->hPoamIrepUpperFo = pCosaDMSecurity->hIrepFolderIAHA;
        pCosaContext2->bNew             = TRUE;

        CosaSListPushEntryByInsNum(pListHead, (ANSC_HANDLE)pCosaContext2);

        CosaSecurityIARegAddInfo
            (
                (ANSC_HANDLE)pCosaDMSecurity,
                COSA_DML_RR_NAME_IA_KEYWORD_NextInsNunmber,
                pDmlIAPolicy->ulNextKeyInsNum,
                COSA_DML_RR_NAME_IA_KEYWORD_Prefix,
                pDmlIAPolicy->InstanceNumber,
                pKeyword->Alias,
                (ANSC_HANDLE)pCosaContext2
            );
    }

    *pInsNumber = pCosaContext2->InstanceNumber;

    return pCosaContext2; /* return the handle */
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Blockedkeyword_DelEntry
            (
                ANSC_HANDLE                 hInsContext,
                ANSC_HANDLE                 hInstance
            );

    description:

        This function is called to delete an exist entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ANSC_HANDLE                 hInstance
                The exist entry handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
BlockedKeyword_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus      = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY    )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext      = (PCOSA_CONTEXT_LINK_OBJECT   )hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy      = (PCOSA_DML_IA_POLICY2        )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead         = (PSLIST_HEADER               )&pDmlIAPolicy->KeywordList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2     = (PCOSA_CONTEXT_LINK_OBJECT   )hInstance;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeyword          = (PCOSA_DML_IA_POLICY_KEYWORD )pCosaContext2->hContext;

    if ( pCosaContext2->bNew )
    {
        returnStatus = CosaSecurityIARegDelInfo((ANSC_HANDLE)pCosaDMSecurity, (ANSC_HANDLE)pCosaContext2);
    }
    else
    {
        returnStatus = CosaDmlIaPolicyDelKeyword(NULL, pDmlIAPolicy->InstanceNumber, pKeyword);
    }

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscSListPopEntryByLink(pListHead, &pCosaContext2->Linkage);

        AnscFreeMemory(pKeyword);
        AnscFreeMemory(pCosaContext2);
    }

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedKeyword_GetParamBoolValue
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
BlockedKeyword_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
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
        BlockedKeyword_GetParamIntValue
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
BlockedKeyword_GetParamIntValue
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
        BlockedKeyword_GetParamUlongValue
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
BlockedKeyword_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedKeyword_GetParamStringValue
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
BlockedKeyword_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT   )hInsContext;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeyword         = (PCOSA_DML_IA_POLICY_KEYWORD )pCosaContext->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pKeyword->Alias);

        return 0;
    }

    if( AnscEqualString(ParamName, "Keyword", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pKeyword->Keyword);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedKeyword_SetParamBoolValue
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
BlockedKeyword_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
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
        BlockedKeyword_SetParamIntValue
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
BlockedKeyword_SetParamIntValue
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
        BlockedKeyword_SetParamUlongValue
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
BlockedKeyword_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
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
        BlockedKeyword_SetParamStringValue
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
BlockedKeyword_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT   )hInsContext;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeyword         = (PCOSA_DML_IA_POLICY_KEYWORD )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        AnscCopyString(pKeyword->Alias, pString);

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Keyword", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pKeyword->Keyword, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedKeyword_Validate
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
BlockedKeyword_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT  )hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2       )pCosaContext->hParentTable;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeyword         = (PCOSA_DML_IA_POLICY_KEYWORD)pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeywordEntry    = (PCOSA_DML_IA_POLICY_KEYWORD)NULL;
    PSLIST_HEADER                   pListHead        = (PSLIST_HEADER              )&pDmlIAPolicy->KeywordList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2    = (PCOSA_CONTEXT_LINK_OBJECT  )NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry      = (PSINGLE_LINK_ENTRY         )NULL;

    pSLinkEntry = AnscSListGetFirstEntry(pListHead);

    while ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry   = AnscSListGetNextEntry(pSLinkEntry);

        pKeywordEntry = (PCOSA_DML_IA_POLICY_KEYWORD)pCosaContext2->hContext;

        if ( 
                 pKeywordEntry && 
                 ((ULONG)pKeywordEntry != (ULONG)pKeyword) && 
                 AnscEqualString(pKeywordEntry->Alias, pKeyword->Alias, TRUE) 
           )
        {
            AnscCopyString(pReturnParamName, "Alias");

            *puLength = AnscSizeOfString("Alias");
             
            return FALSE;
        }
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedKeyword_Commit
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
BlockedKeyword_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus     = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity  = (PCOSA_DATAMODEL_SECURITY    )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT   )hInsContext;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeyword         = (PCOSA_DML_IA_POLICY_KEYWORD )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2        )pCosaContext->hParentTable;
    
    if ( pCosaContext->bNew )
    {
        returnStatus = CosaDmlIaPolicyAddKeyword(NULL, pDmlIAPolicy->InstanceNumber, pKeyword);

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCosaContext->bNew = FALSE;

            CosaSecurityIARegDelInfo ((ANSC_HANDLE)pCosaDMSecurity, (ANSC_HANDLE)pCosaContext);
        }
    }
    else
    {
        returnStatus = CosaDmlIaPolicySetKeyword(NULL, pDmlIAPolicy->InstanceNumber, pKeyword);
    }

    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedKeyword_Rollback
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
BlockedKeyword_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT  )hInsContext;
    PCOSA_DML_IA_POLICY_KEYWORD     pKeyword         = (PCOSA_DML_IA_POLICY_KEYWORD)pCosaContext->hContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2       )pCosaContext->hParentTable;

    return CosaDmlIaPolicyGetKeywordByInsNum
               (
                   NULL, 
                   pDmlIAPolicy->InstanceNumber, 
                   pKeyword->InstanceNumber, 
                   pKeyword
               );
}

/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.InternetAccess.AccessPolicy.{i}.BlockedApplication.{i}.

    *  BlockedApplication_GetEntryCount
    *  BlockedApplication_GetEntry
    *  BlockedApplication_GetParamBoolValue
    *  BlockedApplication_GetParamIntValue
    *  BlockedApplication_GetParamUlongValue
    *  BlockedApplication_GetParamStringValue
    *  BlockedApplication_SetParamBoolValue
    *  BlockedApplication_SetParamIntValue
    *  BlockedApplication_SetParamUlongValue
    *  BlockedApplication_SetParamStringValue
    *  BlockedApplication_Validate
    *  BlockedApplication_Commit
    *  BlockedApplication_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedApplication_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
BlockedApplication_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;

    return AnscSListQueryDepth(&pDmlIAPolicy->AppList);
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        BlockedApplication_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
BlockedApplication_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext  = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy  = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead     = (PSLIST_HEADER            )&pDmlIAPolicy->AppList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2 = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry   = (PSINGLE_LINK_ENTRY       )NULL;

    pSLinkEntry = AnscSListGetEntryByIndex(&pDmlIAPolicy->AppList, nIndex);

    if ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);

        *pInsNumber = pCosaContext2->InstanceNumber;
    }

    return pCosaContext2;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        BlockedApplication_AddEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to add a new entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle of new added entry.

**********************************************************************/
ANSC_HANDLE
BlockedApplication_AddEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext    = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy    = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead       = (PSLIST_HEADER            )&pDmlIAPolicy->AppList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2   = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PCOSA_DML_IA_POLICY_APP         pApp            = (PCOSA_DML_IA_POLICY_APP  )NULL;

    pApp = (PCOSA_DML_IA_POLICY_APP)AnscAllocateMemory(sizeof(COSA_DML_IA_POLICY_APP));

    if ( !pApp )
    {
        return NULL;
    }

    _ansc_sprintf(pApp->Alias, "App%d", pDmlIAPolicy->ulNextAppInsNum);

    /* Update the middle layer cache */
    if ( TRUE )
    {
        pCosaContext2 = (PCOSA_CONTEXT_LINK_OBJECT)AnscAllocateMemory(sizeof(COSA_CONTEXT_LINK_OBJECT));

        if ( !pCosaContext2 )
        {
            AnscFreeMemory(pApp);
            
            return NULL;
        }

        pCosaContext2->InstanceNumber = pApp->InstanceNumber = pDmlIAPolicy->ulNextAppInsNum;

        pDmlIAPolicy->ulNextAppInsNum++;

        if ( pDmlIAPolicy->ulNextAppInsNum == 0 )
        {
            pDmlIAPolicy->ulNextAppInsNum = 1;
        }

        pCosaContext2->hContext         = (ANSC_HANDLE)pApp;
        pCosaContext2->hParentTable     = (ANSC_HANDLE)pDmlIAPolicy;
        pCosaContext2->hPoamIrepUpperFo = pCosaDMSecurity->hIrepFolderIAHA;
        pCosaContext2->bNew             = TRUE;

        CosaSListPushEntryByInsNum(pListHead, (ANSC_HANDLE)pCosaContext2);

        CosaSecurityIARegAddInfo
            (
                (ANSC_HANDLE)pCosaDMSecurity,
                COSA_DML_RR_NAME_IA_APP_NextInsNunmber,
                pDmlIAPolicy->ulNextAppInsNum,
                COSA_DML_RR_NAME_IA_APP_Prefix,
                pDmlIAPolicy->InstanceNumber,
                pApp->Alias,
                (ANSC_HANDLE)pCosaContext2
            );
    }

    *pInsNumber = pCosaContext2->InstanceNumber;

    return pCosaContext2; /* return the handle */
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedApplication_DelEntry
            (
                ANSC_HANDLE                 hInsContext,
                ANSC_HANDLE                 hInstance
            );

    description:

        This function is called to delete an exist entry.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ANSC_HANDLE                 hInstance
                The exist entry handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
BlockedApplication_DelEntry
    (
        ANSC_HANDLE                 hInsContext,
        ANSC_HANDLE                 hInstance
    )
{
    ANSC_STATUS                     returnStatus     = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity  = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hContext;
    PSLIST_HEADER                   pListHead        = (PSLIST_HEADER            )&pDmlIAPolicy->AppList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2    = (PCOSA_CONTEXT_LINK_OBJECT)hInstance;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext2->hContext;

    if ( pCosaContext2->bNew )
    {
        CosaSecurityIARegDelInfo((ANSC_HANDLE)pCosaDMSecurity, (ANSC_HANDLE)pCosaContext2);
    }
    else
    {
        returnStatus = CosaDmlIaPolicyDelBlockedApp(NULL, pDmlIAPolicy->InstanceNumber, pApp);
    }

    if ( returnStatus == ANSC_STATUS_SUCCESS )
    {
        AnscSListPopEntryByLink(pListHead, &pCosaContext2->Linkage);

        AnscFreeMemory(pApp);
        AnscFreeMemory(pCosaContext2);
    }
    
    return returnStatus; /* succeeded */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedApplication_GetParamBoolValue
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
BlockedApplication_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "IsWellKnown", TRUE))
    {
        /* collect value */
        *pBool = pApp->IsWellKnown;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedApplication_GetParamIntValue
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
BlockedApplication_GetParamIntValue
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
        BlockedApplication_GetParamUlongValue
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
BlockedApplication_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "EndPortNumber", TRUE))
    {
        /* collect value */
        *puLong = pApp->EndPortNumber;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "StartPortNumber", TRUE))
    {
        /* collect value */
        *puLong = pApp->StartPortNumber;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Protocol", TRUE))
    {
        /* collect value */
        *puLong = pApp->Protocol;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedApplication_GetParamStringValue
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
BlockedApplication_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;

    /* check the parameter name and return the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pApp->Alias);

        return 0;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedApplication_SetParamBoolValue
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
BlockedApplication_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "IsWellKnown", TRUE))
    {
        /* save update to backup */
#ifdef _COSA_DRG_CNS_
        #if 0
        /*we only support limited well known services in backend*/
        if (!isWellKnownService(pApp->Alias) && bValue)
            return FALSE;
        else if (isWellKnownService(pApp->Alias) && !bValue)
            return FALSE;
        #endif
        return FALSE;
        
#endif

        pApp->IsWellKnown = bValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedApplication_SetParamIntValue
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
BlockedApplication_SetParamIntValue
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
        BlockedApplication_SetParamUlongValue
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
BlockedApplication_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "EndPortNumber", TRUE))
    {
        /* save update to backup */

        pApp->EndPortNumber = (USHORT)uValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "StartPortNumber", TRUE))
    {
        /* save update to backup */
        pApp->StartPortNumber = (USHORT)uValue;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Protocol", TRUE))
    {
        /* save update to backup */
        pApp->Protocol = uValue;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedApplication_SetParamStringValue
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
BlockedApplication_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;

    /* check the parameter name and set the corresponding value */
    if( AnscEqualString(ParamName, "Alias", TRUE))
    {
        /* save update to backup */
        AnscCopyString(pApp->Alias, pString);

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        BlockedApplication_Validate
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
BlockedApplication_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hParentTable;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY_APP         pAppEntry        = (PCOSA_DML_IA_POLICY_APP  )NULL;
    PSLIST_HEADER                   pListHead        = (PSLIST_HEADER            )&pDmlIAPolicy->AppList;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext2    = (PCOSA_CONTEXT_LINK_OBJECT)NULL;
    PSINGLE_LINK_ENTRY              pSLinkEntry      = (PSINGLE_LINK_ENTRY       )NULL;

    pSLinkEntry = AnscSListGetFirstEntry(pListHead);

    while ( pSLinkEntry )
    {
        pCosaContext2 = ACCESS_COSA_CONTEXT_LINK_OBJECT(pSLinkEntry);
        pSLinkEntry   = AnscSListGetNextEntry(pSLinkEntry);

        pAppEntry = (PCOSA_DML_IA_POLICY_APP)pCosaContext2->hContext;

        if ( 
                 pAppEntry && 
                 ((ULONG)pAppEntry != (ULONG)pApp) && 
                 AnscEqualString(pAppEntry->Alias, pApp->Alias, TRUE) 
           )
        {
            AnscCopyString(pReturnParamName, "Alias");

            *puLength = AnscSizeOfString("Alias");
             
            return FALSE;
        }
    }

    if ( pApp->StartPortNumber > pApp->EndPortNumber )
    {
        AnscCopyString(pReturnParamName, "EndPortNumber");

        *puLength = AnscSizeOfString(pReturnParamName);

        return FALSE;
    }

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedApplication_Commit
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
BlockedApplication_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    ANSC_STATUS                     returnStatus     = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity  = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hParentTable;

    if ( pCosaContext->bNew )
    {
        returnStatus = CosaDmlIaPolicyAddApp(NULL, pDmlIAPolicy->InstanceNumber, pApp);

        if ( returnStatus == ANSC_STATUS_SUCCESS )
        {
            pCosaContext->bNew = FALSE;
        
            CosaSecurityIARegDelInfo((ANSC_HANDLE)pCosaDMSecurity, (ANSC_HANDLE)pCosaContext);
        }
    }
    else
    {
        returnStatus = CosaDmlIaPolicySetBlockedApp(NULL, pDmlIAPolicy->InstanceNumber, pApp);
    }
    
    return returnStatus;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        BlockedApplication_Rollback
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
BlockedApplication_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_CONTEXT_LINK_OBJECT       pCosaContext     = (PCOSA_CONTEXT_LINK_OBJECT)hInsContext;
    PCOSA_DML_IA_POLICY_APP         pApp             = (PCOSA_DML_IA_POLICY_APP  )pCosaContext->hContext;
    PCOSA_DML_IA_POLICY2            pDmlIAPolicy     = (PCOSA_DML_IA_POLICY2     )pCosaContext->hParentTable;

    return CosaDmlIaPolicyGetAppByInsNum
               (
                   NULL, 
                   pDmlIAPolicy->InstanceNumber, 
                   pApp->InstanceNumber, 
                   pApp
               );
}

/***********************************************************************

 APIs for Object:

    X_CISCO_COM_Security.InternetAccess.LogEntry.{i}.

    *  LogEntry_GetEntryCount
    *  LogEntry_GetEntry
    *  LogEntry_IsUpdated
    *  LogEntry_Synchronize
    *  LogEntry_GetParamBoolValue
    *  LogEntry_GetParamIntValue
    *  LogEntry_GetParamUlongValue
    *  LogEntry_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        LogEntry_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
LogEntry_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;

    return pCosaDMSecurity->ulLogEntryCount;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        LogEntry_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
LogEntry_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    PCOSA_DML_IA_LOG_ENTRY          pLogBlock         = (PCOSA_DML_IA_LOG_ENTRY   )pCosaDMSecurity->pLog;
    PCOSA_DML_IA_LOG_ENTRY          pLogEntry         = (PCOSA_DML_IA_LOG_ENTRY   )NULL;

    pLogEntry = (PCOSA_DML_IA_LOG_ENTRY)((PCOSA_DML_IA_LOG_ENTRY)pLogBlock + nIndex);

    *pInsNumber  = nIndex + 1;

    return pLogEntry; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LogEntry_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
static ULONG last_tick;
#define REFRESH_INTERVAL 120
#define TIME_NO_NEGATIVE(x) ((long)(x) < 0 ? 0 : (x))

BOOL
LogEntry_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    if ( !last_tick ) 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
    
    if ( last_tick >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - REFRESH_INTERVAL) )
    {
        return FALSE;
    }
    else 
    {
        last_tick = AnscGetTickInSeconds();

        return TRUE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        LogEntry_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
LogEntry_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_SECURITY        pCosaDMSecurity   = (PCOSA_DATAMODEL_SECURITY )g_pCosaBEManager->hSecurity;
    ULONG                           ulEntryCount      = 0;

    if ( pCosaDMSecurity->pLog )
    {
        AnscFreeMemory(pCosaDMSecurity->pLog);
    }

    pCosaDMSecurity->pLog = CosaDmlIaGetLogEntries(NULL, &ulEntryCount);

    pCosaDMSecurity->ulLogEntryCount = ulEntryCount;

    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        LogEntry_GetParamBoolValue
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
LogEntry_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
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
        LogEntry_GetParamIntValue
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
LogEntry_GetParamIntValue
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
        LogEntry_GetParamUlongValue
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
LogEntry_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DML_IA_LOG_ENTRY          pLogEntry         = (PCOSA_DML_IA_LOG_ENTRY   )hInsContext;

    /* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "SourceIP", TRUE))
    {
        /* collect value */
        *puLong = pLogEntry->SourceIP.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "TargetIP", TRUE))
    {
        /* collect value */
        *puLong = pLogEntry->TargetIP.Value;

        return TRUE;
    }

    if( AnscEqualString(ParamName, "Count", TRUE))
    {
        /* collect value */
        *puLong = pLogEntry->Count;

        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        LogEntry_GetParamStringValue
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
LogEntry_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_DML_IA_LOG_ENTRY          pLogEntry         = (PCOSA_DML_IA_LOG_ENTRY   )hInsContext;

    /* check the parameter name and return the corresponding value */

    if( AnscEqualString(ParamName, "OccuranceTime", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pLogEntry->OccuranceTime);

        return 0;
    }

    if( AnscEqualString(ParamName, "User", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pLogEntry->User);

        return 0;
    }

    if( AnscEqualString(ParamName, "Action", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pLogEntry->Action);

        return 0;
    }

    if( AnscEqualString(ParamName, "Description", TRUE))
    {
        /* collect value */
        AnscCopyString(pValue, pLogEntry->Description);

        return 0;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

