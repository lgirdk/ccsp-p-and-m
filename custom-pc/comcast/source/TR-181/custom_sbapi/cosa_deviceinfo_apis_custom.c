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

    module: cosa_deviceinfo_api_custom.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for custom DeviceInfo Data
        Models

    -------------------------------------------------------------------

    environment:

        platform dependent

    -------------------------------------------------------------------

    author:

        Ding Hua

    -------------------------------------------------------------------

    revision:

        07/07/2014    initial revision.

**************************************************************************/

#include "cosa_deviceinfo_apis.h"
#include "cosa_deviceinfo_apis_custom.h"
#include "dml_tr181_custom_cfg.h" 
#include "secure_wrapper.h"

#include "ccsp_psm_helper.h"            // for PSM_Get_Record_Value2
#include "dmsb_tr181_psm_definitions.h" // for DMSB_TR181_PSM_DeviceInfo_Root/ProductClass
 
#include <utctx.h>
#include <utctx_api.h>
#include <utapi.h>
#include <utapi_util.h>
#define _ERROR_ "NOT SUPPORTED"

#define CAPTIVEPORTAL_EANBLE     "CaptivePortal_Enable"

extern void* g_pDslhDmlAgent;

//For PSM Access
extern ANSC_HANDLE bus_handle;
extern char g_Subsystem[32];

static int
PsmGet(const char *param, char *value, int size)
{
    char *val = NULL;

    if (PSM_Get_Record_Value2(g_MessageBusHandle, g_GetSubsystemPrefix(g_pDslhDmlAgent),
                (char *)param, NULL, &val) != CCSP_SUCCESS)
        return -1;
    
    if(val) {
        snprintf(value, size, "%s", val);
        ((CCSP_MESSAGE_BUS_INFO *)g_MessageBusHandle)->freefunc(val);
    }
    else return -1;

    return 0;
}

/*
 *  Procedure    	 : GetInterfaceName
 *  Purpose      	 : Getting Interfcae name from correspondigng configuration file
 *  Parameters   	 : 
 *    interface_name     : Getting interface_name value
 *    conf_file          : Passing current conf file to get interface name
 *  Return Values	 : None
 */

void GetInterfaceName(char interface_name[50],char conf_file[100])
{
        FILE *fp;
        char path[256] = {0},output_string[256] = {0};
        int count = 0;
        char *interface;
        if(strcmp(conf_file,"/nvram/hostapd1.conf") == 0)
                fp = popen("grep -w interface /nvram/hostapd1.conf","r");
        else if(strcmp(conf_file,"/nvram/hostapd0.conf") == 0)
                fp = popen("grep -w interface /nvram/hostapd0.conf","r");
        else if(strcmp(conf_file,"/nvram/hostapd5.conf") == 0)
                fp = popen("grep -w interface /nvram/hostapd5.conf","r");
        else if(strcmp(conf_file,"/nvram/hostapd4.conf") == 0)
                fp = popen("grep -w interface /nvram/hostapd4.conf","r");
        {
                printf("Failed to run command in Function %s\n",__FUNCTION__);
                return 0;
        }
        if(fgets(path, sizeof(path)-1, fp) != NULL)
        {
                interface = strchr(path,'=');
                strcpy(output_string,interface+1);
        }
        for(count = 0;output_string[count]!='\n';count++)
                interface_name[count] = output_string[count];
        interface_name[count]='\0';
        fclose(fp);
}

/*
 *  Procedure       	  : GetInterfaceName_virtualInterfaceName_2G
 *  Purpose     	  : Getting virtual name for xfinity wifi 2.4Ghz
 *  Parameters    	  : 
 *   interface_name       : Getting interface_name value 
 *  Return Values	  : None
 */

void GetInterfaceName_virtualInterfaceName_2G(char interface_name[50])
{
        FILE *fp;
        char path[256] = {0},output_string[256] = {0};
        int count = 0;
        char *interface;
        fp = popen("grep -w bss /nvram/hostapd0.conf","r");
        if(fp == NULL)
        {
                printf("Failed to run command in Function %s\n",__FUNCTION__);
                return 0;
        }
        if(fgets(path, sizeof(path)-1, fp) != NULL)
        {
                interface = strchr(path,'=');
                strcpy(output_string,interface+1);
        }
        for(count = 0;output_string[count]!='\n';count++)
                interface_name[count] = output_string[count];
        interface_name[count]='\0';

        fclose(fp);
}


