/*********************************************************************
 * Copyright 2017-2019 ARRIS Enterprises, LLC.
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
 **********************************************************************/

#ifndef  _COSA_LGI_USERINTERFACE_APIS_H
#define  _COSA_LGI_USERINTERFACE_APIS_H

#include "../middle_layer_src/cosa_apis.h"

/**********************************************************************
                STRUCTURE AND CONSTANT DEFINITIONS
**********************************************************************/

struct
_COSA_DML_STD_RA_CFG
{
    /*
     *  TR-181 Standard UserInterface.RemoteAccess
     */
    BOOLEAN                         bEnabled;
    ULONG                           Port;
    char                            SupportedProtocols[64];
    char                            Protocol[16];
}_struct_pack_;

typedef struct _COSA_DML_STD_RA_CFG  COSA_DML_STD_RA_CFG,  *PCOSA_DML_STD_RA_CFG;

/**********************************************************************
                FUNCTION PROTOTYPES
**********************************************************************/

ANSC_STATUS
CosaDmlStdRaInit
    (
        ANSC_HANDLE                 hDml,
        PANSC_HANDLE                phContext
    );

ANSC_STATUS
CosaDmlStdRaSetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_STD_RA_CFG        pCfg
    );

ANSC_STATUS
CosaDmlStdRaGetCfg
    (
        ANSC_HANDLE                 hContext,
        PCOSA_DML_STD_RA_CFG        pCfg
    );

typedef struct _SYS_CFG_APPLY_LIST
{
    char * sysCfgName;
    char * sysCfgValue;
    struct _SYS_CFG_APPLY_LIST * next;
} _SYS_CFG_APPLY_LIST;

ANSC_STATUS
AddSysCfgValueToCommit
    (
        char *pcName,
        char *pcValue
    );

ANSC_STATUS
CommitChangedSysCfgValues
    (
    );

ANSC_STATUS
DiscardChangedSysCfgValues
    (
    );

#endif
