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
#include "cosa_lgi_applications_internal.h"
#include "cosa_lgi_applications_apis.h"
#include <syscfg/syscfg.h>
#include "cosa_drg_common.h"

static char *propertynames[] = {
    "none",
};

ULONG CosaDmlApplicationsSamKnowsGetEnabled ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[8];

    syscfg_get (NULL, "skenable", buf, sizeof(buf));

    *pValue = (strcmp (buf, "0") == 0) ? 0 : 1;

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlApplicationsSamKnowsSetEnabled ( ANSC_HANDLE hContext, BOOL bValue )
{
    BOOL skenabled = 0;

    CosaDmlApplicationsSamKnowsGetEnabled (NULL, &skenabled);

    if (bValue == skenabled)
    {
        return ANSC_STATUS_SUCCESS;
    }

    syscfg_set_commit (NULL, "skenable", bValue ? "1" : "0");

#ifdef _PUMA6_ARM_
    if (bValue == TRUE) {
        system ("rpcclient2 'sh /etc/init.d/skclient.sh start >/dev/null &'");
    } else {
        system ("rpcclient2 'sh /etc/init.d/skclient.sh stop >/dev/null &'");
    }
#else
    if (bValue == TRUE) {
        CcspTraceInfo(("%s : Starting Samknows\n", __FUNCTION__));    
        system ("/etc/init.d/samknows_ispmon start &");
    } else {
        system ("/etc/init.d/samknows_ispmon stop &");
    }
#endif

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlApplicationsSamKnowsGetProperty ( ANSC_HANDLE hContext, char *pValue, ULONG *pUlSize )
{
    char buf[SAMKNOWS_PROPERTY_STRING_LEN];

    syscfg_get (NULL, "skproperty", buf, sizeof(buf));

    if (AnscSizeOfString (buf) < *pUlSize)
    {
        AnscCopyString (pValue, buf);
        return ANSC_STATUS_SUCCESS;
    }
    else
    {
        *pUlSize = AnscSizeOfString (buf);
    }

    return ANSC_STATUS_FAILURE;
}

ULONG CosaDmlApplicationsSamKnowsSetProperty ( ANSC_HANDLE hContext, char *pValue )
{
    syscfg_set_commit(NULL, "skproperty", pValue);
#ifdef _PUMA6_ARM_
    v_secure_system("rpcclient2 'echo \"%s\" > /tmp/sk_property.txt &'", pValue);
#endif

    return ANSC_STATUS_SUCCESS;
}

BOOL CosaDmlApplicationsSamKnowsValidateProperty ( ANSC_HANDLE hContext, char *pValue )
{
    int i;

    for (i = 0; i < sizeof(propertynames)/sizeof(propertynames[0]); i++)
    {
        if (strcmp (pValue, propertynames[i]) == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}
