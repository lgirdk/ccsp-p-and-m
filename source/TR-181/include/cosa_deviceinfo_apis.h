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

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include <cjson/cJSON.h>

#define IF_NAMESIZE	16
#define INET6_ADDRLEN   16

#define RTF_DEFAULT	0x00010000	/* default - learned via ND	*/
#define RTF_ALLONLINK	0x00020000	/* (deprecated and will be removed)
					   fallback, no routers on link */
#define RTF_ADDRCONF	0x00040000	/* addrconf route - RA		*/
#define RTF_PREFIX_RT	0x00080000	/* A prefix only route - RA	*/
#define RTF_ANYCAST	0x00100000	/* Anycast			*/
#define RTF_NONEXTHOP	0x00200000	/* route with no nexthop	*/
#define RTF_EXPIRES	0x00400000
#define RTF_ROUTEINFO	0x00800000	/* route information - RA	*/
#define RTF_CACHE	0x01000000	/* cache entry			*/
#define RTF_FLOW	0x02000000	/* flow significant route	*/
#define RTF_POLICY	0x04000000	/* policy route			*/

#ifdef FEATURE_SUPPORT_ONBOARD_LOGGING

#define LOGGING_MODULE           "PAM"
#define OnboardLog(...)          rdk_log_onboard(LOGGING_MODULE, __VA_ARGS__)
#else
#define OnboardLog(...)
#endif
#include <sys/sysinfo.h>

#include "cm_hal.h"

#define MAX_SECURITYNUM_LEN         128
#define MAX_SECURITYNAME_LEN        18      // includes room for NULL terminator

#define PARTNERID_FILE  "/nvram/.partner_ID"

/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/

