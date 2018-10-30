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

    module: cosa_deviceinfo_apis.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/11/2011    initial revision.

**************************************************************************/


#ifndef  _COSA_DEVICEINFO_APIS_H
#define  _COSA_DEVICEINFO_APIS_H

#include "../middle_layer_src/cosa_apis.h"
#include "../middle_layer_src/plugin_main_apis.h"
#include "cJSON.h"

#if defined(_ANSC_LINUX)
    #include <sys/sysinfo.h>
#endif

/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/

enum
{
    CNS_FLASH_SerialNo = 1,
    CNS_FLASH_Description,
    CNS_FLASH_ModelNo,
    CNS_FLASH_NP_ID,
    CNS_FLASH_Version,
    CNS_FLASH_MFG_Date,
    CNS_FLASH_HW_Feature
};

enum
{
    COSA_DML_PROC_STATUS_Running = 1,
    COSA_DML_PROC_STATUS_Sleeping,
    COSA_DML_PROC_STATUS_Stopped,
    COSA_DML_PROC_STATUS_Idle,
    COSA_DML_PROC_STATUS_Uninterruptible,
    COSA_DML_PROC_STATUS_Zombie,
};

typedef  struct
_COSA_PROCESS_ENTRY
{
    ULONG                           Pid;
    char                            Command[256];
    ULONG                           Size;
    ULONG                           Priority;
    ULONG                           CPUTime;
    ULONG                           State;
}
COSA_PROCESS_ENTRY, *PCOSA_PROCESS_ENTRY;

typedef  struct
_COSA_DATAMODEL_RDKB_FOOTER_CLASS_CONTENT
{
        CHAR            PartnerLink[512];
        CHAR            UserGuideLink[512];
        CHAR            CustomerCentralLink[512];
        CHAR            PartnerText[512];
        CHAR            UserGuideText[512];
        CHAR            CustomerCentralText[512];
}
COSA_DATAMODEL_RDKB_FOOTER, *PCOSA_DATAMODEL_RDKB_FOOTER;

typedef  struct
_COSA_DATAMODEL_RDKB_CONNECTION_CLASS_CONTENT
{
        CHAR            MSOmenu[512];
        CHAR            MSOinfo[512];
        CHAR            StatusTitle[512];
        CHAR            StatusInfo[512];
}
COSA_DATAMODEL_RDKB_CONNECTION, *PCOSA_DATAMODEL_RDKB_CONNECTION;

typedef  struct
_COSA_DATAMODEL_RDKB_NETWORKDIAGNOSTICTOOLS_CLASS_CONTENT
{
        CHAR            ConnectivityTestURL[512];
}
COSA_DATAMODEL_RDKB_NETWORKDIAGNOSTICTOOLS, *PCOSA_DATAMODEL_RDKB_NETWORKDIAGNOSTICTOOLS;

typedef  struct
_COSA_DATAMODEL_RDKB_WIFIPERSONALIZATION_CLASS_CONTENT
{
        BOOL                    Support;
        CHAR                    PartnerHelpLink[512];
        BOOL                    SMSsupport;
        BOOL                    MyAccountAppSupport;
        CHAR                    MSOLogo[512];
        CHAR                    Title[512];
        CHAR                    WelcomeMessage[512];
}

COSA_DATAMODEL_RDKB_WIFIPERSONALIZATION, *PCOSA_DATAMODEL_RDKB_WIFIPERSONALIZATION;
typedef  struct
_COSA_DATAMODEL_RDKB_LOCALUI_CLASS_CONTENT
{
        CHAR                    MSOLogo[512];
        CHAR                    DefaultLoginUsername[512];
        CHAR                    DefaultLoginPassword[512];
	CHAR                    MSOLogoTitle[512];
        BOOL                    HomeNetworkControl;
}
COSA_DATAMODEL_RDKB_LOCALUI, *PCOSA_DATAMODEL_RDKB_LOCALUI;
typedef  struct
_COSA_DATAMODEL_RDKB_UIBRANDING_CLASS_CONTENT
{
        COSA_DATAMODEL_RDKB_FOOTER                      Footer;
        COSA_DATAMODEL_RDKB_CONNECTION                  Connection;
        COSA_DATAMODEL_RDKB_NETWORKDIAGNOSTICTOOLS      NDiagTool;
        COSA_DATAMODEL_RDKB_WIFIPERSONALIZATION         WifiPersonal;
        COSA_DATAMODEL_RDKB_LOCALUI             LocalUI;
        CHAR                    DefaultAdminIP[512];
    CHAR                        DefaultLocalIPv4SubnetRange[512];
    CHAR                        PauseScreenFileLocation[512];
}
COSA_DATAMODEL_RDKB_UIBRANDING, *PCOSA_DATAMODEL_RDKB_UIBRANDING;

