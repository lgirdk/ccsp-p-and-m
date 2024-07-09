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

extern "C"
{
#include "speedboost_webconfig_apis.h"
#include "speedboost_apis.h"
#include "speedBoostDoc.h"
}
using namespace testing;

SpeedBoost sSpeedBoost;
extern SyscfgMock * g_syscfgMock;
extern scheduleLibMock * g_scheduleLibMock;
extern webconfigFwMock *g_webconfigFwMock;
extern cosaWebconfigApisMock * g_cosaWebconfigApisMock;
extern helpersMock * g_helpersMock;
extern speedboostRbusHandlersMock * g_speedboostRbusHandlersMock;
extern speedboostSchedulerApisMock * g_speedboostSchedulerApisMock;

TEST (unpackAndProcessSpeedBoostData_Test1, unpackAndProcessSpeedBoostDataNullTest)
{
    EXPECT_EQ(false, unpackAndProcessSpeedBoostData(NULL));
}

TEST_F (PandMSSPTestFixture, unpackAndProcessSpeedBoostDataTestReturnFalse1)
{
    char SpeedBoostData [128] = "{\"speedBoost\":{\"enable\":true,\"clients\":[{\"mac\":\"00:00:00:00:00:00\",\"priority\":1}]}}";

    EXPECT_CALL(*g_cosaWebconfigApisMock, get_base64_decodedbuffer(_, _, _)).WillOnce(Return(1));

    EXPECT_EQ(false, unpackAndProcessSpeedBoostData(SpeedBoostData));
}

TEST_F (PandMSSPTestFixture, unpackAndProcessSpeedBoostDataTestReturnFalse2)
{
    char SpeedBoostData [128] = "{\"speedBoost\":{\"enable\":true,\"clients\":[{\"mac\":\"00:00:00:00:00:00\",\"priority\":1}]}}";
    char *mockDecodedMsg = strdup("decoded message"); // Allocate and set a mock decoded message
    int mockSize = strlen(mockDecodedMsg); // Mock size of the decoded message

    // Expect the get_base64_decodedbuffer call, set the second argument (pDecodeMsg) to point to mockDecodedMsg,
    // set the third argument (iSize) to mockSize, and return 0 to indicate success.
    EXPECT_CALL(*g_cosaWebconfigApisMock, get_base64_decodedbuffer(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(mockDecodedMsg), SetArgPointee<2>(mockSize), Return(1)));

    EXPECT_EQ(false, unpackAndProcessSpeedBoostData(SpeedBoostData));
}

TEST_F (PandMSSPTestFixture, unpackAndProcessSpeedBoostDataTestDecodeSuccess)
{
    char SpeedBoostData [128] = "{\"speedBoost\":{\"enable\":true,\"clients\":[{\"mac\":\"00:00:00:00:00:00\",\"priority\":1}]}}";
    char *mockDecodedMsg = strdup("decoded message"); // Allocate and set a mock decoded message
    int mockSize = strlen(mockDecodedMsg); // Mock size of the decoded message

    // Expect the get_base64_decodedbuffer call, set the second argument (pDecodeMsg) to point to mockDecodedMsg,
    // set the third argument (iSize) to mockSize, and return 0 to indicate success.
    EXPECT_CALL(*g_cosaWebconfigApisMock, get_base64_decodedbuffer(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(mockDecodedMsg), SetArgPointee<2>(mockSize), Return(0)));

    EXPECT_CALL (*g_cosaWebconfigApisMock, get_msgpack_unpack_status(_, _)).WillOnce(Return(MSGPACK_UNPACK_PARSE_ERROR));

    EXPECT_EQ(false, unpackAndProcessSpeedBoostData(SpeedBoostData));
}

TEST_F (PandMSSPTestFixture, unpackAndProcessSpeedBoostDataTestMsgUnpackSuccess)
{
    char SpeedBoostData [128] = "{\"speedBoost\":{\"enable\":true,\"clients\":[{\"mac\":\"00:00:00:00:00:00\",\"priority\":1}]}}";
    char *mockDecodedMsg = strdup("decoded message"); // Allocate and set a mock decoded message
    int mockSize = strlen(mockDecodedMsg); // Mock size of the decoded message

    // Expect the get_base64_decodedbuffer call, set the second argument (pDecodeMsg) to point to mockDecodedMsg,
    // set the third argument (iSize) to mockSize, and return 0 to indicate success.
    EXPECT_CALL(*g_cosaWebconfigApisMock, get_base64_decodedbuffer(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(mockDecodedMsg), SetArgPointee<2>(mockSize), Return(0)));

    EXPECT_CALL (*g_cosaWebconfigApisMock, get_msgpack_unpack_status(_, _)).WillOnce(Return(MSGPACK_UNPACK_SUCCESS));

    EXPECT_CALL(*g_helpersMock, helper_convert(_, _, _, _, _, _, _, _)).Times(1).WillOnce(Return(nullptr));

    EXPECT_EQ(false, unpackAndProcessSpeedBoostData(SpeedBoostData));
}

