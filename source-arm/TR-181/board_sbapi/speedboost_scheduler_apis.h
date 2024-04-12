/*
 * If not stated otherwise in this file or this component's LICENSE file the
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
**********************************************************************/

#ifndef __COSA_SPEEDBOOST_SCHEDULER_APIS_H__
#define __COSA_SPEEDBOOST_SCHEDULER_APIS_H__


#define PAM_RDK_LOG_MODULE     "LOG.RDK.PAM"
#define SPEEDBOOST_DATAFILE    "/nvram/speedboost-mac.dat"
#define SPEEDBOOST_MD5FILE     "/nvram/speedboost-mac.dat.md5"

#define SYSCFG_SPEEDBOOST_CLIENT_COUNT            "SpeedBoost_Clients_Count"
#define SYSCFG_SPEEDBOOST_CLIENT_MAC_PREFIX       "SpeedBoost_Clients_Mac"
#define SYSCFG_SPEEDBOOST_NUM_OF_CONFIG_CLINETS   "SpeedBoost_Num_Of_Config_Clients"

//rdkscheduler callback function
int cleanPrevSyscfgParams(void);
bool triggerFirewallRestart(void);

void speedBoostMacOperation(char* pSpeedBoostMacAddress);

int speedBoostSchdeulerInit(void);


#endif /*__COSA_SPEEDBOOST_SCHEDULER_APIS_H__*/