enum {
    PROV_SRC_UNDEFINED,
    PROV_SRC_SERIAL,
    PROV_SRC_CM_MAC,
    PROV_SRC_WAN_MAC,
    PROV_SRC_MTA_MAC,
    PROV_SRC_CODE_SET
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

typedef  struct
_COSA_BOOTSTRAP_STR
{
    CHAR                    ActiveValue[512];
    CHAR		    UpdateSource[16];
}
COSA_BOOTSTRAP_STR;

typedef  struct
_COSA_BOOTSTRAP_BOOL
{
    BOOL                    ActiveValue;
    CHAR		    UpdateSource[16];
}
COSA_BOOTSTRAP_BOOL;

typedef  struct
COSA_AUTO_REBOOT
{
    BOOL            Enable;
    INT	            UpTime;
}
COSA_AUTO_REBOOT;


typedef  struct
_COSA_DATAMODEL_RDKB_UIBRANDING_CLASS_CONTENT
{
	COSA_BOOTSTRAP_STR                        	DefaultLanguage;
}
COSA_DATAMODEL_RDKB_UIBRANDING, *PCOSA_DATAMODEL_RDKB_UIBRANDING;

typedef  struct
_COSA_DATAMODEL_RDKB_CDLDM_CLASS_CONTENT
{
        COSA_BOOTSTRAP_STR                      CDLModuleUrl;
}
COSA_DATAMODEL_RDKB_CDLDM, *PCOSA_DATAMODEL_RDKB_CDLDM;

typedef  struct
_COSA_DATAMODEL_RDKB_WIFI_TELEMETRY_CLASS_CONTENT
{
    INT                 LogInterval;
    INT                 ChUtilityLogInterval;
    CHAR            NormalizedRssiList[256];
    CHAR            CliStatList[256];
    CHAR            TxRxRateList[256];
    CHAR            SNRList[256];
}
COSA_DATAMODEL_RDKB_WIFI_TELEMETRY, *PCOSA_DATAMODEL_RDKB_WIFI_TELEMETRY;

typedef  struct
_COSA_DATAMODEL_RDKB_UNIQUE_TELEMETRY_ID_CLASS_CONTENT
{
    BOOL		Enable;
    CHAR            	TagString[256];
    INT                 TimingInterval;

}
COSA_DATAMODEL_RDKB_UNIQUE_TELEMETRY_ID, *PCOSA_DATAMODEL_RDKB_UNIQUE_TELEMETRY_ID;

typedef  struct
_COSA_DATAMODEL_RDKB_SYNDICATIONFLOWCONTROL_CLASS_CONTENT
{
    COSA_BOOTSTRAP_BOOL    Enable;
    COSA_BOOTSTRAP_STR    InitialForwardedMark;
    COSA_BOOTSTRAP_STR    InitialOutputMark;
}
COSA_DATAMODEL_RDKB_SYNDICATIONFLOWCONTROL, *PCOSA_DATAMODEL_RDKB_SYNDICATIONFLOWCONTROL;


typedef  struct
    _COSA_DATAMODEL_KICKSTARTTABLE
{
    UINT    SecurityNumberLen;
    uint8_t SecurityNumber[MAX_SECURITYNUM_LEN];
    CHAR    SecurityName[MAX_SECURITYNAME_LEN];
}
COSA_DATAMODEL_KICKSTARTTABLE, *PCOSA_DATAMODEL_KICKSTARTTABLE;

#define COSA_DATAMODEL_KICKSTART_CLASS_CONTENT                      \
    /* duplication of the base object class content */              \
    /* COSA_BASE_CONTENT */                                         \
    BOOL                            TableUpdated;                   \
    BOOL                            Enabled;                        \
    UINT                            KickstartTotal;                 \
    UINT                            TableNumberOfEntries;           \
    COSA_DATAMODEL_KICKSTARTTABLE   KickstartTable[MAX_KICKSTART_ROWS];   

typedef  struct
    _COSA_DATAMODEL_KICKSTART
{
    COSA_DATAMODEL_KICKSTART_CLASS_CONTENT
}
COSA_DATAMODEL_KICKSTART,   *PCOSA_DATAMODEL_KICKSTART;

typedef struct
    _COSA_HHT_PTR
{
    UINT CPUThreshold;
    UINT DRAMThreshold;
    UINT Frequency;
    BOOL PTREnable;
}
COSA_HHT_PTR;

#ifdef _LG_MV2_PLUS_
#define MAX_TEMPSENSOR_INSTANCE 3
#else
#define MAX_TEMPSENSOR_INSTANCE 1
#endif

typedef  enum
_COSA_DML_TEMPERATURE_SENSOR_STATUS
{
    COSA_DML_TEMPERATURE_SENSOR_STATUS_Enabled     = 1,
    COSA_DML_TEMPERATURE_SENSOR_STATUS_Disabled,
    COSA_DML_TEMPERATURE_SENSOR_STATUS_Error
}
COSA_DML_TEMPERATURE_SENSOR_STATUS, *PCOSA_DML_TEMPERATURE_SENSOR_STATUS;

typedef  struct
_COSA_TEMPERATURE_SENSOR_ENTRY
{
    char                                    Alias[64];
    int                                     InstanceNumber;
    COSA_DML_TEMPERATURE_SENSOR_STATUS      Status;
    BOOL            	                    Enable;
    char                                    ResetTime[64];
    char                                    Name[256];
    int                                     Value;
    char                                    LastUpdate[64];
    int                                     MinValue;
    char                                    MinTime[64];
    int                                     MaxValue;
    char                                    MaxTime[64];
    int                                     LowAlarmValue;
    char                                    LowAlarmTime[64];
    int                                     HighAlarmValue;
    unsigned long                           PollingInterval;
    char                                    HighAlarmTime[64];
}
COSA_TEMPERATURE_SENSOR_ENTRY, *PCOSA_TEMPERATURE_SENSOR_ENTRY;

#define  COSA_DATAMODEL_TEMPERATURE_CLASS_CONTENT                                           \
    /* duplication of the base object class content */                                      \
    COSA_BASE_CONTENT                                                                       \
    ULONG                                      TemperatureSensorNumberOfEntries;            \
    COSA_TEMPERATURE_SENSOR_ENTRY              TemperatureSensorEntry[MAX_TEMPSENSOR_INSTANCE];

typedef  struct
_COSA_DATAMODEL_TEMPERATURE
{
    COSA_DATAMODEL_TEMPERATURE_CLASS_CONTENT
    pthread_mutex_t rwLock[MAX_TEMPSENSOR_INSTANCE];
}
COSA_DATAMODEL_TEMPERATURE_STATUS, *PCOSA_DATAMODEL_TEMPERATURE_STATUS;

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/
int getRebootCounter();
int setRebootCounter();
int setUnknownRebootReason();
ANSC_HANDLE CosaProcStatusCreate();
void COSADmlRemoveProcessInfo(PCOSA_DATAMODEL_PROCSTATUS pObj);
void COSADmlGetProcessInfo(PCOSA_DATAMODEL_PROCSTATUS p_info);
ULONG COSADmlGetCpuUsage();
ULONG COSADmlGetMemoryStatus(char * ParamName);
ULONG COSADmlGetMaxWindowSize();

ANSC_HANDLE CosaTemperatureStatusCreate (void);
void COSADmlRemoveTemperatureInfo (PCOSA_DATAMODEL_TEMPERATURE_STATUS pObj);
void CosaTemperatureSensorSetLowAlarm (int lowAlarmValue, PCOSA_TEMPERATURE_SENSOR_ENTRY pTempSensor);
void CosaTemperatureSensorSetHighAlarm (int highAlarmValue, PCOSA_TEMPERATURE_SENSOR_ENTRY pTempSensor);
void CosaTemperatureSensorSetPollingTime (ULONG pollingInterval, PCOSA_TEMPERATURE_SENSOR_ENTRY pTempSensor);
void CosaTemperatureSensorReset (BOOL isEnable, PCOSA_TEMPERATURE_SENSOR_ENTRY pTempSensor);

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
CosaDmlDiGetGW_IPv6
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

ANSC_STATUS CosaDmlDiGetProvisioningCodeSource (ANSC_HANDLE hContext, ULONG *puValue);
ANSC_STATUS CosaDmlDiSetProvisioningCodeSource (ANSC_HANDLE hContext, ULONG uValue);

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

ANSC_STATUS
isValidInput
    (
        char                       *inputparam,
        char                       *wrapped_inputparam,
        int                         lengthof_inputparam,
        int                         sizeof_wrapped_inputparam
    );

ANSC_STATUS
CosaDmlDiGetAndProcessDhcpServDetectionFlag
   (
	   ANSC_HANDLE				   hContext,
	   BOOLEAN*			   		   pValue
   );

ANSC_STATUS
CosaDmlDiSetAndProcessDhcpServDetectionFlag
   (
	   ANSC_HANDLE				   hContext,
	   BOOLEAN*					   pValue,
	   BOOLEAN*					   pDhcpServDetectEnable
   );


ANSC_STATUS
CosaDmlDiGetSyndicationPartnerId
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetSyndicationTR69CertLocation
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue
    );