TEST_F (PandMSSPTestFixture, unpackAndProcessSpeedBoostDataTestDocConvertSuccess)
{
    char SpeedBoostData [128] = "{\"speedBoost\":{\"enable\":true,\"clients\":[{\"mac\":\"00:00:00:00:00:00\",\"priority\":1}]}}";
    char *mockDecodedMsg = strdup("decoded message"); // Allocate and set a mock decoded message
    int mockSize = strlen(mockDecodedMsg); // Mock size of the decoded message

    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;

    // Expect the get_base64_decodedbuffer call, set the second argument (pDecodeMsg) to point to mockDecodedMsg,
    // set the third argument (iSize) to mockSize, and return 0 to indicate success.
    EXPECT_CALL(*g_cosaWebconfigApisMock, get_base64_decodedbuffer(_, _, _))
        .WillOnce(DoAll(SetArgPointee<1>(mockDecodedMsg), SetArgPointee<2>(mockSize), Return(0)));

    EXPECT_CALL (*g_cosaWebconfigApisMock, get_msgpack_unpack_status(_, _)).WillOnce(Return(MSGPACK_UNPACK_SUCCESS));

    EXPECT_CALL(*g_helpersMock, helper_convert(_, _, _, _, _, _, _, _)).Times(1).WillOnce(Return(mockSpeedBoostDoc));

    EXPECT_CALL(*g_webconfigFwMock, PushBlobRequest(_)).Times(1);

    EXPECT_EQ(true, unpackAndProcessSpeedBoostData(SpeedBoostData));

    //mockMalloc::destroyInstance();
    free(mockSpeedBoostDoc->pSubDocName);
    delete mockSpeedBoostDoc;
}

