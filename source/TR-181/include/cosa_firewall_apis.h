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

    module: cosa_firewall_apis.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        Tom Chang

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

/*
 * This file defines data types and methods as the middle layer between COSA DM library
 * and backend implementation.
 */

#ifndef  _COSA_FIREWALL_APIS_H
#define  _COSA_FIREWALL_APIS_H

#include "ansc_platform.h"
#include "cosa_apis.h"
#include "plugin_main_apis.h"


typedef  enum
_COSA_DML_FIREWALL_LEVEL
{
    COSA_DML_FIREWALL_LEVEL_High    = 1,
    COSA_DML_FIREWALL_LEVEL_Medium,         /* Not applicable for phase 4 */
    COSA_DML_FIREWALL_LEVEL_Low,
    COSA_DML_FIREWALL_LEVEL_Custom,      
    COSA_DML_FIREWALL_LEVEL_None
}
COSA_DML_FIREWALL_LEVEL, *PCOSA_DML_FIREWALL_LEVEL;


typedef  struct
_COSA_DML_FIREWALL_CFG
{
    COSA_DML_FIREWALL_LEVEL         FirewallLevel;
    char                            Version[8];
    char                            LastChange[32];
}
COSA_DML_FIREWALL_CFG,  *PCOSA_DML_FIREWALL_CFG;

// LGI ADD START
typedef enum
{
    IPADDR_IPV4 = 1,
    IPADDR_IPV6 = 2,
}
COSA_DML_FW_IPADDR_TYPE;

typedef enum
{
    ACTION_ALLOW = 1,
    ACTION_DENY  = 2,
}
COSA_DML_FW_ACTION;

typedef enum
{
    DIRECTION_INCOMING = 1,
    DIRECTION_OUTGOING = 2,
}
COSA_DML_FW_DIRECTION;

typedef enum
{
    PROTO_TCP = 1,
    PROTO_UDP = 2,
    PROTO_BOTH = 3,
    PROTO_ALL = 4,
    PROTO_ICMPV6 = 5,
    PROTO_ESP = 6,
    PROTO_AH = 7,
    PROTO_GRE = 8,
    PROTO_IPV6ENCAP = 9,
    PROTO_IPV4ENCAP = 10,
    PROTO_IPV6FRAGMENT = 11,
    PROTO_L2TP = 12,
}
COSA_DML_FW_PROTO_TYPE;

/*
 * .Firewall.IpFilter.{i}.
 */
typedef struct
_COSA_DML_FW_IPFILTER
{
    ULONG                      InstanceNumber;
    BOOL                       Enable;
    char                       Alias[256];
    char                       Description[64];
    char                       SrcStartIPAddress[64];
    char                       SrcEndIPAddress[64];
    char                       DstStartIPAddress[64];
    char                       DstEndIPAddress[64];
    ULONG                      SrcStartPort;
    ULONG                      SrcEndPort;
    ULONG                      DstStartPort;
    ULONG                      DstEndPort;
    ULONG                      IPv6SrcPrefixLen;
    ULONG                      IPv6DstPrefixLen;
    COSA_DML_FW_PROTO_TYPE     ProtocolType;
    COSA_DML_FW_ACTION         FilterAction;
    COSA_DML_FW_DIRECTION      FilterDirec;
}
COSA_DML_FW_IPFILTER;

typedef struct
COSA_DML_FW_V4_DAYOFWEEK
{
   ULONG          InstanceNumber;
   char           Alias[256];
   char           V4DayOfWeek_BlockTimeBitMask[25];
}
COSA_DML_FW_V4_DAYOFWEEK;

typedef struct
COSA_DML_FW_V6_DAYOFWEEK
{
   ULONG          InstanceNumber;
   char           Alias[256];
   char           V6DayOfWeek_BlockTimeBitMask[25];
}
COSA_DML_FW_V6_DAYOFWEEK;
// LGI ADD END

