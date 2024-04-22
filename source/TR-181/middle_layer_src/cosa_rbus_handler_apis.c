/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sysevent/sysevent.h>
#include "ccsp_trace.h"
#include "cosa_rbus_handler_apis.h"
#include "ccsp_psm_helper.h"
#include "safec_lib_common.h"
#include "cosa_dhcpv6_apis.h"
#include "syscfg/syscfg.h"
#if defined (WIFI_MANAGE_SUPPORTED)
#include "cosa_managedwifi_webconfig_apis.h"
unsigned int gManageWiFiBridgeSubscribersCount = 0;
unsigned int gManageWiFiEnableSubscribersCount = 0;
unsigned int gManageWiFiInterfaceSubscribersCount = 0;
#endif /*WIFI_MANAGE_SUPPORTED*/

rbusHandle_t handle;

#define NUM_OF_RBUS_PARAMS sizeof(devCtrlRbusDataElements)/sizeof(devCtrlRbusDataElements[0])

#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED)
DeviceControl_Net_Mode deviceControl_Net_Mode;


//PSM
extern ANSC_HANDLE g_MessageBusHandle;
extern char g_Subsystem[32];
extern void* g_pDslhDmlAgent;

unsigned int gSubscribersCount = 0;

static int sysevent_fd 	  = -1;
static token_t sysevent_token = 0;
#endif
/***********************************************************************

  Data Elements declaration:

 ***********************************************************************/
rbusDataElement_t devCtrlRbusDataElements[] = {

#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED)
	{DEVCTRL_NET_MODE_TR181, RBUS_ELEMENT_TYPE_EVENT, {getUlongHandler, setUlongHandler, NULL, NULL, eventDevctrlSubHandler, NULL}},
#endif
#if defined (WIFI_MANAGE_SUPPORTED)
    {MANAGE_WIFI_LAN_BRIDGE, RBUS_ELEMENT_TYPE_EVENT, {getStringHandler, setStringHandler, NULL, NULL, eventManageWiFiBridgeSubHandler,NULL}},
    {MANAGE_WIFI_ENABLE, RBUS_ELEMENT_TYPE_EVENT, {getBoolHandler, NULL, NULL, NULL, eventManageWiFiEnableSubHander, NULL}},
    {MANAGE_WIFI_INTERFACES, RBUS_ELEMENT_TYPE_EVENT, {getStringHandler, setStringHandler, NULL, NULL, eventManageWiFiInterfaceSubHandler,NULL}},
#endif
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED)

/***********************************************************************

  Get Handler APIs for objects of type RBUS_Ulong:

 ***********************************************************************/
rbusError_t getUlongHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t *opts)
{	
    pthread_mutex_lock(&mutex);
    char const* name = rbusProperty_GetName(property);
    (void)handle;
    (void)opts;
    rbusValue_t value;
    rbusValue_Init(&value);

    if (strcmp(name, DEVCTRL_NET_MODE_TR181) == 0)
    {
#if defined (RDKB_EXTENDER_ENABLED)
    char buf[ 8 ] = { 0 };
    if( 0 == syscfg_get( NULL, "Device_Mode", buf, sizeof( buf ) ) )
    {
        deviceControl_Net_Mode.DevCtrlNetMode = atoi(buf);
        rbusValue_SetUInt32(value, deviceControl_Net_Mode.DevCtrlNetMode);
    }
    else
    {
        CcspTraceError(("syscfg_get failed to retrieve  device networking mode\n")); 
        deviceControl_Net_Mode.DevCtrlNetMode = 0;
        CcspTraceWarning(("Returning '%lu' as NetworkingMode\n", deviceControl_Net_Mode.DevCtrlNetMode));
        rbusValue_SetUInt32(value, deviceControl_Net_Mode.DevCtrlNetMode);
    }     
#else
        //setting value as router for non-xle devices	
        deviceControl_Net_Mode.DevCtrlNetMode = 0;
        CcspTraceWarning(("Getting Device Networking Mode value for non-xle devices, new value = '%lu'\n", deviceControl_Net_Mode.DevCtrlNetMode));
        rbusValue_SetUInt32(value, deviceControl_Net_Mode.DevCtrlNetMode);
#endif
    }
    else
    {	
        CcspTraceWarning(("Device Networking Mode rbus get handler invalid input\n"));
        pthread_mutex_unlock(&mutex);
        return RBUS_ERROR_INVALID_INPUT;
    }
	
    rbusProperty_SetValue(property, value);
    rbusValue_Release(value);
    pthread_mutex_unlock(&mutex);

    return RBUS_ERROR_SUCCESS;
}

