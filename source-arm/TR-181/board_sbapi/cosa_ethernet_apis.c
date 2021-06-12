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

    module: cosa_ethernet_apis.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CosaDmlEthInit
        *  CosaDmlEthPortGetNumberOfEntries
        *  CosaDmlEthPortGetEntry
        *  CosaDmlEthPortSetCfg
        *  CosaDmlEthPortGetCfg
        *  CosaDmlEthPortGetDinfo
        *  CosaDmlEthPortGetStats
        *  CosaDmlEthLinkGetNumberOfEntries
        *  CosaDmlEthLinkGetEntry
        *  CosaDmlEthLinkAddEntry
        *  CosaDmlEthLinkDelEntry
        *  CosaDmlEthLinkSetCfg
        *  CosaDmlEthLinkGetCfg
        *  CosaDmlEthLinkGetDinfo
        *  CosaDmlEthLinkGetStats
        *  CosaDmlEthVlanTerminationGetNumberOfEntries
        *  CosaDmlEthVlanTerminationGetEntry
        *  CosaDmlEthVlanTerminationAddEntry
        *  CosaDmlEthVlanTerminationDelEntry
        *  CosaDmlEthVlanTerminationSetCfg
        *  CosaDmlEthVlanTerminationGetCfg
        *  CosaDmlEthVlanTerminationGetDinfo
        *  CosaDmlEthVlanTerminationGetStats
    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/

#include "cosa_ethernet_apis.h"
#include "cosa_ethernet_apis_multilan.h"
#include "secure_wrapper.h"
#include "safec_lib_common.h"

#ifdef _HUB4_PRODUCT_REQ_
#include "sysevent/sysevent.h"
#endif

#ifdef _COSA_SIM_
/*Removed code for simulator, because this is usg platform*/
#elif  (_COSA_INTEL_USG_ARM_ || _COSA_BCM_MIPS_)

#include "cosa_ethernet_apis_ext.h"
#include "cosa_drg_common.h"
#include "ansc_string_util.h"

#include "ccsp_psm_helper.h"
#include "dmsb_tr181_psm_definitions.h"

#include "utctx/utctx_api.h"
#include "linux/sockios.h"
#include <sys/ioctl.h>
#ifdef ARRIS_XB3_PLATFORM_CHANGES
  #include "rdk_cm_api_arris.h"
#else
  #include "linux/if.h"
#endif

extern void *g_pDslhDmlAgent;

static int saveID(char* ifName, char* pAlias, ULONG ulInstanceNumber);
static int loadID(char* ifName, char* pAlias, ULONG* ulInstanceNumber);
//static int saveLinkID(char* ifName, char* pAlias, ULONG ulInstanceNumber);
//static int loadLinkID(char* ifName, char* pAlias, ULONG* ulInstanceNumber);
COSA_DML_IF_STATUS getIfStatus(const PUCHAR name, struct ifreq *pIfr);
static int setIfStatus(struct ifreq *pIfr);
int _getMac(char* ifName, char* mac);
void rdkb_api_platform_hal_GetLanMacAddr(char* mac);

/**************************************************************************
                        DATA STRUCTURE DEFINITIONS
**************************************************************************/

/**************************************************************************
                        GLOBAL VARIABLES
**************************************************************************/
#include "ccsp_hal_ethsw.h" 

#if defined _COSA_INTEL_USG_ARM_ || _COSA_BCM_MIPS_

#include "syscfg/syscfg.h"

int puma6_getSwitchCfg(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_CFG pcfg);
int puma6_setSwitchCfg(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_CFG pcfg); 
int puma6_getSwitchDInfo(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_DINFO pDinfo);
int puma6_getSwitchStats(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_STATS pStats);
int puma6_getEntry(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_FULL pEntry);


COSA_DML_ETH_PORT_SINFO      g_EthIntSInfo[] = 
    {
        /* Downstream (LAN) ports */
        {SWITCH_PORT_0_NAME,                FALSE,  {0,0,0,0,0,0}},
        {SWITCH_PORT_1_NAME,                FALSE,  {0,0,0,0,0,0}},
#if defined(ETH_3_PORTS) || defined(ETH_4_PORTS) || defined(ETH_6_PORTS) || defined(ETH_8_PORTS)
        {SWITCH_PORT_2_NAME,                FALSE,  {0,0,0,0,0,0}},
#endif
#if defined(ETH_4_PORTS) || defined(ETH_6_PORTS) || defined(ETH_8_PORTS)
        {SWITCH_PORT_3_NAME,                FALSE,  {0,0,0,0,0,0}},
#endif
#if defined(ETH_6_PORTS) || defined(ETH_8_PORTS)
        {SWITCH_PORT_4_NAME,                FALSE,  {0,0,0,0,0,0}},
        {SWITCH_PORT_5_NAME,                FALSE,  {0,0,0,0,0,0}},
#endif
#if defined(ETH_8_PORTS)
        {SWITCH_PORT_6_NAME,                FALSE,  {0,0,0,0,0,0}},
        {SWITCH_PORT_7_NAME,                FALSE,  {0,0,0,0,0,0}},
#endif
#if defined(_HUB4_PRODUCT_REQ_)
        /* Upstream (WAN) ports */
        {DMSB_ETH_IF_NAME_DFT_WanRouting,   TRUE,   {0,0,0,0,0,0}}
#else        
        /* Upstream (WAN) ports */
        {DMSB_ETH_IF_NAME_DFT_WanRouting,   TRUE,   {0,0,0,0,0,0}},
#if defined(INTEL_PUMA7) && !defined(_ARRIS_XB6_PRODUCT_REQ_)
        {DMSB_ETH_IF_NAME_DFT_WanBridging,  TRUE,   {0,0,0,0,0,0}},
        {SWITCH_PORT_4_NAME,                FALSE,  {0,0,0,0,0,0}},
        {SWITCH_PORT_5_NAME,                FALSE,  {0,0,0,0,0,0}}
#else
        {DMSB_ETH_IF_NAME_DFT_WanBridging,  TRUE,   {0,0,0,0,0,0}}
#endif /* defined(INTEL_PUMA7) && !defined(_ARRIS_XB6_PRODUCT_REQ_) */
#endif /* _HUB4_PRODUCT_REQ_ */ 
    };

static const ULONG g_EthernetIntNum = sizeof(g_EthIntSInfo)/sizeof(g_EthIntSInfo[0]);

#if defined _COSA_INTEL_USG_ARM_ || _COSA_BCM_MIPS_
static int getIfCfg(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_CFG pcfg);
static int setIfCfg(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_CFG pcfg);
static int getIfStats(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_STATS pStats);
static int getIfDInfo(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_DINFO pDinfo);
static int getIfEntry(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_FULL pEntry);
#endif

static int getIfStats2(const PUCHAR pName, PCOSA_DML_ETH_STATS pStats);

static const EthIntControlFuncs ifFuncs = {
    getIfCfg,
    setIfCfg,
    getIfStats,
    getIfDInfo,
    getIfEntry
};

static const EthIntControlFuncs swFuncs = {
    puma6_getSwitchCfg,
    puma6_setSwitchCfg,
    puma6_getSwitchStats,
    puma6_getSwitchDInfo,
    puma6_getEntry
};

static const int g_PortIDs[]={
#if defined(ETH_6_PORTS)
    CCSP_HAL_ETHSW_EthPort1,
    CCSP_HAL_ETHSW_EthPort2,
    CCSP_HAL_ETHSW_EthPort3,
    CCSP_HAL_ETHSW_EthPort4,
    CCSP_HAL_ETHSW_EthPort5,
    CCSP_HAL_ETHSW_EthPort6
#elif defined(ETH_8_PORTS) || defined(INTEL_PUMA7)
    CCSP_HAL_ETHSW_EthPort1,
    CCSP_HAL_ETHSW_EthPort2,
    CCSP_HAL_ETHSW_EthPort3,
    CCSP_HAL_ETHSW_EthPort4,
    CCSP_HAL_ETHSW_EthPort5,
    CCSP_HAL_ETHSW_EthPort6,
    CCSP_HAL_ETHSW_EthPort7,
    CCSP_HAL_ETHSW_EthPort8
#else
    CCSP_HAL_ETHSW_EthPort1,
    CCSP_HAL_ETHSW_EthPort2,
    CCSP_HAL_ETHSW_EthPort3,
    CCSP_HAL_ETHSW_EthPort4
#endif
};

