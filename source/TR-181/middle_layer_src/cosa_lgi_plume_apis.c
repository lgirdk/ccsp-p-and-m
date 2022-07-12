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

#ifdef _PUMA6_ARM_
#define BACKHAUL_2G_IDX "13"
#define BACKHAUL_5G_IDX "14"
#elif defined (_LG_MV2_PLUS_)
#define BACKHAUL_2G_IDX "3"
#define BACKHAUL_5G_IDX "4"
#else
#define BACKHAUL_2G_IDX "13"
#define BACKHAUL_5G_IDX "14"
#endif

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

static ULONG alloc_wiFiDataPaths(PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths, UINT size)
{
    parameterValStruct_t *pNewParamVals;

    pNewParamVals = realloc((pWiFiDataPaths->pParamVals), (pWiFiDataPaths->currIdx + size) * sizeof(parameterValStruct_t));
    if (pNewParamVals == NULL)
    {
        CcspTraceError(("%s:%d Failed to resize WiFi data path array\n", __func__, __LINE__));
        return ANSC_STATUS_FAILURE;
    }
    pWiFiDataPaths->pParamVals = pNewParamVals;

    return ANSC_STATUS_SUCCESS;
}

static void free_wiFiDataPaths(void *bus_handle, PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths)
{
    int i;
    int size;
    CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;

    if (pWiFiDataPaths)
    {
        size = pWiFiDataPaths->currIdx;
        for (i = 0; i < size; i++)
        {
            parameterValStruct_t *pParamVal = pWiFiDataPaths->pParamVals + i;
            if (pParamVal)
            {
                if (pParamVal->parameterName)
                    free(pParamVal->parameterName);
                if (pParamVal->parameterValue)
                    free(pParamVal->parameterValue);
            }
        }
        if (pWiFiDataPaths->pParamVals)
            free(pWiFiDataPaths->pParamVals);

        free(pWiFiDataPaths);
    }
}

static void add_wiFiDataPaths(PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths, const char *name, const char *val, const enum dataType_e type)
{
    parameterValStruct_t *pParamVals = pWiFiDataPaths->pParamVals;

    pParamVals[pWiFiDataPaths->currIdx].parameterName = strdup(name);
    pParamVals[pWiFiDataPaths->currIdx].parameterValue = strdup(val);
    pParamVals[pWiFiDataPaths->currIdx].type = type;
    pWiFiDataPaths->currIdx++;
}

