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

    module: cosa_ra_apis.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

    -------------------------------------------------------------------

    environment:

        platform independent

    -------------------------------------------------------------------

    author:

        COSA XML TOOL CODE GENERATOR 1.0
        leichen2@cisco.com

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.
        23/08/2012    partial param (readonly) support for USGv2.
                      the GNU zebra is used for USGv2 RA, 
                      but we can only read the config now without modifyinig.

**************************************************************************/
#include "cosa_apis.h"
#include "cosa_ra_internal.h"
#include "cosa_ra_apis.h"
#include <ctype.h>
#include <utapi.h>
#include <utapi_util.h>
#include <utctx/utctx_api.h>

#define MAX_PREF        8       /* according to TR-181 */
#define INTERFACE_COUNT 2       /*supported for brlan0 & brlan7*/
#define ZEBRA_CONF      "/var/zebra.conf"

#define RA_CONF_START   "# Based on prefix"
#define RA_CONF_IF      "interface"

typedef enum {
    RT_PREFER_HIGH = 1,
    RT_PREFER_MEDIUM,
    RT_PREFER_LOW,
} RtPrefer_t;

typedef struct RaPrefix_s {
    char    prefix[64];         /* XX::XX/M, including length */
    int     preLen;             /* prefix length */
    int     validLifetime;      /* -1 for infinite */
    int     preferLifetime;     /* -1 for infinite */
} RaPrefix_t;

typedef struct ZebraRaConf_s {
    /* struct ZebraRaConf_s  *next; */
    char            interface[17];
    RaPrefix_t      prefixes[MAX_PREF];
    int             preCnt;
    int             interval;
    int             lifetime;
	int             managedFlag;
	int             otherFlag;
    int             mtu;
    RtPrefer_t      preference;
} ZebraRaConf_t;

#ifndef DEBUG
#define DumpZebraRaConf(x)  
#else
static void DumpZebraRaConf(const ZebraRaConf_t *conf)
{
    int i , j = 0;

    for ( j = 0; j < INTERFACE_COUNT; j++)
    {
        fprintf(stderr, "------- Zebra RA Config -------\n");
        fprintf(stderr, "%s %s\n", "Interface", conf[j].interface);
        for (i = 0; i < conf[j].preCnt; i++) {
            fprintf(stderr, "  prefix[%d]\n    %s length %d valid %d prefer %d\n",
                i + 1, conf[j].prefixes[i].prefix,
                conf[j].prefixes[i].preLen,
                conf[j].prefixes[i].validLifetime,
                conf[j].prefixes[i].preferLifetime);
            fprintf(stderr, "  interval %d\n", conf[j].interval);
            fprintf(stderr, "  lifetime %d\n", conf[j].lifetime);
            fprintf(stderr, "  preference %d\n", conf[j].preference);
        }
    }
    return;
}
#endif

static void trim_leading_space(char *line)
{
    char *cp;
    int len;

    for (cp = line; *cp != '\0' && (*cp == ' ' || *cp == '\t'); cp++)
        ;

    if (cp != line) {
        len = strlen(line) - (cp - line);
        memmove(line, cp, len);
        line[len] = '\0';
    }
    return;
}

/*
 * # cat /etc/zebra.conf 
 * hostname zebra
 * password zebra
 * !enable password admin
 * !log stdout
 * log syslog
 * log file /var/log/zebra.log
 * # Based on prefix=2040:cafe:0:2::/64, old_previous=, LAN IPv6 address=
 * interface brlan0
 *    no ipv6 nd suppress-ra
 *    ipv6 nd prefix 2040:cafe:0:2::/64 300 300
 *    ipv6 nd ra-interval 60
 *    ipv6 nd ra-lifetime 180
 *    ipv6 nd router-preference medium
 *    ipv6 nd rdnss 2018:cafe::20c:29ff:fe97:fccc 300
 * interface brlan0
 *    ip irdp multicast
 * # 
 */
