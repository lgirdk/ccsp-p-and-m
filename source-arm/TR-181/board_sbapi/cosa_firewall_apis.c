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

    module: cosa_firewall_apis.c

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

#include "cosa_firewall_apis.h"
// LGI ADD START
#include <ctype.h>
#include <utctx/utctx.h>
#include <utapi/utapi.h>
#include <utapi/utapi_util.h>
#include "cosa_drg_common.h"
#include <syscfg/syscfg.h>
// LGI ADD END
#define V4_BLOCKFRAGIPPKT   "v4_BlockFragIPPkts"
#define V4_PORTSCANPROTECT  "v4_PortScanProtect"
#define V4_IPFLOODDETECT    "v4_IPFloodDetect"
#define V4_ICMPFLOODDETECT  "v4_ICMPFloodDetect"
#define V4_ICMPFLOODDETECTRATE "v4_ICMPFloodDetectRate"

#define V6_BLOCKFRAGIPPKT   "v6_BlockFragIPPkts"
#define V6_PORTSCANPROTECT  "v6_PortScanProtect"
#define V6_IPFLOODDETECT    "v6_IPFloodDetect"
// LGI ADD START
#define V4_FIREWALL_LEVEL  "firewall_level"
#define V6_FIREWALL_LEVEL  "firewall_levelv6"
#define BUFF_STR_LEN        16
// LGI ADD END

#ifdef _COSA_SIM_

COSA_DML_FIREWALL_CFG g_FirewallConfig = 
{
    COSA_DML_FIREWALL_LEVEL_Low, 
    "1.0",
    "2011-01-01T00:00:00Z"
};


/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFirewallInit
            (
                ANSC_HANDLE                 hDml,
                PANSC_HANDLE                phContext
            )

    Description:

        Backend implementation to get Firewall configuration.

    Arguments:    ANSC_HANDLE                 hDml
                  Reserved.

                  PANSC_HANDLE                phContext
                  Pointer of the configuration to be retrieved.

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFirewallInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFirewallGetConfig
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_FIREWALL_CFG      pCfg
            );

    Description:

        Backend implementation to get Firewall configuration.

    Arguments:    ANSC_HANDLE                 hContext
                  Reserved.

                  PCOSA_DML_FIREWALL_CFG      pCfg
                  Pointer of the configuration to be retrieved.

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFirewallGetConfig
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_FIREWALL_CFG      pCfg
    )
{
    pCfg->FirewallLevel = g_FirewallConfig.FirewallLevel;
    AnscCopyString(pCfg->Version,    g_FirewallConfig.Version);
    AnscCopyString(pCfg->LastChange, g_FirewallConfig.LastChange);

    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFirewallSetConfig
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_FIREWALL_CFG      pCfg
            );

    Description:

        Backend implementation to set Firewall configuration.

    Arguments:    ANSC_HANDLE                 hContext
                  Reserved.

                  PCOSA_DML_FIREWALL_CFG      pCfg
                  Pointer of the configuration to be set.

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFirewallSetConfig
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_FIREWALL_CFG      pCfg
    )
{
    g_FirewallConfig.FirewallLevel = pCfg->FirewallLevel;

    return ANSC_STATUS_SUCCESS;
}

#elif (_COSA_INTEL_USG_ARM_ || _COSA_DRG_TPG_ || _COSA_BCM_MIPS_)

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFirewallInit
            (
                ANSC_HANDLE                 hDml,
                PANSC_HANDLE                phContext
            )

    Description:

        Backend implementation to get Firewall configuration.

    Arguments:    ANSC_HANDLE                 hDml
                  Reserved.

                  PANSC_HANDLE                phContext
                  Pointer of the configuration to be retrieved.

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFirewallInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFirewallGetConfig
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_FIREWALL_CFG      pCfg
            );

    Description:

        Backend implementation to get Firewall configuration.

    Arguments:    ANSC_HANDLE                 hContext
                  Reserved.

                  PCOSA_DML_FIREWALL_CFG      pCfg
                  Pointer of the configuration to be retrieved.

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFirewallGetConfig
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_FIREWALL_CFG      pCfg
    )
{
    return ANSC_STATUS_SUCCESS;
}


/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFirewallSetConfig
            (
                ANSC_HANDLE                 hContext,
                PCOSA_DML_FIREWALL_CFG      pCfg
            );

    Description:

        Backend implementation to set Firewall configuration.

    Arguments:    ANSC_HANDLE                 hContext
                  Reserved.

                  PCOSA_DML_FIREWALL_CFG      pCfg
                  Pointer of the configuration to be set.

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFirewallSetConfig
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_FIREWALL_CFG      pCfg
    )
{
    return ANSC_STATUS_SUCCESS;
}

// LGI ADD START
/*
 * V4 Ip Filter
 */
