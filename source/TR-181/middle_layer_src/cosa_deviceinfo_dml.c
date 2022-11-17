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

    module: cosa_deviceinfo_dml.c

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

    -------------------------------------------------------------------

    revision:

        01/14/2011    initial revision.

**************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <syscfg/syscfg.h>
#include "ansc_platform.h"
#include "ansc_string_util.h"
#include "cosa_deviceinfo_dml.h"
#include "dml_tr181_custom_cfg.h"
#include "safec_lib_common.h"
#include "secure_wrapper.h"
#include "cosa_drg_common.h"
#include "cosa_deviceinfo_apis.h"
#include "cosa_deviceinfo_internal.h"
#include "ccsp/platform_hal.h"
#ifdef _MACSEC_SUPPORT_
#include "ccsp/ccsp_hal_ethsw.h"
#endif
#include <syscfg/syscfg.h>
#include <sys/statvfs.h>
#include "dslh_definitions_database.h"

#if defined (_XB6_PRODUCT_REQ_)
#include "bt_hal.h"
#endif

#if defined (_ARRIS_XB6_PRODUCT_REQ_) //ARRISXB6-7328, ARRISXB6-7332
#include "cm_hal_oem.h"
#endif

#if defined(_PLATFORM_RASPBERRYPI_)
#include <unistd.h>
#include <sys/types.h>
#endif

#include "ccsp_base_api.h"
#include "messagebus_interface_helper.h"

#include <stdbool.h>
#include "cosa_deviceinfo_apis.h"
#include "ccsp_psm_helper.h"
#include <rbus/rbus.h>

extern ULONG g_currentBsUpdate;
extern char g_currentParamFullName[512];
extern ANSC_HANDLE bus_handle;
extern char g_Subsystem[32];
extern void* g_pDslhDmlAgent;
static BOOL g_clearDB = false;

void Send_Notification_Task(char* delay, char* startTime, char* download_status, char* status, char *system_ready_time, char * priority,  char *current_fw_ver, char *download_fw_ver);
void set_firmware_download_start_time(char *start_time);
char* get_firmware_download_start_time();

#if (defined _COSA_INTEL_XB3_ARM_)
BOOL CMRt_Isltn_Enable(BOOL status);
#endif

#define MAX_ALLOWABLE_STRING_LEN  256

#define MAX_T2_VER_LEN 16

#define IS_UPDATE_ALLOWED_IN_DM(paramName, requestorStr) ({                                                                                       \
    int rc = -1;                                                                                                                                  \
    int ind = -1;                                                                                                                                 \
    int found = 0;                                                                                                                                \
    if( g_currentBsUpdate == DSLH_CWMP_BS_UPDATE_firmware )                                                                                       \
    {                                                                                                                                             \
         found = 1;                                                                                                                               \
    }                                                                                                                                             \
    else if(g_currentBsUpdate == DSLH_CWMP_BS_UPDATE_rfcUpdate)                                                                                   \
    {                                                                                                                                             \
        rc = strcmp_s(BS_SOURCE_RFC_STR, strlen(BS_SOURCE_RFC_STR), requestorStr, &ind);                                                          \
        ERR_CHK(rc);                                                                                                                              \
        if((rc == EOK) && (ind))                                                                                                                  \
        {                                                                                                                                         \
            found = 1;                                                                                                                            \
        }                                                                                                                                         \
    }                                                                                                                                             \
    if(found == 1)                                                                                                                                \
    {                                                                                                                                             \
        CcspTraceWarning(("Do NOT allow override of param: %s bsUpdate = %lu, requestor = %s\n", paramName, g_currentBsUpdate, requestorStr));     \
        return FALSE;                                                                                                                             \
    }                                                                                                                                             \
})

// If the requestor is RFC but the param was previously set by webpa, do not override it.
#define IS_UPDATE_ALLOWED_IN_JSON(paramName, requestorStr, UpdateSource) ({                                                                        \
   int rc = -1;                                                                                                                                    \
   int ind = -1;                                                                                                                                   \
   rc = strcmp_s(BS_SOURCE_RFC_STR, strlen(BS_SOURCE_RFC_STR), requestorStr, &ind);                                                                \
   ERR_CHK(rc);                                                                                                                                    \
   if((rc == EOK) && (!ind))                                                                                                                       \
   {                                                                                                                                               \
      rc = strcmp_s(BS_SOURCE_WEBPA_STR, strlen(BS_SOURCE_WEBPA_STR), UpdateSource, &ind);                                                         \
      ERR_CHK(rc);                                                                                                                                 \
      if((rc == EOK) && (!ind))                                                                                                                    \
      {                                                                                                                                            \
        CcspTraceWarning(("Do NOT allow override of param: %s requestor = %lu updateSource = %s\n", paramName, g_currentWriteEntity, UpdateSource));  \
        return FALSE;                                                                                                                              \
      }                                                                                                                                            \
   }                                                                                                                                               \
})

#define DEVICE_PROPS_FILE  "/etc/device.properties"
#define SYSTEMCTL_CMD "systemctl start lxydnld.service &"
#ifdef FEATURE_COGNITIVE_WIFIMOTION
#define COGNITIVE_WIFIMOTION_CFG "wifimotion_enabled"
#endif
// CredDwnld_Use String is restricted to true/false
#define MAX_USE_LEN 8
// Box type will be XB3
#define BOX_TYPE_LEN 5


#ifdef COLUMBO_HWTEST
//RDKB-33114: Default values defined here due to objects not being persistent by design
#define DEFAULT_HWST_PTR_CPU_THRESHOLD 80
#define DEFAULT_HWST_PTR_DRAM_THRESHOLD 20
#define DEFAULT_HWST_PTR_FREQUENCY 720
#define HWSELFTEST_START_MIN_SPACE (200*1024) //200KB
#endif

#ifdef _COSA_INTEL_XB3_ARM_
    #define BLOCKLIST_FILE "/nvram/Blocklist_XB3.txt"
#else
    #define BLOCKLIST_FILE "/opt/secure/Blocklist_file.txt"
#endif

#if defined(_PLATFORM_RASPBERRYPI_)
int sock;
int id = 0;
#endif

#define NUM_OF_DEVICEINFO_VALUES (sizeof(deviceinfo_set_table)/sizeof(deviceinfo_set_table[0]))

enum  pString_val {
    UIACCESS,
    UISUCCESS,
    UIFAILED,
    REBOOTDEVICE,
    FACTORYRESET,
    CAPTIVEPORTALFAILURE
};

typedef struct {
  char     *name;
  enum  pString_val type;
} DEVICEINFO_SET_VALUE;

static const DEVICEINFO_SET_VALUE deviceinfo_set_table[] = {
    { "ui_access",UIACCESS },
    { "ui_success",UISUCCESS},
    { "ui_failed", UIFAILED },
    { "reboot_device", REBOOTDEVICE},
    { "factory_reset",	FACTORYRESET },
    {  "captiveportal_failure" , CAPTIVEPORTALFAILURE }
};

#if defined (_COSA_BCM_ARM_)
static int read_param_string_from_file (char *filename, char *pValue, ULONG *pUlSize)
{
    FILE *fp;

    if ((fp = fopen(filename, "r")) != NULL) {
        if (fgets(pValue, *pUlSize, fp) == NULL) {
            *pValue = 0;
        }
        else {
            size_t len = strlen(pValue);
            if ((len > 0) && (pValue[len - 1] == '\n'))
                pValue[len - 1] = 0;
        }
        fclose(fp);
    }
    else {
        *pValue = 0;
    }

    return 0;
}
#endif

static int GetFirmwareName (char *pValue, unsigned long maxSize)
{
    static char name[64];

    if (name[0] == 0)
    {
        FILE *fp;
        char buf[128];  /* big enough to avoid reading incomplete lines */
        char *s = NULL;
        size_t len = 0;

        if ((fp = fopen ("/version.txt", "r")) != NULL)
        {
            while (fgets (buf, sizeof(buf), fp) != NULL)
            {
                /*
                   The imagename field may use either a ':' or '=' separator
                   and the value may or not be quoted. Handle all 4 cases.
                */
                if ((memcmp (buf, "imagename", 9) == 0) && ((buf[9] == ':') || (buf[9] == '=')))
                {
                    s = (buf[10] == '"') ? &buf[11] : &buf[10];

                    while (1)
                    {
                        int inch = s[len];

                        if ((inch == '"') || (inch == '\n') || (inch == 0))
                        {
                            break;
                        }

                        len++;
                    }

                    break;
                }
            }

            fclose (fp);
        }

        if (len >= sizeof(name))
        {
            len = sizeof(name) - 1;
        }

        memcpy (name, s, len);
        name[len] = 0;
    }

    if (name[0] != 0)
    {
        size_t len = strlen(name);

        if (len >= maxSize)
        {
            len = maxSize - 1;
        }

        memcpy (pValue, name, len);
        pValue[len] = 0;

        return 0;
    }

    pValue[0] = 0;

    return -1;
}

static int update_pValue (char *pValue, PULONG pulSize, char *str)
{
    if (!str)
        return -1;

    size_t len = strlen(str);

    if (len < *pulSize)
    {
        memcpy(pValue, str, len + 1);
        return 0;
    }

    *pulSize = len + 1;
    return 1;
}

static int get_deviceinfo_from_name(char *name, enum pString_val *type_ptr)
{
  int rc = -1;
  int ind = -1;
  int i = 0;
  size_t strsize = 0;
  if((name == NULL) || (type_ptr == NULL))
     return 0;

  strsize = strlen(name);

  for (i = 0 ; i < (int)NUM_OF_DEVICEINFO_VALUES ; ++i)
  {
      rc = strcmp_s(name, strsize, deviceinfo_set_table[i].name, &ind);
      ERR_CHK(rc);
      if((rc == EOK) && (!ind))
      {
          *type_ptr = deviceinfo_set_table[i].type;
          return 1;
      }
  }
  return 0;
}

#ifdef COLUMBO_HWTEST
static unsigned long long GetAvailableSpace_tmp()
{
    //Check if there is enough space to atleast start HHT.
    unsigned long long result = 0;
    struct statvfs sfs;
    if(statvfs("/tmp", &sfs) != -1)
    {
        result = (unsigned long long)sfs.f_bsize * sfs.f_bavail;
        CcspTraceInfo(("%llu space left in tmp\n", result));
    }
    return result;
}

static void UpdateSettingsFile( char param[64], char value[10] )
{
    CcspTraceInfo(("\nUpdateSettingsFile\n"));
    errno_t          rc                  = -1;

    FILE* fp = fopen( "/tmp/.hwselftest_settings", "r");
    if( fp == NULL)
    {
        fp = fopen ("/tmp/.hwselftest_settings", "w");

        if( fp != NULL)
        {
          fprintf(fp, "%s%s\n", param, value);
          fclose(fp);
        }
        return;
    }

    char Line[120] = {'\0'};
    char FileData[1200] = {'\0'};
    int isFound = 0;
    int firstLine = 0;

    while(NULL != fgets(Line, 120, fp))
    {
        if(NULL != strstr(Line, param))
        {
            int index  = 0;
            isFound = 1;
            int startIndex = strlen(Line) - (strlen(Line) - strlen(param));
            int endIndex = strlen(value);

            for(index = 0; index <  endIndex;index ++)
            {
                Line[startIndex] = value[index];
                startIndex++;
            }
            Line[startIndex] = '\0';
            strcat(Line,"\n");
        }

        if(0 == firstLine)
        {
            if(strstr(param, "FREQ") && strstr(Line, "FirstPTR"))
            {
                continue;
            }
            firstLine = 1;
            rc = strcpy_s(FileData, sizeof(FileData), Line);
            if(rc != EOK)
            {
               ERR_CHK(rc);
               fclose(fp);
               return;
            }
        }
        else
        {
            if(strstr(param, "FREQ") && strstr(Line, "FirstPTR"))
            {
                continue;
            }
            strcat(FileData, Line);
        }

    }
    fclose(fp);

    if(0 == isFound)
    {
        rc = sprintf_s(Line, sizeof(Line), "%s%s\n", param,value);
        if(rc < EOK)
        {
          ERR_CHK(rc);
          return;
        }

        strcat(FileData, Line);
    }

    fp = fopen ("/tmp/.hwselftest_settings", "w");

    if( fp != NULL)
    {
        fputs(FileData,fp);
        fclose(fp);
    }
}
#endif

/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply 
 the changes to all of the specified Parameters atomically. That is, either 
 all of the value changes are applied together, or none of the changes are 
 applied at all. In the latter case, the CPE MUST return a fault response 
 indicating the reason for the failure to apply the changes. 
 
 The CPE MUST NOT apply any of the specified changes without applying all 
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }
 
***********************************************************************/

/***********************************************************************

 APIs for Object:

    DeviceInfo.

    *  DeviceInfo_GetParamBoolValue
    *  DeviceInfo_GetParamIntValue
    *  DeviceInfo_GetParamUlongValue
    *  DeviceInfo_GetParamStringValue
    *  DeviceInfo_SetParamBoolValue
    *  DeviceInfo_SetParamIntValue
    *  DeviceInfo_SetParamUlongValue
    *  DeviceInfo_SetParamStringValue
    *  DeviceInfo_Validate
    *  DeviceInfo_Commit
    *  DeviceInfo_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DeviceInfo_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    BOOL                            bReturnValue;
    
    if (strcmp(ParamName, "ClearResetCount") == 0)
    {
        *pBool = FALSE ; 
        return TRUE;
    }

    if (strcmp(ParamName, "X_RDKCENTRAL-COM_AkerEnable") == 0)
    {
        /* collect value */
        char buf[8];
        if( syscfg_get( NULL, "X_RDKCENTRAL-COM_AkerEnable", buf, sizeof(buf))==0)
        {
            if (strcmp(buf, "true") == 0)
            {
                *pBool = TRUE;
            }
            else
            {
                *pBool = FALSE;
            }
        }
        else
        {
            CcspTraceWarning(("%s syscfg_get failed  for AkerEnable\n",__FUNCTION__));
            *pBool = FALSE;
        }

        return TRUE;
    }

  bReturnValue =
        DeviceInfo_GetParamBoolValue_Custom
            (
                hInsContext,
                ParamName,
                pBool
            );

    if ( bReturnValue )
    {
        return bReturnValue;
    }
    else
    {
        /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
        return FALSE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DeviceInfo_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
#ifndef FEATURE_FWUPGRADE_MANAGER
    if (strcmp(ParamName, "X_RDKCENTRAL-COM_FirmwareDownloadAndFactoryReset") == 0)
    {
        *pInt = 0;
        return TRUE;
    }
#else
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
#endif
#ifdef CISCO_XB3_PLATFORM_CHANGES
    if (strcmp(ParamName, "PingPeerRebootThreshold") == 0)
    {
        char buf[10] = { 0 };
        syscfg_get(NULL, "ping_peer_reboot_threshold", buf, sizeof (buf));
        if (buf[0] != '\0')
        {
            *pInt = atoi(buf);
            return TRUE;
        }
    }
#endif
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DeviceInfo_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "UpTime") == 0)
    {
        /* collect value */
        *puLong = CosaDmlDiGetUpTime(NULL);
        return TRUE;
    }
	/* Required for xPC sync */
	if (strcmp(ParamName, "X_RDKCENTRAL-COM_ConfigureDocsicPollTime") == 0)
    {
     #if !defined(_COSA_BCM_MIPS_) && !defined(_ENABLE_DSL_SUPPORT_)
        /* collect value */
	   FILE *fp;
	   char buff[30];
	   int retValue;
	   memset(buff,0,sizeof(buff));
           *puLong = 0;
   	   fp = fopen("/nvram/docsispolltime.txt", "r");
	   if(!fp)
	   {
		 CcspTraceError(("%s falied to open /nvram/docsispolltime.txt file \n",__FUNCTION__));
		 return FALSE;
	   }
           retValue = fscanf(fp, "%s", buff);      

           if( (retValue != -1) && (buff != NULL ) )
	   {
          	 *puLong = atoi(buff);
           }

	   fclose(fp);
	 #else
	   *puLong = 0;
	 #endif
           return TRUE;
    }
    
    if (strcmp(ParamName, "X_RDKCENTRAL-COM_BootTime") == 0)
    {
        /* collect value */
       *puLong = CosaDmlDiGetBootTime(NULL);
        return TRUE;
    }
    
    if (strcmp(ParamName, "FactoryResetCount") == 0)
    {
        /* collect value */
        CosaDmlDiGetFactoryResetCount(NULL,puLong);
        return TRUE;
    }

    if(strcmp(ParamName, "X_LGI-COM_ProvisioningCodeSource") == 0)
    {
        *puLong = pMyObject->ProvisioningCodeSource;
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DeviceInfo_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
DeviceInfo_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    ULONG                           ReturnValue;
    errno_t                         rc        = -1;
    
     strcpy(pValue, "");
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "DeviceCategory") == 0)
    {
#if defined (FEATURE_GPON)
        return update_pValue (pValue, pulSize, "Fiber_Gateway");
#else
        return update_pValue (pValue, pulSize, "DOCSIS_Gateway");
#endif
    }

    if (strcmp(ParamName, "Manufacturer") == 0)
    {
        CosaDmlDiGetManufacturer(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "ManufacturerOUI") == 0)
    {
        CosaDmlDiGetManufacturerOUI(NULL, pValue, pulSize);
        return 0;
    }

    if ((strcmp(ParamName, "ModelName") == 0) ||
        (strcmp(ParamName, "ModelNumber") == 0))
    {
        if (*pulSize <= 64) {
            *pulSize = 64 + 1;
            return 1;
        }

        if (platform_hal_GetModelName(pValue) != RETURN_OK)
            return -1;

        return 0;
    }

    if (strcmp(ParamName, "Description") == 0)
    {
        CosaDmlDiGetDescription(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "ProductClass") == 0)
    {
        CosaDmlDiGetProductClass(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "SerialNumber") == 0)
    {
#if defined (_PUMA6_ARM_)
        if (*pulSize <= 18) {
            *pulSize = 18 + 1;
            return 1;
        }

        if (platform_hal_GetCmMacAddress(pValue, *pulSize) != RETURN_OK)
            return -1;
#else
        CosaDmlDiGetSerialNumber(NULL, pValue, pulSize);
#endif
        return 0;
    }

    /* This parameter always returns the actual serial number */
    if (strcmp(ParamName, "X_LGI-COM_SerialNumber") == 0)
    {
        return CosaDmlDiGetSerialNumber(NULL, pValue, pulSize);
    }

    if (strcmp(ParamName, "X_LGI-COM_GW_IPv6") == 0)
    {
        CosaDmlDiGetGW_IPv6(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "HardwareVersion") == 0)
    {
        if (*pulSize <= 64) {
            *pulSize = 64 + 1;
            return 1;
        }

        if (platform_hal_GetHardwareVersion(pValue) != RETURN_OK)
            return -1;

        return 0;
    }

    if (strcmp(ParamName, "AdditionalHardwareVersion") == 0)
    {
        return update_pValue (pValue, pulSize, "");
    }

    if (strcmp(ParamName, "SoftwareVersion") == 0)
    {
        if (*pulSize <= 64) {
            *pulSize = 64 + 1;
            return 1;
        }

        if (platform_hal_GetSoftwareVersion(pValue, *pulSize) != RETURN_OK)
            return -1;

        return 0;
    }

    if ((strcmp(ParamName, "AdditionalSoftwareVersion") == 0) ||
        (strcmp(ParamName, "X_CISCO_COM_FirmwareName") == 0) ||
        (strcmp(ParamName, "X_RDK_FirmwareName") == 0))
    {
        if (*pulSize <= 64) {
            *pulSize = 64 + 1;
            return 1;
        }

        if (GetFirmwareName(pValue, *pulSize) != 0)
            return -1;

        return 0;
    }

    if (strcmp(ParamName, "ProvisioningCode") == 0)
    {
        CosaDmlDiGetProvisioningCode(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "FirstUseDate") == 0)
    {
        CosaDmlDiGetFirstUseDate(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "X_CISCO_COM_BootloaderVersion") == 0)
    {
        CosaDmlDiGetBootloaderVersion(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "X_LGI-COM_BootloaderBuildTime") == 0)
    {
        if (*pulSize <= 64) {
            *pulSize = 64 + 1;
            return 1;
        }

#if defined(_COSA_BCM_ARM_)
        return read_param_string_from_file ("/proc/device-tree/bolt/date", pValue, pulSize);
#else
        return update_pValue (pValue, pulSize, "UNKNOWN");
#endif
    }

    if (strcmp(ParamName, "X_CISCO_COM_FirmwareBuildTime") == 0)
    {
        CosaDmlDiGetFirmwareBuildTime(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "X_CISCO_COM_BaseMacAddress") == 0)
    {
        CosaDmlDiGetBaseMacAddress(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "Hardware") == 0)
    {
        CosaDmlDiGetHardware(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "Hardware_MemUsed") == 0)
    {
        CosaDmlDiGetHardware_MemUsed(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "Hardware_MemFree") == 0)
    {
        CosaDmlDiGetHardware_MemFree(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "X_CISCO_COM_AdvancedServices") == 0)
    {
        CosaDmlDiGetAdvancedServices(NULL, pValue, pulSize);
        return 0;
    }

    if (strcmp(ParamName, "X_CISCO_COM_ProcessorSpeed") == 0)
    {
        CosaDmlDiGetProcessorSpeed(NULL, pValue, pulSize);
        return 0;
    }

    /* Required for WebPA timestamp */
    if (strcmp(ParamName, "X_RDKCENTRAL-COM_SystemTime") == 0)
    {
        struct timespec sysTime;

        if( clock_gettime(CLOCK_REALTIME, &sysTime) != -1 )
        {
              static long prevtime_sec = 0;
              static long prevtime_nsec = 0;
              static int count = 0;
              char sbuf[32] = {0};

              if( (sysTime.tv_sec == prevtime_sec) && (sysTime.tv_nsec == prevtime_nsec) )
              {
                    count++;
              } else {
                    count = 0;
              }
              prevtime_sec = sysTime.tv_sec;
              prevtime_nsec = sysTime.tv_nsec;
              sysTime.tv_nsec += count;

              if( sysTime.tv_nsec > 999999999L)
              {
                   sysTime.tv_sec = sysTime.tv_sec + 1;
                   sysTime.tv_nsec = sysTime.tv_nsec - 1000000000L;
              }                   

              rc = sprintf_s(sbuf, sizeof(sbuf), "%ld.%09ld", sysTime.tv_sec, sysTime.tv_nsec);
              if(rc < EOK)
              {
                ERR_CHK(rc);
                return -1;
              }

              rc = strcpy_s(pValue, *pulSize, sbuf);
              if(rc != EOK)
              {
                ERR_CHK(rc);
                return -1;
              }
        }
        return 0;
    }

    if (strcmp(ParamName, "X_RDKCENTRAL-COM_LastRebootReason") == 0)
    {
        syscfg_get(NULL, "X_RDKCENTRAL-COM_LastRebootReason", pValue, *pulSize);
        return 0;
    }

    if (strcmp(ParamName, "X_RDK_RDKProfileName") == 0)
    {
        return update_pValue(pValue, pulSize, "RDKB");
    }

    /* Changes for EMS begins here */

    if (strcmp(ParamName, "X_COMCAST-COM_EMS_MobileNumber") == 0)
    {
        return update_pValue(pValue, pulSize, pMyObject->EMS_MobileNo);
    }

    if (strcmp(ParamName, "X_COMCAST-COM_EMS_ServerURL") == 0)
    {
        // Not used in LG environment, updating empty url to avoid adding some wrong vendor URL
        return update_pValue (pValue, pulSize, "");
    }

    if (strcmp(ParamName, "RouterName") == 0)
    {
        syscfg_get(NULL, "router_name", pValue, *pulSize);
        return 0;
    }

    /* Changes for EMS end here */

#if !defined(_COSA_BCM_MIPS_) && !defined(_ENABLE_DSL_SUPPORT_)

    /* Changes for RDKB-6560 */

    if (strcmp(ParamName, "X_RDKCENTRAL-COM_CMTS_MAC") == 0)
    {
        CosaDmlDiGetCMTSMac(NULL, pValue, pulSize);
        return 0;
    }

    /* Changes for RDKB-6560 end */

#endif

    /* Changes for RDKB-5878 */

    if (strcmp(ParamName, "X_RDKCENTRAL-COM_CloudPersonalizationURL") == 0)
    {
        syscfg_get(NULL, "CloudPersonalizationURL", pMyObject->CloudPersonalizationURL, sizeof(pMyObject->CloudPersonalizationURL));
        return update_pValue(pValue, pulSize, pMyObject->CloudPersonalizationURL);
    }

    /* Changes for RDKB-5878 end */

    ReturnValue =
        DeviceInfo_GetParamStringValue_Custom
            (
                hInsContext,
                ParamName,
                pValue,
                pulSize
            );

    if ( ReturnValue == 0 )
    {
        return ReturnValue;
    }
    else
    {
        /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
        return -1;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DeviceInfo_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    BOOL                            bReturnValue;
#if defined(_PLATFORM_RASPBERRYPI_)
    id =getuid();    
#endif
    
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "ClearResetCount") == 0)
    {
        /* collect value */
        CosaDmlDiClearResetCount(NULL,bValue);
        return TRUE;
    }

    if (strcmp(ParamName, "X_RDKCENTRAL-COM_AkerEnable") == 0)
    {
        if (syscfg_set_commit(NULL, "X_RDKCENTRAL-COM_AkerEnable", bValue ? "true" : "false") != 0)
        {
            AnscTraceWarning(("syscfg_set failed for AkerEnable\n"));
        }
        else
        {
		/* Restart Firewall */
		v_secure_system("sysevent set firewall-restart");
#if defined(_PLATFORM_RASPBERRYPI_)
               if(id!=0)
               {
                       char *lxcevt = "sysevent set firewall-restart";
                       send(sock , lxcevt , strlen(lxcevt) , 0 );
               }
#endif
        }

        return TRUE;
    }

    bReturnValue =
        DeviceInfo_SetParamBoolValue_Custom
            (
                hInsContext,
                ParamName,
                bValue
            );

    if ( bReturnValue )
    {
        return bReturnValue;
    }
    else
    {
        /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
        return FALSE;
    }
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DeviceInfo_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */
#ifndef FEATURE_FWUPGRADE_MANAGER
    if (strcmp(ParamName, "X_RDKCENTRAL-COM_FirmwareDownloadAndFactoryReset") == 0)
    {
        if( iValue == 1)
        {
            if(ANSC_STATUS_SUCCESS != CosaDmlDiSetFirmwareDownloadAndFactoryReset())
            {
                return FALSE;
            }
        }
	return TRUE;
    }
#else
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);
#endif
#ifdef CISCO_XB3_PLATFORM_CHANGES
    if (strcmp(ParamName, "PingPeerRebootThreshold") == 0)
    {
        int ret = 0;
        char buf[10] = { 0 };
        snprintf(buf, sizeof(buf) - 1, "%d", iValue);
        if ((ret = syscfg_set_commit(NULL, "ping_peer_reboot_threshold", buf)) != 0)
        {
            CcspTraceError(("syscfg_set ping_peer_reboot_threshold failed. Ret %d\n", ret));
        }
        return TRUE;
    }
