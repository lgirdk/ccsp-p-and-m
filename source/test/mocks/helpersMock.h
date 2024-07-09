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
#ifndef HELPERS_MOCK_H
#define HELPERS_MOCK_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <msgpack.h>

extern "C" 
{
#include "helpers.h"
}

class helperOperations
{
    public:
        virtual ~helperOperations() {}
        virtual void* helper_convert(const void *, size_t, size_t, const char *, msgpack_object_type, bool, process_fn_t, destroy_fn_t) = 0;
};

class helpersMock : public helperOperations
{
    public:
        virtual ~helpersMock() {}
        MOCK_METHOD8(helper_convert, void*(const void *, size_t, size_t, const char *, msgpack_object_type, bool, process_fn_t, destroy_fn_t));
};
#endif