ANSC_STATUS
CosaDmlDiSetSyndicationTR69CertLocation
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue
    );

#ifndef FEATURE_FWUPGRADE_MANAGER
ANSC_STATUS
CosaDmlDiSetFirmwareDownloadAndFactoryReset(void);

void CosaDmlDiGet_DeferFWDownloadReboot(ULONG* puLong);
void CosaDmlDiSet_DeferFWDownloadReboot(ULONG* DeferFWDownloadReboot , ULONG uValue);
#endif
void CosaDmlDiSet_RebootDevice(char* pValue);
void CosaDmlDiSet_DisableRemoteManagement();
BOOL CosaDmlDi_ValidateRebootDeviceParam(char *pValue);

ANSC_STATUS
CosaDmlDiUiBrandingInit
  (
	ANSC_HANDLE                 hContext,
	PCOSA_DATAMODEL_RDKB_UIBRANDING	PUiBrand,
	PCOSA_DATAMODEL_RDKB_CDLDM PCdlDM
  );

void FillPartnerIDValues(cJSON *json , char *partnerID , PCOSA_DATAMODEL_RDKB_UIBRANDING PUiBrand, PCOSA_DATAMODEL_RDKB_CDLDM PCdlDM, ANSC_HANDLE hContext);

ANSC_STATUS UpdateJsonParam
	(
		char*           pKey,
		char*			PartnerId,
		char*			pValue,
		char*                   pSource,
		char*			pCurrentTime
    );