static CosaEthInterfaceInfo g_EthEntries[] = 
    {
#if defined(ETH_2_PORTS)
        {g_EthIntSInfo + 0, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 0, {0}},
        {g_EthIntSInfo + 1, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 1, {0}},
        {g_EthIntSInfo + 2, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}},
        {g_EthIntSInfo + 3, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}}
#elif defined(ETH_3_PORTS)
        {g_EthIntSInfo + 0, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 0, {0}},
        {g_EthIntSInfo + 1, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 1, {0}},
        {g_EthIntSInfo + 2, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 2, {0}},
        {g_EthIntSInfo + 3, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}},
        {g_EthIntSInfo + 4, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}}
#elif defined(ETH_6_PORTS)
        {g_EthIntSInfo + 0, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 0, {0}},
        {g_EthIntSInfo + 1, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 1, {0}},
        {g_EthIntSInfo + 2, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 2, {0}},
        {g_EthIntSInfo + 3, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 3, {0}},
        {g_EthIntSInfo + 4, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 4, {0}},
        {g_EthIntSInfo + 5, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 5, {0}},
        {g_EthIntSInfo + 6, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}},
        {g_EthIntSInfo + 7, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}}
#elif defined(ETH_8_PORTS)
        {g_EthIntSInfo + 0, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 0, {0}},
        {g_EthIntSInfo + 1, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 1, {0}},
        {g_EthIntSInfo + 2, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 2, {0}},
        {g_EthIntSInfo + 3, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 3, {0}},
        {g_EthIntSInfo + 4, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 4, {0}},
        {g_EthIntSInfo + 5, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 5, {0}},
        {g_EthIntSInfo + 6, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 6, {0}},
        {g_EthIntSInfo + 7, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 7, {0}},
        {g_EthIntSInfo + 8, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}},
        {g_EthIntSInfo + 9, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}}
#else
        {g_EthIntSInfo + 0, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 0, {0}},
        {g_EthIntSInfo + 1, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 1, {0}},
        {g_EthIntSInfo + 2, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 2, {0}},
        {g_EthIntSInfo + 3, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 3, {0}},
        {g_EthIntSInfo + 4, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}},
#if defined(INTEL_PUMA7) && !defined(_ARRIS_XB6_PRODUCT_REQ_)
        {g_EthIntSInfo + 5, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}},
        {g_EthIntSInfo + 6, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 4, {0}},
        {g_EthIntSInfo + 7, {'\0'}, 0, 0, &swFuncs, g_PortIDs + 5, {0}}
#else
        {g_EthIntSInfo + 5, {'\0'}, 0, 0, &ifFuncs, NULL,          {0}}
#endif
#endif
    };

#endif


/**********************************************************************
                            Routine Trunks
**********************************************************************/

ANSC_STATUS
CosaDmlEthInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    ANSC_STATUS                     returnStatus;

#if defined _COSA_INTEL_USG_ARM_ || _COSA_BCM_MIPS_
    CHAR strMac[128]        = {0};
    ULONG i                 = 0;
    ULONG wanIndex          = 0;

    UNREFERENCED_PARAMETER(phContext);

#if !defined(_HUB4_PRODUCT_REQ_)
    ULONG lbrIndex          = 0;
#endif

#ifdef FEATURE_RDKB_XDSL_PPP_MANAGER
    char wanPhyName[32] = {0};
    char out_value[32] = {0};
#endif

    /*
     *  Manufacturer programmed MAC address for LAN interface should be read out here. -- DH  
     *
     *  It doesn't make sense to even have a MAC address in Ethernet Interface DM object,
     *  so we are not going to fill the MAC address for Upstream interfaces.
     */
#ifdef ARRIS_XB3_PLATFORM_CHANGES
    rdkb_api_platform_hal_GetLanMacAddr(strMac);
#else
    _getMac("brlan0", strMac);
#endif

    /*  Iterate through Ethernet ports, assign LAN mac to downstream ports
        Keep track of the index of upstream ports to assign their MAC addresses
     */
    for (i=0; i < g_EthernetIntNum; ++i) {
        if (!g_EthIntSInfo[i].bUpstream) {
            if ( AnscSizeOfString(strMac) != 0 )
                AnscCopyMemory(g_EthIntSInfo[i].MacAddress, strMac, 6);
        }
        else {
            if (AnscEqualString(g_EthIntSInfo[i].Name, DMSB_ETH_IF_NAME_DFT_WanRouting, TRUE))
                wanIndex = i;
#if !defined(_HUB4_PRODUCT_REQ_)
            if (AnscEqualString(g_EthIntSInfo[i].Name, DMSB_ETH_IF_NAME_DFT_WanBridging, TRUE))
                lbrIndex = i;
#endif
        }
    }
#ifdef FEATURE_RDKB_XDSL_PPP_MANAGER
    if (syscfg_get(NULL, "wan_physical_ifname", out_value, sizeof(out_value)) == 0)
    {
       strncpy(wanPhyName, out_value, sizeof(wanPhyName));
       CcspTraceInfo(("%s %d - WanPhyName=%s \n", __FUNCTION__,__LINE__, wanPhyName));
    }
    else
    {
       strncpy(wanPhyName, "erouter0", sizeof(wanPhyName));
       CcspTraceInfo(("%s %d - WanPhyName=%s \n", __FUNCTION__,__LINE__, wanPhyName));
    }
    if ( (-1 != _getMac(wanPhyName, strMac)))
#else
    if ( (-1 != _getMac("erouter0", strMac)))
#endif	    
                AnscCopyMemory(g_EthIntSInfo[wanIndex].MacAddress, strMac, 6);

#if !defined(_HUB4_PRODUCT_REQ_)
    if ( (-1 != _getMac("lbr0", strMac)) )
                AnscCopyMemory(g_EthIntSInfo[lbrIndex].MacAddress, strMac, 6);
#endif

#endif 

    returnStatus = CosaDmlEthMlanInit(hDml, phContext);

    if ( returnStatus  != ANSC_STATUS_SUCCESS )
    {
        return  returnStatus;
    }

    for (i=0; i < g_EthernetIntNum; ++i) {
        loadID(g_EthEntries[i].sInfo->Name, g_EthEntries[i].Alias, &g_EthEntries[i].instanceNumber);
        g_EthEntries[i].LastChange = AnscGetTickInSeconds();
    }

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlEthPortGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{  
    UNREFERENCED_PARAMETER(hContext);
    return g_EthernetIntNum;
}