/*************************Set hanlder for local data*****************************************/
rbusError_t setUlongHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts)
{
#if defined (RDKB_EXTENDER_ENABLED)
    pthread_mutex_lock(&mutex);
    (void)handle;
    (void)opts;
    char const* name = rbusProperty_GetName(prop);
    rbusValue_t value = rbusProperty_GetValue(prop);
    rbusValueType_t type = rbusValue_GetType(value);
    rbusError_t ret = RBUS_ERROR_SUCCESS;
    char strValue[3] = {0};
    errno_t  rc = -1;
    uint32_t rVal = 0;
    char buf[8] = {0};
   
    //Here deviceControl_Net_Mode is global

    if(strcmp(name, DEVCTRL_NET_MODE_TR181) == 0)
    {
        if (type != RBUS_UINT32)
        {
            CcspTraceWarning(("Device Networking Mode input value is of invalid type\n"));
            pthread_mutex_unlock(&mutex);
            return RBUS_ERROR_INVALID_INPUT;
        }

        //Getting value from rbus set
        rVal = rbusValue_GetUInt32(value);
        if (rVal > 1) {
            CcspTraceError(("Invalid set value for the parameter '%s'\n", DEVCTRL_NET_MODE_TR181));
            return RBUS_ERROR_INVALID_INPUT;
        }

        /* Updating the Device Networking Mode in PSM database over sysevent */
        rc = sprintf_s(strValue, sizeof(strValue),"%lu", rVal);
        if(rc < EOK)
        {
            ERR_CHK(rc);
        }

        if (0 > sysevent_fd)
        {
            CcspTraceError(("Failed to execute sysevent_set. sysevent_fd have no value:'%d'\n", sysevent_fd));
            return RBUS_ERROR_BUS_ERROR;
        }
		
        // Fetch old value    
        uint32_t oldDevCtrlNetMode = deviceControl_Net_Mode.DevCtrlNetMode;
        //update DevCtrlNetMode with new value and publish
        if (oldDevCtrlNetMode != rVal) 	
        {
            snprintf(buf,sizeof(buf),"%d",rVal);
        
            //Setting Device Mode
            if (syscfg_set(NULL, "Device_Mode", buf) != 0)
            {
                CcspTraceInfo(("\n Device_Mode set syscfg failed\n"));       
            }
            else
            {
                if (syscfg_commit() != 0)
                {
                    CcspTraceInfo(("\nDevice_Mode syscfg_commit failed\n"));
                }
                else
                {
                    if(sysevent_set(sysevent_fd, sysevent_token, "DeviceMode", strValue, 0) != 0)
                    {
                        CcspTraceError(("Failed to execute sysevent_set from %s:%d\n", __FUNCTION__, __LINE__));
                        return RBUS_ERROR_BUS_ERROR;
                    }
                    CcspTraceInfo(("sysevent_set execution success.\n"));
                    deviceControl_Net_Mode.DevCtrlNetMode = rVal;
                    ret = publishDevCtrlNetMode(rVal, oldDevCtrlNetMode);
                    if (ret != RBUS_ERROR_SUCCESS)
                    {
                        CcspTraceError(("%s-%d: Failed to update and publish device mode value\n", __FUNCTION__, __LINE__));
                        return ret;
                    }
                    configureIpv6Route(rVal);
                }
            }
        }
    }    
    pthread_mutex_unlock(&mutex);
    return RBUS_ERROR_SUCCESS;
#else	
    (void)handle;
    (void)opts;
    (void)prop;
    CcspTraceError(("Set handler not supported for this device.\n"));
    return RBUS_ERROR_BUS_ERROR;
#endif
}

