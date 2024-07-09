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
    #include "speedBoostDoc.h"
    #include "speedboost_webconfig_apis.h"
}

using namespace testing;

extern helpersMock * g_helpersMock;
extern scheduleLibMock * g_scheduleLibMock;

TEST (speedBoostDocConvert_Test1, speedBoostDocConvertNULLCheck)
{
    // Call the function you're testing
    speedBoostDoc_t* pSpeedDocRes = speedBoostDocConvert(NULL, 0);
    // Check that the result is NULL
    EXPECT_EQ(pSpeedDocRes, nullptr);
}

TEST_F (PandMSSPTestFixture, speedBoostDocConvertNULLCheck2)
{
    char cBuf [ ] = "It is empty buffer";
    int iBufLen = strlen(cBuf);

    EXPECT_CALL(*g_helpersMock, helper_convert(_, _, _, _, _, _, _, _)).Times(1).WillOnce(Return(nullptr));

    speedBoostDoc_t * pSpeedDocRes = speedBoostDocConvert(cBuf, iBufLen);

    // Check that the result is NULL
    EXPECT_EQ(pSpeedDocRes, nullptr);
}

TEST (speedBoostDocDestroy_Test1, speedBoostDocDestroyNullCheck)
{
   // Call the function with NULL
    speedBoostDocDestroy(NULL);
    // No assertions are needed because we're just checking that it doesn't crash
}

TEST (speedBoostDocDestroy_Test2, speedBoostDocDestroyValidDoc)
{
    // Create a valid speedBoostDoc_t object
    speedBoostDoc_t* pSpeedDoc = (speedBoostDoc_t*) malloc (sizeof(speedBoostDoc_t));
    pSpeedDoc->pSchedulerInfo = nullptr;
    pSpeedDoc->pSubDocName = nullptr;

    // Call the function with the valid doc
    speedBoostDocDestroy(pSpeedDoc);
    // No assertions are needed because we're just checking that it doesn't crash
}

TEST_F (PandMSSPTestFixture, speedBoostDocDestroyValidDoc2)
{
    // Create a valid speedBoostDoc_t object
    speedBoostDoc_t* pSpeedDoc = (speedBoostDoc_t*) malloc(sizeof(speedBoostDoc_t));
    pSpeedDoc->pSchedulerInfo = (schedule_info_t*) malloc(sizeof(schedule_info_t));
    pSpeedDoc->pSubDocName = nullptr;

    // Set expectations on the freeScheduleInfo mock method
    EXPECT_CALL(*g_scheduleLibMock, freeScheduleInfo(_)).Times(1);

    // Call the function with the valid doc
    speedBoostDocDestroy(pSpeedDoc);
}

TEST_F (PandMSSPTestFixture, speedBoostDocDestroyValidDoc3)
{
    // Create a valid speedBoostDoc_t object
    speedBoostDoc_t* pSpeedDoc = (speedBoostDoc_t*) malloc (sizeof(speedBoostDoc_t));
    pSpeedDoc->pSchedulerInfo = (schedule_info_t*) malloc (sizeof(schedule_info_t));
    pSpeedDoc->pSubDocName = strdup("Test");

    // Set expectations on the freeScheduleInfo mock method
    EXPECT_CALL(*g_scheduleLibMock, freeScheduleInfo(_)).Times(1);

    // Call the function with the valid doc
    speedBoostDocDestroy(pSpeedDoc);
    // No assertions are needed because we're just checking that it doesn't crash
}

TEST (createScheduleInput_Test1, createScheduleInputCheck)
{
    // Call the function you're testing
    input_t *pInput = createScheduleInput(260);

    EXPECT_EQ(pInput, nullptr);
}

TEST (createScheduleInput_Test2, createScheduleInputCheck1)
{
    // Call the function you're testing
    input_t *pInput = createScheduleInput(0);

    EXPECT_NE(pInput, nullptr);
    if (NULL != pInput)
        free(pInput);
}

TEST (createScheduleInput_Test3, createScheduleInputCheck2)
{
    // Call the function you're testing
    input_t *pInput = createScheduleInput(150);

    EXPECT_NE(pInput, nullptr);
    if (pInput->action_count != 150)
        fprintf(stderr, "%s:%d: Action count is not matching\n", __FUNCTION__, __LINE__);
    if (NULL != pInput)
        free(pInput);
}