#endif
	/* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DeviceInfo_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "X_RDKCENTRAL-COM_ConfigureDocsicPollTime") == 0)
    {
        /* collect value */
           FILE *fp;
           char buff[30];
	   snprintf(buff,sizeof(buff),"%d",(int)uValue);

           fp = fopen("/nvram/docsispolltime.txt", "w+");
           if(!fp)
           {
                 CcspTraceError(("%s falied to open /nvram/docsispolltime.txt file \n",__FUNCTION__));
                 return FALSE;
           }
	   fprintf(fp, "%s\n", buff);
 	   fclose(fp);
	   return TRUE;
    } 

    if (strcmp(ParamName, "X_LGI-COM_ProvisioningCodeSource") == 0)
    {
        pMyObject->ProvisioningCodeSource = uValue;
        CosaDmlDiSetProvisioningCodeSource(NULL, pMyObject->ProvisioningCodeSource);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**
 *  RFC Features TelemetryEndpoint
*/

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TelemetryEndpoint_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
TelemetryEndpoint_GetParamBoolValue

(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8];

    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        /* CID: 58662 Array compared against 0*/
        if(!syscfg_get( NULL, "TelemetryEndpointEnabled", buf, sizeof(buf)))
        {
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        }
        return TRUE;                }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TelemetryEndpoint_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TelemetryEndpoint_SetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, TelemetryEndpoint_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit (NULL, "TelemetryEndpointEnabled", bValue ? "true" : "false") != 0)
        {
            CcspTraceError(("syscfg_set TelemetryEndpointEnabled failed\n"));
            return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TelemetryEndpoint_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue
                The string value buffer;


    return:     TRUE if succeeded;
                FALSE if not supported.

**********************************************************************/
ULONG
    TelemetryEndpoint_GetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                       pValue,
 ULONG*                      pUlSize
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    errno_t   rc   = -1;
    /* Required for xPC sync */
    if (strcmp(ParamName, "URL") == 0)
    {
        /* collect value */
        char buf[64];
        memset(buf, 0 ,sizeof(buf));
        /* CID: 70225 Array compared against 0*/
        /* CID: 56488 Logically dead code*/
        if(!syscfg_get( NULL, "TelemetryEndpointURL", buf, sizeof(buf)))
        {
            rc = strcpy_s(pValue, *pUlSize, buf);
            if(rc != EOK)
            {
                ERR_CHK(rc);
                return -1;
            }
            return 0;
        }
        return -1;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TelemetryEndpoint_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

	**********************************************************************/
BOOL
    TelemetryEndpoint_SetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                       pString
 )
{
    if (IsStringSame(hInsContext, ParamName, pString, Telemetry_GetParamStringValue))
        return TRUE;

    /* Required for xPC sync */
    if (strcmp(ParamName, "URL") == 0)
    {
        if (syscfg_set_commit(NULL, "TelemetryEndpointURL", pString) != 0)
        {
            CcspTraceError(("syscfg_set failed\n"));

        }
        else
        {
            return TRUE;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

static BOOL valid_url (char *buff)
{
    int i=0;
    char *token=NULL;
    char *delim=".";
    char *str=NULL;
    int len=_ansc_strlen(buff);
    int count=0;

    while(buff[i] != '\0')
    {
        //Allowing only integers, alphabets(lower and upper) and certain special characters
        if(((buff[i] >= '-') && (buff[i] <= ':')) || ((buff[i]>='A') && (buff[i]<='Z')) || ((buff[i]>='a') && (buff[i]<='z')) || (buff[i]=='#') || (buff[i]=='@') || (buff[i]=='~'))
            i++;
        else
            return FALSE;
    }    
    if((strncasecmp(buff,"http://",_ansc_strlen("http://"))!=0) && (strncasecmp(buff,"https://",_ansc_strlen("https://"))!=0))
        return FALSE;

    if(buff[len-1] == '.')
        return FALSE;

    str = strdup(buff);
    if (! str) {
        return FALSE;
    }

    token=strtok(str,delim);
    while(token!=NULL)
    {
        count++;
        token=strtok(NULL,delim);
    }
    if(count<2)
    {
        AnscFreeMemory(str);
        return FALSE;
    }
    AnscFreeMemory(str);

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DeviceInfo_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    ANSC_STATUS ret=ANSC_STATUS_FAILURE;
    errno_t rc =-1;
    int ind =-1; 
    /* check the parameter name and set the corresponding value */
    rc = strcmp_s("ProvisioningCode", strlen("ProvisioningCode"),ParamName, &ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
         rc = STRCPY_S_NOCLOBBER((char *)pMyObject->ProvisioningCode,sizeof(pMyObject->ProvisioningCode), pString);
         if(rc != EOK)
         {
              ERR_CHK(rc);
               return FALSE;
          }

        CosaDmlDiSetProvisioningCode(NULL, (char *)pMyObject->ProvisioningCode);
        return TRUE;
    }
#ifdef CONFIG_INTERNET2P0
    rc = strcmp_s( "X_RDKCENTRAL-COM_CloudUIWebURL",strlen("X_RDKCENTRAL-COM_CloudUIWebURL"),ParamName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))

    {
        char wrapped_inputparam[256];

	   ret=isValidInput(pString,wrapped_inputparam, AnscSizeOfString(pString), sizeof( wrapped_inputparam ));
	  if(ANSC_STATUS_SUCCESS != ret)
	    return FALSE;

        if (syscfg_set_commit(NULL, "redirection_url", pString) != 0)
        {
            AnscTraceWarning(("syscfg_set failed\n"));
            return FALSE;
        }
        else
        {
            char url[400];
            snprintf(url,sizeof(url),"/etc/whitelist.sh '%s'", pString);
            system(url);
            AnscCopyString(pMyObject->WebURL, pString);
            CcspTraceWarning(("CaptivePortal:Cloud URL is changed, new URL is %s ...\n",pMyObject->WebURL));
            return TRUE;
       }
    }
#endif
#define BUFF_SIZE 2048
    int len=0;
    /*Changes for 5878 start*/
    
    if(pString == NULL)
    return FALSE;
    
    rc = strcmp_s("X_RDKCENTRAL-COM_CloudPersonalizationURL",strlen( "X_RDKCENTRAL-COM_CloudPersonalizationURL"),ParamName,&ind);
    ERR_CHK(rc);
    len=_ansc_strlen(pString);
    if (len > BUFF_SIZE) {
        return FALSE;
    }
    if((!ind) && (rc == EOK))
    {    
        /* input string size check to avoid truncated data on database  */
        if((valid_url(pString)) && (strlen(pString) < (sizeof(pMyObject->CloudPersonalizationURL))))
        {	
		   if (syscfg_set_commit(NULL, "CloudPersonalizationURL", pString) != 0)
		  {
	        AnscTraceWarning(("syscfg_set failed\n"));
	      } 
		 else
		 {
			rc = STRCPY_S_NOCLOBBER(pMyObject->CloudPersonalizationURL,sizeof(pMyObject->CloudPersonalizationURL), pString);
                        if(rc != EOK)
                        {
                          ERR_CHK(rc);
                          return FALSE;
                        }
			CcspTraceWarning(("CloudPersonalizationURL  URL is changed, new URL is %s ...\n",pMyObject->CloudPersonalizationURL));
	     }
         

	  	 return TRUE;
        } 
        else
       {
           return FALSE;
       }

 }
	/*Changes for 5878 end*/
   enum pString_val type;
   rc = strcmp_s( "X_RDKCENTRAL-COM_UI_ACCESS",strlen("X_RDKCENTRAL-COM_UI_ACCESS"),ParamName, &ind);
   ERR_CHK(rc);
   if((!ind) && (rc == EOK))

   {

     /* helper function to make code more readable by removing multiple if else */
     if(get_deviceinfo_from_name(pString, &type))  
     {    
           if (type == UIACCESS)
           {
		 CcspTraceInfo(("Local UI Access : RDKB_LOCAL_UI_ACCESS\n"));
           }
           else if (type ==  UISUCCESS)
           {
		CcspTraceInfo(("Local UI Access : RDKB_LOCAL_UI_SUCCESS\n"));
                CcspTraceInfo(("WebUi admin login success\n"));
           }
           else if(type == UIFAILED)
           {
         	CcspTraceInfo(("Local UI Access : RDKB_LOCAL_UI_FAILED\n"));
                CcspTraceInfo(("WebUi admin login failed\n"));
           }
	   else if(type == REBOOTDEVICE)
           {
                CcspTraceInfo(("RDKB_REBOOT : RebootDevice triggered from GUI\n"));
                OnboardLog("RDKB_REBOOT : RebootDevice triggered from GUI\n");
		 
                #if defined (_ARRIS_XB6_PRODUCT_REQ_) //ARRISXB6-7328, ARRISXB6-7332
                ARRIS_RESET_REASON("RDKB_REBOOT : RebootDevice triggered from GUI\n");
                #endif

		char buffer[8] = {0};
		syscfg_get( NULL, "restore_reboot", buffer, sizeof(buffer));

		rc = strcmp_s( "true",strlen("true"),buffer,&ind);
                ERR_CHK(rc);
                if((rc == EOK) && (ind))
		{
			if (syscfg_set(NULL, "X_RDKCENTRAL-COM_LastRebootReason", "gui-reboot") != 0)
			{
				AnscTraceWarning(("RDKB_REBOOT : RebootDevice syscfg_set failed GUI\n"));
			}
	        
			if (syscfg_set_commit(NULL, "X_RDKCENTRAL-COM_LastRebootCounter", "1") != 0)
			{
				AnscTraceWarning(("syscfg_set failed\n"));
			}
	        }
		else
		{
			CcspTraceInfo(("RDKB_REBOOT : RebootDevice to restore configuration\n"));
		}
	 }
         else if(type == FACTORYRESET)
         {
               CcspTraceInfo(("RDKB_REBOOT : Reboot Device triggered through Factory reset from GUI\n"));
               OnboardLog("RDKB_REBOOT : Reboot Device triggered through Factory reset from GUI\n");
			 
               if ((syscfg_set(NULL, "X_RDKCENTRAL-COM_LastRebootReason", "Reboot Factory reset UI") != 0) ||
                   (syscfg_set_commit(NULL, "X_RDKCENTRAL-COM_LastRebootCounter", "1") != 0))
               {
                   AnscTraceWarning(("syscfg_set failed\n"));
               }

               #if defined (_ARRIS_XB6_PRODUCT_REQ_) //ARRISXB6-7328, ARRISXB6-7332
               ARRIS_RESET_REASON("RDKB_REBOOT : Reboot Device triggered through Factory reset from GUI\n");
               #endif
         }
	 else if(type == CAPTIVEPORTALFAILURE) {

	 	CcspTraceInfo(("Local UI Access : Out of Captive Poratl, Captive Portal is disabled\n"));
	 }
      }
      else
      {
	        CcspTraceInfo(("Local UI Access : Unsupported value\n"));
      }
         return TRUE;
   }

   /* Changes for EMS */
    rc = strcmp_s("X_COMCAST-COM_EMS_ServerURL", strlen("X_COMCAST-COM_EMS_ServerURL"), ParamName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
   
    {
       
        char wrapped_inputparam[256]={0};
	    ret=isValidInput(pString,wrapped_inputparam, AnscSizeOfString(pString), sizeof( wrapped_inputparam ));
	    if(ANSC_STATUS_SUCCESS != ret)
	    return FALSE;
	if(sizeof( wrapped_inputparam ) < sizeof(pMyObject->EMS_ServerURL) )
       {
	    if (syscfg_set_commit(NULL, "ems_server_url", wrapped_inputparam) != 0) {
             AnscTraceWarning(("syscfg_set failed\n"));
          } else {
		 char ems_url[150];	
		 rc = sprintf_s(ems_url,sizeof(ems_url),"/etc/whitelist.sh %s",wrapped_inputparam);
                if(rc < EOK)
               {
                   ERR_CHK(rc);
                   return FALSE;
               }

                v_secure_system("/etc/whitelist.sh %s",wrapped_inputparam);
		rc = STRCPY_S_NOCLOBBER(pMyObject->EMS_ServerURL,sizeof(pMyObject->EMS_ServerURL), wrapped_inputparam);
		if(rc != EOK)
         {
              ERR_CHK(rc);
               return FALSE;
         }
             }

	return TRUE;
      }
      
      else
     {
         return FALSE;
     }

    }
    
    rc = strcmp_s("X_RDKCENTRAL-COM_LastRebootReason", strlen("X_RDKCENTRAL-COM_LastRebootReason"), ParamName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
        {
		OnboardLog("Device reboot due to reason %s\n", pString);
                if (syscfg_set(NULL, "X_RDKCENTRAL-COM_LastRebootReason", pString) != 0)
	            {
			        AnscTraceWarning(("syscfg_set failed for Reason and counter \n"));
			    }
		        if (syscfg_set_commit(NULL, "X_RDKCENTRAL-COM_LastRebootCounter", "1") != 0)
	            {
                      AnscTraceWarning(("syscfg_set failed\n"));
                }
	    return TRUE;
				
        }
    
    rc = strcmp_s("X_COMCAST-COM_EMS_MobileNumber", strlen("X_COMCAST-COM_EMS_MobileNumber"), ParamName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
        /* save update to backup */
        rc = STRCPY_S_NOCLOBBER(pMyObject->EMS_MobileNo,sizeof(pMyObject->EMS_MobileNo), pString);
	if(rc != EOK)
         {
              ERR_CHK(rc);
              return FALSE;
         }
        return TRUE;
		
    }

    rc = strcmp_s("RouterName", strlen("RouterName"), ParamName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))
    {
      if (syscfg_set_commit(NULL, "router_name", pString) != 0)
      {
	AnscTraceWarning(("syscfg_set failed for RouterName\n"));
      } 
      else
      {
	CcspTraceWarning(("RouterName is changed, new RouterName: %s ...\n", pString));
      }
      return TRUE;

    }
	
/* Changes end here */
	
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        DeviceInfo_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
DeviceInfo_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DeviceInfo_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
DeviceInfo_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        DeviceInfo_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
DeviceInfo_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    ULONG pulSize = 0;
    /*CID:78739 Out-of-bounds access - updted the ProvisioningCode with 256 in the declaration*/
    CosaDmlDiGetProvisioningCode(NULL,(char *)pMyObject->ProvisioningCode, &pulSize);
    CosaDmlDiGetProvisioningCodeSource(NULL, &pMyObject->ProvisioningCodeSource);
    
    return 0;
}

/***********************************************************************

 APIs for Object:

    DeviceInfo.X_RDKCENTRAL-COM_WIFI_TELEMETRY.{i}.

    *  WiFi_Telemetry_SetParamIntValue
    *  WiFi_Telemetry_SetParamStringValue
    *  WiFi_Telemetry_GetParamIntValue
    *  WiFi_Telemetry_GetParamStringValue
***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFi_Telemetry_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
WiFi_Telemetry_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    errno_t                         rc        = -1;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "LogInterval") == 0)
    {
        char str[12];
        int retPsmGet = CCSP_SUCCESS;

        /* Updating the LogInterval  in PSM database  */
        rc = sprintf_s(str, sizeof(str),"%d",iValue);
        if(rc < EOK)
        {
          ERR_CHK(rc);
          return FALSE;
        }
        retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_WHIX.LogInterval", ccsp_string, str);
        if (retPsmGet != CCSP_SUCCESS) {
        CcspTraceError(("Set failed for LogInterval Support \n"));
        return FALSE;
        }
        CcspTraceInfo(("Successfully set  LogInterval in PSM \n"));
        /* save update to backup */
        pMyObject->WiFi_Telemetry.LogInterval = iValue;
        return TRUE;
    }

    if (strcmp(ParamName, "ChUtilityLogInterval") == 0)
    {
        char str[12];
        int retPsmGet = CCSP_SUCCESS;

        /* Updating the ChUtilityLogInterval  in PSM database  */
        sprintf(str,"%d",iValue);
        retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_WHIX.ChUtilityLogInterval", ccsp_string, str);
        if (retPsmGet != CCSP_SUCCESS) {
        CcspTraceError(("Set failed for LogInterval Support \n"));
        return FALSE;
        }
        CcspTraceInfo(("Successfully set  LogInterval in PSM \n"));
        /* save update to backup */
        pMyObject->WiFi_Telemetry.ChUtilityLogInterval = iValue;
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFi_Telemetry_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
WiFi_Telemetry_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    errno_t   rc = -1;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "NormalizedRssiList") == 0)
    {
        int retPsmGet = CCSP_SUCCESS;

        /* Updating the NormalizedRssiList in PSM database  */
        retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_WHIX.NormalizedRssiList", ccsp_string, pString);
        if (retPsmGet != CCSP_SUCCESS) {
        CcspTraceError(("Set failed for NormalizedRssiList Support \n"));
        return FALSE;
        }
        CcspTraceInfo(("Successfully set  NormalizedRssiList in PSM \n"));
        /* save update to backup */
        rc = STRCPY_S_NOCLOBBER(pMyObject->WiFi_Telemetry.NormalizedRssiList, sizeof(pMyObject->WiFi_Telemetry.NormalizedRssiList), pString);
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return FALSE;
        }
        return TRUE;
    }

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "CliStatList") == 0)
    {
        int retPsmGet = CCSP_SUCCESS;

        /* Updating the CliStatList in PSM database  */
        retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_WHIX.CliStatList", ccsp_string, pString);
        if (retPsmGet != CCSP_SUCCESS) {
        CcspTraceError(("Set failed for CliStatList Support \n"));
        return FALSE;
        }
        CcspTraceInfo(("Successfully set  CliStatList in PSM \n"));
        /* save update to backup */
        rc = STRCPY_S_NOCLOBBER( pMyObject->WiFi_Telemetry.CliStatList, sizeof(pMyObject->WiFi_Telemetry.CliStatList), pString );
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return FALSE;
        }
        return TRUE;
    }

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "TxRxRateList") == 0)
    {
        int retPsmGet = CCSP_SUCCESS;

        /* Updating the TxRxRateList  in PSM database  */
        retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_WHIX.TxRxRateList", ccsp_string, pString);
        if (retPsmGet != CCSP_SUCCESS) {
        CcspTraceError(("Set failed for TxRxRateList Support \n"));
        return FALSE;
        }
        CcspTraceInfo(("Successfully set  TxRxRateList in PSM \n"));
        /* save update to backup */
        rc = STRCPY_S_NOCLOBBER( pMyObject->WiFi_Telemetry.TxRxRateList, sizeof(pMyObject->WiFi_Telemetry.TxRxRateList), pString );
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return FALSE;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "SNRList") == 0)
    {
        int retPsmSet = CCSP_SUCCESS;
    
        retPsmSet = PSM_Set_Record_Value2( bus_handle, g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_WIFI_TELEMETRY.SNRList", ccsp_string, pString );

        if (retPsmSet != CCSP_SUCCESS )
        {
            CcspTraceInfo(("Failed to set SNRList in PSM"));
            return FALSE;
        }
        
        CcspTraceInfo(("Successfully set SNRList in PSM \n"));
    
        rc = STRCPY_S_NOCLOBBER( pMyObject->WiFi_Telemetry.SNRList, sizeof(pMyObject->WiFi_Telemetry.SNRList), pString );
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFi_Telemetry_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
WiFi_Telemetry_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "LogInterval") == 0)
    {
        *pInt =  pMyObject->WiFi_Telemetry.LogInterval;
        return TRUE;
    }

    if (strcmp(ParamName, "ChUtilityLogInterval") == 0)
    {
        *pInt =  pMyObject->WiFi_Telemetry.ChUtilityLogInterval;
        return TRUE;
    }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        WiFi_Telemetry_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/


ULONG
WiFi_Telemetry_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    errno_t   rc = -1;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "NormalizedRssiList") == 0)
    {
        /* collect value */
        rc = strcpy_s( pValue, *pulSize, pMyObject->WiFi_Telemetry.NormalizedRssiList);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }

    if (strcmp(ParamName, "CliStatList") == 0)
    {
        /* collect value */
        rc = strcpy_s( pValue, *pulSize, pMyObject->WiFi_Telemetry.CliStatList);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }


    if (strcmp(ParamName, "TxRxRateList") == 0)
    {
        /* collect value */
        rc = strcpy_s( pValue, *pulSize, pMyObject->WiFi_Telemetry.TxRxRateList);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }

    if (strcmp(ParamName, "SNRList") == 0)
    {
        /* collect value */
        rc = strcpy_s( pValue, *pulSize, pMyObject->WiFi_Telemetry.SNRList);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }


    return -1;
}

/***********************************************************************

 APIs for Object:

   .X_RDKCENTRAL-COM_RFC.Feature.ImageHealthChecker

    * IHC_GetParamStringValue
    * IHC_SetParamStringValue
***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype

        BOOL
        IHC_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

    description:

        This function is called to retrieve BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                        pValue
                The buffer of returned BOOL value;

    return:     pValue if succeeded.

**********************************************************************/

 BOOL 
 IHC_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
 {
    UNREFERENCED_PARAMETER(hInsContext);
    errno_t rc = -1;
    int ind    = -1;
    rc = strcmp_s("OperatingMode", strlen("OperatingMode"), ParamName, &ind);
    ERR_CHK(rc);
    if((rc == EOK) && (!ind))
    {
        char buf[64]={0};
        syscfg_get( NULL, "IHC_Mode", buf, sizeof(buf));
        if( buf[0] != '\0' )
        {
             rc = strcpy_s(pValue, *pUlSize, buf);
             ERR_CHK(rc);
             return TRUE;
        }
    }
    return FALSE;
 }


BOOL
IHC_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    errno_t rc = -1;
    int ind    = -1;
    rc = strcmp_s("OperatingMode", strlen("OperatingMode"), ParamName, &ind);
    ERR_CHK(rc);
    if((rc == EOK) && (!ind))
    {
        if(strcmp_s("Self Heal", strlen("Self Heal"), strValue , &ind) == EOK && !ind)
        {
            CcspTraceWarning(("%s: %s Mode will be implemented in next phase. So please use Monitor or Disabled \n", __FUNCTION__, strValue));
            return FALSE;
        }
        else if( (strcmp_s("Disabled", strlen("Disabled"), strValue , &ind) == EOK && !ind) || (strcmp_s("Monitor", strlen("Monitor"), strValue , &ind) == EOK && !ind ) )
        {
            if (syscfg_set_commit(NULL, "IHC_Mode", strValue) != 0)
            {
                    CcspTraceWarning(("%s: syscfg_set failed for %s\n", __FUNCTION__, ParamName));
                    return FALSE;
            }
            return TRUE;
        }
        CcspTraceWarning(("%s: invalid Parameter value for IHC: %s \n", __FUNCTION__, strValue));
    }
    else
    {
        CcspTraceWarning(("%s: invalid Parameter for IHC: %s \n", __FUNCTION__, ParamName));
    }
    return FALSE;
}

/***********************************************************************

 APIs for Object:

    DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UniqueTelemetryId.

    *  UniqueTelemetryId_GetParamBoolValue
    *  UniqueTelemetryId_GetParamStringValue
    *  UniqueTelemetryId_GetParamIntValue
    *  UniqueTelemetryId_SetParamBoolValue
    *  UniqueTelemetryId_SetParamStringValue
    *  UniqueTelemetryId_SetParamIntValue
***********************************************************************/

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        UniqueTelemetryId_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
UniqueTelemetryId_GetParamBoolValue
	(
		ANSC_HANDLE 				hInsContext,
		char*						ParamName,
		BOOL*						pBool
	)
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;


    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        *pBool = pMyObject->UniqueTelemetryId.Enable;
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        UniqueTelemetryId_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/

ULONG
UniqueTelemetryId_GetParamStringValue
	(
		ANSC_HANDLE 				hInsContext,
		char*						ParamName,
		char*						pValue,
		ULONG*						pUlSize
	)
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "TagString") == 0)
    {
        return  update_pValue(pValue,pUlSize, pMyObject->UniqueTelemetryId.TagString);
	    /* CID: 64836 Structurally dead code*/
    }

    return -1;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        UniqueTelemetryId_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
UniqueTelemetryId_GetParamIntValue
	(
		ANSC_HANDLE 				hInsContext,
		char*						ParamName,
		int*						pInt
	)
{
    UNREFERENCED_PARAMETER(hInsContext);
	PCOSA_DATAMODEL_DEVICEINFO		pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
	
	/* check the parameter name and return the corresponding value */
	if (strcmp(ParamName, "TimingInterval") == 0)
	{
		*pInt =  pMyObject->UniqueTelemetryId.TimingInterval;
		return TRUE;
	}

	return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        UniqueTelemetryId_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
UniqueTelemetryId_SetParamBoolValue
	(
		ANSC_HANDLE 				hInsContext,
		char*						ParamName,
		BOOL						bValue
	)
{
    PCOSA_DATAMODEL_DEVICEINFO		pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    char buf[8] = {0};
    errno_t rc  = -1;

    if (IsBoolSame(hInsContext, ParamName, bValue, UniqueTelemetryId_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        rc = strcpy_s(buf, sizeof(buf), (bValue ? "true" : "false"));
        if(rc != EOK) {
           ERR_CHK(rc);
           return FALSE;
        }

        if (syscfg_set_commit(NULL, "unique_telemetry_enable", buf) != 0)
        {
            AnscTraceWarning(("syscfg_set failed\n"));
        } 
        else
        {
            pMyObject->UniqueTelemetryId.Enable = bValue;
        }

	UniqueTelemetryCronJob(pMyObject->UniqueTelemetryId.Enable, pMyObject->UniqueTelemetryId.TimingInterval, pMyObject->UniqueTelemetryId.TagString);

        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        UniqueTelemetryId_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
UniqueTelemetryId_SetParamStringValue
	(
		ANSC_HANDLE 				hInsContext,
		char*						ParamName,
		char*						strValue
	)
{
    PCOSA_DATAMODEL_DEVICEINFO		pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    errno_t   rc  =  -1;

    if (IsStringSame(hInsContext, ParamName, strValue, UniqueTelemetryId_GetParamStringValue))
        return TRUE;

    if (strcmp(ParamName, "TagString") == 0)
    {

        if (syscfg_set_commit(NULL, "unique_telemetry_tag", strValue) != 0)
        {
            AnscTraceWarning(("syscfg_set failed\n"));
        }
        else
        {
            rc = STRCPY_S_NOCLOBBER(pMyObject->UniqueTelemetryId.TagString, sizeof(pMyObject->UniqueTelemetryId.TagString),strValue);
            if(rc != EOK)
            {
               ERR_CHK(rc);
               return FALSE;
            }
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        UniqueTelemetryId_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
UniqueTelemetryId_SetParamIntValue
	(
		ANSC_HANDLE 				hInsContext,
		char*						ParamName,
		int 						value
	)
{
    PCOSA_DATAMODEL_DEVICEINFO		pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    char buf[16]={0};
    errno_t rc = -1;

    if (IsIntSame(hInsContext, ParamName, value, UniqueTelemetryId_GetParamIntValue))
        return TRUE;

    if (strcmp(ParamName, "TimingInterval") == 0)
    {
        rc = sprintf_s(buf, sizeof(buf), "%d", value);
        if(rc < EOK)
        {
          ERR_CHK(rc);
          return FALSE;
        }

        if (syscfg_set_commit(NULL, "unique_telemetry_interval", buf) != 0)
        {
            AnscTraceWarning(("syscfg_set failed\n"));
        } 
        else
        {
            pMyObject->UniqueTelemetryId.TimingInterval = value;
        }

	UniqueTelemetryCronJob(pMyObject->UniqueTelemetryId.Enable, pMyObject->UniqueTelemetryId.TimingInterval, pMyObject->UniqueTelemetryId.TagString);

        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ManageableNotification_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
ManageableNotification_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8];

    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        /*CID: 67293 Array compared against 0*/
        if(!syscfg_get( NULL, "ManageableNotificationEnabled", buf, sizeof(buf)))
        {
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
           return TRUE;
        } else
          return FALSE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ManageableNotification_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
ManageableNotification_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, ManageableNotification_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        syscfg_set_commit(NULL, "ManageableNotificationEnabled", (bValue == TRUE) ? "true" : "false");
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TR069support_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
//TR069 support is removed for XB8 (RDKB-32781)
#ifndef _XB8_PRODUCT_REQ_
BOOL
TR069support_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "Enable") == 0)
    {
        *pBool = TRUE;
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TR069support_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TR069support_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, TR069support_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        if( bValue == FALSE)
        {
            AnscTraceWarning(("Disabling Tr069 from RFC \n"));
            v_secure_system("/usr/ccsp/pam/launch_tr69.sh disable &");
        }
        else
        {
            AnscTraceWarning(("Enabling Tr069 from RFC \n"));
            v_secure_system("/usr/ccsp/pam/launch_tr69.sh enable &");
        }
		
        return TRUE;
    }

    return FALSE;
}
#endif

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        newNTP_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
newNTP_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8];

    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        /*CID:54377 Array compared against 0*/
        if(!syscfg_get( NULL, "new_ntp_enabled", buf, sizeof(buf))) 
        {
            if (strcmp(buf, "false") == 0)
                *pBool = FALSE;
            else
                *pBool = TRUE;
        }
        return TRUE;
    }

    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        newNTP_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
newNTP_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, newNTP_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if( ANSC_STATUS_SUCCESS != CosaDmlSetnewNTPEnable(bValue))
	    return FALSE;
        return TRUE;
    }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        MACsecRequired_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
#ifdef _MACSEC_SUPPORT_
BOOL
MACsecRequired_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if ( RETURN_ERR == platform_hal_GetMACsecEnable( ETHWAN_DEF_INTF_NUM, pBool )) {
            return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        MACsecRequired_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MACsecRequired_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, MACsecRequired_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if ( RETURN_ERR == platform_hal_SetMACsecEnable( ETHWAN_DEF_INTF_NUM, bValue )) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}
#endif


/***********************************************************************

 APIs for Object:

    DeviceInfo.VendorConfigFile.{i}.

    *  VendorConfigFile_GetEntryCount
    *  VendorConfigFile_GetEntry
    *  VendorConfigFile_GetParamBoolValue
    *  VendorConfigFile_GetParamIntValue
    *  VendorConfigFile_GetParamUlongValue
    *  VendorConfigFile_GetParamStringValue
    *  VendorConfigFile_SetParamBoolValue
    *  VendorConfigFile_SetParamIntValue
    *  VendorConfigFile_SetParamUlongValue
    *  VendorConfigFile_SetParamStringValue
    *  VendorConfigFile_Validate
    *  VendorConfigFile_Commit
    *  VendorConfigFile_Rollback

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        VendorConfigFile_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
VendorConfigFile_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        VendorConfigFile_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
VendorConfigFile_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    *pInsNumber  = nIndex + 1; 
    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VendorConfigFile_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VendorConfigFile_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VendorConfigFile_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(puLong);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        VendorConfigFile_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
VendorConfigFile_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    
    strcpy(pValue, "");
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Alias") == 0)
    {
        /* collect value */
        return 0;
    }

    if (strcmp(ParamName, "Name") == 0)
    {
        /* collect value */
        return 0;
    }

    if (strcmp(ParamName, "Version") == 0)
    {
        /* collect value */
        return 0;
    }

    if (strcmp(ParamName, "Date") == 0)
    {
        /* collect value */
        return 0;
    }

    if (strcmp(ParamName, "Description") == 0)
    {
        /* collect value */
        return 0;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VendorConfigFile_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(bValue);
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The updated integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VendorConfigFile_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(iValue);
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VendorConfigFile_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(uValue);
    /* check the parameter name and set the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
VendorConfigFile_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pString);
    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "Alias") == 0)
    {
        /* save update to backup */
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        VendorConfigFile_Validate
            (
                ANSC_HANDLE                 hInsContext,
                char*                       pReturnParamName,
                ULONG*                      puLength
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation. 

                ULONG*                      puLength
                The output length of the param name. 

    return:     TRUE if there's no validation.

**********************************************************************/
BOOL
VendorConfigFile_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pReturnParamName);
    UNREFERENCED_PARAMETER(puLength);
    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        VendorConfigFile_Commit
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
VendorConfigFile_Commit
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        VendorConfigFile_Rollback
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to roll back the update whenever there's a 
        validation found.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
VendorConfigFile_Rollback
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/***********************************************************************

 APIs for Object:

    DeviceInfo.SupportedDataModel.{i}.

    *  SupportedDataModel_GetEntryCount
    *  SupportedDataModel_GetEntry
    *  SupportedDataModel_GetParamBoolValue
    *  SupportedDataModel_GetParamIntValue
    *  SupportedDataModel_GetParamUlongValue
    *  SupportedDataModel_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SupportedDataModel_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
SupportedDataModel_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        SupportedDataModel_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
SupportedDataModel_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    *pInsNumber  = nIndex + 1; 
    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SupportedDataModel_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SupportedDataModel_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SupportedDataModel_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SupportedDataModel_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SupportedDataModel_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SupportedDataModel_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(puLong);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        SupportedDataModel_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
SupportedDataModel_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    
    strcpy(pValue, "");
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "URL") == 0)
    {
        /* collect value */
        return 0;
    }

    if (strcmp(ParamName, "URN") == 0)
    {
        /* collect value */
        return 0;
    }

    if (strcmp(ParamName, "Features") == 0)
    {
        /* collect value */
        return 0;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    DeviceInfo.MemoryStatus.

    *  MemoryStatus_GetParamBoolValue
    *  MemoryStatus_GetParamIntValue
    *  MemoryStatus_GetParamUlongValue
    *  MemoryStatus_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MemoryStatus_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MemoryStatus_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MemoryStatus_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MemoryStatus_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MemoryStatus_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MemoryStatus_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Total") == 0)
    {
         /* collect value */
        *puLong = COSADmlGetMemoryStatus(ParamName);
        return TRUE;
    }

    if (strcmp(ParamName, "Free") == 0)
    {
        /* collect value */
        *puLong = COSADmlGetMemoryStatus(ParamName);
        return TRUE;
    }


    if (strcmp(ParamName, "Used") == 0)
    {
        /* collect value */
        *puLong = COSADmlGetMemoryStatus(ParamName);
        return TRUE;
    }


    if (strcmp(ParamName, "X_RDKCENTRAL-COM_FreeMemThreshold") == 0)
    {
        char buf[12];
        syscfg_get (NULL, "MinMemoryThreshold_Value", buf, sizeof(buf));
        *puLong = atoi(buf);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        MemoryStatus_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
MemoryStatus_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}



/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        MemoryStatus_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MemoryStatus_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "X_RDKCENTRAL-COM_FreeMemThreshold") == 0)
    {
        syscfg_set_u_commit (NULL, "MinMemoryThreshold_Value", uValue);
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}
/***********************************************************************

 APIs for Object:

    DeviceInfo.X_RDKCENTRAL-COM.

    *  X_RDKCENTRAL-COM_GetParamBoolValue
    *  X_RDKCENTRAL-COM_GetParamIntValue
    *  X_RDKCENTRAL-COM_GetParamUlongValue
    *  X_RDKCENTRAL-COM_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_RDKCENTRAL-COM_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_RDKCENTRAL_COM_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "DhcpServDetectEnable") == 0)
    {
        *pBool = pMyObject->bDhcpServDetectEnable;
        return TRUE;
    }

    if (strcmp(ParamName, "MultipleGW") == 0)
    {
        *pBool = pMyObject->bMultipleGW;
        return TRUE;
    }

    if (strcmp(ParamName, "InternetStatus") == 0)
    {
        *pBool = CosaDmlGetInternetStatus();
        return TRUE;
    }
	
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_RDKCENTRAL-COM_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_RDKCENTRAL_COM_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    /* check the parameter name and return the corresponding value */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_RDKCENTRAL-COM_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_RDKCENTRAL_COM_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(puLong);
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        X_RDKCENTRAL-COM_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
X_RDKCENTRAL_COM_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    /* check the parameter name and return the corresponding value */
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        X_RDKCENTRAL_COM_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
X_RDKCENTRAL_COM_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "DhcpServDetectEnable") == 0)
    {
		/* Same value should not be process at anytime */
		if( bValue == pMyObject->bDhcpServDetectEnable )
		{
			return TRUE;
		}

		CosaDmlDiSetAndProcessDhcpServDetectionFlag( pMyObject, &bValue, &pMyObject->bDhcpServDetectEnable );

        return TRUE;
    }

    if (strcmp(ParamName, "MultipleGW") == 0)
    {
		pMyObject->bMultipleGW = bValue;

        return TRUE;
    }

    return FALSE;
}