ANSC_STATUS
CosaDmlEthPortGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_DML_ETH_PORT_FULL     pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if (pEntry)
    {
        _ansc_memset(pEntry, 0, sizeof(COSA_DML_ETH_PORT_FULL));
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

#if defined _COSA_INTEL_USG_ARM_ || _COSA_BCM_MIPS_
    if (ulIndex < g_EthernetIntNum)
    {
        AnscCopyMemory(&pEntry->StaticInfo, &g_EthIntSInfo[ulIndex], sizeof(COSA_DML_ETH_PORT_SINFO));
        g_EthEntries[ulIndex].control->getEntry(g_EthEntries + ulIndex, pEntry);
        pEntry->Cfg.InstanceNumber = g_EthEntries[ulIndex].instanceNumber;
        AnscCopyString(pEntry->Cfg.Alias, g_EthEntries[ulIndex].Alias);
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
#endif
}

ANSC_STATUS
CosaDmlEthPortSetValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UNREFERENCED_PARAMETER(hContext);
    g_EthEntries[ulIndex].instanceNumber=ulInstanceNumber;
    errno_t rc = -1;
    rc = strcpy_s(g_EthEntries[ulIndex].Alias,sizeof(g_EthEntries[ulIndex].Alias), pAlias);
    ERR_CHK(rc);
    saveID(g_EthIntSInfo[ulIndex].Name, pAlias, ulInstanceNumber);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlEthPortSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_ETH_PORT_CFG      pCfg
    )
{
    COSA_DML_ETH_PORT_CFG origCfg;
    PCosaEthInterfaceInfo pEthIf = (PCosaEthInterfaceInfo  )NULL;
    errno_t rc = -1;
    UNREFERENCED_PARAMETER(hContext);

    /*RDKB-6838, CID-32984, null check before use*/
    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }

    pEthIf = getIF(pCfg->InstanceNumber);

    if ( !pEthIf )
    {
        return ANSC_STATUS_FAILURE;
    }


    pEthIf->control->getCfg(pEthIf, &origCfg);

    pEthIf->control->setCfg(pEthIf, pCfg);

    if ( origCfg.bEnabled != pCfg->bEnabled )
    {
        //pEthIf->control->getStats(pEthIf, &pEthIf->LastStats);

        pEthIf->LastChange = AnscGetTickInSeconds();
    }
    
    if ( !AnscEqualString(pCfg->Alias, pEthIf->Alias, TRUE) )
    {
        rc = strcpy_s(pEthIf->Alias,sizeof(pEthIf->Alias), pCfg->Alias);
        ERR_CHK(rc);
        saveID(pEthIf->sInfo->Name, pCfg->Alias, pCfg->InstanceNumber);
    }
    
    CosaDmlEEEPortSetCfg(pCfg->InstanceNumber, pCfg);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlEthPortGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_ETH_PORT_CFG      pCfg
    )
{
    PCosaEthInterfaceInfo pEthIf = (PCosaEthInterfaceInfo  )NULL;
    errno_t rc = -1;
    UNREFERENCED_PARAMETER(hContext);

    /*RDKB-6838, CID-33167, null check before use*/
    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }

    pEthIf = getIF(pCfg->InstanceNumber);

    if ( !pEthIf )
    {
        return ANSC_STATUS_FAILURE;
    }


    pEthIf->control->getCfg(pEthIf, pCfg);
    
    rc = strcpy_s(pCfg->Alias,sizeof(pCfg->Alias), pEthIf->Alias);
    ERR_CHK(rc);

    pCfg->InstanceNumber = pEthIf->instanceNumber;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlEthPortGetDinfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_ETH_PORT_DINFO    pInfo
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if (!pInfo)
    {
        return ANSC_STATUS_FAILURE;
    }

    PCosaEthInterfaceInfo pEthIf = getIF(ulInstanceNumber);

    if ( !pEthIf )
    {
        return ANSC_STATUS_FAILURE;
    }

    pEthIf->control->getDInfo(pEthIf, pInfo);

    pInfo->LastChange = pEthIf->LastChange;
    
    return ANSC_STATUS_SUCCESS;
}

static int ethGetClientsCount
    (
        ULONG PortId,
        LONG num_eth_device,
        eth_device_t *eth_device
        )
{
    int idx;
    int count_client = 0;

    if (!eth_device)
    {
        CcspTraceWarning(("ethGetClientsCount Invalid input Param\n"));
        return 0;
    }

    for (idx = 0; idx < num_eth_device; idx++)
    {
        if (PortId == (ULONG)eth_device[idx].eth_port)
        {
            count_client++;
        }
    }

    return count_client;
}

static void ethGetClientMacDetails
    (
	LONG PortId,
        LONG client_num,
        LONG num_eth_device,
        eth_device_t *eth_device,
        char *mac
    )
{
	int idx;
	int isClient = 0;

	if (!eth_device || !mac)
	{
		CcspTraceWarning(("ethGetClientMacDetails Invalid input Param\n"));
		return;
	}

	for (idx = 0; idx < num_eth_device; idx++)
	{
		if (PortId == eth_device[idx].eth_port)
		{
			isClient++;
			if (isClient == client_num) {
				sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
					eth_device[idx].eth_devMacAddress[0],
					eth_device[idx].eth_devMacAddress[1],
					eth_device[idx].eth_devMacAddress[2],
					eth_device[idx].eth_devMacAddress[3],
					eth_device[idx].eth_devMacAddress[4],
					eth_device[idx].eth_devMacAddress[5]);
				return;
			}
		}
	}
}

ANSC_STATUS
CosaDmlEthPortGetClientMac
    (
	PCOSA_DML_ETH_PORT_FULL pEthernetPortFull,
        ULONG			ulInstanceNumber
    )
{
	int ret = ANSC_STATUS_FAILURE;

	ULONG total_eth_device = 0;
	eth_device_t *output_struct = NULL;

	ret = CcspHalExtSw_getAssociatedDevice(&total_eth_device, &output_struct);
	if (ANSC_STATUS_SUCCESS != ret)
	{
		CcspTraceError(("%s CcspHalExtSw_getAssociatedDevice failed\n", __func__));
		return ret;
	}

        if ( total_eth_device )
        {    
           ULONG i = 1; 
           ULONG ulNumClients = 0; 

           //Get the no of clients associated with port
           ulNumClients = ethGetClientsCount(ulInstanceNumber, total_eth_device, output_struct);
           pEthernetPortFull->DynamicInfo.AssocDevicesCount = 0; 

           if( ulNumClients  > 0 )
           {    
               pEthernetPortFull->DynamicInfo.AssocDevicesCount = ulNumClients;

               //Get Mac for associated clients
               for ( i = 1; i <= ulNumClients; i++ )
               {    
                    ethGetClientMacDetails(
                                   ulInstanceNumber,
                                   i,   
                                   total_eth_device,
                                   output_struct,
                                   (char*)pEthernetPortFull->AssocClient[i - 1].MacAddress);
               }    
           }    

           //Release the allocated memory by HAL
           if( NULL != output_struct )
           {    
              free(output_struct);
              output_struct = NULL;
           }    
        } 

   return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlEthPortGetStats
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_ETH_STATS         pStats
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if (!pStats)
    {
        return ANSC_STATUS_FAILURE;
    }

    _ansc_memset(pStats, 0, sizeof(COSA_DML_ETH_STATS));

    PCosaEthInterfaceInfo pEthIf = getIF(ulInstanceNumber);

    if ( !pEthIf )
    {
        return ANSC_STATUS_FAILURE;
    }

    pEthIf->control->getStats(pEthIf, pStats);

    pStats->BroadcastPacketsReceived    -= pEthIf->LastStats.BroadcastPacketsReceived;
    pStats->BroadcastPacketsSent        -= pEthIf->LastStats.BroadcastPacketsSent;
    pStats->BytesReceived               -= pEthIf->LastStats.BytesReceived;
    pStats->BytesSent                   -= pEthIf->LastStats.BytesSent;
    pStats->DiscardPacketsReceived      -= pEthIf->LastStats.DiscardPacketsReceived;
    pStats->DiscardPacketsSent          -= pEthIf->LastStats.DiscardPacketsSent;
    pStats->ErrorsReceived              -= pEthIf->LastStats.ErrorsReceived;
    pStats->ErrorsSent                  -= pEthIf->LastStats.ErrorsSent;
    pStats->MulticastPacketsReceived    -= pEthIf->LastStats.MulticastPacketsReceived;
    pStats->MulticastPacketsSent        -= pEthIf->LastStats.MulticastPacketsSent;
    pStats->PacketsReceived             -= pEthIf->LastStats.PacketsReceived;
    pStats->PacketsSent                 -= pEthIf->LastStats.PacketsSent;
    pStats->UnicastPacketsReceived      -= pEthIf->LastStats.UnicastPacketsReceived;
    pStats->UnicastPacketsSent          -= pEthIf->LastStats.UnicastPacketsSent;
    pStats->UnknownProtoPacketsReceived -= pEthIf->LastStats.UnknownProtoPacketsReceived;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlEEEPortGetCfg (ULONG ulInstanceNumber, PCOSA_DML_ETH_PORT_CFG pCfg)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    BOOLEAN enable = FALSE;
    int portIdx;

    portIdx = getPortID(ulInstanceNumber);

    if ((portIdx == CCSP_HAL_ETHSW_EthPort1) ||
        (portIdx == CCSP_HAL_ETHSW_EthPort2) ||
        (portIdx == CCSP_HAL_ETHSW_EthPort3) ||
        (portIdx == CCSP_HAL_ETHSW_EthPort4))
    {
        if (CcspHalEthSwGetEEEPortEnable(portIdx, &enable) == RETURN_OK)
        {
            returnStatus = ANSC_STATUS_SUCCESS;
        }
    }

    pCfg->bEEEEnabled = enable;

    return returnStatus;
}

