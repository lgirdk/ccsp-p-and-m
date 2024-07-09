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

#ifndef COSA_WEBCONFIG_APIS_MOCK_H
#define COSA_WEBCONFIG_APIS_MOCK_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <msgpack.h>

class cosaWebconfigApis
{
    public:
        virtual ~cosaWebconfigApis() {}
        virtual int  get_base64_decodedbuffer(char *, char **, int *) = 0;
        virtual msgpack_unpack_return get_msgpack_unpack_status(char *, int) = 0;
};

class cosaWebconfigApisMock : public cosaWebconfigApis
{
    public:
        virtual ~cosaWebconfigApisMock() {}
        MOCK_METHOD3(get_base64_decodedbuffer, int(char *, char **, int *));
        MOCK_METHOD2(get_msgpack_unpack_status, msgpack_unpack_return(char *, int));
};
#endif