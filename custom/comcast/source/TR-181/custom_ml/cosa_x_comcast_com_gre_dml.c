/************************************************************************************
  If not stated otherwise in this file or this component's Licenses.txt file the
  following copyright and licenses apply:

  Copyright 2018 RDK Management

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
**************************************************************************/

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

/*
 *  Where is the file header section? Taking shortcut?...
 */

#include "dml_tr181_custom_cfg.h"

#ifdef CONFIG_CISCO_HOTSPOT
#include "cosa_x_comcast_com_gre_dml.h"
#include "cosa_x_comcast_com_gre_internal.h"

ULONG
GreIf_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    COSA_DATAMODEL_GRE              *pMyObject = (COSA_DATAMODEL_GRE *)g_pCosaBEManager->hGRE;

    return CosaDml_GreIfGetNumberOfEntries();
}

ANSC_HANDLE
GreIf_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    COSA_DATAMODEL_GRE              *pMyObject   = (COSA_DATAMODEL_GRE *)g_pCosaBEManager->hGRE;

    if (nIndex >= MAX_GRE_IF)
        return NULL;

    *pInsNumber = pMyObject->GreIf[nIndex].InstanceNumber;
    return (ANSC_HANDLE)&pMyObject->GreIf[nIndex];
}

BOOL
GreIf_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;
    ULONG                           ins = pGreIf->InstanceNumber;

    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        *pBool = pGreIf->Enable;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "UseSequenceNumber", TRUE))
    {
        *pBool = pGreIf->UseSequenceNumber;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "UseChecksum", TRUE))
    {
        *pBool = pGreIf->UseChecksum;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DHCPCircuitIDSSID", TRUE))
    {
        *pBool = pGreIf->DHCPCircuitIDSSID;
        if(pGreIf->DHCPCircuitIDSSID) {
		// update  circuit ids 
			hotspot_update_circuit_ids(1,1);
	    }
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DHCPRemoteID", TRUE))
    {
        *pBool = pGreIf->DHCPRemoteID;
        return TRUE;
    }

    return FALSE;
}

BOOL
GreIf_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;
    ULONG                           ins = pGreIf->InstanceNumber;

    if (AnscEqualString(ParamName, "Status", TRUE))
    {
        if (CosaDml_GreIfGetStatus(ins, (COSA_DML_GRE_STATUS *)pUlong) != ANSC_STATUS_SUCCESS)
            return FALSE;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "LastChange", TRUE))
    {
        if (CosaDml_GreIfGetLastchange(ins, pUlong) != ANSC_STATUS_SUCCESS)
            return FALSE;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeyIdentifierGenerationPolicy", TRUE))
    {
        *pUlong = pGreIf->KeyIdentifierGenerationPolicy;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAlivePolicy", TRUE))
    {
        *pUlong = pGreIf->KeepAlivePolicy;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveInterval", TRUE))
    {
        *pUlong = pGreIf->KeepAliveInterval;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveThreshold", TRUE))
    {
        *pUlong = pGreIf->KeepAliveThreshold;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveCount", TRUE))
    {
        *pUlong = pGreIf->KeepAliveCount;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveFailInterval", TRUE))
    {
        *pUlong = pGreIf->KeepAliveFailInterval;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "ReconnectPrimary", TRUE))
    {
        *pUlong = pGreIf->ReconnectPrimary;
        return TRUE;
    }

    return FALSE;
}

ULONG
GreIf_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;
    ULONG                           ins = pGreIf->InstanceNumber;

    if (AnscEqualString(ParamName, "LocalInterfaces", TRUE))
    {
        snprintf(pValue, *pUlSize, "%s", pGreIf->LocalInterfaces);
        return 0;
    }
    if (AnscEqualString(ParamName, "RemoteEndpoints", TRUE))
    {
        snprintf(pValue, *pUlSize, "%s", pGreIf->RemoteEndpoints);
        return 0;
    }
    if (AnscEqualString(ParamName, "ConnectedRemoteEndpoint", TRUE))
    {	
		CosaDml_GreIfGetConnectedRemoteEndpoint(ins,pGreIf);
	   	snprintf(pValue, *pUlSize, "%s", pGreIf->ConnectedRemoteEndpoint);
        return 0;
    }
    if (AnscEqualString(ParamName, "KeyIdentifier", TRUE))
    {
        snprintf(pValue, *pUlSize, "%s", pGreIf->KeyIdentifier);
        return 0;
    }
    if (AnscEqualString(ParamName, "AssociatedBridges", TRUE))
    {
        snprintf(pValue, *pUlSize, "%s", pGreIf->AssociatedBridges);
        return 0;
    }
    if (AnscEqualString(ParamName, "AssociatedBridgesWiFiPort", TRUE))
    {
        snprintf(pValue, *pUlSize, "%s", pGreIf->AssociatedBridgesWiFiPort);
        return 0;
    }
    if (AnscEqualString(ParamName, "GRENetworkInterface", TRUE))
    {
        snprintf(pValue, *pUlSize, "%s", pGreIf->GRENetworkInterface);
        return 0;
    }

    return -1;
}

BOOL
GreIf_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;
    ULONG                           ins = pGreIf->InstanceNumber;

    if (AnscEqualString(ParamName, "DSCPMarkPolicy", TRUE))
    {
        *pInt = pGreIf->DSCPMarkPolicy;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "VLANID", TRUE))
    {
        *pInt = pGreIf->VLANID;
        return TRUE;
    }

    return FALSE;
}

BOOL
GreIf_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;

    if (AnscEqualString(ParamName, "Enable", TRUE))
    {
        pGreIf->Enable = bValue;
        pGreIf->ChangeFlag |= GREIF_CF_ENABLE;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "UseSequenceNumber", TRUE))
    {
        pGreIf->UseSequenceNumber = bValue;
        pGreIf->ChangeFlag |= GREIF_CF_USESEQ;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "UseChecksum", TRUE))
    {
        pGreIf->UseChecksum = bValue;
        pGreIf->ChangeFlag |= GREIF_CF_USECSUM;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DHCPCircuitIDSSID", TRUE))
    {
        pGreIf->DHCPCircuitIDSSID = bValue;
        pGreIf->ChangeFlag |= GREIF_CF_DHCPCIRID;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DHCPRemoteID", TRUE))
    {
        pGreIf->DHCPRemoteID = bValue;
        pGreIf->ChangeFlag |= GREIF_CF_DHCPRMID;
        return TRUE;
    }

    return FALSE;
}

BOOL
GreIf_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;

    if (AnscEqualString(ParamName, "KeyIdentifierGenerationPolicy", TRUE))
    {
        pGreIf->KeyIdentifierGenerationPolicy = uValue;
        pGreIf->ChangeFlag |= GREIF_CF_KEYGENPOL;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAlivePolicy", TRUE))
    {
        pGreIf->KeepAlivePolicy = uValue;
        pGreIf->ChangeFlag |= GREIF_CF_KEEPPOL;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveInterval", TRUE))
    {
        pGreIf->KeepAliveInterval = uValue;
        pGreIf->ChangeFlag |= GREIF_CF_KEEPITVL;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveThreshold", TRUE))
    {
        pGreIf->KeepAliveThreshold = uValue;
        pGreIf->ChangeFlag |= GREIF_CF_KEEPTHRE;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveCount", TRUE))
    {
        pGreIf->KeepAliveCount = uValue;
        pGreIf->ChangeFlag |= GREIF_CF_KEEPCNT;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveFailInterval", TRUE))
    {
        pGreIf->KeepAliveFailInterval = uValue;
        pGreIf->ChangeFlag |= GREIF_CF_KEEPFAILITVL;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "ReconnectPrimary", TRUE))
    {
        pGreIf->ReconnectPrimary = uValue;
        pGreIf->ChangeFlag |= GREIF_CF_RECONNPRIM;
        return TRUE;
    }

    return FALSE;
}

BOOL
GreIf_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;

    if (AnscEqualString(ParamName, "LocalInterfaces", TRUE))
    {
        snprintf(pGreIf->LocalInterfaces, sizeof(pGreIf->LocalInterfaces), "%s", strValue);
        pGreIf->ChangeFlag |= GREIF_CF_LOCALIF;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "RemoteEndpoints", TRUE))
    {
        snprintf(pGreIf->RemoteEndpoints, sizeof(pGreIf->RemoteEndpoints), "%s", strValue);
        pGreIf->ChangeFlag |= GREIF_CF_RMEP;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeyIdentifier", TRUE))
    {
        snprintf(pGreIf->KeyIdentifier, sizeof(pGreIf->KeyIdentifier), "%s", strValue);
        pGreIf->ChangeFlag |= GREIF_CF_KEYID;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "AssociatedBridges", TRUE))
    {
        snprintf(pGreIf->AssociatedBridges, sizeof(pGreIf->AssociatedBridges), "%s", strValue);
        pGreIf->ChangeFlag |= GREIF_CF_ASSOBR;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "AssociatedBridgesWiFiPort", TRUE))
    {
        snprintf(pGreIf->AssociatedBridgesWiFiPort, sizeof(pGreIf->AssociatedBridgesWiFiPort), "%s", strValue);
        pGreIf->ChangeFlag |= GREIF_CF_ASSOBRWFP;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "GRENetworkInterface", TRUE))
    {
        snprintf(pGreIf->GRENetworkInterface, sizeof(pGreIf->GRENetworkInterface), "%s", strValue);
        pGreIf->ChangeFlag |= GREIF_CF_GREIF;
        return TRUE;
    }

    return FALSE;
}