/***********************************************************************

  Event subscribe handler API for objects:

 ***********************************************************************/
rbusError_t eventDevctrlSubHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char *eventName, rbusFilter_t filter, int32_t interval, bool *autoPublish)
{
	(void)handle;
	(void)filter;
	(void)interval;

	*autoPublish = false;
  
  	CcspTraceWarning(("Event devctrl sub handler called.\n"));

	if (strcmp(eventName, DEVCTRL_NET_MODE_TR181) == 0)
	{
		if (action == RBUS_EVENT_ACTION_SUBSCRIBE)
		{
			gSubscribersCount += 1;
		}
		else
		{
			if (gSubscribersCount > 0)
			{
				gSubscribersCount -= 1;
			}
		}
		CcspTraceWarning(("Subscribers count changed, new value=%d\n", gSubscribersCount));
	}
	else
	{
		CcspTraceWarning(("provider: eventSubHandler unexpected eventName %s\n", eventName));
	}
	return RBUS_ERROR_SUCCESS;
}


/*******************************************************************************

  initNetMode(): Initialize deviceControl_Net_Mode struct with default values

 ********************************************************************************/
bool initNetMode()
{
		char strValue[3];
        errno_t  rc = -1;
        ULONG uValue = 1;

        /* Updating the Device Networking Mode in PSM database over sysevent */
        rc = sprintf_s(strValue, sizeof(strValue),"%lu",uValue);
        if(rc < EOK)
        {
          ERR_CHK(rc);
        }
        
		//sysevent set for default value
		if (0 > sysevent_fd)
		{
			CcspTraceError(("Failed to execute sysevent_set. sysevent_fd have no value:'%d'\n", sysevent_fd));
			return false;
		}
		if(sysevent_set(sysevent_fd, sysevent_token, "DeviceMode", strValue, 0) != 0)
		{
			CcspTraceError(("Failed to execute sysevent_set from %s:%d\n", __FUNCTION__, __LINE__));
			return false;
		}
		CcspTraceInfo(("sysevent_set execution success.\n"));
		CcspTraceWarning(("Initialized Device Networking Mode with default values.\n"));
		return true;
}


/*******************************************************************************

  sendUpdateEvent(): publish event after event value gets updated

 ********************************************************************************/
rbusError_t sendUlongUpdateEvent(char* event_name , uint32_t eventNewData, uint32_t eventOldData)
{
	rbusEvent_t event;
	rbusObject_t data;
	rbusValue_t value;
	rbusValue_t oldVal;
	rbusValue_t byVal;
	rbusError_t ret = RBUS_ERROR_SUCCESS;
	
	//initialize and set previous value for the event
	rbusValue_Init(&oldVal);
	rbusValue_SetUInt32(oldVal, eventOldData);
	//initialize and set new value for the event
	rbusValue_Init(&value);
	rbusValue_SetUInt32(value, eventNewData);
	//initialize and set responsible component name for value change
	rbusValue_Init(&byVal);
	rbusValue_SetString(byVal, RBUS_COMPONENT_NAME);
	//initialize and set rbusObject with desired values
	rbusObject_Init(&data, NULL);
	rbusObject_SetValue(data, "value", value);
	rbusObject_SetValue(data, "oldValue", oldVal);
	rbusObject_SetValue(data, "by", byVal);
	//set data to be transferred
	event.name = event_name;
	event.data = data;
	event.type = RBUS_EVENT_VALUE_CHANGED;
	//publish the event
	ret = rbusEvent_Publish(handle, &event);
	if(ret != RBUS_ERROR_SUCCESS) {
			CcspTraceWarning(("rbusEvent_Publish for %s failed: %d\n", event_name, ret));
	}
	//release all initialized rbusValue objects
	rbusValue_Release(value);
	rbusValue_Release(oldVal);
	rbusValue_Release(byVal);
	rbusObject_Release(data);
	return ret;
}