/***********************************************************************

 APIs for Object:

    DeviceInfo.ProcessStatus.

    *  ProcessStatus_GetParamBoolValue
    *  ProcessStatus_GetParamIntValue
    *  ProcessStatus_GetParamUlongValue
    *  ProcessStatus_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ProcessStatus_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ProcessStatus_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ProcessStatus_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ProcessStatus_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        ProcessStatus_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ProcessStatus_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "CPUUsage") == 0)
    {
        /* collect value */
         *puLong = COSADmlGetCpuUsage();
         
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        ProcessStatus_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
ProcessStatus_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pValue);
    UNREFERENCED_PARAMETER(pUlSize);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/***********************************************************************

 APIs for Object:

    DeviceInfo.ProcessStatus.Process.{i}.

    *  Process_GetEntryCount
    *  Process_GetEntry
    *  Process_IsUpdated
    *  Process_Synchronize
    *  Process_GetParamBoolValue
    *  Process_GetParamIntValue
    *  Process_GetParamUlongValue
    *  Process_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Process_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
Process_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_PROCSTATUS      pProc     = (PCOSA_DATAMODEL_PROCSTATUS)g_pCosaBEManager->hProcStatus;

    return pProc->ProcessNumberOfEntries;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ANSC_HANDLE
        Process_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
Process_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_PROCSTATUS      pProc     = (PCOSA_DATAMODEL_PROCSTATUS)g_pCosaBEManager->hProcStatus;

    *pInsNumber  = nIndex + 1;             

    if (nIndex < pProc->ProcessNumberOfEntries)
    {
        return pProc->pProcTable+nIndex;
    }

    return NULL; /* return the handle */
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Process_IsUpdated
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is checking whether the table is updated or not.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     TRUE or FALSE.

**********************************************************************/
static ULONG last_tick;
#define REFRESH_INTERVAL 120
#define TIME_NO_NEGATIVE(x) ((long)(x) < 0 ? 0 : (x))
BOOL
Process_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (!last_tick) 
    {
        last_tick = AnscGetTickInSeconds();
        return TRUE;
    }
    
    if (last_tick >= TIME_NO_NEGATIVE(AnscGetTickInSeconds() - REFRESH_INTERVAL))
    {
        return FALSE;
    }
    else 
    {
        last_tick = AnscGetTickInSeconds();
        return TRUE;
    }
    
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Process_Synchronize
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to synchronize the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/
ULONG
Process_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_PROCSTATUS      pProc     = (PCOSA_DATAMODEL_PROCSTATUS)g_pCosaBEManager->hProcStatus;

    if (pProc->pProcTable != NULL)
    {
         AnscFreeMemory(pProc->pProcTable);
         pProc->pProcTable = NULL;
         pProc->ProcessNumberOfEntries = 0;
    }

    COSADmlGetProcessInfo(pProc);
    
    return 0;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Process_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Process_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Process_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Process_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Process_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Process_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_PROCESS_ENTRY        p_proc = (PCOSA_PROCESS_ENTRY)hInsContext;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "PID") == 0)
    {
        /* collect value */
        *puLong = p_proc->Pid;
        return TRUE;
    }

    if (strcmp(ParamName, "Size") == 0)
    {
        /* collect value */
        *puLong = p_proc->Size;        
        return TRUE;
    }

    if (strcmp(ParamName, "Priority") == 0)
    {
        /* collect value */
        *puLong = p_proc->Priority;                
        return TRUE;
    }

    if (strcmp(ParamName, "CPUTime") == 0)
    {
        /* collect value */
        *puLong = p_proc->CPUTime;                
        return TRUE;
    }

    if (strcmp(ParamName, "State") == 0)
    {
        /* collect value */
        *puLong = p_proc->State;                
        return TRUE;
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Process_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Process_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_PROCESS_ENTRY        p_proc = (PCOSA_PROCESS_ENTRY)hInsContext;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Command") == 0)
    {
        /* collect value */
        return  update_pValue(pValue,pUlSize, p_proc->Command);
    }


    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}


/***********************************************************************

 APIs for Object:

    DeviceInfo.TemperatureStatus.TemperatureSensor.{i}.

    *  TemperatureSensor_GetEntryCount
    *  TemperatureSensor_GetEntry
    *  TemperatureSensor_IsUpdated
    *  TemperatureSensor_Synchronize
    *  TemperatureSensor_GetParamBoolValue
    *  TemperatureSensor_GetParamIntValue
    *  TemperatureSensor_GetParamUlongValue
    *  TemperatureSensor_GetParamStringValue
    *  TemperatureSensor_SetParamBoolValue
    *  TemperatureSensor_SetParamIntValue
    *  TemperatureSensor_SetParamUlongValue
    *  TemperatureSensor_SetParamStringValue

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        TemperatureSensor_GetEntryCount
            (
                ANSC_HANDLE                 hInsContext
            );

    description:

        This function is called to retrieve the count of the table.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

    return:     The count of the table

**********************************************************************/
ULONG
TemperatureSensor_GetEntryCount
    (
        ANSC_HANDLE                 hInsContext
    )
{
    PCOSA_DATAMODEL_TEMPERATURE_STATUS      pTempStatus     = (PCOSA_DATAMODEL_TEMPERATURE_STATUS)g_pCosaBEManager->hTemperatureStatus;

    return pTempStatus->TemperatureSensorNumberOfEntries;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ANSC_HANDLE
        TemperatureSensor_GetEntry
            (
                ANSC_HANDLE                 hInsContext,
                ULONG                       nIndex,
                ULONG*                      pInsNumber
            );

    description:

        This function is called to retrieve the entry specified by the index.

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                ULONG                       nIndex,
                The index of this entry;

                ULONG*                      pInsNumber
                The output instance number;

    return:     The handle to identify the entry

**********************************************************************/
ANSC_HANDLE
TemperatureSensor_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    )
{
    PCOSA_DATAMODEL_TEMPERATURE_STATUS      pTempStatus     = (PCOSA_DATAMODEL_TEMPERATURE_STATUS)g_pCosaBEManager->hTemperatureStatus;

    *pInsNumber  = nIndex + 1;

    if (nIndex < pTempStatus->TemperatureSensorNumberOfEntries)
    {
        return pTempStatus->TemperatureSensorEntry+nIndex;
    }

    return NULL; /* return the handle */
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TemperatureSensor_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TemperatureSensor_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          PTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        *pBool = PTempSensorEntry->Enable;
        return TRUE;
    }

    if (strcmp(ParamName, "Reset") == 0)
    {
        *pBool = FALSE;
        return TRUE;
    }

    if (strcmp(ParamName, "X_LGI-COM_CutOutTempExceeded") == 0)
    {
        *pBool = PTempSensorEntry->CutOutTempExceeded;
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TemperatureSensor_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TemperatureSensor_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          PTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;
    PCOSA_DATAMODEL_TEMPERATURE_STATUS      pTempStatus = (PCOSA_DATAMODEL_TEMPERATURE_STATUS)g_pCosaBEManager->hTemperatureStatus;
    int                                     index = PTempSensorEntry->InstanceNumber;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Value") == 0)
    {
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        *pInt = PTempSensorEntry->Value;
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return TRUE;
    }

    if (strcmp(ParamName, "MinValue") == 0)
    {
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        *pInt = PTempSensorEntry->MinValue;
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return TRUE;
    }

    if (strcmp(ParamName, "MaxValue") == 0)
    {
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        *pInt = PTempSensorEntry->MaxValue;
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return TRUE;
    }

    if (strcmp(ParamName, "LowAlarmValue") == 0)
    {
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        *pInt = PTempSensorEntry->LowAlarmValue;
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return TRUE;
    }

    if (strcmp(ParamName, "HighAlarmValue") == 0)
    {
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        *pInt = PTempSensorEntry->HighAlarmValue;
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TemperatureSensor_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TemperatureSensor_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          PTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Status") == 0)
    {
        *puLong = PTempSensorEntry->Status;
        return TRUE;
    }

    if (strcmp(ParamName, "PollingInterval") == 0)
    {
        *puLong = PTempSensorEntry->PollingInterval;
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        TemperatureSensor_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
TemperatureSensor_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          PTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;
    PCOSA_DATAMODEL_TEMPERATURE_STATUS      pTempStatus = (PCOSA_DATAMODEL_TEMPERATURE_STATUS)g_pCosaBEManager->hTemperatureStatus;
    int                                     index = PTempSensorEntry->InstanceNumber;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "Alias") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->Alias))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->Alias);
            return 1;
        }
        AnscCopyString(pValue,  PTempSensorEntry->Alias);
        return 0;
    }

    if (strcmp(ParamName, "ResetTime") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->ResetTime))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->ResetTime);
            return 1;
        }
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        AnscCopyString(pValue,  PTempSensorEntry->ResetTime);
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return 0;
    }

    if (strcmp(ParamName, "Name") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->Name))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->Name);
            return 1;
        }
        AnscCopyString(pValue,  PTempSensorEntry->Name);
        return 0;
    }

    if (strcmp(ParamName, "LastUpdate") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->LastUpdate))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->LastUpdate);
            return 1;
        }
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        AnscCopyString(pValue,  PTempSensorEntry->LastUpdate);
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return 0;
    }

    if (strcmp(ParamName, "MinTime") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->MinTime))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->MinTime);
            return 1;
        }
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        AnscCopyString(pValue,  PTempSensorEntry->MinTime);
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return 0;
    }

    if (strcmp(ParamName, "MaxTime") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->MaxTime))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->MaxTime);
            return 1;
        }
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        AnscCopyString(pValue,  PTempSensorEntry->MaxTime);
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return 0;
    }

    if (strcmp(ParamName, "LowAlarmTime") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->LowAlarmTime))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->LowAlarmTime);
            return 1;
        }
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        AnscCopyString(pValue,  PTempSensorEntry->LowAlarmTime);
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return 0;
    }

    if (strcmp(ParamName, "HighAlarmTime") == 0)
    {
        if (*pUlSize < AnscSizeOfString(PTempSensorEntry->HighAlarmTime))
        {
            *pUlSize = AnscSizeOfString(PTempSensorEntry->HighAlarmTime);
            return 1;
        }
        pthread_mutex_lock(&(pTempStatus->rwLock[index-1]));
        AnscCopyString(pValue,  PTempSensorEntry->HighAlarmTime);
        pthread_mutex_unlock(&(pTempStatus->rwLock[index-1]));
        return 0;
    }

    return -1;
}
/**********************************************************************


 APIs for Object:

    Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.SHORTS.Enable

    *  SHORTS_SetParamBoolValue // Set args required for SHORTS
    *  SHORTS_GetParamBoolValue // Get args set for SHORTS

***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SHORTS_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool,
            );

    description:

        This function is called to retrieve boolean  parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;
                BOOL*                       pBool,
                The buffer of returned boolean value;

     return:     TRUE if succeeded.


**********************************************************************/
BOOL
SHORTS_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "Enable") == 0)
    {
        char buf[8] = {0};
        if (syscfg_get(NULL, "ShortsEnabled", buf, sizeof(buf)) == 0)
        {
            if (strncmp(buf, "true", sizeof(buf)) == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        }
        else
        {
            CcspTraceError(("%s syscfg_get failed  for SHORTSEnable\n", __FUNCTION__));
            *pBool = FALSE;
        }
        return TRUE;
    }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SHORTS_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SHORTS_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, SHORTS_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set(NULL, "ShortsEnabled", (bValue == TRUE) ? "true" : "false") != 0)
        {
            CcspTraceError(("[%s] syscfg_set failed for SHORTS\n", __FUNCTION__));
            return FALSE;
        }
        if (syscfg_commit() != 0)
        {
            AnscTraceWarning(("syscfg_commit failed for SHORTS param update\n"));
            return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TemperatureSensor_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
TemperatureSensor_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          pTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (pTempSensorEntry->Enable != bValue)
        {
            CosaTemperatureSensorReset(bValue, pTempSensorEntry);
            pTempSensorEntry->Enable = bValue;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "Reset") == 0)
    {
        CosaTemperatureSensorReset(pTempSensorEntry->Enable, pTempSensorEntry);
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TemperatureSensor_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                        bValue
                The updated int value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TemperatureSensor_SetParamIntValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         bValue
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          pTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "LowAlarmValue") == 0)
    {
        if (CosaTemperatureSensorSetLowAlarm(bValue, pTempSensorEntry) != ANSC_STATUS_SUCCESS)
        {
            return FALSE;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "HighAlarmValue") == 0)
    {
        if (CosaTemperatureSensorSetHighAlarm(bValue, pTempSensorEntry) != ANSC_STATUS_SUCCESS)
        {
            return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TemperatureSensor_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TemperatureSensor_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          pTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "PollingInterval") == 0)
    {
        if (pTempSensorEntry->PollingInterval != uValue)
        {
            CosaTemperatureSensorSetPollingTime(uValue, pTempSensorEntry);
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TemperatureSensor_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
TemperatureSensor_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_TEMPERATURE_SENSOR_ENTRY          PTempSensorEntry = (PCOSA_TEMPERATURE_SENSOR_ENTRY)hInsContext;

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "Alias") == 0)
    {
        AnscCopyString(PTempSensorEntry->Alias, pString);
        return TRUE;
    }

    return FALSE;
}


/* HTTPS config download can be enabled/disabled for bci routers */
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        HTTPSConfigDownload_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
HTTPSConfigDownload_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enabled") == 0)
    {
        char *strValue = NULL;
        int retPsmGet = CCSP_SUCCESS;

        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_RFC.Feature.HTTPSConfigDownload.Enabled", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS)
        {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
        {
            *pBool = FALSE;
        }

        return TRUE;
    }

    return FALSE;
}


/* HTTPS config download can be enabled/disabled for bci routers */
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        HTTPSConfigDownload_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
HTTPSConfigDownload_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enabled") == 0)
    {
        char *strValue = NULL;
        int retPsmGet = CCSP_SUCCESS;
        BOOL getVal = 0;

        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_RFC.Feature.HTTPSConfigDownload.Enabled", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS)
        {
            getVal = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }

        if(getVal != bValue)
        {
            retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "dmsb.device.deviceinfo.X_RDKCENTRAL-COM_RFC.Feature.HTTPSConfigDownload.Enabled", ccsp_string, bValue ? "1" : "0");
            if (retPsmGet != CCSP_SUCCESS)
            {
                CcspTraceError(("Set failed for HTTPSConfigDownloadEnabled \n"));
                return FALSE;
            }
        }
        else
        {
            CcspTraceInfo(("HTTPSConfigDownloadEnabled is already %d \n",getVal));
        }

        return TRUE;
    }

    return FALSE;
}


/**********************************************************************

 APIs for Object:

    DeviceInfo.NetworkProperties.

    *  NetworkProperties_GetParamBoolValue
    *  NetworkProperties_GetParamIntValue
    *  NetworkProperties_GetParamUlongValue
    *  NetworkProperties_GetParamStringValue

***********************************************************************/
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        NetworkProperties_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
NetworkProperties_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        NetworkProperties_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pInt
            );

    description:

        This function is called to retrieve integer parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                        pInt
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
NetworkProperties_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pInt);
    /* check the parameter name and return the corresponding value */

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        NetworkProperties_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
NetworkProperties_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "MaxTCPWindowSize") == 0)
    {
        /* collect value */
        *puLong = COSADmlGetMaxWindowSize();
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        NetworkProperties_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
NetworkProperties_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "TCPImplementation") == 0)
    {
	/* collect value */
        CosaDmlGetTCPImplementation(NULL,pValue,pulSize);

        return 0;
    }
	
    return -1;
}

/* PresenceDetect can be enabled/disabled to detect connected clients presence */
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        PresenceDetect_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
PresenceDetect_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    char buf[8];
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        /*CID: 67242 Array compared against 0*/
        if(!syscfg_get( NULL, "PresenceDetectEnabled", buf, sizeof(buf)))
        {
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        } else 
           return FALSE;

        return TRUE;
    }

    return FALSE;
}

/* PresenceDetect can be enabled/disabled to detect connected clients presence */
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        PresenceDetect_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
PresenceDetect_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, PresenceDetect_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "PresenceDetectEnabled", (bValue == TRUE) ? "true" : "false") != 0)
        {
             AnscTraceWarning(("syscfg_set failed for Presence feature param update\n"));
             return FALSE;
        }
        CosaDmlPresenceEnable(bValue);
        return TRUE;
    }

    return FALSE;
}

/* LostandFoundInternet can be blocked/allowed */
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        LostandFoundInternet_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
LostandFoundInternet_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8];

    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        /* CID: 119814 Array compared against 0*/
       if(!syscfg_get( NULL, "BlockLostandFoundInternet", buf, sizeof(buf))) 
       {
        if (strcmp(buf, "true") == 0)
            *pBool = TRUE;
        else
            *pBool = FALSE;
       } else 
          return FALSE;

        return TRUE;
    }

    return FALSE;
}

