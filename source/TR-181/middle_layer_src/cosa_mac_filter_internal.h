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

#ifndef  _COSA_MAC_FILTER_INTERNAL_H
#define  _COSA_MAC_FILTER_INTERNAL_H

#include "poam_irepfo_interface.h"
#include "sys_definitions.h"
#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_mac_filter_apis.h"

#define BT_MASK_LEN                                  45
#define COSA_IREP_FOLDER_NAME_MAC_FILTER_FIREWALL            "MacFilterFirewall"

#define COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_Alias           "MacFilterAlias"
#define COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_InsNum          "MacFilterInstanceNumber"
#define COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_NextInsNumber   "MacFilterNextInstanceNumber"
#define COSA_DML_RR_NAME_MAC_FILTER_FIREWALL_NewlyAdded      "MacFilterNewlyAdded"

#define  COSA_IREP_FOLDER_NAME_MACDAYOFWEEK          "MacDayOfWeek"
#define  COSA_DML_RR_NAME_MacDayOfWeekNextInsNumber  "NextInstanceNumber"
#define  COSA_DML_RR_NAME_MacDayOfWeekInsNumber      "InstanceNumber"
#define  COSA_DML_RR_NAME_MacDayOfWeekAlias          "MacDayOfWeekAlias"

#define  COSA_DATAMODEL_MAC_FILTER_CLASS_CONTENT                          \
    /* start of MAC Filter object class content */                        \
    COSA_BASE_CONTENT                                                     \
    /* MAC Filter*/                                                       \
    ANSC_HANDLE                     hIrepFolderMACCOSA;                   \
    SLIST_HEADER                    MACFilterList;                        \
    ULONG                           MACFilterNextInsNum;                  \
    ANSC_HANDLE                     hIrepFolderFwMACFilter;               \
    ULONG                           MacDayOfWeekBlockTimeBitMaskType;     \
    ANSC_HANDLE                     hIrepFolderMacDayOfWeekCOSA;          \
    SLIST_HEADER                    MacDayOfWeekList;                     \
    ULONG                           MacDayOfWeekNextInsNum;               \
    ANSC_HANDLE                     hIrepFolderFwMacDayOfWeek;            \

typedef  struct
_COSA_DATAMODEL_MAC_FILTER_CLASS_CONTENT
{
    COSA_DATAMODEL_MAC_FILTER_CLASS_CONTENT
    /* start of MAC Filter object class content */
}
COSA_DATAMODEL_MAC_FILTER, *PCOSA_DATAMODEL_MAC_FILTER;

/*
    Standard function declaration
*/
ANSC_HANDLE
CosaMacFilterCreate
    (
        VOID
    );

ANSC_STATUS
CosaMacFilterInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaMacFilterRemove
    (
        ANSC_HANDLE                 hThisObject
    );

//LG MAC Filter
ANSC_STATUS
CosaFwReg_MACFilterGetInfo(
        ANSC_HANDLE                 hThisObject
    );
ANSC_STATUS
CosaFwReg_MACFilterAddInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    );
ANSC_STATUS
CosaFwReg_MACFilterDelInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    );
ANSC_STATUS
CosaFwReg_MacDayOfWeekGetInfo(
        ANSC_HANDLE                 hThisObject
    );
#endif
