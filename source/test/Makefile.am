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
AM_LDFLAGS = -lccsp_common
AM_CPPFLAGS = -Wall -g -Werror
AM_CXXFLAGS = -std=c++11


ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = CcspPandMSsp_gtest.bin

CcspPandMSsp_gtest_bin_CPPFLAGS = -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/gtest \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/ccsp \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/glib-2.0 \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(libdir)/glib-2.0/include \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/libqmi-glib \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/dbus-1.0 \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(libdir)/dbus-1.0/include \
                                  -I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/libsafec \
                                  -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rbus \
                                  -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/rtmessage \
                                  -I${top_srcdir}/gtest/include \
                                  -I${top_srcdir}/source \
                                  -I${top_srcdir}/source/TR-181/include \
                                  -I$(top_srcdir)/../hal/include \
                                  -I$(top_srcdir)/../CcspCommonLibrary/source/cosa/include/ \
                                  -I$(top_srcdir)/../CcspCommonLibrary/source/ccsp/components/include/ \
                                  -I$(top_srcdir)/source/Custom \
                                  -I$(top_srcdir)/source/TR-181/board_sbapi \
                                  -I$(top_srcdir)/source/TR-181/middle_layer_src \
                                  -I$(top_srcdir)/custom/comcast/source/TR-181/custom_include \
                                  -I$(top_srcdir)/custom/comcast/source/TR-181/custom_ml \
                                  -I$(top_srcdir)/custom/comcast \
                                  -I$(top_srcdir)/source-arm/TR-181/board_include \
                                  -I$(top_srcdir)/source-arm/TR-181/board_sbapi \
                                  -I$(top_srcdir)/source-arm/TR-181/board_ml
CcspPandMSsp_gtest_bin_LDADD = ${top_builddir}/source-arm/TR-181/board_sbapi/libCcspPandM_board_sbapi.la \
                               ${top_builddir}/source/TR-181/integration_src.shared/libCcspPandM_integration_src_shared.la
