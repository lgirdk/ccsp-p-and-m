#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <experimental/filesystem>
#include "pandmssp_mock.h"
#include "mocks/rbushandler_mock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define MOCK_RBUS_HANDLER_PAM_DML

using namespace std;
using std::experimental::filesystem::exists;
using ::testing::_;
using ::testing::Return;
using ::testing::StrEq;

extern "C"
{
#include "cosa_rbus_handler_apis.h"
#include <rbus.h>
#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED)
extern rbusError_t publishDevCtrlNetMode(uint32_t new_val, uint32_t old_val);
#endif
extern rbusError_t publishWanIpAddr(char* event_name, char* new_val, char* old_val);
extern rbusError_t sendUpdateEvent(char* event_name , void* eventNewData, void* eventOldData, rbusValueType_t rbus_type);
extern char const* GetParamName(char const* path);
extern rbusError_t getStringHandlerWANIP_RBUS(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t *opts); 
extern rbusError_t eventWANIPSubHandler(rbusHandle_t handle, rbusEventSubAction_t action, const char *eventName, rbusFilter_t filter, int32_t interval, bool *autoPublish);
extern rbusError_t devCtrlRbusInit();
}

char  g_Subsystem[32]         = {0};
extern rbusHandlerMock * g_rbusHandlerMock;

//Failure case for sendUpdateEvent
TEST (sendUpdateEventTest1, sendUpdateEvent_Test1)
{
        void* new_val = nullptr;
        uint32_t old_val = 1;
        rbusValueType_t rbus_type = RBUS_INT32;
        char* pParamName = "Device.X_RDKCENTRAL-COM_DeviceControl.DeviceNetworkingMode";

        rbusError_t ret = sendUpdateEvent(pParamName,new_val,(void*)&old_val,rbus_type);
        EXPECT_EQ(ret, RBUS_ERROR_BUS_ERROR);
}

TEST (sendUpdateEventTest2, sendUpdateEvent_Test2)
{
        uint32_t new_val = 0;
        uint32_t old_val = 1;
        rbusValueType_t rbus_type = RBUS_INT32;
        char* pParamName = nullptr;

        rbusError_t ret = sendUpdateEvent(pParamName,(void*)&new_val,(void*)&old_val,rbus_type);
        EXPECT_EQ(ret, RBUS_ERROR_BUS_ERROR);
}

TEST (sendUpdateEventTest3, sendUpdateEvent_Test3)
{
        uint32_t new_val = 0;
        uint32_t old_val = 1;
        rbusValueType_t rbus_type = RBUS_INT32;
        char* pParamName = "Device.X_RDKCENTRAL-COM_DeviceControl.DeviceNetworkingMode";
        rbusHandle_t handle = nullptr;

        rbusError_t ret = sendUpdateEvent(pParamName,(void*)&new_val,(void*)&old_val,rbus_type);
        EXPECT_EQ(ret, RBUS_ERROR_BUS_ERROR);
}

TEST_F(PandMSSPTestFixture, GetParamName_1) {
    const char* path = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";
    EXPECT_STREQ(GetParamName(path), "X_COMCAST-COM_WAN_IP");
}

TEST_F(PandMSSPTestFixture, GetParamName_2) {
    const char* path = "X_COMCAST-COM_WAN_IPv6";
    EXPECT_STREQ(GetParamName(path), "X_COMCAST-COM_WAN_IPv6");
}

TEST_F(PandMSSPTestFixture, GetParamName_3) {
    const char* path = ".X_COMCAST-COM_WAN_IP";
    EXPECT_STREQ(GetParamName(path), "X_COMCAST-COM_WAN_IP");
}

#define MOCK_RBUS_HANDLER_PAM

//TODO:
#if  defined  (WAN_FAILOVER_SUPPORTED) || defined(RDKB_EXTENDER_ENABLED)
extern unsigned int gSubscribersCount;

//Failure case for publishDevCtrlNetMode
TEST_F (PandMSSPTestFixture, publishDevCtrlNetMode_Test1)
{
        uint32_t new_val = 0;
        uint32_t old_val = 1;
        gSubscribersCount = 1;
        char* pParamName = "Device.X_RDKCENTRAL-COM_DeviceControl.DeviceNetworkingMode";

        ON_CALL(*g_rbusHandlerMock, sendUpdateEvent(StrEq(pParamName), _, _, RBUS_INT32))
        .WillByDefault(Return(RBUS_ERROR_BUS_ERROR));

        rbusError_t ret = publishDevCtrlNetMode(new_val, old_val);
        EXPECT_EQ(ret, RBUS_ERROR_BUS_ERROR);
}