/* BlockLostandFoundInternet can be enabled/disabled to allow internet to lnf clients */
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        LostandFoundInternet_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
LostandFoundInternet_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "BlockLostandFoundInternet", (bValue == TRUE) ? "true" : "false") != 0)
        {
             AnscTraceWarning(("syscfg_set failed for block lnf internet update\n"));
             return FALSE;
        }
	    v_secure_system("sysevent set firewall-restart");
        return TRUE;
    }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
                BOOL
                Control_GetParamBoolValue
                    (
                        ANSC_HANDLE         hInsContext,
                        char*               ParamName,
                        BOOL*               pBool
                    );

    description:

        This function is called to get bool parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Control_GetParamBoolValue
    (
        ANSC_HANDLE         hInsContext,
        char*               ParamName,
        BOOL*               pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
#if defined(FEATURE_HOSTAP_AUTHENTICATOR)
    char *psmStrValue = NULL;

    *pBool = FALSE; //Initialize to FALSE

    if (strcmp(ParamName, "DisableNativeHostapd") == 0)
    {
        CcspTraceInfo(("[%s] Get DisableNativeHostapd Value \n",__FUNCTION__));

        if (PSM_Get_Record_Value2(bus_handle, g_Subsystem,
            "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.DisableNativeHostapd",
            NULL, &psmStrValue) == CCSP_SUCCESS)
        {
            *pBool = _ansc_atoi(psmStrValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(psmStrValue);
        }
        return TRUE;
    }
#endif //FEATURE_HOSTAP_AUTHENTICATOR

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Control_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Control_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(bValue);
    CcspTraceWarning(("g_currentParamFullName = %s\n", g_currentParamFullName));
#if defined(FEATURE_HOSTAP_AUTHENTICATOR)
    if (strcmp(ParamName, "DisableNativeHostapd") == 0)
    {
        CcspTraceInfo(("[%s] set DisableNativeHostapd Value [%s]\n",__FUNCTION__, (bValue ? "TRUE" : "FALSE")));
        return CosaDmlSetNativeHostapdState(bValue);
    }
#endif //FEATURE_HOSTAP_AUTHENTICATOR
    return FALSE;
}

/**
 *  RFC Feature for CrashUpload S3signing url
*/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        CrashUpload_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pulSize
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue
                The buffer of returned string value;

                ULONG*                      pulSize
                The buffer of returned string size;

    return:     TRUE if succeeded.

**********************************************************************/
ULONG
    CrashUpload_GetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                       pValue,
 ULONG*                      pulSize
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pulSize);
    errno_t   rc   = -1;

    if (strcmp(ParamName, "S3SigningUrl") == 0)
    {
        /* collect value */
        char buf[64] = {'\0'};
        memset(buf, 0 ,sizeof(buf));
        if(!syscfg_get( NULL, "CrashUpload_S3SigningUrl", buf, sizeof(buf)))
        {
            rc = strcpy_s(pValue, *pulSize, buf);
            if(rc != EOK)
            {
                ERR_CHK(rc);
                return -1;
            }
            return 0;
        }
        return -1;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        CrashUpload_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated String value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
    CrashUpload_SetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                       pString
 )
{
    if (IsStringSame(hInsContext, ParamName, pString, CrashUpload_GetParamStringValue))
        return TRUE;

    if (strcmp(ParamName, "S3SigningUrl") == 0)
    {
        if (syscfg_set_commit(NULL, "CrashUpload_S3SigningUrl", pString) != 0)
        {
            CcspTraceError(("syscfg_set failed\n"));

        }
        else
        {
            return TRUE;
        }
    }

/* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

	BOOL
        Feature_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        pint
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                       pint
                The buffer of returned integer value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
Feature_GetParamIntValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*	                    pint
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
   /* check the parameter name and return the corresponding value */
    //CcspTraceInfo(("Feature_GetParamIntValue: RDKLowQueueRebootThreshold\n"));

    if (strcmp(ParamName, "RDKLowQueueRebootThreshold") == 0)
    {
         /* collect value */
         char buf[10];
         syscfg_get( NULL, "low_queue_reboot_threshold", buf, sizeof(buf));
         if( buf [0] != '\0' )
         {
             *pint= ( atoi(buf) );
             return TRUE;
         }
    }
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Feature_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
Feature_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
#if defined(MOCA_HOME_ISOLATION)
    if (strcmp(ParamName, "HomeNetworkIsolation") == 0)
    {
        /* collect value */
    char *strValue = NULL;
    int retPsmGet = CCSP_SUCCESS;

    retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.HomeNetworkIsolation", NULL, &strValue);
    if (retPsmGet == CCSP_SUCCESS) {
        *pBool = _ansc_atoi(strValue);
        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
    }
    else
        *pBool = FALSE;
     return TRUE;   
    }
#endif

#if defined(_COSA_FOR_BCI_)
    if (strcmp(ParamName, "OneToOneNAT") == 0)
    {
         char value[8];
         /*CID: 66608 Array compared against 0*/
         if(!syscfg_get(NULL,"one_to_one_nat",value, sizeof(value)))
         {
             if (strcmp(value, "true") == 0)
                 *pBool = TRUE;
             else
                 *pBool = FALSE;
         } else
           return FALSE;

         return TRUE;
    }

    if (strcmp(ParamName, "EnableMultiProfileXDNS") == 0)
    {
        char buf[5] = {0};
         /*CID: 66608 Array compared against 0*/
        if(!syscfg_get( NULL, "MultiProfileXDNS", buf, sizeof(buf)))
        {
                if (strcmp(buf,"1") == 0)
                {
                        *pBool = TRUE;
                        return TRUE;
                }
        } else 
             return FALSE; 

        *pBool = FALSE;

        return TRUE;
    }
#endif

#if defined(_XB6_PRODUCT_REQ_)   
   if (strcmp(ParamName, "BLERadio") == 0)
    {
        BLE_Status_e status;
        if(!ble_GetStatus(&status))
        {
           if(status == BLE_ENABLE)
            {
                *pBool = TRUE;
            } else {
                *pBool = FALSE;
            }
            return TRUE;
        }
        else {
            CcspTraceWarning(("%s: ble_GetStatus failed\n", __func__));
	    }
    }
#endif
    if (strcmp(ParamName, "Xupnp") == 0)
    {
	 char value[8];
         /*CID: 66608 Array compared against 0*/
         if(!syscfg_get(NULL, "start_upnp_service", value, sizeof(value)))
         {
             if (strcmp(value, "true") == 0)
                 *pBool = TRUE;
             else
                 *pBool = FALSE;
            return TRUE;
         } else {
	    return FALSE;
         }
    }

#ifdef _BRIDGE_UTILS_BIN_ 
    if (strcmp(ParamName, "BridgeUtilsEnable") == 0)
    {
           char value[8] = {0};
           syscfg_get(NULL, "bridge_util_enable", value, sizeof(value));
           if( value[0] != '\0' )
           {
               if (strcmp(value, "true") == 0)
                   *pBool = TRUE;
               else
                   *pBool = FALSE;
           }
           return TRUE;
    }
#endif
#if (defined _COSA_INTEL_XB3_ARM_)
    if (strcmp(ParamName, "CMRouteIsolationEnable") == 0)
    {
	   char value[8] = {0};
           syscfg_get(NULL, "CMRouteIsolation_Enable", value, sizeof(value));
           if( value[0] != '\0' )
           {
               if (strcmp(value, "true") == 0)
                   *pBool = TRUE;
               else
                   *pBool = FALSE;
           }
           return TRUE;
    }
#endif
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        MEMSWAP_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MEMSWAP_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
       /* Collect Value */
       char *strValue = NULL;
       int retPsmGet = CCSP_SUCCESS;


        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MEMSWAP.Enable", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS) {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
            *pBool = FALSE;

         return TRUE;
    }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        DNSSTRICTORDER_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DNSSTRICTORDER_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8];
        if(!syscfg_get(NULL,"DNSStrictOrder",value, sizeof(value))) {
        /* CID: 119695 Array compared against 0*/
        if (strcmp(value, "true") == 0)
            *pBool = TRUE;
        else
           *pBool = FALSE;
        } else
           return FALSE;

        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ActiveMeasurements_RFC_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ActiveMeasurements_RFC_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
       /* Collect Value */
       char *strValue = NULL;
       int retPsmGet = CCSP_SUCCESS;


        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WifiClient.ActiveMeasurements.Enable", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS) {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
            *pBool = TRUE;

         return TRUE;
    }
    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ActiveMeasurements_RFC_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ActiveMeasurements_RFC_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, ActiveMeasurements_RFC_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
       int retPsmGet = CCSP_SUCCESS;

       retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WifiClient.ActiveMeasurements.Enable", ccsp_string, bValue ? "1" : "0");
       if (retPsmGet != CCSP_SUCCESS) {
           CcspTraceError(("Set failed for Active Measurement RFC enable \n"));
           return FALSE;
       }
       CcspTraceInfo(("Successfully set Active Measurement RFC enable \n"));
       return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WPA3_Personal_Transition_RFC_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value for
		WPA3 Transition RFC;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WPA3_Personal_Transition_RFC_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (!ParamName || !pBool)
        return FALSE;

    if (strcmp(ParamName, "Enable") == 0)
    {
       /* Collect Value */
       char *strValue = NULL;
       int retPsmGet = CCSP_SUCCESS;


        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WPA3_Personal_Transition.Enable", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS) {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
            *pBool = FALSE;

         return TRUE;
    }
    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WPA3_Personal_Transition_RFC_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value for
		WPA3 Transition RFC;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WPA3_Personal_Transition_RFC_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, WPA3_Personal_Transition_RFC_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        char *secMode;
        int retPsmGet = CCSP_SUCCESS;
        errno_t rc  = -1;
        int ret = -1;
        int size = 0;
        componentStruct_t ** ppComponents = NULL;
        char* faultParam = NULL;
        char dst_pathname_cr[64];

        CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;

        retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WPA3_Personal_Transition.Enable", ccsp_string, bValue ? "1" : "0");
        if (retPsmGet != CCSP_SUCCESS) {
            CcspTraceError(("Set failed for WPA3 Transition RFC Enable \n"));
            return FALSE;
        }
        CcspTraceInfo(("Successfully set WPA3 Transition RFC Enable \n"));

        secMode = bValue ? "WPA3-Personal-Transition" : "WPA2-Personal";

       /* calling the DML code to set the security modes accordingly */
        rc = sprintf_s(dst_pathname_cr, sizeof(dst_pathname_cr),"%s%s", g_Subsystem, CCSP_DBUS_INTERFACE_CR);
        if(rc < EOK)
        {
          ERR_CHK(rc);
          return FALSE;
        }
        ret = CcspBaseIf_discComponentSupportingNamespace(bus_handle,
                dst_pathname_cr,
                "Device.WiFi.X_CISCO_COM_ResetRadios",
                g_Subsystem,        /* prefix */
                &ppComponents,
                &size);

        if ( ret == CCSP_SUCCESS && size == 1)
        {
            parameterValStruct_t    val[] = 
            {
                { "Device.WiFi.AccessPoint.1.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.2.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.3.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.4.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.7.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.8.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.15.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.16.Security.ModeEnabled", secMode, ccsp_string},
                { "Device.WiFi.AccessPoint.1.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.AccessPoint.2.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.AccessPoint.3.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.AccessPoint.4.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.AccessPoint.7.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.AccessPoint.8.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.AccessPoint.15.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.AccessPoint.16.Security.X_RDKCENTRAL-COM_TransitionDisable", "false", ccsp_boolean},
                { "Device.WiFi.Radio.1.X_CISCO_COM_ApplySetting", "true", ccsp_boolean},
                { "Device.WiFi.Radio.2.X_CISCO_COM_ApplySetting", "true", ccsp_boolean},
            };

            ret = CcspBaseIf_setParameterValues
                (
                 bus_handle,
                 ppComponents[0]->componentName,
                 ppComponents[0]->dbusPath,
                 0, 0x0,
                 val,
                 18,
                 TRUE,
                 &faultParam
                );

            if (ret != CCSP_SUCCESS && faultParam)
            {
                AnscTraceError(("Error:Failed to SetValue for param '%s'\n", faultParam));
                bus_info->freefunc(faultParam);
            }
            free_componentStruct_t(bus_handle, size, ppComponents);
        }
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ErrorsReceived_RFC_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value for
		ErrorsReceived RFC;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ErrorsReceived_RFC_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (!ParamName || !pBool)
        return FALSE;

    if (strcmp(ParamName, "Enable") == 0)
    {
       /* Collect Value */
       char *strValue = NULL;
       int retPsmGet = CCSP_SUCCESS;

        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ErrorsReceived.Enable", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS) {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
            *pBool = FALSE;

         return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ErrorsReceived_RFC_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value for
		ErrorsReceived RFC;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
ErrorsReceived_RFC_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, ErrorsReceived_RFC_GetParamBoolValue))
        return TRUE;
    
    if (strcmp(ParamName, "Enable") == 0)
    {
       int retPsmGet = CCSP_SUCCESS;

       retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.ErrorsReceived.Enable", ccsp_string, bValue ? "1" : "0");
       if (retPsmGet != CCSP_SUCCESS) {
           CcspTraceError(("Set failed for ErrorsReceived RFC enable \n"));
           return FALSE;
       }
       CcspTraceInfo(("Successfully set ErrorsReceived RFC enable \n"));
       return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Feature_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                        bValue
                The updated int value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Feature_SetParamIntValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */
    CcspTraceInfo(("Feature_SetParamIntValue: RDKLowQueueRebootThreshold"));

    if (strcmp(ParamName, "RDKLowQueueRebootThreshold") == 0)
    {
        char buf[8]={0};
        snprintf(buf, sizeof(buf), "%d", bValue);

        if (syscfg_set_commit(NULL, "low_queue_reboot_threshold", buf) != 0)
        {
               CcspTraceInfo(("syscfg_set low_queue_reboot_threshold failed\n"));
        }
	return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        DFS_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
DFS_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
       /* Collect Value */
       char *strValue = NULL;
       int retPsmGet = CCSP_SUCCESS;

       retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DFS.Enable", NULL, &strValue);
       if (retPsmGet == CCSP_SUCCESS) {
           *pBool = _ansc_atoi(strValue);
           ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
       }
       else
           *pBool = FALSE;

       return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EasyConnect_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
EasyConnect_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
       /* Collect Value */
       char *strValue = NULL;
       int retPsmGet = CCSP_SUCCESS;


        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EasyConnect.Enable", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS) {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
            *pBool = TRUE;

         return TRUE;
    }

    if (strcmp(ParamName, "EnableAPISecurity") == 0)
    {
       /* Collect Value */
       char *strValue = NULL;
       int retPsmGet = CCSP_SUCCESS;


        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EasyConnect.EnableAPISecurity", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS) {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
            *pBool = FALSE;

         return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        DFS_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
DFS_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        CcspTraceInfo(("[%s] set  DFS Value [%s]\n",__FUNCTION__, (bValue ? "TRUE" : "FALSE")));
        return CosaDmlSetDFS(bValue);
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EasyConnect_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
EasyConnect_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, EasyConnect_GetParamBoolValue))
        return TRUE;


    if (strcmp(ParamName, "Enable") == 0)
    {
       int retPsmGet = CCSP_SUCCESS;

       retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EasyConnect.Enable", ccsp_string, bValue ? "1" : "0");
       if (retPsmGet != CCSP_SUCCESS) {
           CcspTraceError(("Set failed for EasyConnect support \n"));
           return FALSE;
       }
       CcspTraceInfo(("Successfully set EasyConnect support \n"));
       return TRUE;
    }

    if (strcmp(ParamName, "EnableAPISecurity") == 0)
    {
       int retPsmGet = CCSP_SUCCESS;

       retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.EasyConnect.EnableAPISecurity", ccsp_string, bValue ? "1" : "0");
       if (retPsmGet != CCSP_SUCCESS) {
           CcspTraceError(("Set failed for EasyConnect APISecurity support \n"));
           return FALSE;
       }
       CcspTraceInfo(("Successfully set EasyConnect APISecurity support \n"));
       return TRUE;
    }

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Feature_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Feature_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, Feature_GetParamBoolValue))
        return TRUE;

#if defined(MOCA_HOME_ISOLATION)
    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "HomeNetworkIsolation") == 0)
    {
    int retPsmGet = CCSP_SUCCESS;

   /* char *strValue = NULL;
    BOOL getVal = 0;
    retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.HomeNetworkIsolation", NULL, &strValue);
    if (retPsmGet == CCSP_SUCCESS) {
        getVal = _ansc_atoi(strValue);
        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
    }

   / if(getVal != bValue)*/
	{
             retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.HomeNetworkIsolation", ccsp_string, bValue ? "1" : "0");
             if (retPsmGet != CCSP_SUCCESS) {
             CcspTraceError(("Set failed for HomeNetworkIsolation \n"));
             return FALSE;
             }
                if(bValue)
                {
                    CcspTraceInfo(("Apply changes for HomeNetworkIsolation \n"));
#if defined(_COSA_BCM_MIPS_)
                    v_secure_system("sh /usr/ccsp/lan_handler.sh home_lan_isolation_enable &");
                    sleep(2);
#else
                    v_secure_system("sysevent set multinet-restart 1");
#endif
                    v_secure_system("sh /usr/ccsp/moca/MoCA_isolation.sh &");
                    
                }
                else
                {

                    CcspTraceInfo(("reverting changes for HomeNetworkIsolation \n"));
#if defined(_COSA_BCM_MIPS_)
                    v_secure_system("sh /usr/ccsp/lan_handler.sh home_lan_isolation_disable &");
                    v_secure_system("rm /tmp/MoCABridge_up");
#else
                    v_secure_system("sysevent set multinet-down 9");
                    v_secure_system("rm /tmp/MoCABridge_up");
                    v_secure_system("sysevent set multinet-restart 1");
#endif
                    v_secure_system("killall mcsender; killall MRD; killall smcroute; mcproxy_v4 -r -f /tmp/mcproxy_v4.conf &");

                    v_secure_system("sh /usr/ccsp/moca/MoCA_isolation.sh &");
                }
	}
	//else
	//	CcspTraceInfo(("HomeNetworkIsolation is already %d \n",getVal));
	
    	return TRUE;
    }
#endif

#if defined(_COSA_FOR_BCI_)
    if (strcmp(ParamName, "OneToOneNAT") == 0)
    {
        BOOL bNatEnable = FALSE;
        bNatEnable  = g_GetParamValueBool(g_pDslhDmlAgent, "Device.NAT.X_Comcast_com_EnableNATMapping");
        if ( bValue != bNatEnable )
        {
            g_SetParamValueBool("Device.NAT.X_Comcast_com_EnableNATMapping", bValue);
        }

        syscfg_set_commit(NULL, "one_to_one_nat", (bValue == TRUE) ? "true" : "false");
        return TRUE;
    }

    if (strcmp(ParamName, "EnableMultiProfileXDNS") == 0)
    {
        char buf[5];
        syscfg_get( NULL, "X_RDKCENTRAL-COM_XDNS", buf, sizeof(buf));
        if( buf != NULL && !strcmp(buf,"1") )
        {
                if(!setMultiProfileXdnsConfig(bValue))
                        return FALSE;

                if (syscfg_set_commit(NULL, "MultiProfileXDNS", bValue ? "1" : "0") != 0)
                {
                        AnscTraceWarning(("[XDNS] syscfg_set MultiProfileXDNS failed!\n"));
                }
        }
        else
        {
                CcspTraceError(("XDNS Feature is not Enabled. so,EnableMultiProfileXDNS set operation to %d failed \n",bValue));
                return FALSE;
        }

        return TRUE;

    }
#endif
#if defined (_XB6_PRODUCT_REQ_)
    if (strcmp(ParamName, "BLERadio") == 0)
    {
       BLE_Status_e status;
       if(bValue == TRUE)
       {
          status = BLE_ENABLE;
          CcspTraceInfo(("***BLE_ENABLED***\n"));
       }
       else
       {
          status = BLE_DISABLE;
          CcspTraceInfo(("***BLE_DISABLED***\n"));
       }
       if( !ble_Enable(status))
       {
          if (syscfg_set_commit(NULL, "BLEEnabledOnBoot", (status == BLE_ENABLE) ? "true" : "false") != 0) 
          {
             AnscTraceWarning(("syscfg_set BLEEnabledOnBoot failed\n"));
          }
          return TRUE;
       }
       else {
            CcspTraceWarning(("%s: ble_Enable failed\n", __func__));
       }
    }
#endif

    if (strcmp(ParamName, "Xupnp") == 0)
    {
       if ( bValue == TRUE)
       {
           if (syscfg_set_commit(NULL, "start_upnp_service", "true") != 0)
           {
               AnscTraceWarning(("syscfg_set start_upnp_service:true failed\n"));
           }
       }
       else
       {
           if (syscfg_set_commit(NULL, "start_upnp_service", "false") != 0)
           {
               AnscTraceWarning(("syscfg_set start_upnp_service:false failed\n"));
           }

		   if(access("/lib/rdk/start-upnp-service", F_OK) == 0)
		   {
			   v_secure_system("/lib/rdk/start-upnp-service stop;killall xcal-device;killall xdiscovery");
		   }
		   else
		   {
			   v_secure_system("systemctl stop xcal-device;systemctl stop xupnp");
		   }

		   if(access("/nvram/output.json", F_OK) == 0)
		   {
			   v_secure_system("rm /nvram/output.json");
		   }
           v_secure_system("ifconfig brlan0:0 down");
       }
       return TRUE;
    }
#ifdef _BRIDGE_UTILS_BIN_ 
    if (strcmp(ParamName, "BridgeUtilsEnable") == 0)
    {
        if (syscfg_set_commit(NULL, "bridge_util_enable", (bValue == TRUE) ? "true" : "false") != 0)
        {
            AnscTraceWarning(("syscfg_set bridge_util_enable failed\n"));
            return FALSE;
        }
        return TRUE;
    }
#endif
#if (defined _COSA_INTEL_XB3_ARM_)
    if (strcmp(ParamName, "CMRouteIsolationEnable") == 0)
    {
       CcspTraceInfo(("CM Route Isolation Enable:%s\n",bValue?"true":"false"));
       
       if( CMRt_Isltn_Enable(bValue) == TRUE )
       {
          if (syscfg_set_commit(NULL, "CMRouteIsolation_Enable", bValue ? "true" : "false") != 0)
          {
             AnscTraceWarning(("syscfg_set CMRouteIsolationEnable failed\n"));
	     return FALSE;
          }
          return TRUE;
       }
       else {
            CcspTraceWarning(("%s: CMRouteIsolationEnable failed\n", __func__));
	    return FALSE;
       }
    }
#endif
    return FALSE;
}

#if (defined _COSA_INTEL_XB3_ARM_)
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL CMRt_Isltn_Enable
            (
                BOOL                        bValue
            );

    description:

        This function is called to configure/deconfigure CM Wan0
	Route isolation.

    argument:   BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL CMRt_Isltn_Enable(BOOL status)
{
    /* if Enable is true and wan0 is not configured earlier,
     * handle it here */	
    if (status == TRUE) 
    {
       if (access( "/tmp/wan0_configured", F_OK ) != 0)
       {
	  ipv6_addr_info_t * p_v6addr = NULL;
          int  v6addr_num = 0, i;
	  int rc = -1; 
	  rc = CosaUtilGetIpv6AddrInfo("wan0", &p_v6addr, &v6addr_num);     
	  if (0 == rc)
	  {	  
	     for(i = 0; i < v6addr_num; i++ )
	     {
                if ((p_v6addr[i].scope == IPV6_ADDR_SCOPE_GLOBAL)
		     && (strncmp(p_v6addr[i].v6addr, "fd", 2) != 0) 
		     && (strncmp(p_v6addr[i].v6addr, "fc", 2) != 0))
	        {
		    break;
	        }
	     }
	     if(strlen(p_v6addr[i].v6addr))
             {		     
	        _write_sysctl_file("/proc/sys/net/ipv6/conf/wan0/accept_ra_table", 1024);
	        v_secure_system( "ip -6 rule add from %s lookup cmwan && "
                                 "ip -6 rule add from all oif wan0 table cmwan && "
                                 "ip -6 route del default dev wan0 && "
                                 "touch /tmp/wan0_configured ",p_v6addr[i].v6addr);
	     }
	     else
	     {
		CcspTraceError(("WAN0 IPv6 null,Unable to configure route table cmwan\n"));
		if(p_v6addr)
                   free(p_v6addr);
		return FALSE;
	     }

	     if(p_v6addr)
                free(p_v6addr);

	  }
	  else
          {
                /* ??? we can't be empty, either the box management is ipv4 only or
                rare conditon of wan0 is empty. can't taken any action for now
                */
                CcspTraceError(("WAN0 IPv6 empty,Unable to configure route table cmwan\n"));
                return FALSE;
	   }
	}
	else
	  CcspTraceInfo(("CM Route Isolation already configured"));

        /* check to ensure, we are properly configured, partial configuration 
	 * may lead to indefinite behaviour,revert to original state
	 * and return false if not properly configured*/  
	if (access( "/tmp/wan0_configured", F_OK ) != 0)
	{
           CcspTraceError(("CM Route Isolation Enable failed for wan0,Reset tables and rules\n"));		
	   _write_sysctl_file("/proc/sys/net/ipv6/conf/wan0/accept_ra_table", 254);
           v_secure_system("ip -6 rule del lookup cmwan && "
			   "ip -6 rule del from all oif wan0 lookup cmwan && "
                           "ip -6 route del default dev wan0 table cmwan && "
                           "sysevent set wan-stop && "
                           "sysevent set wan-start ");
	   return FALSE;
	 }
	 else
	  return TRUE;
     }
     else /*disable case*/
     {
        if (access( "/tmp/wan0_configured", F_OK ) == 0)
    	{
           /* After deleting and adding back to the original table, default
	      route for wan0 come as second entry, do restart wan at end to bring 
	      back to original state
	   */		
           CcspTraceInfo(("Reset Route table and routing rules for wan0\n"));
	   _write_sysctl_file("/proc/sys/net/ipv6/conf/wan0/accept_ra_table", 254);
	   v_secure_system("ip -6 rule del from all oif wan0 lookup cmwan && "
                           "ip -6 rule del lookup cmwan && "
			   "ip -6 route del default dev wan0 table cmwan && "
                           "rm /tmp/wan0_configured && "
			   "sysevent set wan-stop && "
			   "sysevent set wan-start ");
    	}
	return TRUE;
     }
}
#endif

#define BS_SOURCE_WEBPA_STR "webpa"
#define BS_SOURCE_RFC_STR "rfc"

char * getRequestorString()
{
   switch(g_currentWriteEntity)
   {
      case DSLH_MPA_ACCESS_CONTROL_WEBPA:
      case DSLH_MPA_ACCESS_CONTROL_XPC:
         return BS_SOURCE_WEBPA_STR;

      case DSLH_MPA_ACCESS_CONTROL_CLI:
      case DSLH_MPA_ACCESS_CONTROL_CLIENTTOOL:
         return BS_SOURCE_RFC_STR;

      default:
         return "unknown";
   }
}

char * getTime()
{
    time_t timer;
    static char buffer[50];
    struct tm tm_info = {0};
    time(&timer);
    localtime_r(&timer,&tm_info);
    strftime(buffer, 50, "%Y-%m-%d %H:%M:%S ", &tm_info);
    return buffer;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        MEMSWAP_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
MEMSWAP_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, MEMSWAP_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
       int retPsmGet = CCSP_SUCCESS;

       retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MEMSWAP.Enable", ccsp_string, bValue ? "1" : "0");
       if (retPsmGet != CCSP_SUCCESS) {
           CcspTraceError(("Set failed for MEMSWAP support \n"));
           return FALSE;
       }
       CcspTraceInfo(("Successfully set MEMSWAP support \n"));
       return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        DNSSTRICTORDER_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
DNSSTRICTORDER_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, DNSSTRICTORDER_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        syscfg_set_commit(NULL, "DNSStrictOrder", (bValue == TRUE) ? "true" : "false");
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ShortsDL_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ShortsDL_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char buf[8];

        /* collect value */
        /* CID: 114926 Array compared against 0*/
       if(!syscfg_get( NULL, "ShortsDL", buf, sizeof(buf)))
       {
        if (strcmp(buf, "true") == 0)
            *pBool = TRUE;
        else
            *pBool = FALSE;
       } else
          return FALSE;

        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ShortsDL_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
ShortsDL_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "ShortsDL", bValue ? "true" : "false") != 0)
        {
            CcspTraceError(("syscfg_set ShortsDLEnabled failed\n"));
        }
        else
        {
            return TRUE;
        }
    }
    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SSIDPSWDCTRL_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SSIDPSWDCTRL_GetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8];

    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "SnmpEnable") == 0)
    {
        /* collect value */
        /* CID: 54616 Array compared against 0*/
        if(!syscfg_get( NULL, "SNMPPSWDCTRLFLAG", buf, sizeof(buf)))
        {
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        }
        return TRUE;
    }
    if (strcmp(ParamName, "Tr069Enable") == 0)
    {
        /* collect value */
        syscfg_get( NULL, "TR069PSWDCTRLFLAG", buf, sizeof(buf));

        if( buf != NULL )
        {
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SSIDPSWDCTRL_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       bValue
            );

    description:

        This function is called to set Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       bValue
                The buffer with updated value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SSIDPSWDCTRL_SetParamBoolValue

(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, SSIDPSWDCTRL_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "SnmpEnable") == 0)
    {
        syscfg_set_commit(NULL, "SNMPPSWDCTRLFLAG", (bValue == TRUE) ? "true" : "false");
        return TRUE;
    }
    if (strcmp(ParamName, "Tr069Enable") == 0)
    {
        syscfg_set_commit(NULL, "TR069PSWDCTRLFLAG", (bValue == TRUE) ? "true" : "false");
        return TRUE;
    }
    return FALSE;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AllowOpenPorts_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
AllowOpenPorts_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            char buf[8];
            /* CID: 63052 Array compared against 0*/
            if(!syscfg_get( NULL, "RFCAllowOpenPorts", buf, sizeof(buf)))
            {
                if (strcmp(buf, "true") == 0)
                    *pBool = TRUE;
                else
                    *pBool = FALSE;
               return TRUE;
            } else 
               return FALSE;
        }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        AllowOpenPorts_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AllowOpenPorts_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
        if (IsBoolSame(hInsContext, ParamName, bValue, AllowOpenPorts_GetParamBoolValue))
            return TRUE;

        if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            syscfg_set_commit(NULL, "RFCAllowOpenPorts", (bValue == TRUE) ? "true" : "false");

            // Log that we are allowing open ports (or not)
            CcspTraceWarning(("RFC_AllowOpenPorts set to '%s'\n", (bValue == TRUE ? "true":"false")));

            // restart firewall
            v_secure_system("sysevent set firewall-restart");
#if defined(_PLATFORM_RASPBERRYPI_)
      if(id!=0)
       {
		   char *lxcevt = "sysevent set firewall-restart";
                   send(sock ,  lxcevt, strlen(lxcevt) , 0 );
       }
#endif
            return TRUE;
        }
    return FALSE;
}

/*********************************************************************************************

//BLUEZ RFC :: Box will run with RCP firmware, if this is enabled. Else run with NCP firmware

**********************************************************************************************

   caller: owner of this object

   prototype:

       BOOL
       BLUEZ_GetParamBoolValue
           (
               ANSC_HANDLE                 hInsContext,
               char*                       ParamName,
               BOOL*                       pBool
           );

   description:

       This function is called to retrieve Boolean parameter value;

   argument:   ANSC_HANDLE                 hInsContext,
               The instance handle;

               char*                       ParamName,
               The parameter name;

               BOOL*                       pBool
               The buffer of returned boolean value;

   return:     TRUE if succeeded.

**********************************************************************/

BOOL
BLUEZ_GetParamBoolValue
(
ANSC_HANDLE                 hInsContext,
char*                       ParamName,
BOOL*                       pBool
)
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if(access("/nvram/bluez_enable", F_OK) != -1)
        {
            *pBool = TRUE;
        }
        else
        {
            *pBool = FALSE;
        }

        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        BLUEZ_SetParamBoolValue
        (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
        );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
BLUEZ_SetParamBoolValue
(
ANSC_HANDLE                 hInsContext,
char*                       ParamName,
BOOL                        bValue
)
{
    if (IsBoolSame(hInsContext, ParamName, bValue, BLUEZ_GetParamBoolValue))
        return TRUE;
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (bValue == 1)
        {
            v_secure_system("touch /nvram/bluez_enable");
            CcspTraceInfo(("Successfully enabled Bluez \n"));
            return TRUE;
        }
        else if (bValue == 0)
        {
            v_secure_system("rm /nvram/bluez_enable");
            CcspTraceInfo(("Successfully disabled Bluez \n"));
            return TRUE;
        }
    }
    return FALSE;
}

/**********************************************************************  

//RBUS RFC :: Box will run in DBUS mode if this if disabled, RBUS mode if enabled

***********************************************************************

   caller: owner of this object

   prototype:

       BOOL
       RBUS_GetParamBoolValue
           (
               ANSC_HANDLE                 hInsContext,
               char*                       ParamName,
               BOOL*                       pBool
           );

   description:

       This function is called to retrieve Boolean parameter value;

   argument:   ANSC_HANDLE                 hInsContext,
               The instance handle;

               char*                       ParamName,
               The parameter name;

               BOOL*                       pBool
               The buffer of returned boolean value;

   return:     TRUE if succeeded.

**********************************************************************/