typedef  struct
_COSA_DATAMODEL_RDKB_WIFI_TELEMETRY_CLASS_CONTENT
{
    INT                 LogInterval;
    CHAR            NormalizedRssiList[256];
    CHAR            CliStatList[256];
    CHAR            TxRxRateList[256];
    CHAR            SNRList[256];
}
COSA_DATAMODEL_RDKB_WIFI_TELEMETRY, *PCOSA_DATAMODEL_RDKB_WIFI_TELEMETRY;

typedef  struct
_COSA_DATAMODEL_RDKB_UNIQUE_TELEMETRY_ID_CLASS_CONTENT
{
    BOOL                Enable;
    CHAR                TagString[256];
    INT                 TimingInterval;

}
COSA_DATAMODEL_RDKB_UNIQUE_TELEMETRY_ID, *PCOSA_DATAMODEL_RDKB_UNIQUE_TELEMETRY_ID;



#define  COSA_DATAMODEL_PROCESS_CLASS_CONTENT                                                   \
    /* duplication of the base object class content */                                      \
    COSA_BASE_CONTENT                                                                       \
    ULONG                           ProcessNumberOfEntries;                             \
    ULONG                           CPUUsage;                                           \
    PCOSA_PROCESS_ENTRY             pProcTable;                                       \

typedef  struct
_COSA_DATAMODEL_PROCESS                                               
{
    COSA_DATAMODEL_PROCESS_CLASS_CONTENT
}
COSA_DATAMODEL_PROCSTATUS,  *PCOSA_DATAMODEL_PROCSTATUS;

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ANSC_HANDLE CosaProcStatusCreate();
void COSADmlRemoveProcessInfo(PCOSA_DATAMODEL_PROCSTATUS pObj);
void COSADmlGetProcessInfo(PCOSA_DATAMODEL_PROCSTATUS p_info);
ULONG COSADmlGetCpuUsage();
ULONG COSADmlGetMemoryStatus(char * ParamName);
ULONG COSADmlGetMaxWindowSize();

ANSC_STATUS
CosaDmlDiInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    );

ANSC_STATUS
CosaDmlDiGetManufacturer
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetManufacturerOUI
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetModelName
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetDescription
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetProductClass
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetSerialNumber
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetHardwareVersion
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetSoftwareVersion
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetAdditionalHardwareVersion
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetAdditionalSoftwareVersion
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetProvisioningCode
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiSetProvisioningCode
    (
        ANSC_HANDLE                 hContext,
        char*                       pProvisioningCode
    );

ULONG
CosaDmlDiGetUpTime
    (
        ANSC_HANDLE                 hContext
    );

ANSC_STATUS
CosaDmlDiGetFirstUseDate
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetHardware
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetBootloaderVersion
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetFirmwareName
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetFirmwareBuildTime
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetBaseMacAddress
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );
    
ANSC_STATUS
CosaDmlGetTCPImplementation
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize    
    );

ANSC_STATUS
CosaDmlDiGetAdvancedServices
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetProcessorSpeed
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

	
/*WebPA*/
ANSC_STATUS
CosaDmlDiGetWebPACfg
    (
        char*                       ParamName,
        char*                       pValue
    );

ANSC_STATUS
CosaDmlDiSetWebPACfg
    (
        char*                       ParamName,
        char*                       pValue
    );
	
ANSC_STATUS
isValidInput
    (
        char                       *inputparam,
        char                       *wrapped_inputparam,
    	int							lengthof_inputparam,
    	int							sizeof_wrapped_inputparam    	
    );

int setXOpsReverseSshTrigger(char *input);

int setXOpsReverseSshArgs(char* pString);

ANSC_STATUS getXOpsReverseSshArgs
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

int isRevSshActive(void);

static int writeToJson(char *data);

#endif