static void apply_wiFiDataPaths(void *arg)
{
    char *faultParam = NULL;
    int ret = 0;
    CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
    PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths = (PCOSA_LGI_PLUME_DATAPATHS)arg;

    pthread_detach(pthread_self());

    if (ppComponents == NULL && initWifiComp())
    {
        CcspTraceError(("initWifiComp error ...\n", __FUNCTION__));
        goto end;
    }

    if ((pWiFiDataPaths->applyToRadio) & (1 << RADIO_2G_IDX))
    {
        if (alloc_wiFiDataPaths(pWiFiDataPaths, 1) != ANSC_STATUS_SUCCESS)
        {
            goto end;
        }
        add_wiFiDataPaths(pWiFiDataPaths, "Device.WiFi.Radio.1.X_CISCO_COM_ApplySetting", "true", ccsp_boolean);
    }
    if ((pWiFiDataPaths->applyToRadio) & (1 << RADIO_5G_IDX))
    {

        if (alloc_wiFiDataPaths(pWiFiDataPaths, 1) != ANSC_STATUS_SUCCESS)
        {
            goto end;
        }
        add_wiFiDataPaths(pWiFiDataPaths, "Device.WiFi.Radio.2.X_CISCO_COM_ApplySetting", "true", ccsp_boolean);
    }

    ret = CcspBaseIf_setParameterValues(bus_handle,
                                        ppComponents[0]->componentName,
                                        ppComponents[0]->dbusPath,
                                        0,
                                        0,
                                        pWiFiDataPaths->pParamVals,
                                        pWiFiDataPaths->currIdx,
                                        TRUE,
                                        &faultParam
                                        );
    if(ret != CCSP_SUCCESS && faultParam)
    {
        CcspTraceError(("%s: Disable Native BS - Failed to SetValue for param '%s'\n",__FUNCTION__,faultParam));
        bus_info->freefunc(faultParam);
    }
end:
    free_wiFiDataPaths(bus_handle, pWiFiDataPaths);
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
#ifdef _PUMA6_ARM_
        char rpc_cmd[80];
        snprintf(rpc_cmd, sizeof(rpc_cmd), "rpcclient2 'echo %s > /tmp/.syscfg_son_admin_status'", value ? "1" : "0");
        system(rpc_cmd);
#endif
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
#ifdef _PUMA6_ARM_
        char rpc_cmd[80];
        snprintf(rpc_cmd, sizeof(rpc_cmd), "rpcclient2 'echo %s > /tmp/.syscfg_son_operational_status'", value ? "1" : "0");
        system(rpc_cmd);
#endif
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
#ifdef _PUMA6_ARM_
        char rpc_cmd[80];
        snprintf(rpc_cmd, sizeof(rpc_cmd), "rpcclient2 'echo %s > /tmp/.syscfg_son_dfs_enable'", value ? "1" : "0");
        system(rpc_cmd);
#endif
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

BOOL CosaDmlSetPlumeNativeAtmBsControl ( PANSC_HANDLE phContext, BOOL value )
{
    /* TODO: Check if Plume channel optimisation is enabled first */
    int i,size=0;
    BOOL enable = !value;
    int ret = 0, val_size = 0;
    char *parameterNames[3];
    parameterValStruct_t **parameterval = NULL;

    PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths = (PCOSA_LGI_PLUME_DATAPATHS) phContext;

    if (enable)
    {
        goto end;
    }

    if ((ppComponents == NULL) && initWifiComp())
    {
        CcspTraceError(("%s: initWifiComp error ...\n", __FUNCTION__));
        return FALSE;
    }

    parameterNames[0] = "Device.WiFi.X_LGI-COM_ATM.Radio.1.Enable";
    parameterNames[1] = "Device.WiFi.X_LGI-COM_ATM.Radio.2.Enable";
    parameterNames[2] = "Device.WiFi.X_LGI-COM_BandSteering.SSID.1.Enable";

    ret = CcspBaseIf_getParameterValues(bus_handle,
                                        ppComponents[0]->componentName,
                                        ppComponents[0]->dbusPath,
                                        parameterNames,
                                        3,
                                        &val_size,
                                        &parameterval
                                        );

    if ((ret == CCSP_SUCCESS) && (val_size == 3))
    {
        if(!strcmp(parameterval[0]->parameterValue, "false") &&
            !strcmp(parameterval[1]->parameterValue, "false") &&
            !strcmp(parameterval[2]->parameterValue, "false"))
            {
                free_parameterValStruct_t (bus_handle, val_size, parameterval);
                goto end;
            }
    }

    if (parameterval != NULL)
    {
        free_parameterValStruct_t (bus_handle, val_size, parameterval);
    }

    if (alloc_wiFiDataPaths(pWiFiDataPaths, 3) != ANSC_STATUS_SUCCESS)
    {
        CcspTraceError(("%s:%d Failed to resize WiFi data path array\n", __func__, __LINE__));
        return FALSE;
    }
    add_wiFiDataPaths(pWiFiDataPaths, "Device.WiFi.X_LGI-COM_ATM.Radio.1.Enable", enable ? "true" : "false", ccsp_boolean);
    add_wiFiDataPaths(pWiFiDataPaths, "Device.WiFi.X_LGI-COM_ATM.Radio.2.Enable", enable ? "true" : "false", ccsp_boolean);
    add_wiFiDataPaths(pWiFiDataPaths, "Device.WiFi.X_LGI-COM_BandSteering.SSID.1.Enable", enable ? "true" : "false", ccsp_boolean);
    pWiFiDataPaths->applyToRadio |= 1 << RADIO_2G_IDX;
    pWiFiDataPaths->applyToRadio |= 1 << RADIO_5G_IDX;

end:
    if (syscfg_set_commit(NULL, "son_native_atm_bs_disable", value ? "1" : "0") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlGetPlumeLogpullEnable ( ANSC_HANDLE hContext, BOOL *pValue )
{
    char buf[8];

    syscfg_get (NULL, "son_logpull_enable", buf, sizeof(buf));

    *pValue = (strcmp (buf, "1") == 0);

    return TRUE;
}

BOOL CosaDmlSetPlumeLogpullEnable ( ANSC_HANDLE hContext, BOOL value )
{
#ifdef _PUMA6_ARM_
    if (value)
    {
        // Start icu if not already running
        system("pidof -x /usr/sbin/icu > /dev/null || /usr/sbin/icu -R -B -p 192.168.254.253:2222");
    }
    else
    {
        char value[8];

        // icu should only be stopped if both Plume logpull and telemetry are disabled
        if (syscfg_get(NULL, "T2Enable", value, sizeof(value)) == 0)
        {
            if (strcmp(value, "true") != 0)
            {
                system("killall -9 icu");
            }
        }
    }
#endif

    if (syscfg_set_commit (NULL, "son_logpull_enable", value ? "1" : "0") == 0)
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CosaDmlGetPlumeMeshEnabled ( ANSC_HANDLE hContext, BOOL *pValue )
{
    int ret = 0, val_size = 0;
    char *parameterNames[2];
    parameterValStruct_t **parameterval = NULL;

    *pValue = FALSE;

    if ((ppComponents == NULL) && initWifiComp())
    {
        CcspTraceError(("%s: initWifiComp error ...\n", __FUNCTION__));
        return FALSE;
    }

    parameterNames[0] = "Device.WiFi.SSID." BACKHAUL_2G_IDX ".Enable";
    parameterNames[1] = "Device.WiFi.SSID." BACKHAUL_5G_IDX ".Enable";

    ret = CcspBaseIf_getParameterValues(bus_handle,
                                        ppComponents[0]->componentName,
                                        ppComponents[0]->dbusPath,
                                        parameterNames,
                                        2,
                                        &val_size,
                                        &parameterval
                                        );

    if ((ret == CCSP_SUCCESS) && (val_size == 2))
    {
        /* If both backhaul SSIDs are disabled, then mesh is disabled. Enabled in every other case */
        *pValue = (!strcmp(parameterval[0]->parameterValue, "false") && !strcmp(parameterval[1]->parameterValue, "false") ? FALSE : TRUE);
        free_parameterValStruct_t (bus_handle, val_size, parameterval);
        return TRUE;
    }

    free_parameterValStruct_t (bus_handle, val_size, parameterval);

    return FALSE;
}

ULONG CosaDmlSetPlumeBackhaulSSIDsState ( PANSC_HANDLE phContext, BOOL value )
{
    BOOL enable = value;
    PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths = (PCOSA_LGI_PLUME_DATAPATHS) phContext;

    if (alloc_wiFiDataPaths(pWiFiDataPaths, 2) != ANSC_STATUS_SUCCESS)
    {
        CcspTraceError(("%s:%d Failed to resize WiFi data path array\n", __func__, __LINE__));
        return FALSE;
    }

    add_wiFiDataPaths(pWiFiDataPaths, "Device.WiFi.SSID." BACKHAUL_2G_IDX ".Enable", enable ? "true" : "false", ccsp_boolean);
    add_wiFiDataPaths(pWiFiDataPaths, "Device.WiFi.SSID." BACKHAUL_5G_IDX ".Enable", enable ? "true" : "false", ccsp_boolean);

    pWiFiDataPaths->applyToRadio |= 1 << RADIO_2G_IDX;
    pWiFiDataPaths->applyToRadio |= 1 << RADIO_5G_IDX;

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlSetRRMState ( PANSC_HANDLE phContext, BOOL value )
{
    BOOL enable = value;
    PCOSA_LGI_PLUME_DATAPATHS pWiFiDataPaths = (PCOSA_LGI_PLUME_DATAPATHS) phContext;

    if (alloc_wiFiDataPaths(pWiFiDataPaths, 2) != ANSC_STATUS_SUCCESS)
    {
        CcspTraceError(("%s:%d Failed to resize WiFi data path array\n", __func__, __LINE__));
        return FALSE;
    }
    add_wiFiDataPaths(pWiFiDataPaths,
                      "Device.WiFi.AccessPoint.1.X_RDKCENTRAL-COM_NeighborReportActivated",
                      enable ? "true" : "false",
                      ccsp_boolean);
    add_wiFiDataPaths(pWiFiDataPaths,
                      "Device.WiFi.AccessPoint.2.X_RDKCENTRAL-COM_NeighborReportActivated",
                      enable ? "true" : "false",
                      ccsp_boolean);
    pWiFiDataPaths->applyToRadio |= 1 << RADIO_2G_IDX;
    pWiFiDataPaths->applyToRadio |= 1 << RADIO_5G_IDX;

    return ANSC_STATUS_SUCCESS;
}

ULONG CosaDmlApplyPlumeWiFiChanges ( PANSC_HANDLE phContext )
{
    pthread_t tid;

    pthread_create(&tid, NULL, (void *)apply_wiFiDataPaths, phContext);
}