ANSC_STATUS CosaDmlEEEPortSetCfg (ULONG ulInstanceNumber, PCOSA_DML_ETH_PORT_CFG pCfg)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    int portIdx;

    portIdx = getPortID(ulInstanceNumber);

    if ((portIdx == CCSP_HAL_ETHSW_EthPort1) ||
        (portIdx == CCSP_HAL_ETHSW_EthPort2) ||
        (portIdx == CCSP_HAL_ETHSW_EthPort3) ||
        (portIdx == CCSP_HAL_ETHSW_EthPort4))
    {
        if (CcspHalEthSwSetEEEPortEnable(portIdx, pCfg->bEEEEnabled) == RETURN_OK)
        {
            returnStatus = ANSC_STATUS_SUCCESS;
        }
    }

    return returnStatus;
}

/**********************************************************************
       Ethernet Link  -- moved to cosa_ethernet_apis_multilan.c

**********************************************************************/

/**********************************************************************
                            Ethernet VLAN Termination
**********************************************************************/

COSA_DML_ETH_VLAN_TERMINATION_FULL  g_EthernetVlanTermination[MAXINSTANCE];

ULONG                               g_EthernetVlanTerminationNum = 0;

ULONG
CosaDmlEthVlanTerminationGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    return g_EthernetVlanTerminationNum;
}

ANSC_STATUS
CosaDmlEthVlanTerminationGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_DML_ETH_VLAN_TERMINATION_FULL pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    errno_t rc = -1;
    if ( !pEntry )
        return ANSC_STATUS_FAILURE;

    if ( ulIndex < g_EthernetVlanTerminationNum )
    {
        AnscCopyMemory(pEntry, &g_EthernetVlanTermination[ulIndex], sizeof(COSA_DML_ETH_VLAN_TERMINATION_FULL));

        char ifName[256];
        rc = sprintf_s(ifName, sizeof(ifName), "%s.%lu", pEntry->Cfg.EthLinkName, pEntry->Cfg.VLANID);
        if(rc < EOK)
        {
            ERR_CHK(rc);
            return ANSC_STATUS_FAILURE;
        }
        pEntry->DynamicInfo.Status = getIfStatus((PUCHAR)ifName, NULL);
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlEthVlanTerminationSetValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulIndex);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    UNREFERENCED_PARAMETER(pAlias);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlEthVlanTerminationAddEntry
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_ETH_VLAN_TERMINATION_FULL pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    errno_t rc = -1;
    if ( !pEntry )
    {
        return ANSC_STATUS_FAILURE;
    }
    
    if ( g_EthernetVlanTerminationNum < MAXINSTANCE )
    {
        AnscCopyMemory(&g_EthernetVlanTermination[g_EthernetVlanTerminationNum], pEntry, sizeof(COSA_DML_ETH_VLAN_TERMINATION_FULL));
        
        g_EthernetVlanTerminationNum++;

        if (pEntry->Cfg.EthLinkName[0] && pEntry->Cfg.VLANID)
        {
            char cmd[256];
            v_secure_system("vconfig add %s %lu", pEntry->Cfg.EthLinkName, pEntry->Cfg.VLANID);

            if (pEntry->Cfg.bEnabled)
            {
                rc = sprintf_s(cmd, sizeof(cmd), "ip link set %s.%lu up", pEntry->Cfg.EthLinkName, pEntry->Cfg.VLANID);
                if(rc < EOK)
                {
                    ERR_CHK(rc);
                }
            }
            else
            {
                pEntry->DynamicInfo.Status = COSA_DML_IF_STATUS_Down;
            }

            pEntry->DynamicInfo.LastChange = AnscGetTickInSeconds();
        }
        else
        {
            pEntry->DynamicInfo.Status = COSA_DML_IF_STATUS_NotPresent;
        }
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }
    
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlEthVlanTerminationDelEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    ULONG                           i = 0;
    ULONG                           j = 0;
    
    UNREFERENCED_PARAMETER(hContext);

    for ( i = 0; i < g_EthernetVlanTerminationNum; i++ )
    {
        if ( g_EthernetVlanTermination[i].Cfg.InstanceNumber == ulInstanceNumber )
        {
            if (g_EthernetVlanTermination[i].Cfg.EthLinkName[0] && g_EthernetVlanTermination[i].Cfg.VLANID)
            {
                v_secure_system("vconfig rem %s.%lu", g_EthernetVlanTermination[i].Cfg.EthLinkName, g_EthernetVlanTermination[i].Cfg.VLANID);
            }

            for ( j = i; j < g_EthernetVlanTerminationNum; j++ )
            {
                AnscCopyMemory(&g_EthernetVlanTermination[j], &g_EthernetVlanTermination[j+1], sizeof(COSA_DML_ETH_VLAN_TERMINATION_FULL));
            }

            g_EthernetVlanTerminationNum--;

            return ANSC_STATUS_SUCCESS;
        }
    }
    
    return ANSC_STATUS_CANT_FIND;
}

ANSC_STATUS
CosaDmlEthVlanTerminationValidateCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_ETH_VLAN_TERMINATION_CFG pCfg,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hContext);
    errno_t rc = -1;
    if ( !pCfg )
    {
        return FALSE;
    }

    PCOSA_DML_ETH_VLAN_TERMINATION_FULL pEntry  = getVlanTermination(pCfg->InstanceNumber);

    if (!pEntry || strcmp(pEntry->Cfg.LowerLayers, pCfg->LowerLayers))
    {
        if (pCfg->LowerLayers[0])
        {
            // LowerLayers or VLANID updated, need to come up a new device name
            ULONG                           ulEntryNameLen              = 256;
            CHAR                            ucEntryParamName[256]       = {0};
            CHAR                            ucEntryNameValue[256]       = {0};

            rc = sprintf_s(ucEntryParamName, sizeof(ucEntryParamName), "%sName", pCfg->LowerLayers);
            if(rc < EOK)
            {
                ERR_CHK(rc);
                return FALSE;
            }

            if ( ( 0 == CosaGetParamValueString(ucEntryParamName, ucEntryNameValue, &ulEntryNameLen)) &&
                 ( AnscSizeOfString((const char*)ucEntryNameValue) != 0                                        ) )
            {
                rc = strcpy_s(pCfg->EthLinkName,sizeof(pCfg->EthLinkName), ucEntryNameValue);
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return FALSE;
                }
            }
            else
            {
                pCfg->EthLinkName[0] = '\0';
            }
        }
        else
            pCfg->EthLinkName[0] = '\0';
    }

    if (pCfg->bEnabled)
    {
        if (!pCfg->VLANID && pEntry && pCfg->VLANID != pEntry->Cfg.VLANID)
        {
            rc = strcpy_s(pReturnParamName,*puLength, "VLANID");
            if(rc != EOK)
            {
                ERR_CHK(rc);
                return FALSE;
            }
            *puLength = AnscSizeOfString("VLANID");
            return FALSE;
        }
        if (!pCfg->EthLinkName[0])
        {
            if (pEntry && strcmp(pCfg->LowerLayers, pEntry->Cfg.LowerLayers))
            {
                rc = strcpy_s(pReturnParamName,*puLength, "LowerLayers");
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return FALSE;
                }
                *puLength = AnscSizeOfString("LowerLayers");
            }
            else
            {
                rc = strcpy_s(pReturnParamName,*puLength, "Enable");
                if(rc != EOK)
                {
                    ERR_CHK(rc);
                    return FALSE;
                }
                *puLength = AnscSizeOfString("Enable");
            }
            return FALSE;
        }
    }

    return TRUE;
}

