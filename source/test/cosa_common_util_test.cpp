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

#include "pandmssp_mock.h"
#define RBUS_WAN_IP

using namespace testing;

extern "C"
{
#include "cosa_common_util.h"
extern void* Set_Notifi_ParamName(void *args);
}

ANSC_HANDLE bus_handle;
extern SyseventMock * g_syseventMock;

TEST (FreeArgsStructTest_1, FreeArgsStruct)
{
    arg_struct_t* param = nullptr;

    EXPECT_EQ(param, nullptr);
    free_args_struct(param);
    EXPECT_EQ(param, nullptr);
}

TEST (FreeArgsStructTest_2, FreeArgsStruct_Test2)
{
    arg_struct_t* param = (arg_struct_t*)malloc(sizeof(arg_struct_t));
    memset(param, 0, sizeof(arg_struct_t));
    char parameterName[] = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";

    param->parameterName = strdup(parameterName);
    param->newValue = nullptr;
    param->oldValue = nullptr;

    EXPECT_NE(param, nullptr);
    EXPECT_NE(param->parameterName, nullptr);
    EXPECT_EQ(param->newValue, nullptr);
    EXPECT_EQ(param->oldValue, nullptr);

    free_args_struct(param);
}

TEST (FreeArgsStructTest_3, FreeArgsStruct_Test3)
{
    arg_struct_t* param = (arg_struct_t*)malloc(sizeof(arg_struct_t));
    memset(param, 0, sizeof(arg_struct_t));
    char parameterName[] = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";
    char newValue[] = "67.164.67.77";

    param->parameterName = strdup(parameterName);
    param->newValue = strdup(newValue);
    param->oldValue = nullptr;

    EXPECT_NE(param, nullptr);
    EXPECT_NE(param->parameterName, nullptr);
    EXPECT_NE(param->newValue, nullptr);
    EXPECT_EQ(param->oldValue, nullptr);

    free_args_struct(param);
}

TEST (FreeArgsStructTest_4, FreeArgsStruct_Test4)
{
    arg_struct_t* param = (arg_struct_t*)malloc(sizeof(arg_struct_t));
    memset(param, 0, sizeof(arg_struct_t));
    char parameterName[] = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";
    char newValue[] = "67.164.67.77";
    char oldValue[] = "N.A";

    param->parameterName = strdup(parameterName);
    param->newValue = strdup(newValue);
    param->oldValue = strdup(oldValue);

    EXPECT_NE(param, nullptr);
    EXPECT_NE(param->parameterName, nullptr);
    EXPECT_NE(param->newValue, nullptr);
    EXPECT_NE(param->oldValue, nullptr);

    free_args_struct(param);
}

TEST (Set_Notifi_ParamNameTest, Set_Notifi_ParamName_Test1)
{
    arg_struct_t* args = nullptr;
    EXPECT_EQ(args, nullptr);
    EXPECT_EQ(Set_Notifi_ParamName(args), nullptr);
}