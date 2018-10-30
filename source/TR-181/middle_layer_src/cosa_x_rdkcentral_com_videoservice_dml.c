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

#include "cosa_x_rdkcentral_com_videoservice_dml.h"
#include "dml_tr181_custom_cfg.h"
#include "ccsp_trace.h"

BOOL videoServiceEnable = FALSE;
BOOL videoServiceEnableInProgress = FALSE;
pthread_mutex_t     g_videoservice_mutex = PTHREAD_MUTEX_INITIALIZER;
extern void* g_pDslhDmlAgent;
extern ANSC_HANDLE bus_handle;

void SetVideoServiceConfig();

BOOL
VideoService_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{

    if (AnscEqualString(ParamName, "Enabled", TRUE))
    {
        char buf[5] = {0};
        syscfg_get( NULL, "X_RDKCENTRAL-COM_VIDEOSERVICE", buf, sizeof(buf));
        if( buf != NULL )
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

    return FALSE;
}

BOOL
VideoService_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
  
    if (AnscEqualString(ParamName, "Enabled", TRUE))
    {   
        char bval[2] = {0};
        if( bValue == TRUE )
        {
            char buf[5] = {0};
            syscfg_get( NULL, "X_RDKCENTRAL-COM_VIDEOSERVICE", buf, sizeof(buf));
            if( buf != NULL )
            {
                    if (strcmp(buf,"1") == 0)
                    {
                        return TRUE;
                    }
            }

            if(videoServiceEnableInProgress==FALSE)
            {         
                bval[0] = '1';

                pthread_mutex_lock(&g_videoservice_mutex);
                videoServiceEnableInProgress = TRUE;                          
     
                pthread_t videoServiceThread;
                if (pthread_create(&videoServiceThread, NULL, &SetVideoServiceConfig, NULL))
                {
                    CcspTraceError(("RDK_LOG_ERROR, CcspPandM %s : Failed to Start Thread to start SetVideoServiceConfig  \n", __FUNCTION__ ));
                    return FALSE;
                }

                pthread_mutex_unlock(&g_videoservice_mutex);

                CcspTraceWarning(("VIDEOSERVICE is ENABLED\n"));
            }

        }
        else
        {
            bval[0] = '0';
            CcspTraceWarning(("VIDEOSERVICE is DISABLED\n"));
        }

        if (syscfg_set(NULL, "X_RDKCENTRAL-COM_VIDEOSERVICE", bval) != 0)
        {
            AnscTraceWarning(("[VideoService] syscfg_set X_RDKCENTRAL-COM_VIDEOSERVICE failed!\n"));
        }
        else
        {
            if (syscfg_commit() != 0)
            {
                AnscTraceWarning(("[VideoService] syscfg_commit X_RDKCENTRAL-COM_VIDEOSERVICE failed!\n"));
            }
        }

        return TRUE;
    }  

    return FALSE;

}