TEST (processSpeedBoostWebConfigRequest_Test1, processSpeedBoostWebConfigRequestNullTest)
{
    pErr result = processSpeedBoostWebConfigRequest(NULL);
    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, NULL_BLOB_EXEC_POINTER); // Check if the error code is set correctly
    free(result); // Clean up allocated memory to prevent memory leak
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestBridgeModeEnabled)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("1", "1" + 1), Return(0)));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, VALIDATION_FALIED); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "SpeedBoost is Not supported when Gatewaty is in bridge mode"); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSchedulerNullTest)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = nullptr;

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    EXPECT_CALL(*g_scheduleLibMock, delete_schedule(_)).WillOnce(Return(1));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, BLOB_EXEC_FAILURE); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "Failed to stop scheduler"); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestDeleteScheduleSuccess)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = nullptr;

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    EXPECT_CALL(*g_scheduleLibMock, delete_schedule(_)).WillOnce(Return(0));

    EXPECT_CALL(*g_speedboostSchedulerApisMock, cleanPrevSyscfgParams()).WillOnce(Return(1));

    EXPECT_CALL(*g_speedboostSchedulerApisMock, triggerFirewallRestart()).WillOnce(Return(false));

    EXPECT_CALL(*g_speedboostRbusHandlersMock, publishEventChangeIfSubscribed(_,_,_));

    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_, _)).WillOnce(Return(1));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, SYSCFG_FAILURE); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "syscfg commit failed"); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSyscfgSetSuccess)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = nullptr;

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    EXPECT_CALL(*g_scheduleLibMock, delete_schedule(_)).WillOnce(Return(0));

    EXPECT_CALL(*g_speedboostSchedulerApisMock, cleanPrevSyscfgParams()).WillOnce(Return(1));

    EXPECT_CALL(*g_speedboostSchedulerApisMock, triggerFirewallRestart()).WillOnce(Return(false));

    EXPECT_CALL(*g_speedboostRbusHandlersMock, publishEventChangeIfSubscribed(_,_,_));

    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_, _)).WillOnce(Return(0));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, BLOB_EXEC_SUCCESS); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "Empty XmSpeedBoost blob execution Success"); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSchedulerAbsoluteIsNULL)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = nullptr;

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, NULL_BLOB_EXEC_POINTER); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "NULL pointer passed for absolute"); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    free(mockSpeedBoostDoc->pSchedulerInfo);
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSchedulerAbsoluteNotNULL)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = new input_t();

    // get a time_t value for the current time and add 120 days to it
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].time = time(NULL) + 120 * 24 * 60 * 60;

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, VALIDATION_FALIED); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "The Unix time is more than 90 days from now."); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    free(mockSpeedBoostDoc->pSchedulerInfo->absolute);
    free(mockSpeedBoostDoc->pSchedulerInfo);
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSchedulerInvalidActSize)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = new input_t[2];

    // get a time_t value for the current time and add 2 days to it
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].time = time(NULL) + 2 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].time = time(NULL) + 3 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions_size = 15;

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, VALIDATION_FALIED); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "Number of Mac Addresses exceeds the limit"); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    free(mockSpeedBoostDoc->pSchedulerInfo->absolute);
    free(mockSpeedBoostDoc->pSchedulerInfo);
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSchedulerNullAction)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = new input_t[2];

    // get a time_t value for the current time and add 2 days to it
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].time = time(NULL) + 2 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].time = time(NULL) + 3 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions_size = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions = nullptr;

    EXPECT_CALL(*g_syscfgMock, syscfg_get( _, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    ASSERT_NE(result, nullptr); // Ensure the function does not return a NULL pointer
    EXPECT_EQ(result->ErrorCode, NULL_BLOB_EXEC_POINTER); // Check if the error code is set correctly
    EXPECT_STREQ(result->ErrorMsg, "NULL pointer passed for actions"); // Check if the error message is set correctly

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    free(mockSpeedBoostDoc->pSchedulerInfo->absolute);
    free(mockSpeedBoostDoc->pSchedulerInfo);
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSchedulerActValIsNull)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = new input_t[2];

    // Get a time_t value for the current time and add 2 and 3 days to it
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].time = time(NULL) + 2 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].time = time(NULL) + 3 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions_size = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions = new char*[1];
    mockSpeedBoostDoc->pSchedulerInfo->actions[0] = strdup(""); // Set the first action to NULL

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    // Ensure the function does not return a NULL pointer
    ASSERT_NE(result, nullptr);

    // Check if the error code is set correctly
    EXPECT_EQ(result->ErrorCode, VALIDATION_FALIED);

    // Check if the error message is set correctly
    EXPECT_STREQ(result->ErrorMsg, "Invalid MAC Address");

    // Clean up allocated memory to prevent memory leak
    free(result);
    free(mockSpeedBoostDoc->pSubDocName);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->absolute;
    free(mockSpeedBoostDoc->pSchedulerInfo->actions[0]);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->actions;
    delete mockSpeedBoostDoc->pSchedulerInfo;
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestFailedToStartScheduler)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = new input_t[2];

    // Get a time_t value for the current time and add 2 and 3 days to it
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].time = time(NULL) + 2 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].time = time(NULL) + 3 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->actions = new char*[2];

    //Write a valid MAC address
    mockSpeedBoostDoc->pSchedulerInfo->actions[0] = strdup("00:1A:2B:3C:4D:5E");
    mockSpeedBoostDoc->pSchedulerInfo->actions[1] = strdup("CD:1A:2B:3C:4D:5F");

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    EXPECT_CALL(*g_scheduleLibMock, run_schedule(_,_)).WillOnce(Return(1));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    // Ensure the function does not return a NULL pointer
    ASSERT_NE(result, nullptr);

    // Check if the error code is set correctly
    EXPECT_EQ(result->ErrorCode, BLOB_EXEC_FAILURE);

    // Check if the error message is set correctly
    EXPECT_STREQ(result->ErrorMsg, "Failed to start scheduler");

    // Clean up allocated memory to prevent memory leak
    free(result);
    free(mockSpeedBoostDoc->pSubDocName);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->absolute;
    free(mockSpeedBoostDoc->pSchedulerInfo->actions[0]);
    free(mockSpeedBoostDoc->pSchedulerInfo->actions[1]);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->actions;
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestSchedulerStarted)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = new input_t[2];

    // Get a time_t value for the current time and add 2 and 3 days to it
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].time = time(NULL) + 2 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].time = time(NULL) + 3 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->actions = new char*[2];

    //Write a valid MAC address
    mockSpeedBoostDoc->pSchedulerInfo->actions[0] = strdup("00:1A:2B:3C:4D:5E");
    mockSpeedBoostDoc->pSchedulerInfo->actions[1] = strdup("CD:1A:2B:3C:4D:5F");

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    EXPECT_CALL(*g_scheduleLibMock, run_schedule(_,_)).WillOnce(Return(0));

    EXPECT_CALL(*g_speedboostRbusHandlersMock, publishEventChangeIfSubscribed(_,_,_));

    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_, _)).WillOnce(Return(1));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    // Ensure the function does not return a NULL pointer
    ASSERT_NE(result, nullptr);

    // Check if the error code is set correctly
    EXPECT_EQ(result->ErrorCode, SYSCFG_FAILURE);

    // Check if the error message is set correctly
    EXPECT_STREQ(result->ErrorMsg, "syscfg commit failed");

    // Clean up allocated memory to prevent memory leak
    free(result);
    free(mockSpeedBoostDoc->pSubDocName);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->absolute;
    free(mockSpeedBoostDoc->pSchedulerInfo->actions[0]);
    free(mockSpeedBoostDoc->pSchedulerInfo->actions[1]);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->actions;
    delete mockSpeedBoostDoc;
}

