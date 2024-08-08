/*
* If not stated otherwise in this file or this component's LICENSE file the
* following copyright and licenses apply:
*
* Copyright 2021 RDK Management
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
*/ 

#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdbool.h>
#include <stdint.h>
#include <ulog/ulog.h>
#include <openssl/hmac.h>

extern "C"
{
#include <ulog/ulog.h>
#include <openssl/hmac.h>
#include "cosa_x_cisco_com_ddns_internal.h"
#include "cosa_onboardlogging_dml.h"
#include "cosa_dns_internal.h"
#include "cosa_bridging_internal.h"
#include "cosa_deviceinfo_dml.h"
#include "cosa_x_comcast_com_parentalcontrol_internal.h"
#include "cosa_onboardlogging_internal.h"
#include "cosa_ra_internal.h"
#include "portmappingdoc.h"
#include "cosa_dynamicdns_internal.h"
#include "cosa_advsec_utils.h"
#include "speedboost_apis.h"
#include "speedboost_scheduler_apis.h"
#include "speedboost_dml.h"
#include "speedboost_rbus_handler.h"
#include "speedboost_webconfig_apis.h"
#include "cosa_x_rdkcentral_com_xpc_dml.h"
#include "cosa_lanmanagement_internal.h"
#include "cosa_dns_dml.h"
#include "plugin_main_apis.h"
#include "cosa_firewall_dml.h"
#include "cosa_time_dml.h"
#include "cosa_x_cisco_com_mld_dml.h"
#include "cosa_x_cisco_com_diagnostics_dml.h"
#include "cosa_ethernet_internal.h"
#include "cosa_upnp_internal.h"
#include "cosa_managedwifi_dml.h"
#include "cosa_com_cisco_spvtg_ccsp_pam_dml.h"
#include "cosa_x_cisco_com_rlog_internal.h"
#include "cosa_apis_util.h"
#include "cosa_nat_dml.h"
#include "naf.h"
#include "cosa_nat_internal.h"
#include "cosa_x_cisco_com_rlog_dml.h"
#include "cosa_users_internal.h"
#include "cosa_x_cisco_com_security_dml.h"
#include "cosa_bridging_dml.h"
#include "cosa_ipv6rd_internal.h"
#include "managedwifidoc.h"
#include "cosa_hosts_dml.h"
#include "cosa_common_util.h"
#include "cosa_upnp_dml.h"
#include "cosa_x_cisco_com_mld_internal.h"
#include "cosa_firewall_internal.h"
#include "cosa_ppp_dml.h"
#include "hotspot_helpers.h"
#include "cosa_apis_busutil.h"
#include "cosa_x_cisco_com_devicecontrol_internal.h"
#include "cosa_interfacestack_internal.h"
#include "cosa_x_rdkcentral_com_videoservice_dml.h"
#include "cosa_routing_dml.h"
#include "cosa_dynamicdns_dml.h"
#include "cosa_dslite_dml.h"
#include "hotspotdoc.h"
#include "cosa_gatewayinfo_dml.h"
#include "cosa_interfacestack_dml.h"
#include "cosa_neighdisc_dml.h"
#include "cosa_deviceinfo_internal.h"
#include "cosa_time_internal.h"
#include "cosa_ipv6rd_dml.h"
#include "cosa_x_cisco_com_multilan_dml.h"
#include "cosa_x_cisco_com_security_internal.h"
#include "cosa_users_dml.h"
#include "cosa_x_cisco_com_multilan_internal.h"
#include "cosa_ra_dml.h"
#include "cosa_userinterface_internal.h"
#include "cosa_x_comcast_com_parentalcontrol_dml.h"
#include "cosa_x_rdkcentral_com_xpc_internal.h"
#include "cosa_x_cisco_com_diagnostics_internal.h"
#include "cosa_dhcpv6_dml.h"
#include "cosa_dhcpv4_dml.h"
#include "cosa_lanmanagement_dml.h"
#include "plugin_main.h"
#include "cosa_x_cisco_com_devicecontrol_dml.h"
#include "cosa_dslite_internal.h"
#include "cosa_ip_dml.h"
#include "cosa_ethernet_dml.h"
#include "cosa_apis.h"
#include "cosa_ppp_internal.h"
#include "cosa_GRE_dml.h"
#include "webcfgparam.h"
#include "cosa_rbus_handler_apis.h"
#include "cosa_userinterface_dml.h"
#include "cosa_dml_api_common.h"
#include "cosa_webconfig_api.h"
#include "dml_tr181_custom_cfg.h"
#include "cosa_GRE_webconfig_apis.h"
#include "cosa_drg_common.h"
#include "dmz_wan_doc.h"
#include "dmsb_tr181_psm_definitions.h"
#include "cosa_nat_webconfig_apis.h"
#include "cosa_ra_apis.h"
#include "cosa_ip_apis.h"
#include "cosa_dhcpv4_apis.h"
#include "cosa_interfacestack_apis.h"
#include "cosa_time_apis.h"
#include "cosa_x_cisco_com_ddns_apis.h"
//#include "cosa_routing_apis.h"
#include "cosa_neighdisc_apis.h"
#include "cosa_bridging_apis.h"
#include "cosa_ipv6rd_apis.h"
#include "cosa_ppp_apis.h"
#include "cosa_deviceinfo_apis.h"
#include "cosa_nat_apis_custom.h"
#include "cosa_firewall_apis.h"
#include "cosa_dslite_apis.h"
#include "cosa_x_cisco_com_security_apis.h"
#include "cosa_dhcpv4_webconfig_apis.h"
#include "cosa_nat_apis.h"
#include "cosa_x_cisco_com_diagnostics_apis.h"
#include "cosa_lanmanagement_apis.h"
#include "cosa_gatewayinfo_apis.h"
#include "cosa_users_apis.h"
#include "cosa_x_cisco_com_rlog_apis.h"
//#include "cosa_ip_apis_multilan.h"
#include "cosa_mapt_apis.h"
#include "lan_webconfig_param.h"
#include "cosa_x_cisco_com_filetransfer_apis.h"
#include "cosa_x_cisco_com_devicecontrol_apis.h"
#include "cosa_x_cisco_com_mld_apis.h"
#include "cosa_x_cisco_com_multilan_apis.h"
#include "cosa_hosts_apis.h"
#include "cosa_userinterface_apis.h"
#include "macbinding_webconfig_param.h"
#include "cosa_x_comcast_com_parentalcontrol_apis.h"
#include "cosa_upnp_apis.h"
#include "cosa_ethernet_apis_multilan.h"
#include "cosa_nat_dml_custom.h"
#include "naf_api.h"
#include "cosa_dml_api_dns.h"
#include "cosa_ethernet_apis.h"
#include "cosa_dynamicdns_apis.h"
#include <rbus.h>
}