TEST (processSpeedBoostInfo_Test1, ProcessSpeedBoostInfo_NullParameters)
{
    // Act
    int result = processSpeedBoostInfo(NULL, NULL);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ERR);
}

TEST (processSpeedBoostInfo_Test2, ProcessSpeedBoostInfo_NullMsgPackObjKeyVal)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;

    // Initialize your objects here...
    msgPackObjMap.ptr = NULL;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ERR);
}

TEST (processSpeedBoostInfo_Test3, ProcessSpeedBoostInfo_NullStrPtrMsgPackObjKey)
{
    // Arrange
    schedule_info_t schedulerInfo  ;
    msgpack_object_map msgPackObjMap ;
    msgpack_object_kv msgPackObjKeyVal  ;
    msgpack_object msgPackObjVal ;
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));

    // Initialize your objects here...
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjMap.ptr->key.via.str.ptr = NULL;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ERR);
}

TEST (processSpeedBoostInfo_Test4, ProcessSpeedBoostInfo_StrSizeZeroMsgPackObjKey)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjMap.ptr->key.via.str.size = 0;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ERR);
}

TEST (processSpeedBoostInfo_Test5, ProcessSpeedBoostInfo_NotMatchingAbsoluteSchedule)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;

    // Set the key to a value that does not match ABSOLUTE_SCHEDULE
    msgPackObjMap.ptr->key.via.str.ptr = "NOT_ABSOLUTE_SCHEDULE";
    msgPackObjMap.ptr->key.via.str.size = strlen(msgPackObjMap.ptr->key.via.str.ptr);

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test6, ProcessSpeedBoostInfo_NotMatchingXMSpeedBoostRules)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;

    // Set the key to a value that does not match XM_SPEED_BOOST_RULES
    msgPackObjMap.ptr->key.via.str.ptr = "NOT_XM_SPEED_BOOST_RULES";
    msgPackObjMap.ptr->key.via.str.size = strlen("NOT_XM_SPEED_BOOST_RULES");

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test7, ProcessSpeedBoostInfo_DecodeScheduleTableSuccess)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = "absolute";
    msgPackObjMap.ptr->key.via.str.size = strlen("absolute");

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test8, ProcessSpeedBoostInfo_DecodeScheduleTableFails)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjVal.via.array.ptr = nullptr;
    msgPackObjVal.via.array.size = -1;
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = "absolute";
    msgPackObjMap.ptr->key.via.str.size = strlen("absolute");
    msgPackObjKeyVal.val.type = MSGPACK_OBJECT_ARRAY;
    schedulerInfo.absolute = nullptr;
    schedulerInfo.absolute_size = 0;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ABSOLUTE_ERR);
}