static int ParseZebraRaConf(ZebraRaConf_t *conf)
{
    FILE *fp;
    char line[256];
    RaPrefix_t *prefix;
    char sVal[2][64];
    char curr_interface[8] = {};
    int i = 0, ret;
    bool valid_interface = false;

    memset(conf, 0, (INTERFACE_COUNT * sizeof(ZebraRaConf_t)));

    if (access(ZEBRA_CONF, F_OK) != 0) {
        return -1;
    }

    if ((fp = fopen(ZEBRA_CONF, "rb")) == NULL) {
        fprintf(stderr, "%s: fail to open file %s\n", __FUNCTION__, ZEBRA_CONF);
        return -1;
    }

    /* skip the configs before RA part */
    while (fgets(line, sizeof(line), fp) != NULL
            && strncmp(line, RA_CONF_START, strlen(RA_CONF_START)) != 0)
        continue;

    while (fgets(line, sizeof(line), fp) != NULL) {
        trim_leading_space(line);

        /* Skip the commented entries */
        if (line[0] == '#')
            continue;

        //Updating Interface
        if (strstr(line, "interface") != NULL)
        {
            ret = sscanf(line, "interface %s", curr_interface);

            if (ret != 1)
            {
                goto BAD_FORMAT;
            }

            //To avoid Interface duplicates
            if( strncmp(curr_interface, "brlan0", sizeof("brlan0")) == 0 )
            {
                i = 0;
                snprintf(conf[i].interface, sizeof(curr_interface), "%s", curr_interface);
                valid_interface = true;
            }
            else if( strncmp(curr_interface, "brlan7", sizeof("brlan7")) == 0 )
            {
                i = 1;
                snprintf(conf[i].interface, sizeof(curr_interface), "%s", curr_interface);
                valid_interface = true;
            }
            else
            {
                valid_interface = false;
            }
            continue;
        }

        if(!valid_interface)
            continue;
        if (strstr(line, " prefix") != NULL) {
            if (conf[i].preCnt == MAX_PREF)
                continue;

            prefix = &conf[i].prefixes[conf[i].preCnt];

            if (sscanf(line, "ipv6 nd prefix %s %s %s", 
                        prefix->prefix, sVal[0], sVal[1]) != 3)
                goto BAD_FORMAT;

            if (strcmp(sVal[0], "infinite") == 0)
                prefix->validLifetime = -1;
            else
                prefix->validLifetime = atoi(sVal[0]);

            if (strcmp(sVal[1], "infinite") == 0)
                prefix->preferLifetime = -1;
            else
                prefix->preferLifetime = atoi(sVal[1]);

            if (sscanf(prefix->prefix, "%[:0-9a-fA-F]/%d", 
                        sVal[0], &prefix->preLen) != 2)
                goto BAD_FORMAT;

            conf[i].preCnt++;
        } else if (strstr(line, "ra-interval") != NULL) {
            if (sscanf(line, "ipv6 nd ra-interval %d", &conf[i].interval) != 1)
                goto BAD_FORMAT;
        } else if (strstr(line, "ra-lifetime") != NULL) {
            if (sscanf(line, "ipv6 nd ra-lifetime %d", &conf[i].lifetime) != 1)
                goto BAD_FORMAT;
		} else if (strstr(line, "managed-config-flag") != NULL) {
			conf[i].managedFlag = 1;
		} else if (strstr(line, "other-config-flag") != NULL) {
			conf[i].otherFlag = 1;
        } else if (strstr(line, "mtu") != NULL) {
            if (sscanf(line, "ipv6 nd mtu %d", &conf[i].mtu) != 1)
                goto BAD_FORMAT;
        } else if (strstr(line, "router-preference") != NULL) {
            if (sscanf(line, "ipv6 nd router-preference %s", sVal[0]) != 1)
                goto BAD_FORMAT;

            if (strcmp(sVal[0], "high") == 0)
                conf[i].preference = RT_PREFER_HIGH;
            else if (strcmp(sVal[0], "medium") == 0)
                conf[i].preference = RT_PREFER_MEDIUM;
            else if (strcmp(sVal[0], "low") == 0)
                conf[i].preference = RT_PREFER_LOW;
            else
                goto BAD_FORMAT;
        }
    }

    DumpZebraRaConf(conf);
    fclose(fp);
    return 0;

BAD_FORMAT:
    fprintf(stderr, "%s: bad format\n", __FUNCTION__);
    fclose(fp);
    return -1;
}

