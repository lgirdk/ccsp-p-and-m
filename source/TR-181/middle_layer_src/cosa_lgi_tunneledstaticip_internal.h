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

#ifndef _COSA_LGI_TUNNELEDSTATICIP_INTERNAL_H
#define _COSA_LGI_TUNNELEDSTATICIP_INTERNAL_H

#include "cosa_apis.h"
#include "plugin_main_apis.h"
#include "cosa_lgi_tunneledstaticip_apis.h"

#define  COSA_DATAMODEL_LGI_TUNNELEDSTATICIP_CLASS_CONTENT \
    /* duplication of the base object class content */     \
    COSA_BASE_CONTENT                                      \

typedef  struct
_COSA_DML_LGI_TUNNELEDSTATICIP_CFG
{
    BOOL     Enable;
    char     Username[256];
    char     Password[256];
    int      RadiusInterface;
    char     RadiusAuthServerIPAddr[45];
    char     RadiusAuthSecret[45];
    int      RadiusAuthServerPort;
    char     NASIdentifier[45];
    char     RadiusAccServerIPAddr[45];
    char     RadiusAccSecret[45];
    int      RadiusAccServerPort;
}
COSA_DML_LGI_TUNNELEDSTATICIP_CFG, *PCOSA_DML_LGI_TUNNELEDSTATICIP_CFG;

#define TUNNELEDSTATICIP_CFG_MEMBER_SIZE(__member) sizeof(((PCOSA_DML_LGI_TUNNELEDSTATICIP_CFG)NULL)->__member)

typedef  struct
_COSA_DATAMODEL_LGI_TUNNELEDSTATICIP_CLASS_CONTENT
{
    COSA_DATAMODEL_LGI_TUNNELEDSTATICIP_CLASS_CONTENT
    /* start of LGITUNNELEDSTATICIP object class content */
    COSA_DML_LGI_TUNNELEDSTATICIP_CFG Cfg;
    COSA_DML_LGI_TUNNELEDSTATICIP_CFG OldCfg;
}
COSA_DATAMODEL_LGI_TUNNELEDSTATICIP, *PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP;

ANSC_HANDLE CosaLgiTunneledStaticIPCreate (void);
ANSC_STATUS CosaLgiTunneledStaticIPInitialize (ANSC_HANDLE hThisObject);
ANSC_STATUS CosaLgiTunneledStaticIPRemove (ANSC_HANDLE hThisObject);
ANSC_STATUS CosaDmlTunneledStaticIPRestart (ANSC_HANDLE hThisObject);

#endif
