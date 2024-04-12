/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
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
 *
**********************************************************************/
#include <stdbool.h>
#include <syscfg/syscfg.h>
#include "speedboost_dml.h"
#include "speedboost_apis.h"
#include "speedboost_rbus_handler.h"
#include "speedboost_webconfig_apis.h"
#include "speedboost_scheduler_apis.h"

static int pvd_seqNum_subscribe = 0;
static int pvd_enable_subscribe = 0;
static int pvd_fqdn_subscribe = 0;
static int pvd_delay_subscribe = 0;
static int pvd_hflag_subscribe = 0;
int speed_boost_subscribe = 0;
int speed_boost_normal_subscribe = 0;

//speedboost global value
char current_speedboost_value[64] = {0};
char current_normal_speedboost_value[64] = {0};

extern SpeedBoost sSpeedBoost;
extern SpeedBoostType boostType;
static unsigned int uiConfiguredClientsCountSubscribed = 0;
static unsigned int uiEligibleClientsCountSubscribed = 0;
static unsigned int uiActiveClientsCountSubscribed = 0;

rbusError_t Pvd_GetBoolHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts) 
{
    (void) handle;
    (void) opts;
    char const* propertyName;

    propertyName = rbusProperty_GetName(property);
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }

    if(strcmp(propertyName, ADVERTISEMENT_PVD_ENABLE) == 0)
    {
        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetBoolean(value, sSpeedBoost.pvd_enabled);
        rbusProperty_SetValue(property, value);
        rbusValue_Release(value);
    }
    else if(strcmp(propertyName, ADVERTISEMENT_PVD_H_FLAG) == 0)
    {
        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetBoolean(value, sSpeedBoost.pvd_h_flag);
        rbusProperty_SetValue(property, value);
        rbusValue_Release(value);
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Set handler for pvd Bool

*************************************************************************************/
rbusError_t Pvd_SetBoolHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts)
{
    (void) handle;
    (void) opts;
    char const* propertyName;
    propertyName = rbusProperty_GetName(prop);
    bool prevValue = false;
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    if(strcmp(propertyName, ADVERTISEMENT_PVD_ENABLE) == 0)
    {
        rbusValue_t value = rbusProperty_GetValue(prop);
        if(rbusValue_GetType(value) == RBUS_BOOLEAN)
        {
            prevValue = sSpeedBoost.pvd_enabled ;
            sSpeedBoost.pvd_enabled = rbusValue_GetBoolean(value);
            if ( prevValue != sSpeedBoost.pvd_enabled )
            {
               // char buf[8] = {0}; 
               // snprintf(buf, sizeof(buf), "%s", sSpeedBoost.pvd_enabled);
                if((syscfg_set_commit(NULL,"Advertisement_pvd_enable",(sSpeedBoost.pvd_enabled == true) ? "true" : "false" ) != 0))
                {
                    CcspTraceError(("syscfg_set failed for Advertisement_pvd_enable\n"));
                    return RBUS_ERROR_BUS_ERROR;
                }
                CcspTraceInfo(("Set value is %d\n", sSpeedBoost.pvd_enabled));
                trigger_ra_service_restart();
                triggerFirewallRestart();
                if (pvd_enable_subscribe)
                {
                    Publish_PvdValueOnChange(ADVERTISEMENT_PVD_ENABLE, &prevValue, &sSpeedBoost.pvd_enabled, RBUS_BOOLEAN);
                }
            }
            else
            {
                CcspTraceInfo(("[%s]: The value is same as the previous value\n", __FUNCTION__));
            }
            return RBUS_ERROR_SUCCESS;
        }
        else
        {
            CcspTraceError(("Invalid value type\n"));
            return RBUS_ERROR_INVALID_INPUT;
        }
    }
    else if(strcmp(propertyName, ADVERTISEMENT_PVD_H_FLAG) == 0)
    {
    	rbusValue_t value = rbusProperty_GetValue(prop);
        if(rbusValue_GetType(value) == RBUS_BOOLEAN)
        {
            char buf[8] = {0};
            prevValue = sSpeedBoost.pvd_h_flag;
            sSpeedBoost.pvd_h_flag = rbusValue_GetBoolean(value);
            if ( prevValue != sSpeedBoost.pvd_h_flag )
            {
                snprintf(buf, sizeof(buf), "%d", sSpeedBoost.pvd_h_flag);
                if((syscfg_set_commit(NULL,"Advertisement_pvd_hflag",buf) != 0))
                {
                    CcspTraceError(("syscfg_set failed for Advertisement_pvd_hflag\n"));
                    return RBUS_ERROR_BUS_ERROR;
                }
                CcspTraceInfo(("Set value is %d\n", sSpeedBoost.pvd_h_flag));
                trigger_ra_service_restart();
                if (pvd_hflag_subscribe)
                {
                    Publish_PvdValueOnChange(ADVERTISEMENT_PVD_H_FLAG, &prevValue, &sSpeedBoost.pvd_h_flag, RBUS_BOOLEAN);
                }            
            }
            else
            {
                CcspTraceInfo(("[%s]: The value is same as the previous value\n", __FUNCTION__));
            }
            return RBUS_ERROR_SUCCESS;
        }
        else
        {
            CcspTraceError(("[%s]: Invalid value type\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Subscribe handler for pvd bool

*************************************************************************************/

rbusError_t Pvd_subBoolHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;
    *autoPublish = false;

    CcspTraceInfo(("PvdEnable_subHandler: action=%s eventName=%s\n", action==RBUS_EVENT_ACTION_SUBSCRIBE?"subscribe":"unsubscribe", eventName));

    if(!strcmp(ADVERTISEMENT_PVD_ENABLE, eventName))
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == action )
        {
            pvd_enable_subscribe++;
            CcspTraceInfo(("Pvd_subBoolHandler: Subscribed to PvdEnable, status - %d\n",pvd_enable_subscribe));
        }
        else
        {
            if(pvd_enable_subscribe > 0)
                pvd_enable_subscribe--;
            CcspTraceInfo(("Pvd_subBoolHandler: Unsubscribed from PvdEnable, status - %d\n",pvd_enable_subscribe));
        }
    }
    else if(!strcmp(ADVERTISEMENT_PVD_H_FLAG, eventName))
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == action )
        {
            pvd_hflag_subscribe++;
            CcspTraceInfo(("Pvd_subBoolHandler: Subscribed to PvdHflag, status - %d\n",pvd_hflag_subscribe));
        }
        else
        {
            if(pvd_hflag_subscribe > 0)
                pvd_hflag_subscribe--;
            CcspTraceInfo(("Pvd_subBoolHandler: Unsubscribed from PvdHflag, status - %d\n",pvd_hflag_subscribe));
        }
    }
    else
    {
        CcspTraceError(("Pvd_subBoolHandler: Invalid event name\n"));
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Get handler for pvd int

*************************************************************************************/

rbusError_t Pvd_GetIntHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts) 
{
    (void) handle;
    (void) opts;
    char const* propertyName;
    propertyName = rbusProperty_GetName(property);
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    if(strcmp(propertyName, ADVERTISEMENT_PVD_DELAY) == 0)
    {
        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetInt32(value, sSpeedBoost.pvd_delay);
        rbusProperty_SetValue(property, value);
        rbusValue_Release(value);
    }
    else if(strcmp(propertyName, ADVERTISEMENT_PVD_SEQ_NUMBER) == 0)
    {

        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetInt32(value, sSpeedBoost.pvd_seqNum);
        rbusProperty_SetValue(property, value);
        rbusValue_Release(value);
    } 
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Set handler for pvd int

*************************************************************************************/

rbusError_t Pvd_SetIntHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts)
{
    (void) handle;
    (void) opts;
    char const* propertyName;
    propertyName = rbusProperty_GetName(prop);
    int prevValue = 0;
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    if(strcmp(propertyName, ADVERTISEMENT_PVD_DELAY) == 0)
    {
        rbusValue_t value = rbusProperty_GetValue(prop);
        if(rbusValue_GetType(value) == RBUS_INT32)
        {
            char buf[8] = {0};
            prevValue = sSpeedBoost.pvd_delay;
            sSpeedBoost.pvd_delay = rbusValue_GetInt32(value);
            if(sSpeedBoost.pvd_delay < 0 || sSpeedBoost.pvd_delay >15)
            {
                CcspTraceError(("[%s]: pvd_delay cannot be less than 0 and more than 15\n", __FUNCTION__));
                 sSpeedBoost.pvd_delay = prevValue;
                return RBUS_ERROR_INVALID_INPUT;
            }
            else
            {
                if ( prevValue != sSpeedBoost.pvd_delay )
                {
                    snprintf(buf, sizeof(buf), "%d", sSpeedBoost.pvd_delay);
                    if((syscfg_set_commit(NULL,"Advertisement_pvd_delay",buf) != 0))
                    {
                        CcspTraceError(("syscfg_set failed\n"));
                        return RBUS_ERROR_BUS_ERROR;
                    }
                    CcspTraceInfo(("Set value is %d\n", sSpeedBoost.pvd_delay));
                    trigger_ra_service_restart();
                    if(pvd_delay_subscribe)
                    {
                        Publish_PvdValueOnChange(ADVERTISEMENT_PVD_DELAY, &prevValue, &sSpeedBoost.pvd_delay, RBUS_INT32);
                    }
                }
                else
                {
                    CcspTraceInfo(("[%s]: The value is same as the previous value\n", __FUNCTION__));
                    return RBUS_ERROR_SUCCESS;
                }
                return RBUS_ERROR_SUCCESS;
            }
        }
        else
        {
            CcspTraceError(("[%s]: Invalid value type\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
    }
    else if(strcmp(propertyName, ADVERTISEMENT_PVD_SEQ_NUMBER) == 0)
    {
    	rbusValue_t value = rbusProperty_GetValue(prop);
        if(rbusValue_GetType(value) == RBUS_INT32)
        {
            char buf[8] = {0};
            prevValue = sSpeedBoost.pvd_seqNum;
            sSpeedBoost.pvd_seqNum = rbusValue_GetInt32(value);
            if(sSpeedBoost.pvd_seqNum < 0 || sSpeedBoost.pvd_seqNum > 65535)
            {
                CcspTraceError(("[%s]: pvd_seqNum cannot be less than 0 and more than 65535\n", __FUNCTION__));
                sSpeedBoost.pvd_seqNum = prevValue;
                return RBUS_ERROR_INVALID_INPUT;
            }
            else
            {
                if(prevValue != sSpeedBoost.pvd_seqNum)
                {
                    snprintf(buf, sizeof(buf), "%d", sSpeedBoost.pvd_seqNum);
                    if((syscfg_set_commit(NULL,"Advertisement_pvd_seqNum",buf) != 0))
                    {
                        CcspTraceError(("syscfg_set failed\n"));
                        return RBUS_ERROR_BUS_ERROR;
                    }
                    CcspTraceInfo(("Set value is %d\n", sSpeedBoost.pvd_seqNum));
                    trigger_ra_service_restart();
                    if(pvd_seqNum_subscribe)
                    {
                        Publish_PvdValueOnChange(ADVERTISEMENT_PVD_SEQ_NUMBER, &prevValue, &sSpeedBoost.pvd_seqNum, RBUS_INT32);
                    }
                }
                else
                {
                    CcspTraceInfo(("[%s]: The value is same as the previous value\n", __FUNCTION__));
                }
                return RBUS_ERROR_SUCCESS;
            }
        }
        else
        {
            CcspTraceError(("[%s]: Invalid value type\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Subscribe handler for pvd int 

*************************************************************************************/

rbusError_t Pvd_subIntHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;
    *autoPublish = false;

    CcspTraceInfo(("PvdDelay_subHandler: action=%s eventName=%s\n", action==RBUS_EVENT_ACTION_SUBSCRIBE?"subscribe":"unsubscribe", eventName));

    if(!strcmp(ADVERTISEMENT_PVD_DELAY, eventName))
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == action )
        {
            pvd_delay_subscribe++;
            CcspTraceInfo(("Pvd_subIntHandler: Subscribed to PvdDelay, status - %d\n",pvd_delay_subscribe));
        }
        else
        {
            if(pvd_delay_subscribe > 0)
                pvd_delay_subscribe--;
            CcspTraceInfo(("Pvd_subIntHandler: Unsubscribed from PvdDelay, status - %d\n",pvd_delay_subscribe));
        }
    }
    else if(!strcmp(ADVERTISEMENT_PVD_SEQ_NUMBER, eventName))
    {        
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == action )
        {
            pvd_seqNum_subscribe++;
            CcspTraceInfo(("Pvd_subIntHandler: Subscribed to PvdSeqNumber, status - %d\n",pvd_seqNum_subscribe));
        }
        else
        {
            if(pvd_seqNum_subscribe > 0)
                pvd_seqNum_subscribe--;
            CcspTraceInfo(("Pvd_subIntHandler: Unsubscribed from PvdSeqNumber, status - %d\n",pvd_seqNum_subscribe));
        }
    }
    else
    {
        CcspTraceError(("Pvd_subIntHandler: Invalid event name\n"));
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Get handler for pvd string

*************************************************************************************/

rbusError_t Pvd_GetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts) 
{
    (void) handle;
    (void) opts;
    char const* propertyName;
    propertyName = rbusProperty_GetName(property);
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    if(strcmp(propertyName, ADVERTISEMENT_PVD_FQDN) == 0)
    {

            rbusValue_t value;
            rbusValue_Init(&value);
            rbusValue_SetString(value, sSpeedBoost.pvd_fqdn);
            rbusProperty_SetValue(property, value);
            rbusValue_Release(value);
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Set handler for pvd string

*************************************************************************************/

rbusError_t Pvd_SetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusSetHandlerOptions_t* opts) 
{
    (void) handle;
    (void) opts;
    char const* propertyName;
    propertyName = rbusProperty_GetName(property);
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    if(strcmp(propertyName, ADVERTISEMENT_PVD_FQDN) == 0)
    {
        char prev_val[256] = {0};
        rbusValue_t value = rbusProperty_GetValue(property);
        if(rbusValue_GetType(value) != RBUS_STRING)
        {
            CcspTraceError(("[%s]: Invalid gettype name\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
        strncpy(prev_val, sSpeedBoost.pvd_fqdn, sizeof(prev_val)-1);
        const char* str = rbusValue_GetString(value,NULL);
        if(strlen(str) > 255)
        {
            CcspTraceError(("[%s]: Invalid string length\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }

        if (strcmp(sSpeedBoost.pvd_fqdn, str) == 0)
        {
            CcspTraceInfo(("[%s]: The value is same as the previous value\n", __FUNCTION__));
            return RBUS_ERROR_SUCCESS;
        }
        else{
            strncpy(sSpeedBoost.pvd_fqdn, str, sizeof(sSpeedBoost.pvd_fqdn)-1);
            if(syscfg_set_commit(NULL, "Advertisement_pvd_fqdn", sSpeedBoost.pvd_fqdn) != 0)
            {
                CcspTraceError(("syscfg_set failed\n"));
                return RBUS_ERROR_BUS_ERROR;
            }
            trigger_ra_service_restart();        
            if(pvd_fqdn_subscribe)
            {
                Publish_PvdValueOnChange(ADVERTISEMENT_PVD_FQDN, prev_val, sSpeedBoost.pvd_fqdn, RBUS_STRING);
            }
        }

    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    Subscribe handler for pvd string

*************************************************************************************/

rbusError_t Pvd_subStringHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;
    *autoPublish = false;

    CcspTraceInfo(("Pvd_subStringHandler: action=%s eventName=%s\n", action==RBUS_EVENT_ACTION_SUBSCRIBE?"subscribe":"unsubscribe", eventName));

    if(strcmp(ADVERTISEMENT_PVD_FQDN, eventName) == 0 )
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == action )
        { 
            pvd_fqdn_subscribe++;
            CcspTraceInfo(("Pvd_subStringHandler: Subscribed to PvdFQDN, status - %d\n",pvd_fqdn_subscribe));
        }
        else
        {
            if(pvd_fqdn_subscribe > 0)
                pvd_fqdn_subscribe--;
            CcspTraceInfo(("Pvd_subStringHandler: Unsubscribed from PvdFQDN, status - %d\n",pvd_fqdn_subscribe));

        }
    }
    else
    {
        CcspTraceError(("Pvd_subStringHandler: Invalid event name\n"));
    }
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    set blob handler for speedBoost blob

*************************************************************************************/

rbusError_t setBlobHandler(rbusHandle_t rbusHandle, rbusProperty_t rbusProperty, rbusSetHandlerOptions_t* pRbusHandleOpts)
{
    const char *pBlobData = NULL;
    char *     pParamName = NULL;
    int              iLen = 0;
    bool             bRet = false;
    
    UNUSED_PARAMETER(pRbusHandleOpts);
    UNUSED_PARAMETER(rbusHandle);

    rbusValue_t rbusValue         = rbusProperty_GetValue(rbusProperty);
    rbusValueType_t rbusValueType = rbusValue_GetType(rbusValue);
    char const* pRbusPropertyName = rbusProperty_GetName(rbusProperty);
    if (NULL != pRbusPropertyName)
        pParamName = strdup(GetParamName(pRbusPropertyName));
    else
    {
        CcspTraceError(("%s:%d, rbus property Name is NULL",__FUNCTION__,__LINE__));
        return RBUS_ERROR_INVALID_INPUT;
    }

    if (NULL == pParamName)
    {
        CcspTraceError(("%s:%d, Failed to allocate memory for parameter name\n",__FUNCTION__,__LINE__));
        return RBUS_ERROR_BUS_ERROR;
    }

    if (rbusValueType != RBUS_STRING)
    {
        CcspTraceWarning(("input value is of invalid type\n"));
        free(pParamName);
        return RBUS_ERROR_INVALID_INPUT;
    }

    pBlobData = rbusValue_GetString(rbusValue, &iLen);
    if (NULL == pBlobData)
    {
        CcspTraceError(("Invalid set value for the parameter '%s'\n", SPEEDBOOST_BLOB_DATA));
        free(pParamName);
        return RBUS_ERROR_INVALID_INPUT;
    }

    CcspTraceInfo(("%s:%d, ParamName:%s\n",__FUNCTION__,__LINE__,pParamName));
    CcspTraceInfo(("%s:%d, pBlobData:%s, len:%d\n",__FUNCTION__,__LINE__,pBlobData, iLen));

    bRet = setBlobDataOfSpeedBoost (NULL, pParamName, (char*)pBlobData);

    if (false == bRet)
    {
        CcspTraceError(("%s:%d, Failed to set the Blob data\n",__FUNCTION__,__LINE__));
        free(pParamName);
        return RBUS_ERROR_BUS_ERROR;
    }
    free(pParamName);
    return RBUS_ERROR_SUCCESS;
}

/************************************************************************************

    get blob handler for speedBoost blob

*************************************************************************************/
rbusError_t getBlobHandler(rbusHandle_t rbusHandle, rbusProperty_t rbusProperty, rbusGetHandlerOptions_t* pRbusHandleOpts)
{
    char *             pParamName = NULL;
    char aParamVal [BUFF_SIZE_64] = {0};

    UNUSED_PARAMETER(pRbusHandleOpts);
    UNUSED_PARAMETER(rbusHandle);
    UNUSED_PARAMETER(rbusProperty);

    rbusValue_t rbusValue;
    rbusValue_Init(&rbusValue);

    char const* pRbusPropertyName  = rbusProperty_GetName(rbusProperty);
    if (NULL == pRbusPropertyName)
    {
        CcspTraceError(("%s:%d, rbus property Name is NULL",__FUNCTION__,__LINE__));
        return RBUS_ERROR_INVALID_INPUT;
    }

    pParamName = strdup(GetParamName(pRbusPropertyName));

    if (NULL == pParamName)
    {
        CcspTraceError(("%s:%d, Failed to allocate memory for parameter name\n",__FUNCTION__,__LINE__));
        return RBUS_ERROR_BUS_ERROR;
    }

    CcspTraceInfo(("%s:%d, ParamName:%s\n",__FUNCTION__,__LINE__,pRbusPropertyName));

    if (!strcmp(pRbusPropertyName, SPEEDBOOST_BLOB_DATA))
    {
        snprintf(aParamVal, BUFF_SIZE_64, "%s",getBlobDataOfSpeedBoost(NULL, pParamName,aParamVal, BUFF_SIZE_64) ? aParamVal : "");
    }
    else
    {
        CcspTraceWarning(("%s:%d, rbus get handler invalid input\n",__FUNCTION__,__LINE__));
        free(pParamName);
        return RBUS_ERROR_INVALID_INPUT;
    }

    free(pParamName);
    rbusValue_SetString(rbusValue, aParamVal);
    rbusProperty_SetValue(rbusProperty, rbusValue);
    rbusValue_Release(rbusValue);

    return RBUS_ERROR_SUCCESS;
}

/********************************************************************************************************************************

    subscribe handler for Device.X_RDK_Speedboost.NumberConfiguredDevices, Device.X_RDK_Speedboost.NumberEligibleDevices

*********************************************************************************************************************************/
rbusError_t clientIntSubHandler(rbusHandle_t rbusHandle, rbusEventSubAction_t rbusAction, const char *pEventName, rbusFilter_t rbusFilter, int32_t i32Interval, bool *pAutoPublish)
{

    UNUSED_PARAMETER(rbusFilter);
    UNUSED_PARAMETER(rbusHandle);
    UNUSED_PARAMETER(i32Interval);

    *pAutoPublish = false;

    CcspTraceInfo(("%s:%d, action=%s eventName=%s\n", __FUNCTION__,__LINE__, rbusAction==RBUS_EVENT_ACTION_SUBSCRIBE?"subscribe":"unsubscribe", pEventName));

    if (!strcmp(SPEEDBOOST_NUMBER_OF_CONFIGURED_DEVICES, pEventName))
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == rbusAction )
        {
            CcspTraceInfo(("%s:%d, Subscribed to Configured clients of Speed Boost\n",__FUNCTION__,__LINE__));
            uiConfiguredClientsCountSubscribed++;
            CcspTraceInfo(("%s:%d, Configured clients subscription count :%d\n",__FUNCTION__,__LINE__,uiConfiguredClientsCountSubscribed));
        }
        else
        {
            CcspTraceInfo(("%s:%d, Un Subscribed to Configured clients of Speed Boost\n",__FUNCTION__,__LINE__));
            if (uiConfiguredClientsCountSubscribed > 0)
            {
                uiConfiguredClientsCountSubscribed--;
                CcspTraceInfo(("%s:%d, Configured clients subscription count :%d\n",__FUNCTION__,__LINE__,uiConfiguredClientsCountSubscribed));
            }
            
        }
    }
    else if (!strcmp(SPEEDBOOST_NUMBER_OF_ELIGIBLE_DEVICES, pEventName))
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == rbusAction )
        {
            CcspTraceInfo(("%s:%d, Subscribed to eligible clients of Speed Boost\n",__FUNCTION__,__LINE__));
            uiEligibleClientsCountSubscribed++;
            CcspTraceInfo(("%s:%d, Eligible clients subscription count :%d\n",__FUNCTION__,__LINE__,uiEligibleClientsCountSubscribed));
        }
        else
        {
            CcspTraceInfo(("%s:%d, Un Subscribed to eligible clients of Speed Boost\n",__FUNCTION__,__LINE__));
            if (uiEligibleClientsCountSubscribed > 0)
            {
                uiEligibleClientsCountSubscribed--;
                CcspTraceInfo(("%s:%d, Eligible clients subscription count :%d\n",__FUNCTION__,__LINE__,uiEligibleClientsCountSubscribed));
            }
        }
    }
    return RBUS_ERROR_SUCCESS;
}

/***************************************************************************************************************************************************************************

    Get handler for Device.X_RDK_Speedboost.NumberConfiguredDevices, Device.X_RDK_Speedboost.NumberEligibleDevices, Device.X_RDK_Speedboost.CurrentDeviceList

****************************************************************************************************************************************************************************/
rbusError_t clientsInfoGetHandler(rbusHandle_t rbusHandle, rbusProperty_t rbusProperty, rbusGetHandlerOptions_t* pRbusHandleOpts)
{
    int              iVal = -1;
    char *     pParamName = NULL;

    UNUSED_PARAMETER(pRbusHandleOpts);
    UNUSED_PARAMETER(rbusHandle);
    UNUSED_PARAMETER(rbusProperty);
 
    rbusValue_t rbusValue;
    rbusValue_Init(&rbusValue);

    char const* pRbusPropertyName  = rbusProperty_GetName(rbusProperty);
    if (NULL == pRbusPropertyName)
    {
        CcspTraceError(("%s:%d, rbus property Name is NULL",__FUNCTION__,__LINE__));
        return RBUS_ERROR_INVALID_INPUT;
    }

    pParamName = strdup(GetParamName(pRbusPropertyName));
    if (NULL == pParamName)
    {
        CcspTraceError(("%s:%d, Failed to allocate memory for parameter name\n",__FUNCTION__,__LINE__));
        return RBUS_ERROR_BUS_ERROR;
    }

    CcspTraceInfo(("%s:%d, ParamName:%s\n",__FUNCTION__,__LINE__,pParamName));

    if (!strcmp(pRbusPropertyName, SPEEDBOOST_NUMBER_OF_CONFIGURED_DEVICES))
    {
        if(getParamIntValOfSpeedBoost(NULL,pParamName, &iVal))
        {
            rbusValue_SetInt32(rbusValue,iVal);
        }
    }
    else if (!strcmp(pRbusPropertyName,SPEEDBOOST_NUMBER_OF_ELIGIBLE_DEVICES))
    {
        if (getParamIntValOfSpeedBoost(NULL, pParamName, &iVal))
        {
            rbusValue_SetInt32(rbusValue,iVal);
        }
    }
    else if (!strcmp(pRbusPropertyName, SPEEDBOOST_CURRENT_ACTIVE_DEVICE_LIST))
    {
        char aParamVal[BUFF_SIZE_256] = {0};
        if (getParamStringValOfSpeedBoost(NULL,pParamName, aParamVal, BUFF_SIZE_256))
        {
            rbusValue_SetString(rbusValue, aParamVal);
        }
    }
    else
    {
        CcspTraceWarning(("%s:%d, rbus get handler invalid input\n",__FUNCTION__,__LINE__));
        free(pParamName);
        return RBUS_ERROR_INVALID_INPUT;
    }
    rbusProperty_SetValue(rbusProperty, rbusValue);
    rbusValue_Release(rbusValue);
    free(pParamName);
    return RBUS_ERROR_SUCCESS;
}

/********************************************************************************

    subscribe handler for  Device.X_RDK_Speedboost.CurrentDeviceList

*********************************************************************************/
rbusError_t clientStringSubHandler(rbusHandle_t rbusHandle, rbusEventSubAction_t rbusAction, const char *pEventName, rbusFilter_t rbusFilter, int32_t i32Interval, bool *pAutoPublish)
{

    UNUSED_PARAMETER(rbusFilter);
    UNUSED_PARAMETER(rbusHandle);
    UNUSED_PARAMETER(i32Interval);

    *pAutoPublish = false;

    CcspTraceInfo(("%s:%d, action=%s eventName=%s\n", __FUNCTION__,__LINE__, rbusAction==RBUS_EVENT_ACTION_SUBSCRIBE?"subscribe":"unsubscribe", pEventName));

    if (!strcmp(SPEEDBOOST_CURRENT_ACTIVE_DEVICE_LIST, pEventName))
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == rbusAction )
        {
            CcspTraceInfo(("%s:%d, Subscribed to Active clients list of Speed Boost\n",__FUNCTION__,__LINE__));
            uiActiveClientsCountSubscribed++;
            CcspTraceInfo(("%s:%d, Active Device list subscription count :%d\n",__FUNCTION__,__LINE__,uiActiveClientsCountSubscribed));
        }
        else
        {
            CcspTraceInfo(("%s:%d, Un Subscribed to Active clients of Speed Boost\n",__FUNCTION__,__LINE__));
            if (uiActiveClientsCountSubscribed > 0)
            {
                uiActiveClientsCountSubscribed--;
                CcspTraceInfo(("%s:%d, Active Device list subscription count :%d\n",__FUNCTION__,__LINE__,uiActiveClientsCountSubscribed));
            }
        }
    }
    return RBUS_ERROR_SUCCESS;
}

void publishEventChangeIfSubscribed(const char *pEventName, void *pPrevVal, void *pCurrVal)
{
    if ((NULL == pPrevVal) || (NULL == pCurrVal) || (NULL == pEventName))
    {
        CcspTraceError(("%s:%d, NULL parameters passed\n",__FUNCTION__,__LINE__));
        return;
    }

    if (!strcmp(SPEEDBOOST_NUMBER_OF_CONFIGURED_DEVICES, pEventName))
    {
        if (uiConfiguredClientsCountSubscribed > 0)
        {
            Publish_PvdValueOnChange(SPEEDBOOST_NUMBER_OF_CONFIGURED_DEVICES, pPrevVal, pCurrVal, RBUS_INT32);
        }
    }
    else if (!strcmp(SPEEDBOOST_NUMBER_OF_ELIGIBLE_DEVICES, pEventName))
    {
        if (uiEligibleClientsCountSubscribed > 0)
        {
            Publish_PvdValueOnChange(SPEEDBOOST_NUMBER_OF_ELIGIBLE_DEVICES, pPrevVal, pCurrVal, RBUS_INT32);
        }
    }
    else if (!strcmp(SPEEDBOOST_CURRENT_ACTIVE_DEVICE_LIST, pEventName))
    {
        if (uiActiveClientsCountSubscribed > 0)
        {
            Publish_PvdValueOnChange(SPEEDBOOST_CURRENT_ACTIVE_DEVICE_LIST, pPrevVal, pCurrVal, RBUS_STRING);
        }
    }
    else if (!strcmp(SPEED_BOOST_PORT_RANGE, pEventName))
    {
        if(speed_boost_subscribe > 0)
        {
            Publish_PvdValueOnChange(SPEED_BOOST_PORT_RANGE, pPrevVal, pCurrVal, RBUS_STRING);
        }
    }
    else if (!strcmp(SPEED_BOOST_NORMAL_PORT_RANGE, pEventName))
    {
      	if(speed_boost_normal_subscribe > 0)
        {
            Publish_PvdValueOnChange(SPEED_BOOST_NORMAL_PORT_RANGE, pPrevVal, pCurrVal, RBUS_STRING);
        }

    }
    else
    {
        CcspTraceWarning(("%s:%d, Invalid event name\n",__FUNCTION__,__LINE__));
    }
}

rbusError_t speed_GetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts) 
{
    (void) handle;
    (void) opts;
    char const* propertyName;
    propertyName = rbusProperty_GetName(property);
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    if(strcmp(propertyName, SPEED_BOOST_PORT_RANGE) == 0)
    {
        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetString(value, current_speedboost_value);
        rbusProperty_SetValue(property, value);
        rbusValue_Release(value);
    }
    if(strcmp(propertyName, SPEED_BOOST_NORMAL_PORT_RANGE) == 0)
    {
        rbusValue_t value;
        rbusValue_Init(&value);
        rbusValue_SetString(value, current_normal_speedboost_value);
        rbusProperty_SetValue(property, value);
        rbusValue_Release(value);
    }
    return RBUS_ERROR_SUCCESS;
}

rbusError_t speed_SetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusSetHandlerOptions_t* opts) 
{
    (void) handle;
    (void) opts;
    char const* propertyName;
    propertyName = rbusProperty_GetName(property);
    if(propertyName) 
    {
        CcspTraceInfo(("Called %s for [%s]\n", __FUNCTION__, propertyName));
    }
    else
    {
        CcspTraceError(("[%s]: Invalid property name\n", __FUNCTION__));
        return RBUS_ERROR_INVALID_INPUT;
    }
    if(strcmp(propertyName, SPEED_BOOST_PORT_RANGE) == 0)
    {
        char prev_val[256] = {0};
        rbusValue_t value = rbusProperty_GetValue(property);
        if(rbusValue_GetType(value) != RBUS_STRING)
        {
            CcspTraceError(("[%s]: Invalid gettype name\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
        strncpy(prev_val, current_speedboost_value, sizeof(prev_val)-1);
        const char* str = rbusValue_GetString(value,NULL);
        if(strlen(str) > 1024){
            CcspTraceError(("[%s]: Invalid string length\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
        if (strcasecmp(prev_val, str) == 0)
        {
            CcspTraceInfo(("[%s]: The value is same as the previous value\n", __FUNCTION__));
            return RBUS_ERROR_SUCCESS;
        }
        boostType = CURRENT_SPEEDBOOST_VALUE;
        int result_value = processSpeedBoostValue(str, prev_val, &current_speedboost_value[0], sizeof(current_speedboost_value), boostType);
        if(result_value != 0)
        {
            return RBUS_ERROR_INVALID_INPUT;  
        }
        if(speed_boost_subscribe)
        {
            publishEventChangeIfSubscribed(SPEED_BOOST_PORT_RANGE, &prev_val, &current_speedboost_value);
        }
    }
    if(strcmp(propertyName, SPEED_BOOST_NORMAL_PORT_RANGE) == 0)
    {
       char prev_val[256] = {0};
        rbusValue_t value = rbusProperty_GetValue(property);
        if(rbusValue_GetType(value) != RBUS_STRING)
        {
            CcspTraceError(("[%s]: Invalid gettype name\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
        strncpy(prev_val, current_normal_speedboost_value, sizeof(prev_val)-1);
      	const char* str = rbusValue_GetString(value,NULL);
        if(strlen(str) > 1024){
            CcspTraceError(("[%s]: Invalid string length\n", __FUNCTION__));
            return RBUS_ERROR_INVALID_INPUT;
        }
        if (strcasecmp(prev_val, str) == 0)
        {
            CcspTraceInfo(("[%s]: The value is same as the previous value\n", __FUNCTION__));
            return RBUS_ERROR_SUCCESS;
        }
        boostType = CURRENT_NORMAL_SPEEDBOOST_VALUE;
      	int result_value = processSpeedBoostValue(str, prev_val, &current_normal_speedboost_value[0], sizeof(current_normal_speedboost_value), CURRENT_NORMAL_SPEEDBOOST_VALUE);
        if(result_value != 0)
        {
            return RBUS_ERROR_INVALID_INPUT;  
        }
        if(speed_boost_normal_subscribe)
        {
            publishEventChangeIfSubscribed(SPEED_BOOST_NORMAL_PORT_RANGE, &prev_val, &current_speedboost_value);
        }
    }
    return RBUS_ERROR_SUCCESS;
}

rbusError_t speed_subStringHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish)
{
    (void)handle;
    (void)filter;
    (void)interval;
    *autoPublish = false;

    CcspTraceInfo(("speed_subStringHandler: action=%s eventName=%s\n", action==RBUS_EVENT_ACTION_SUBSCRIBE?"subscribe":"unsubscribe", eventName));

    if(strcmp(SPEED_BOOST_PORT_RANGE, eventName) == 0 )
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == action )
        {
            speed_boost_subscribe++;
            CcspTraceInfo(("speed_subStringHandler: Subscribed to speedboost, status - %d\n",speed_boost_subscribe));
        }
        else
        {
            if(speed_boost_subscribe > 0)
            {
            	speed_boost_subscribe--;
            	CcspTraceInfo(("speed_subStringHandler: Unsubscribed from speedboost, status - %d\n",speed_boost_subscribe));
            }
        }
    }
    else if(strcmp(SPEED_BOOST_NORMAL_PORT_RANGE, eventName) == 0 )
    {
        if ( RBUS_EVENT_ACTION_SUBSCRIBE == action )
        {
            speed_boost_normal_subscribe++;
            CcspTraceInfo(("speed_subStringHandler: Subscribed to speedboost normal, status - %d\n",speed_boost_normal_subscribe));
        }
        else
        {
            if(speed_boost_normal_subscribe > 0)
            {
            	speed_boost_normal_subscribe--;
            	CcspTraceInfo(("speed_subStringHandler: Unsubscribed from speedboost normal, status - %d\n",speed_boost_normal_subscribe));
            }
        }
    }
    else
    {
        CcspTraceError(("speed_subStringHandler: Invalid event name\n"));
    }
    return RBUS_ERROR_SUCCESS;
}