static int LoadRaInterface(PCOSA_DML_RA_IF_FULL raif, ULONG ulIndex)
{
    ZebraRaConf_t raConf[INTERFACE_COUNT];
    int left, i;
    RaPrefix_t *prefix;
    char *fmt;
   
    if (ParseZebraRaConf(raConf) != 0) 
        return -1;

    memset(raif, 0, sizeof(COSA_DML_RA_IF_FULL));

    /* 
     * XXX: hard-coded the configs not support for now 
     */
    raif->Cfg.InstanceNumber        = ulIndex + 1;
    raif->Cfg.bEnabled              = TRUE;
    raif->Cfg.ManualPrefixes[0]     = '\0';
    raif->Cfg.MaxRtrAdvInterval     = raConf[ulIndex].interval * 2;
    raif->Cfg.MinRtrAdvInterval     = raConf[ulIndex].interval;
    raif->Cfg.AdvDefaultLifetime    = raConf[ulIndex].lifetime;
    raif->Cfg.bAdvManagedFlag       = raConf[ulIndex].managedFlag;
    raif->Cfg.bAdvOtherConfigFlag   = raConf[ulIndex].otherFlag;
    raif->Cfg.bAdvMobileAgentFlag   = FALSE;
    raif->Cfg.bAdvNDProxyFlag       = FALSE;
    raif->Cfg.AdvLinkMTU            = raConf[ulIndex].mtu;
    raif->Cfg.AdvReachableTime      = 1;
    raif->Cfg.AdvRetransTimer       = 0;
    raif->Cfg.AdvCurHopLimit        = 1;
    snprintf(raif->Cfg.Alias, sizeof(raif->Cfg.Alias), "cpe-RA-Interface-%d",raif->Cfg.InstanceNumber);
    snprintf(raif->Cfg.Interface, sizeof(raif->Cfg.Interface), "%s", raConf[ulIndex].interface);

    switch (raConf[ulIndex].preference) {
    case RT_PREFER_HIGH:
        raif->Cfg.AdvPreferredRouterFlag = COSA_DML_RA_PREFER_ROUTER_High;
        break;
    case RT_PREFER_MEDIUM:
        raif->Cfg.AdvPreferredRouterFlag = COSA_DML_RA_PREFER_ROUTER_Medium;
        break;
    case RT_PREFER_LOW:
    default:
        raif->Cfg.AdvPreferredRouterFlag = COSA_DML_RA_PREFER_ROUTER_Low;
        break;
    }

    raif->Info.Status = COSA_DML_RA_IF_STATUS_Enabled;

    raif->Info.Prefixes[0] = '\0';
    left = sizeof(raif->Info.Prefixes);

    for (i = 0; i < raConf[ulIndex].preCnt && left > 0; i++) {
        if (i == 0)
            fmt = "%s";
        else
            fmt = ",%s";

        prefix = &raConf[ulIndex].prefixes[i];
        left -= snprintf(raif->Info.Prefixes + strlen(raif->Info.Prefixes),
                left, fmt, raConf[ulIndex].prefixes[i].prefix);
    }

    return 0;
}

ANSC_STATUS
CosaDmlRAInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    )
{
    UNREFERENCED_PARAMETER(hDml);
    UNREFERENCED_PARAMETER(phContext);
    return ANSC_STATUS_SUCCESS;
}

ULONG 
CosaDmlRAGetEnabled
    (
        BOOLEAN * pEnabled
    )
{
    COSA_DML_RA_IF_FULL raif;

    if (!pEnabled)
        return ANSC_STATUS_FAILURE;

    if (LoadRaInterface(&raif, 0) != 0)
        return ANSC_STATUS_FAILURE;

    *pEnabled = raif.Cfg.bEnabled;
    return ANSC_STATUS_SUCCESS;
}


ULONG 
CosaDmlRASetEnabled
    (
        BOOLEAN  bEnabled
    )
{
    UNREFERENCED_PARAMETER(bEnabled);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ULONG
CosaDmlRaIfGetNumberOfEntries
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);

    return INTERFACE_COUNT;
}