/*
 *  Procedure     : killXfinityWiFi
 *  Purpose       : Disabling the xfinity-wifi feature in Emulator
 *  Parameters    : None
 *  Return Values : None
 */

int _syscmd(char *cmd, char *retBuf, int retBufSize)
{
    FILE *f;
    char *ptr = retBuf;
        int bufSize=retBufSize, bufbytes=0, readbytes=0;

    if((f = popen(cmd, "r")) == NULL) {
        printf("popen %s error\n", cmd);
        return -1;
    }

    while(!feof(f))
    {
        *ptr = 0;
                if(bufSize>=128) {
                        bufbytes=128;
                } else {
                        bufbytes=bufSize-1;
                }

        fgets(ptr,bufbytes,f);
                readbytes=strlen(ptr);
        if( readbytes== 0)
            break;
        bufSize-=readbytes;
        ptr += readbytes;
    }
    pclose(f);
        retBuf[retBufSize-1]=0;
    return 0;
}

void killXfinityWiFi()//LNT_EMU
{
/*        system("killall CcspHotspot");
        system("killall hotspot_arpd");
        system("brctl delif brlan1 gretap0");
        system("brctl delif brlan1 wlan0_0");
        system("ifconfig brlan1 down");
        system("brctl delbr brlan1");
        system("ip link del gretap0");
        system("iptables -D FORWARD -j general_forward");
        system("iptables -D OUTPUT -j general_output");
        system("iptables -F general_forward");
        system("iptables -F general_output");*/
	char interface_name[512];
        char virtual_interface_name[512],buf[512];
	char cmd[512] = {0};
        GetInterfaceName(interface_name,"/nvram/hostapd5.conf");
	sprintf(cmd,"%s","cat /nvram/hostapd0.conf | grep bss=");
	_syscmd(cmd,buf,sizeof(buf));
        if(buf[0] == '#')
                GetInterfaceName(virtual_interface_name,"/nvram/hostapd4.conf");
        else
                GetInterfaceName_virtualInterfaceName_2G(virtual_interface_name);
	system("killall CcspHotspot");
        system("killall hotspot_arpd");
        system("brctl delif brlan1 gretap0.100");
        v_secure_system("brctl delif brlan1 %s", virtual_interface_name);

        //system("brctl delif brlan1 wlan0_0");
        system("ifconfig brlan1 down");
        system("brctl delbr brlan1");
        system("vconfig rem gretap0.100");
        system("brctl delif brlan2 gretap0.101");
        v_secure_system("brctl delif brlan2 %s", interface_name);

        //system("brctl delif brlan2 wlan2");
        system("ifconfig brlan2 down");
        system("brctl delbr brlan2");
        system("vconfig rem gretap0.101");
        system("ip link del gretap0");
        system("iptables -D FORWARD -j general_forward");
        system("iptables -D OUTPUT -j general_output");
        system("iptables -F general_forward");
        system("iptables -F general_output");

}


#ifdef CONFIG_VENDOR_CUSTOMER_COMCAST

extern ANSC_HANDLE bus_handle;

static ANSC_STATUS
Local_CosaDmlGetParamValueByPathName
    (
        const char *pathName,
        char *pValue,
        PULONG pulSize
    )
{

    ANSC_STATUS retval = ANSC_STATUS_FAILURE;
    parameterValStruct_t varStruct;
    char outdata[80];
    int size = sizeof(outdata);

    varStruct.parameterName = pathName;
    varStruct.parameterValue = outdata;

    retval = COSAGetParamValueByPathName(bus_handle, &varStruct, &size);
    
    if ( retval != ANSC_STATUS_SUCCESS) 
    {
        return ANSC_STATUS_FAILURE;
    }
    else 
    {
        AnscCopyString(pValue, outdata);
        *pulSize = AnscSizeOfString(pValue);
        return ANSC_STATUS_SUCCESS;
    }
}