/*******************************************************************************

  publishDevCtrlNetMode(): publish DevCtrlNetMode event after event value gets updated

 ********************************************************************************/
 
rbusError_t publishDevCtrlNetMode(uint32_t new_val, uint32_t old_val)
{
	rbusError_t ret = RBUS_ERROR_SUCCESS;
	CcspTraceInfo(("Publishing Device Networking Mode with updated value=%d\n", new_val));
	if (gSubscribersCount > 0)
	{
		ret = sendUlongUpdateEvent(DEVCTRL_NET_MODE_TR181, new_val, old_val);
		if(ret == RBUS_ERROR_SUCCESS) {
			CcspTraceInfo(("Published Device Networking Mode with updated value.\n"));
		}
	}
	return ret;
}

bool PAM_Rbus_SyseventInit()
{		
	if (0 > sysevent_fd)
	{
		if ((sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "PAM_Rbus", &sysevent_token)) < 0)
		{
			CcspTraceError(("Failed to open sysevent.\n"));
			return false;
		}
		CcspTraceInfo(("sysevent_open success.\n"));
		return true;
	}
	CcspTraceError(("Failed to open sysevent. sysevent_fd already have a value '%d'\n", sysevent_fd));
	return false;
}
#endif

#if defined (WIFI_MANAGE_SUPPORTED)
rbusError_t getStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t *opts)
{
    pthread_mutex_lock(&mutex);
    (void)handle;
    (void)opts;
    char const* name = rbusProperty_GetName(property);
    rbusValue_t value;
    rbusValue_Init(&value);
    ManageWiFiInfo_t sManageWifiDetails = {0};
    char aParamVal[BUFF_LEN_64] = {0};

    getManageWiFiDetails(&sManageWifiDetails);
    if (0 == strcmp(name,MANAGE_WIFI_LAN_BRIDGE))
    {
        snprintf(aParamVal, BUFF_LEN_64-1, "%s%s",sManageWifiDetails.aKey,sManageWifiDetails.aBridgeName);
    }
    else if (0 == strcmp(name, MANAGE_WIFI_INTERFACES))
    {
        snprintf(aParamVal, BUFF_LEN_64-1, "%s%s",sManageWifiDetails.aKey,sManageWifiDetails.aWiFiInterfaces);
    }
    else
    {
        CcspTraceWarning(("Device Managed WiFi Bridge rbus get handler invalid input\n"));
        pthread_mutex_unlock(&mutex);
        return RBUS_ERROR_INVALID_INPUT;
    }
    rbusValue_SetString(value,aParamVal);

    rbusProperty_SetValue(property, value);
    rbusValue_Release(value);
    pthread_mutex_unlock(&mutex);

    return RBUS_ERROR_SUCCESS;
}
rbusError_t setStringHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts)
{
    (void)handle;
    (void)opts;
    char const* name = rbusProperty_GetName(prop);
    rbusValue_t value = rbusProperty_GetValue(prop);
    rbusValueType_t type = rbusValue_GetType(value);
    const char *strVal = NULL;
    char aString[BUFF_LEN_128] = {'\0'};
    char *pKeyVal = aString;
    ManageWiFiInfo_t sManageWifiDetails = {0};
    int len = 0;

    if (type != RBUS_STRING)
    {
        CcspTraceWarning(("input value is of invalid type\n"));
        return RBUS_ERROR_INVALID_INPUT;
    }

    strVal = rbusValue_GetString(value, &len);
    if (NULL == strVal)
    {
        CcspTraceError(("Invalid set value for the parameter '%s'\n", MANAGE_WIFI_LAN_BRIDGE));
        return RBUS_ERROR_INVALID_INPUT;
    }

    CcspTraceInfo(("%s:%d, StrVal:%s, len:%d\n",__FUNCTION__,__LINE__,strVal, len));
    snprintf(aString, BUFF_LEN_128,strVal);
    CcspTraceInfo(("%s:%d, aString:%s\n",__FUNCTION__,__LINE__,aString));
    pKeyVal = strtok(pKeyVal, ":");
    if (NULL != pKeyVal)
    {
        CcspTraceInfo(("%s:%d, pKeyVal:%s\n",__FUNCTION__,__LINE__,pKeyVal));
        strcpy(sManageWifiDetails.aKey,pKeyVal);
        CcspTraceInfo(("%s:%d, sManageWifiDetails.aKey:%s\n",__FUNCTION__,__LINE__,sManageWifiDetails.aKey));
    }
    pKeyVal = strtok(NULL, ":");

    if (NULL == pKeyVal)
    {
        CcspTraceWarning(("%s:%d, Value is NULL in the Key:Value pair\n",__FUNCTION__,__LINE__));
        return RBUS_ERROR_INVALID_INPUT;
    }

    if (0 == strcmp(name,MANAGE_WIFI_LAN_BRIDGE))
    {
	sManageWifiDetails.eUpdateType = BRIDGE_NAME;
        CcspTraceInfo(("%s:%d, pKeyVal:%s\n",__FUNCTION__,__LINE__,pKeyVal));
        strcpy(sManageWifiDetails.aBridgeName,pKeyVal);
        CcspTraceInfo(("%s:%d, sManageWifiDetails.aBridgeName:%s\n",__FUNCTION__,__LINE__,sManageWifiDetails.aBridgeName));
        setManageWiFiDetails (&sManageWifiDetails);
    }
    else if (0 == strcmp(name, MANAGE_WIFI_INTERFACES))
    {
	sManageWifiDetails.eUpdateType = WIFI_INTERFACES;
        CcspTraceInfo(("%s:%d, pKeyVal:%s\n",__FUNCTION__,__LINE__,pKeyVal));
        strcpy(sManageWifiDetails.aWiFiInterfaces,pKeyVal);
        CcspTraceInfo(("%s:%d, sManageWifiDetails.aWiFiInterfaces:%s\n",__FUNCTION__,__LINE__,sManageWifiDetails.aWiFiInterfaces));
        setManageWiFiDetails (&sManageWifiDetails);
    }
    else
    {
        CcspTraceWarning(("Device Managed WiFi rbus set handler invalid input\n"));
        return RBUS_ERROR_INVALID_INPUT;
    }
    return RBUS_ERROR_SUCCESS;
}