ANSC_STATUS
CosaDmlRaIfGetEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulIndex,
        PCOSA_DML_RA_IF_FULL        pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);

    if (!pEntry)
        return ANSC_STATUS_FAILURE;

    if (LoadRaInterface(pEntry, ulIndex) != 0)
        return ANSC_STATUS_FAILURE;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlRaIfSetValues
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
    /* let it return success , even we don't support it */ 
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlRaIfAddEntry
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_RA_IF_FULL        pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(pEntry);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfDelEntry
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_RA_IF_CFG         pCfg
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UtopiaContext utctx = {0};
    char syscfgName[36];
    char out[16];
	unsigned int  managedFlag = 0;
	unsigned int  otherFlag   = 0;
	unsigned int  ra_interval = 0;
	unsigned int  ra_lifetime = 0;
    unsigned int  mtu   = 0;
    bool breset = false;
    unsigned int rc;
    enum {
        DHCPV6_SERVER_TYPE_STATEFUL  =1,
        DHCPV6_SERVER_TYPE_STATELESS
    };
    ULONG dhcpv6ServerType = DHCPV6_SERVER_TYPE_STATEFUL;
	
    fprintf(stderr, "%s: Only support O/M flags. NOT SUPPORTED other flags FOR NOW!!\n", __FUNCTION__);
	
	if (Utopia_Init(&utctx))
	{
		out[0] = 0;
		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"router_managed_flag");
		else
        	    sprintf(syscfgName, "router_managed_flag_%d",pCfg->InstanceNumber);
		Utopia_RawGet(&utctx,NULL,syscfgName,out,sizeof(out));
		if ( out[0] == '1' )
			managedFlag = 1;

		out[0] = 0;
		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"router_other_flag");
		else
        	sprintf(syscfgName, "router_other_flag_%d",pCfg->InstanceNumber);
		Utopia_RawGet(&utctx,NULL,syscfgName,out,sizeof(out));
		if ( out[0] == '1' )
			otherFlag = 1;

		out[0] = 0;
		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"ra_interval");
		else
        	    sprintf(syscfgName, "ra_interval_%d",pCfg->InstanceNumber);
		rc = Utopia_RawGet(&utctx,NULL,syscfgName,out,sizeof(out));
		if(rc == 1)
			ra_interval = atoi(out);
		else
			breset = true;

		out[0] = 0;
		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"ra_lifetime");
		else
        	    sprintf(syscfgName, "ra_lifetime_%d",pCfg->InstanceNumber);
		rc = Utopia_RawGet(&utctx,NULL,syscfgName,out,sizeof(out));
		if(rc == 1)
		    ra_lifetime = atoi(out);
		else
		    breset = true;

		out[0] = 0;
		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"router_mtu");
		else
        	    sprintf(syscfgName, "router_mtu_%d",pCfg->InstanceNumber);
		rc = Utopia_RawGet(&utctx,NULL,syscfgName,out,sizeof(out));
		if(rc == 1)
		    mtu = atoi(out);
		else
		    breset = true;

		if ( ( !(pCfg->bAdvManagedFlag)     == !managedFlag ) && 
		     ( !(pCfg->bAdvOtherConfigFlag) == !otherFlag   ) &&
		     (   pCfg->MinRtrAdvInterval    == ra_interval  ) &&
		     (   pCfg->AdvDefaultLifetime   == ra_lifetime  ) &&
		     (   pCfg->AdvLinkMTU           == mtu          ) &&
		     (   breset                     == false        ) )
                {
                    Utopia_Free(&utctx,0);
                    return ANSC_STATUS_FAILURE;
                }

		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"router_managed_flag");
		else
        	    sprintf(syscfgName, "router_managed_flag_%d",pCfg->InstanceNumber);
		Utopia_RawSet(&utctx, NULL, syscfgName, pCfg->bAdvManagedFlag ? "1" : "0");

		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"router_other_flag");
		else
        	sprintf(syscfgName, "router_other_flag_%d",pCfg->InstanceNumber);
		Utopia_RawSet(&utctx, NULL, syscfgName, pCfg->bAdvOtherConfigFlag ? "1" : "0");

		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"ra_interval");
		else
        	    sprintf(syscfgName, "ra_interval_%d",pCfg->InstanceNumber);
		snprintf(out, sizeof(out), "%u", pCfg->MinRtrAdvInterval);
		Utopia_RawSet(&utctx,NULL,syscfgName,out);

		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"ra_lifetime");
		else
        	    sprintf(syscfgName, "ra_lifetime_%d",pCfg->InstanceNumber);
		snprintf(out, sizeof(out), "%u", pCfg->AdvDefaultLifetime);
		Utopia_RawSet(&utctx,NULL,syscfgName,out);

		if(pCfg->InstanceNumber == 1)
		    strcpy(syscfgName,"router_mtu");
		else
        	    sprintf(syscfgName, "router_mtu_%d",pCfg->InstanceNumber);
		snprintf(out, sizeof(out), "%u", pCfg->AdvLinkMTU);
		Utopia_RawSet(&utctx,NULL,syscfgName,out);

		dhcpv6ServerType = pCfg->bAdvManagedFlag ? DHCPV6_SERVER_TYPE_STATEFUL : DHCPV6_SERVER_TYPE_STATELESS;

		if(dhcpv6ServerType == DHCPV6_SERVER_TYPE_STATEFUL)
		    Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "0");
		else if(dhcpv6ServerType == DHCPV6_SERVER_TYPE_STATELESS)
		    Utopia_RawSet(&utctx, NULL, "router_autonomous_flag", "1");

		Utopia_Free(&utctx,1);

		/*
		 * When RA M/O flags are changed, we need to restart DHCPv6 service(Dibbler) as Device.DHCPv6.Server.X_CISCO_COM_Type did.
		 * Also need to set correct server type for Dibbler config file in order to respond Solicit sent from LAN CPE when M flag = 1
		 */
		CosaDmlDhcpv6sRestartOnRaChanged(dhcpv6ServerType);
		
	    return ANSC_STATUS_SUCCESS;

	}
	
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_RA_IF_CFG         pCfg,
        ULONG                       ulInstanceNumber
    )
{
    UNREFERENCED_PARAMETER(hContext);
    COSA_DML_RA_IF_FULL             raif;

    if (!pCfg)
        return ANSC_STATUS_FAILURE;

    if (LoadRaInterface(&raif, ulInstanceNumber) != 0)
        return ANSC_STATUS_FAILURE;

    memcpy(pCfg, &raif.Cfg, sizeof(COSA_DML_RA_IF_CFG));
    return ANSC_STATUS_SUCCESS;
}