/* 
 *  The actual function declaration 
 */

ANSC_STATUS
CosaDmlFirewallInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    );

ANSC_STATUS
CosaDmlFirewallSetConfig
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_FIREWALL_CFG      pCfg
    );

ANSC_STATUS
CosaDmlFirewallGetConfig
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_FIREWALL_CFG      pCfg
    );

// LGI ADD START
ANSC_STATUS
CosaDmlGatewayV4GetFwEnable
    (
        BOOL *pValue
    );
ANSC_STATUS
CosaDmlGatewayV4SetFwEnable
    (
        BOOL bValue
    );
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_GetBlockTimeBitMaskType
    (
        ULONG *pulBlockTimeBitMaskType
    );
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_SetBlockTimeBitMaskType
    (
        ULONG blockTimeBitMaskType
    );
// LGI ADD END

ULONG
CosaDmlGatewayV4GetBlockFragIPPkts
    (
        BOOL *pBool
    );

ULONG
CosaDmlGatewayV4GetPortScanProtect
    (
        BOOL *pBool
    );

ULONG
CosaDmlGatewayV4GetIPFloodDetect
    (
        BOOL *pBool
    );

ULONG
CosaDmlGatewayV4GetICMPFloodDetect
    (
        BOOL *pBool
    );

ANSC_STATUS
CosaDmlGatewayV4GetICMPFloodDetectRate
    (
        ULONG *puLong
    );

ULONG
CosaDmlGatewayV4SetBlockFragIPPkts
    (
        BOOL bValue
    );

ULONG
CosaDmlGatewayV4SetPortScanProtect
    (
        BOOL bValue
    );

ULONG
CosaDmlGatewayV4SetIPFloodDetect
    (
        BOOL bValue
    );

ULONG
CosaDmlGatewayV4SetICMPFloodDetect
    (
        BOOL bValue
    );

ANSC_STATUS
CosaDmlGatewayV4SetICMPFloodDetectRate
    (
        ULONG ulValue
    );

// LGI ADD START
ULONG
CosaDmlFW_V4_IPFilter_GetNumberOfEntries
    (
        void
    );

ANSC_STATUS
CosaDmlFW_V4_IPFilter_GetEntryByIndex
    (
        ULONG                  index,
        COSA_DML_FW_IPFILTER   *pEntry
    );

ANSC_STATUS
CosaDmlFW_V4_IPFilter_SetValues
    (
        ULONG         index,
        ULONG         ins,
        const char    *alias
    );

ANSC_STATUS
CosaDmlFW_V4_IPFilter_AddEntry
    (
        COSA_DML_FW_IPFILTER   *pEntry
    );

ANSC_STATUS
CosaDmlFW_V4_IPFilter_DelEntry
    (
        ULONG ins
    );

ANSC_STATUS
CosaDmlFW_V4_IPFilter_GetConf
    (
        ULONG                    ins,
        COSA_DML_FW_IPFILTER     *pEntry
    );

ANSC_STATUS
CosaDmlFW_V4_IPFilter_SetConf
    (
        ULONG                  ins,
        COSA_DML_FW_IPFILTER   *pEntry
    );

ULONG
CosaDmlFW_V4DayOfWeek_GetNumberOfEntries
    (
        void
    );

ANSC_STATUS
CosaDmlFW_V4DayOfWeek_GetEntryByIndex
    (
        ULONG                       index,
        COSA_DML_FW_V4_DAYOFWEEK    *pEntry
    );

ANSC_STATUS
CosaDmlFW_V4DayOfWeek_SetValues
    (
        ULONG         index,
        ULONG         ins,
        const char    *alias,
        char*         bitmask
    );

ANSC_STATUS
CosaDmlFW_V4DayOfWeek_GetConf
    (
        ULONG                       ins,
        COSA_DML_FW_V4_DAYOFWEEK    *pEntry
    );

