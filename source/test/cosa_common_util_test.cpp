#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <experimental/filesystem>
#include "pandmssp_mock.h"
#include "mocks/commonutil_mock.h"
#define MOCK_COMMON_UTIL_PAM

using namespace std;
using std::experimental::filesystem::exists;
using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;

extern "C"
{
#include "cosa_common_util.h"
extern void free_args_struct(arg_struct_t *param);
extern void* Set_Notifi_ParamName(void *args);
}

extern rbusCommonUtilMock * g_rbusCommonUtilMock;

TEST(FreeArgsStructTest_1, FreeArgsStruct)
{
    arg_struct_t* param = nullptr;

    EXPECT_EQ(param, nullptr);
    free_args_struct(param);
    EXPECT_EQ(param, nullptr);
}

TEST(FreeArgsStructTest_2, FreeArgsStruct_Test2)
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

TEST(FreeArgsStructTest_3, FreeArgsStruct_Test3)
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

TEST(FreeArgsStructTest_4, FreeArgsStruct_Test4)
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

#define MOCK_COMMON_UTIL_PAM_1
TEST(Set_Notifi_ParamNameTest, Set_Notifi_ParamName_Test1)
{
    arg_struct_t* args = nullptr;
    EXPECT_EQ(args, nullptr);
    EXPECT_EQ(Set_Notifi_ParamName(args), nullptr);
}
