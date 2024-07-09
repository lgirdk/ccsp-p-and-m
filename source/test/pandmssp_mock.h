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
#ifndef PANDMSSP_FIXTURE_H
#define PANDMSSP_FIXTURE_H

#include "gtest/gtest.h"
#include "mocks/rbushandler_mock.h"
#include "mocks/helpersMock.h"
#include "mocks/cosaWebconfigApisMock.h"
#include "mocks/speedboostRbusHandlersMock.h"
#include "mocks/speedboostSchedulerApisMock.h"
#include <mocks/mock_utopia.h>
#include <mocks/mock_rbus.h>
#include <mocks/mock_usertime.h>
#include <mocks/mock_trace.h>
#include <mocks/mock_sysevent.h>
#include <mocks/mock_syscfg.h>
#include <mocks/mock_safec_lib.h>
#include <mocks/mock_pthread.h>
#include <mocks/mock_psm.h>
#include <mocks/mock_base_api.h>
#include <mocks/mock_scheduler.h>
#include <mocks/mock_webconfigframework.h>

class PandMSSPTestFixture : public ::testing::Test {
  protected:
        rbusHandlerMock mockedrbushandler;
        PsmMock mockedPsm;
        BaseAPIMock mockedbaseapi;
        rbusMock mockedRbus;
        SafecLibMock mockedSafecLibMock;
        utopiaMock mockedUtopia;
        UserTimeMock mockedUserTime;
        TraceMock mockedTrace;
        SyscfgMock mockedSyscfg;
        PtdHandlerMock mockedPtdHandler;
        helpersMock mockedhelpers;
        cosaWebconfigApisMock mockedcosaWebconfig;
        speedboostRbusHandlersMock mockedspeedboostRbus;
        speedboostSchedulerApisMock mockedspeedboostScheduler;
        webconfigFwMock mockedwebconfigFwMock;
        scheduleLibMock mockedscheduleLibMock;

        PandMSSPTestFixture();
        virtual ~PandMSSPTestFixture();
        virtual void SetUp() override;
        virtual void TearDown() override;

        void TestBody() override;
};

#endif