TEST (processSpeedBoostInfo_Test9, ProcessSpeedBoostInfo_DecodeScheduleTableEmptyArrayList)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjVal.via.array.ptr = nullptr;
    msgPackObjVal.via.array.size = 0;
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = "absolute";
    msgPackObjMap.ptr->key.via.str.size = strlen("absolute");
    msgPackObjKeyVal.val.type = MSGPACK_OBJECT_ARRAY;
    schedulerInfo.absolute = nullptr;
    schedulerInfo.absolute_size = 0;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test10, ProcessSpeedBoostInfo_DecodeScheduleTableObjMap)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjVal.via.array.size = 1;
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = "absolute";
    msgPackObjMap.ptr->key.via.str.size = strlen("absolute");
    msgPackObjKeyVal.val.type = MSGPACK_OBJECT_ARRAY;
    msgPackObjKeyVal.val.via.array.ptr = (msgpack_object*) malloc (sizeof(msgpack_object));
    msgPackObjKeyVal.val.via.array.ptr->type = MSGPACK_OBJECT_MAP;
    msgPackObjKeyVal.val.via.array.ptr->via.map.size = 0;
    msgPackObjKeyVal.val.via.array.ptr->via.map.ptr = nullptr;
    schedulerInfo.absolute = nullptr;
    schedulerInfo.absolute_size = 0;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test11, ProcessSpeedBoostInfo_DecodeScheduleTableProcessMap)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal, msgPackObjKeyVal1;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjVal.via.array.size = 1;
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = "absolute";
    msgPackObjMap.ptr->key.via.str.size = strlen("absolute");
    msgPackObjKeyVal.val.type = MSGPACK_OBJECT_ARRAY;
    msgPackObjKeyVal.val.via.array.ptr = (msgpack_object*) malloc (sizeof(msgpack_object));
    msgPackObjKeyVal.val.via.array.ptr->type = MSGPACK_OBJECT_MAP;
    msgPackObjKeyVal.val.via.array.ptr->via.map.size = 1;
    msgPackObjKeyVal.val.via.array.ptr->via.map.ptr = &msgPackObjKeyVal1;
    schedulerInfo.absolute = nullptr;
    schedulerInfo.absolute_size = 0;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test12, ProcessSpeedBoostInfo_DecodeScheduleTableProcessMap1)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal, msgPackObjKeyVal1;
    msgpack_object msgPackObjKey, msgPackObjKey1;
    msgpack_object msgPackObjVal, msgPackObjVal1;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKeyVal1, 0, sizeof(msgPackObjKeyVal1));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjKey1, 0, sizeof(msgPackObjKey1));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    memset (&msgPackObjVal1, 0, sizeof(msgPackObjVal1));
    msgPackObjVal.via.array.size = 1;
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = "absolute";
    msgPackObjMap.ptr->key.via.str.size = strlen("absolute");
    msgPackObjKeyVal.val.type = MSGPACK_OBJECT_ARRAY;
    msgPackObjKeyVal.val.via.array.ptr = (msgpack_object*) malloc (sizeof(msgpack_object));
    msgPackObjKeyVal.val.via.array.ptr->type = MSGPACK_OBJECT_MAP;
    msgPackObjKeyVal.val.via.array.ptr->via.map.size = 2;
    msgPackObjKeyVal.val.via.array.ptr->via.map.ptr = &msgPackObjKeyVal1;
    msgPackObjKeyVal1.key = msgPackObjKey1;
    msgPackObjKeyVal1.val = msgPackObjVal1;
    msgPackObjKeyVal1.key.type = MSGPACK_OBJECT_STR;
    msgPackObjKeyVal1.key.via.str.ptr = "unix_time";
    msgPackObjKeyVal1.val.type = MSGPACK_OBJECT_NIL;
    msgPackObjKeyVal1.val.via.u64 = 1234567890;
    schedulerInfo.absolute = nullptr;
    schedulerInfo.absolute_size = 0;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test13, ProcessSpeedBoostInfo_DecodeScheduleTableProcessMap2)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal, msgPackObjKeyVal1;
    msgpack_object msgPackObjKey, msgPackObjKey1;
    msgpack_object msgPackObjVal, msgPackObjVal1;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKeyVal1, 0, sizeof(msgPackObjKeyVal1));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjKey1, 0, sizeof(msgPackObjKey1));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    memset (&msgPackObjVal1, 0, sizeof(msgPackObjVal1));
    msgPackObjVal.via.array.size = 1;
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = "absolute";
    msgPackObjMap.ptr->key.via.str.size = strlen("absolute");
    msgPackObjKeyVal.val.type = MSGPACK_OBJECT_ARRAY;
    msgPackObjKeyVal.val.via.array.ptr = (msgpack_object*) malloc (sizeof(msgpack_object));
    msgPackObjKeyVal.val.via.array.ptr->type = MSGPACK_OBJECT_MAP;
    msgPackObjKeyVal.val.via.array.ptr->via.map.size = 2;
    msgPackObjKeyVal.val.via.array.ptr->via.map.ptr = &msgPackObjKeyVal1;
    msgPackObjKeyVal1.key = msgPackObjKey1;
    msgPackObjKeyVal1.val = msgPackObjVal1;
    msgPackObjKeyVal1.key.type = MSGPACK_OBJECT_STR;
    msgPackObjKeyVal1.key.via.str.ptr = "unix_time";
    msgPackObjKeyVal1.val.type = MSGPACK_OBJECT_POSITIVE_INTEGER;
    msgPackObjKeyVal1.val.via.u64 = 1234567890;
    schedulerInfo.absolute = nullptr;
    schedulerInfo.absolute_size = 0;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test14, ProcessSpeedBoostInfo_DecodeActionsTableFails)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;
    msgpack_object msgPackObjKey;
    msgpack_object msgPackObjVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;
    msgPackObjKeyVal.key = msgPackObjKey;
    msgPackObjKeyVal.val = msgPackObjVal;
    msgPackObjMap.ptr->key.via.str.ptr = XM_SPEED_BOOST_RULES;
    msgPackObjMap.ptr->key.via.str.size = strlen(XM_SPEED_BOOST_RULES);
    memset(&schedulerInfo.actions, 0, sizeof(schedulerInfo.actions));

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ACTIONS_ERR);
}

