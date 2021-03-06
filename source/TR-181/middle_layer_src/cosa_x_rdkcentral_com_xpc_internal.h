/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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

#ifndef  _COSA_X_RDKCENTRAL_COM_XPC_INTERNAL_H
#define  _COSA_X_RDKCENTRAL_COM_XPC_INTERNAL_H

#include "ansc_platform.h"
#include "ansc_string_util.h"

typedef  struct
_COSA_DATAMODEL_XPC
{
    BOOL            bEnable;
}
COSA_DATAMODEL_XPC,  *PCOSA_DATAMODEL_XPC;

/*
    Standard function declaration 
*/
ANSC_STATUS CosaXpcEnable(ANSC_HANDLE hThisObject);

ANSC_STATUS CosaXpcDisable(ANSC_HANDLE hThisObject);

ANSC_HANDLE
CosaXpcCreate
    (
        VOID
    );

ANSC_STATUS
CosaXpcInitialize
    (
        ANSC_HANDLE                 hThisObject
    );

ANSC_STATUS
CosaXpcRemove
    (
        ANSC_HANDLE                 hThisObject
    );
#endif //_COSA_X_RDKCENTRAL_COM_XPC_INTERNAL_H