/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4GetFwEnable
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V4 Firewall Enable value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlGatewayV4GetFwEnable
    (
        BOOL        *pValue
    )
{
    char buf[64];
    memset(buf, 0, sizeof(buf));

    if (syscfg_init() == 0) {
        syscfg_get( NULL, V4_FIREWALL_LEVEL, buf, sizeof(buf));
        if (strcmp(buf, "None") == 0) {
            *pValue = false;
        }
        else {
            *pValue = true;
        }
        return ANSC_STATUS_SUCCESS;
    }
    return ANSC_STATUS_FAILURE;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4SetFwEnable
            (
                BOOL                        bValue
            );

    Description:

        Backend implementation to set V4 Firewall Enable value to syscfg

    Arguments:    BOOL                     bValue

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlGatewayV4SetFwEnable
    (
        BOOL          bValue
    )
{
    char buf[64];

    if (syscfg_init() != 0) {
        return ANSC_STATUS_FAILURE;
    }

    if (bValue == false) {
        if (syscfg_set(NULL, V4_FIREWALL_LEVEL, "None") == 0 )
            syscfg_commit();
    }
    else {
        memset(buf, 0, sizeof(buf));
        syscfg_get( NULL, V4_FIREWALL_LEVEL, buf, sizeof(buf));
        if (strcmp(buf, "None") == 0) {
            if( syscfg_set(NULL, V4_FIREWALL_LEVEL, "Low" ) == 0)
                syscfg_commit();
        }
    }
    return ANSC_STATUS_SUCCESS;
}
// LGI ADD END

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4GetBlockFragIPPkts
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V4_BLOCKFRAGIPPKT value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV4GetBlockFragIPPkts
(
    BOOL                        *pValue
)
{
    char buf[64];
    memset(buf, 0, sizeof(buf));
    syscfg_get( NULL, V4_BLOCKFRAGIPPKT, buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4GetPortScanProtect
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V4_PORTSCANPROTECT value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV4GetPortScanProtect
(
    BOOL                        *pValue
)
{
    char buf[64];
    memset(buf, 0, sizeof(buf));
    syscfg_get( NULL, V4_PORTSCANPROTECT, buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);
    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4GetIPFloodDetect
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V4_IPFLOODDETECT value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV4GetIPFloodDetect
(
   BOOL                        *pValue
)
{
    char buf[64];
    memset(buf, 0, sizeof(buf));
    syscfg_get( NULL, V4_IPFLOODDETECT, buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);
    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGatewayV4GetICMPFloodDetect
(
   BOOL                        *pValue
)
{
    char buf[8];

    syscfg_get( NULL, V4_ICMPFLOODDETECT, buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGatewayV4GetICMPFloodDetectRate
(
   ULONG                       *puLong
)
{
   char buf[12];

   syscfg_get( NULL, V4_ICMPFLOODDETECTRATE, buf, sizeof(buf));
   *puLong = atoi(buf);

   return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4SetBlockFragIPPkts
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to set V4_BLOCKFRAGIPPKT value to syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV4SetBlockFragIPPkts
(
    BOOL                        bValue
)
{
    if( syscfg_set(NULL, V4_BLOCKFRAGIPPKT, bValue ? "1" : "0") == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4SetPortScanProtect
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to set V4_PORTSCANPROTECT value to syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV4SetPortScanProtect
(
    BOOL                        bValue
)
{
    if( syscfg_set(NULL, V4_PORTSCANPROTECT, bValue ? "1" : "0") == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV4SetIPFloodDetect
            (
                BOOL                        bValue
            );

    Description:

        Backend implementation to set V4_IPFLOODDETECT value to syscfg

    Arguments:    BOOL                     bValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV4SetIPFloodDetect
(
    BOOL                        bValue
)
{
    if( syscfg_set(NULL, V4_IPFLOODDETECT, bValue ? "1" : "0") == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlGatewayV4SetICMPFloodDetect
(
    BOOL                        bValue
)
{
    if( syscfg_set(NULL, V4_ICMPFLOODDETECT, bValue ? "1" : "0") == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlGatewayV4SetICMPFloodDetectRate
(
   ULONG                       ulValue
)
{
    char buf[12];

    sprintf(buf, "%d", ulValue);

    if( syscfg_set(NULL, V4_ICMPFLOODDETECTRATE, buf) == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

// LGI ADD START
/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4DayOfWeek_GetBlockTimeBitMaskType
            (
                ULONG              *pulBlockTimeBitMaskType
            );

    Description:

        Backend implementation to get V4 Schedule Enable value from syscfg

    Arguments:    ULONG              *pulBlockTimeBitMaskType

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_GetBlockTimeBitMaskType
    (
        ULONG *pulBlockTimeBitMaskType
    )
{
    char buf[BUFF_STR_LEN] = {0};

    if (syscfg_init() == 0) {
        syscfg_get(NULL, "v4_dayofweek_block_time_bitmask_type", buf, sizeof(buf));
        *pulBlockTimeBitMaskType = atoi(buf);
        return ANSC_STATUS_SUCCESS;
    }
    return ANSC_STATUS_FAILURE;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4DayOfWeek_SetBlockTimeBitMaskType
            (
                ULONG              blockTimeBitMaskType
            );

    Description:

        Backend implementation to set V4 Schedule Enable value to syscfg

    Arguments:    ULONG              blockTimeBitMaskType

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_SetBlockTimeBitMaskType
    (
        ULONG blockTimeBitMaskType
    )
{
    char buf[BUFF_STR_LEN] = {0};

    sprintf(buf, "%lu", blockTimeBitMaskType);

    if (syscfg_init() != 0) {
        return ANSC_STATUS_FAILURE;
    }

    if(syscfg_set(NULL, "v4_dayofweek_block_time_bitmask_type", buf) != 0) {
        return ANSC_STATUS_FAILURE;
    }
    syscfg_commit();
    return ANSC_STATUS_SUCCESS;
}

static int g_NrFwV4IpFilter =  0;
/**********************************************************************

    caller:     COSA DML

    prototype:

        static int
        FW_V4_IPFilter_InsGetIndex
            (
                ULONG      ins
            );

    Description:

        Backend implementation to get instance index for IPv4.

    Arguments:    ULONG      ins

    Return:       The status of the operation.

**********************************************************************/
static int
FW_V4_IPFilter_InsGetIndex
    (
        ULONG       ins
    )
{
    int i, ins_num, ret = -1;
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    for (i = 0; i < g_NrFwV4IpFilter; i++)
    {
        Utopia_GetV4IpFilterInsNumByIndex(&ctx, i, &ins_num);
        if (ins_num == ins) {
            ret = i;
            break;
        }
    }

    Utopia_Free(&ctx, 0);
    return ret;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlFW_V4_IPFilter_GetNumberOfEntries
            (
                void
            );

    Description:

        Backend implementation to get number of entries for IPv4.

    Arguments:    void

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlFW_V4_IPFilter_GetNumberOfEntries
    (
        void
    )
{
    UtopiaContext ctx;

    if(!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfV4IpFilter(&ctx, &g_NrFwV4IpFilter);

    Utopia_Free(&ctx, 0);

    return g_NrFwV4IpFilter;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4_IPFilter_GetEntryByIndex
            (
                ULONG                    index,
                COSA_DML_FW_IPFILTER     *pEntry
            );

    Description:

        Backend implementation to get entry by index for IPv4.

    Arguments:    ULONG                    index,
                  COSA_DML_FW_IPFILTER     *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4_IPFilter_GetEntryByIndex
    (
        ULONG                    index,
        COSA_DML_FW_IPFILTER     *pEntry
    )
{
    UtopiaContext ctx;
    fwipfilter_t IpFilter;

    if (index >= g_NrFwV4IpFilter || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetV4IpFilterByIndex(&ctx, index, &IpFilter);

    pEntry->InstanceNumber = IpFilter.InstanceNumber;
    pEntry->Enable = IpFilter.Enable;
    pEntry->SrcStartPort = IpFilter.SrcStartPort;
    pEntry->SrcEndPort = IpFilter.SrcEndPort;
    pEntry->DstStartPort = IpFilter.DstStartPort;
    pEntry->DstEndPort = IpFilter.DstEndPort;

    _ansc_strncpy(pEntry->Alias, IpFilter.Alias, sizeof(pEntry->Alias));
    _ansc_strncpy(pEntry->Description, IpFilter.Description, sizeof(pEntry->Description));

    _ansc_strncpy(pEntry->SrcStartIPAddress, IpFilter.SrcStartIPAddress, sizeof(pEntry->SrcStartIPAddress));
    _ansc_strncpy(pEntry->SrcEndIPAddress, IpFilter.SrcEndIPAddress, sizeof(pEntry->SrcEndIPAddress));
    _ansc_strncpy(pEntry->DstStartIPAddress, IpFilter.DstStartIPAddress, sizeof(pEntry->DstStartIPAddress));
    _ansc_strncpy(pEntry->DstEndIPAddress, IpFilter.DstEndIPAddress, sizeof(pEntry->DstEndIPAddress));

    if(!_ansc_strcmp(IpFilter.FilterAction, "ALLOW"))
        pEntry->FilterAction = ACTION_ALLOW;
    else
        pEntry->FilterAction = ACTION_DENY;

    if(!_ansc_strcmp(IpFilter.FilterDirec, "INCOMING"))
        pEntry->FilterDirec = DIRECTION_INCOMING;
    else
        pEntry->FilterDirec = DIRECTION_OUTGOING;

    if(!_ansc_strcmp(IpFilter.ProtocolType, "TCP"))
        pEntry->ProtocolType = PROTO_TCP;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "UDP"))
        pEntry->ProtocolType = PROTO_UDP;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "BOTH"))
        pEntry->ProtocolType = PROTO_BOTH;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "All"))
        pEntry->ProtocolType = PROTO_ALL;
    else
        pEntry->ProtocolType = 0;

    Utopia_Free(&ctx, 0);

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4_IPFilter_SetValues
            (
                ULONG           index,
                ULONG           ins,
                const char      *alias
            );

    Description:

        Backend implementation to set values for IPv4.

    Arguments:  ULONG           index,
                ULONG           ins,
                const char      *alias

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4_IPFilter_SetValues
    (
        ULONG           index,
        ULONG           ins,
        const char      *alias
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (index >= g_NrFwV4IpFilter || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_SetV4IpFilterInsAndAliasByIndex(&ctx, index, ins, alias);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
        return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4_IPFilter_AddEntry
            (
                COSA_DML_FW_IPFILTER          *pEntry
            );

    Description:

        Backend implementation to add entry for IPv4.

    Arguments:   COSA_DML_FW_IPFILTER          *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4_IPFilter_AddEntry
    (
        COSA_DML_FW_IPFILTER          *pEntry
    )
{
    int rc = -1;
    UtopiaContext ctx;
    fwipfilter_t IpFilter;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    IpFilter.InstanceNumber = pEntry->InstanceNumber;
    IpFilter.Enable = pEntry->Enable;
    IpFilter.SrcStartPort = pEntry->SrcStartPort;
    IpFilter.SrcEndPort = pEntry->SrcEndPort;
    IpFilter.DstStartPort = pEntry->DstStartPort;
    IpFilter.DstEndPort = pEntry->DstEndPort;

    _ansc_strncpy(IpFilter.Alias, pEntry->Alias, sizeof(IpFilter.Alias));
    _ansc_strncpy(IpFilter.Description, pEntry->Description, sizeof(IpFilter.Description));

    _ansc_strncpy(IpFilter.SrcStartIPAddress, pEntry->SrcStartIPAddress, sizeof(IpFilter.SrcStartIPAddress));
    _ansc_strncpy(IpFilter.SrcEndIPAddress, pEntry->SrcEndIPAddress, sizeof(IpFilter.SrcEndIPAddress));
    _ansc_strncpy(IpFilter.DstStartIPAddress, pEntry->DstStartIPAddress, sizeof(IpFilter.DstStartIPAddress));
    _ansc_strncpy(IpFilter.DstEndIPAddress, pEntry->DstEndIPAddress, sizeof(IpFilter.DstEndIPAddress));

    if(pEntry->FilterAction == ACTION_ALLOW)
        _ansc_strcpy(IpFilter.FilterAction, "ALLOW");
    else
        _ansc_strcpy(IpFilter.FilterAction, "DENY");

    if(pEntry->FilterDirec == DIRECTION_INCOMING)
        _ansc_strcpy(IpFilter.FilterDirec, "INCOMING");
    else
        _ansc_strcpy(IpFilter.FilterDirec, "OUTGOING");

    if(pEntry->ProtocolType == PROTO_TCP)
        _ansc_strcpy(IpFilter.ProtocolType, "TCP");
    else if(pEntry->ProtocolType == PROTO_UDP)
        _ansc_strcpy(IpFilter.ProtocolType, "UDP");
    else if(pEntry->ProtocolType == PROTO_BOTH)
        _ansc_strcpy(IpFilter.ProtocolType, "BOTH");
    else if(pEntry->ProtocolType == PROTO_ALL)
        _ansc_strcpy(IpFilter.ProtocolType, "All");
    else
        _ansc_strcpy(IpFilter.ProtocolType, "");

    rc = Utopia_AddV4IpFilter(&ctx, &IpFilter);

    Utopia_GetNumberOfV4IpFilter(&ctx, &g_NrFwV4IpFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }

}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4_IPFilter_DelEntry
            (
                ULONG        ins
            );

    Description:

        Backend implementation to delete entry for IPv4.

    Arguments:  ULONG        ins

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4_IPFilter_DelEntry
    (
        ULONG        ins
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_DelV4IpFilter(&ctx, ins);
    Utopia_GetNumberOfV4IpFilter(&ctx, &g_NrFwV4IpFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4_IPFilter_GetConf
            (
                ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry
            );

    Description:

        Backend implementation to get configuration for IPv4.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4_IPFilter_GetConf
    (
        ULONG                       ins,
        COSA_DML_FW_IPFILTER        *pEntry
    )
{
    int index;

    if ((index = FW_V4_IPFilter_InsGetIndex(ins)) == -1)
        return ANSC_STATUS_FAILURE;

    return CosaDmlFW_V4_IPFilter_GetEntryByIndex(index, pEntry);
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4_IPFilter_SetConf
            (
                ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry
            );

    Description:

        Backend implementation to set configuration for IPv4.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4_IPFilter_SetConf
    (
        ULONG                        ins,
        COSA_DML_FW_IPFILTER         *pEntry
    )
{
    int index;
    UtopiaContext ctx;
    fwipfilter_t IpFilter;
    int rc = -1;

    if ((index = FW_V4_IPFilter_InsGetIndex(ins)) == -1 || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    index = FW_V4_IPFilter_InsGetIndex(ins);

    IpFilter.InstanceNumber = pEntry->InstanceNumber;
    IpFilter.Enable = pEntry->Enable;
    IpFilter.SrcStartPort = pEntry->SrcStartPort;
    IpFilter.SrcEndPort = pEntry->SrcEndPort;
    IpFilter.DstStartPort = pEntry->DstStartPort;
    IpFilter.DstEndPort = pEntry->DstEndPort;

    _ansc_strncpy(IpFilter.Alias, pEntry->Alias, sizeof(IpFilter.Alias));
    _ansc_strncpy(IpFilter.Description, pEntry->Description, sizeof(IpFilter.Description));

    _ansc_strncpy(IpFilter.SrcStartIPAddress, pEntry->SrcStartIPAddress, sizeof(IpFilter.SrcStartIPAddress));
    _ansc_strncpy(IpFilter.SrcEndIPAddress, pEntry->SrcEndIPAddress, sizeof(IpFilter.SrcEndIPAddress));
    _ansc_strncpy(IpFilter.DstStartIPAddress, pEntry->DstStartIPAddress, sizeof(IpFilter.DstStartIPAddress));
    _ansc_strncpy(IpFilter.DstEndIPAddress, pEntry->DstEndIPAddress, sizeof(IpFilter.DstEndIPAddress));

    if(pEntry->FilterAction == ACTION_ALLOW)
        _ansc_strcpy(IpFilter.FilterAction, "ALLOW");
    else
        _ansc_strcpy(IpFilter.FilterAction, "DENY");

    if(pEntry->FilterDirec == DIRECTION_INCOMING)
        _ansc_strcpy(IpFilter.FilterDirec, "INCOMING");
    else
        _ansc_strcpy(IpFilter.FilterDirec, "OUTGOING");

    if(pEntry->ProtocolType == PROTO_TCP)
        _ansc_strcpy(IpFilter.ProtocolType, "TCP");
    else if(pEntry->ProtocolType == PROTO_UDP)
        _ansc_strcpy(IpFilter.ProtocolType, "UDP");
    else if(pEntry->ProtocolType == PROTO_BOTH)
        _ansc_strcpy(IpFilter.ProtocolType, "BOTH");
    else if(pEntry->ProtocolType == PROTO_ALL)
        _ansc_strcpy(IpFilter.ProtocolType, "All");
    else
        _ansc_strcpy(IpFilter.ProtocolType, "");

    rc = Utopia_SetV4IpFilterByIndex(&ctx, index, &IpFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");

        return ANSC_STATUS_SUCCESS;
    }
}

static int g_NrFwV4DayOfWeek = 0;
/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlFW_V4DayOfWeek_GetNumberOfEntries
            (
                void
            );

    Description:

        Backend implementation to get number of entries for DayOfWeek of IPv4.

    Arguments:    void

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlFW_V4DayOfWeek_GetNumberOfEntries
    (
        void
    )
{
    UtopiaContext ctx;
    if(!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfV4DayOfWeek(&ctx, &g_NrFwV4DayOfWeek);
    Utopia_Free(&ctx, 0);

    return g_NrFwV4DayOfWeek;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4DayOfWeek_GetEntryByIndex
            (
                ULONG                    index,
                COSA_DML_FW_V4_DAYOFWEEK *pEntry
            );

    Description:

        Backend implementation to get entry by index for DayOfWeek of IPv4.

    Arguments:    ULONG                    index,
                  COSA_DML_FW_V4_DAYOFWEEK *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_GetEntryByIndex
    (
        ULONG                         index,
        COSA_DML_FW_V4_DAYOFWEEK      *pEntry
    )
{
    UtopiaContext ctx;
    fwv4dayofweek_t V4DayOfWeek;

    if (index >= g_NrFwV4DayOfWeek || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetV4DayOfWeekByIndex(&ctx, index, &V4DayOfWeek);

    pEntry->InstanceNumber = V4DayOfWeek.InstanceNumber;

    _ansc_strncpy(pEntry->V4DayOfWeek_BlockTimeBitMask, V4DayOfWeek.V4DayOfWeek_BlockTimeBitMask, sizeof(pEntry->V4DayOfWeek_BlockTimeBitMask)-1);
    _ansc_strncpy(pEntry->Alias, V4DayOfWeek.Alias, sizeof(pEntry->Alias)-1);

    Utopia_Free(&ctx, 0);
    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4DayOfWeek_SetValues
            (
                ULONG           index,
                ULONG           ins,
                const char      *alias,
                char*          bitmask
            );

    Description:

        Backend implementation to set values for DayOfWeek of IPv4.

    Arguments:  ULONG           index,
                ULONG           ins,
                const char      *alias,
                char*          bitmask

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_SetValues
    (
        ULONG          index,
        ULONG          ins,
        const char     *alias,
        char*          bitmask
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (index >= g_NrFwV4DayOfWeek || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_SetV4DayOfWeekInsAndAliasByIndex(&ctx, index, ins, alias, bitmask);
    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
        return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4DayOfWeek_GetConf
            (
                ULONG                       ins,
                COSA_DML_FW_V4_DAYOFWEEK    *pEntry
            );

    Description:

        Backend implementation to get configuration for DayOfWeek of IPv4.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_V4_DAYOFWEEK    *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_GetConf
    (
        ULONG                            ins,
        COSA_DML_FW_V4_DAYOFWEEK         *pEntry
    )
{
    return CosaDmlFW_V4DayOfWeek_GetEntryByIndex(ins, pEntry);
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V4DayOfWeek_SetConf
            (
                ULONG                       ins,
                COSA_DML_FW_V4_DAYOFWEEK    *pEntry
            );

    Description:

        Backend implementation to set configuration for DayOFWeek of IPv4.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_V4_DAYOFWEEK    *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V4DayOfWeek_SetConf
    (
        ULONG                            ins,
        COSA_DML_FW_V4_DAYOFWEEK         *pEntry
    )
{
    UtopiaContext ctx;
    fwv4dayofweek_t V4DayOfWeek;
    int rc = -1;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    V4DayOfWeek.InstanceNumber = pEntry->InstanceNumber;

    _ansc_strncpy(V4DayOfWeek.V4DayOfWeek_BlockTimeBitMask, pEntry->V4DayOfWeek_BlockTimeBitMask, sizeof(V4DayOfWeek.V4DayOfWeek_BlockTimeBitMask)-1);
    _ansc_strncpy(V4DayOfWeek.Alias, pEntry->Alias, sizeof(V4DayOfWeek.Alias)-1);

    rc = Utopia_SetV4DayOfWeekByIndex(&ctx, ins, &V4DayOfWeek);
    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }
}

/*
 * V6 Ip Filter
 */
/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6GetFwEnable
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V6 Firewall Enable value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlGatewayV6GetFwEnable
    (
        BOOL      *pValue
    )
{
    char buf[64];
    memset(buf, 0, sizeof(buf));

    if (syscfg_init() == 0) {
        syscfg_get( NULL, V6_FIREWALL_LEVEL, buf, sizeof(buf));
        if (strcmp(buf, "None") == 0) {
            *pValue = false;
        }
        else {
            *pValue = true;
        }
        return ANSC_STATUS_SUCCESS;
    }
    return ANSC_STATUS_FAILURE;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6SetFwEnable
            (
                BOOL                        bValue
            );

    Description:

        Backend implementation to set V6 Firewall Enable value to syscfg

    Arguments:    BOOL                     bValue

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlGatewayV6SetFwEnable
    (
        BOOL        bValue
    )
{
    char buf[64];

    if (syscfg_init() != 0) {
        return ANSC_STATUS_FAILURE;
    }

    if (bValue == false) {
        if( syscfg_set(NULL, V6_FIREWALL_LEVEL, "None" )  == 0)
            syscfg_commit();
    }
    else {
        memset(buf, 0, sizeof(buf));
        syscfg_get( NULL, V6_FIREWALL_LEVEL, buf, sizeof(buf));
        if (strcmp(buf, "None") == 0) {
            if( syscfg_set(NULL, V6_FIREWALL_LEVEL, "High" ) == 0)
                syscfg_commit();
        }
    }
    return ANSC_STATUS_SUCCESS;
}
// LGI ADD END

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6GetBlockFragIPPkts
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V6_BLOCKFRAGIPPKT value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV6GetBlockFragIPPkts
(
    BOOL                        *pValue
)
{
    char buf[64];
    memset(buf, 0, sizeof(buf));
    syscfg_get( NULL, V6_BLOCKFRAGIPPKT, buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);
    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6GetPortScanProtect
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V6_PORTSCANPROTECT value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV6GetPortScanProtect
(
    BOOL                        *pValue
)
{
    char buf[64];
    memset(buf, 0, sizeof(buf));
    syscfg_get( NULL, V6_PORTSCANPROTECT, buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);
    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6GetIPFloodDetect
            (
                BOOL                        *pValue
            );

    Description:

        Backend implementation to get V6_IPFLOODDETECT value from syscfg

    Arguments:    BOOL                     *pValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV6GetIPFloodDetect
(
    BOOL                        *pValue
)
{
    char buf[64];
    memset(buf, 0, sizeof(buf));
    syscfg_get( NULL, V6_IPFLOODDETECT, buf, sizeof(buf));
    *pValue = (strcmp(buf, "1") == 0);
    return ANSC_STATUS_SUCCESS;

}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6SetBlockFragIPPkts
            (
                BOOL                       bValue
            );

    Description:

        Backend implementation to set V6_BLOCKFRAGIPPKT value to syscfg

    Arguments:    BOOL                     bValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV6SetBlockFragIPPkts
(
    BOOL                        bValue
)
{
    if( syscfg_set(NULL, V6_BLOCKFRAGIPPKT, bValue ? "1" : "0") == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6SetPortScanProtect
            (
                BOOL                        bValue
            );

    Description:

        Backend implementation to set V6_PORTSCANPROTECT value to syscfg

    Arguments:    BOOL                     bValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV6SetPortScanProtect
(
    BOOL                        bValue
)
{
    if( syscfg_set(NULL, V6_PORTSCANPROTECT, bValue ? "1" : "0") == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlGatewayV6SetIPFloodDetect
            (
                BOOL                        bValue
            );

    Description:

        Backend implementation to set V6_IPFLOODDETECT value to syscfg

    Arguments:    BOOL                     bValue

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlGatewayV6SetIPFloodDetect
(
    BOOL                        bValue
)
{
    if( syscfg_set(NULL, V6_IPFLOODDETECT, bValue ? "1" : "0") == 0)
    {
        syscfg_commit();
    }

    return ANSC_STATUS_SUCCESS;
}

// LGI ADD START
/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6DayOfWeek_GetBlockTimeBitMaskType
            (
                ULONG              *pulBlockTimeBitMaskType
            );

    Description:

        Backend implementation to get V6 Schedule Enable value from syscfg

    Arguments:    ULONG              *pulBlockTimeBitMaskType

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6DayOfWeek_GetBlockTimeBitMaskType
    (
        ULONG            *pulBlockTimeBitMaskType
    )
{
    char buf[BUFF_STR_LEN] = {0};

    if (syscfg_init() == 0) {
        syscfg_get(NULL, "v6_dayofweek_block_time_bitmask_type", buf, sizeof(buf));
        *pulBlockTimeBitMaskType = atoi(buf);
        return ANSC_STATUS_SUCCESS;
    }
    return ANSC_STATUS_FAILURE;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6DayOfWeek_SetBlockTimeBitMaskType
            (
                ULONG              blockTimeBitMaskType
            );

    Description:

        Backend implementation to set V6 Schedule Enable value to syscfg

    Arguments:    ULONG              blockTimeBitMaskType

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6DayOfWeek_SetBlockTimeBitMaskType
    (
        ULONG             blockTimeBitMaskType
    )
{
    char buf[BUFF_STR_LEN] = {0};

    sprintf(buf, "%lu", blockTimeBitMaskType);

    if (syscfg_init() != 0) {
        return ANSC_STATUS_FAILURE;
    }

    if(syscfg_set(NULL, "v6_dayofweek_block_time_bitmask_type", buf) != 0) {
        return ANSC_STATUS_FAILURE;
    }
    syscfg_commit();
    return ANSC_STATUS_SUCCESS;
}

static int g_NrFwV6IpFilter =  0;
/**********************************************************************

    caller:     COSA DML

    prototype:

        static int
        FW_V6_IPFilter_InsGetIndex
            (
                ULONG      ins
            );

    Description:

        Backend implementation to get instance index for IPv6.

    Arguments:    ULONG      ins

    Return:       The status of the operation.

**********************************************************************/
static int
FW_V6_IPFilter_InsGetIndex
    (
        ULONG             ins
    )
{
    int i, ins_num, ret = -1;
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    for (i = 0; i < g_NrFwV6IpFilter; i++)
    {
        Utopia_GetV6IpFilterInsNumByIndex(&ctx, i, &ins_num);
        if (ins_num == ins) {
            ret = i;
            break;
        }
    }

    Utopia_Free(&ctx, 0);

    return ret;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlFW_V6_IPFilter_GetNumberOfEntries
            (
                void
            );

    Description:

        Backend implementation to get number of entries for IPv6.

    Arguments:    void

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlFW_V6_IPFilter_GetNumberOfEntries
    (
        void
    )
{
    UtopiaContext ctx;

    if(!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfV6IpFilter(&ctx, &g_NrFwV6IpFilter);

    Utopia_Free(&ctx, 0);

    return g_NrFwV6IpFilter;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6_IPFilter_GetEntryByIndex
            (
                ULONG                    index,
                COSA_DML_FW_IPFILTER     *pEntry
            );

    Description:

        Backend implementation to get entry by index for IPv6.

    Arguments:    ULONG                    index,
                  COSA_DML_FW_IPFILTER     *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6_IPFilter_GetEntryByIndex
    (
        ULONG                      index,
        COSA_DML_FW_IPFILTER       *pEntry
    )
{
    UtopiaContext ctx;
    fwipfilter_t IpFilter;

    if (index >= g_NrFwV6IpFilter || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetV6IpFilterByIndex(&ctx, index, &IpFilter);

    pEntry->InstanceNumber = IpFilter.InstanceNumber;
    pEntry->SrcStartPort = IpFilter.SrcStartPort;
    pEntry->SrcEndPort = IpFilter.SrcEndPort;
    pEntry->DstStartPort = IpFilter.DstStartPort;
    pEntry->DstEndPort = IpFilter.DstEndPort;
    pEntry->IPv6SrcPrefixLen = IpFilter.IPv6SrcPrefixLen;
    pEntry->IPv6DstPrefixLen = IpFilter.IPv6DstPrefixLen;
    pEntry->Enable = IpFilter.Enable;

    _ansc_strncpy(pEntry->Alias, IpFilter.Alias, sizeof(pEntry->Alias));
    _ansc_strncpy(pEntry->Description, IpFilter.Description, sizeof(pEntry->Description));

    _ansc_strncpy(pEntry->SrcStartIPAddress, IpFilter.SrcStartIPAddress, sizeof(pEntry->SrcStartIPAddress));
    _ansc_strncpy(pEntry->DstStartIPAddress, IpFilter.DstStartIPAddress, sizeof(pEntry->DstStartIPAddress));

    if(!_ansc_strcmp(IpFilter.ProtocolType, "TCP"))
        pEntry->ProtocolType = PROTO_TCP;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "UDP"))
        pEntry->ProtocolType = PROTO_UDP;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "BOTH"))
        pEntry->ProtocolType = PROTO_BOTH;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "All"))
        pEntry->ProtocolType = PROTO_ALL;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "ICMPv6"))
        pEntry->ProtocolType = PROTO_ICMPV6;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "ESP"))
        pEntry->ProtocolType = PROTO_ESP;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "AH"))
        pEntry->ProtocolType = PROTO_AH;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "GRE"))
        pEntry->ProtocolType = PROTO_GRE;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "IPv6Encap"))
        pEntry->ProtocolType = PROTO_IPV6ENCAP;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "IPv4Encap"))
        pEntry->ProtocolType = PROTO_IPV4ENCAP;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "IPv6Fragment"))
        pEntry->ProtocolType = PROTO_IPV6FRAGMENT;
    else if(!_ansc_strcmp(IpFilter.ProtocolType, "L2TP"))
        pEntry->ProtocolType = PROTO_L2TP;
    else
        pEntry->ProtocolType = 0;

    if(!_ansc_strcmp(IpFilter.FilterAction, "ALLOW"))
        pEntry->FilterAction = ACTION_ALLOW;
    else if(!_ansc_strcmp(IpFilter.FilterAction, "DENY"))
        pEntry->FilterAction = ACTION_DENY;
    else
        pEntry->FilterAction = 0;

    if(!_ansc_strcmp(IpFilter.FilterDirec, "INCOMING"))
            pEntry->FilterDirec = DIRECTION_INCOMING;
        else if(!_ansc_strcmp(IpFilter.FilterDirec, "OUTGOING"))
            pEntry->FilterDirec = DIRECTION_OUTGOING;
        else
            pEntry->FilterDirec = 0;

    Utopia_Free(&ctx, 0);

    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6_IPFilter_SetValues
            (
                ULONG           index,
                ULONG           ins,
                const char      *alias
            );

    Description:

        Backend implementation to set values for IPv6.

    Arguments:  ULONG           index,
                ULONG           ins,
                const char      *alias

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6_IPFilter_SetValues
    (
        ULONG             index,
        ULONG             ins,
        const char        *alias
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (index >= g_NrFwV6IpFilter || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_SetV6IpFilterInsAndAliasByIndex(&ctx, index, ins, alias);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
        return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6_IPFilter_AddEntry
            (
                COSA_DML_FW_IPFILTER          *pEntry
            );

    Description:

        Backend implementation to add entry for IPv6.

    Arguments:   COSA_DML_FW_IPFILTER          *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6_IPFilter_AddEntry
    (
        COSA_DML_FW_IPFILTER            *pEntry
    )
{
    int rc = -1;
    UtopiaContext ctx;
    fwipfilter_t IpFilter;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    IpFilter.InstanceNumber = pEntry->InstanceNumber;
    IpFilter.SrcStartPort = pEntry->SrcStartPort;
    IpFilter.SrcEndPort = pEntry->SrcEndPort;
    IpFilter.DstStartPort = pEntry->DstStartPort;
    IpFilter.DstEndPort = pEntry->DstEndPort;
    IpFilter.IPv6SrcPrefixLen = pEntry->IPv6SrcPrefixLen;
    IpFilter.IPv6DstPrefixLen = pEntry->IPv6DstPrefixLen ;
    IpFilter.Enable = pEntry->Enable;

    _ansc_strncpy(IpFilter.Alias, pEntry->Alias, sizeof(IpFilter.Alias));
    _ansc_strncpy(IpFilter.Description, pEntry->Description, sizeof(IpFilter.Description));

    _ansc_strncpy(IpFilter.SrcStartIPAddress, pEntry->SrcStartIPAddress, sizeof(IpFilter.SrcStartIPAddress));
    _ansc_strncpy(IpFilter.DstStartIPAddress, pEntry->DstStartIPAddress, sizeof(IpFilter.DstStartIPAddress));

    if(pEntry->ProtocolType == PROTO_TCP)
        _ansc_strcpy(IpFilter.ProtocolType, "TCP");
    else if(pEntry->ProtocolType == PROTO_UDP)
        _ansc_strcpy(IpFilter.ProtocolType, "UDP");
    else if(pEntry->ProtocolType == PROTO_BOTH)
        _ansc_strcpy(IpFilter.ProtocolType, "BOTH");
    else if(pEntry->ProtocolType == PROTO_ALL)
        _ansc_strcpy(IpFilter.ProtocolType, "All");
    else if(pEntry->ProtocolType == PROTO_ICMPV6)
        _ansc_strcpy(IpFilter.ProtocolType, "ICMPv6");
    else if(pEntry->ProtocolType == PROTO_ESP)
        _ansc_strcpy(IpFilter.ProtocolType, "ESP");
    else if(pEntry->ProtocolType == PROTO_AH)
        _ansc_strcpy(IpFilter.ProtocolType, "AH");
    else if(pEntry->ProtocolType == PROTO_GRE)
        _ansc_strcpy(IpFilter.ProtocolType, "GRE");
    else if(pEntry->ProtocolType == PROTO_IPV6ENCAP)
        _ansc_strcpy(IpFilter.ProtocolType, "IPv6Encap");
    else if(pEntry->ProtocolType == PROTO_IPV4ENCAP)
        _ansc_strcpy(IpFilter.ProtocolType, "IPv4Encap");
    else if(pEntry->ProtocolType == PROTO_IPV6FRAGMENT)
        _ansc_strcpy(IpFilter.ProtocolType, "IPv6Fragment");
    else if(pEntry->ProtocolType == PROTO_L2TP)
        _ansc_strcpy(IpFilter.ProtocolType, "L2TP");
    else
        _ansc_strcpy(IpFilter.ProtocolType, "");

    if(pEntry->FilterAction == ACTION_ALLOW)
        _ansc_strcpy(IpFilter.FilterAction, "ALLOW");
    else if(pEntry->FilterAction == ACTION_DENY)
        _ansc_strcpy(IpFilter.FilterAction, "DENY");
    else
        _ansc_strcpy(IpFilter.FilterAction, "");

    if(pEntry->FilterDirec == DIRECTION_INCOMING)
        _ansc_strcpy(IpFilter.FilterDirec, "INCOMING");
    else if(pEntry->FilterDirec == DIRECTION_OUTGOING)
        _ansc_strcpy(IpFilter.FilterDirec, "OUTGOING");
    else
        _ansc_strcpy(IpFilter.FilterDirec, "");

    rc = Utopia_AddV6IpFilter(&ctx, &IpFilter);

    Utopia_GetNumberOfV6IpFilter(&ctx, &g_NrFwV6IpFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6_IPFilter_DelEntry
            (
                ULONG        ins
            );

    Description:

        Backend implementation to delete entry for IPv6.

    Arguments:  ULONG        ins

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6_IPFilter_DelEntry
    (
        ULONG            ins
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_DelV6IpFilter(&ctx, ins);
    Utopia_GetNumberOfV6IpFilter(&ctx, &g_NrFwV6IpFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6_IPFilter_GetConf
            (
                ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry
            );

    Description:

        Backend implementation to get configuration for IPv6.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6_IPFilter_GetConf
    (
        ULONG                        ins,
        COSA_DML_FW_IPFILTER         *pEntry
    )
{
    int index;

    if ((index = FW_V6_IPFilter_InsGetIndex(ins)) == -1)
        return ANSC_STATUS_FAILURE;

    return CosaDmlFW_V6_IPFilter_GetEntryByIndex(index, pEntry);
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6_IPFilter_SetConf
            (
                ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry
            );

    Description:

        Backend implementation to set configuration for IPv6.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_IPFILTER        *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6_IPFilter_SetConf
    (
        ULONG                      ins,
        COSA_DML_FW_IPFILTER       *pEntry
    )
{
    int index;
    UtopiaContext ctx;
    fwipfilter_t IpFilter;
    int rc = -1;

    if ((index = FW_V6_IPFilter_InsGetIndex(ins)) == -1 || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    index = FW_V6_IPFilter_InsGetIndex(ins);

    IpFilter.InstanceNumber = pEntry->InstanceNumber;
    IpFilter.SrcStartPort = pEntry->SrcStartPort;
    IpFilter.SrcEndPort = pEntry->SrcEndPort;
    IpFilter.DstStartPort = pEntry->DstStartPort;
    IpFilter.DstEndPort = pEntry->DstEndPort;
    IpFilter.IPv6SrcPrefixLen = pEntry->IPv6SrcPrefixLen;
    IpFilter.IPv6DstPrefixLen = pEntry->IPv6DstPrefixLen ;
    IpFilter.Enable = pEntry->Enable;

    _ansc_strncpy(IpFilter.Alias, pEntry->Alias, sizeof(IpFilter.Alias));
    _ansc_strncpy(IpFilter.Description, pEntry->Description, sizeof(IpFilter.Description));

    _ansc_strncpy(IpFilter.SrcStartIPAddress, pEntry->SrcStartIPAddress, sizeof(IpFilter.SrcStartIPAddress));
    _ansc_strncpy(IpFilter.DstStartIPAddress, pEntry->DstStartIPAddress, sizeof(IpFilter.DstStartIPAddress));

    if(pEntry->ProtocolType == PROTO_TCP)
        _ansc_strcpy(IpFilter.ProtocolType, "TCP");
    else if(pEntry->ProtocolType == PROTO_UDP)
        _ansc_strcpy(IpFilter.ProtocolType, "UDP");
    else if(pEntry->ProtocolType == PROTO_BOTH)
        _ansc_strcpy(IpFilter.ProtocolType, "BOTH");
    else if(pEntry->ProtocolType == PROTO_ALL)
        _ansc_strcpy(IpFilter.ProtocolType, "All");
    else if(pEntry->ProtocolType == PROTO_ICMPV6)
        _ansc_strcpy(IpFilter.ProtocolType, "ICMPv6");
    else if(pEntry->ProtocolType == PROTO_ESP)
        _ansc_strcpy(IpFilter.ProtocolType, "ESP");
    else if(pEntry->ProtocolType == PROTO_AH)
        _ansc_strcpy(IpFilter.ProtocolType, "AH");
    else if(pEntry->ProtocolType == PROTO_GRE)
        _ansc_strcpy(IpFilter.ProtocolType, "GRE");
    else if(pEntry->ProtocolType == PROTO_IPV6ENCAP)
        _ansc_strcpy(IpFilter.ProtocolType, "IPv6Encap");
    else if(pEntry->ProtocolType == PROTO_IPV4ENCAP)
        _ansc_strcpy(IpFilter.ProtocolType, "IPv4Encap");
    else if(pEntry->ProtocolType == PROTO_IPV6FRAGMENT)
        _ansc_strcpy(IpFilter.ProtocolType, "IPv6Fragment");
    else if(pEntry->ProtocolType == PROTO_L2TP)
        _ansc_strcpy(IpFilter.ProtocolType, "L2TP");
    else
        _ansc_strcpy(IpFilter.ProtocolType, "");

    if(pEntry->FilterAction == ACTION_ALLOW)
        _ansc_strcpy(IpFilter.FilterAction, "ALLOW");
    else if(pEntry->FilterAction == ACTION_DENY)
        _ansc_strcpy(IpFilter.FilterAction, "DENY");
    else
        _ansc_strcpy(IpFilter.FilterAction, "");

    if(pEntry->FilterDirec == DIRECTION_INCOMING)
        _ansc_strcpy(IpFilter.FilterDirec, "INCOMING");
    else if(pEntry->FilterDirec == DIRECTION_OUTGOING)
        _ansc_strcpy(IpFilter.FilterDirec, "OUTGOING");
    else
        _ansc_strcpy(IpFilter.FilterDirec, "");

    rc = Utopia_SetV6IpFilterByIndex(&ctx, index, &IpFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");

        return ANSC_STATUS_SUCCESS;
    }
}

static int g_NrFwV6DayOfWeek = 0;
/**********************************************************************

    caller:     COSA DML

    prototype:

        ULONG
        CosaDmlFW_V6DayOfWeek_GetNumberOfEntries
            (
                void
            );

    Description:

        Backend implementation to get number of entries for DayOfWeek of IPv6.

    Arguments:    void

    Return:       The status of the operation.

**********************************************************************/
ULONG
CosaDmlFW_V6DayOfWeek_GetNumberOfEntries
    (
        void
    )
{
    UtopiaContext ctx;

    if(!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfV6DayOfWeek(&ctx, &g_NrFwV6DayOfWeek);
    Utopia_Free(&ctx, 0);

    return g_NrFwV6DayOfWeek;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6DayOfWeek_GetEntryByIndex
            (
                ULONG                    index,
                COSA_DML_FW_V6_DAYOFWEEK *pEntry
            );

    Description:

        Backend implementation to get entry by index for DayOfWeek of IPv6.

    Arguments:    ULONG                    index,
                  COSA_DML_FW_V6_DAYOFWEEK *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6DayOfWeek_GetEntryByIndex
    (
        ULONG                         index,
        COSA_DML_FW_V6_DAYOFWEEK      *pEntry
    )
{
    UtopiaContext ctx;
    fwv6dayofweek_t V6DayOfWeek;

    if (index >= g_NrFwV6DayOfWeek || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetV6DayOfWeekByIndex(&ctx, index, &V6DayOfWeek);

    pEntry->InstanceNumber = V6DayOfWeek.InstanceNumber;

    _ansc_strncpy(pEntry->V6DayOfWeek_BlockTimeBitMask, V6DayOfWeek.V6DayOfWeek_BlockTimeBitMask, sizeof(pEntry->V6DayOfWeek_BlockTimeBitMask)-1);
    _ansc_strncpy(pEntry->Alias, V6DayOfWeek.Alias, sizeof(pEntry->Alias)-1);

    Utopia_Free(&ctx, 0);
    return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6DayOfWeek_SetValues
            (
                ULONG           index,
                ULONG           ins,
                const char      *alias,
                char*          bitmask
            );

    Description:

        Backend implementation to set values for DayOfWeek of IPv6.

    Arguments:  ULONG           index,
                ULONG           ins,
                const char      *alias,
                char*          bitmask

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6DayOfWeek_SetValues
    (
        ULONG                index,
        ULONG                ins,
        const char           *alias,
        char*                bitmask
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (index >= g_NrFwV6DayOfWeek || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_SetV6DayOfWeekInsAndAliasByIndex(&ctx, index, ins, alias, bitmask);
    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
        return ANSC_STATUS_SUCCESS;
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6DayOfWeek_GetConf
            (
                ULONG                       ins,
                COSA_DML_FW_V6_DAYOFWEEK    *pEntry
            );

    Description:

        Backend implementation to get configuration for DayOfWeek of IPv6.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_V6_DAYOFWEEK    *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6DayOfWeek_GetConf
    (
        ULONG                             ins,
        COSA_DML_FW_V6_DAYOFWEEK          *pEntry
    )
{
    return CosaDmlFW_V6DayOfWeek_GetEntryByIndex(ins, pEntry);
}

/**********************************************************************

    caller:     COSA DML

    prototype:

        ANSC_STATUS
        CosaDmlFW_V6DayOfWeek_SetConf
            (
                ULONG                       ins,
                COSA_DML_FW_V6_DAYOFWEEK    *pEntry
            );

    Description:

        Backend implementation to set configuration for DayOFWeek of IPv6.

    Arguments:  ULONG                       ins,
                COSA_DML_FW_V6_DAYOFWEEK    *pEntry

    Return:       The status of the operation.

**********************************************************************/
ANSC_STATUS
CosaDmlFW_V6DayOfWeek_SetConf
    (
        ULONG                                ins,
        COSA_DML_FW_V6_DAYOFWEEK             *pEntry
    )
{
    UtopiaContext ctx;
    fwv6dayofweek_t V6DayOfWeek;
    int rc = -1;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    V6DayOfWeek.InstanceNumber = pEntry->InstanceNumber;
    _ansc_strncpy(V6DayOfWeek.V6DayOfWeek_BlockTimeBitMask, pEntry->V6DayOfWeek_BlockTimeBitMask, sizeof(V6DayOfWeek.V6DayOfWeek_BlockTimeBitMask)-1);
    _ansc_strncpy(V6DayOfWeek.Alias, pEntry->Alias, sizeof(V6DayOfWeek.Alias)-1);

    rc = Utopia_SetV6DayOfWeekByIndex(&ctx, ins, &V6DayOfWeek);
    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }
}
// LGI ADD END
#endif