#define GTEST_DEFAULT_RESULT_FILEPATH "/tmp/Gtest_Report/"
#define GTEST_DEFAULT_RESULT_FILENAME "CcspPandMSsp_gtest_report.json"
#define GTEST_REPORT_FILEPATH_SIZE 128

class PandMSSPEnvironment : public ::testing::Environment
{
    public:
        virtual ~PandMSSPEnvironment()
        {
        }
        virtual void SetUp()
        {
            printf("%s Environment\n", __func__);
        }
        virtual void TearDown()
        {
            printf("%s Environment\n", __func__);
        }
};

GTEST_API_ int main(int argc, char *argv[])
{
    char testresults_fullfilepath[GTEST_REPORT_FILEPATH_SIZE];
    char buffer[GTEST_REPORT_FILEPATH_SIZE];

    memset( testresults_fullfilepath, 0, GTEST_REPORT_FILEPATH_SIZE );
    memset( buffer, 0, GTEST_REPORT_FILEPATH_SIZE );

    snprintf( testresults_fullfilepath, GTEST_REPORT_FILEPATH_SIZE, "json:%s%s" , GTEST_DEFAULT_RESULT_FILEPATH , GTEST_DEFAULT_RESULT_FILENAME);
    ::testing::GTEST_FLAG(output) = testresults_fullfilepath;
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