/*X_COMCAST-COM_CM_MAC*/
ANSC_STATUS
CosaDmlDiGetCMMacAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{

    return Local_CosaDmlGetParamValueByPathName("Device.X_CISCO_COM_CableModem.MACAddress", pValue, pulSize);
}

/*X_COMCAST-COM_WAN_MAC*/
ANSC_STATUS
CosaDmlDiGetRouterMacAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{
    //s_get_interface_mac("erouter0", pValue, 18);
    s_get_interface_mac("eth0", pValue, 18);
    *pulSize = AnscSizeOfString(pValue);

    return ANSC_STATUS_SUCCESS;
}

/*X_COMCAST-COM_MTA_MAC*/
ANSC_STATUS
CosaDmlDiGetMTAMacAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{
    return Local_CosaDmlGetParamValueByPathName("Device.X_CISCO_COM_MTA.MACAddress", pValue, pulSize);
}

/*X_COMCAST-COM_WAN_IP*/
ANSC_STATUS
CosaDmlDiGetRouterIPAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{
    char *interface = "eth0";

    uint32_t ip = (uint32_t) CosaUtilGetIfAddr (interface);
    unsigned char *a = (unsigned char *) &ip;

    /*
       The value returned by CosaUtilGetIfAddr() is in network byte order
       (ie it's always big endian). Processing as bytes allows this code to
       be agnostic of target endianness.
    */
    *pulSize = sprintf (pValue, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);

    return ANSC_STATUS_SUCCESS;
}

/*X_COMCAST-COM_WAN_IPv6*/
ANSC_STATUS
CosaDmlDiGetRouterIPv6Address
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{
	ipv6_addr_info_t * p_v6addr = NULL;
    int  v6addr_num = 0, i, l_iIpV6AddrLen;

	CosaUtilGetIpv6AddrInfo("erouter0", &p_v6addr, &v6addr_num);
    for(i = 0; i < v6addr_num; i++ )
    {
        if(p_v6addr[i].scope == IPV6_ADDR_SCOPE_GLOBAL)
        {
			l_iIpV6AddrLen = strlen(p_v6addr[i].v6addr);
			strncpy(pValue, p_v6addr[i].v6addr, l_iIpV6AddrLen);
			pValue[l_iIpV6AddrLen] = '\0';
        }
    }
	if(p_v6addr)
        free(p_v6addr);

    return ANSC_STATUS_SUCCESS;
}

/*X_COMCAST-COM_MTA_IP*/
ANSC_STATUS
CosaDmlDiGetMTAIPAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{
    return Local_CosaDmlGetParamValueByPathName("Device.X_CISCO_COM_MTA.IPAddress", pValue, pulSize);
}

/*X_COMCAST-COM_MTA_IPV6*/
ANSC_STATUS
CosaDmlDiGetMTAIPV6Address
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{
    return Local_CosaDmlGetParamValueByPathName("Device.X_CISCO_COM_MTA_V6.IPV6Address", pValue, pulSize);
}

/*X_COMCAST-COM_CM_IP*/
ANSC_STATUS
CosaDmlDiGetCMIPAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    )
{
    return Local_CosaDmlGetParamValueByPathName("Device.X_CISCO_COM_CableModem.IPAddress", pValue, pulSize);
}

#endif


#ifdef CONFIG_CISCO_HOTSPOT

ANSC_STATUS
CosaDmlDiGetXfinityWiFiCapable
    (
        BOOL *pValue
    )
{
    *pValue = TRUE;
    return ANSC_STATUS_SUCCESS;
}

#define HOTSPOT_PSM_EANBLE     "dmsb.hotspot.enable"

static int
PsmSet(const char *param, const char *value)
{
    if (PSM_Set_Record_Value2(g_MessageBusHandle, g_GetSubsystemPrefix(g_pDslhDmlAgent),
                (char *)param, ccsp_string, (char *)value) != CCSP_SUCCESS)
        return -1;
    return 0;
}

