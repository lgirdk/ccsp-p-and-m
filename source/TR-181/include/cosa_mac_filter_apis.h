/*********************************************************************
 * Copyright 2017-2019 ARRIS Enterprises, LLC.
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

#ifndef  _COSA_MAC_FILTER_APIS_H
#define  _COSA_MAC_FILTER_APIS_H

#include "../middle_layer_src/cosa_apis.h"
/*
 * .Firewall.MACFiltering.Device.{i}.
 */

typedef struct
_COSA_DML_FW_MACFILTER
{
   ULONG          InstanceNumber;
   char           Alias[256];
   char           Hostname[65];
   char           MACAddress[64];
   char           Description[64];
   BOOL           Enable;
}
COSA_DML_FW_MACFILTER;

typedef struct
COSA_DML_FW_MAC_DAYOFWEEK
{
   ULONG          InstanceNumber;
   char           Alias[256];
   char           MacDayOfWeek_BlockTimeBitMask[25];
}
COSA_DML_FW_MAC_DAYOFWEEK;

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/
ULONG CosaDmlFW_MACFilter_GetNumberOfEntries(void);
ANSC_STATUS CosaDmlFW_MACFilter_GetEntryByIndex(ULONG index, COSA_DML_FW_MACFILTER *pEntry);
ANSC_STATUS CosaDmlFW_MACFilter_SetValues(ULONG index, ULONG ins, const char *alias);
ANSC_STATUS CosaDmlFW_MACFilter_AddEntry(COSA_DML_FW_MACFILTER *pEntry);
ANSC_STATUS CosaDmlFW_MACFilter_DelEntry(ULONG ins);
ANSC_STATUS CosaDmlFW_MACFilter_GetConf(ULONG ins, COSA_DML_FW_MACFILTER *pEntry);
ANSC_STATUS CosaDmlFW_MACFilter_SetConf(ULONG ins, COSA_DML_FW_MACFILTER *pEntry);
ANSC_STATUS CosaDmlFW_MACDayOfWeek_GetBlockTimeBitMaskType(ULONG *pulBlockTimeBitMaskType);
ANSC_STATUS CosaDmlFW_MACDayOfWeek_SetBlockTimeBitMaskType(ULONG blockTimeBitMaskType);
ULONG CosaDmlFW_MacDayOfWeek_GetNumberOfEntries(void);
ANSC_STATUS CosaDmlFW_MacDayOfWeek_GetEntryByIndex(ULONG index, COSA_DML_FW_MAC_DAYOFWEEK *pEntry);
ANSC_STATUS CosaDmlFW_MacDayOfWeek_SetValues(ULONG index, ULONG ins, const char *alias, char* bitmask);
ANSC_STATUS CosaDmlFW_MacDayOfWeek_GetConf(ULONG ins, COSA_DML_FW_MAC_DAYOFWEEK *pEntry);
ANSC_STATUS CosaDmlFW_MacDayOfWeek_SetConf(ULONG ins, COSA_DML_FW_MAC_DAYOFWEEK *pEntry);
ANSC_STATUS CosaDmlFW_MacDayOfWeek_GetBlockTimeBitMask(char* pMask, ULONG *pSize);
#endif
