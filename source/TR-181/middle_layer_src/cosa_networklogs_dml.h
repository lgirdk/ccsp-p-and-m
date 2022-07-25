/*****************************************************************************
 * Copyright 2022 Liberty Global B.V.
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
 ****************************************************************************/

#ifdef FEATURE_RDKB_WAN_MANAGER

#ifndef _COSA_NETWORKLOGS_DML_H_
#define _COSA_NETWORKLOGS_DML_H_

/***********************************************************************

 APIs for Object:

    NetworkLogs.Logs.{i}.

    *   Logs_GetEntryCount
    *   Logs_GetEntry
    *   Logs_IsUpdated
    *   Logs_Synchronize
    *   Logs_GetParamUlongValue
    *   Logs_GetParamStringValue

***********************************************************************/

/**
 * @brief This function is called to retrieve the log count associated with the NetworkLogs.Logs datamodel.
 *
 * @param[in] hInsContext  Instance handle.
 *
 * @return  Returns the total count.
 */
ULONG
Logs_GetEntryCount
    (
        ANSC_HANDLE         hInsContext
    );

/**
 * @brief This function is called to retrieve the network log entry specified by the index associated with the NetworkLogs.Logs datamodel.
 *
 * @param[in] hInsContext      Instance handle.
 * @param[in] nIndex           Index number to get network log entry information.
 * @param[in] pInsNumber       Output instance number.
 *
 * @return  Returns handle to identify the entry on success case, Otherwise returns NULL.
 */
ANSC_HANDLE
Logs_GetEntry
    (
        ANSC_HANDLE                 hInsContext,
        ULONG                       nIndex,
        ULONG*                      pInsNumber
    );

/**
 * @brief This function is used to check whether the network log info table is updated or not
 * associated with the NetworkLogs.Logs datamodel.
 *
 * @param[in] hInsContext    Instance handle.
 *
 * @return  Returns TRUE once updated.
 */
BOOL
Logs_IsUpdated
    (
        ANSC_HANDLE                 hInsContext
    );

/**
 * @brief This function is called to synchronize the table info associated with the NetworkLogs.Log datamodel.
 *
 * @param[in] hInsContext  Instance handle.
 *
 * @return  Returns 0 once synchronized.
 */
ULONG
Logs_Synchronize
    (
        ANSC_HANDLE                 hInsContext
    );

/**
 * @brief This function checks for the parameter field and retrieves the value of network log info.
 * Details associated with the NetworkLogs.Logs datamodel.
 *
 * @param[in] hInsContext     Instance handle.
 * @param[in] ParamName       Parameter field.
 * @param[in] pUlong          Parameter Value.
 *
 * @return  Returns TRUE once get the value, returns FALSE when receive unsupported parameter.
 */
BOOL
Logs_GetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        ULONG*                      puLong
    );

/**
 * @brief This function is called to retrieve String parameter value of network log info
 * associated with the NetworkLogs.Log datamodel.
 *
 * @param[in] hInsContext   Object handle.
 * @param[in] ParamName     Parameter field to get.
 * @param[out] pValue       Parameter value.
 * @param[out] pUlSize      String length.
 *
 * @return  Returns 0 once get the value, Otherwise returns -1 on failure case.
 */
ULONG
Logs_GetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        char*                       pValue,
        ULONG*                      pUlSize  
    );

#endif

#endif