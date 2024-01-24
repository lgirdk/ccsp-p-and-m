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
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __MANAGEDWIFI_WEBCONFIG_PARAM_H__
#define __MANAGEDWIFI_WEBCONFIG_PARAM_H__
#include "ccsp_base_api.h"
#include "messagebus_interface_helper.h"
#include "msgpack.h"
#include "webcfgparam.h"
#include "managedwifi_helpers.h"
#include "managedwifidoc.h"
#include "ccsp_trace.h"
#include <trower-base64/base64.h>
#include "cosa_webconfig_api.h"
#include "cosa_rbus_handler_apis.h"
#include <syscfg/syscfg.h>

BOOL unpackAndProcessManagedWifiData(char* pString);
void freeMem_managedwifi(void* arg);
pErr processManagedWebconfigConfig(void *Data);
size_t calculateTimeout(size_t numOfEntries);
pErr create_manage_wifi_bridge(lanconfig_t * pLanConfigParams);
void initManageWiFiBacupStruct(void);

#define BR106_PSM_INDEX "6"
#define MAX_MANAGE_WIFI_BRIDGE_THREAD_EXEC_TIMEOUT 30
#define BUFF_LEN_128 128
#define BUFF_LEN_64 64
#define BUFF_LEN_32 32
#define BUFF_LEN_8  8

#define MANAGE_WIFI_BRIDGE_INDEX "dmsb.MultiLAN.ManageWiFi_l3net"
#define MANAGE_WIFI_LAN_BRIDGE "Device.LAN.Bridge.1.Name"
#define MANAGE_WIFI_ENABLE "Device.X_RDK_LANConfig.ManagedWiFiEnable"
#define MANAGE_WIFI_INTERFACES "Device.LAN.Bridge.1.WiFiInterfaces"

typedef enum
{
    BRIDGE_NAME,
    WIFI_INTERFACES,
    OTHER_UPDATE
}updateType_t;
typedef struct
{
    char aKey[BUFF_LEN_32];
    char aBridgeName[BUFF_LEN_32];
    char aBridgeIndex[BUFF_LEN_8];
    char aWiFiInterfaces[BUFF_LEN_128];
    updateType_t eUpdateType;
}ManageWiFiInfo_t;

typedef struct
{
    bool bMwEnable;
    char aAlias [BUFF_LEN_128];
    bool bDhcpServerEnable;
    char aLanIpAddr [BUFF_LEN_32];
    char aLanSubnetMask [BUFF_LEN_32];
    char aDhcpStartIpAdd [BUFF_LEN_32];
    char aDhcpEndIpAdd [BUFF_LEN_32];
    char aLeaseTime [BUFF_LEN_32];
    bool bIpv6Enable;
    char aWiFiInterfaces[BUFF_LEN_128];
} backupLanconfig_t;

typedef enum
{
    RBUS,
    OTHER_EVENT
}EventType;

typedef enum
{
    PRIVATE,
    XHS,
    MANAGE_WIFI
}InterfaceType_t;

typedef struct
{
    InterfaceType_t eInterfaceType;
    char aIpAddr[BUFF_LEN_32];
    char aStartIpAddr[BUFF_LEN_32];
    char aEndIpAddr[BUFF_LEN_32];
}LanDetails_t;

typedef struct
{
   bool bMwEnable;
   char cFlag;
}threadStruct_t;

void getBridgeDetailsFromPsm(void);
void getManageWiFiDetails(ManageWiFiInfo_t * pManageWifiInfo);
void setManageWiFiDetails(ManageWiFiInfo_t * pManageWifiInfo);
rbusError_t notifyViaRbus(char * pTr181Param, char *pTr181ParamVal);
void processManageWifiData(backupLanconfig_t * pLanConfig, char cFlag, pErr pErrRetVal);
int rollbackLanconfig(void);
int removeSubstring(char * pMainString, char * pSubstring, bool bExtraSpaceRemoval);
void updateBackupConfig(void);
void getManageWiFiEnable(BOOL * pWiFiEnable);
void publishEvent(char * pTr181Param, char *pTr181ParamVal, EventType eEventType);
int confirmManageWifiVap(void);
void restorePreviousPsmValue(void);
int validateIpRange(char *pIpAddr, char *pStartAddr, char *pEndAddr, pErr pErrRetVal);
int validateLeaseTime(char *pLeaseTime, pErr pErrRetVal);
void extractLeaseTime(char *pBegin, char *pEnd, int * pLeaseTime);
void getManageWiFiAddrRange(LanDetails_t * pManageWiFiAddrDetails);
#endif