ANSC_STATUS
CosaDmlEthVlanTerminationSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_ETH_VLAN_TERMINATION_CFG pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }

    PCOSA_DML_ETH_VLAN_TERMINATION_FULL pEntry  = getVlanTermination(pCfg->InstanceNumber);

    if (pEntry)
    {
        char ifName[256];
        errno_t rc = -1;
        if (strcmp(pEntry->Cfg.EthLinkName, pCfg->EthLinkName) || pEntry->Cfg.VLANID != pCfg->VLANID)
        {
            if (pEntry->Cfg.EthLinkName[0] && pEntry->Cfg.VLANID)
            {
                v_secure_system("vconfig rem %s.%lu", pEntry->Cfg.EthLinkName, pEntry->Cfg.VLANID);
            }
            if (pCfg->EthLinkName[0] && pCfg->VLANID)
            {
                v_secure_system("vconfig add %s %lu", pCfg->EthLinkName, pCfg->VLANID);
                if (pCfg->bEnabled)
                {
                    v_secure_system("ip link set %s.%lu up", pCfg->EthLinkName, pCfg->VLANID);
                }
            }
            pEntry->DynamicInfo.LastChange = AnscGetTickInSeconds();
            _ansc_memset(&pEntry->LastStats, 0, sizeof(COSA_DML_ETH_STATS));
        }
        else if (!pEntry->Cfg.bEnabled && pCfg->bEnabled)
        {
            v_secure_system("ip link set %s.%lu up", pCfg->EthLinkName, pCfg->VLANID);

            pEntry->DynamicInfo.LastChange = AnscGetTickInSeconds();

            rc = sprintf_s(ifName, sizeof(ifName), "%s.%lu", pCfg->EthLinkName, pCfg->VLANID);
            if(rc < EOK)
            {
                ERR_CHK(rc);
                return ANSC_STATUS_FAILURE;
            }
            getIfStats2((PUCHAR)ifName, &pEntry->LastStats);
        }
        else if (pEntry->Cfg.bEnabled && !pCfg->bEnabled)
        {
            v_secure_system("ip link set %s.%lu down", pCfg->EthLinkName, pCfg->VLANID);

            pEntry->DynamicInfo.LastChange = AnscGetTickInSeconds();
        }

        AnscCopyMemory(&pEntry->Cfg, pCfg, sizeof(COSA_DML_ETH_VLAN_TERMINATION_CFG));

        return ANSC_STATUS_SUCCESS;
    }

    return ANSC_STATUS_CANT_FIND;
}

ANSC_STATUS
CosaDmlEthVlanTerminationGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_ETH_VLAN_TERMINATION_CFG pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( !pCfg )
    {
        return ANSC_STATUS_FAILURE;
    }

    PCOSA_DML_ETH_VLAN_TERMINATION_FULL pEntry  = getVlanTermination(pCfg->InstanceNumber);

    if (pEntry)
    {
        AnscCopyMemory(pCfg, &pEntry->Cfg, sizeof(COSA_DML_ETH_VLAN_TERMINATION_CFG));

        return ANSC_STATUS_SUCCESS;
    }

    return ANSC_STATUS_CANT_FIND;
}

ANSC_STATUS
CosaDmlEthVlanTerminationGetDinfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_ETH_VLAN_TERMINATION_DINFO pInfo
    )
{
    UNREFERENCED_PARAMETER(hContext);
    if ( !pInfo )
    {
        return ANSC_STATUS_FAILURE;
    }

    PCOSA_DML_ETH_VLAN_TERMINATION_FULL pEntry  = getVlanTermination(ulInstanceNumber);

    if (pEntry)
    {
        if (!(pEntry->Cfg.EthLinkName[0] && pEntry->Cfg.VLANID))
        {
           pEntry->DynamicInfo.Status = COSA_DML_IF_STATUS_NotPresent;
        }
        else if (!pEntry->Cfg.bEnabled)
        {
           pEntry->DynamicInfo.Status = COSA_DML_IF_STATUS_Down;
        }
        else
        {
           char ifName[256];
           errno_t rc = -1;
           rc = sprintf_s(ifName, sizeof(ifName), "%s.%lu", pEntry->Cfg.EthLinkName, pEntry->Cfg.VLANID);
           if(rc < EOK)
           {
               ERR_CHK(rc);
               return ANSC_STATUS_FAILURE;
           }
           pEntry->DynamicInfo.Status = getIfStatus((PUCHAR)ifName, NULL);
        }

        AnscCopyMemory(pInfo, &pEntry->DynamicInfo , sizeof(COSA_DML_ETH_VLAN_TERMINATION_DINFO));

        return ANSC_STATUS_SUCCESS;
    }

    return ANSC_STATUS_CANT_FIND;
}

ANSC_STATUS
CosaDmlEthVlanTerminationGetStats
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_ETH_STATS         pStats
    )
{
    UNREFERENCED_PARAMETER(hContext);
    errno_t rc = -1;
    if (!pStats)
        return ANSC_STATUS_FAILURE;

    _ansc_memset(pStats, 0, sizeof(COSA_DML_ETH_STATS));

    PCOSA_DML_ETH_VLAN_TERMINATION_FULL pEntry  = getVlanTermination(ulInstanceNumber);

    if (!pEntry)
    {
        return ANSC_STATUS_FAILURE;
    }

    if (pEntry->Cfg.EthLinkName[0] && pEntry->Cfg.VLANID && pEntry->Cfg.bEnabled)
    {
       char ifName[256];
       rc = sprintf_s(ifName, sizeof(ifName), "%s.%lu", pEntry->Cfg.EthLinkName, pEntry->Cfg.VLANID);
       if(rc < EOK)
       {
           ERR_CHK(rc);
           return ANSC_STATUS_FAILURE;
       }
       pEntry->DynamicInfo.Status = getIfStatus((PUCHAR)ifName, NULL);

       if (pEntry->DynamicInfo.Status == COSA_DML_IF_STATUS_Up)
       {
           getIfStats2((PUCHAR)ifName, pStats);
    
           pStats->BroadcastPacketsReceived    -= pEntry->LastStats.BroadcastPacketsReceived;
           pStats->BroadcastPacketsSent        -= pEntry->LastStats.BroadcastPacketsSent;
           pStats->BytesReceived               -= pEntry->LastStats.BytesReceived;
           pStats->BytesSent                   -= pEntry->LastStats.BytesSent;
           pStats->DiscardPacketsReceived      -= pEntry->LastStats.DiscardPacketsReceived;
           pStats->DiscardPacketsSent          -= pEntry->LastStats.DiscardPacketsSent;
           pStats->ErrorsReceived              -= pEntry->LastStats.ErrorsReceived;
           pStats->ErrorsSent                  -= pEntry->LastStats.ErrorsSent;
           pStats->MulticastPacketsReceived    -= pEntry->LastStats.MulticastPacketsReceived;
           pStats->MulticastPacketsSent        -= pEntry->LastStats.MulticastPacketsSent;
           pStats->PacketsReceived             -= pEntry->LastStats.PacketsReceived;
           pStats->PacketsSent                 -= pEntry->LastStats.PacketsSent;
           pStats->UnicastPacketsReceived      -= pEntry->LastStats.UnicastPacketsReceived;
           pStats->UnicastPacketsSent          -= pEntry->LastStats.UnicastPacketsSent;
           pStats->UnknownProtoPacketsReceived -= pEntry->LastStats.UnknownProtoPacketsReceived;
       }
    }

    return ANSC_STATUS_SUCCESS;
}

