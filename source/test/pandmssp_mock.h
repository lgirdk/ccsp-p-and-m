#ifndef PANDMSSP_FIXTURE_H
#define PANDMSSP_FIXTURE_H

#include "gtest/gtest.h"
#include "mocks/rbushandler_mock.h"
#include "mocks/commonutil_mock.h"

class PandMSSPTestFixture : public ::testing::Test {
  protected:
        rbusHandlerMock mockedrbushandler;
        rbusCommonUtilMock mockedrbusCommonUtil;

        PandMSSPTestFixture();
        virtual ~PandMSSPTestFixture();
        virtual void SetUp() override;
        virtual void TearDown() override;

        void TestBody() override;
};

#endif
