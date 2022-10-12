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


#ifndef  _COSA_LGI_TUNNELEDSTATICIP_APIS_H
#define  _COSA_LGI_TUNNELEDSTATICIP_APIS_H

#include "cosa_apis.h"



ANSC_STATUS
CosaDmlTunneledStaticIPGetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetEnable
    (
        ANSC_HANDLE                 hContext,
        BOOL                        bValue
    );


ANSC_STATUS 
CosaDmlTunneledStaticIPGetRadiusAuthServerPort 
    ( 
        ANSC_HANDLE                 hContext, 
        int                         *pValue 
    );


ANSC_STATUS 
CosaDmlTunneledStaticIPSetRadiusAuthServerPort 
    ( 
        ANSC_HANDLE                 hContext, 
        int                         value 
    );


ANSC_STATUS 
CosaDmlTunneledStaticIPGetRadiusAccServerPort
    ( 
        ANSC_HANDLE                 hContext, 
        int                         *pValue 
    );


ANSC_STATUS 
CosaDmlTunneledStaticIPSetRadiusAccServerPort
    ( 
        ANSC_HANDLE                 hContext, 
        int                         value 
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetUsername
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetUsername
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetPassword
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetPassword
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetRadiusAuthServerIPAddr
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetRadiusAuthServerIPAddr
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetRadiusAuthSecret
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetRadiusAuthSecret
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetNASIdentifier
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetNASIdentifier
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetRadiusAccServerIPAddr
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetRadiusAccServerIPAddr
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetRadiusAccSecret
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetRadiusAccSecret
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPGetRadiusInterface
    (
        ANSC_HANDLE                 hContext,
        ULONG                        *pValue
    );


ANSC_STATUS
CosaDmlTunneledStaticIPSetRadiusInterface
    (
        ANSC_HANDLE                 hContext,
        ULONG                        value
    );

#endif

