##########################################################################
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2021 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
AM_CFLAGS = -D_ANSC_LINUX
AM_CFLAGS += -D_ANSC_USER -DWBCFG_MULTI_COMP_SUPPORT

AM_CPPFLAGS = -Wall -g -Werror
AM_CXXFLAGS = -std=c++11

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = CcspPandMSsp_gtest.bin

CcspPandMSsp_gtest_bin_CPPFLAGS = -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/gtest \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/ccsp \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/dbus-1.0 \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(libdir)/dbus-1.0/include \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/libsafec \
                                  -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rbus \
                                  -I${top_srcdir}/gtest/include \
                                  -I${top_srcdir}/source \
                                  -I${top_srcdir}/source/TR-181/include \
                                  -I$(top_srcdir)/../hal/include \
                                  -I$(top_srcdir)/../CcspCommonLibrary/source/cosa/include/ \
                                  -I$(top_srcdir)/../CcspCommonLibrary/source/ccsp/components/include/ \
                                  -I$(top_srcdir)/source/Custom \
                                  -I$(top_srcdir)/source/PandMSsp \
                                  -I$(top_srcdir)/source/TR-181/board_sbapi \
                                  -I$(top_srcdir)/source/TR-181/middle_layer_src \
                                  -I$(top_srcdir)/custom/comcast/source/TR-181/custom_include \
                                  -I$(top_srcdir)/custom/comcast/source/TR-181/custom_ml \
                                  -I$(top_srcdir)/custom/comcast \
                                  -I$(top_srcdir)/source-arm/TR-181/board_include \
                                  -I$(top_srcdir)/source-arm/TR-181/board_sbapi \
                                  -I$(top_srcdir)/source-arm/TR-181/board_ml \
                                  -DRBUS_WAN_IP \
                                  -DRDKB_EXTENDER_ENABLED \
                                  -DFEATURE_SUPPORT_SPEED_BOOST \
                                  -DWAN_FAILOVER_SUPPORTED

CcspPandMSsp_gtest_bin_SOURCES = pandmssp_mock.cpp \
                                 $(top_srcdir)/source-arm/TR-181/board_sbapi/cosa_common_util.c \
                                 $(top_srcdir)/source/TR-181/middle_layer_src/cosa_rbus_handler_apis.c \
                                 $(top_srcdir)/source-arm/TR-181/board_sbapi/speedboost_webconfig_apis.c \
                                 $(top_srcdir)/source/TR-181/middle_layer_src/speedBoostDoc.c \
                                 rbus_handler_test.cpp \
                                 cosa_common_util_test.cpp \
                                 speedBoostWebconfigApisGtest.cpp \
                                 speedBoostDocGtest.cpp \
                                 mocks/rbushandler_mock.cpp \
                                 mocks/speedboostSchedulerApisMock.cpp \
                                 mocks/speedboostRbusHandlersMock.cpp \
                                 mocks/helpersMock.cpp \
                                 mocks/cosaWebconfigApisMock.cpp \
                                 gtest_main.cpp

CcspPandMSsp_gtest_bin_LDFLAGS = -lgtest -lgmock -lgcov

CcspPandMSsp_gtest_bin_LDADD = $(HOME)/usr/local/lib/libmock_ccsp_common.la \
                               $(HOME)/usr/local/lib/libmock_rbus.la \
                               $(HOME)/usr/local/lib/libmock_sysevent.la \
                               $(HOME)/usr/local/lib/libmock_utopia.la \
                               $(HOME)/usr/local/lib/libmock_safec_lib.la \
                               $(HOME)/usr/local/lib/libmock_syscfg.la \
                               $(HOME)/usr/local/lib/libmock_scheduler.la \
                               $(HOME)/usr/local/lib/libmock_webconfigframework.la \
                               $(HOME)/usr/local/lib/libmock_psm.la \
                               $(HOME)/usr/local/lib/libmock_pthread.la

AM_CFLAGS += -fprofile-arcs -ftest-coverage
AM_CXXFLAGS += -fprofile-arcs -ftest-coverage
CcspPandMSsp_gtest_bin_LDFLAGS += --coverage