static int 
DmSetBool(const char *param, BOOL value)
{
    parameterValStruct_t val[1];
    char crname[256], *fault = NULL;
    int err;

    val[0].parameterName  = param;
    val[0].parameterValue = (value ? "true" : "false");
    val[0].type           = ccsp_boolean;

    snprintf(crname, sizeof(crname), "%s%s", g_GetSubsystemPrefix(g_pDslhDmlAgent), CCSP_DBUS_INTERFACE_CR);

    if ((err = CcspBaseIf_SetRemoteParameterValue(g_MessageBusHandle, 
                crname, param, g_GetSubsystemPrefix(g_pDslhDmlAgent), 0, 0xFFFF, val, 1, 1, &fault)) != CCSP_SUCCESS)

    if (fault)
        AnscFreeMemory(fault);

    return (err == CCSP_SUCCESS) ? 0 : -1;
}

ANSC_STATUS
CosaDmlDiGetXfinityWiFiEnable
    (
        BOOL *pValue
    )
{
    char val[64];

    if (PsmGet(HOTSPOT_PSM_EANBLE, val, sizeof(val)) != 0)
        return ANSC_STATUS_FAILURE;

    *pValue = (atoi(val) == 1) ? TRUE : FALSE;
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDiSetXfinityWiFiEnable
    (
        BOOL value
    )
{
#if 0
    if (DmSetBool("Device.WiFi.SSID.5.Enable", value) != ANSC_STATUS_SUCCESS) {
        AnscTraceError(("%s: set WiFi SSID Enable error\n", __FUNCTION__));
    } else {
        AnscTraceWarning(("%s: set WiFi SSID Enable OK\n", __FUNCTION__));
    }

    if (DmSetBool("Device.WiFi.Radio.1.X_CISCO_COM_ApplySetting", value) != ANSC_STATUS_SUCCESS) {
        AnscTraceError(("%s: set WiFi ApplySetting error\n", __FUNCTION__));
    } else {
        AnscTraceWarning(("%s: set WiFi ApplySetting OK\n", __FUNCTION__));
    }
#endif
	//zqiu>>
    //if (g_SetParamValueBool("Device.X_COMCAST_COM_GRE.Interface.1.Enable", value) != ANSC_STATUS_SUCCESS) {
    //    AnscTraceError(("%s: set WiFi ApplySetting error\n", __FUNCTION__));
    //} else {
    //    AnscTraceWarning(("%s: set WiFi ApplySetting OK\n", __FUNCTION__));
    //}


    //Only in Xfinitywifi Disable case SSIDs 5,6,9,10 should be disabled so that 
    //Xfinitywifi SSIDs broadcast will be stopped.
    //In enable case it is not required because user has to explicitly set SSIDs 5 and 6 or
    //SSIDs 9 and 10 that will be used for broadcast.
    if (FALSE == value)
    {
	 killXfinityWiFi();//LNT_EMU
        //SSIDs 5 and 6 case
        if (DmSetBool("Device.WiFi.SSID.5.Enable", value) != ANSC_STATUS_SUCCESS) {
            fprintf(stderr, "%s: set WiFi.SSID.5 Disable error\n", __FUNCTION__);
        } else {
            fprintf(stderr, "%s: set WiFi.SSID.5 Disable OK\n", __FUNCTION__);
        }

        if (DmSetBool("Device.WiFi.SSID.6.Enable", value) != ANSC_STATUS_SUCCESS) {
            fprintf(stderr, "%s: set WiFi.SSID.6 Disable error\n", __FUNCTION__);
        } else {
            fprintf(stderr, "%s: set WiFi.SSID.6 Disable OK\n", __FUNCTION__);
        }
    	
        //SSIDs 9 and 10 case
        if (DmSetBool("Device.WiFi.SSID.9.Enable", value) != ANSC_STATUS_SUCCESS) {
            fprintf(stderr, "%s: set WiFi.SSID.9 Disable error\n", __FUNCTION__);
        } else {
            fprintf(stderr, "%s: set WiFi.SSID.9 Disable OK\n", __FUNCTION__);
        }
	
        if (DmSetBool("Device.WiFi.SSID.10.Enable", value) != ANSC_STATUS_SUCCESS) {
            fprintf(stderr, "%s: set WiFi.SSID.10 Disable error\n", __FUNCTION__);
        } else {
            fprintf(stderr, "%s: set WiFi.SSID.10 Diable OK\n", __FUNCTION__);
        }
    }
    if (TRUE == value)//LNT_EMU
    {
	if (DmSetBool("Device.WiFi.SSID.5.Enable", value) != ANSC_STATUS_SUCCESS) {
            fprintf(stderr, "%s: set WiFi.SSID.5 Enable error\n", __FUNCTION__);
        } else {
            fprintf(stderr, "%s: set WiFi.SSID.5 Enable OK\n", __FUNCTION__);
        }
	if (DmSetBool("Device.WiFi.SSID.6.Enable", value) != ANSC_STATUS_SUCCESS) {
            fprintf(stderr, "%s: set WiFi.SSID.6 Enable error\n", __FUNCTION__);
        } else {
            fprintf(stderr, "%s: set WiFi.SSID.6 Enable OK\n", __FUNCTION__); //keerthu
        }
        system("/lib/rdk/handle_emu_gre.sh create");
    }
	
	if (g_SetParamValueBool("Device.X_COMCAST-COM_GRE.Tunnel.1.Enable", value) != ANSC_STATUS_SUCCESS) {
		fprintf(stderr, "%s: set X_COMCAST-COM_GRE.Tunnel.1.Enable error\n", __FUNCTION__);
        AnscTraceError(("%s: set X_COMCAST-COM_GRE.Tunnel.1.Enable error\n", __FUNCTION__));
		return ANSC_STATUS_FAILURE;
    } else {
        AnscTraceWarning(("%s: set X_COMCAST-COM_GRE.Tunnel.1.Enable OK\n", __FUNCTION__));
    }
	if (g_SetParamValueBool("Device.X_COMCAST-COM_GRE.Tunnel.1.Interface.1.Enable", value) != ANSC_STATUS_SUCCESS) {
        AnscTraceError(("%s: set X_COMCAST-COM_GRE.Tunnel.1.Interface.1.Enable error\n", __FUNCTION__));
    } else {
        AnscTraceWarning(("%s: set X_COMCAST-COM_GRE.Tunnel.1.Interface.1.Enable OK\n", __FUNCTION__));
    }
	if (g_SetParamValueBool("Device.X_COMCAST-COM_GRE.Tunnel.1.Interface.2.Enable", value) != ANSC_STATUS_SUCCESS) {
        AnscTraceError(("%s: set X_COMCAST-COM_GRE.Tunnel.1.Interface.2.Enable error\n", __FUNCTION__));
    } else {
        AnscTraceWarning(("%s: set X_COMCAST-COM_GRE.Tunnel.1.Interface.2.Enable OK\n", __FUNCTION__));
    }
	//zqiu<<
    if (value)
        PsmSet(HOTSPOT_PSM_EANBLE, "1");
    else
        PsmSet(HOTSPOT_PSM_EANBLE, "0");

    return ANSC_STATUS_SUCCESS;
}
#endif

ANSC_STATUS
CosaDmlGetCaptivePortalEnable
    (
        BOOL *pValue
    )
{
	char *param_value = NULL;
	PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_CaptivePortalEnable", NULL, &param_value);
	if(strcmp(param_value,"true") == 0)
		*pValue = true;
	else
		*pValue = false;

	return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlSetCaptivePortalEnable
    (
        BOOL value
    )
{
	char buf[10];
	char cmd[50];
	memset(buf,0,sizeof(buf));
	memset(cmd,0,sizeof(cmd));
	if (value)
	{
		strcpy(buf,"true");
		CcspTraceWarning(("CaptivePortal: Enabling Captive Portal switch ...\n"));		
	}
	else
	{
		CcspTraceWarning(("CaptivePortal: Disabling Captive Portal switch ...\n"));		
		strcpy(buf,"false");
	}
	PSM_Set_Record_Value2(bus_handle,g_Subsystem,"Device.DeviceInfo.X_RDKCENTRAL-COM_CaptivePortalEnable", ccsp_string,buf);

	return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
	CosaDmlDiGetCloudCapable
	(
     	BOOL *pValue
	)
{
	int rc;
    char buf[5];

	memset(buf, 0, sizeof(buf));
	rc = syscfg_get( NULL, "cloud_capable_flag", buf, sizeof(buf));
    if( rc == 0 )
    {
        if (strcmp(buf,"1") == 0)
            *pValue = TRUE;
        else
    		*pValue = FALSE;
    }
	return ANSC_STATUS_SUCCESS;
}