TEST (processSpeedBoostInfo_Test15, ProcessSpeedBoostInfo_TraverseWhileLoop)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal[2];
    msgpack_object msgPackObjKey[2];
    msgpack_object msgPackObjVal[2];

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjKey, 0, sizeof(msgPackObjKey));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjMap.ptr = msgPackObjKeyVal;
    msgPackObjMap.size = 2;
    msgPackObjKey[0].via.str.ptr = ABSOLUTE_SCHEDULE;
    msgPackObjKey[0].via.str.size = strlen(ABSOLUTE_SCHEDULE);
    msgPackObjKey[1].via.str.ptr = XM_SPEED_BOOST_RULES;
    msgPackObjKey[1].via.str.size = strlen(XM_SPEED_BOOST_RULES);
    msgPackObjKeyVal[0].key = msgPackObjKey[0];
    msgPackObjKeyVal[0].val = msgPackObjVal[0];
    msgPackObjKeyVal[1].key = msgPackObjKey[1];
    msgPackObjKeyVal[1].val = msgPackObjVal[1];
    memset(&schedulerInfo.actions, 0, sizeof(schedulerInfo.actions));

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ACTIONS_ERR);
}

TEST (processSpeedBoostInfo_Test16, ProcessSpeedBoostInfo_NullMsgPackObjKeyInLoop)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal[2];
    msgpack_object msgPackObjVal[2];

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    memset (&msgPackObjVal, 0, sizeof(msgPackObjVal));
    msgPackObjMap.ptr = msgPackObjKeyVal;
    msgPackObjMap.size = 2;
    memset(&msgPackObjKeyVal[0], 0, sizeof(msgPackObjKeyVal[0]));
    msgPackObjKeyVal[0].val = msgPackObjVal[0];
    memset(&msgPackObjKeyVal[1], 0, sizeof(msgPackObjKeyVal[1]));
    msgPackObjKeyVal[1].val = msgPackObjVal[1];

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ERR);
}

TEST (processSpeedBoostInfo_Test17, ProcessSpeedBoostInfo_EmptyMsgPackObjKeyVal)
{
    // Arrange
    schedule_info_t schedulerInfo;
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;

    // Initialize your objects here...
    memset (&msgPackObjMap, 0, sizeof(msgPackObjMap));
    memset (&msgPackObjKeyVal, 0, sizeof(msgPackObjKeyVal));
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 0; // No elements in the map

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, 0);
}

TEST (processSpeedBoostInfo_Test18, ProcessSpeedBoostInfo_NullmsgPackObjMap)
{
    // Arrange
    schedule_info_t schedulerInfo;

    // Act
    int result = processSpeedBoostInfo(&schedulerInfo, nullptr);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ERR);
}

TEST (processSpeedBoostInfo_Test19, ProcessSpeedBoostInfo_NullSchedulerInfo)
{
    // Arrange
    msgpack_object_map msgPackObjMap;
    msgpack_object_kv msgPackObjKeyVal;

    // Initialize your objects here...
    msgPackObjMap.ptr = &msgPackObjKeyVal;
    msgPackObjMap.size = 1;

    // Act
    int result = processSpeedBoostInfo(nullptr, &msgPackObjMap);

    // Assert
    EXPECT_EQ(result, SPEEDBOOST_INFO_ERR);
}