void SetVideoServiceConfig()
{
    int ret = ANSC_STATUS_SUCCESS;
    BOOL isBridgeMode = FALSE;
    char bval[2] = {0};
    char pMoCAComponentName[64]="eRT.com.cisco.spvtg.ccsp.moca";
    char pComponentPath[64]="/com/cisco/spvtg/ccsp/moca";
    char *paramNames[]={"Device.MoCA.Interface.1.Enable"};
    char buf[5] = {0};
    char FirewallLevel[32] = {0};
    ULONG len = 0;

    len = sizeof(FirewallLevel);
    ret = g_GetParamValueString(g_pDslhDmlAgent, "Device.X_CISCO_COM_Security.Firewall.FirewallLevel", FirewallLevel, &len );
    if(ret != ANSC_STATUS_SUCCESS)
    {
        pthread_mutex_lock(&g_videoservice_mutex);
        videoServiceEnableInProgress = FALSE;   
        pthread_mutex_unlock(&g_videoservice_mutex);
        AnscTraceError(("[%s] VideoService : Fail to get Firewall Level \n", __FUNCTION__));
        return;
    } 

    if(!strcmp(FirewallLevel, "High"))
    {
        ret = g_SetParamValueString("Device.X_CISCO_COM_Security.Firewall.FirewallLevel", "Medium");
        if(ret != ANSC_STATUS_SUCCESS)
        {
            pthread_mutex_lock(&g_videoservice_mutex);
            videoServiceEnableInProgress = FALSE;   
            pthread_mutex_unlock(&g_videoservice_mutex);
            AnscTraceError(("[%s] VideoService : Fail to set Firewall Level to Medium\n", __FUNCTION__));
            return;
        }
        else
        {
            CcspTraceWarning(("Firewall Level being set to MEDIUM when VIDEOSERVICE is ENABLED\n"));
        }     
    }

    ret = is_usg_in_bridge_mode(&isBridgeMode);
    if ( ret == ANSC_STATUS_SUCCESS )
    {
        if(isBridgeMode)
        {
            ret = g_SetParamValueString("Device.X_CISCO_COM_DeviceControl.LanManagementEntry.1.LanMode","router");
            if(ret != ANSC_STATUS_SUCCESS)
            {
                pthread_mutex_lock(&g_videoservice_mutex);
                videoServiceEnableInProgress = FALSE;   
                pthread_mutex_unlock(&g_videoservice_mutex);
                AnscTraceError(("[%s] VideoService : Fail to set Disable Bridge Mode \n", __FUNCTION__));
                return;
            }
            else
            {
                CcspTraceWarning(("Bridge Mode being DISABLED when VIDEOSERVICE is ENABLED\n"));
            }            
        }
    }
    else
    {
        pthread_mutex_lock(&g_videoservice_mutex);
        videoServiceEnableInProgress = FALSE;   
        pthread_mutex_unlock(&g_videoservice_mutex);
        AnscTraceError(("[%s] VideoService : Fail to query bridge mode \n", __FUNCTION__));
        return;
    }


    syscfg_get( NULL, "moca_enabled", buf, sizeof(buf));
    if( buf != NULL )
    {
        if (strcmp(buf,"0") == 0)
        {
            parameterValStruct_t  value = {"Device.MoCA.Interface.1.Enable", "true", ccsp_boolean};
            char compo[256] = "eRT.com.cisco.spvtg.ccsp.moca";
            char bus[256] = "/com/cisco/spvtg/ccsp/moca";
            char* faultParam = NULL;
            int ret1 = CCSP_FAILURE;

            CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;

            ret1 = CcspBaseIf_setParameterValues(
                      bus_handle,
                      compo,
                      bus,
                      0,
                      0,
                      &value,
                      1,
                      TRUE,
                      &faultParam
                      );

            if(ret1 != CCSP_SUCCESS)
            {
                if(faultParam)
                {
                    bus_info->freefunc(faultParam);
                }

                pthread_mutex_lock(&g_videoservice_mutex);
                videoServiceEnableInProgress = FALSE;   
                pthread_mutex_unlock(&g_videoservice_mutex);
                CcspTraceError(("RDK_LOG_ERROR,  [%s] VideoService MoCA Enable FAILED: Failed ret %d\n",__FUNCTION__,ret1));
                return;
            }
            else
            {
                CcspTraceWarning(("MoCA is being ENABLED when VIDEOSERVICE is ENABLED\n"));
            }
        }
    }

    pthread_mutex_lock(&g_videoservice_mutex);
    videoServiceEnableInProgress = FALSE;                          
    pthread_mutex_unlock(&g_videoservice_mutex);
}



/*BOOL
VideoService_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

ULONG
VideoService_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    COSA_DATAMODEL_PARENTALCONTROL  *pParCtrl = (COSA_DATAMODEL_PARENTALCONTROL*)g_pCosaBEManager->hParentalControl;
    COSA_DML_MANAGEDSITES           *pMngSites = &pParCtrl->ManagedSites;

    if (CosaDmlMngSites_SetConf(pMngSites) != ANSC_STATUS_SUCCESS)
    {
        CosaDmlMngSites_GetConf(pMngSites);
        return -1;
    }

    return 0;
}

ULONG
VideoService_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    COSA_DATAMODEL_PARENTALCONTROL  *pParCtrl = (COSA_DATAMODEL_PARENTALCONTROL*)g_pCosaBEManager->hParentalControl;
    COSA_DML_MANAGEDSITES           *pMngSites = &pParCtrl->ManagedSites;

    if (CosaDmlMngSites_GetConf(pMngSites) != ANSC_STATUS_SUCCESS)
        return -1;

    return 0;
}
*/