BOOL
RBUS_GetParamBoolValue
(
ANSC_HANDLE                 hInsContext,
char*                       ParamName,
BOOL*                       pBool
)
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if(RBUS_ENABLED == rbus_checkStatus())
            *pBool = TRUE;
        else
            *pBool = FALSE;

        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Collectd_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Collectd_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8];
        syscfg_get(NULL,"CollectdEnable",value, sizeof(value));
        if( value != NULL )
        {
             if (strcmp(value, "true") == 0)
                 *pBool = TRUE;
             else
                 *pBool = FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        COllectd_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Collectd_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "Enable") == 0)
    {
        if(syscfg_set_commit(NULL, "CollectdEnable", (bValue == TRUE) ? "true" : "false") != 0)
        {
            AnscTraceWarning(("CollectdEnable : Enabling Collectd using syscfg_set failed!!!\n"));
        }

        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        WANLinkHeal_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WANLinkHeal_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
  UNREFERENCED_PARAMETER(hInsContext);
  if (strcmp(ParamName, "Enable") == 0)
    {
      char value[8];

      if(syscfg_get(NULL,"wanlinkheal",value, sizeof(value)) == 0)
      {
	if( value[0]!='\0')
        {
             if (strcmp(value, "true") == 0)
                 *pBool = TRUE;
             else
                 *pBool = FALSE;
        }
        return TRUE;
      }
      else
      {
	CcspTraceError(("syscfg_get failed for wanlinkheal\n"));
      }
      return TRUE;
    }
  return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        WANLinkHeal_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
		BOOL                        bValue
            )


    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

		BOOL                        bValue
		The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WANLinkHeal_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
	if (syscfg_set_commit(NULL, "wanlinkheal", bValue ? "true" : "false") != 0)
	{
		CcspTraceError(("syscfg_set wanlinkheal failed\n"));
	}
	else
	{
		return TRUE;
	}
     }
 return FALSE;
}
/**********************************************************************  

    caller:     owner of this object

    prototype: 

        BOOL
        IPv6subPrefix_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
IPv6subPrefix_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            char buf[8];
            syscfg_get( NULL, "IPv6subPrefix", buf, sizeof(buf));

            if( buf != NULL )
            {
                if (strcmp(buf, "true") == 0)
                    *pBool = TRUE;
                else
                    *pBool = FALSE;
            }
            return TRUE;
        }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        IPv6subPrefix_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
IPv6subPrefix_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
        if (IsBoolSame(hInsContext, ParamName, bValue, IPv6subPrefix_GetParamBoolValue))
            return TRUE;

        if (strcmp(ParamName, "Enable") == 0)
        {
            syscfg_set_commit(NULL, "IPv6subPrefix", (bValue == TRUE) ? "true" : "false");
            return TRUE;
        }
    return FALSE;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        IPv6onLnF_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
IPv6onLnF_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            char buf[128];
	    char Inf_name[32];

            syscfg_get( NULL, "iot_brname", Inf_name, sizeof(Inf_name));
            if ( (Inf_name[0] == '\0') && (strlen(Inf_name)) == 0 )
            {
                syscfg_get( NULL, "iot_ifname", Inf_name, sizeof(Inf_name));

            }
	
	    if( Inf_name != NULL )
            {
            
            syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));

		    if( buf != NULL )
		    {
		        if (strstr(buf, Inf_name))
		            *pBool = TRUE;
		        else
		            *pBool = FALSE;
		    }
		    return TRUE;
	    }
        }

    return FALSE;
}
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        WebUI_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WebUI_GetParamUlongValue
(
    ANSC_HANDLE                 hInsContext,
    char*                       ParamName,
    ULONG*                      puLong
)
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8] = {0};

    if (strcmp(ParamName, "Enable") == 0)
    {
        syscfg_get( NULL, "WebUIEnable", buf, sizeof(buf));
        if( 0 == strlen(buf) )
        {
             *puLong = 1; // default value of WebUIEnable
        }
        else
        {
             *puLong = atol(buf);
        }
        return TRUE;
    }
    AnscTraceWarning(("%s is invalid argument!\n", ParamName));
    return FALSE;
}
    
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        WebUI_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WebUI_SetParamUlongValue
(
    ANSC_HANDLE                 hInsContext,
    char*                       ParamName,
    ULONG                       uValue
)
{

    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "Enable") == 0)
    {
	if (syscfg_set_u_commit(NULL, "WebUIEnable", uValue) != 0)
	{
		CcspTraceWarning(("syscfg_set failed to set WebUIEnable \n"));
		return FALSE;
	}
	if(uValue == 0 || uValue == 2)
	{
		CosaDmlDiSet_DisableRemoteManagement();
	}
	return TRUE;
    } 
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WebUIRemoteMgtOption_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WebUIRemoteMgtOption_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8] = {0};
	/* collect value */
        if (syscfg_get(NULL,"WebUIRemoteMgtOptionEnable",value, sizeof(value)) == 0)
        {
             if (strncmp(value, "true", sizeof(value)) == 0)
                     *pBool = TRUE;
             else
                     *pBool = FALSE;
        }
        else
        {
             CcspTraceError(("%s syscfg_get failed  for WebUIRemoteMgtOptionEnable\n",__FUNCTION__));
             *pBool = TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WebUIRemoteMgtOption_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WebUIRemoteMgtOption_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, WebUIRemoteMgtOption_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "WebUIRemoteMgtOptionEnable", (bValue == TRUE) ? "true" : "false") != 0)
        {
            CcspTraceError(("[%s] syscfg_set failed for RemoteMgtOptionEnable\n",__FUNCTION__));
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        CognitiveMotionDetection_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

#ifdef FEATURE_COGNITIVE_WIFIMOTION
BOOL
CognitiveMotionDetection_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    char buf[6] = { 0 };

    UNREFERENCED_PARAMETER(hInsContext);
    /* RDKB-38634: TR-181 implementation
     * DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CognitiveMotionDetection.Enable
     */
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_get(NULL, COGNITIVE_WIFIMOTION_CFG, buf, sizeof(buf)) != 0)
        {
            *pBool = FALSE;
            return TRUE;
        }

        if (!strncmp(buf, "true", 4))
        {
            *pBool = TRUE;
        }
        else if (!strncmp(buf, "false", 5))
        {
            *pBool = FALSE;
        }
        else
        {
            CcspTraceWarning(("syscfg_get: value of %s is invalid!\n", COGNITIVE_WIFIMOTION_CFG));
            return FALSE;
        }

        return TRUE;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        CognitiveMotionDetection_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
CognitiveMotionDetection_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    char buf[6] = { 0 };

    UNREFERENCED_PARAMETER(hInsContext);
    /* RDKB-38634: TR-181 implementation
     * DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CognitiveMotionDetection.Enable
     */
    if (strcmp(ParamName, "Enable") == 0)
    {
        char *value = (bValue == TRUE) ? "true" : "false";

        syscfg_get(NULL, COGNITIVE_WIFIMOTION_CFG, buf, sizeof(buf));

        if (!strncmp(buf, value, strlen(value)))
        {
            return TRUE;
        }

        if (syscfg_set_commit(NULL, COGNITIVE_WIFIMOTION_CFG, value) != 0)
        {
            CcspTraceWarning(("syscfg_set failed to set %s\n", COGNITIVE_WIFIMOTION_CFG));
            return FALSE;
        }

        if (bValue == TRUE)
        {
            v_secure_system("systemctl start systemd-cognitive_wifimotion.service");
        }
        else
        {
            v_secure_system("systemctl stop systemd-cognitive_wifimotion.service");
        }

        return TRUE;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}
#endif // FEATURE_COGNITIVE_WIFIMOTION

 #if defined (FEATURE_SUPPORT_INTERWORKING)
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFiInterworking_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WiFiInterworking_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
	/* Collect Value */
	char *strValue = NULL;
	int retPsmGet = CCSP_SUCCESS;

	retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi-Interworking.Enable", NULL, &strValue);
	if (retPsmGet == CCSP_SUCCESS) {
	    *pBool = _ansc_atoi(strValue);
	    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
	}
	else
	    *pBool = FALSE;
	return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFiInterworking_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WiFiInterworking_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, WiFiInterworking_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
	int retPsmGet = CCSP_SUCCESS;
	errno_t rc = -1;

        if(bValue == FALSE)
	{
		CcspTraceError(("turn off WiFiInterworkingSupport \n"));
		int ret = -1;
		int size = 0;
		componentStruct_t ** ppComponents = NULL;
		char* faultParam = NULL;
		char dst_pathname_cr[64]  =  {0};

		CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;

		rc = sprintf_s(dst_pathname_cr,  sizeof(dst_pathname_cr),"%s%s", g_Subsystem, CCSP_DBUS_INTERFACE_CR);
		if(rc < EOK)
		{
			ERR_CHK(rc);
			return FALSE;
		}

		ret = CcspBaseIf_discComponentSupportingNamespace(bus_handle,
				dst_pathname_cr,
				"Device.WiFi.X_CISCO_COM_ResetRadios",
				g_Subsystem,        /* prefix */
				&ppComponents,
				&size);

		if ( ret == CCSP_SUCCESS && size == 1)
		{
			parameterValStruct_t    val[] = { 
				{ "Device.WiFi.AccessPoint.1.X_RDKCENTRAL-COM_InterworkingServiceEnable", "false", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.2.X_RDKCENTRAL-COM_InterworkingServiceEnable", "false", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.5.X_RDKCENTRAL-COM_InterworkingServiceEnable", "false", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.6.X_RDKCENTRAL-COM_InterworkingServiceEnable", "false", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.9.X_RDKCENTRAL-COM_InterworkingServiceEnable", "false", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.10.X_RDKCENTRAL-COM_InterworkingServiceEnable", "false", ccsp_boolean},	
				{ "Device.WiFi.AccessPoint.1.X_RDKCENTRAL-COM_InterworkingApplySettings", "true", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.2.X_RDKCENTRAL-COM_InterworkingApplySettings", "true", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.5.X_RDKCENTRAL-COM_InterworkingApplySettings", "true", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.6.X_RDKCENTRAL-COM_InterworkingApplySettings", "true", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.9.X_RDKCENTRAL-COM_InterworkingApplySettings", "true", ccsp_boolean},
				{ "Device.WiFi.AccessPoint.10.X_RDKCENTRAL-COM_InterworkingApplySettings", "true", ccsp_boolean},

			};

			ret = CcspBaseIf_setParameterValues
				(
				 bus_handle,
				 ppComponents[0]->componentName,
				 ppComponents[0]->dbusPath,
				 0, 0x0,
				 val,
				 12,
				 TRUE,
				 &faultParam
				);

			if (ret != CCSP_SUCCESS && faultParam)
			{
				AnscTraceError(("Error:Failed to SetValue for param '%s'\n", faultParam));
				bus_info->freefunc(faultParam);
			}
			free_componentStruct_t(bus_handle, size, ppComponents);
		}
        }

	retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi-Interworking.Enable", ccsp_string, bValue ? "1" : "0");
	if (retPsmGet != CCSP_SUCCESS) {
	    CcspTraceError(("Set failed for WiFiInterworkingSupport \n"));
	    return FALSE;
	}
	CcspTraceInfo(("Successfully set WiFiInterworkingSupport \n"));
	return TRUE;
    }
    return FALSE;
}
#endif

#if defined (FEATURE_SUPPORT_PASSPOINT)
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFiPasspoint_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WiFiPasspoint_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
	/* Collect Value */
	char *strValue = NULL;
	int retPsmGet = CCSP_SUCCESS;

	retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi-Passpoint.Enable", NULL, &strValue);
	if (retPsmGet == CCSP_SUCCESS) {
	    *pBool = _ansc_atoi(strValue);
	    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
	}
	else
	    *pBool = FALSE;
	return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFiPasspoint_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WiFiPasspoint_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "Enable") == 0)
    {
	int retPsmGet = CCSP_SUCCESS;

	retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi-Passpoint.Enable", ccsp_string, bValue ? "1" : "0");
	if (retPsmGet != CCSP_SUCCESS) {
	    CcspTraceError(("Set failed for WiFiPasspointSupport \n"));
	    return FALSE;
    }
    CcspTraceInfo(("Successfully set WiFiPasspointSupport \n"));
#ifndef RDK_ONEWIFI
    if(bValue == FALSE){
        int ret = -1;
        int size = 0;
        componentStruct_t ** ppComponents = NULL;
        char* faultParam = NULL;
        char dst_pathname_cr[64]  =  {0};
        errno_t rc = -1;

        CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;

        rc = sprintf_s(dst_pathname_cr, sizeof(dst_pathname_cr),"%s%s", g_Subsystem, CCSP_DBUS_INTERFACE_CR);
        if(rc < EOK)
        {
          ERR_CHK(rc);
          return FALSE;
        }

        ret = CcspBaseIf_discComponentSupportingNamespace(bus_handle,
                dst_pathname_cr,
                "Device.WiFi.X_CISCO_COM_ResetRadios",
                g_Subsystem,        /* prefix */
                &ppComponents,
                &size);

        if ( ret == CCSP_SUCCESS && size == 1)
        {
            parameterValStruct_t    val[] = { { "Device.WiFi.AccessPoint.9.X_RDKCENTRAL-COM_Passpoint.Enable", "false", ccsp_boolean}, { "Device.WiFi.AccessPoint.10.X_RDKCENTRAL-COM_Passpoint.Enable", "false", ccsp_boolean} };

            ret = CcspBaseIf_setParameterValues
                (
                 bus_handle,
                 ppComponents[0]->componentName,
                 ppComponents[0]->dbusPath,
                 0, 0x0,
                 val,
                 2,
                 TRUE,
                 &faultParam
                );

            if (ret != CCSP_SUCCESS && faultParam)
            {
                AnscTraceError(("Error:Failed to SetValue for param '%s'\n", faultParam));
                bus_info->freefunc(faultParam);
            }
            free_componentStruct_t(bus_handle, size, ppComponents);
        }
    }
#else
        parameterValStruct_t pVal[1];
        char                 paramName[256] = "Device.WiFi.WiFi-Passpoint";
        char                 compName[256]  = "eRT.com.cisco.spvtg.ccsp.wifi";
        char                 dbusPath[256]  = "/com/cisco/spvtg/ccsp/wifi";
        char*                faultParam     = NULL;
        int                  ret            = 0;
        CCSP_MESSAGE_BUS_INFO *bus_info               = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
           pVal[0].parameterName  = paramName;
           pVal[0].parameterValue = bValue ? "true" : "false";
           pVal[0].type           = ccsp_boolean;
    
           ret = CcspBaseIf_setParameterValues(
                     bus_handle,
                     compName,
                     dbusPath,
                     0,
                     0,
                     pVal,
                     1,
                     TRUE,
                     &faultParam
                 );
    
           if (ret != CCSP_SUCCESS)
           {
               CcspTraceError(("%s - %d - Failed to notify WiFi component - Error [%s]\n", __FUNCTION__, __LINE__, faultParam));
               bus_info->freefunc(faultParam);
               return FALSE;
           }
#endif
    return TRUE;
    }
    return FALSE;
}
#endif


#if defined (FEATURE_OFF_CHANNEL_SCAN_5G)
BOOL
off_channel_scan_GetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* Collect Value */
        char *strValue = NULL;
        int retPsmGet = CCSP_SUCCESS;

        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDK_RFC.Feature.OffChannelScan.Enable", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS)
        {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
        {
            *pBool = FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL
off_channel_scan_SetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        int retPsmGet = CCSP_SUCCESS;

        retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDK_RFC.Feature.OffChannelScan.Enable", ccsp_string, bValue ? "1" : "0");
        if (retPsmGet != CCSP_SUCCESS)
        {
            CcspTraceError(("Set failed for 5G off channel scan RFC  \n"));
            return FALSE;
        }
        CcspTraceInfo(("Successfully set 5g off channel scan RFC \n"));

        v_secure_system("sysevent set wifi_OffChannelScanEnable %s", bValue ? "true" : "false");

        return TRUE;
    }
    return FALSE;
}
#endif // (FEATURE_OFF_CHANNEL_SCAN_5G)

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFiPsmDb_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WiFiPsmDb_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
//#if defined(FEATURE_SUPPORT_WIFIDB)
    if (strcmp(ParamName, "Enable") == 0)
    {
	/* Collect Value */
	char *strValue = NULL;
	int retPsmGet = CCSP_SUCCESS;

	retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi-PSM-DB.Enable", NULL, &strValue);
	if (retPsmGet == CCSP_SUCCESS) {
	    *pBool = _ansc_atoi(strValue);
	    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
	}
	else
	    *pBool = FALSE;
	return TRUE;
    }

    return FALSE;
/*#else
    CcspTraceInfo(("WiFi-PSM-DB not supported \n"));
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    return FALSE;
#endif*/
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        WiFiPsmDb_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
WiFiPsmDb_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
#if defined(FEATURE_SUPPORT_WIFIDB)
    int   ret   = 0;
    if (strcmp(ParamName, "Enable") == 0)
    {
	int retPsmGet = CCSP_SUCCESS;

	retPsmGet = PSM_Set_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.WiFi-PSM-DB.Enable", ccsp_string, bValue ? "1" : "0");
	if (retPsmGet != CCSP_SUCCESS) {
	    CcspTraceError(("Set failed for WiFi-PSM-DB \n"));
	    return FALSE;
	}
	CcspTraceInfo(("Successfully set WiFi-PSM-DB \n"));
        ret = CcspBaseIf_SendSignal_WithData(bus_handle, "WifiDbStatus", bValue ? "1" : "0");
        if ( ret != CCSP_SUCCESS ) {
            CcspTraceError(("%s : WifiDbStatus send data failed,  ret value is %d\n",__FUNCTION__ ,ret));
        }
	return TRUE;
    }
    return FALSE;
#else
    CcspTraceInfo(("WiFi-PSM-DB not supported \n"));
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(bValue);
    return FALSE;
#endif
}

#if defined (FEATURE_SUPPORT_RADIUSGREYLIST)
/**********************************************************************
    caller:     owner of this object

    prototype:

	BOOL
	RadiusGreyList_GetParamBoolValue
	    (
		ANSC_HANDLE                 hInsContext,
		char*                       ParamName,
		BOOL*                       pBool
	    );

     description:

	This function is called to retrieve Boolean parameter value;

     argument:  ANSC_HANDLE                 hInsContext,
		The instance handle;

		char*                       ParamName,
		The parameter name;

		BOOL*                       pBool
		The buffer of returned boolean value;

     return:     TRUE if succeeded.
**********************************************************************/

BOOL
RadiusGreyList_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* Collect Value */
        char *strValue = NULL;
        int retPsmGet = CCSP_SUCCESS;
        retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RadiusGreyList.Enable", NULL, &strValue);
        if (retPsmGet == CCSP_SUCCESS) {
            *pBool = _ansc_atoi(strValue);
            ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(strValue);
        }
        else
            *pBool = FALSE;
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

	BOOL
	RadiusGreyList_SetParamBoolValue
	    (
		ANSC_HANDLE                 hInsContext,
		char*                       ParamName,
		BOOL                        bValue
	    );

     description:

	This function is called to set BOOL parameter value;

     argument:  ANSC_HANDLE                 hInsContext,
		The instance handle;

		char*                       ParamName,
		The parameter name;

		BOOL                        bValue
		The updated BOOL value;

     return:     TRUE if succeeded.
**********************************************************************/
BOOL
RadiusGreyList_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue,RadiusGreyList_GetParamBoolValue)) {
         CcspTraceError(("[%s] set RadiusGreyListEnable [%s]\n",__FUNCTION__, (bValue ? "TRUE" : "FALSE")))
	return TRUE;
    }
    if (strcmp(ParamName, "Enable") == 0)
    {
        return CosaDmlSetRadiusGreyListEnable(bValue);
    }
    return FALSE;
}
#endif

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        IPv6onLnF_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
IPv6onLnF_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
        char *token = NULL;char *pt;

        if (IsBoolSame(hInsContext, ParamName, bValue, IPv6onLnF_GetParamBoolValue))
            return TRUE;

        if (strcmp(ParamName, "Enable") == 0)
        {
     	    char buf[128], OutBuff[128];
	    char Inf_name[32];
	    BOOL bFound = FALSE;
	    errno_t rc = -1;
	
            memset(buf,0,sizeof(buf));
            memset(OutBuff,0,sizeof(OutBuff));
 	    syscfg_get( NULL, "iot_brname", Inf_name, sizeof(Inf_name));
            if ( (Inf_name[0] == '\0') && (strlen(Inf_name)) == 0 )
            {
             	syscfg_get( NULL, "iot_ifname", Inf_name, sizeof(Inf_name));
            
       	    }
	    
	    if( Inf_name != NULL )
            {
            	    syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));

		    if( buf != NULL )
		    {
		        if (strstr(buf, Inf_name))
		            bFound = TRUE;
		        else
		            bFound = FALSE;

			
			if(bValue)
	                {
					if(bFound == FALSE)
					{
					// interface is not present in the list, we need to add interface to enable IPv6 PD

							rc = sprintf_s(OutBuff, sizeof(OutBuff),"%s%s,",buf,Inf_name);
							if(rc < EOK)
							{
								ERR_CHK(rc);
								return FALSE;
							}
							syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);

					}
			}
			else
			{
				
					if(bFound == TRUE)
					{
					// interface is present in the list, we need to remove interface to disable IPv6 PD
						pt = buf;
						   while((token = strtok_r(pt, ",", &pt))) {
							if(strncmp(Inf_name,token,strlen(Inf_name)))
							{
								strcat(OutBuff,token);
								strcat(OutBuff,",");
							}

						   }
					
						syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);
					}
			}
		    }
		    else
			{
				if(bValue)
				{
				strcat(OutBuff,Inf_name);
				strcat(OutBuff,",");
				syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);
				}
			}
		    return TRUE;
	    } else  
               return FALSE;

            return TRUE;
        }
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        IPv6onXHS_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
IPv6onXHS_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            char buf[128];
	        char *Inf_name = NULL;
    	    int retPsmGet = CCSP_SUCCESS;

            retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.2.Port.1.Name", NULL, &Inf_name);
            if (retPsmGet == CCSP_SUCCESS) {
	    	if( Inf_name != NULL )
            	{
                    /* CID: 68662 Array compared against 0*/ 
               	    if(!syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf)))
		    {
		        if (strstr(buf, Inf_name))
		            *pBool = TRUE;
		        else
		            *pBool = FALSE;
		    } else 
                       return FALSE;

		    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
		    return TRUE;
	        }
            }
            else
            *pBool = FALSE;
        }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        IPv6onXHS_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
IPv6onXHS_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
 char *token = NULL;char *pt;

 if (IsBoolSame(hInsContext, ParamName, bValue, IPv6onXHS_GetParamBoolValue))
        return TRUE;

 if (strcmp(ParamName, "Enable") == 0)
        {
     	    char buf[128], OutBuff[128];
	    char *Inf_name = NULL;
	    BOOL bFound = FALSE;
    	    int retPsmGet = CCSP_SUCCESS;
            errno_t rc = -1;

            retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.2.Port.1.Name", NULL, &Inf_name);
            if (retPsmGet == CCSP_SUCCESS)
				{		
                    memset(buf,0,sizeof(buf));
                    memset(OutBuff,0,sizeof(OutBuff));

					if( Inf_name != NULL )
						{
			 
					        /*CID: 65587 Array compared against 0*/
                                                /* CID: 53214 Logically dead code*/	
						if(!syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf)))
						{
							if (strstr(buf, Inf_name))
								bFound = TRUE;
							else
								bFound = FALSE;

						
							if(bValue)
							{
								if(bFound == FALSE)
								{
								// interface is not present in the list, we need to add interface to enable IPv6 PD

										rc = sprintf_s(OutBuff,sizeof(OutBuff),"%s%s,",buf,Inf_name);
										if(rc < EOK)
										{
											ERR_CHK(rc);
											return FALSE;
										}
										syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);

								}
							}
							else
							{
							
								if(bFound == TRUE)
								{
								// interface is present in the list, we need to remove interface to disable IPv6 PD
									pt = buf;
									   while((token = strtok_r(pt, ",", &pt))) {
										if(strncmp(Inf_name,token,strlen(Inf_name)))
										{
											strcat(OutBuff,token);
											strcat(OutBuff,",");
										}

									   }
								
									syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);
								}
							}
						}
						else
						{
							if(bValue)
							{
							strcat(OutBuff,Inf_name);
							strcat(OutBuff,",");
							syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);
							}
						}
					    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
						return TRUE;
					}
				   ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
            }
            return TRUE;
        }
    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        IPv6onPOD_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
IPv6onPOD_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            char buf[128];
            char *Inf_name = NULL;
    	    int retPsmGet = CCSP_SUCCESS;

            retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.10.Name", NULL, &Inf_name);
			//Setting the interface name, in case it is null.
			if(!retPsmGet) {
				retPsmGet = CCSP_SUCCESS;
				Inf_name = "br403";		
			}
            if (retPsmGet == CCSP_SUCCESS) {
	    	if( Inf_name != NULL )
            	{
               	    syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));
	        if( buf != NULL )
		    {
		        if (strstr(buf, Inf_name))
		            *pBool = TRUE;
		        else
		            *pBool = FALSE;
		    }
		    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
	        }
            }
            else
            *pBool = FALSE;
            return TRUE;
        }

    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        IPv6onPOD_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
IPv6onPOD_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
 char *token = NULL;char *pt;

 if (IsBoolSame(hInsContext, ParamName, bValue, IPv6onPOD_GetParamBoolValue))
        return TRUE;

 if (strcmp(ParamName, "Enable") == 0)
        {
     	    char buf[128], OutBuff[128];
			char *Inf_name = NULL;
			BOOL bFound = FALSE;
    	    int retPsmGet = CCSP_SUCCESS;

            retPsmGet = PSM_Get_Record_Value2(bus_handle,g_Subsystem, "dmsb.l2net.10.Name", NULL, &Inf_name);
			if(!retPsmGet)
			{
				retPsmGet = CCSP_SUCCESS;
				Inf_name = "br403";		
			}
            if (retPsmGet == CCSP_SUCCESS)
				{
                    memset(buf,0,sizeof(buf));
                    memset(OutBuff,0,sizeof(OutBuff));

					if( Inf_name != NULL )
						{
						syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));
						if( buf != NULL )
						{
							if (strstr(buf, Inf_name))
								bFound = TRUE;
							else
								bFound = FALSE;


							if(bValue)
							{
								if(bFound == FALSE)
								{
								// interface is not present in the list, we need to add interface to enable IPv6 PD

										strncpy(OutBuff, buf, sizeof(buf));
										strcat(OutBuff,Inf_name);
										strcat(OutBuff,",");
										syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);

								}
							}
							else
							{

								if(bFound == TRUE)
								{
								// interface is present in the list, we need to remove interface to disable IPv6 PD
									pt = buf;
									   while((token = strtok_r(pt, ",", &pt))) {
										if(strncmp(Inf_name,token,strlen(Inf_name)))
										{
											strcat(OutBuff,token);
											strcat(OutBuff,",");
										}
									   }

									syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);
								}
							}
						}
						else
						{
							if(bValue)
							{
							strcat(OutBuff,Inf_name);
							strcat(OutBuff,",");
							syscfg_set_commit(NULL, "IPv6_Interface",OutBuff);
							}
						}
					    ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
						return TRUE;
					}
				   ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(Inf_name);
            }
            return TRUE;
        }
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object

    prototype: 

        BOOL
        EvoStream_DirectConnect_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
EvoStream_DirectConnect_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            char buf[8];
            syscfg_get( NULL, "EvoStreamDirectConnect", buf, sizeof(buf));

            if( buf != NULL )
            {
                if (strcmp(buf, "true") == 0)
                    *pBool = TRUE;
                else
                    *pBool = FALSE;
            }
            return TRUE;
        }

    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EvoStream_DirectConnect_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
EvoStream_DirectConnect_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
        if (IsBoolSame(hInsContext, ParamName, bValue, EvoStream_DirectConnect_GetParamBoolValue))
            return TRUE;

        if (strcmp(ParamName, "Enable") == 0)
        {
            /* collect value */
            if(syscfg_set_commit(NULL, "EvoStreamDirectConnect", (bValue == TRUE) ? "true" : "false") != 0)
		{
			CcspTraceError(("EvoStreamDirectConnect :%d Failed to SET\n", bValue ));
		}
		else
		{
			CcspTraceInfo(("EvoStreamDirectConnect :%d Success\n", bValue ));
			v_secure_system("sysevent set firewall-restart");
		}
            return TRUE;
        }
    return FALSE;
}
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        RDKFirmwareUpgrader_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RDKFirmwareUpgrader_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char buf[8];

        /* collect value */
        syscfg_get( NULL, "RDKFirmwareUpgraderEnabled", buf, sizeof(buf));

        if( buf != NULL )
        {
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        RDKFirmwareUpgrader_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RDKFirmwareUpgrader_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "RDKFirmwareUpgraderEnabled", bValue ? "true" : "false") != 0)
        {
            CcspTraceError(("syscfg_set RDKFirmwareUpgraderEnabled failed\n"));
        }
        else
        {
            return TRUE;
        }
    }
     return FALSE;
}
/***********************************************************************

 APIs for Object:

    Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH

    *  ReverseSSH_SetParamStringValue // Set args required for reverse SSH
    *  ReverseSSH_GetParamStringValue // Get args set for reverse SSH
    *  ReverseSSH_SetParamUlongValue // Set args required for reverse SSH
    *  ReverseSSH_GetParamUlongValue // Get args set for reverse SSH

***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        ReverseSSH_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
ReverseSSH_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char* activeStr = "ACTIVE";
    char* inActiveStr = "INACTIVE";
    errno_t  rc = -1;

    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "xOpsReverseSshArgs") == 0)
    {
        rc = strcpy_s(pValue, *pulSize, "");
        if (rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }

        return 0;
    }

    if (strcmp(ParamName, "xOpsReverseSshStatus") == 0)
    {
        if (isRevSshActive())
        {
            rc = strcpy_s(pValue, *pulSize, activeStr);
            if (rc != EOK)
            {
                ERR_CHK(rc);
                return -1;
            }
        }
        else
        {
            rc = strcpy_s(pValue, *pulSize, inActiveStr);
            if (rc != EOK)
            {
                ERR_CHK(rc);
                return -1;
            }
        }

        return 0;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return -1;
}



/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        ReverseSSH_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded,
                FALSE if failed.

**********************************************************************/
BOOL
ReverseSSH_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "xOpsReverseSshArgs") == 0)
    {
        setXOpsReverseSshArgs(pString);
        return TRUE;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));

    return FALSE;
}

ULONG
ReverseSSH_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8];

    if (strcmp(ParamName, "xOpsReverseSshIdleTimeout") == 0)
    {
        /* collect value */
        if (getxOpsReverseSshIdleTimeout(hInsContext, buf, sizeof(buf)) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceWarning(("getxOpsReverseSshIdleTimeout failed\n"));
            return FALSE;
        }

        *puLong = atoi(buf);
        return TRUE;
    }
    return FALSE;
}

BOOL
ReverseSSH_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "xOpsReverseSshIdleTimeout") == 0)
    {
        if (setxOpsReverseSshIdleTimeout(uValue) != ANSC_STATUS_SUCCESS)
        {
            CcspTraceWarning(("setxOpsReverseSshIdleTimeout failed\n"));
            return FALSE;
        }
        return TRUE;
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

BOOL
ReverseSSH_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "xOpsReverseSshTrigger") == 0)
    {
        *pBool = FALSE;
        return TRUE;
    }

    return FALSE;
}

