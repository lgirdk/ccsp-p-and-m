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

/**************************************************************************

    module: cosa_firewall_internal.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the internal apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        Tom Chang

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

#ifndef  _COSA_FIREWALL_INTERNAL_H
#define  _COSA_FIREWALL_INTERNAL_H

// LGI ADD START
#include "poam_irepfo_interface.h"
#include "sys_definitions.h"
// LGI ADD END
#include "cosa_firewall_apis.h"

// LGI ADD START
#define COSA_IREP_FOLDER_NAME_IP_FILTER_FIREWALL              "IpFilterFirewall"
#define COSA_DML_RR_NAME_IP_FILTER_FIREWALL_Alias             "IpFilterAlias"
#define COSA_DML_RR_NAME_IP_FILTER_FIREWALL_InsNum            "IpFilterInstanceNumber"
#define COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NextInsNumber     "IpFilterNextInstanceNumber"
#define COSA_DML_RR_NAME_IP_FILTER_FIREWALL_NewlyAdded        "IpFilterNewlyAdded"
#define  COSA_IREP_FOLDER_NAME_V4DAYOFWEEK                    "V4DayOfWeek"
#define  COSA_DML_RR_NAME_V4DayOfWeekNextInsNumber            "NextInstanceNumber"
#define  COSA_DML_RR_NAME_V4DayOfWeekInsNumber                "InstanceNumber"
#define  COSA_DML_RR_NAME_V4DayOfWeekAlias                    "V4DayOfWeekAlias"
#define  COSA_IREP_FOLDER_NAME_V6DAYOFWEEK                    "V6DayOfWeek"
#define  COSA_DML_RR_NAME_V6DayOfWeekNextInsNumber            "NextInstanceNumber"
#define  COSA_DML_RR_NAME_V6DayOfWeekInsNumber                "InstanceNumber"
#define  COSA_DML_RR_NAME_V6DayOfWeekAlias                    "V6DayOfWeekAlias"
// LGI ADD END

#define  COSA_DATAMODEL_FIREWALL_CLASS_CONTENT                                              \
    /* duplication of the base object class content */                                      \
    COSA_BASE_CONTENT                                                                       \
    /* start of FIREWALL object class content */                                            \
    COSA_DML_FIREWALL_CFG           FirewallConfig;                                         \
    /* LGI ADD START */                                                                     \
    /* v4 ip filter*/                                                                       \
    ANSC_HANDLE                     hIrepFolderV4IPCOSA;                                    \
    SLIST_HEADER                    FwV4IpFilterList;                                       \
    ULONG                           FwV4IpFilterNextInsNum;                                 \
    ANSC_HANDLE                     hIrepFolderFwV4IpFiler;                                 \
    /* v6 ip filter*/                                                                       \
    ANSC_HANDLE                     hIrepFolderV6IPCOSA;                                    \
    SLIST_HEADER                    FwV6IpFilterList;                                       \
    ULONG                           FwV6IpFilterNextInsNum;                                 \
    ANSC_HANDLE                     hIrepFolderFwV6IpFiler;                                 \
    ULONG                           V4DayOfWeekBlockTimeBitMaskType;                        \
    ULONG                           V6DayOfWeekBlockTimeBitMaskType;                        \
    ANSC_HANDLE                     hIrepFolderV4DayOfWeekCOSA;                             \
    SLIST_HEADER                    V4DayOfWeekList;                                        \
    ULONG                           V4DayOfWeekNextInsNum;                                  \
    ANSC_HANDLE                     hIrepFolderFwV4DayOfWeek;                               \
    ANSC_HANDLE                     hIrepFolderV6DayOfWeekCOSA;                             \
    SLIST_HEADER                    V6DayOfWeekList;                                        \
    ULONG                           V6DayOfWeekNextInsNum;                                  \
    ANSC_HANDLE                     hIrepFolderFwV6DayOfWeek;                               \
    /* LGI ADD END */                                                                       \


typedef  struct
_COSA_DATAMODEL_FIREWALL
{
	COSA_DATAMODEL_FIREWALL_CLASS_CONTENT
}
COSA_DATAMODEL_FIREWALL,  *PCOSA_DATAMODEL_FIREWALL;


/*
 *  Standard function declaration 
 */

ANSC_HANDLE
CosaFirewallCreate
    (
        VOID
    );

ANSC_STATUS
CosaFirewallInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaFirewallRemove
    (
        ANSC_HANDLE                 hThisObject
    );

// LGI ADD START
// V4 IP Filter----------------------------------------------------------------
ANSC_STATUS
CosaFwReg_V4_IpFilterGetInfo(
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaFwReg_V4_IpFilterAddInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    );

ANSC_STATUS
CosaFwReg_V4_IpFilterDelInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    );

ANSC_STATUS
CosaFwReg_V4DayOfWeekGetInfo(
        ANSC_HANDLE                 hThisObject
    );

// V6 IP Filter----------------------------------------------------------------
ANSC_STATUS
CosaFwReg_V6_IpFilterGetInfo(
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaFwReg_V6_IpFilterAddInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    );

ANSC_STATUS
CosaFwReg_V6_IpFilterDelInfo(
        ANSC_HANDLE                 hThisObject,
        ANSC_HANDLE                 hCosaContext
    );

ANSC_STATUS
CosaFwReg_V6DayOfWeekGetInfo(
        ANSC_HANDLE                 hThisObject
    );
// LGI ADD END
#endif
