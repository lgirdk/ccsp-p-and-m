/*****************************************************************************
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2024 RDK Management
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
*******************************************************************************/

#ifndef MOCK_RBUS_HANDLER_H
#define MOCK_RBUS_HANDLER_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

extern "C"
{
#include <rbus.h>
#include "cosa_deviceinfo_dml.h"
#include "cosa_dhcpv6_apis.h"
}

class rbusHandlerInterface {
public:
        virtual ~rbusHandlerInterface() {}
        virtual ULONG DeviceInfo_GetParamStringValue(ANSC_HANDLE, char*, char*, ULONG*) = 0;
        virtual void configureIpv6Route(uint32_t) = 0;
};

class rbusHandlerMock: public rbusHandlerInterface {
public:
        virtual ~rbusHandlerMock() {}
        MOCK_METHOD4(DeviceInfo_GetParamStringValue, ULONG(ANSC_HANDLE, char*, char*, ULONG*));
        MOCK_METHOD1(configureIpv6Route, void(uint32_t));
};
#endif