BOOL
ReverseSSH_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "xOpsReverseSshTrigger") == 0)
    {
        if (setXOpsReverseSshTrigger(bValue ? "start" : "stop") == 0)
        {
            CcspTraceError(("setXOpsReverseSshTrigger failed\n"));
            return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

#if 0
/***********************************************************************

 APIs for Object:

    Device.DeviceInfo.X_RDKCENTRAL-COM_EthernetWAN.

    *  EthernetWAN_GetParamStringValue

***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EthernetWAN_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     TRUE if succeeded;
                FALSE if failed

**********************************************************************/

ULONG
EthernetWAN_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pUlSize);
    errno_t     rc = -1;
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "CurrentOperationalMode") == 0)
    {
        /* collect value */
        char buf[32];
		
        if( ( 0 == syscfg_get( NULL, "eth_wan_enabled", buf, sizeof( buf ) ) ) && \
			( '\0' != buf[ 0 ] )
		   )
        {
			if( 0 == strcmp( buf, "true" ) )
			{
				rc = strcpy_s( pValue, *pUlSize, "Ethernet");
				if(rc != EOK)
				{
					ERR_CHK(rc);
					return -1;
				}
			}
			else
			{
				rc = strcpy_s( pValue, *pUlSize, "DOCSIS");
				if(rc != EOK)
				{
					ERR_CHK(rc);
					return -1;
				}
			}
        }
        else
        {
            CcspTraceError(("%s syscfg_get failed for eth_wan_enabled. so giving default as DOCSIS\n",__FUNCTION__));
			rc = strcpy_s( pValue, *pUlSize, "DOCSIS");
			if(rc != EOK)
			{
				ERR_CHK(rc);
				return -1;
			}
        }
		
        return 0;
    }

    return -1;
}

#endif
/***********************************************************************

 APIs for Object:

    Device.DeviceInfo.X_RDKCENTRAL-COM_EthernetWAN.MACsec.OperationalStatus

    *  EthernetWAN_MACsec_GetParamStringValue

***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        EthernetWAN_MACsec_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
#ifdef _MACSEC_SUPPORT_
ULONG
EthernetWAN_MACsec_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    errno_t  rc = -1;
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "OperationalStatus") == 0)
    {
        BOOL flag;

        if ( RETURN_OK == platform_hal_GetMACsecOperationalStatus( ETHWAN_DEF_INTF_NUM, &flag )) {
           rc = strcpy_s(pValue, *pUlSize, ((TRUE == flag) ? "Enabled" : "Disabled") );
           if(rc != EOK)
           {
              ERR_CHK(rc);
              return -1;
           }

           return 0;
        }
    }
    return 0;
}
#endif


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Logging_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
	        ULONG*                      puLong
            );

    description:

        This function is called to retrieve ulong parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

     		   ULONG*                      puLong
                The buffer of returned ulong value;

    return:     TRUE if succeeded.

**********************************************************************/


BOOL
Logging_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "DmesgLogSyncInterval") == 0)
    {
        /* collect value */
	 char buf[8];

	 syscfg_get(NULL, "dmesglogsync_interval", buf, sizeof(buf));
         *puLong = atoi(buf);
        return TRUE;
    }

	
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}


/**********************************************************************  

    caller:     owner of this object 

    prototype: 

BOOL
Logging_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    ) ;
    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

 	        ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Logging_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    /* check the parameter name and set the corresponding value */
    if (strcmp(ParamName, "DmesgLogSyncInterval") == 0)
    {
		if (syscfg_set_u_commit(NULL, "dmesglogsync_interval", uValue) != 0) 
		{
			CcspTraceWarning(("syscfg_set failed to set DmesgLogSyncInterval \n"));
		       return FALSE;
		}
		return TRUE;
    } 

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SwitchToDibbler_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
#if defined(_COSA_INTEL_XB3_ARM_) || defined(INTEL_PUMA7)
BOOL
SwitchToDibbler_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
	/* This Get API is only for XB3,AXB6 devices */
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        char buf[8];
        if( syscfg_get( NULL, "dibbler_client_enable", buf, sizeof(buf))==0)
        {
            if (strcmp(buf, "true") == 0)
            {
                *pBool = TRUE;
            }
            else
            {
                *pBool = FALSE;
            }
        }
        else
        {
            CcspTraceWarning(("%s syscfg_get failed  for dibbler_client_enable\n",__FUNCTION__));
            *pBool = FALSE;
        }

        return TRUE;
    }
    return FALSE;
}

void* dhcpSwitchThread(void* vptr_value)
{
        pthread_detach(pthread_self());
        v_secure_system("/etc/dhcpswitch.sh %s &", (char *) vptr_value);
        return NULL;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SwitchToDibbler_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SwitchToDibbler_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
     /* This set API is only for XB3,AXB6 devices */

    if (strcmp(ParamName, "Enable") == 0)
    {
        char buf[8];
        char event[8];
        pthread_t tid;
        char* operation = NULL;
        int val;

        /* collect previous flag value */
       if( syscfg_get( NULL, "dibbler_client_enable", buf, sizeof(buf)) == 0)
        {
                val = strcmp(buf,"true")?0:1;
                if (val != bValue)
                {
                        if (bValue)
                        {
                                commonSyseventGet("dhcpclient_v4", event, sizeof(event));
                                if (atoi(event) == 1)
                                {
                                        commonSyseventSet("dhcpclient_v6", "1");
                                        AnscTraceWarning(("dhcpclient_v4 event is enabled. So enabling dhcpclient_v6 event \n"));
                                }
                                else
                                {
                                        operation = "schedule_v6_cron";
                                        AnscTraceWarning(("dhcpclient_v4 event is not enabled.scheduling cron \n"));
                                        pthread_create(&tid, NULL, &dhcpSwitchThread, (void *)operation);
                                }
                        }
                        else
                        {
                                operation = "clear_v6_cron";
                                AnscTraceWarning(("dhcp client switching back to default \n"));
                                pthread_create(&tid, NULL, &dhcpSwitchThread, (void *)operation);
                        }
                }
                else if(!bValue)
                {
                        char v6event[8];
                        commonSyseventGet("dhcpclient_v6", v6event, sizeof(v6event));
                        if (atoi(v6event) == 1)
                        {
                                char v4event[8];
                                commonSyseventGet("dhcpclient_v4", v4event, sizeof(v4event));
                                if (atoi(v4event) ==1)
                                {
                                        commonSyseventSet("dhcpclient_v6", "0");
                                        AnscTraceWarning(("dhcpclient_v6 is disabled\n"));
                                }
                                else
                                {
                                        commonSyseventSet("dhcpclient_v6", "0");
                                        v_secure_system("sed -i '/dhcpswitch.sh/d' /var/spool/cron/crontabs/root &");
                                        AnscTraceWarning(("dhcpclient_v6 is disabled and scheduled cron removed\n"));
                                }

                        }
                        else
                        {
                                AnscTraceWarning(("No set operation done since dibbler_client_enable flag already set to %d\n", bValue));
                        }
                }
                else
                {
                        AnscTraceWarning(("No set operation done since dibbler_client_enable flag already set to %d\n", bValue));
                }
        }
        else
        {
                AnscTraceWarning(("syscfg_get failed for diibler_client_enable\n"));
                return FALSE;
        }


        return TRUE;
    }
   return FALSE;

}
#endif

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        Syndication_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
Syndication_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    errno_t      rc  = -1;

    if (strcmp(ParamName, "PartnerId") == 0)
    {
        /* collect value */
        //CosaDmlDiGetSyndicationPartnerId(NULL, pValue,pulSize);
        rc = strcpy_s( pValue, *pulSize, pMyObject->PartnerID);
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return -1;
        }
        return 0;
    }

    if (strcmp(ParamName, "TR69CertLocation") == 0)
    {
        /* collect value */
        CosaDmlDiGetSyndicationTR69CertLocation( hInsContext, pMyObject->TR69CertLocation.ActiveValue );
        rc = strcpy_s( pValue, *pulSize, pMyObject->TR69CertLocation.ActiveValue );
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return -1;
        }
        return 0;
    }
#if defined(_COSA_BCM_ARM_) && !defined(_CBR_PRODUCT_REQ_)
    if (strcmp(ParamName, "CMVoiceImageSelect") == 0)
    {
	char buf[64];

	if(0 == syscfg_get(NULL, "CMVoiceImg", buf, sizeof(buf)))
	{
	    return  update_pValue(pValue,pulSize, buf);
	}
	else
	{
		CcspTraceError(("syscfg_get for CMVoiceImg failed\n"));
		return 1;
	}
    }
#endif
    if (strcmp(ParamName, "XconfURL") == 0)
    {
        return  update_pValue(pValue,pulSize, pMyObject->XconfURL.ActiveValue);
    }
    if (strcmp(ParamName, "LogUploadURL") == 0)
    {
        return  update_pValue(pValue,pulSize, pMyObject->LogUploadURL.ActiveValue);
    }
    if (strcmp(ParamName, "Telemetry") == 0)
    {
        return  update_pValue(pValue,pulSize, pMyObject->TelemetryURL.ActiveValue);
    }
    if (strcmp(ParamName, "CrashPortal") == 0)
    {
        return  update_pValue(pValue,pulSize, pMyObject->CrashPortalURL.ActiveValue);
    }
    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Syndication_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded,
                FALSE if failed.

**********************************************************************/
BOOL
Syndication_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    ANSC_STATUS 					retValue  = ANSC_STATUS_FAILURE;
    char PartnerID[PARTNER_ID_LEN] = {0};
    char * requestorStr = getRequestorString();
    char * currentTime = getTime();

    IS_UPDATE_ALLOWED_IN_DM(ParamName, requestorStr);
    errno_t rc                     = -1;
    int ind                        = -1;

     if((pString == NULL) || (ParamName == NULL))
     {
         AnscTraceWarning(("RDK_LOG_WARN, %s %s:%d\n",__FILE__,__FUNCTION__,__LINE__));
         return FALSE;
     }

    /* check the parameter name and set the corresponding value */
    if ( !(rc = strcmp_s("TR69CertLocation", strlen("TR69CertLocation"), ParamName, &ind)) )
    {
        if(!ind)
        {
        /* check the length of pString and restricting the maximum length to 256 as CosaDmlDiGetSyndicationTR69CertLocation reading only 256 bytes*/
        /* the scope of this length check is particular to TR69CertLocation */
                if(strlen(pString) >= MAX_ALLOWABLE_STRING_LEN)
                {
                    AnscTraceWarning(("Maximum Allowable string length is less than 256 bytes %s %d\n", __FUNCTION__, __LINE__));
                    return FALSE;
                }

                IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pMyObject->TR69CertLocation.UpdateSource);

		retValue = CosaDmlDiSetSyndicationTR69CertLocation( hInsContext, pString );
		if( ANSC_STATUS_SUCCESS == retValue )
		{
                        rc = STRCPY_S_NOCLOBBER(pMyObject->TR69CertLocation.ActiveValue, sizeof(pMyObject->TR69CertLocation.ActiveValue), pString);
                        if(rc != EOK)
                        {
                             AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                        } 
                        rc = STRCPY_S_NOCLOBBER(pMyObject->TR69CertLocation.UpdateSource, sizeof(pMyObject->TR69CertLocation.UpdateSource), pString);
                        if(rc != EOK)
                        {
                             AnscTraceWarning(("RDK_LOG_WARN, safeclib strcpy_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                        }
                        getPartnerId(PartnerID);
                        if ( PartnerID[ 0 ] != '\0')
                            UpdateJsonParam("Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.TR69CertLocation",PartnerID, pString, requestorStr, currentTime);
		}
	        return TRUE;
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

    if ( !(rc = strcmp_s("PartnerId", strlen("PartnerId"), ParamName, &ind)) )
    {
        if(!(ind))
        {
#if defined (_RDK_REF_PLATFORM_)
		ind = 0;
		if ( !(rc = strcmp_s("comcast", strlen("comcast"), pString, &ind) ) ) //Compare if input string is comcast
		{
			if( ind != 0 )//if input partner ID string is comcast,you wont enter this 'if' loop
			{
#endif
                if ( !(rc = strcmp_s(pMyObject->PartnerID, sizeof(pMyObject->PartnerID), pString, &ind)) )
		{
                        if(ind != 0)
                        {
			    retValue = setTempPartnerId( pString );
			    if( ANSC_STATUS_SUCCESS == retValue )
			    {
			        ULONG    size = 0;
				//Get the Factory PartnerID
			        memset(PartnerID, 0, sizeof(PartnerID));
			        getFactoryPartnerId(PartnerID, &size);
			
			        CcspTraceInfo(("[SET-PARTNERID] Factory_Partner_ID:%s\n", ( PartnerID[ 0 ] != '\0' ) ? PartnerID : "NULL" ));
			        CcspTraceInfo(("[SET-PARTNERID] Current_PartnerID:%s\n", pMyObject->PartnerID ));
			        CcspTraceInfo(("[SET-PARTNERID] Overriding_PartnerID:%s\n", pString ));
								
				return TRUE;
			    }
                        }
		}
#if defined (_RDK_REF_PLATFORM_)
			}
		}
#endif
                else if(rc != EOK)
                {
                     AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
                     return FALSE;
                }
        }
    }
    else if(rc != EOK)
    {
        AnscTraceWarning(("RDK_LOG_WARN, safeclib strcmp_s- %s %s:%d rc =%d \n",__FILE__, __FUNCTION__,__LINE__,rc));
        return FALSE;
    }

     return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SwitchToUDHCPC_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
#if defined(_COSA_INTEL_XB3_ARM_) || defined(INTEL_PUMA7)
BOOL
SwitchToUDHCPC_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
	/* This Get API is only for XB3,AXB6 devices */
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        char buf[8];
        if( syscfg_get( NULL, "UDHCPEnable", buf, sizeof(buf))==0)
        {
            if (strcmp(buf, "true") == 0)
            {
                *pBool = TRUE;
            }
            else
            {
                *pBool = FALSE;
            }
        }
        else
        {
            CcspTraceWarning(("%s syscfg_get failed  for UDHCPEnable\n",__FUNCTION__));
            *pBool = FALSE;
        }

        return TRUE;
    }
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        SwitchToUDHCPC_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SwitchToUDHCPC_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
     /* This set API is only for XB3,AXB6 devices */

    if (strcmp(ParamName, "Enable") == 0)
    {
        char buf[8];
        char event[8];
        int val;

        /* collect previous flag value */
       if( syscfg_get( NULL, "UDHCPEnable", buf, sizeof(buf)) == 0)
        {
        	val = strcmp(buf,"true")?0:1;
                if (val != bValue)
                {
                        if (bValue)
                        {
                                commonSyseventGet("dhcpclient_v6", event, sizeof(event));
                                if (atoi(event) == 1)
                                {
                                        commonSyseventSet("dhcpclient_v4", "1");
                                        AnscTraceWarning(("dhcpclient_v6 event is enabled.so,enabling dhcpclient_v4 event \n"));
                                }
                                else
                                {
                                        v_secure_system("sh /etc/dhcpswitch.sh schedule_v4_cron &");
                                        AnscTraceWarning(("dhcpclient_v6 event is not enabled.scheduling cron \n"));
                                }
                        }
                        else
                        {
                                v_secure_system("sh /etc/dhcpswitch.sh clear_v4_cron &");
                                AnscTraceWarning(("dhcp client switching back to default \n"));
                        }
                }
                else if(!bValue)
                {
                        char v4event[8];
                        commonSyseventGet("dhcpclient_v4", v4event, sizeof(v4event));
                        if (atoi(v4event) == 1)
                        {
                                char v6event[8];
                                commonSyseventGet("dhcpclient_v6", v6event, sizeof(v6event));
                                if (atoi(v6event) ==1)
                                {
                                        commonSyseventSet("dhcpclient_v4", "0");
                                        AnscTraceWarning(("dhcpclient_v4 is disabled and no wan restart\n"));
                                }
                                else
                                {
                                        commonSyseventSet("dhcpclient_v4", "0");
                                        v_secure_system("sh /etc/dhcpswitch.sh removecron &");
                                        AnscTraceWarning(("dhcpclient_v4 is disabled and scheduled cron removed\n"));
                                }


                        }
                        else
                        {
                                AnscTraceWarning(("No set operation done since UDHCPEnable flag already set to %d\n", bValue));
                        }
                }
                else
                {
                        AnscTraceWarning(("No set operation done since UDHCPEnable flag already set to %d\n", bValue));
                }
        }
        else
        {
                AnscTraceWarning(("syscfg_get failed for UDHCPEnable\n"));
                return FALSE;
        }


        return TRUE;
    }
   return FALSE;
}
#endif
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        Syndication_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
Syndication_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(pBool);
    /* check the parameter name and return the corresponding value */

    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
       Syndication_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Syndication_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    UNREFERENCED_PARAMETER(bValue);
    return FALSE;
}

/***********************************************************************
APIs for Object:
        Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.WANsideSSH.
          *  WANsideSSH_GetParamBoolValue
          *  WANsideSSH_SetParamBoolValue

***********************************************************************/

BOOL
WANsideSSH_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
	/* check the parameter name and return the corresponding value */
        PCOSA_DATAMODEL_DEVICEINFO              pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;


        if (strcmp(ParamName, "Enable") == 0)
        {
		*pBool = pMyObject->bWANsideSSHEnable.ActiveValue;
                return TRUE;
        }


	return FALSE;
}

BOOL
WANsideSSH_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
   UNREFERENCED_PARAMETER(hInsContext);
   PCOSA_DATAMODEL_DEVICEINFO              pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

   if (strcmp(ParamName, "Enable") == 0)
   {
        char * requestorStr = getRequestorString();
        char * currentTime = getTime();

        IS_UPDATE_ALLOWED_IN_DM(ParamName, requestorStr);
        IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pMyObject->bWANsideSSHEnable.UpdateSource);

	if (pMyObject->bWANsideSSHEnable.ActiveValue == bValue)
	{
		CcspTraceInfo(("WANsideSSH is already %s\n", ( bValue ==TRUE ) ?  "Enabled" : "Disabled"));
		return TRUE;
	}

	if (syscfg_set_commit(NULL, "WANsideSSH_Enable", (bValue == TRUE) ? "true" : "false") != 0)
	{
		return FALSE;
	}

	pMyObject->bWANsideSSHEnable.ActiveValue = bValue;
	if (bValue == TRUE)
		v_secure_system("sh /lib/rdk/wan_ssh.sh enable &");
	else
		v_secure_system("sh /lib/rdk/wan_ssh.sh disable &");

        char *value = ( bValue ==TRUE ) ?  "true" : "false";
        char PartnerID[PARTNER_ID_LEN] = {0};
        if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
            UpdateJsonParam("Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.WANsideSSH.Enable",PartnerID, value, requestorStr, currentTime);

	return TRUE;

   }
   return FALSE;
}
/***********************************************************************
APIs for Object:
	DeviceInfo.X_RDKCENTRAL-COM_Syndication.RDKB_UIBranding.
	   *  RDKB_UIBranding_GetParamBoolValue
	   *  RDKB_UIBranding_GetParamStringValue
	   *  RDKB_UIBranding_SetParamStringValue
	  
***********************************************************************/


ULONG
RDKB_UIBranding_GetParamStringValue

	(
		ANSC_HANDLE 				hInsContext,
		char*						ParamName,
		char*						pValue,
		ULONG*						pulSize
	)
{
    UNREFERENCED_PARAMETER(hInsContext);
	PCOSA_DATAMODEL_DEVICEINFO		pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
	PCOSA_DATAMODEL_RDKB_UIBRANDING	pBindObj =	& pMyObject->UiBrand;
	
	if (strcmp(ParamName, "DefaultLanguage") == 0)
        {
            return  update_pValue(pValue,pulSize, pBindObj->DefaultLanguage.ActiveValue);
        }
	
	 return -1;
}


BOOL
RDKB_UIBranding_SetParamStringValue


    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
	PCOSA_DATAMODEL_DEVICEINFO              pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
        PCOSA_DATAMODEL_RDKB_UIBRANDING pBindObj =      & pMyObject->UiBrand;
        char PartnerID[PARTNER_ID_LEN] = {0};
        char * requestorStr = getRequestorString();
        char * currentTime = getTime();
        errno_t rc = -1;
        int ind = -1;

        if((ParamName == NULL) || (pString == NULL))
            return FALSE;

        IS_UPDATE_ALLOWED_IN_DM(ParamName, requestorStr);

        if (AnscValidStringCheck2(pString, "<>%`|'") != TRUE)
            return FALSE;

   if((CCSP_SUCCESS == getPartnerId(PartnerID) ) && (PartnerID[ 0 ] != '\0') )
   {

            rc = strcmp_s("DefaultLanguage", strlen("DefaultLanguage"), ParamName, &ind);
            ERR_CHK(rc);
            if((rc == EOK) && (!ind))
            {
                IS_UPDATE_ALLOWED_IN_JSON(ParamName, requestorStr, pBindObj->DefaultLanguage.UpdateSource);

                /* Below condition is added to restrict the maximum pString length less than acceptable value for buffer overflow issues
                 * UpdateJsonParam function doesn't have the maximum permissible string length validation check.
                 */
                if(strlen(pString) < sizeof(pBindObj->DefaultLanguage.ActiveValue))
                {
                        if ( ANSC_STATUS_SUCCESS == UpdateJsonParam("Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.RDKB_UIBranding.DefaultLanguage",PartnerID,pString, requestorStr, currentTime))
                        {
                                rc = STRCPY_S_NOCLOBBER(pBindObj->DefaultLanguage.ActiveValue, sizeof(pBindObj->DefaultLanguage.ActiveValue), pString);
                                if(rc != EOK)
                                {
                                     ERR_CHK(rc);
                                     return FALSE;
                                }

                                rc = STRCPY_S_NOCLOBBER(pBindObj->DefaultLanguage.UpdateSource, sizeof(pBindObj->DefaultLanguage.UpdateSource), requestorStr);
                                if(rc != EOK)
                                {
                                     ERR_CHK(rc);
                                     return FALSE;
                                }
                                return TRUE;
                        }
                 }
                 else
                 {
                     CcspTraceError(("pString length more than permissible value - %s:%d\n", __FUNCTION__, __LINE__));
                     return FALSE;
                 }

            }

   }
   return FALSE;
}


#if !defined(_COSA_FOR_BCI_) || defined(_CBR2_PRODUCT_REQ_)
/***********************************************************************

 APIs for Object:

    DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.

    *  RPC_GetParamUlongValue
    *  RPC_SetParamUlongValue
    *  RPC_GetParamStringValue
    *  RPC_SetParamStringValue

***********************************************************************/

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        RPC_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            );

    description:

        This function is called to retrieve ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RPC_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

	
    /* check the parameter name and return the corresponding value */
#ifndef FEATURE_FWUPGRADE_MANAGER
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    if (strcmp(ParamName, "DeferFWDownloadReboot") == 0)
    {
        /* collect value */
        *puLong = pMyObject->DeferFWDownloadReboot;
        return TRUE;
    }	
#endif    
    if (strcmp(ParamName, "RebootPendingNotification") == 0)
    {
        /* collect value */
        *puLong = 0;
        return TRUE;
    }	    
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        RPC_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uValue
            );

    description:

        This function is called to set ULONG parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uValue
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RPC_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
	
    /* check the parameter name and set the corresponding value */
#ifndef FEATURE_FWUPGRADE_MANAGER
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    if (strcmp(ParamName, "DeferFWDownloadReboot") == 0)
    {
        /* collect value */
		CosaDmlDiSet_DeferFWDownloadReboot(&(pMyObject->DeferFWDownloadReboot),uValue);
		return TRUE;
    } 
#endif    
    if (strcmp(ParamName, "RebootPendingNotification") == 0)
    {
        /* collect value */
        char buff[64] = {0};
        errno_t rc = -1;
        rc = sprintf_s(buff, sizeof(buff),"%lu", uValue);
        if(rc < EOK)
        {
           ERR_CHK(rc);
           return FALSE;
        }
        Send_Notification_Task(buff, NULL, NULL, "reboot-pending", NULL, NULL, NULL, NULL);
        return TRUE;
    }	       
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        ULONG
        RPC_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
RPC_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pulSize
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    errno_t  rc  = -1;
    /* check the parameter name and return the corresponding value */
    if (strcmp(ParamName, "RebootDevice") == 0)
    {
        /* collect value */
        rc = strcpy_s(pValue, *pulSize, "");
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }

    if (strcmp(ParamName, "FirmwareDownloadStartedNotification") == 0)
    {
	/* collect value */
	rc = strcpy_s( pValue, *pulSize, pMyObject->FirmwareDownloadStartedNotification);
	if(rc != EOK)
	{
		ERR_CHK(rc);
		return -1;
	}
	return 0;
    }

    if (strcmp(ParamName, "DeviceManageableNotification") == 0)
    {
	/* collect value */
	rc = strcpy_s( pValue, *pulSize, pMyObject->DeviceManageableNotification);
	if(rc != EOK)
	{
		ERR_CHK(rc);
		return -1;
	}
	return 0;
    }

    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return -1;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        RPC_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set string parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RPC_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    char *current_time = NULL;
    char *priority = NULL;
    char *current_fw_ver = NULL;
    char *download_fw_ver = NULL;
    const char s[2] = ",";
    char *notifyStr = NULL;
    errno_t rc =-1;
    int ind =-1;
    /* check the parameter name and set the corresponding value */

    rc = strcmp_s("RebootDevice",strlen( "RebootDevice"),ParamName,&ind);
    ERR_CHK(rc);
    if((!ind) && (rc == EOK))  
    {
      
        if( TRUE == CosaDmlDi_ValidateRebootDeviceParam( pString ) )
		{
			CosaDmlDiSet_RebootDevice(pString);
        	return TRUE;
		}
    }

        rc =  strcmp_s("FirmwareDownloadStartedNotification", strlen("FirmwareDownloadStartedNotification"),ParamName, &ind);
	ERR_CHK(rc);
        if((!ind) && (rc == EOK)) 
        {
	    char notifyEnable[64] = {0};
        

             syscfg_get( NULL, "ManageableNotificationEnabled", notifyEnable, sizeof(notifyEnable));
             if(notifyEnable[0] != '\0') 
             {
		  
		  rc = strcmp_s("true", strlen("true"),notifyEnable,&ind);
		  ERR_CHK(rc);
                 if((!ind) && (rc == EOK))  
		  {
		     /* collect value */
		     char buff[128];
		     char *timeValue = NULL;
		     memset(buff, 0, sizeof(buff));
		     snprintf(buff,sizeof(buff),"%s",pString);	
		     rc =  memset_s( pMyObject->FirmwareDownloadStartedNotification, sizeof( pMyObject->FirmwareDownloadStartedNotification ),0, sizeof( pMyObject->FirmwareDownloadStartedNotification ));
		     ERR_CHK(rc);
		     rc = STRCPY_S_NOCLOBBER( pMyObject->FirmwareDownloadStartedNotification,sizeof( pMyObject->FirmwareDownloadStartedNotification ), pString );
		     if(rc != EOK)
                    {
	               ERR_CHK(rc);
	               return FALSE;
                    }
		    
		     timeValue = notifyStr = strdup(buff);
		     // loop through the string to extract all tokens
			if( notifyStr != NULL )
			{
				current_time = strsep(&notifyStr, s);
				priority = strsep(&notifyStr, s);
				current_fw_ver = strsep(&notifyStr, s);
				download_fw_ver = strsep(&notifyStr, s);
			}
			if( current_time != NULL )
			{
				set_firmware_download_start_time(strdup(current_time));
			}
			if(current_time != NULL && priority != NULL && current_fw_ver != NULL && download_fw_ver !=NULL)
			{
				Send_Notification_Task(NULL,current_time, NULL, "firmware-download-started", NULL, priority,current_fw_ver,download_fw_ver);
			}
			else
			{
				CcspTraceWarning(("Received insufficient data to process notification, firmware download started notfication is not sent\n"));			
			}
			free(timeValue);
		}	
                  
                 else
	        {
		   CcspTraceWarning(("ManageableNotificationEnabled is false, firmware download start notification is not sent\n"));
	        }
	   }
	   else
	   {
		   CcspTraceWarning(("ManageableNotificationEnabled is false, firmware download start notification is not sent\n"));
	   }
	return TRUE;
      }

        rc = strcmp_s( "DeviceManageableNotification",strlen("DeviceManageableNotification"),ParamName, &ind);
	ERR_CHK(rc);
       if((!ind) && (rc == EOK)) 
	 
       {
          char status[64] = {0};
        
          syscfg_get( NULL, "ManageableNotificationEnabled", status, sizeof(status));
          if(status[0] != '\0') 
          {
		rc =strcmp_s("true", strlen("true"),status,&ind);
		ERR_CHK(rc);
                if((!ind) && (rc == EOK)) 
		{
                   
		    rc = memset_s( pMyObject->DeviceManageableNotification,sizeof( pMyObject->DeviceManageableNotification ), 0,sizeof( pMyObject->DeviceManageableNotification ));
		    ERR_CHK(rc);
	            rc = STRCPY_S_NOCLOBBER( pMyObject->DeviceManageableNotification,sizeof( pMyObject->DeviceManageableNotification ) ,pString );
	           if(rc != EOK)
                  {
                    ERR_CHK(rc);
                    return FALSE;
                  }
                 Send_Notification_Task(NULL, NULL, NULL, "fully-manageable", pString,NULL, NULL,NULL);
	      }	 
              else
	     {
		CcspTraceWarning(("ManageableNotificationEnabled is false, device manage notification is not sent\n"));
	     }
        }
	else
	{
		CcspTraceWarning(("ManageableNotificationEnabled is false, device manage notification is not sent\n"));
	}
        return TRUE;
     }

     CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
	 return FALSE;
}
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        RPC_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RPC_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    if (strcmp(ParamName, "AbortReboot") == 0)
    {
        /*CcspTraceWarning(("supported parameter '%s'\n", ParamName));*/
        *pBool = pMyObject->AbortReboot;
        return TRUE;
    }

    if (strcmp(ParamName, "FirmwareDownloadCompletedNotification") == 0)
    {
	/*CcspTraceWarning(("supported parameter '%s'\n", ParamName));*/
	*pBool = pMyObject->FirmwareDownloadCompletedNotification;
	return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        RPC_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
RPC_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
   UNREFERENCED_PARAMETER(hInsContext);
   PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

   if (strcmp(ParamName, "AbortReboot") == 0)
   {
       {
           AnscTraceWarning(("Triggering abortReboot from TR181\n"));
           // static collection as we don't want upgrade path to be changed without a reboot
           static BOOL rdkfwupgraderEnabledCollected = false;
           static BOOL RDKFWUpgraderEnabled = false;

           if (!rdkfwupgraderEnabledCollected) {
               if (RDKFirmwareUpgrader_GetParamBoolValue(hInsContext,
                                                         "Enable",
                                                         &RDKFWUpgraderEnabled) ) {
                   rdkfwupgraderEnabledCollected = true;
               }
           }

           // NOTE:: this might have an addional issue, rfc enabled but rdkfwupgrader was disabled at
           // build time. Do we need to take care of that like checking for presense /usr/bin/rdkfwupgrader
           // at runtime
           if (RDKFWUpgraderEnabled) {
               v_secure_system("/lib/rdk/rdkfwupgrader_abort_reboot.sh &");
               return TRUE; //always true, let the statemachine decide if there is a reboot operation pending or not.
           } else {
               FILE *file = NULL;
               FILE *Abortfile = NULL;
               if ((file = fopen("/tmp/.deferringreboot", "r"))) {
                   if ((Abortfile = fopen("/tmp/AbortReboot", "r"))) {
                       fclose(Abortfile);
                       fclose(file);
                       CcspTraceWarning(("Abort already done '%s'\n", ParamName));
                       return TRUE;
                   }
                   pMyObject->AbortReboot = bValue;
                   if(pMyObject->AbortReboot == TRUE)
                       v_secure_system("touch /tmp/AbortReboot");
                   else
                       CcspTraceWarning(("Parameter '%s' set to false\n", ParamName));
                   fclose(file);
                   return TRUE;
               } else {
                   CcspTraceWarning(("Invalid request for parameter, no FW DL reboot wait in progress '%s'\n", ParamName));
                   return FALSE;
               }
           }
       }
       return FALSE;
   } 

	if (strcmp(ParamName, "FirmwareDownloadCompletedNotification") == 0)
	{
		char notifyEnable[64];
		memset(notifyEnable, 0, sizeof(notifyEnable));

		syscfg_get( NULL, "ManageableNotificationEnabled", notifyEnable, sizeof(notifyEnable));
		if((notifyEnable[0] != '\0') && (strncmp(notifyEnable, "true", strlen("true")) == 0))
		{
			/* collect value */
			char buff[8];

			memset(buff, 0, sizeof(buff));
			snprintf(buff, sizeof(buff), "%s", bValue ? "true" : "false");
			pMyObject->FirmwareDownloadCompletedNotification = bValue;
			char *start_time = get_firmware_download_start_time();
			Send_Notification_Task(NULL, start_time, buff, "firmware-download-completed", NULL, NULL, NULL, NULL);
		}
		else
		{
			CcspTraceWarning(("ManageableNotificationEnabled is false, firmware download completed notfication is not sent\n"));
		}
		return TRUE;
	}
	/* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
	return FALSE;
}
#endif

static int write_hashedID_to_file(const cJSON *cjson)
{
    int ret = FALSE;
    char *hashed_id_buf = NULL;
    int idx;
    cJSON *item = cJSON_GetObjectItem(cjson, "hashed_ids");

    CcspTraceInfo(("The whole hashed ID msg is:\n %s \n", cJSON_Print(item)));

    if ( item != NULL )
    {
        ret = TRUE;
        FILE *fp = fopen("/tmp/hashed_id", "w");

        if(!fp)
        {
            CcspTraceError(("%s unable to create /tmp/hashed_id file \n",__FUNCTION__));
            ret = FALSE;
        }
        else
        {
            for(idx=0; idx < cJSON_GetArraySize(item); idx++)
            {
                cJSON * subitem = cJSON_GetArrayItem(item, idx);
                hashed_id_buf = subitem->valuestring;
                if(hashed_id_buf == NULL)
                {
                    CcspTraceError(("Unable to get the hashed id. \n"));
                    ret = FALSE;
                    break;
                }

                fprintf(fp,"%s\n",hashed_id_buf);
            }
            fclose(fp);	   
        }

        if(ret != FALSE)
        {
            CcspTraceInfo(("Successfully write hashed ids to /tmp/hashed_id file. \n"));
        }
    }

    return ret;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        MessageBusSource_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
MessageBusSource_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
 UNREFERENCED_PARAMETER(hInsContext);
 if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "MessageBusSource", value, sizeof(value)) == 0 )
        {
            /* CID: 63644 Array compared against 0*/
                 if (strcmp(value, "true") == 0)
                     *pBool = TRUE;
                 else
                     *pBool = FALSE;

            return TRUE;
        }
        else
        {
            CcspTraceError(("syscfg_get failed for MessageBusSource\n"));
        }
    }
  return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        MessageBusSource_SetParamBoolValu
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
MessageBusSource_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
  if (IsBoolSame(hInsContext, ParamName, bValue, MessageBusSource_GetParamBoolValue))
        return TRUE;

  if (strcmp(ParamName, "Enable") == 0)
    {
        if( syscfg_set_commit(NULL, "MessageBusSource", bValue ? "true" : "false") != 0 )
        {
            CcspTraceError(("syscfg_set failed for MessageBusSource\n"));
        }
        else
        {
            return TRUE;
        }

    }
  return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        MTLS_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