ANSC_STATUS
CosaDmlFW_V4DayOfWeek_SetConf
    (
        ULONG                       ins,
        COSA_DML_FW_V4_DAYOFWEEK    *pEntry
    );

ANSC_STATUS
CosaDmlFW_V4DayOfWeek_GetBlockTimeBitMask
    (
        char*  pMask,
        ULONG  *pSize
    );

ANSC_STATUS
CosaDmlGatewayV6GetFwEnable
    (
        BOOL *pValue
    );

ANSC_STATUS
CosaDmlGatewayV6SetFwEnable
    (
        BOOL bValue
    );

ANSC_STATUS
CosaDmlFW_V6DayOfWeek_GetBlockTimeBitMaskType
    (
        ULONG  *pulBlockTimeBitMaskType
    );

ANSC_STATUS
CosaDmlFW_V6DayOfWeek_SetBlockTimeBitMaskType
    (
        ULONG   blockTimeBitMaskType
    );
//LGI ADD END

ULONG
CosaDmlGatewayV6GetBlockFragIPPkts
    (
        BOOL *pBool
    );

ULONG
CosaDmlGatewayV6GetPortScanProtect
    (
        BOOL *pBool
    );

ULONG
CosaDmlGatewayV6GetIPFloodDetect
    (
        BOOL *pBool
    );

ULONG
CosaDmlGatewayV6SetBlockFragIPPkts
    (
        BOOL bValue
    );

ULONG
CosaDmlGatewayV6SetPortScanProtect
    (
        BOOL bValue
    );

ULONG
CosaDmlGatewayV6SetIPFloodDetect
    (
        BOOL bValue
    );

// LGI ADD START
ULONG
CosaDmlFW_V6_IPFilter_GetNumberOfEntries
    (
        void
    );

ANSC_STATUS
CosaDmlFW_V6_IPFilter_GetEntryByIndex
    (
        ULONG                    index,
        COSA_DML_FW_IPFILTER     *pEntry
    );

ANSC_STATUS
CosaDmlFW_V6_IPFilter_SetValues
    (
        ULONG           index,
        ULONG           ins,
        const char      *alias
    );

ANSC_STATUS
CosaDmlFW_V6_IPFilter_AddEntry
    (
        COSA_DML_FW_IPFILTER  *pEntry
    );

ANSC_STATUS
CosaDmlFW_V6_IPFilter_DelEntry
    (
        ULONG ins
    );

ANSC_STATUS
CosaDmlFW_V6_IPFilter_GetConf
    (
        ULONG                ins,
        COSA_DML_FW_IPFILTER *pEntry
    );

ANSC_STATUS
CosaDmlFW_V6_IPFilter_SetConf
    (
        ULONG                   ins,
        COSA_DML_FW_IPFILTER    *pEntry
    );

ULONG
CosaDmlFW_V6DayOfWeek_GetNumberOfEntries
    (
        void
    );

ANSC_STATUS
CosaDmlFW_V6DayOfWeek_GetEntryByIndex
    (
        ULONG                       index,
        COSA_DML_FW_V6_DAYOFWEEK    *pEntry
    );

ANSC_STATUS
CosaDmlFW_V6DayOfWeek_SetValues
    (
        ULONG       index,
        ULONG       ins,
        const char  *alias,
        char*       bitmask
    );

ANSC_STATUS
CosaDmlFW_V6DayOfWeek_GetConf
    (
        ULONG                     ins,
        COSA_DML_FW_V6_DAYOFWEEK  *pEntry
    );

ANSC_STATUS
CosaDmlFW_V6DayOfWeek_SetConf
    (
        ULONG                       ins,
        COSA_DML_FW_V6_DAYOFWEEK    *pEntry
    );

ANSC_STATUS
CosaDmlFW_V6DayOfWeek_GetBlockTimeBitMask
    (
        char* pMask,
        ULONG *pSize
    );
// LGI ADD END
#endif
