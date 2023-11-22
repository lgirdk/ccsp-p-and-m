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

    module: cosa_FileTransfer_api.c

        For COSA Data Model Library Development

    -------------------------------------------------------------------

    description:

        This file implementes back-end apis for the COSA Data Model Library

        *  CosaDmlDiInit
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

#include "cosa_x_cisco_com_filetransfer_apis.h"
#include "cosa_x_cisco_com_filetransfer_internal.h"
#include "dmsb_tr181_psm_definitions.h"
#include "ccsp_psm_helper.h"
#include <curl/curl.h>
#include "secure_wrapper.h"
#include "safec_lib_common.h"

/* Set up default https server table */
typedef struct
{
    COSA_DML_FILETRANSFER_SERVER    mStatus;
    char                  *mServer;
    char                  *mServerName;
} FILETRANSFER_SERVER_item;


FILETRANSFER_SERVER_item FileTransfer_HTTPSServers[] = {
    {COSA_DML_FILETRANSFER_SERVER_NONE, "", ""},
    {COSA_DML_FILETRANSFER_SERVER_HTTPS1, "tintaiih.comcast.net", "HTTPS Server 1"}
};


ANSC_STATUS
CosaDmlFileTransferInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
)
{
    UNREFERENCED_PARAMETER(hDml);
    UNREFERENCED_PARAMETER(phContext);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlFileTransferGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_FILETRANSFER_CFG  pCfg
    )
{
    int                             iReturnValue    = CCSP_SUCCESS;
    char                            pParamPath[64]  = {0};
    unsigned int                    RecordType      = 0;
    SLAP_VARIABLE                   SlapValue       = {0};
    errno_t  safec_rc = -1;

    UNREFERENCED_PARAMETER(hContext);
    /* Fetch Cfg, we only support https download for now */

    if ( TRUE )     /* Server */
    {
        SlapInitVariable(&SlapValue);

        safec_rc = sprintf_s
            (
                pParamPath, sizeof(pParamPath),
                DMSB_TR181_PSM_ft_Root DMSB_TR181_PSM_ft_Server
            );
        if(safec_rc < EOK)
        {
           ERR_CHK(safec_rc);
        }

        iReturnValue =
            PSM_Get_Record_Value
                (
                    g_MessageBusHandle,
                    g_SubsystemPrefix,
                    pParamPath,
                    &RecordType,
                    &SlapValue
                );

        if ( (iReturnValue != CCSP_SUCCESS) || (RecordType != ccsp_unsignedInt))
        {
            AnscTraceWarning
                ((
                    "%s -- failed to retrieve the parameter %s, error code %d, type %d\n",
                    __FUNCTION__,
                    pParamPath,
                    iReturnValue,
                    RecordType
                ));
        }
        else
        {
            //pCfg->Server, SlapValue.Variant.varUint32;
            /* CID: 56945,62853  Expression with no effect*/
            //pCfg->Server = SlapValue.Variant.varUint32;
        }

        SlapCleanVariable(&SlapValue);
    }

    if ( TRUE )     /* Protocol */
    {
        SlapInitVariable(&SlapValue);

        safec_rc = sprintf_s
            (
                pParamPath, sizeof(pParamPath),
                DMSB_TR181_PSM_ft_Root DMSB_TR181_PSM_ft_Protocol
            );
        if(safec_rc < EOK)
        {
           ERR_CHK(safec_rc);
        }

        iReturnValue =
            PSM_Get_Record_Value
                (
                    g_MessageBusHandle,
                    g_SubsystemPrefix,
                    pParamPath,
                    &RecordType,
                    &SlapValue
                );

        if ( (iReturnValue != CCSP_SUCCESS) || (RecordType != ccsp_unsignedInt))
        {
            AnscTraceWarning
                ((
                    "%s -- failed to retrieve the parameter %s, error code %d, type %d\n",
                    __FUNCTION__,
                    pParamPath,
                    iReturnValue,
                    RecordType
                ));
        }
        else
        {
            pCfg->Protocol = SlapValue.Variant.varUint32;
        }

        SlapCleanVariable(&SlapValue);
    }

    if ( TRUE )     /* FileName */
    {
        SlapInitVariable(&SlapValue);

        safec_rc = sprintf_s
            (
                pParamPath, sizeof(pParamPath),
                DMSB_TR181_PSM_ft_Root DMSB_TR181_PSM_ft_FileName
            );
        if(safec_rc < EOK)
        {
           ERR_CHK(safec_rc);
        }

        iReturnValue =
            PSM_Get_Record_Value
                (
                    g_MessageBusHandle,
                    g_SubsystemPrefix,
                    pParamPath,
                    &RecordType,
                    &SlapValue
                );

        if ( (iReturnValue != CCSP_SUCCESS) || (RecordType != ccsp_string))
        {
            AnscTraceWarning
                ((
                    "%s -- failed to retrieve the parameter %s, error code %d, type %d\n",
                    __FUNCTION__,
                    pParamPath,
                    iReturnValue,
                    RecordType
                ));
        }
        else
        {
            safec_rc = strcpy_s((char*)pCfg->FileName,sizeof(pCfg->FileName), SlapValue.Variant.varString);
            ERR_CHK(safec_rc);
        }

        SlapCleanVariable(&SlapValue);
    }

    return ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlFileTransferGetStatus
    (
        ANSC_HANDLE                 hContext
    )
{
    UNREFERENCED_PARAMETER(hContext);
    PCOSA_DATAMODEL_FILETRANSFER    pMyObject = (PCOSA_DATAMODEL_FILETRANSFER)hContext;

    return pMyObject->Status;
}

