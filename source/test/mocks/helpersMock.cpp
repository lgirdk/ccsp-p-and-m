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
#include "helpersMock.h"

using namespace std;

extern helpersMock * g_helpersMock;

extern "C" void* helper_convert(const void *buf, size_t len, size_t struct_size, const char *wrapper, msgpack_object_type expect_type, bool optional, process_fn_t process, destroy_fn_t destroy)
{
    if (g_helpersMock)
    {
        return g_helpersMock->helper_convert(buf, len, struct_size, wrapper, expect_type, optional, process, destroy);
    }
    return nullptr;
}