//No subscribers
TEST (PandMSSPTest_1, publishDevCtrlNetMode_Test1)
{
        rbusError_t ret = RBUS_ERROR_SUCCESS;
        unsigned int new_val = 0;
        unsigned int old_val = 1;
        gSubscribersCount = 0;

        ret = publishDevCtrlNetMode(new_val, old_val);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}
#endif

//Failure case for publishWanIpAddr
TEST_F (PandMSSPTestFixture, publishWanIpAddr_Test1)
{
        char* event_name = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";
        char* new_val = "67.164.67.77";
        char* old_val = "N.A";

        ON_CALL(*g_rbusHandlerMock, sendUpdateEvent(StrEq(event_name), _, _, RBUS_STRING))
        .WillByDefault(Return(RBUS_ERROR_BUS_ERROR));

        rbusError_t ret = publishWanIpAddr(event_name, new_val, old_val);
        EXPECT_EQ(ret, RBUS_ERROR_BUS_ERROR);
}

extern unsigned int gSubscribersCount_IPv4;
extern unsigned int gSubscribersCount_IPv6;

TEST (eventWANIPSubHandlerTest1, eventWANIPSubHandler_Test1)
{
        rbusHandle_t handle = NULL;
        rbusEventSubAction_t action = RBUS_EVENT_ACTION_SUBSCRIBE;
        const char *eventName = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";
        rbusFilter_t filter = NULL;
        int32_t interval = 1000;
        bool autoPublish = false;
        gSubscribersCount_IPv4 = 1;

        EXPECT_EQ(eventName,"Device.DeviceInfo.X_COMCAST-COM_WAN_IP");
        EXPECT_EQ(action, RBUS_EVENT_ACTION_SUBSCRIBE);
        gSubscribersCount_IPv4 += 1;

        rbusError_t ret = eventWANIPSubHandler(handle, action, eventName, filter, interval, &autoPublish);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}

TEST (eventWANIPSubHandlerTest2, eventWANIPSubHandler_Test2)
{
        rbusHandle_t handle = NULL;
        rbusEventSubAction_t action = RBUS_EVENT_ACTION_UNSUBSCRIBE;
        const char *eventName = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";
        rbusFilter_t filter = NULL;
        int32_t interval = 1000;
        bool autoPublish = false;
        gSubscribersCount_IPv4 = 1;

        EXPECT_EQ(eventName,"Device.DeviceInfo.X_COMCAST-COM_WAN_IP");
        EXPECT_EQ(action, RBUS_EVENT_ACTION_UNSUBSCRIBE);
        EXPECT_NE(0, gSubscribersCount_IPv4);
        gSubscribersCount_IPv4 -= 1;

        rbusError_t ret = eventWANIPSubHandler(handle, action, eventName, filter, interval, &autoPublish);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}


TEST (eventWANIPSubHandlerTest3, eventWANIPSubHandler_Test3)
{
        rbusHandle_t handle = NULL;
        rbusEventSubAction_t action = RBUS_EVENT_ACTION_SUBSCRIBE;
        const char *eventName = "Device.DeviceInfo.X_COMCAST-COM_WAN_IPv6";
        rbusFilter_t filter = NULL;
        int32_t interval = 1000;
        bool autoPublish = false;
        gSubscribersCount_IPv6 = 1;

        EXPECT_EQ(eventName,"Device.DeviceInfo.X_COMCAST-COM_WAN_IPv6");
        EXPECT_EQ(action, RBUS_EVENT_ACTION_SUBSCRIBE);
        gSubscribersCount_IPv6 += 1;

        rbusError_t ret = eventWANIPSubHandler(handle, action, eventName, filter, interval, &autoPublish);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}


TEST (eventWANIPSubHandlerTest4, eventWANIPSubHandler_Test4)
{
        rbusHandle_t handle = NULL;
        rbusEventSubAction_t action = RBUS_EVENT_ACTION_UNSUBSCRIBE;
        const char *eventName = "Device.DeviceInfo.X_COMCAST-COM_WAN_IPv6";
        rbusFilter_t filter = NULL;
        int32_t interval = 1000;
        bool autoPublish = false;
        gSubscribersCount_IPv6 = 1;

        EXPECT_EQ(eventName,"Device.DeviceInfo.X_COMCAST-COM_WAN_IPv6");
        EXPECT_EQ(action, RBUS_EVENT_ACTION_UNSUBSCRIBE);
        EXPECT_NE(0, gSubscribersCount_IPv6);
        gSubscribersCount_IPv6 -= 1;

        rbusError_t ret = eventWANIPSubHandler(handle, action, eventName, filter, interval, &autoPublish);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}