rbusError_t getBoolHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t *opts)
{
    pthread_mutex_lock(&mutex);
    char const* name = rbusProperty_GetName(property);
    (void)handle;
    (void)opts;
    rbusValue_t value;
    rbusValue_Init(&value);
    BOOL bEnable = false;

    if (0 == strcmp(name,MANAGE_WIFI_ENABLE))
    {
        getManageWiFiEnable(&bEnable);
        CcspTraceInfo(("%s:%d, bEnable:%d\n",__FUNCTION__,__LINE__,bEnable));
        rbusValue_SetBoolean(value,bEnable);
    }
    else
    {
        CcspTraceWarning(("Device Managed WiFi rbus Enable get handler invalid input\n"));
        pthread_mutex_unlock(&mutex);
        return RBUS_ERROR_INVALID_INPUT;
    }

    rbusProperty_SetValue(property, value);
    rbusValue_Release(value);
    pthread_mutex_unlock(&mutex);

    return RBUS_ERROR_SUCCESS;
}
rbusError_t eventManageWiFiBridgeSubHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char *eventName, rbusFilter_t filter, int32_t interval, bool *autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;

    *autoPublish = false;

    CcspTraceWarning(("Event WiFiManage Bridge sub handler called.\n"));

    if (strcmp(eventName, MANAGE_WIFI_LAN_BRIDGE) == 0)
    {
        if (action == RBUS_EVENT_ACTION_SUBSCRIBE)
        {
            gManageWiFiBridgeSubscribersCount += 1;
        }
        else
        {
            if (gManageWiFiBridgeSubscribersCount > 0)
            {
                gManageWiFiBridgeSubscribersCount -= 1;
            }
        }
        CcspTraceWarning(("WiFi Manage Bridge Subscribers count changed, new value=%d\n", gManageWiFiBridgeSubscribersCount));
    }
    else
    {
        CcspTraceWarning(("provider: eventSubHandler unexpected eventName %s\n", eventName));
    }
    return RBUS_ERROR_SUCCESS;
}
rbusError_t eventManageWiFiEnableSubHander(rbusHandle_t handle, rbusEventSubAction_t action, const char *eventName, rbusFilter_t filter, int32_t interval, bool *autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;

    *autoPublish = false;

    CcspTraceWarning(("Event Manage WiFi Enable sub handler called.\n"));

    if (strcmp(eventName, MANAGE_WIFI_ENABLE) == 0)
    {
        if (action == RBUS_EVENT_ACTION_SUBSCRIBE)
        {
            gManageWiFiEnableSubscribersCount+= 1;
        }
        else
        {
            if (gManageWiFiEnableSubscribersCount> 0)
            {
                gManageWiFiEnableSubscribersCount -= 1;
            }
        }
        CcspTraceWarning(("Manage WiFi Enable Subscribers count changed, new value=%d\n", gManageWiFiEnableSubscribersCount));
    }
    else
    {
        CcspTraceWarning(("provider: eventSubHandler unexpected eventName %s\n", eventName));
    }
    return RBUS_ERROR_SUCCESS;
}

