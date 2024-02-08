#ifndef MOCK_RBUS_HANDLER_H
#define MOCK_RBUS_HANDLER_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdio.h>
#include <iostream>
#include "ansc_platform.h"

extern "C"
{
#include <rbus.h>
#include "cosa_deviceinfo_dml.h"
#include "cosa_ip_apis.h"
#include "plugin_main_apis.h"
#include "cosa_ip_internal.h"
#include "cosa_ip_dml.h"
}

class rbusHandlerInterface {
public:
        virtual ~rbusHandlerInterface() {}
        virtual rbusError_t sendUpdateEvent(char*, void*, void*, rbusValueType_t) = 0;
        virtual ULONG DeviceInfo_GetParamStringValue(ANSC_HANDLE, char*, char*, ULONG*) = 0;
        virtual void get_uptime(long *) = 0;
        virtual rbusError_t rbusEvent_Publish(rbusHandle_t, rbusEvent_t*) = 0;
        virtual char const* rbusProperty_GetName(rbusProperty_t) = 0;
        virtual char const* GetParamName(char const*) = 0;
        virtual rbusStatus_t rbus_checkStatus() = 0;
        virtual rbusError_t rbus_open(rbusHandle_t*, char const*) = 0;
        virtual rbusError_t rbus_regDataElements(rbusHandle_t, int, rbusDataElement_t) = 0;
        virtual bool PAM_Rbus_SyseventInit() = 0;
};

class rbusHandlerMock: public rbusHandlerInterface {
public:
        virtual ~rbusHandlerMock() {}
        MOCK_METHOD4(sendUpdateEvent, rbusError_t(char*, void*, void*, rbusValueType_t));
        MOCK_METHOD4(DeviceInfo_GetParamStringValue, ULONG(ANSC_HANDLE, char*, char*, ULONG*));
        MOCK_METHOD1(get_uptime, void(long *));
        MOCK_METHOD2(rbusEvent_Publish, rbusError_t(rbusHandle_t, rbusEvent_t*));
        MOCK_METHOD1(rbusProperty_GetName, char const*(rbusProperty_t));
        MOCK_METHOD1(GetParamName, char const*(char const*));
        MOCK_METHOD(rbusStatus_t, rbus_checkStatus, ());
        MOCK_METHOD2(rbus_open, rbusError_t(rbusHandle_t*, char const*));
        MOCK_METHOD3(rbus_regDataElements, rbusError_t(rbusHandle_t, int, rbusDataElement_t));
        MOCK_METHOD0(PAM_Rbus_SyseventInit, bool());
};
#endif