CcspPandMSsp_gtest_bin_SOURCES = ../TR-181/middle_layer_src/parodus_interface.c \
                                 ../TR-181/middle_layer_src/cosa_ipv6rd_dml.c \
                                 ../TR-181/middle_layer_src/plugin_main.c \
                                 ../TR-181/middle_layer_src/cosa_GRE_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_rlog_internal.c \
                                 ../TR-181/middle_layer_src/cosa_neighdisc_dml.c \
                                 ../TR-181/middle_layer_src/hotspotdoc.c \
                                 ../TR-181/middle_layer_src/cosa_bridging_internal.c \
                                 ../TR-181/middle_layer_src/cosa_dhcpv6_internal.c \
                                 ../TR-181/middle_layer_src/cosa_onboardlogging_dml.c \
                                 ../TR-181/middle_layer_src/cosa_interfacestack_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_diagnostics_dml.c \
                                 ../TR-181/middle_layer_src/plugin_main_apis.c \
                                 ../TR-181/middle_layer_src/webcfgparam.c \
                                 ../TR-181/middle_layer_src/cosa_ethernet_internal.c \
                                 ../TR-181/middle_layer_src/cosa_dslite_dml.c \
                                 ../TR-181/middle_layer_src/cosa_firewall_dml.c \
                                 ../TR-181/middle_layer_src/cosa_dns_dml.c \
                                 ../TR-181/middle_layer_src/portmappingdoc.c \
                                 ../TR-181/middle_layer_src/cosa_nat_dml.c \
                                 ../TR-181/middle_layer_src/cosa_hosts_internal.c \
                                 ../TR-181/middle_layer_src/cosa_dns_internal.c \
                                 ../TR-181/middle_layer_src/cosa_dhcpv4_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_comcast_com_parentalcontrol_dml.c \
                                 ../TR-181/middle_layer_src/cosa_bridging_dml.c \
                                 ../TR-181/middle_layer_src/cosa_dynamicdns_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_rdkcentral_com_videoservice_dml.c \
                                 ../TR-181/middle_layer_src/cosa_apis_util.c \
                                 ../TR-181/middle_layer_src/managedwifi_helpers.c \
                                 ../TR-181/middle_layer_src/cosa_apis_busutil.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_ddns_dml.c \
                                 ../TR-181/middle_layer_src/cosa_ppp_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_rdkcentral_com_xpc_internal.c \
                                 ../TR-181/middle_layer_src/cosa_x_comcast_com_parentalcontrol_internal.c \
                                 ../TR-181/middle_layer_src/cosa_advsec_utils.c \
                                 ../TR-181/middle_layer_src/cosa_neighdisc_internal.c \
                                 ../TR-181/middle_layer_src/managedwifidoc.c \
                                 ../TR-181/middle_layer_src/cosa_ppp_internal.c \
                                 ../TR-181/middle_layer_src/cosa_userinterface_internal.c \
                                 ../TR-181/middle_layer_src/cosa_ra_dml.c \
                                 ../TR-181/middle_layer_src/cosa_rbus_handler_apis.c \
                                 ../TR-181/middle_layer_src/cosa_time_internal.c \
                                 ../TR-181/middle_layer_src/array_helpers.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_multilan_dml.c \
                                 ../TR-181/middle_layer_src/cosa_interfacestack_internal.c \
                                 ../TR-181/middle_layer_src/cosa_users_internal.c \
                                 ../TR-181/middle_layer_src/cosa_dhcpv6_dml.c \
                                 ../TR-181/middle_layer_src/cosa_ra_internal.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_diagnostics_internal.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_ddns_internal.c \
                                 ../TR-181/middle_layer_src/cosa_routing_internal.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_mld_internal.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_rlog_dml.c \
                                 ../TR-181/middle_layer_src/cosa_deviceinfo_internal.c \
                                 ../TR-181/middle_layer_src/cosa_time_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_devicecontrol_internal.c \
                                 ../TR-181/middle_layer_src/cosa_ipv6rd_internal.c \
                                 ../TR-181/middle_layer_src/cosa_userinterface_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_mld_dml.c \
                                 ../TR-181/middle_layer_src/cosa_lanmanagement_internal.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_security_dml.c \
                                 ../TR-181/middle_layer_src/cosa_lanmanagement_dml.c \
                                 ../TR-181/middle_layer_src/cosa_dynamicdns_internal.c \
                                 ../TR-181/middle_layer_src/hotspot_helpers.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_security_internal.c \
                                 ../TR-181/middle_layer_src/cosa_users_dml.c \
                                 ../TR-181/middle_layer_src/cosa_firewall_internal.c \
                                 ../TR-181/middle_layer_src/cosa_gatewayinfo_dml.c \
                                 ../TR-181/middle_layer_src/cosa_dhcpv4_internal.c \
                                 ../TR-181/middle_layer_src/cosa_routing_dml.c \
                                 ../TR-181/middle_layer_src/cosa_ethernet_dml.c \
                                 ../TR-181/middle_layer_src/cosa_x_cisco_com_devicecontrol_dml.c \
                                 ../TR-181/middle_layer_src/cosa_dslite_internal.c \
                                 ../TR-181/middle_layer_src/cosa_x_rdkcentral_com_xpc_dml.c \
                                 ../TR-181/middle_layer_src/cosa_com_cisco_spvtg_ccsp_pam_dml.c \
                                 ../TR-181/middle_layer_src/cosa_upnp_dml.c \
                                 ../TR-181/middle_layer_src/cosa_webconfig_api.c \
                                 ../TR-181/middle_layer_src/cosa_nat_internal.c \
                                 ../TR-181/middle_layer_src/cosa_upnp_internal.c \
                                 ../TR-181/middle_layer_src/cosa_deviceinfo_dml.c \
                                 ../TR-181/middle_layer_src/cosa_nat_dml_custom.c \
                                 ../TR-181/middle_layer_src/cosa_onboardlogging_internal.c \
                                 ../TR-181/middle_layer_src/cosa_ip_internal.c \
                                 ../../custom/comcast/source/TR-181/custom_ml/cosa_x_comcast_com_gre_internal.c \
                                 ../../custom/comcast/source/TR-181/custom_sbapi/cosa_x_comcast_com_gre_apis.c \
                                 ../../custom/comcast/source/TR-181/custom_sbapi/cosa_deviceinfo_apis_custom.c \
                                 ../../custom/comcast/source/TR-181/custom_ml/cosa_deviceinfo_dml_custom.c \
                                 ../../custom/comcast/source/TR-181/custom_ml/cosa_x_cisco_com_hotspot_internal.c \
                                 ../../custom/comcast/source/TR-181/custom_sbapi/cosa_x_cisco_com_hotspot_apis.c \
                                 ../../custom/comcast/source/TR-181/custom_ml/cosa_x_cisco_com_truestaticip_internal.c \
                                 ../../custom/comcast/source/TR-181/custom_sbapi/cosa_x_cisco_com_truestaticip_apis.c \
                                 ../../custom/comcast/source/TR-181/custom_ml/cosa_x_comcast-com_gre_internal.c \
                                 ../../custom/comcast/source/TR-181/custom_sbapi/cosa_x_comcast-com_gre_apis.c \
                                 ../../custom/comcast/source/TR-181/custom_ml/cosa_x_cisco_com_gre_internal.c \
                                 ../../custom/comcast/source/TR-181/custom_sbapi/cosa_x_cisco_com_gre_apis.c \
                                 ../../source-arm/TR-181/board_ml/cosa_x_cisco_com_filetransfer_internal.c \
                                 ../../source-arm/TR-181/board_sbapi/cosa_deviceinfo_apis.c \
                                 ../../source-arm/TR-181/board_sbapi/cosa_ip_apis.c \
                                 ../../source-arm/TR-181/board_sbapi/cosa_bridging_apis.c \
                                 ../../source-arm/TR-181/board_sbapi/cosa_hosts_apis.c \
                                 ../../source-arm/TR-181/board_sbapi/cosa_x_cisco_com_devicecontrol_apis.c \
                                 ../../source-arm/TR-181/board_sbapi/cosa_ethernet_apis.c\
                                 ../PandMSsp/dm_pack_datamodel.c \
                                 rbus_handler_test.cpp \
                                 cosa_common_util_test.cpp \
                                 mocks/rbushandler_mock.cpp \
                                 mocks/commonutil_mock.cpp \
                                 pandmssp_mock.cpp \
                                 gtest_main.cpp