rbusError_t eventManageWiFiInterfaceSubHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char *eventName, rbusFilter_t filter, int32_t interval, bool *autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;

    *autoPublish = false;

    CcspTraceWarning(("Event Manage WiFi Interface sub handler called.\n"));

    if (strcmp(eventName, MANAGE_WIFI_INTERFACES) == 0)
    {
        if (action == RBUS_EVENT_ACTION_SUBSCRIBE)
        {
            gManageWiFiInterfaceSubscribersCount += 1;
        }
        else
        {
            if (gManageWiFiInterfaceSubscribersCount > 0)
            {
                gManageWiFiInterfaceSubscribersCount -= 1;
            }
        }
        CcspTraceWarning(("Manage WiFi Interface Subscribers count changed, new value=%d\n", gManageWiFiInterfaceSubscribersCount));
    }
    else
    {
        CcspTraceWarning(("provider: eventSubHandler unexpected eventName %s\n", eventName));
    }
    return RBUS_ERROR_SUCCESS;
}
#endif /*WIFI_MANAGE_SUPPORTED*/
#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED) ||  defined(RBUS_BUILD_FLAG_ENABLE) ||  defined(_HUB4_PRODUCT_REQ_) || defined (_PLATFORM_RASPBERRYPI_) || defined (WIFI_MANAGE_SUPPORTED)
/***********************************************************************

  devCtrlRbusInit(): Initialize Rbus and data elements

 ***********************************************************************/
rbusError_t devCtrlRbusInit()
{
	int rc = RBUS_ERROR_SUCCESS;

	if(RBUS_ENABLED != rbus_checkStatus())
    {
		CcspTraceWarning(("%s: RBUS not available. Events is not supported\n", __FUNCTION__));
		return RBUS_ERROR_BUS_ERROR;
    }

	rc = rbus_open(&handle, RBUS_COMPONENT_NAME);
	if (rc != RBUS_ERROR_SUCCESS)
	{
		CcspTraceWarning(("DevCtrl rbus initialization failed\n"));
		rc = RBUS_ERROR_NOT_INITIALIZED;
		return rc;
	}
#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED) || defined (WIFI_MANAGE_SUPPORTED)
	// Register data elements
	rc = rbus_regDataElements(handle, NUM_OF_RBUS_PARAMS, devCtrlRbusDataElements);
#endif
	if (rc != RBUS_ERROR_SUCCESS)
	{
		CcspTraceWarning(("rbus register data elements failed\n"));
		rc = rbus_close(handle);
		return rc;
	}
#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED)	
	//initialize sysevent
	PAM_Rbus_SyseventInit();
#endif
	return rc;
}
#endif