TEST (eventWANIPSubHandlerTest5, eventWANIPSubHandler_Test5)
{
        rbusHandle_t handle = NULL;
        rbusEventSubAction_t action;
        const char *eventName = "Device.DeviceInfo.X_COMCAST-COM_WAN_IP";
        rbusFilter_t filter = NULL;
        int32_t interval = 1000;
        bool autoPublish = false;
        gSubscribersCount_IPv4 = 0;

        rbusError_t ret = eventWANIPSubHandler(handle, action, eventName, filter, interval, &autoPublish);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}

TEST (eventWANIPSubHandlerTest6, eventWANIPSubHandler_Test6)
{
        rbusHandle_t handle = NULL;
        rbusEventSubAction_t action;
        const char *eventName = "Device.DeviceInfo.X_COMCAST-COM_WAN_IPv6";
        rbusFilter_t filter = NULL;
        int32_t interval = 1000;
        bool autoPublish = false;
        gSubscribersCount_IPv6 = 1;

        rbusError_t ret = eventWANIPSubHandler(handle, action, eventName, filter, interval, &autoPublish);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}

TEST(devCtrlRbusTest1, devCtrlRbusInit_Test2)
{
        int rc = RBUS_ERROR_SUCCESS;

        rbusError_t ret = devCtrlRbusInit();
        EXPECT_EQ(ret, rc);
}

/* TODO:
TEST_F (PandMSSPTestFixture, devCtrlRbusInit_Test1)
{
        int rc = RBUS_ERROR_SUCCESS;
        rbusStatus_t status = RBUS_ENABLED;
        
        EXPECT_CALL(*g_rbusHandlerMock, rbus_checkStatus())
            .Times(1)
            .WillOnce(Return(RBUS_DISABLED));

        EXPECT_NE(status, RBUS_DISABLED);
       
        rbusError_t ret = devCtrlRbusInit();
        EXPECT_EQ(ret, RBUS_ERROR_BUS_ERROR);
}

TEST_F (PandMSSPTestFixture, devCtrlRbusInit_Test3)
{
        int rc = RBUS_ERROR_SUCCESS;

        EXPECT_CALL(*g_rbusHandlerMock, rbus_checkStatus())
            .Times(1)
            .WillOnce(Return(RBUS_ENABLED));

        rbusError_t ret = devCtrlRbusInit();
        EXPECT_EQ(ret, rc);
}

TEST_F (PandMSSPTestFixture, devCtrlRbusInit_Test1)
{
        int rc = RBUS_ERROR_SUCCESS;
        //rbusHandle_t handle = NULL;


        EXPECT_CALL(*g_rbusHandlerMock, rbus_open( _, "CcspPandMSsp"))
            .Times(1)
            .WillOnce(Return(RBUS_ERROR_SUCCESS));	
        EXPECT_EQ(rc, RBUS_ERROR_SUCCESS);


        EXPECT_CALL(*g_rbusHandlerMock, rbus_regDataElements( _, _, _))
            .Times(1)
            .WillOnce(Return(RBUS_ERROR_SUCCESS));
        EXPECT_EQ(rc, RBUS_ERROR_SUCCESS);


        EXPECT_CALL(*g_rbusHandlerMock, PAM_Rbus_SyseventInit())
            .Times(1)
            .WillOnce(Return(TRUE));

        rbusError_t ret = devCtrlRbusInit();
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}


TEST_F (PandMSSPTestFixture, getStringHandlerWANIP_RBUS_Test1)
{
        rbusHandle_t handle = NULL;
        rbusGetHandlerOptions_t opts;
        rbusProperty_t property;
        char val[256];
        ULONG ulen = 256;

        EXPECT_CALL(*g_rbusHandlerMock, DeviceInfo_GetParamStringValue(NULL, "X_COMCAST-COM_WAN_IP", _, _))
            .Times(1)
            .WillOnce(Return(0));

        rbusError_t ret = getStringHandlerWANIP_RBUS(handle, property, &opts);
        EXPECT_EQ(ret, RBUS_ERROR_SUCCESS);
}
*/