ANSC_STATUS
CosaDmlRaIfGetInfo
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulInstanceNumber,
        PCOSA_DML_RA_IF_INFO        pInfo
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    COSA_DML_RA_IF_FULL             raif;

    if (!pInfo)
        return ANSC_STATUS_FAILURE;

    if (LoadRaInterface(&raif, ulInstanceNumber) != 0)
        return ANSC_STATUS_FAILURE;

    memcpy(pInfo, &raif.Info, sizeof(COSA_DML_RA_IF_INFO));
    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlRaIfGetNumberOfOptions
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulRaIfInstanceNumber
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulRaIfInstanceNumber);
    return 0;
}

ANSC_STATUS
CosaDmlRaIfGetOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulRaIfInstanceNumber,
        ULONG                       ulIndex,
        PCOSA_DML_RA_OPTION         pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulRaIfInstanceNumber);
    UNREFERENCED_PARAMETER(ulIndex);
    UNREFERENCED_PARAMETER(pEntry);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfSetOptionValues
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulRaIfInstanceNumber,
        ULONG                       ulIndex,
        ULONG                       ulInstanceNumber,
        char*                       pAlias
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulRaIfInstanceNumber);
    UNREFERENCED_PARAMETER(ulIndex);
    UNREFERENCED_PARAMETER(ulInstanceNumber);
    UNREFERENCED_PARAMETER(pAlias);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfAddOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulRaIfInstanceNumber,
        PCOSA_DML_RA_OPTION         pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulRaIfInstanceNumber);
    UNREFERENCED_PARAMETER(pEntry);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfDelOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulRaIfInstanceNumber,
        PCOSA_DML_RA_OPTION         pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulRaIfInstanceNumber);
    UNREFERENCED_PARAMETER(pEntry);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfSetOption
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulRaIfInstanceNumber,
        PCOSA_DML_RA_OPTION         pEntry
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulRaIfInstanceNumber);
    UNREFERENCED_PARAMETER(pEntry);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfGetOption2
    (
        ANSC_HANDLE                 hContext,
        ULONG                       ulRaIfInstanceNumber,
        PCOSA_DML_RA_OPTION         pEntry         
    )
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(ulRaIfInstanceNumber);
    UNREFERENCED_PARAMETER(pEntry);
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlRaIfBERemove
    (
        void
    )
{
    fprintf(stderr, "%s: NOT SUPPORTED FOR NOW!!\n", __FUNCTION__);
    return ANSC_STATUS_FAILURE;
}
