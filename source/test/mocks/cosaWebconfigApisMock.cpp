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
#include "cosaWebconfigApisMock.h"

using namespace std;

extern cosaWebconfigApisMock * g_cosaWebconfigApisMock;

extern "C" int get_base64_decodedbuffer(char *pString, char **buffer, int *size)
{
    if (g_cosaWebconfigApisMock)
    {
        return g_cosaWebconfigApisMock->get_base64_decodedbuffer(pString, buffer, size);
    }
    return -1;
}

extern "C" msgpack_unpack_return get_msgpack_unpack_status(char *decodedbuf, int size)
{
    if (g_cosaWebconfigApisMock)
    {
        return g_cosaWebconfigApisMock->get_msgpack_unpack_status(decodedbuf, size);
    }
    return MSGPACK_UNPACK_SUCCESS;
}