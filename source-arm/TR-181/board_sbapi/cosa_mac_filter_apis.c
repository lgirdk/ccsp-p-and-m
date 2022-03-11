 /*********************************************************************
 * Copyright 2017-2019 ARRIS Enterprises, LLC.
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
 **********************************************************************/

#include <ctype.h>
#include "cosa_mac_filter_apis.h"
#include <utctx/utctx.h>
#include <utapi/utapi.h>
#include <utapi/utapi_util.h>
#include "cosa_drg_common.h"
#include <syscfg/syscfg.h>

static int g_NrFwMACFilter =  0;

static int
FW_MACFilter_InsGetIndex(ULONG ins)
{
    int i, ins_num, ret = -1;
    UtopiaContext ctx;

    CosaDmlFW_MACFilter_GetNumberOfEntries();

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    for (i = 0; i < g_NrFwMACFilter; i++)
    {
        Utopia_GetMACFilterInsNumByIndex(&ctx, i, &ins_num);
        if (ins_num == ins) {
            ret = i;
            break;
        }
    }

    Utopia_Free(&ctx, 0);

    return ret;
}

ULONG
CosaDmlFW_MACFilter_GetNumberOfEntries(void)
{
    UtopiaContext ctx;

    if(!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfMACFilter(&ctx, &g_NrFwMACFilter);

    Utopia_Free(&ctx, 0);

    return g_NrFwMACFilter;
}

ANSC_STATUS
CosaDmlFW_MACFilter_GetEntryByIndex(ULONG index, COSA_DML_FW_MACFILTER *pEntry)
{
    UtopiaContext ctx;
    fwmacfilter_t MACFilter;

    if (index >= g_NrFwMACFilter || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetMACFilterByIndex(&ctx, index, &MACFilter);

    pEntry->InstanceNumber = MACFilter.InstanceNumber;
    pEntry->Enable = MACFilter.Enable;

    _ansc_strncpy(pEntry->Alias, MACFilter.Alias, sizeof(pEntry->Alias));
    _ansc_strncpy(pEntry->Hostname, MACFilter.Hostname, sizeof(pEntry->Hostname));
    _ansc_strncpy(pEntry->MACAddress, MACFilter.MACAddress, sizeof(pEntry->MACAddress));
    _ansc_strncpy(pEntry->Description, MACFilter.Description, sizeof(pEntry->Description));

    Utopia_Free(&ctx, 0);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlFW_MACFilter_SetValues(ULONG index, ULONG ins, const char *alias)
{
    int rc = -1;
    UtopiaContext ctx;

    if (index >= g_NrFwMACFilter || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_SetMACFilterInsAndAliasByIndex(&ctx, index, ins, alias);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
        return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlFW_MACFilter_AddEntry(COSA_DML_FW_MACFILTER *pEntry)
{
    int rc = -1;
    UtopiaContext ctx;
    fwmacfilter_t MACFilter;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    MACFilter.InstanceNumber = pEntry->InstanceNumber;
    MACFilter.Enable = pEntry->Enable;

    _ansc_strncpy(MACFilter.Alias, pEntry->Alias, sizeof(MACFilter.Alias));
    _ansc_strncpy(MACFilter.Hostname, pEntry->Hostname, sizeof(MACFilter.Hostname));
    _ansc_strncpy(MACFilter.MACAddress, pEntry->MACAddress, sizeof(MACFilter.MACAddress));
    _ansc_strncpy(MACFilter.Description, pEntry->Description, sizeof(MACFilter.Description));

    rc = Utopia_AddMACFilter(&ctx, &MACFilter);

    Utopia_GetNumberOfMACFilter(&ctx, &g_NrFwMACFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }

}

ANSC_STATUS
CosaDmlFW_MACFilter_DelEntry(ULONG ins)
{
    int rc = -1;
    UtopiaContext ctx;
    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_DelMACFilter(&ctx, ins);
    Utopia_GetNumberOfMACFilter(&ctx, &g_NrFwMACFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }
}

ANSC_STATUS
CosaDmlFW_MACFilter_GetConf(ULONG ins, COSA_DML_FW_MACFILTER *pEntry)
{
    int index;

    if ((index = FW_MACFilter_InsGetIndex(ins)) == -1)
        return ANSC_STATUS_FAILURE;

    return CosaDmlFW_MACFilter_GetEntryByIndex(index, pEntry);
}

ANSC_STATUS
CosaDmlFW_MACFilter_SetConf(ULONG ins, COSA_DML_FW_MACFILTER *pEntry)
{
    int index;
    UtopiaContext ctx;
    fwmacfilter_t MACFilter;
    int rc = -1;

    if ((index = FW_MACFilter_InsGetIndex(ins)) == -1 || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    index = FW_MACFilter_InsGetIndex(ins);

    MACFilter.InstanceNumber = pEntry->InstanceNumber;
    MACFilter.Enable = pEntry->Enable;

    _ansc_strncpy(MACFilter.Alias, pEntry->Alias, sizeof(MACFilter.Alias));
    _ansc_strncpy(MACFilter.Hostname, pEntry->Hostname, sizeof(MACFilter.Hostname));
    _ansc_strncpy(MACFilter.MACAddress, pEntry->MACAddress, sizeof(MACFilter.MACAddress));
    _ansc_strncpy(MACFilter.Description, pEntry->Description, sizeof(MACFilter.Description));

    rc = Utopia_SetMACFilterByIndex(&ctx, index, &MACFilter);

    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");

        return ANSC_STATUS_SUCCESS;
    }
}

ANSC_STATUS CosaDmlFW_MACDayOfWeek_GetBlockTimeBitMaskType(ULONG *pulBlockTimeBitMaskType) {

    char buf[16];
    syscfg_get(NULL, "mac_dayofweek_block_time_bitmask_type", buf, sizeof(buf));
    *pulBlockTimeBitMaskType = atoi(buf);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlFW_MACDayOfWeek_SetBlockTimeBitMaskType(ULONG blockTimeBitMaskType) {

    char buf[16];

    sprintf(buf, "%lu", blockTimeBitMaskType);

    if(syscfg_set_commit(NULL, "mac_dayofweek_block_time_bitmask_type", buf) != 0) {
        return ANSC_STATUS_FAILURE;
    }
    return ANSC_STATUS_SUCCESS;
}

static int g_NrFwMacDayOfWeek = 0;

ULONG
CosaDmlFW_MacDayOfWeek_GetNumberOfEntries(void)
{
    UtopiaContext ctx;

    if(!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfMacDayOfWeek(&ctx, &g_NrFwMacDayOfWeek);
    Utopia_Free(&ctx, 0);

    return g_NrFwMacDayOfWeek;
}

ANSC_STATUS
CosaDmlFW_MacDayOfWeek_GetEntryByIndex(ULONG index, COSA_DML_FW_MAC_DAYOFWEEK *pEntry)
{
    UtopiaContext ctx;
    fwmacdayofweek_t MacDayOfWeek;

    if (index >= g_NrFwMacDayOfWeek || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetMacDayOfWeekByIndex(&ctx, index, &MacDayOfWeek);

    pEntry->InstanceNumber = MacDayOfWeek.InstanceNumber;
    _ansc_strncpy(pEntry->MacDayOfWeek_BlockTimeBitMask, MacDayOfWeek.MacDayOfWeek_BlockTimeBitMask, sizeof(pEntry->MacDayOfWeek_BlockTimeBitMask)-1);
    _ansc_strncpy(pEntry->Alias, MacDayOfWeek.Alias, sizeof(pEntry->Alias)-1);

    Utopia_Free(&ctx, 0);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlFW_MacDayOfWeek_SetValues(ULONG index, ULONG ins, const char *alias, char* bitmask)
{
    int rc = -1;
    UtopiaContext ctx;

    if (index >= g_NrFwMacDayOfWeek || !Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_SetMacDayOfWeekInsAndAliasByIndex(&ctx, index, ins, alias, bitmask);
    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
        return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlFW_MacDayOfWeek_GetConf(ULONG ins, COSA_DML_FW_MAC_DAYOFWEEK *pEntry)
{
    return CosaDmlFW_MacDayOfWeek_GetEntryByIndex(ins, pEntry);
}

ANSC_STATUS
CosaDmlFW_MacDayOfWeek_SetConf(ULONG ins, COSA_DML_FW_MAC_DAYOFWEEK *pEntry)
{
    UtopiaContext ctx;
    fwmacdayofweek_t MacDayOfWeek;
    int rc = -1;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    MacDayOfWeek.InstanceNumber = pEntry->InstanceNumber;
    _ansc_strncpy(MacDayOfWeek.MacDayOfWeek_BlockTimeBitMask, pEntry->MacDayOfWeek_BlockTimeBitMask, sizeof(MacDayOfWeek.MacDayOfWeek_BlockTimeBitMask)-1);
    _ansc_strncpy(MacDayOfWeek.Alias, pEntry->Alias, sizeof(MacDayOfWeek.Alias)-1);

    rc = Utopia_SetMacDayOfWeekByIndex(&ctx, ins, &MacDayOfWeek);
    Utopia_Free(&ctx, !rc);

    if (rc != 0)
        return ANSC_STATUS_FAILURE;
    else
    {
        commonSyseventSet("pp_flush", "1");
        commonSyseventSet("firewall-restart", "");
        return ANSC_STATUS_SUCCESS;
    }
}