ANSC_STATUS
CosaDmlDiWiFiTelemetryInit
  (
	PCOSA_DATAMODEL_RDKB_WIFI_TELEMETRY PWiFi_Telemetry
  );

ANSC_STATUS
CosaDmlDiUniqueTelemetryIdInit
  (
	PCOSA_DATAMODEL_RDKB_UNIQUE_TELEMETRY_ID PUniqueTelemetryId
  );

void ConvertTime(int time, char day[], char hour[], char mins[]);

void UniqueTelemetryCronJob(BOOL enable, INT timeInterval, char* tagString);

ULONG
CosaDmlDiGetBootTime
    (
        ANSC_HANDLE                 Context
    );

ANSC_STATUS
CosaDmlDiGetFactoryResetCount
    (
        ANSC_HANDLE                 hContext,
        ULONG                       *pValue
    );

ANSC_STATUS
CosaDmlDiGetHardware_MemUsed
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetHardware_MemFree
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDmlDiGetCMTSMac
    (
        ANSC_HANDLE                 hContext,
        char*                       pValue,
        ULONG*                      pulSize
    );

ANSC_STATUS CosaDmlDiClearResetCount
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
   );

ANSC_STATUS
CosaDmlSetnewNTPEnable(BOOL bValue);

BOOL CosaDmlGetInternetStatus();

void CosaDmlPresenceEnable(BOOL enable);


ANSC_STATUS
CosaDmlDiSet_SyndicationFlowControl_Enable
    (
        char bValue
    );

ANSC_STATUS
CosaDmlDiSet_SyndicationFlowControl_InitialForwardedMark
    (
        char *pString
    );

ANSC_STATUS
CosaDmlDiSet_SyndicationFlowControl_InitialOutputMark
    (
        char *pString
    );

ANSC_STATUS 
setTempPartnerId
    (
        char*                       pValue
    );

ANSC_STATUS 
getFactoryPartnerId
    (
        char*                       pValue,
        PULONG                      pulSize
    );

ANSC_STATUS
CosaDeriveSyndicationPartnerID
    (
        char *Partner_ID
    );

ANSC_STATUS
CosaDmlDiSyndicationFlowControlInit
    (
        PCOSA_DATAMODEL_RDKB_SYNDICATIONFLOWCONTROL pSyndicatonFlowControl
    );

ANSC_STATUS
CosaDmlDiRfcDefaultsInit
    (
        cJSON **pRfcDefaults
    );

ANSC_STATUS
CosaDmlDiRfcStoreInit
    (
        cJSON **pRfcStore
    );

ANSC_STATUS
ProcessRfcSet(cJSON **pRfcStore, BOOL clearDB, char *paramFullName, char *value, char *pSource, char *pCurrentTime);

ANSC_STATUS
CosaDmlScheduleAutoReboot(int ConfiguredUpTime, BOOL bValue);

int 
setMultiProfileXdnsConfig(BOOL bValue);

#if defined (FEATURE_SUPPORT_RADIUSGREYLIST)
BOOL
CosaDmlSetRadiusGreyListEnable
    (
        BOOL        bValue
    );
#endif
#if defined(FEATURE_HOSTAP_AUTHENTICATOR)
BOOL
CosaDmlSetNativeHostapdState
    (
        BOOL        bValue
    );
#endif //FEATURE_HOSTAP_AUTHENTICATOR

BOOL CosaDmlSetDFS(BOOL bValue);

#endif