#ifdef _HUB4_PRODUCT_REQ_
ANSC_STATUS
CosaDmlEthLinkGetWanUpDownTime(char* pcWanUpDownTime, int nTimeLength)
{
    int ret 			  = 0;
    static int sysevent_fd 	  = -1;
    static token_t sysevent_token = 0;
    if (0 > sysevent_fd)
    {
        if ((sysevent_fd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "wan_up_down_time", &sysevent_token)) < 0)
        {
            fprintf(stderr, "%s: fail to open sysevent\n", __FUNCTION__);
            return -1;
        }
    }
    /*Get the time in ticks at which the Last Wan Up/Down happened*/
    if(sysevent_get(sysevent_fd, sysevent_token, "last_wan_up_down_time", pcWanUpDownTime, nTimeLength) != 0)
    {
	ret = -1;
    }
return ret;
}
#endif

/**********************************************************************
                        HELPER ROUTINES
**********************************************************************/


#if defined _COSA_INTEL_USG_ARM_ || _COSA_BCM_MIPS_
int puma6_getSwitchCfg(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_CFG pcfg)
{
    CCSP_HAL_ETHSW_PORT         port        = *((PCCSP_HAL_ETHSW_PORT)eth->hwid);
    INT                         status;
    CCSP_HAL_ETHSW_ADMIN_STATUS AdminStatus;
    CCSP_HAL_ETHSW_LINK_RATE    LinkRate;
    CCSP_HAL_ETHSW_DUPLEX_MODE  DuplexMode;

    /* By default, port is enabled */
    pcfg->bEnabled = TRUE;

    status = CcspHalEthSwGetPortAdminStatus(port, &AdminStatus);

    if ( status == RETURN_OK )
    {
        switch ( AdminStatus )
        {
            case CCSP_HAL_ETHSW_AdminUp:
            {
                pcfg->bEnabled = TRUE;
                break;
            }
            case CCSP_HAL_ETHSW_AdminDown:
            {
                pcfg->bEnabled = FALSE;
                break;
            }
            default:
            {
                pcfg->bEnabled = TRUE;
                break;
            }
        }        
    }

    status = CcspHalEthSwGetPortCfg(port, &LinkRate, &DuplexMode);

    if ( status == RETURN_OK )
    {
        switch ( LinkRate )
        {
            case CCSP_HAL_ETHSW_LINK_10Mbps:
            {
                pcfg->MaxBitRate = 10;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_100Mbps:
            {
                pcfg->MaxBitRate = 100;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_1Gbps:
            {
                pcfg->MaxBitRate = 1000;
                break;
            }
#ifdef _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_2_5Gbps:
            {
                pcfg->MaxBitRate = 2500;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_5Gbps:
            {
                pcfg->MaxBitRate = 5000;
                break;
            }
#endif // _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_10Gbps:
            {
                pcfg->MaxBitRate = 10000;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Auto:
            {
                pcfg->MaxBitRate = -1;
                break;
            }
            default:
            {
                pcfg->MaxBitRate = -1;
                break;
            }
        }

        switch ( DuplexMode )
        {
            case CCSP_HAL_ETHSW_DUPLEX_Auto:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Auto; 
                break;
            }
            case CCSP_HAL_ETHSW_DUPLEX_Half:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Half; 
                break;
            }
            case CCSP_HAL_ETHSW_DUPLEX_Full:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Full; 
                break;
            }
            default:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Auto; 
                break;
            }
        }
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS; 
     
}
int puma6_setSwitchCfg(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_CFG pcfg) {
    CCSP_HAL_ETHSW_PORT         port        = *((PCCSP_HAL_ETHSW_PORT)eth->hwid);
	CCSP_HAL_ETHSW_ADMIN_STATUS AdminStatus;
    CCSP_HAL_ETHSW_LINK_RATE    LinkRate; 
    CCSP_HAL_ETHSW_DUPLEX_MODE  DuplexMode;
	if(pcfg->bEnabled == TRUE)
	{
		AdminStatus = CCSP_HAL_ETHSW_AdminUp;
	}
	else
	{	
		AdminStatus = CCSP_HAL_ETHSW_AdminDown;
	}
	CcspHalEthSwSetPortAdminStatus(port,AdminStatus);
    
    switch ( pcfg->MaxBitRate )
    {
        case 10:
        {
            LinkRate = CCSP_HAL_ETHSW_LINK_10Mbps;
            break;
        }
        case 100:
        {
            LinkRate = CCSP_HAL_ETHSW_LINK_100Mbps;
            break;
        }
        case 1000:
        {
            LinkRate = CCSP_HAL_ETHSW_LINK_1Gbps;
            break;
        }
        case 10000:
        {
            LinkRate = CCSP_HAL_ETHSW_LINK_10Gbps;
            break;
        }
        case -1:
        {
           LinkRate = CCSP_HAL_ETHSW_LINK_Auto;
           break;
        }
        default:
        {
            LinkRate = CCSP_HAL_ETHSW_LINK_Auto;
            break;
        }
    }

    switch ( pcfg->DuplexMode )
    {
        case COSA_DML_ETH_DUPLEX_Half:
        {
            DuplexMode = CCSP_HAL_ETHSW_DUPLEX_Half;
            break;
        }
        case COSA_DML_ETH_DUPLEX_Full:
        {
            DuplexMode = CCSP_HAL_ETHSW_DUPLEX_Full;
            break;
        }
        case COSA_DML_ETH_DUPLEX_Auto: // Note: driver doesn't handle/would ignore "Auto"
        {
            DuplexMode = CCSP_HAL_ETHSW_DUPLEX_Auto;
            break;
        }
        default:
        {
            DuplexMode = CCSP_HAL_ETHSW_DUPLEX_Full;
            break;
        }
    }
    CcspHalEthSwSetPortCfg(port,LinkRate,DuplexMode);
    return ANSC_STATUS_SUCCESS; 
}
int puma6_getSwitchDInfo(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_DINFO pDinfo){
    CCSP_HAL_ETHSW_PORT         port        = *((PCCSP_HAL_ETHSW_PORT)eth->hwid);
    INT                         status;
    CCSP_HAL_ETHSW_LINK_RATE    LinkRate;
    CCSP_HAL_ETHSW_DUPLEX_MODE  DuplexMode;
    CCSP_HAL_ETHSW_LINK_STATUS  LinkStatus;

    pDinfo->Status         = COSA_DML_IF_STATUS_Down;
    pDinfo->CurrentBitRate = 0;

    status = CcspHalEthSwGetPortStatus(port, &LinkRate, &DuplexMode, &LinkStatus);

    if ( status == RETURN_OK )
    {
        switch ( LinkStatus )
        {
            case CCSP_HAL_ETHSW_LINK_Up:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Up;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Down:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Down;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Disconnected:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Down;
                break;
            }
            default:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Down;
                break;
            }
        }

        switch ( LinkRate )
        {
            case CCSP_HAL_ETHSW_LINK_10Mbps:
            {
                pDinfo->CurrentBitRate = 10;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_100Mbps:
            {
                pDinfo->CurrentBitRate = 100;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_1Gbps:
            {
                pDinfo->CurrentBitRate = 1000;
                break;
            }
#ifdef _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_2_5Gbps:
            {
                pDinfo->CurrentBitRate = 2500;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_5Gbps:
            {
                pDinfo->CurrentBitRate = 5000;
                break;
            }
#endif // _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_10Gbps:
            {
                pDinfo->CurrentBitRate = 10000;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Auto:
            {
                pDinfo->CurrentBitRate = 0;
                break;
            }
            default:
            {
                pDinfo->CurrentBitRate = 0;
                break;
            }
        }
    }
    else
    {
        return ANSC_STATUS_FAILURE; 
    }

    return ANSC_STATUS_SUCCESS; 
}

int puma6_getSwitchStats(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_STATS pStats){
#if defined(ETH_STATS_ENABLED)
    CCSP_HAL_ETHSW_PORT   port  = *((PCCSP_HAL_ETHSW_PORT)eth->hwid);
    CcspHalEthSwGetEthPortStats(port, (PCCSP_HAL_ETH_STATS)pStats);
#else
    UNREFERENCED_PARAMETER(eth);
    UNREFERENCED_PARAMETER(pStats);
#endif
    return ANSC_STATUS_SUCCESS;
}

int puma6_getEntry(PCosaEthInterfaceInfo eth, PCOSA_DML_ETH_PORT_FULL pEntry)
{
    char *strValue = NULL;
    char recName[256];
    extern ANSC_HANDLE bus_handle;
    extern char g_Subsystem[32];
    char *eeeenabled = "Device.Ethernet.Interface.%d.EEEEnable";
    CCSP_HAL_ETHSW_PORT         port        = *((PCCSP_HAL_ETHSW_PORT)eth->hwid);
    CCSP_HAL_ETH_FULL_CFG       fullEntry;
    PCOSA_DML_ETH_PORT_CFG      pcfg = &(pEntry->Cfg);
    PCOSA_DML_ETH_PORT_DINFO    pDinfo = &(pEntry->DynamicInfo);
    INT                         status;

    status = CcspHalEthSwGetPortEntry(port, &fullEntry);
    if ( status == RETURN_OK )
    {
        switch ( fullEntry.linkStatus )
        {
            case CCSP_HAL_ETHSW_LINK_Up:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Up;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Down:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Down;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Disconnected:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Down;
                break;
            }
            default:
            {
                pDinfo->Status = COSA_DML_IF_STATUS_Down;
                break;
            }
        }

        switch ( fullEntry.currLinkRate )
        {
            case CCSP_HAL_ETHSW_LINK_10Mbps:
            {
                pDinfo->CurrentBitRate = 10;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_100Mbps:
            {
                pDinfo->CurrentBitRate = 100;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_1Gbps:
            {
                pDinfo->CurrentBitRate = 1000;
                break;
            }
#ifdef _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_2_5Gbps:
            {
                pDinfo->CurrentBitRate = 2500;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_5Gbps:
            {
                pDinfo->CurrentBitRate = 5000;
                break;
            }
#endif // _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_10Gbps:
            {
                pDinfo->CurrentBitRate = 10000;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Auto:
            {
                pDinfo->CurrentBitRate = 0;
                break;
            }
            default:
            {
                pDinfo->CurrentBitRate = 0;
                break;
            }
        }

        switch ( fullEntry.adminStatus )
        {
            case CCSP_HAL_ETHSW_AdminUp:
            {
                pcfg->bEnabled = TRUE;
                break;
            }
            case CCSP_HAL_ETHSW_AdminDown:
            {
                pcfg->bEnabled = FALSE;
                break;
            }
            default:
            {
                pcfg->bEnabled = TRUE;
                break;
            }
        }

	switch ( fullEntry.maxBitRate )
        {
            case CCSP_HAL_ETHSW_LINK_10Mbps:
            {
                pcfg->MaxBitRate = 10;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_100Mbps:
            {
                pcfg->MaxBitRate = 100;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_1Gbps:
            {
                pcfg->MaxBitRate = 1000;
                break;
            }
#ifdef _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_2_5Gbps:
            {
                pcfg->MaxBitRate = 2500;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_5Gbps:
            {
                pcfg->MaxBitRate = 5000;
                break;
            }
#endif // _2_5G_ETHERNET_SUPPORT_
            case CCSP_HAL_ETHSW_LINK_10Gbps:
            {
                pcfg->MaxBitRate = 10000;
                break;
            }
            case CCSP_HAL_ETHSW_LINK_Auto:
            {
                pcfg->MaxBitRate = -1;
                break;
            }
            default:
            {
                pcfg->MaxBitRate = -1;
                break;
            }
        }

        switch ( fullEntry.duplexMode )
        {
            case CCSP_HAL_ETHSW_DUPLEX_Auto:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Auto;
                break;
            }
            case CCSP_HAL_ETHSW_DUPLEX_Half:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Half;
                break;
            }
            case CCSP_HAL_ETHSW_DUPLEX_Full:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Full;
                break;
            }
            default:
            {
                pcfg->DuplexMode = COSA_DML_ETH_DUPLEX_Auto;
                break;
            }
        }

        /*Reading eeeenable value from psm*/
        sprintf(recName, eeeenabled, port);
        if (CCSP_SUCCESS == PSM_Get_Record_Value2(bus_handle,
                                                  g_Subsystem, recName, NULL, &strValue))
            if (0 == strcmp(strValue, "true"))
            {
                pcfg->bEEEEnabled = TRUE;
            }
            else
            {
                pcfg->bEEEEnabled = FALSE;
            }
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
    }
    else
    {
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}

static int getIfCfg(PCosaEthInterfaceInfo pEthIf, PCOSA_DML_ETH_PORT_CFG pCfg)
{    
    if ( getIfStatus( (PUCHAR)pEthIf->sInfo->Name, NULL ) == COSA_DML_IF_STATUS_Up )
    {
        pCfg->bEnabled = TRUE;
    }
    else
    {
        pCfg->bEnabled = FALSE;
    }

    pCfg->DuplexMode = COSA_DML_ETH_DUPLEX_Auto;
    pCfg->MaxBitRate = 1000;

    return 0;
}

static int setIfCfg(PCosaEthInterfaceInfo pEthIf, PCOSA_DML_ETH_PORT_CFG pCfg)
{
    struct ifreq ifr;
	COSA_DML_IF_STATUS enifStatus = COSA_DML_IF_STATUS_Unknown;

	enifStatus = getIfStatus((PUCHAR)pEthIf->sInfo->Name, &ifr);

    if ( ( enifStatus == COSA_DML_IF_STATUS_Unknown ) || \
		 ( enifStatus == COSA_DML_IF_STATUS_NotPresent )
		)
    {
        return ANSC_STATUS_FAILURE;
    }
    
    if ( pCfg->bEnabled && !(ifr.ifr_flags & IFF_UP) )
    {
        ifr.ifr_flags |= IFF_UP;

        if ( setIfStatus(&ifr) )
        {
            return -1;
        }  

        /*
         *  Do not trigger a respective wan-restart or multinet events for now
         *      pEthIf->sInfo->bUpstream == TRUE -> sysevent set wan-restart
         */
    }
    else if ( !(pCfg->bEnabled) && (ifr.ifr_flags & IFF_UP) )
    {
        ifr.ifr_flags &= ~IFF_UP;

        if ( setIfStatus(&ifr) )
        {
            return -1;
        }
    }

    return 0;
}

static int getIfStats(PCosaEthInterfaceInfo pEthIf, PCOSA_DML_ETH_STATS pStats)
{
    return getIfStats2((PUCHAR)pEthIf->sInfo->Name, pStats);
}

static int getIfDInfo(PCosaEthInterfaceInfo pEthIf, PCOSA_DML_ETH_PORT_DINFO pInfo)
{
    pInfo->Status = getIfStatus((PUCHAR)pEthIf->sInfo->Name, NULL);
    
    return 0;
}

static int getIfEntry(PCosaEthInterfaceInfo pEthIf, PCOSA_DML_ETH_PORT_FULL pEntry)
{
    PCOSA_DML_ETH_PORT_CFG      pCfg = &(pEntry->Cfg);
    PCOSA_DML_ETH_PORT_DINFO    pDinfo = &(pEntry->DynamicInfo);

    pDinfo->Status = getIfStatus((PUCHAR)pEthIf->sInfo->Name, NULL);
    if ( pDinfo->Status == COSA_DML_IF_STATUS_Up )
    {
        pCfg->bEnabled = TRUE;
    }
    else
    {
        pCfg->bEnabled = FALSE;
    }

    pCfg->DuplexMode = COSA_DML_ETH_DUPLEX_Auto;
    pCfg->MaxBitRate = 1000;

    return 0;
}

#endif

/*
   Note: this function duplicates CosaUtilGetIfStats(), except that in
   this case the results are returned in a COSA_DML_ETH_STATS struct and
   in CosaUtilGetIfStats() results are returned in a COSA_DML_IF_STATS
   struct. The return values are encoded differently too.
*/
static int getIfStats2(const PUCHAR pName, PCOSA_DML_ETH_STATS pStats)
{
    FILE *fp;
    char buf[512];
    char *device = (char *) pName;
    int device_len;
    int result = -1;

    /*
       The data types in COSA_DML_ETH_STATS are currently ULONG, which is
       not enough to hold 64bit byte and packet counter values. As a
       workaround, use local variables to hold those values during parsing.
       Note that fixing this isn't a simple change as the data types in
       COSA_DML_ETH_STATS reflect those used in the data model (ie we need
       to change the data model in order to return 64bit byte and packet
       counters...).
    */
    unsigned long long rx_bytes;
    unsigned long long rx_packets;
    unsigned long long tx_bytes;
    unsigned long long tx_packets;

    memset (pStats, 0, sizeof(COSA_DML_ETH_STATS));

#ifdef _HUB4_PRODUCT_REQ_
    if ((fp = fopen ("/proc/net/dev_extstats", "r")) == NULL)
#else
    if ((fp = fopen ("/proc/net/dev", "r")) == NULL)
#endif /* _HUB4_PRODUCT_REQ_ */
        return -1;
    if (fgets (buf, sizeof(buf), fp) == NULL)
        goto done;
    if (fgets (buf, sizeof(buf), fp) == NULL)
        goto done;

    device_len = strlen (device);

    while (fgets (buf, sizeof(buf), fp) != NULL) {
        char *p = buf;
        while ((*p == ' ') || (*p == '\t'))
            p++;
        if (strncmp (p, device, device_len) != 0)
            continue;
        p += device_len;
        if (*p++ != ':')
            continue;

#ifdef _HUB4_PRODUCT_REQ_

#error "Fixme: sscanf() parsing of /proc/net/dev_extstats not yet implemented..."

#else
        if (sscanf (p, "%llu%llu%lu%lu%*u%*u%*u%*u%llu%llu%lu%lu",
                       &rx_bytes, &rx_packets, &pStats->ErrorsReceived, &pStats->DiscardPacketsReceived,
                       &tx_bytes, &tx_packets, &pStats->ErrorsSent, &pStats->DiscardPacketsSent) == 8)
        {
            pStats->BytesSent       = (ULONG) tx_bytes;      /* Truncate !! */
            pStats->BytesReceived   = (ULONG) rx_bytes;      /* Truncate !! */
            pStats->PacketsSent     = (ULONG) tx_packets;    /* Truncate !! */
            pStats->PacketsReceived = (ULONG) rx_packets;    /* Truncate !! */
            result = 0;
        }
#endif
        break;
    }

done:
    fclose (fp);

    return result;
}

static int setIfStatus(struct ifreq *pIfr)
{
    int skfd;
    
    AnscTraceFlow(("%s...\n", __FUNCTION__));

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    /* CID: 73861 Argument cannot be negative*/
    if(skfd == -1)
       return -1;

    if (ioctl(skfd, SIOCSIFFLAGS, pIfr) < 0) {
        CcspTraceWarning(("cosa_ethernet_apis.c - setIfStatus: Set interface %s error...\n", pIfr->ifr_name));
        close(skfd);
        return -1;
    }
    close(skfd);

    return 0;
}

PCosaEthInterfaceInfo getIF(const ULONG instanceNumber) {
    ULONG i;
    for (i = 0; i < g_EthernetIntNum; ++i) {
        if (g_EthEntries[i].instanceNumber == instanceNumber) {
            break;
        }
    }

    if (i == g_EthernetIntNum) {
        return NULL;
    }
    return g_EthEntries + i;
}

int getPortID(const ULONG instanceNumber)
{
    int PortIdx = 0;
    PCosaEthInterfaceInfo pEthIf = getIF(instanceNumber);

    if ((pEthIf != NULL) && (pEthIf->hwid != NULL))
    {
        PortIdx = *((PCCSP_HAL_ETHSW_PORT)pEthIf->hwid);
    }
    return PortIdx;
}

PCOSA_DML_ETH_VLAN_TERMINATION_FULL getVlanTermination(const ULONG ulInstanceNumber) {
    ULONG i;
    for ( i = 0; i < g_EthernetVlanTerminationNum; i++ )
    {
        if ( ulInstanceNumber == g_EthernetVlanTermination[i].Cfg.InstanceNumber )
        {
            break;
        }
    }

    if (i == g_EthernetVlanTerminationNum ) {
        return NULL;
    }

    return g_EthernetVlanTermination + i;
}
static int saveID(char* ifName, char* pAlias, ULONG ulInstanceNumber) {
    UtopiaContext utctx;
    char idStr[COSA_DML_IF_NAME_LENGTH+10] = {0};
    errno_t rc = -1;
    /* CID: 58910 Unchecked return value*/
    if(!Utopia_Init(&utctx))
       return -1;

    rc = sprintf_s(idStr, sizeof(idStr), "%s,%lu", pAlias,ulInstanceNumber);
    if(rc < EOK)
    {
        ERR_CHK(rc);
    }
    Utopia_RawSet(&utctx,COSA_ETH_INT_ID_SYSCFG_NAMESPACE,ifName,idStr);

    Utopia_Free(&utctx,TRUE);

    return 0;
}

/*static int saveLinkID(char* ifName, char* pAlias, ULONG ulInstanceNumber) {
    UtopiaContext utctx;
    char idStr[COSA_DML_IF_NAME_LENGTH+10] = {0};
    //CID: 65042 Unchecked return value
    if(!Utopia_Init(&utctx))
        return -1;

    sprintf(idStr,"%s,%u", pAlias,ulInstanceNumber);
    Utopia_RawSet(&utctx,COSA_ETH_LINK_ID_SYSCFG_NAMESPACE,ifName,idStr);

    Utopia_Free(&utctx,TRUE);

    return 0;
}*/

static int loadID(char* ifName, char* pAlias, ULONG* ulInstanceNumber) {
    UtopiaContext utctx;
    char idStr[COSA_DML_IF_NAME_LENGTH+10] = {0};
    char* instNumString;
    int rv;
    errno_t rc = -1;
    /*CID: 70909 Unchecked return value*/
    if(!Utopia_Init(&utctx))
        return -1;

    rv =Utopia_RawGet(&utctx, COSA_ETH_INT_ID_SYSCFG_NAMESPACE, ifName, idStr, sizeof(idStr));
    if (rv == -1 || idStr[0] == '\0') {
        Utopia_Free(&utctx, 0);
        return -1;
    }
    instNumString=idStr + AnscSizeOfToken(idStr, ",", sizeof(idStr))+1;
    *(instNumString-1)='\0';

    rc = strcpy_s(pAlias, COSA_DML_IF_NAME_LENGTH, idStr);   // Here pAlias having the size is COSA_DML_IF_NAME_LENGTH(512 bytes) from calling funtion
    if(rc != EOK)
    {
        ERR_CHK(rc);
        return -1;
    }

    *ulInstanceNumber = AnscGetStringUlong(instNumString);
    Utopia_Free(&utctx, 0);

    return 0;
}

/*static int loadLinkID(char* ifName, char* pAlias, ULONG* ulInstanceNumber) {
    UtopiaContext utctx;
    char idStr[COSA_DML_IF_NAME_LENGTH+10] = {0};
    char* instNumString;
    int rv;
    //CID: 61200 Unchecked return value
    if(!Utopia_Init(&utctx))
       return -1;

    rv =Utopia_RawGet(&utctx, COSA_ETH_LINK_ID_SYSCFG_NAMESPACE, ifName, idStr, sizeof(idStr));
    if (rv == -1 || idStr[0] == '\0') {
        Utopia_Free(&utctx, 0);
        return -1;
    }
    instNumString=idStr + AnscSizeOfToken(idStr, ",", sizeof(idStr))+1;
    *(instNumString-1)='\0';

    AnscCopyString(pAlias, idStr);
    *ulInstanceNumber = AnscGetStringUlong(instNumString);
    Utopia_Free(&utctx, 0);

    return 0;
}*/

#endif