MTLS_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
 UNREFERENCED_PARAMETER(hInsContext);
 if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "T2mTLSEnable", value, sizeof(value)) == 0 )
        {
                 if (strcmp(value, "true") == 0)
                     *pBool = TRUE;
                 else
                     *pBool = FALSE;

            return TRUE;
        }
        else
        {
            CcspTraceError(("syscfg_get failed for MTLS\n"));
        }
    }
  return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        MTLS_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
MTLS_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
  if (IsBoolSame(hInsContext, ParamName, bValue, MTLS_GetParamBoolValue))
        return TRUE;

  if (strcmp(ParamName, "Enable") == 0)
    {
        if( syscfg_set_commit(NULL, "T2mTLSEnable", bValue ? "true" : "false") != 0 )
        {
            CcspTraceError(("syscfg_set failed for MTLS\n"));
        }
        else
        {
            return TRUE;
        }

    }
  return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        TR104_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.


**********************************************************************/
#ifdef MTA_TR104SUPPORT
BOOL
TR104_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "TR104enable", value, sizeof(value)) == 0 )
        {
		/* CID: 71977 Array compared against 0*/
		if (strcmp(value, "true") == 0)
			*pBool = TRUE;
		else
			*pBool = FALSE;
		return TRUE;
	}
        else
        {
            CcspTraceError(("syscfg_get failed for TR104enable\n"));
        }
    }
  return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        TR104_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
TR104_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
  if (IsBoolSame(hInsContext, ParamName, bValue, TR104_GetParamBoolValue))
        return TRUE;

  if (strcmp(ParamName, "Enable") == 0)
    {
        if( syscfg_set_commit(NULL, "TR104enable", bValue ? "true" : "false") != 0 )
        {
            CcspTraceError(("syscfg_set failed for TR104enable \n"));
        }
        else
        {
            return TRUE;
        }
    }
  return FALSE;
}
#endif

#if defined(FEATURE_MAPT) || defined(FEATURE_SUPPORT_MAPT_NAT46)
/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        MAPT_DeviceInfo_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.


**********************************************************************/

BOOL
MAPT_DeviceInfo_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "MAPT_Enable", value, sizeof(value)) == 0 )
        {
		if (strcmp(value, "true") == 0)
			*pBool = TRUE;
		else
			*pBool = FALSE;
		return TRUE;
	}
        else
        {
            CcspTraceError(("syscfg_get failed for MAPT_Enable\n"));
        }
    }
  return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        MAPT_DeviceInfo_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
MAPT_DeviceInfo_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
  if (IsBoolSame(hInsContext, ParamName, bValue, MAPT_DeviceInfo_GetParamBoolValue))
        return TRUE;

  if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "MAPT_Enable", bValue ? "true" : "false") != 0 )
        {
            CcspTraceError(("syscfg_set failed for MAPT_Enable \n"));
        }
        else
        {
#ifdef FEATURE_MAPT
            v_secure_system("sysevent set MAPT_Enable %s", bValue ? "true" : "false");
#endif
            return TRUE;
        }
    }
  return FALSE;
}
#endif

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTestEnable_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.


**********************************************************************/
#ifdef COLUMBO_HWTEST
BOOL
HwHealthTestEnable_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{

    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "hwHealthTest", value, sizeof(value)) == 0 )
        {
            if( value != NULL )
            {
                 if (strncmp(value, "true", sizeof(value)) == 0)
                     *pBool = TRUE;
                 else
                     *pBool = FALSE;
            }
            return TRUE;
        }
        else
        {
            CcspTraceError(("syscfg_get failed for hwHealthTest Enable\n"));
        }
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTestEnable_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
HwHealthTestEnable_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    if (IsBoolSame(hInsContext, ParamName, bValue, HwHealthTestEnable_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if( syscfg_set_commit(NULL, "hwHealthTest", bValue ? "true" : "false") != 0 )
        {
            CcspTraceError(("syscfg_set failed for hwHealthTest Enable \n"));
        }
        else
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTest_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            )



    description:

        This function is called to retrieve unsigned long parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned unsigned long value;

    return:     TRUE if succeeded.


**********************************************************************/

BOOL
HwHealthTest_GetParamUlongValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    if (strcmp(ParamName, "cpuThreshold") == 0)
    {
        *puLong = pMyObject->HwHealtTestPTR.CPUThreshold;
        if(0 == *puLong)
        {
            //Value is not set. Hence using default value
            *puLong = DEFAULT_HWST_PTR_CPU_THRESHOLD;
        }
        return TRUE;
    }

    if (strcmp(ParamName, "dramThreshold") == 0)
    {
        *puLong = pMyObject->HwHealtTestPTR.DRAMThreshold;
        if(0 == *puLong)
        {
            //Value is not set. Hence using default value
            *puLong = DEFAULT_HWST_PTR_DRAM_THRESHOLD;
        }
        return TRUE;
    }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTest_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uLong
            )


    description:

        This function is called to set ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uLong
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
HwHealthTest_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uLong
    )
{
    /*Changes for RDKB-31737: AC#2 and AC#3
      To reject the set request from any API other than RFC and dmcli
    */
    if(strcmp(getRequestorString(), BS_SOURCE_RFC_STR))
    {
        return FALSE;
    }

    if(GetAvailableSpace_tmp() < HWSELFTEST_START_MIN_SPACE)
    {
        CcspTraceError(("\nNot enough space in DRAM to save the threshold value in .hwselftest_settings file. Exit\n"));
        return FALSE;
    }

    //Check if the new value is same as the old. If so, it is
    //not required to update settings file.
    if (IsUlongSame(hInsContext, ParamName, uLong, HwHealthTest_GetParamUlongValue))
        return TRUE;

    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    if (strcmp(ParamName, "cpuThreshold") == 0)
    {
        //Threshold value should be in range as per the requirement in COLBO-132
        if( (uLong >= 1) && ( uLong <= 95))
        {
            pMyObject->HwHealtTestPTR.CPUThreshold = uLong;

            //Write the parameter to settings file
            char buf[8] = {'\0'};
            snprintf(buf, sizeof(buf), "%d", (int)uLong);
            UpdateSettingsFile("HWST_CPU_THRESHOLD=", buf);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

    if (strcmp(ParamName, "dramThreshold") == 0)
    {
        //Threshold value should be in range as per the requirement in COLBO-132
        if( uLong >= 20 )
        {
            pMyObject->HwHealtTestPTR.DRAMThreshold = uLong;

            char buf[8] = {'\0'};
            snprintf(buf, sizeof(buf), "%d", (int)uLong);
            UpdateSettingsFile("HWST_DRAM_THRESHOLD=",buf);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTestPTREnable_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.


**********************************************************************/

BOOL
HwHealthTestPTREnable_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "enable") == 0)
    {
        PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
        *pBool = pMyObject->HwHealtTestPTR.PTREnable;
        return TRUE;
    }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTestPTREnable_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
HwHealthTestPTREnable_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    /*Changes for RDKB-31737: AC#1 and AC#3
      To reject the set request from any API other than RFC and dmcli
    */
    if(strcmp(getRequestorString(), BS_SOURCE_RFC_STR))
    {
        return FALSE;
    }

    if(GetAvailableSpace_tmp() < HWSELFTEST_START_MIN_SPACE)
    {
        CcspTraceError(("\nNot enough space in DRAM to enable PTR. Exit\n"));
        return FALSE;
    }

    if (IsBoolSame(hInsContext, ParamName, bValue, HwHealthTestPTREnable_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "enable") == 0)
    {
        PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

        char buf[8] = {'\0'};
        errno_t rc  = -1;
        snprintf(buf, sizeof(buf), "%s", bValue ? "true" : "false");

        //Read client version
        FILE* fp = fopen("/tmp/.hwst_run", "r");
        char* clientVer = (char*)malloc(8*sizeof(char));
        char version[8] = {'\0'};
        if( NULL != fp)
        {
            if( NULL != clientVer )
            {
                fscanf(fp, "%s", clientVer);
                rc = strcpy_s(version,sizeof(version),clientVer);
                if(rc != EOK)
                {
                  ERR_CHK(rc);
                  free(clientVer);
                  fclose(fp);
                  return FALSE;
                }
                free(clientVer);
                clientVer = NULL;
            }
        }
        if( NULL != clientVer )
        {
            free(clientVer);
        }

        if( (NULL !=fp) && strcmp(version, "0002") && !strcmp(buf,"true"))
        {
            CcspTraceError(("\nMultiple connection not allowed"));
            fclose(fp);
            return FALSE;
        }

        pMyObject->HwHealtTestPTR.PTREnable = bValue;

        //If the PTR is enabled, add the hwselftest to crontab
        if(!strcmp(buf, "true"))
        {
            if(strcmp(version, "0002"))
            {
                char cmd[128] = {0};
                rc = strcpy_s(cmd, sizeof(cmd), "/usr/bin/hwselftest_cronjobscheduler.sh true &");
                if(rc != EOK)
                {
                   ERR_CHK(rc);
                   return FALSE;
                }
                CcspTraceInfo(("\nExecuting command: %s\n", cmd));
                v_secure_system("/usr/bin/hwselftest_cronjobscheduler.sh true &");
            }
            else
            {
                CcspTraceInfo(("Hwselftest service already running through PTR"));
            }
        }
        else
        {
            //Remove all the hwselftest job from crontab
            v_secure_system("/usr/bin/hwselftest_cronjobscheduler.sh false &");
        }
        return TRUE;
   }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTestPTRFrequency_GetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG*                      puLong
            )



    description:

        This function is called to retrieve unsigned long parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG*                      puLong
                The buffer of returned unsigned long value;

    return:     TRUE if succeeded.


**********************************************************************/

BOOL
HwHealthTestPTRFrequency_GetParamUlongValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "frequency") == 0)
    {
        PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
        *puLong = pMyObject->HwHealtTestPTR.Frequency;
        if(0 == *puLong)
        {
            //Value is not set. Hence using default value
            *puLong = DEFAULT_HWST_PTR_FREQUENCY;
        }
        return TRUE;
    }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        HwHealthTestPTRFrequency_SetParamUlongValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                ULONG                       uLong
            )


    description:

        This function is called to set ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                ULONG                       uLong
                The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
HwHealthTestPTRFrequency_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uLong
    )
{
    /*Changes for RDKB-31737: AC#1 and AC#3
      To reject the set request from any API other than RFC and dmcli
    */
    if(strcmp(getRequestorString(), BS_SOURCE_RFC_STR))
    {
        return FALSE;
    }

    if(GetAvailableSpace_tmp() < HWSELFTEST_START_MIN_SPACE)
    {
        CcspTraceError(("\nNot enough space in DRAM to save the value for PTR frequency in .hwselftest_settings file. Exit\n"));
        return FALSE;
    }

    //Check if the new value is same as the old. If so, it is
    //not required to update settings file.
    if (IsUlongSame(hInsContext, ParamName, uLong, HwHealthTestPTRFrequency_GetParamUlongValue))
        return TRUE;

    if (strcmp(ParamName, "frequency") == 0)
    {
        PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

        // Frequency should be minimum 2 minutes as per the requirement in COLBO 132.
        if( uLong >= 2 )
        {
            pMyObject->HwHealtTestPTR.Frequency = uLong;

            char buf[8] = {'\0'};
            snprintf(buf, sizeof(buf), "%d", (int)uLong);
            UpdateSettingsFile("HWST_PERIODIC_FREQ=", buf);

            // Call the cronjob scheduler.sh script to update the cron job

            //Read the PTR enable param
            if (IsBoolSame(hInsContext, "enable", true, HwHealthTestPTREnable_GetParamBoolValue))
            {
                CcspTraceInfo(("\n\nExecuting the command: /usr/bin/hwselftest_cronjobscheduler.sh true frequencyUpdate"));
                v_secure_system("/usr/bin/hwselftest_cronjobscheduler.sh true frequencyUpdate");
            }
            else
            {
                CcspTraceInfo(("\n\nExecuting the command: /usr/bin/hwselftest_cronjobscheduler.sh false"));
                v_secure_system("/usr/bin/hwselftest_cronjobscheduler.sh false" );
            }
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    return FALSE;
}
#endif

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        Telemetry_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
Telemetry_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool) {
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0) {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "T2Enable", value, sizeof(value)) == 0 ) {
		/*CID: 60562 Array compared against 0*/
		if (strcmp(value, "true") == 0)
			*pBool = TRUE;
		else
			*pBool = FALSE;
	} else {
            CcspTraceError(("syscfg_get failed for MessageBusSource\n"));
        }
        return TRUE;
    }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        Telemetry_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
Telemetry_SetParamBoolValue (ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue) {
    if (IsBoolSame(hInsContext, ParamName, bValue, Telemetry_GetParamBoolValue))
        return TRUE;

    if (strcmp(ParamName, "Enable") == 0) {
        if (syscfg_set(NULL, "T2Enable", bValue ? "true" : "false") != 0) {
            CcspTraceError(("syscfg_set failed for Telemetry.Enable\n"));
        } else {
            if (syscfg_set_commit(NULL, "T2Version", bValue ? "2.0.1" : "1") != 0) {
                CcspTraceError(("syscfg_set failed\n"));
            } else {
                return TRUE;
            }
        }

    }
    return FALSE;
}



/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        TelemetryEndpoint_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue
                The string value buffer;


    return:     TRUE if succeeded;
                FALSE if not supported.

**********************************************************************/
ULONG
Telemetry_GetParamStringValue (ANSC_HANDLE hInsContext, char* ParamName, char* pValue,
                               ULONG* pUlSize) {
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pUlSize);
    errno_t   rc  = -1;
    /* Required for xPC sync */
    if (strcmp(ParamName, "ConfigURL") == 0) {
        /* collect value */
        char buf[128] = {'\0'};
        /* CID: 68487 Array compared against 0*/
        /* CID: 63245 Logically dead code*/
        if(!syscfg_get(NULL, "T2ConfigURL", buf, sizeof(buf)))
        {
            rc = strcpy_s(pValue, *pUlSize, buf);
            if(rc != EOK)
            {
               ERR_CHK(rc);
               return -1;
            }
            return 0;
        }
        return -1;
    }

    if (strcmp(ParamName, "Version") == 0) {
        /* collect value */
        char buf[MAX_T2_VER_LEN] = {'\0'};
        /* CID: 68487 Array compared against 0*/
        if(!syscfg_get(NULL, "T2Version", buf, sizeof(buf)))
        {
            rc = strcpy_s(pValue, *pUlSize, buf);
            if(rc != EOK)
            {
               ERR_CHK(rc);
               return -1;
            }
            return 0;
        }
        return -1;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        Telemetry_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

        **********************************************************************/
BOOL
Telemetry_SetParamStringValue (ANSC_HANDLE hInsContext, char* ParamName, char* pString) {

    if (IsStringSame(hInsContext, ParamName, pString, Telemetry_GetParamStringValue))
        return TRUE;

    if (strcmp(ParamName, "ConfigURL") == 0) {
        if (syscfg_set_commit(NULL, "T2ConfigURL", pString) != 0) {
            CcspTraceError(("syscfg_set failed\n"));

        } else {
            return TRUE;
        }
    }

    if (strcmp(ParamName, "Version") == 0) {

        if ((strncmp(pString, "2", MAX_T2_VER_LEN) == 0) || (strncmp(pString, "2.0.1", MAX_T2_VER_LEN) == 0)) {
            if (syscfg_set_commit(NULL, "T2Version", pString) != 0) {
                CcspTraceError(("syscfg_set failed\n"));
            } else {
                return TRUE;
            }
        } else {
            CcspTraceError(("Unsupported version value : %s \n", pString ));
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        CaptivePortalForNoCableRF_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            )



    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

#if defined (_XB6_PRODUCT_REQ_)
BOOL
CaptivePortalForNoCableRF_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
 if (strcmp(ParamName, "Enable") == 0)
    {
	 char value[8];
         /* CID: 92210 Array compared against 0*/
	 if(!syscfg_get(NULL,"enableRFCaptivePortal",value, sizeof(value)))
         {
		 if (strcmp(value, "true") == 0)
			 *pBool = TRUE;
		 else
	  		 *pBool = FALSE;
         } else 
            return FALSE;

	 return TRUE;
    }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:
        BOOL
        CaptivePortalForNoCableRF_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            )


    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
CaptivePortalForNoCableRF_SetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
  UNREFERENCED_PARAMETER(hInsContext);

  if (strcmp(ParamName, "Enable") == 0)
    {
	  if (syscfg_set_commit(NULL, "enableRFCaptivePortal", (bValue == TRUE) ? "true" : "false") != 0)
	  {
		 AnscTraceWarning(("syscfg_set enableRFCaptivePortal failed\n"));
	  }
	  
	  return TRUE;
    }
    return FALSE;
}
#endif

/**
 *  RFC Features SecureWebUI
*/

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SecureWebUI_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
SecureWebUI_GetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8];

	/* check the parameter name and return the corresponding value */

    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        /* CID: 68633 Array compared against 0*/
        if(!syscfg_get( NULL, "SecureWebUI_Enable", buf, sizeof(buf)))
        {
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
            return TRUE;
        } else {
              return FALSE;
        }
    }
    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SecureWebUI_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       bValue
            );

    description:

        This function is called to set Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       bValue
                The buffer with updated value

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SecureWebUI_SetParamBoolValue

(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "SecureWebUI_Enable", (bValue == TRUE) ? "true" : "false") != 0)
        {
            AnscTraceWarning(("syscfg_set failed\n"));
        }
        return TRUE;
    }

    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SecureWebUI_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue
                The string value buffer;


    return:     0 if succeeded;
                -1 if not supported.

**********************************************************************/
ULONG
    SecureWebUI_GetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                       pValue,
 ULONG*                      pUlSize
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(pUlSize);
    errno_t  rc = -1;
    /* Required for xPC sync */
    if (strcmp(ParamName, "LocalFqdn") == 0)
    {
        /* collect value */
        char buf[64];
         /*CID: 59203 Array compared against 0*/
        if(!syscfg_get( NULL, "SecureWebUI_LocalFqdn", buf, sizeof(buf)))
        {
            rc = strcpy_s(pValue, *pUlSize, buf);
            if(rc != EOK)
            {
               ERR_CHK(rc);
               return -1;
            }
        }
        return 0;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        SecureWebUI_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pString
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pString
                The updated string value to be set;

    return:     TRUE if succeeded.

	**********************************************************************/
BOOL
    SecureWebUI_SetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                       pString
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "LocalFqdn") == 0)
    {
        if (syscfg_set_commit(NULL, "SecureWebUI_LocalFqdn", pString) != 0)
        {
            CcspTraceError(("syscfg_set failed\n"));

        }
        else
        {
            return TRUE;
        }
    }

    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**
***  RFC Feature LnFUseXPKI
**/
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        LnFUseXPKI_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );
    description:
        This function is called to retrieve Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL*                       pBool
                The buffer of returned boolean value;
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
LnFUseXPKI_GetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if( (pBool != NULL) && (AnscEqualString(ParamName, "Enable", TRUE)))
    {
        char value[8];
        memset(value, 0, sizeof(value));
        if( syscfg_get(NULL, "LnFUseXPKI_Enable", value, sizeof(value)) == 0 ) {

            if (strncmp(value, "true", sizeof(value)) == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;

            return TRUE;
        } else {
              CcspTraceError(("syscfg_get failed for MessageBusSource\n"));
        }
    }
    return FALSE;
}
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        LnFUseXPKI_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );
    description:
        This function is called to set Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL                        bValue
                The buffer with updated value
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
LnFUseXPKI_SetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        if (syscfg_set_commit(NULL, "LnFUseXPKI_Enable", (bValue == FALSE) ? "false" : "true") != 0) {
            AnscTraceWarning(("syscfg_set failed\n"));
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}


/**
***  RFC Feature UseXPKI
**/
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        UseXPKI_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );
    description:
        This function is called to retrieve Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL*                       pBool
                The buffer of returned boolean value;
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
UseXPKI_GetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if( (pBool != NULL) && (AnscEqualString(ParamName, "Enable", TRUE)))
    {
        char value[8];
        memset(value, 0, sizeof(value));
        if( syscfg_get(NULL, "UseXPKI_Enable", value, sizeof(value)) == 0 ) {

            if (strncmp(value, "true", sizeof(value)) == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;

            return TRUE;
        } else {
              CcspTraceError(("syscfg_get failed for MessageBusSource\n"));
        }
    }
    return FALSE;
}
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        UseXPKI_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );
    description:
        This function is called to set Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL                        bValue
                The buffer with updated value
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
UseXPKI_SetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        if (syscfg_set_commit(NULL, "UseXPKI_Enable", (bValue==FALSE)?"false":"true") != 0) {
            AnscTraceWarning(("syscfg_set failed\n"));
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}

/**
 *  RFC Feature mTlsLogUpload
*/
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        mTlsLogUpload_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );
    description:
        This function is called to retrieve Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL*                       pBool
                The buffer of returned boolean value;
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
mTlsLogUpload_GetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if( (pBool != NULL) && (strcmp(ParamName, "Enable") == 0))
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "mTlsLogUpload_Enable", value, sizeof(value)) == 0 ) {
         /* CID: 133796 Array compared against 0*/
            if (strncmp(value, "true", sizeof(value)) == 0)
                 *pBool = TRUE;
            else
                 *pBool = FALSE;
        } else {
              CcspTraceError(("syscfg_get failed for MessageBusSource\n"));
          }
          return TRUE;
        }
    return FALSE;
}
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        mTlsLogUpload_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );
    description:
        This function is called to set Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL                        bValue
                The buffer with updated value
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
mTlsLogUpload_SetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        if (syscfg_set_commit(NULL, "mTlsLogUpload_Enable", (bValue==FALSE)?"false":"true") != 0) {
            AnscTraceWarning(("syscfg_set failed\n"));
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}        
/**
 *  RFC Feature mTlsCrashdumpUpload
*/
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        mTlsCrashdumpUpload_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );
    description:
        This function is called to retrieve Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL*                       pBool
                The buffer of returned boolean value;
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
mTlsCrashdumpUpload_GetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if( (pBool != NULL) && (AnscEqualString(ParamName, "Enable", TRUE)))
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "mTlsCrashdumpUpload_Enable", value, sizeof(value)) == 0 ) {
            
            if (strncmp(value, "true", sizeof(value)) == 0)
                 *pBool = TRUE;
            else
                 *pBool = FALSE;

            return TRUE;
        } else {
              CcspTraceError(("syscfg_get failed for MessageBusSource\n"));
          }
        }
    return FALSE;
}
/**********************************************************************
    caller:     owner of this object
    prototype:
        BOOL
        mTlsCrashdumpUpload_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );
    description:
        This function is called to set Boolean parameter value;
    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;
                char*                       ParamName,
                The parameter name;
                BOOL                        bValue
                The buffer with updated value
    return:     TRUE if succeeded.
**********************************************************************/
BOOL
mTlsCrashdumpUpload_SetParamBoolValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        /* collect value */
        if (syscfg_set_commit(NULL, "mTlsCrashdumpUpload_Enable", (bValue == FALSE) ? "false" : "true") != 0) {
            AnscTraceWarning(("syscfg_set failed\n"));
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
    return FALSE;
}
/**
 *  RFC Feature XHFW
*/
/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        XHFW_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
#if defined(_XB6_PRODUCT_REQ_) || defined(_XB7_PRODUCT_REQ_)
BOOL
XHFW_GetParamBoolValue ( ANSC_HANDLE hInsContext, char* ParamName, BOOL* pBool)
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "XHFW_Enable", value, sizeof(value)) == 0 )
        {
            /* CID: 155008 Array name value  compared against 0*/
            *pBool = (strcmp(value, "true") == 0) ? TRUE : FALSE;
            return TRUE;
        }
        else
        {
            CcspTraceError(("syscfg_get failed for XHFW.Enable\n"));
        }
    }
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        XHFW_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
XHFW_SetParamBoolValue (ANSC_HANDLE hInsContext, char* ParamName, BOOL bValue)
{
    UNREFERENCED_PARAMETER(hInsContext);
    BOOL result = FALSE;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "XHFW_Enable", bValue ? "true" : "false") != 0)
        {
            CcspTraceError(("syscfg_set failed for XHFW.Enable\n"));
        }
        else
        {
            result = TRUE;
        }

        if (bValue)
        {
            v_secure_system("systemctl start zilker");
        }
        else
        {
            v_secure_system("systemctl stop zilker");
        }
    }
    return result;
}
#endif