TEST_F (PandMSSPTestFixture, processSpeedBoostWebConfigRequestBlobExecutionSuccess)
{
    // Create and initialize a mock speedBoostDoc_t object
    speedBoostDoc_t *mockSpeedBoostDoc = new speedBoostDoc_t();
    mockSpeedBoostDoc->pSubDocName = strdup("speedBoost");
    mockSpeedBoostDoc->ui32Version = 1;
    mockSpeedBoostDoc->ui16TranscationId = 123;
    mockSpeedBoostDoc->pSchedulerInfo = new schedule_info_t();
    mockSpeedBoostDoc->pSchedulerInfo->absolute_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->absolute = new input_t[2];

    // Get a time_t value for the current time and add 2 and 3 days to it
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].time = time(NULL) + 2 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].time = time(NULL) + 3 * 24 * 60 * 60;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[0].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->absolute[1].action_count = 1;
    mockSpeedBoostDoc->pSchedulerInfo->actions_size = 2;
    mockSpeedBoostDoc->pSchedulerInfo->actions = new char*[2];

    //Write a valid MAC address
    mockSpeedBoostDoc->pSchedulerInfo->actions[0] = strdup("00:1A:2B:3C:4D:5E");
    mockSpeedBoostDoc->pSchedulerInfo->actions[1] = strdup("CD:1A:2B:3C:4D:5F");

    EXPECT_CALL(*g_syscfgMock, syscfg_get(_, _, _, _)).WillOnce(DoAll(SetArrayArgument<2>("0", "0" + 1), Return(0)));

    EXPECT_CALL(*g_scheduleLibMock, run_schedule(_,_)).WillOnce(Return(0));

    EXPECT_CALL(*g_speedboostRbusHandlersMock, publishEventChangeIfSubscribed(_,_,_));

    EXPECT_CALL(*g_syscfgMock, syscfg_set_nns_commit(_, _)).WillOnce(Return(0));

    pErr result = processSpeedBoostWebConfigRequest(mockSpeedBoostDoc);

    // Ensure the function does not return a NULL pointer
    ASSERT_NE(result, nullptr);

    // Check if the error code is set correctly
    EXPECT_EQ(result->ErrorCode, BLOB_EXEC_SUCCESS);

    // Check if the error message is set correctly
    EXPECT_STREQ(result->ErrorMsg, "XmSpeedBoost blob execution Success");

    free(result); // Clean up allocated memory to prevent memory leak
    free(mockSpeedBoostDoc->pSubDocName);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->absolute;
    free(mockSpeedBoostDoc->pSchedulerInfo->actions[0]);
    free(mockSpeedBoostDoc->pSchedulerInfo->actions[1]);
    delete[] mockSpeedBoostDoc->pSchedulerInfo->actions;
    delete mockSpeedBoostDoc;
}

TEST (freeResourcesSpeedBoostDoc_Test1,freeResourcesSpeedBoostDocNullTest)
{
    freeResourcesSpeedBoostDoc(NULL);
}

TEST (freeResourcesSpeedBoostDoc_Test2, freeResourcesSpeedBoostDocUserDataNullTest)
{
    execData *mockExecData = (execData *)malloc(sizeof(execData));
    mockExecData->user_data = NULL;
    freeResourcesSpeedBoostDoc(mockExecData);
}

TEST_F (PandMSSPTestFixture, freeResourcesSpeedBoostDocValidDataTest)
{
    execData *mockExecData = (execData *)malloc(sizeof(execData));
    speedBoostDoc_t *mockSpeedBoostDoc = (speedBoostDoc_t *)malloc(sizeof(speedBoostDoc_t));
    mockExecData->user_data = mockSpeedBoostDoc;

    // Set expectations on the freeScheduleInfo mock method
    EXPECT_CALL(*g_scheduleLibMock, freeScheduleInfo(_)).Times(1);

    freeResourcesSpeedBoostDoc(mockExecData);
}

