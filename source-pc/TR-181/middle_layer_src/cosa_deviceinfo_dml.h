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

    module: cosa_deviceinfo_dml.h

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file defines the apis for objects to support Data Model Library.

    -------------------------------------------------------------------


    author:

        COSA XML TOOL CODE GENERATOR 1.0

    -------------------------------------------------------------------

    revision:

        01/14/2011    initial revision.

**************************************************************************/


#ifndef  _COSA_DEVICEINFO_DML_H
#define  _COSA_DEVICEINFO_DML_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_deviceinfo_internal.h"
#include "cosa_deviceinfo_apis.h"

#include "cosa_deviceinfo_dml_custom.h"

#define  PARTNER_ID_LEN  64

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
BOOL
DeviceInfo_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
DeviceInfo_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
DeviceInfo_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
DeviceInfo_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
DeviceInfo_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
DeviceInfo_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    );

BOOL
DeviceInfo_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    );

BOOL
DeviceInfo_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
DeviceInfo_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
DeviceInfo_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
DeviceInfo_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

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
ULONG
VendorConfigFile_GetEntryCount
    (
        ANSC_HANDLE
    );

ANSC_HANDLE
VendorConfigFile_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

BOOL
VendorConfigFile_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
VendorConfigFile_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
VendorConfigFile_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
VendorConfigFile_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

BOOL
VendorConfigFile_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
VendorConfigFile_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         value
    );

BOOL
VendorConfigFile_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValuepUlong
    );

BOOL
VendorConfigFile_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       strValue
    );

BOOL
VendorConfigFile_Validate
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pReturnParamName,
        ULONG*                      puLength
    );

ULONG
VendorConfigFile_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
VendorConfigFile_Rollback
    (
        ANSC_HANDLE                 hInsContext
    );

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
ULONG
SupportedDataModel_GetEntryCount
    (
        ANSC_HANDLE
    );

ANSC_HANDLE
SupportedDataModel_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

BOOL
SupportedDataModel_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
SupportedDataModel_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
SupportedDataModel_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
SupportedDataModel_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

/***********************************************************************

 APIs for Object:

    DeviceInfo.MemoryStatus.

    *  MemoryStatus_GetParamBoolValue
    *  MemoryStatus_GetParamIntValue
    *  MemoryStatus_GetParamUlongValue
    *  MemoryStatus_GetParamStringValue

***********************************************************************/
BOOL
MemoryStatus_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
MemoryStatus_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
MemoryStatus_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
MemoryStatus_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

/***********************************************************************

 APIs for Object:

    DeviceInfo.ProcessStatus.

    *  ProcessStatus_GetParamBoolValue
    *  ProcessStatus_GetParamIntValue
    *  ProcessStatus_GetParamUlongValue
    *  ProcessStatus_GetParamStringValue

***********************************************************************/
BOOL
ProcessStatus_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
ProcessStatus_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
ProcessStatus_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
ProcessStatus_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

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
ULONG
Process_GetEntryCount
    (
        ANSC_HANDLE
    );

ANSC_HANDLE
Process_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

BOOL
Process_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
Process_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    );

BOOL
Process_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
Process_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
Process_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
Process_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

/***********************************************************************

 APIs for Object:

    DeviceInfo.NetworkProperties.

    *  NetworkProperties_GetParamBoolValue
    *  NetworkProperties_GetParamIntValue
    *  NetworkProperties_GetParamUlongValue
    *  NetworkProperties_GetParamStringValue

***********************************************************************/
BOOL
NetworkProperties_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
NetworkProperties_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
NetworkProperties_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      pUlong
    );

ULONG
NetworkProperties_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize
    );

/***********************************************************************

 APIs for Object:

    DeviceInfo.Webpa.

    *  Webpa_GetParamBoolValue
    *  Webpa_GetParamIntValue
    *  Webpa_GetParamUlongValue
    *  Webpa_GetParamStringValue
	*  Webpa_SetParamBoolValue
    *  Webpa_SetParamIntValue
    *  Webpa_SetParamUlongValue
    *  Webpa_SetParamStringValue

***********************************************************************/

BOOL
Webpa_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );
	
BOOL
Webpa_GetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int*                        pInt
    );

BOOL
Webpa_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    );

ULONG
Webpa_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
		ULONG*                      pUlSize
    );

BOOL
Webpa_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL                        bValue
    );

BOOL
Webpa_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        int                         iValue
    );

BOOL
Webpa_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG                       uValue
    );

BOOL
Webpa_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pString
    );

	
#endif
