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

#include "cosa_rbus_handler_apis.h"

#define UNUSED_PARAMETER(x) (void)(x)

#define ADVERTISEMENT_PVD_ENABLE "Device.RouterAdvertisement.X_RDK_PvD.Enable"
#define ADVERTISEMENT_PVD_H_FLAG "Device.RouterAdvertisement.X_RDK_PvD.H-Flag"
#define ADVERTISEMENT_PVD_DELAY "Device.RouterAdvertisement.X_RDK_PvD.Delay"
#define ADVERTISEMENT_PVD_SEQ_NUMBER "Device.RouterAdvertisement.X_RDK_PvD.SeqNumber"
#define ADVERTISEMENT_PVD_FQDN "Device.RouterAdvertisement.X_RDK_PvD.FQDN"
#define SPEED_BOOST_PORT_RANGE "Device.X_RDK_Speedboost.PortRanges"
#define SPEED_BOOST_NORMAL_PORT_RANGE "Device.X_RDK_Speedboost.NormalPortRange"

// speedboost.h
extern char current_speedboost_value[64];
extern char current_normal_speedboost_value[64];

rbusError_t Pvd_GetBoolHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts);
rbusError_t Pvd_SetBoolHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts);
rbusError_t Pvd_subBoolHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish);
rbusError_t Pvd_GetIntHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts);
rbusError_t Pvd_SetIntHandler(rbusHandle_t handle, rbusProperty_t prop, rbusSetHandlerOptions_t* opts);
rbusError_t Pvd_subIntHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish);
rbusError_t Pvd_GetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts);
rbusError_t Pvd_SetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusSetHandlerOptions_t* opts);
rbusError_t Pvd_subStringHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish);
rbusError_t speed_SetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusSetHandlerOptions_t* opts);
rbusError_t speed_GetStringHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts);
rbusError_t speed_subStringHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char* eventName, rbusFilter_t filter, int32_t interval, bool* autoPublish);

int Publish_PvdValueOnChange(char *dm_event, void *prev_dm_value, void *dm_value, rbusValueType_t rbus_type);
rbusError_t setBlobHandler(rbusHandle_t rbusHandle, rbusProperty_t rbusProperty, rbusSetHandlerOptions_t* pRbusHandleOpts);

rbusError_t getBlobHandler(rbusHandle_t rbusHandle, rbusProperty_t rbusProperty, rbusGetHandlerOptions_t* pRbusHandleOpts);

rbusError_t clientIntSubHandler(rbusHandle_t rbusHandle, rbusEventSubAction_t rbusAction, const char *pEventName, rbusFilter_t rbusFilter, int32_t i32Interval, bool *pAutoPublish);

rbusError_t clientsInfoGetHandler(rbusHandle_t rbusHandle, rbusProperty_t rbusProperty, rbusGetHandlerOptions_t* pRbusHandleOpts);

rbusError_t clientStringSubHandler(rbusHandle_t rbusHandle, rbusEventSubAction_t rbusAction, const char *pEventName, rbusFilter_t rbusFilter, int32_t i32Interval, bool *pAutoPublish);

void publishEventChangeIfSubscribed(const char *pEventName, void *pPrevVal, void *pCurrVal);
