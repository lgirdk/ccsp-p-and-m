/*********************************************************************************
 * Copyright 2023 Liberty Global B.V.
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
#ifdef FEATURE_STATIC_IPV4

#include "cosa_lgi_staticip_apis.h"
#include "cosa_lgi_staticip_internal.h"
#include "cosa_routing_apis.h"
#include <syscfg/syscfg.h>
#include <ifaddrs.h>

extern ANSC_HANDLE bus_handle;

static void Get_InterfaceStatus(ULONG* puLong)
{
    char buffer[8];
    ULONG hasIPv4 = 0;
    struct ifaddrs *ifaddr, *ifa;
    char *if_name = NULL;

    //Initialize to 1 - Down
    *puLong = 1;

    syscfg_get(NULL, "staticipadminstatus", buffer, sizeof(buffer));
    if (strcmp(buffer, "2") == 0)
    {
        if_name = "erouter0";
    }
    else if (strcmp(buffer, "3") == 0)
    {
        if_name = "brlan0";
    }

    if (if_name != NULL )    
    {
        if (getifaddrs(&ifaddr) == 0)
        {
            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
            {
                if (strcmp(ifa->ifa_name, if_name) == 0)
                {
                    if (ifa->ifa_addr == NULL)
                    {
                        break;
                    }

                    if (ifa->ifa_addr->sa_family == AF_INET)
                    {
                        hasIPv4 = 1;
                    }
                }
            }

            if (hasIPv4 == 1)
            {
                *puLong = 2;
            }
            else
            {
                *puLong = 3;
            }

            freeifaddrs(ifaddr);
        }
    }

}

/* Tis function is used to trigger wan refresh when switching between the Static IPv4 service */
static void *WanRefresh (void)
{
    char* faultParam = NULL;
    int ret = 0;
    CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
    pthread_detach(pthread_self());

    parameterValStruct_t param_val[] = {  { "Device.X_RDK_WanManager.CPEInterface.2.Wan.Refresh", "true", ccsp_boolean} };


    ret = CcspBaseIf_setParameterValues(bus_handle,
                                        "eRT.com.cisco.spvtg.ccsp.wanmanager",
                                        "/com/cisco/spvtg/ccsp/wanmanager",
                                        0,
                                        0,
                                        param_val,
                                        1,
                                        TRUE,
                                        &faultParam
                                        );
    if (ret != CCSP_SUCCESS && faultParam)
    {
        CcspTraceError(("%s: Failed to SetValue for param '%s'\n",__FUNCTION__,faultParam));
        bus_info->freefunc(faultParam);
    }
    return NULL;
}

ANSC_STATUS CosaDmlStaticIPGetOperationalStatus ( ANSC_HANDLE hContext, ULONG *pValue )
{
    Get_InterfaceStatus(pValue);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlStaticIPGetAdministrativeStatus ( ANSC_HANDLE hContext, ULONG *pValue )
{
    char buf[8];

    syscfg_get(NULL, "staticipadminstatus", buf, sizeof(buf));
    *pValue = atoi(buf);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlStaticIPSetAdministrativeStatus ( ANSC_HANDLE hContext, ULONG bValue )
{
    char staticIpAdministrativeStatus[8];

    syscfg_get(NULL, "staticipadminstatus", staticIpAdministrativeStatus, sizeof(staticIpAdministrativeStatus));

    if (atoi(staticIpAdministrativeStatus) == bValue)
    {
        return ANSC_STATUS_SUCCESS;
    }

    pthread_t WanRefresh_thread;	
    if (syscfg_set_u(NULL, "staticipadminstatus", bValue) != 0)
    {	    
        return ANSC_STATUS_FAILURE;
    }

    RestartRIPInterfaces(FALSE);    
    pthread_create(&WanRefresh_thread, NULL, WanRefresh, NULL);

    return ANSC_STATUS_SUCCESS;
}

#endif
