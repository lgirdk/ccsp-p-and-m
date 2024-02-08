#include <gmock/gmock.h>
#include "pandmssp_mock.h"

#ifndef PANDMSSP_FIXTURE_G_MOCK
#define PANDMSSP_FIXTURE_G_MOCK

rbusHandlerMock * g_rbusHandlerMock = nullptr;
rbusCommonUtilMock * g_rbusCommonUtilMock = nullptr;

PandMSSPTestFixture::PandMSSPTestFixture()
{
    g_rbusHandlerMock = new rbusHandlerMock;
    g_rbusCommonUtilMock = new rbusCommonUtilMock;
}

PandMSSPTestFixture::~PandMSSPTestFixture()
{
    delete g_rbusHandlerMock;
    delete g_rbusCommonUtilMock;

    g_rbusHandlerMock = nullptr;
    g_rbusCommonUtilMock = nullptr;
}

void PandMSSPTestFixture::SetUp()
{
}

void PandMSSPTestFixture::TearDown() {}
void PandMSSPTestFixture::TestBody() {}

void get_uptime(long* uptime) {
        // Provide a dummy implementation here
        *uptime = 0; // Or any other suitable value
    }
#endif