TEST (validateUnixTime_Test1, validateUnixTimeTest_ValidTime)
{
    int64_t currentTime = (int64_t)time(NULL);
    char errMsg[100] = {0};
    bool result = validateUnixTime(currentTime + (30 * 24 * 60 * 60), errMsg, sizeof(errMsg)); // 30 days from now

    EXPECT_TRUE(result);
    EXPECT_STREQ(errMsg, "");
}

TEST (validateUnixTime_Test2, validateUnixTimeTest_Exactly90Days)
{
    int64_t currentTime = (int64_t)time(NULL);
    char errMsg[100] = {0};
    bool result = validateUnixTime(currentTime + NINETY_DAYS_IN_SECONDS, errMsg, sizeof(errMsg)); // Exactly 90 days from now

    EXPECT_TRUE(result);
    EXPECT_STREQ(errMsg, "");
}

TEST (validateUnixTime_Test3, validateUnixTimeTest_MoreThan90Days)
{
    int64_t currentTime = (int64_t)time(NULL);
    char errMsg[100] = {0};
    bool result = validateUnixTime(currentTime + NINETY_DAYS_IN_SECONDS + 1, errMsg, sizeof(errMsg)); // More than 90 days from now

    EXPECT_FALSE(result);
    EXPECT_STREQ(errMsg, "The Unix time is more than 90 days from now.");
}

TEST (validateUnixTime_Test4, validateUnixTimeTest_PastTime)
{
    int64_t currentTime = (int64_t)time(NULL);
    char errMsg[100] = {0};
    bool result = validateUnixTime(currentTime - (30 * 24 * 60 * 60), errMsg, sizeof(errMsg)); // 30 days in the past

    EXPECT_TRUE(result);
    EXPECT_STREQ(errMsg, "");
}

TEST (validateUnixTime_Test5, validateUnixTimeTest_NullErrMsg)
{
    int64_t currentTime = (int64_t)time(NULL);
    bool result = validateUnixTime(currentTime + NINETY_DAYS_IN_SECONDS + 1, NULL, 0); // More than 90 days from now, with NULL error message buffer

    EXPECT_FALSE(result);
}

TEST (ValidateMac_Test1, ValidateMac_ValidMac)
{
    char validMac[] = "01:23:45:67:89:AB";
    EXPECT_TRUE(ValidateMac(validMac));
}

TEST (ValidateMac_Test2, ValidateMac_NullPointer)
{
    EXPECT_FALSE(ValidateMac(NULL));
}

TEST (ValidateMac_Test3, ValidateMac_AllZeros)
{
    char invalidMac[] = "00:00:00:00:00:00";
    EXPECT_FALSE(ValidateMac(invalidMac));
}

TEST (ValidateMac_Test4, ValidateMac_WrongLength)
{
    char invalidMac[] = "01:23:45:67:89";
    EXPECT_FALSE(ValidateMac(invalidMac));
}

TEST (ValidateMac_Test5, ValidateMac_InvalidCharacters)
{
    char invalidMac[] = "01:23:45:67:89:ZZ";
    EXPECT_FALSE(ValidateMac(invalidMac));
}
TEST (CheckMacHasValidCharacter_Test1, CheckMacHasValidCharacter_ValidMac)
{
    char validMac[] = "01:23:45:67:89:AB";
    EXPECT_TRUE(CheckMacHasValidCharacter(validMac));
}

TEST (CheckMacHasValidCharacter_Test2, CheckMacHasValidCharacter_NullPointer)
{
    EXPECT_FALSE(CheckMacHasValidCharacter(NULL));
}

TEST (CheckMacHasValidCharacter_Test3, CheckMacHasValidCharacter_InvalidCharacter)
{
    char invalidMac[] = "01:23:45:67:89:ZZ";
    EXPECT_FALSE(CheckMacHasValidCharacter(invalidMac));
}

TEST (CheckMacHasValidCharacter_Test4, CheckMacHasValidCharacter_MissingColon)
{
    char invalidMac[] = "01:23:4567:89:AB";
    EXPECT_FALSE(CheckMacHasValidCharacter(invalidMac));
}

TEST (CheckMacHasValidCharacter_Test5, CheckMacHasValidCharacter_TooShort)
{
    char invalidMac[] = "01:23:45:67:89";
    EXPECT_FALSE(CheckMacHasValidCharacter(invalidMac));
}

TEST (CheckMacHasValidCharacter_Test6, CheckMacHasValidCharacter_TooLong)
{
    char invalidMac[] = "01:23:45:67:89:AB:CD";
    EXPECT_FALSE(CheckMacHasValidCharacter(invalidMac));
}