static void Replace_AllOccurrence(char *str, int size, char ch, char Newch)
{
  int i=0;
  for(i = 0; i<size-1; i++)
  {
      if(str[i] == ch || str[i] == '\r')
      {
         str[i] = Newch;
      }
  }
  str[i]='\0';
}

ULONG
NonRootSupport_GetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                       pValue,
 ULONG*                      pUlSize
)
{
  UNREFERENCED_PARAMETER(hInsContext);
  #define APPARMOR_BLOCKLIST_FILE "/opt/secure/Apparmor_blocklist"
  #define APPARMOR_PROFILE_DIR "/etc/apparmor.d"
  #define SIZE_LEN 32
  char *buf = NULL;
  FILE *fp = NULL;
  size_t len = 0;
  ssize_t read = 0;
  DIR *dir=NULL;
  struct dirent *entry=NULL;
  char tmp[64]={0};
  char files_name[MAX_SIZE]={0};
  char *sptr=NULL;
  /* check the parameter name and return the corresponding value */
  if (strcmp(ParamName, "ApparmorBlocklist") == 0)
  {
      dir = opendir(APPARMOR_PROFILE_DIR);
      if( (dir == NULL) ) {
            CcspTraceError(("Failed to open the %s directory: profiles does not exist\n", APPARMOR_PROFILE_DIR));
            return FALSE;
      }
      memset(files_name,'\0',sizeof(files_name));
      while((entry = readdir(dir)) != NULL) {
             strncat(files_name,entry->d_name,strlen(entry->d_name));
      }
      closedir(dir);
      fp=fopen(APPARMOR_BLOCKLIST_FILE,"r");
      if(fp != NULL) {
         while((read = getline(&buf, &len, fp)) != -1) {
             strncpy(tmp,buf,sizeof(tmp));
             strtok_r(buf,":",&sptr);
             if(buf != NULL) {
                if(strstr(files_name, buf) != NULL )  {
                   strncat(pValue,tmp,strlen(tmp));
                }
             }
         }
         fclose(fp);
         Replace_AllOccurrence( pValue, AnscSizeOfString(pValue), '\n', ',');
         CcspTraceWarning(("Apparmor profile configuration:%s\n", pValue));
         return 0;
      }
      else {
         CcspTraceWarning(("%s does not exist\n", APPARMOR_BLOCKLIST_FILE));
         strncpy(pValue,"Apparmorblocklist is empty",SIZE_LEN);
      }
  }
  //Blocklist RFC
  if (strcmp(ParamName, "Blocklist") == 0)
  {
      fp = fopen(BLOCKLIST_FILE,"r");
      if(fp!=NULL) {
         read = getdelim( &buf, &len, '\0', fp);
         if (read != -1) {
             AnscCopyString(pValue, buf);
             *pUlSize = AnscSizeOfString(pValue);
             Replace_AllOccurrence( pValue, *pUlSize, '\n', ',');
             CcspTraceWarning(("Blocklist processes:%s\n", pValue));
         }
         return 0;
      }
      else if((fp == NULL) || (isspace(buf[0])) || (buf == NULL)) {
         CcspTraceWarning(("Blocklist file does not exist or no process to be blocklisted\n"));
         strncpy(pValue,"No blocklisted process",SIZE_LEN);
      }
  }
  return -1;
}

static BOOL ValidateInput_Arguments(char *input, FILE *tmp_fptr)
{
  #define APPARMOR_PROFILE_DIR "/etc/apparmor.d"
  #define BUF_SIZE 64
  struct dirent *entry=NULL;
  DIR *dir=NULL;
  char files_name[1024]={0};
  char *token=NULL;
  char *subtoken=NULL;
  char *sub_string=NULL;
  char *sp=NULL;
  char *sptr=NULL;
  char tmp[BUF_SIZE]={0};
  char *arg=NULL;
  dir=opendir(APPARMOR_PROFILE_DIR);
  if( (dir == NULL) || (tmp_fptr == NULL) ) {
     CcspTraceError(("Failed to open the %s directory\n", APPARMOR_PROFILE_DIR));
     return FALSE;
  }
  memset(files_name,'\0',sizeof(files_name));
  /* storing Apparmor profile (file) names into files_name which can be used to check with input arguments using strstr() */
  while((entry = readdir(dir)) != NULL) {
        strncat(files_name,entry->d_name,strlen(entry->d_name));
  }
  if (closedir(dir) != 0) {
      CcspTraceError(("Failed to close %s directory\n", APPARMOR_PROFILE_DIR));
      fclose(tmp_fptr);
      return FALSE;
  }
  /* Read the input arguments and ensure the corresponding profiles exist or not by searching in
     Apparmor profile directory (/etc/apparmor.d/). Returns false if input does not have the
     apparmor profile, Returns true if apparmor profile finds for the input */
  token=strtok_r( input,",", &sp);
  while(token != NULL) {
        arg=strchr(token,':');
        if ( ( (strcmp(arg+1,"disable") != 0) && (strcmp(arg+1,"complain") != 0) && (strcmp(arg+1,"enforce") != 0) ) ) {
              CcspTraceWarning(("Invalid arguments in the parser:%s\n", token));
              return FALSE;
        }
        strncpy(tmp,token,sizeof(tmp));
        subtoken=strtok_r(tmp,":",&sptr);
        if(subtoken != NULL) {
           sub_string=strstr(files_name, subtoken);
           if(sub_string != NULL) {
              fprintf(tmp_fptr,"%s\n",token);
           }
           else {
              CcspTraceWarning(("Invalid arguments %s error found in the parser\n", subtoken));
              return FALSE;
           }
        }
  token=strtok_r(NULL,",",&sp);
  }
  return TRUE;
}

BOOL
NonRootSupport_SetParamStringValue
(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 char*                        pValue
 )
{
  UNREFERENCED_PARAMETER(hInsContext);
  #define APPARMOR_BLOCKLIST_FILE "/opt/secure/Apparmor_blocklist"
  #define TMP_FILE "/opt/secure/Apparmor_blocklist_bck.txt"
  #define SIZE 128
  #define MAX_SIZE 1024
  FILE *fptr = NULL;
  FILE *tmp_fptr = NULL;
  char *boxType = NULL, *atomIp = NULL;
  if (strcmp(ParamName, "ApparmorBlocklist") == 0)
  {
     fptr = fopen(APPARMOR_BLOCKLIST_FILE,"r");
     tmp_fptr = fopen(TMP_FILE,"w+");
     if( (!pValue) || (strstr(pValue,":") == NULL) || (tmp_fptr == NULL) ) {
         CcspTraceError(("Failed to open the file or invalid argument\n"));
         if(fptr)
            fclose(fptr);
         if(tmp_fptr)
            fclose(tmp_fptr);
         return FALSE;
     }
     /* To ensure input arguments are valid or not */
     if (ValidateInput_Arguments(pValue, tmp_fptr) != TRUE) {
         return FALSE;
     }
     /* Copying tmp file contents into main file by using rename() */
     if(fptr != NULL)
        fclose(fptr);
     fclose(tmp_fptr);
     if(rename( TMP_FILE, APPARMOR_BLOCKLIST_FILE) != 0) {
        CcspTraceError(("Error in renaming  file\n"));
        return FALSE;
     }
     return TRUE;
  }

  //Blocklist RFC
  if (strcmp(ParamName, "Blocklist") == 0)
  {
      char buf[MAX_SIZE] = {'\0'};
      snprintf(buf,sizeof(buf),"%s",pValue);
      fptr = fopen(BLOCKLIST_FILE,"w+");
      boxType=getenv("BOX_TYPE");
      if(boxType != NULL)
      {
         if(strcmp(boxType, "XB3") ==0)
         {
            atomIp=getenv("ATOM_ARPING_IP");
            if(atomIp != NULL)
            {
               v_secure_system("/usr/bin/rpcclient %s \"echo '%s' > /nvram/Blocklist_XB3.txt\"", atomIp, buf);
            }
         }
      }
      if(!fptr){
      CcspTraceError(("%s failed to open %s file \n",__FUNCTION__,BLOCKLIST_FILE));
      return FALSE;
      }
      fprintf(fptr, "%s\n", buf);
      fclose(fptr);
      return TRUE;
  }
  CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName));
  return FALSE;
}

// Generic RFC get handlers
static char *Generic_GetParamJsonValue (void)
{
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    CcspTraceWarning(("g_currentParamFullName = %s\n", g_currentParamFullName));
    if (pMyObject->pRfcStore)
    {
        cJSON *valueObj = cJSON_GetObjectItem(pMyObject->pRfcStore, g_currentParamFullName);
        if (valueObj)
        {
            cJSON *value = cJSON_GetObjectItem(valueObj, "Value");
            if (value)
                return value->valuestring;
        }
        CcspTraceWarning(("Param %s not available in RFC store\n", g_currentParamFullName));
    }
    else
        CcspTraceWarning(("RFC store not present to retrieve %s\n", g_currentParamFullName));

    // If value is not present in RFC override store, find it in RFC defaults json.
    if (pMyObject->pRfcDefaults)
    {
        cJSON *value = cJSON_GetObjectItem(pMyObject->pRfcDefaults, g_currentParamFullName);
        if (value)
        {
            return value->valuestring;
        }
        CcspTraceWarning(("Param %s not available in RFC defaults\n", g_currentParamFullName));
    }
    else
        CcspTraceWarning(("RFC defaults not present to retrieve %s\n", g_currentParamFullName));

    return NULL;
}

static int StartsWith (const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

ULONG
Generic_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    if ( !pValue )
    {
        CcspTraceWarning(("pValue is NULL in Generic_GetParamStringValue\n"));
        return -1;
    }

    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    errno_t   rc  = -1;

    if (pMyObject->pRfcStore == NULL)
    {
        return 0;
    }

    char *strValue = Generic_GetParamJsonValue();
    if (strValue)
    {
        rc = strcpy_s( pValue, *pUlSize, strValue );
        if(rc != EOK)
        {
           ERR_CHK(rc);
           return -1;
        }
        CcspTraceWarning(("param = %s, value = %s\n", ParamName, pValue));
        return 0;
    }

    return -1;
}

BOOL
Generic_GetParamBoolValue

    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    char *value = Generic_GetParamJsonValue();
    if( value != NULL )
    {
        if (strcmp(value, "true") == 0)
            *pBool = TRUE;
        else
            *pBool = FALSE;

        return TRUE;
    }
    return FALSE;
}

BOOL
Generic_GetParamUlongValue
    (
        ANSC_HANDLE             hInsContext,
        char*                   ParamName,
        ULONG*                  pValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    UNREFERENCED_PARAMETER(ParamName);
    char *value = Generic_GetParamJsonValue();
    if( value != NULL )
    {
        *pValue = atoi(value);
        return TRUE;
    }
    return FALSE;
}

BOOL
Generic_GetParamIntValue
        (
                ANSC_HANDLE                             hInsContext,
                char*                                           ParamName,
                int*                                            pInt
        )
{
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    if (pMyObject->pRfcStore == NULL)
    {
        return 0;
    }

    char *value = Generic_GetParamJsonValue();
    if( value != NULL )
    {
        *pInt = atoi(value);
        return TRUE;
    }
    return FALSE;
}

// Generic RFC set handlers

BOOL IsBoolSame(ANSC_HANDLE hInsContext,char* ParamName, BOOL bValue, GETBOOL_FUNC_PTR getBoolFunc)
{
    Generic_SetParamBoolValue(hInsContext, ParamName, bValue);
    BOOL prevValue = false;
    getBoolFunc( hInsContext, ParamName, &prevValue );
    if (prevValue == bValue)
    {
        CcspTraceWarning(("%s values are same...\n", __FUNCTION__));
        return TRUE;
    }
    CcspTraceWarning(("%s values are different...\n", __FUNCTION__));
    return FALSE;
}

BOOL IsStringSame(ANSC_HANDLE hInsContext,char* ParamName, char* pValue, GETSTRING_FUNC_PTR getStringFunc)
{
    Generic_SetParamStringValue(hInsContext, ParamName, pValue);
    char prevValue[1024];
    ULONG size = 1024;
    getStringFunc( hInsContext, ParamName, (char *)&prevValue, &size );
    if ( strcmp(prevValue, pValue) == 0 )
    {
        CcspTraceWarning(("%s values are same...\n", __FUNCTION__));
        return TRUE;
    }
    CcspTraceWarning(("%s values are different...\n", __FUNCTION__));
    return FALSE;
}

BOOL IsUlongSame(ANSC_HANDLE hInsContext,char* ParamName, ULONG ulValue, GETULONG_FUNC_PTR getUlongFunc)
{
    Generic_SetParamUlongValue(hInsContext, ParamName, ulValue);
    ULONG prevValue = 0;
    getUlongFunc( hInsContext, ParamName, &prevValue );
    if (prevValue == ulValue)
    {
        CcspTraceWarning(("%s values are same...\n", __FUNCTION__));
        return TRUE;
    }
    CcspTraceWarning(("%s values are different...\n", __FUNCTION__));
    return FALSE;
}

BOOL IsIntSame(ANSC_HANDLE hInsContext,char* ParamName, int value, GETINT_FUNC_PTR getIntFunc)
{
    Generic_SetParamIntValue(hInsContext, ParamName, value);
    int prevValue = 0;
    getIntFunc( hInsContext, ParamName, &prevValue );
    if (prevValue == value)
    {
        CcspTraceWarning(("%s values are same...\n", __FUNCTION__));
        return TRUE;
    }
    CcspTraceWarning(("%s values are different...\n", __FUNCTION__));
    return FALSE;
}

BOOL
Generic_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    )
{
   AnscTraceWarning(("Generic_SetParamStringValue: param = %s, value = %s\n", ParamName, strValue));
   PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    char * requestorStr = getRequestorString();
    char * currentTime = getTime();
   ProcessRfcSet(&pMyObject->pRfcStore, g_clearDB, g_currentParamFullName, strValue, requestorStr, currentTime);
   return TRUE;
}

BOOL
Generic_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
   AnscTraceWarning(("Generic_SetParamBoolValue: param = %s\n", ParamName));
   PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

   char * requestorStr = getRequestorString();
   char * currentTime = getTime();

   if ( bValue == TRUE)
      ProcessRfcSet(&pMyObject->pRfcStore, g_clearDB, g_currentParamFullName, "true", requestorStr, currentTime);
   else
      ProcessRfcSet(&pMyObject->pRfcStore, g_clearDB, g_currentParamFullName, "false", requestorStr, currentTime);
   return TRUE;
}

BOOL
Generic_SetParamUlongValue
    (
        ANSC_HANDLE             hInsContext,
        char*                   ParamName,
        ULONG                   ulValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[64]={0};
    errno_t rc  = -1;

    rc = sprintf_s(buf, sizeof(buf), "%lu", ulValue);
    if(rc < EOK)
    {
      ERR_CHK(rc);
      return FALSE;
    }

   AnscTraceWarning(("Generic_SetParamIntValue: param = %s\n", ParamName));
   PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

   char * requestorStr = getRequestorString();
   char * currentTime = getTime();
   ProcessRfcSet(&pMyObject->pRfcStore, g_clearDB, g_currentParamFullName, buf, requestorStr, currentTime);
   return TRUE;
}

BOOL
Generic_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    )
{
    char buf[16]={0};
    errno_t rc  = -1;

    rc = sprintf_s(buf, sizeof(buf), "%d", value);
    if(rc < EOK)
    {
      ERR_CHK(rc);
      return FALSE;
    }

    AnscTraceWarning(("Generic_SetParamIntValue: param = %s\n", ParamName));

    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;
    char * requestorStr = getRequestorString();
    char * currentTime = getTime();
    ProcessRfcSet(&pMyObject->pRfcStore, g_clearDB, g_currentParamFullName, buf, requestorStr, currentTime);

    return TRUE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
       AutoReboot_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AutoReboot_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    )
{
    UNREFERENCED_PARAMETER(hInsContext);  

    if (strcmp(ParamName, "Enable") == 0)
    {
        CcspTraceInfo(("[%s :] AutoReboot Getparam Enable value\n",__FUNCTION__));
        char value[8] = {'\0'};
        if( syscfg_get(NULL, "AutoReboot", value, sizeof(value)) == 0 )
        {
            *pBool = (strcmp(value, "true") == 0) ? TRUE : FALSE;
            return TRUE;
        }
        else
        {
            CcspTraceError(("syscfg_get failed for AutoReboot.Enable\n"));
        } 
        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AutoReboot_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to set Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       bValue
                The buffer with updated value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AutoReboot_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    )
{   
    UNREFERENCED_PARAMETER(hInsContext);
    PCOSA_DATAMODEL_DEVICEINFO  pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    if (strcmp(ParamName, "Enable") == 0)
    {
        if (IsBoolSame(hInsContext, ParamName, bValue, AutoReboot_GetParamBoolValue))
        {
            CcspTraceInfo(("[%s:] AutoReboot Set current and previous values are same\n", __FUNCTION__ ));
            return TRUE;
        }
        if (syscfg_set_commit(NULL, "AutoReboot", bValue ? "true" : "false") != 0)
        {
            CcspTraceError(("syscfg_set failed for AutoReboot.Enable\n"));
        }
        CcspTraceInfo(("[%s:] AutoReboot Set param Enable value %d\n", __FUNCTION__, bValue));
        pMyObject->AutoReboot.Enable = bValue;
        CosaDmlScheduleAutoReboot( pMyObject->AutoReboot.UpTime, bValue );
        return TRUE;
    }
    /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AutoReboot_SetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int                         iValue
            );

    description:

        This function is called to set Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int                         iValue
                The buffer with updated value

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AutoReboot_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */    
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    if (strcmp(ParamName, "UpTime") == 0)
    {
        CcspTraceInfo(("[%s:] AutoReboot Set uptime \n", __FUNCTION__ ));
        if((1 > iValue || iValue > 365))
        {
            CcspTraceWarning(("The value is not in the expected range. keeping the previous value \n"));
            return TRUE;
        }
        if(pMyObject->AutoReboot.Enable )
        {
            if( pMyObject->AutoReboot.UpTime != iValue)
            {
                CosaDmlScheduleAutoReboot( iValue, true );
            }
        }
        pMyObject->AutoReboot.UpTime = iValue;
    
        return TRUE;
    }
   /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}
/**********************************************************************  

    caller:     owner of this object 

    prototype: 

        BOOL
        AutoReboot_GetParamIntValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                int*                        iValue
            );

    description:

        This function is called to set Boolean parameter value; 

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                int*                       iValue
                The buffer of returned int value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
AutoReboot_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        iValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    /* check the parameter name and set the corresponding value */   
    const int DEFAULT_UPTIME = 120;
    PCOSA_DATAMODEL_DEVICEINFO      pMyObject = (PCOSA_DATAMODEL_DEVICEINFO)g_pCosaBEManager->hDeviceInfo;

    if (strcmp(ParamName, "UpTime") == 0)
    {
        *iValue = pMyObject->AutoReboot.UpTime;
        if( (1 > *iValue || *iValue > 365) )
        {
            *iValue  = DEFAULT_UPTIME;
            pMyObject->AutoReboot.UpTime=DEFAULT_UPTIME;  
        }
        return TRUE;
    }
   /* CcspTraceWarning(("Unsupported parameter '%s'\n", ParamName)); */
    return FALSE;
}

/**
 *  RFC Features OCSP
*/

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EnableOCSPStapling_GetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL*                       pBool
            );

    description:

        This function is called to retrieve Boolean parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL*                       pBool
                The buffer of returned boolean value;

    return:     TRUE if succeeded.

**********************************************************************/

BOOL
EnableOCSPStapling_GetParamBoolValue

(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL*                       pBool
 )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "Enable") == 0)
    {
        char buf[8];
        /* collect value */
        if(!syscfg_get( NULL, "EnableOCSPStapling", buf, sizeof(buf))) {
        /* CID: 154685 Array compared against 0*/
            if (strcmp(buf, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        } else 
           return FALSE;

        return TRUE;                
    }

    if (strcmp(ParamName, "DirectOCSP") == 0)
    {
        char buf1[8];
        /* collect value */
        /* CID: 154685 Array compared against 0*/
        if(!syscfg_get( NULL, "EnableOCSPCA", buf1, sizeof(buf1)))
        {
            if (strcmp(buf1, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        } else 
           return FALSE;

        return TRUE;                
    }

    return FALSE;
}


/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        EnableOCSPStapling_SetParamBoolValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
            );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
EnableOCSPStapling_SetParamBoolValue

(
 ANSC_HANDLE                 hInsContext,
 char*                       ParamName,
 BOOL                        bValue
 )
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        if (syscfg_set_commit(NULL, "EnableOCSPStapling", bValue ? "true" : "false") != 0)
        {
            CcspTraceError(("syscfg_set EnableOCSPStapling failed\n"));
        }
        else
        {
            return TRUE;
        }
    }

    if (strcmp(ParamName, "DirectOCSP") == 0)
    {
        if (syscfg_set_commit(NULL, "EnableOCSPCA", bValue ? "true" : "false") != 0)
        {
            CcspTraceError(("syscfg_set EnableOCSPCA failed\n"));
        }
        else
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void copy_command_output (char *cmd, char *out, int len)
{
    FILE *fp;

    out[0] = 0;

    fp = popen (cmd, "r");
    if (fp)
    {
        if (fgets (out, len, fp) != NULL)
        {
            size_t len = strlen (out);
            if ((len > 0) && (out[len - 1] == '\n'))
                out[len - 1] = 0;
        }

        pclose (fp);
    }
}


/**********************************************************************

    caller:     owner of this object

    prototype:

	BOOL
	SelfHeal_GetParamUlongValue
	    (
		ANSC_HANDLE                 hInsContext,
		char*                       ParamName,
		ULONG*                      puLong
	    );

    description:

	This function is called to retrieve ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
		The instance handle;

		char*                       ParamName,
		The parameter name;

		ULONG*                      puLong
		The buffer of returned ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SelfHeal_GetParamUlongValue
(
    ANSC_HANDLE                 hInsContext,
    char*                       ParamName,
    ULONG*                      puLong
    )
{
    UNREFERENCED_PARAMETER(hInsContext);
    char buf[8] = {0};

    if (strcmp(ParamName, "AggressiveInterval") == 0)
    {
        syscfg_get( NULL, ParamName, buf, sizeof(buf));
        if( 0 == strlen(buf) )
            return FALSE;
        *puLong = atol(buf);
        return TRUE;
    }
    AnscTraceWarning(("%s is invalid argument!\n", ParamName));
    return FALSE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

	BOOL
	SelfHeal_SetParamUlongValue
	    (
		ANSC_HANDLE                 hInsContext,
		char*                       ParamName,
		ULONG                       uValue
	    );

    description:

	This function is called to set ULONG parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
		The instance handle;

		char*                       ParamName,
		The parameter name;

		ULONG                       uValue
		The updated ULONG value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
SelfHeal_SetParamUlongValue
(
    ANSC_HANDLE                 hInsContext,
    char*                       ParamName,
    ULONG                       uValue
    )
{
    UNREFERENCED_PARAMETER(hInsContext);

    if (strcmp(ParamName, "AggressiveInterval") == 0)
    {
        char buf[16];

        if (uValue < 2) /* Minimum interval is 2 as per the aggressive selfheal US [RDKB-25546] */
	{
	    AnscTraceWarning(("Minimum interval is 2 for %s !\n", ParamName));
	    return FALSE;
	}
#if defined(_ARRIS_XB6_PRODUCT_REQ_) || defined(_CBR_PRODUCT_REQ_) || \
(defined(_XB6_PRODUCT_REQ_) && defined(_COSA_BCM_ARM_))
	syscfg_get( NULL, "resource_monitor_interval", buf, sizeof(buf));
        if( 0 == strlen(buf) )
	{
	    AnscTraceWarning(("syscfg_get returns NULL for resource_monitor_interval !\n"));
	    return FALSE;
	}
	ULONG resource_monitor_interval = atol(buf);
	if (uValue >= resource_monitor_interval)
	{
	    CcspTraceWarning(("AggressiveInterval should be lesser than resource_monitor_interval\n"));
	    return FALSE;
	}
#endif
        if (syscfg_set_u_commit(NULL, ParamName, uValue) != 0)
        {
            AnscTraceWarning(("%s syscfg_set failed!\n", ParamName));
            return FALSE;
        }

        copy_command_output("pidof selfheal_aggressive.sh", buf, sizeof(buf));
        if (buf[0] != 0) {
          v_secure_system("kill -9 %s", buf);
        }
        v_secure_system("/usr/ccsp/tad/selfheal_aggressive.sh &");
    }
    else
    {
        AnscTraceWarning(("%s is invalid argument!\n", ParamName));
        return FALSE;
    }
    return TRUE;
}

/***********************************************************************

 APIs for Object:

    LANConfigSecurity.

    *  LANCfgSec_GetParamStringValue
    *  LANCfgSec_SetParamStringValue

***********************************************************************/
/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG
        LANCfgSec_GetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
                ULONG*                      pUlSize
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

                ULONG*                      pUlSize
                The buffer of length of string value;
                Usually size of 1023 will be used.
                If it's not big enough, put required size here and return 1;

    return:     0 if succeeded;
                1 if short of buffer size; (*pUlSize = required size)
                -1 if not supported.

**********************************************************************/
ULONG
LANCfgSec_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    )
{
    if (strcmp(ParamName, "ConfigPassword") == 0)
    {
        AnscCopyString(pValue, "");
        return 0;
    }
    return -1;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        LANCfgSec_SetParamStringValue
            (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                char*                       pValue,
            );

    description:

        This function is called to retrieve string parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                char*                       pValue,
                The string value buffer;

    return:     TRUE if succeeded;

**********************************************************************/
BOOL
LANCfgSec_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue
    )
{
    if (strcmp(ParamName, "ConfigPassword") == 0)
    {
        /*TODO: implementation if required*/
        return TRUE;
    }
    return FALSE;
}

#if defined(FEATURE_RDKB_NFC_MANAGER)
//NFC RFC :: NFC Feature should enable.

/***********************************************************************

   caller: owner of this object

   prototype:

       BOOL
       NFC_GetParamBoolValue
           (
               ANSC_HANDLE                 hInsContext,
               char*                       ParamName,
               BOOL*                       pBool
           );

   description:

       This function is called to retrieve Boolean parameter value;

   argument:   ANSC_HANDLE                 hInsContext,
               The instance handle;

               char*                       ParamName,
               The parameter name;

               BOOL*                       pBool
               The buffer of returned boolean value;

   return:     TRUE if succeeded.

**********************************************************************/
BOOL
NFC_GetParamBoolValue
(
ANSC_HANDLE                 hInsContext,
char*                       ParamName,
BOOL*                       pBool
)
{
    UNREFERENCED_PARAMETER(hInsContext);
    if (strcmp(ParamName, "Enable") == 0)
    {
        char nfc_status[8] = {0};
        if(!syscfg_get(NULL,"nfc_enabled",nfc_status, sizeof(nfc_status)))
        {
            CcspTraceInfo(("nfc_enabled = %s \n", nfc_status));
            if (strcmp(nfc_status, "true") == 0)
                *pBool = TRUE;
            else
                *pBool = FALSE;
        }else{
            CcspTraceError(("Failed to get `nfc_enabled` syscfg status \n"));
            return FALSE;
        }
    }
    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL
        NFC_SetParamBoolValue
        (
                ANSC_HANDLE                 hInsContext,
                char*                       ParamName,
                BOOL                        bValue
        );

    description:

        This function is called to set BOOL parameter value;

    argument:   ANSC_HANDLE                 hInsContext,
                The instance handle;

                char*                       ParamName,
                The parameter name;

                BOOL                        bValue
                The updated BOOL value;

    return:     TRUE if succeeded.

**********************************************************************/
BOOL
NFC_SetParamBoolValue
(
ANSC_HANDLE                 hInsContext,
char*                       ParamName,
BOOL                        bValue
)
{
    if (IsBoolSame(hInsContext, ParamName, bValue, NFC_GetParamBoolValue))
    {
        return TRUE;
    }
    if (strcmp(ParamName, "Enable") == 0)
    {

       if ( bValue == TRUE)
       {
          syscfg_set_commit(NULL, "nfc_enabled", "true");
          //Start RdkNfcManager systemd service to start nfc services.
          v_secure_system("systemctl start RdkNfcManager.service");
       }
       else
       {
          syscfg_set_commit(NULL, "nfc_enabled", "false");
          //Stop RdkNfcManager systemd service to stop nfc services.
          v_secure_system("systemctl stop RdkNfcManager.service");
       }

       return TRUE;
    }
    return FALSE;
}
#endif //FEATURE_RDKB_NFC_MANAGER
