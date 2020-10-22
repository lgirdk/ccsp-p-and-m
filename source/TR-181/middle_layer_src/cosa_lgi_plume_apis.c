/*********************************************************************
 * Copyright 2019 ARRIS Enterprises, LLC.
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

#include <pthread.h>
#include "cosa_lgi_plume_apis.h"
#include <syscfg/syscfg.h>
#include "cosa_lgi_plume_internal.h"

extern ANSC_HANDLE bus_handle;
static componentStruct_t **ppComponents = NULL;
extern char g_Subsystem[32];

static int initWifiComp (void)
{
    int size = 0, ret;
    char dst_pathname_cr[64];

    snprintf(dst_pathname_cr, sizeof(dst_pathname_cr), "%s%s", g_Subsystem, CCSP_DBUS_INTERFACE_CR);

    ret = CcspBaseIf_discComponentSupportingNamespace(bus_handle,
                dst_pathname_cr,
                "Device.WiFi.",
                g_Subsystem,        /* prefix */
                &ppComponents,
                &size);

    return (ret == CCSP_SUCCESS) ? 0 : ret;
}

ANSC_STATUS CosaDmlGetPlumeUrl ( ANSC_HANDLE hContext, char *pValue, ULONG *pUlSize )
{
    char buf[URL_LEN];

    if (pValue) {
        if (syscfg_get(NULL, "son_url", buf, URL_LEN) == 0) {
            if (strlen(buf) < *pUlSize) {
                strcpy(pValue, buf);
                return ANSC_STATUS_SUCCESS;
            }
        }
        strcpy(pValue, "");
        return ANSC_STATUS_SUCCESS;
    }

    return ANSC_STATUS_FAILURE;
}

ULONG CosaDmlSetPlumeUrl ( ANSC_HANDLE hContext, char *pValue )
{
    syscfg_set_commit(NULL, "son_url", pValue);

    return ANSC_STATUS_SUCCESS;
}

BOOL CosaDmlGetPlumeAdminStatus ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[64];

    if (pValue) {
        syscfg_get(NULL, "son_admin_status", buf, sizeof(buf));
        *pValue = (strcmp(buf, "1") == 0);
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlSetPlumeAdminStatus ( ANSC_HANDLE hContext, BOOL value )
{
    if (syscfg_set_commit(NULL, "son_admin_status", value ? "1" : "0") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlGetPlumeOperationalStatus ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[64];

    if (pValue) {
        syscfg_get(NULL, "son_operational_status", buf, sizeof(buf));
        *pValue = (strcmp(buf, "1") == 0);
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlSetPlumeOperationalStatus ( ANSC_HANDLE hContext, BOOL value )
{
    if (syscfg_set_commit(NULL, "son_operational_status", value ? "1" : "0") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlGetPlumeDFSEnable ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[64];

    if (pValue) {
        syscfg_get(NULL, "son_dfs_enable", buf, sizeof(buf));
        *pValue = (strcmp(buf, "1") == 0);
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlSetPlumeDFSEnable ( ANSC_HANDLE hContext, BOOL value )
{
    if (syscfg_set_commit(NULL, "son_dfs_enable", value ? "1" : "0") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlGetPlumeNativeAtmBsControl ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[64];

    if (pValue) {
        syscfg_get(NULL, "son_native_atm_bs_disable", buf, sizeof(buf));
        *pValue = (strcmp(buf, "1") == 0);
        return TRUE;
    }

    return FALSE;
}

static void updateNativeATMAndBS(BOOL enable)
{
    char *faultParam = NULL;
    int ret = 0;

    CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
    parameterValStruct_t param_val[] = {  { "Device.WiFi.X_LGI-COM_ATM.Radio.1.Enable", "false", ccsp_boolean},
                                          { "Device.WiFi.X_LGI-COM_ATM.Radio.2.Enable", "false", ccsp_boolean},
                                          { "Device.WiFi.X_LGI-COM_BandSteering.SSID.1.Enable", "false", ccsp_boolean},
                                          { "Device.WiFi.Radio.1.X_CISCO_COM_ApplySetting", "true", ccsp_boolean},
                                          { "Device.WiFi.Radio.2.X_CISCO_COM_ApplySetting", "true", ccsp_boolean} };
    if (enable) {
        param_val[0].parameterValue = "true";
        param_val[1].parameterValue = "true";
        param_val[2].parameterValue = "true";
    }

    ret = CcspBaseIf_setParameterValues(bus_handle,
                                        ppComponents[0]->componentName,
                                        ppComponents[0]->dbusPath,
                                        0,
                                        0,
                                        param_val,
                                        sizeof(param_val)/sizeof(param_val[0]),
                                        TRUE,
                                        &faultParam
                                        );

    if (ret != CCSP_SUCCESS && faultParam)
    {
        CcspTraceError(("%s: Disable Native BS - Failed to SetValue for param '%s'\n",__FUNCTION__,faultParam));
        bus_info->freefunc(faultParam);
    }
}

ANSC_STATUS updateNativeAtmAndBs(void *arg)
{
    int value = (int) arg;
    pthread_detach(pthread_self());
    updateNativeATMAndBS(!value);
}

BOOL CosaDmlSetPlumeNativeAtmBsControl ( ANSC_HANDLE hContext, BOOL value )
{
    pthread_t tid;
    /* TODO: Check if Plume channel optimisation is enabled first */
    if (ppComponents == NULL && initWifiComp())
    {
        CcspTraceError(("initWifiComp error ...\n",__FUNCTION__));
        return TRUE;
    }

    pthread_create(&tid, NULL, updateNativeAtmAndBs, value);

    if (syscfg_set_commit(NULL, "son_native_atm_bs_disable", value ? "1" : "0") == 0)
    {
        return TRUE;
    }

    return FALSE;
}