if FEATURE_SUPPORT_SPEED_BOOST
CcspPandMSsp_gtest_bin_SOURCES += ../TR-181/middle_layer_src/speedboost_apis.c \
                                  ../TR-181/middle_layer_src/speedBoostDoc.c \
                                  ../TR-181/middle_layer_src/speedboost_dml.c \
                                  ../TR-181/middle_layer_src/speedboost_rbus_handlers.c \
                                  ../TR-181/middle_layer_src/speedboost_event_publish.c \
                                  ../../source-arm/TR-181/board_sbapi/speedboost_webconfig_apis.c \
                                  ../../source-arm/TR-181/board_sbapi/speedboost_scheduler_apis.c
endif

CcspPandMSsp_gtest_bin_LDFLAGS = -lgtest -lgmock -lgcov -pthread -lccsp_common -ldl -lrbus $(SYSTEMD_LDFLAGS) -lrdkloggers -lsyscfg -lnanomsg -lsysevent -lwebconfig_framework -lmsgpackc -lcurl -ltrower-base64 -lsecure_wrapper -lrtMessage -lhal_platform -lhal_moca -lapi_dhcpv4c -lhal_msomgmt -lhal_ethsw -llmapi -lcm_mgnt -lutapi -lutctx -llibparodus -lcrypto -lulog