BOOL
GreIf_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;

    if (AnscEqualString(ParamName, "DSCPMarkPolicy", TRUE))
    {
        if(pGreIf->DSCPMarkPolicy == value)
            return TRUE;

        pGreIf->DSCPMarkPolicy = value;
        pGreIf->ChangeFlag |= GREIF_CF_DSCP;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "VLANID", TRUE))
    {
        pGreIf->VLANID = value;
        pGreIf->ChangeFlag |= GREIF_CF_VLANID;
        return TRUE;
    }

    return FALSE;
}

BOOL
GreIf_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    return TRUE;
}

ULONG
GreIf_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;
    ULONG                           ins = pGreIf->InstanceNumber;

    if (pGreIf->ChangeFlag == 0)
        return ANSC_STATUS_SUCCESS;

    if (pGreIf->ChangeFlag & GREIF_CF_ENABLE)
    {
        if (CosaDml_GreIfSetEnable(ins, pGreIf->Enable) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_LOCALIF)
    {
        if (CosaDml_GreIfSetLocalInterfaces(ins, pGreIf->LocalInterfaces) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_RMEP)
    {
        if (CosaDml_GreIfSetEndpoints(ins, pGreIf->RemoteEndpoints) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_KEYGENPOL)
    {
        if (CosaDml_GreIfSetKeyGenPolicy(ins, pGreIf->KeyIdentifierGenerationPolicy) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_KEYID)
    {
        if (CosaDml_GreIfSetKeyId(ins, pGreIf->KeyIdentifier) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_USESEQ)
    {
        if (CosaDml_GreIfSetUseSeqNum(ins, pGreIf->UseSequenceNumber) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_USECSUM)
    {
        if (CosaDml_GreIfSetUseChecksum(ins, pGreIf->UseChecksum) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_DSCP)
    {
        if (CosaDml_GreIfSetDSCPMarkPolicy(ins, pGreIf->DSCPMarkPolicy) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_VLANID)
    {
        if (CosaDml_GreIfSetVlanId(ins, pGreIf->VLANID) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_KEEPPOL)
    {
        if (CosaDml_GreIfSetKeepAlivePolicy(ins, pGreIf->KeepAlivePolicy) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_KEEPITVL)
    {
        if (CosaDml_GreIfSetKeepAliveInterval(ins, pGreIf->KeepAliveInterval) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_KEEPTHRE)
    {
        if (CosaDml_GreIfSetKeepAliveThreshold(ins, pGreIf->KeepAliveThreshold) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_KEEPCNT)
    {
        if (CosaDml_GreIfSetKeepAliveCount(ins, pGreIf->KeepAliveCount) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_KEEPFAILITVL)
    {
        if (CosaDml_GreIfSetKeepAliveFailInterval(ins, pGreIf->KeepAliveFailInterval) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_RECONNPRIM)
    {
        if (CosaDml_GreIfSetReconnPrimary(ins, pGreIf->ReconnectPrimary) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_DHCPCIRID)
    {
        if (CosaDml_GreIfSetDhcpCircuitSsid(ins, pGreIf->DHCPCircuitIDSSID) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_DHCPRMID)
    {
        if (CosaDml_GreIfSetDhcpRemoteId(ins, pGreIf->DHCPRemoteID) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_ASSOBR)
    {
        if (CosaDml_GreIfSetAssociatedBridges(ins, pGreIf->AssociatedBridges) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_ASSOBRWFP)
    {
        if (CosaDml_GreIfSetAssociatedBridgesWiFiPort(ins, pGreIf->AssociatedBridgesWiFiPort) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }
    if (pGreIf->ChangeFlag & GREIF_CF_GREIF)
    {
        if (CosaDml_GreIfSetGREInterface(ins, pGreIf->GRENetworkInterface) != ANSC_STATUS_SUCCESS)
            goto rollback;
    }

    pGreIf->ChangeFlag = 0;
    return ANSC_STATUS_SUCCESS;

rollback:
    pGreIf->ChangeFlag = 0;
    GreIf_Rollback((ANSC_HANDLE)pGreIf);
    return ANSC_STATUS_FAILURE;
}

ULONG
GreIf_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;
    ULONG                           ins = pGreIf->InstanceNumber;

    if (CosaDml_GreIfGetEnable(ins, &pGreIf->Enable) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetLocalInterfaces(ins, pGreIf->LocalInterfaces, sizeof(pGreIf->LocalInterfaces)) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetEndpoints(ins, pGreIf->RemoteEndpoints, sizeof(pGreIf->RemoteEndpoints)) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetKeyGenPolicy(ins, &pGreIf->KeyIdentifierGenerationPolicy) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetKeyId(ins, pGreIf->KeyIdentifier, sizeof(pGreIf->KeyIdentifier)) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetUseSeqNum(ins, &pGreIf->UseSequenceNumber) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetUseChecksum(ins, &pGreIf->UseChecksum) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetDSCPMarkPolicy(ins, &pGreIf->DSCPMarkPolicy) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetVlanId(ins, &pGreIf->VLANID) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetKeepAlivePolicy(ins, &pGreIf->KeepAlivePolicy) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetKeepAliveInterval(ins, &pGreIf->KeepAliveInterval) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetKeepAliveThreshold(ins, &pGreIf->KeepAliveThreshold) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetKeepAliveCount(ins, &pGreIf->KeepAliveCount) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetKeepAliveFailInterval(ins, &pGreIf->KeepAliveFailInterval) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetReconnPrimary(ins, &pGreIf->ReconnectPrimary) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetDhcpCircuitSsid(ins, &pGreIf->DHCPCircuitIDSSID) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetDhcpRemoteId(ins, &pGreIf->DHCPRemoteID) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetAssociatedBridges(ins, &pGreIf->AssociatedBridges, sizeof(pGreIf->AssociatedBridges)) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetAssociatedBridgesWiFiPort(ins, &pGreIf->AssociatedBridgesWiFiPort, sizeof(pGreIf->AssociatedBridgesWiFiPort)) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;
    if (CosaDml_GreIfGetGREInterface(ins, &pGreIf->GRENetworkInterface, sizeof(pGreIf->GRENetworkInterface)) != ANSC_STATUS_SUCCESS)
        return ANSC_STATUS_FAILURE;

    return ANSC_STATUS_SUCCESS;
}

BOOL
GreIfStat_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    )
{
    COSA_DML_GRE_IF                 *pGreIf      = (COSA_DML_GRE_IF *)hInsContext;
    COSA_DML_GRE_IF_STATS           *pStats      = (COSA_DML_GRE_IF_STATS *)&pGreIf->IfStats;

    CosaDml_GreIfGetStats(pGreIf->InstanceNumber, pStats);

    if (AnscEqualString(ParamName, "KeepAliveSent", TRUE))
    {
        *pUlong = pStats->KeepAliveSent;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "KeepAliveReceived", TRUE))
    {
        *pUlong = pStats->KeepAliveReceived;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DiscardChecksumReceived", TRUE))
    {
        *pUlong = pStats->DiscardChecksumReceived;
        return TRUE;
    }
    if (AnscEqualString(ParamName, "DiscardSequenceNumberReceived", TRUE))
    {
        *pUlong = pStats->DiscardSequenceNumberReceived;
        return TRUE;
    }

    return FALSE;
}

#endif
