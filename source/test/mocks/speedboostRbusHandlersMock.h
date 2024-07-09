/*************************************************************************
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
**************************************************************************/
#ifndef SPEEDBOOST_RBUS_HANDLERS_MOCK_H
#define SPEEDBOOST_RBUS_HANDLERS_MOCK_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class speedboostRbusHandlers
{
    public:
        virtual ~speedboostRbusHandlers() {}
        virtual void publishEventChangeIfSubscribed(const char *, void *, void *) = 0;
};

class speedboostRbusHandlersMock : public speedboostRbusHandlers
{
    public:
        virtual ~speedboostRbusHandlersMock() {}
        MOCK_METHOD3(publishEventChangeIfSubscribed, void(const char *, void *, void *));
};
#endif