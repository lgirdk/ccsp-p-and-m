/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2015 RDK Management
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

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]
 
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
 
       http://www.apache.org/licenses/LICENSE-2.0
 
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

/**************************************************************************

    module: cosa_common_util.c

        Common utilities

    -------------------------------------------------------------------

    description:

        This file implements common back-end apis for the COSA Data Model Library
    -------------------------------------------------------------------

    author:

        Chen Changchun <chchangc@cisco.com>

    -------------------------------------------------------------------

    revision:

        11/01/2013    initial revision.

**************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <utapi.h>
#include "cosa_common_util.h"
#include "cosa_apis_util.h"
#if defined (WIFI_MANAGE_SUPPORTED)
#include "ccsp_message_bus.h"
#endif /*WIFI_MANAGE_SUPPORTED*/
#include "ccsp_base_api.h"
#include "ccsp_psm_helper.h"
#include "cosa_rbus_handler_apis.h"
#include "safec_lib_common.h"
#include "dslh_definitions_database.h"

#ifdef _HUB4_PRODUCT_REQ_

#include "cosa_lanmanagement_apis.h"

#define PSM_LANMANAGEMENTENTRY_LAN_ULA_ENABLE  "dmsb.lanmanagemententry.lanulaenable"
#define PSM_LANMANAGEMENTENTRY_LAN_IPV6_ENABLE "dmsb.lanmanagemententry.lanipv6enable"
#define PSM_LANMANAGEMENTENTRY_LAN_ULA  "dmsb.lanmanagemententry.lanula"
#define PSM_LANMANAGEMENTENTRY_LAN_ULA_PREFIX  "dmsb.lanmanagemententry.lanulaprefix"
extern void* g_pDslhDmlAgent;


void ValidUlaHandleEventAsync(void);
#endif

extern ANSC_HANDLE bus_handle;
#if defined (WIFI_MANAGE_SUPPORTED)
extern  char   g_Subsystem[32];

int psmGet(char *pParamName, char *pParamValue, size_t len)
{
    char *pVal = NULL;

    if ((pParamValue == NULL) || (len == 0))
        return -1;

    *pParamValue = 0;

    if (pParamName == NULL)
        return -1;

    if ((PSM_Get_Record_Value2(bus_handle, g_Subsystem, pParamName, NULL, &pVal) == CCSP_SUCCESS) && (NULL != pVal))
    {
        snprintf(pParamValue, len, "%s", pVal);
        ((CCSP_MESSAGE_BUS_INFO *)bus_handle)->freefunc(pVal);
        return 0;
    }

    return -1;
}

int psmSet(char *pParamName, char *pParamValue)
{
    if ((NULL == pParamValue) || (NULL == pParamName))
    {
        CcspTraceError(("%s:%d, NULL parameter Passed\n",__FUNCTION__,__LINE__));
        return -1;
    }
    if (PSM_Set_Record_Value2(bus_handle, g_Subsystem,(char *)pParamName, ccsp_string, (char *)pParamValue) != CCSP_SUCCESS){
        CcspTraceError(("%s:%d, PSM set is unsuccessful \n",__FUNCTION__,__LINE__));
        return -1;
    }
    CcspTraceInfo(("%s:%d,PSM Set for %s with value %s is successful \n", __FUNCTION__,__LINE__,pParamName, pParamValue));
    return 0;
}
#endif /*WIFI_MANAGE_SUPPORTED*/
/**
 * common callback function dispatcher for event
 */


EVT_DISPTER_CB_LIST g_CallbackFuncList = NULL;

/*Initialize the event dispatcher callback list */
EVT_DISPTER_CB_LIST
EvtDispterInitCallbackList
    (
        EVT_DISPTER_CB_LIST *pCbList
    )
{
    EVT_DISPTER_CB_LIST       listHead = NULL;

    listHead = (EVT_DISPTER_CB_LIST)malloc(sizeof(EVT_DISPTER_CB_LIST_ENTRY));
    if(NULL == listHead)
        return NULL;

    listHead->EventCbList = NULL;
    listHead->Next = NULL;

    *pCbList = (EVT_DISPTER_CB_LIST)listHead;

    return *pCbList;
}

/*get the callback function list based on the specified event name*/
static EVT_DISPTER_EVENT_CB_LIST 
EvtDispterFindListByEventName
    (
        EVT_DISPTER_CB_LIST     cbList,
        char *                  eventName
    )
{
    PEVT_DISPTER_CB_LIST_ENTRY      pCbListEntry = NULL;
    PEVT_DISPTER_CB_LIST_ENTRY      tmpListEntry = NULL;
    PEVT_DISPTER_METHOD_NODE        pMethodNode = NULL;
    
    pCbListEntry = (PEVT_DISPTER_CB_LIST_ENTRY)cbList->Next;
    tmpListEntry = (PEVT_DISPTER_CB_LIST_ENTRY)cbList;
    /*find the event list by name ,if not exist, create it*/
    while((NULL != pCbListEntry))
    {
        if(0 == strcmp(pCbListEntry->EventName, eventName)) {
            return pCbListEntry->EventCbList;
        }
        else {
            tmpListEntry = pCbListEntry;
            pCbListEntry = pCbListEntry->Next;
        }
    }

    /*create new callback list entry by the new event name*/
    pCbListEntry = (PEVT_DISPTER_CB_LIST_ENTRY)malloc(sizeof(EVT_DISPTER_CB_LIST_ENTRY));
    if(NULL == pCbListEntry)
        return NULL;
    
    pCbListEntry->Next = NULL;
    strncpy(pCbListEntry->EventName, eventName, sizeof(pCbListEntry->EventName)-1);
    
    /*Create event callback list for this new event*/
    pMethodNode = (PEVT_DISPTER_METHOD_NODE)malloc(sizeof(EVT_DISPTER_METHOD_NODE));
    if(NULL != pMethodNode) {
        pMethodNode->CallbackFunc = NULL;
        pMethodNode->Next = NULL;
    }
    pCbListEntry->EventCbList = (EVT_DISPTER_EVENT_CB_LIST)pMethodNode;

    tmpListEntry->Next = pCbListEntry;

    return pCbListEntry->EventCbList;
}

/*register the new callback function to the cbList*/
ANSC_STATUS
EvtDispterRgstCbFuncToList
    (
        EVT_DISPTER_CB_LIST     cbList,
        char *                  eventName,
        CALLBACK_FUNC_NAME      func,
        void *                  arg 
    )
{
    ANSC_STATUS                 returnStatus = ANSC_STATUS_SUCCESS;
    PEVT_DISPTER_CALLBACK_FUNC  pCbFunc = NULL;
    PEVT_DISPTER_METHOD_NODE    pNewMethodNode = NULL;
    EVT_DISPTER_EVENT_CB_LIST   eventCbList = NULL;
    PEVT_DISPTER_METHOD_NODE    tmpNode = NULL;

    if ( (NULL == eventName) || (NULL == func))
    {
        CcspTraceWarning(("event/function name should not be null!\n"));
        return ANSC_STATUS_FAILURE;
    }

    pCbFunc = (PEVT_DISPTER_CALLBACK_FUNC)malloc(sizeof(EVT_DISPTER_CALLBACK_FUNC));
    if(NULL != pCbFunc) {
        pCbFunc->MethodName = NULL; /*currently, methodname not used*/
        pCbFunc->Func = func;
        pCbFunc->Arg = arg;
    }
    else /*RDKB-6747, CID-32958, return if malloc failed */
    {
        returnStatus = ANSC_STATUS_FAILURE;
        return returnStatus;
    }

    /*create the new callback function node*/
    pNewMethodNode = (PEVT_DISPTER_METHOD_NODE)malloc(sizeof(EVT_DISPTER_METHOD_NODE));
    if(NULL == pNewMethodNode) {
        free(pCbFunc);  /*RDKB-6747, CID-32958, free unused resources */
        returnStatus = ANSC_STATUS_FAILURE;
        return returnStatus;
    }
    pNewMethodNode->CallbackFunc = pCbFunc;
    pNewMethodNode->Next = NULL;

    /*insert the node into the right event list*/
    eventCbList = EvtDispterFindListByEventName(cbList, eventName);
    tmpNode = eventCbList;
    while((NULL != tmpNode) && (NULL != tmpNode->Next))
    {
        tmpNode = tmpNode->Next;
    }

    tmpNode->Next = pNewMethodNode;

    return returnStatus;
}

/*call the callback functin step by step from cbList*/
ANSC_STATUS
EvtDispterCallFuncFromList
    (
        EVT_DISPTER_CB_LIST     cbList,
        char *                  eventName           
    )
{
    ANSC_STATUS                 returnStatus = ANSC_STATUS_SUCCESS;
    EVT_DISPTER_EVENT_CB_LIST   eventCbList = NULL;
    PEVT_DISPTER_METHOD_NODE    pMethodNode = NULL;
    PEVT_DISPTER_CALLBACK_FUNC  pCallbackFunc = NULL;
    int                         err = -1; 

    if(NULL == eventName)
        return returnStatus;

    eventCbList = EvtDispterFindListByEventName(cbList, eventName);
    
    /*call the callback function step by step in the eventCbList*/
    pMethodNode = (PEVT_DISPTER_METHOD_NODE)eventCbList->Next;

    while(NULL != pMethodNode)
    {
        pCallbackFunc = pMethodNode->CallbackFunc;
        pMethodNode = pMethodNode->Next;
        
        if((NULL == pCallbackFunc) || (NULL == pCallbackFunc->Func))
            continue;

        err = pCallbackFunc->Func(pCallbackFunc->Arg);
        if(0 != err) {
            returnStatus = ANSC_STATUS_FAILURE;
        }
    }

    return returnStatus;
}

/*clear the callback list*/
VOID
EvtDispterClearCallbackList
    (
        EVT_DISPTER_CB_LIST          cbList
    )
{
    PEVT_DISPTER_CB_LIST_ENTRY  pCbListEntry = NULL;
    EVT_DISPTER_EVENT_CB_LIST   eventCbList = NULL;
    PEVT_DISPTER_METHOD_NODE    pMethodNode = NULL;


    pCbListEntry = (PEVT_DISPTER_CB_LIST_ENTRY)cbList->Next;

    while(NULL != pCbListEntry)
    {
        eventCbList = pCbListEntry->EventCbList;
        pMethodNode = eventCbList->Next;
        
        while(NULL != pMethodNode)
        {
            eventCbList->Next = pMethodNode->Next;
            /* free current method node */
            free(pMethodNode->CallbackFunc);
            free(pMethodNode);
            
            pMethodNode = eventCbList->Next;
        }
        free(eventCbList);
        cbList->Next = pCbListEntry->Next;
        free(pCbListEntry);
        pCbListEntry = cbList->Next;
    }

}

/*Destroy the callback list after clear*/
VOID
EvtDispterDestroyCallbackList
    (
        EVT_DISPTER_CB_LIST * pCbList
    )
{
    EvtDispterClearCallbackList(*pCbList);
    free(*pCbList);
    *pCbList = NULL;
}


/********************************************************************************

    Description:
    
          This function register the callback functions for event
        
    Arguments:

          eventName: the event name, should not be null

               func: the callback function which you want register for the event

                arg: the argument of the callback function
    
    Return: ANSC_STATUS (ANSC_STATUS_SUCCESS or ANSC_STATUS_FAILURE)
        

**********************************************************************************/
ANSC_STATUS
EvtDispterRgstCallbackForEvent
    (
        char *              eventName,
        CALLBACK_FUNC_NAME  func,
        void *              arg 
    )
{
    if (NULL ==  g_CallbackFuncList)
        EvtDispterInitCallbackList(&g_CallbackFuncList);

    return EvtDispterRgstCbFuncToList(g_CallbackFuncList, eventName, func, arg);
}

/********************************************************************************

    Description:
    
          This function call the callback functions step by step when the
          specified event trigger
        
    Arguments:

          eventName: the event name, should not be null


    Return: ANSC_STATUS (ANSC_STATUS_SUCCESS or ANSC_STATUS_FAILURE)
        

**********************************************************************************/
ANSC_STATUS
EvtDispterCallFuncByEvent
    (
        char *          eventName           
    )
{
    return EvtDispterCallFuncFromList(g_CallbackFuncList, eventName);
}

/************************************************common event handler********************************************************/

static int se_fd = 0; 
static token_t token;

static async_id_t async_id[6];

static short server_port;
static char  server_ip[19];
#ifdef _HUB4_PRODUCT_REQ_
enum {EVENT_ERROR=-1, EVENT_OK, EVENT_TIMEOUT, EVENT_HANDLE_EXIT, EVENT_LAN_STARTED=0x10, EVENT_LAN_STOPPED, 
        EVENT_WAN_STARTED=0x20, EVENT_WAN_STOPPED,EVENT_WAN_IPV4_RECD=0x30, EVENT_WAN_IPV6_RECD, EVENT_VALID_ULA_ADDRESS=0x40, EVENT_DIBBLER_SERVER_RESTART=0x50};
#else
enum {EVENT_ERROR=-1, EVENT_OK, EVENT_TIMEOUT, EVENT_HANDLE_EXIT, EVENT_LAN_STARTED=0x10, EVENT_LAN_STOPPED,
        EVENT_WAN_STARTED=0x20, EVENT_WAN_STOPPED,EVENT_WAN_IPV4_RECD=0x30, EVENT_WAN_IPV6_RECD};
#endif

#if defined (RBUS_WAN_IP)
void free_args_struct(arg_struct_t *param){

    if(param != NULL)
    {
        if(param->parameterName != NULL)
        {
            free(param->parameterName);
            param->parameterName = NULL;
        }
        if(param->newValue != NULL)
        {
            free(param->newValue);
            param->newValue = NULL;
        }
        if(param->oldValue != NULL)
        {
            free(param->oldValue);
            param->oldValue = NULL;
        }
        free(param);
        param = NULL;
    }

}

void*
Set_Notifi_ParamName(void *args)
{
    errno_t rc = -1;
    char  str1[512] = {0};
    FILE *file;
    char *path = "/tmp/webpanotifyready";
    bool WebpaNotifyReady= false;
    char compo[256] = "eRT.com.cisco.spvtg.ccsp.webpaagent";
    char bus[256] = "/com/cisco/spvtg/ccsp/webpaagent";
    parameterValStruct_t value[1];

    char* faultParam = NULL;
    int ret = 0;

    pthread_detach(pthread_self());    

    arg_struct_t *arguments = (arg_struct_t*)args;
    if(arguments == NULL){
        CcspTraceError(("%s: arguments struct is NULL", __FUNCTION__)); 
        return NULL;
    }
    
    rc = sprintf_s(str1,sizeof(str1),"%s,%u,%s,%s,%d",arguments->parameterName, arguments->writeID, (arguments->newValue)!=NULL ? (strlen(arguments->newValue)>0 ? arguments->newValue : "NULL") : "NULL", (arguments->oldValue)!=NULL ? (strlen(arguments->oldValue)>0 ? arguments->oldValue : "NULL") : "NULL", arguments->type);
    CcspTraceInfo(("%s rc: %d, str1: %s\n", __FUNCTION__ , rc, str1));

    if(rc < EOK)
    {
        ERR_CHK(rc);
        CcspTraceError(("%s Failed to create WAN IP event data. Returning :%d\n", __FUNCTION__, __LINE__));
        goto EXIT;
    }

    value[0].parameterName = "Device.Webpa.X_RDKCENTRAL-COM_WebPA_Notification";
    value[0].parameterValue = str1;
    value[0].type = ccsp_string;
    
    //wait for 30s to update wan status and send notification
    sleep(30);
    CcspTraceInfo(("Sending WAN_IP notification after 30sec\n"));
    /* Check if file exists. Wait for max 2 mins for WebPA to create the file
     * and send the notification.
     */
    
    for(int count=0; count<24; count++){
        file = fopen(path, "rb");
        if(file == NULL)
        {
            if(count%4==0){
                CcspTraceWarning(("%s WebPA is not ready to receive notifications. Waiting for /tmp/webpanotifyready file: %d\n", __FUNCTION__, __LINE__));
            }
            sleep(5);
        }
        else{
            WebpaNotifyReady=true;
            fclose(file);
            break;
        }
    }
    if(!WebpaNotifyReady)
    {
        CcspTraceError(("/tmp/webpanotifyready file is not created even after 2 mins. WebPA is not ready to receive notifications. Exiting\n"));
        goto EXIT;
    }
    else{
        CcspTraceDebug(("%s /tmp/webpanotifyready file exists. WebPA is ready to receive notifications: %d\n", __FUNCTION__, __LINE__));
    }

    ret = CcspBaseIf_setParameterValues(  bus_handle,
                                            compo,
                                            bus,
                                            0,
                                            DSLH_MPA_ACCESS_CONTROL_PAM,
                                            value,
                                            1,
                                            TRUE,
                                            &faultParam );
    
    if (ret != CCSP_SUCCESS && faultParam)
    {   
        CcspTraceError(("%s CcspBaseIf_setParameterValues:Failed to SetValue for param '%s'\n", __FUNCTION__, faultParam));
        CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
        bus_info->freefunc(faultParam);
    }
    else
    {
        CcspTraceWarning(("%s: CcspBaseIf_setParameterValues: Param value set succesfully and sent to WebPA.\n", __FUNCTION__ ));
    }
  
    EXIT:
        free_args_struct(arguments);
        return NULL;
}

int Send_WebPANotification_WANIP(char* parameterName, char *ip_addrs, char *previous_ip){
    if((parameterName == NULL) || (ip_addrs == NULL) || (previous_ip == NULL))
    {
        CcspTraceError(("%s arguments are NULL\n", __FUNCTION__ ));
        return EVENT_ERROR;
    }

    pthread_t threadId;
    arg_struct_t *wanip_args = NULL;
    wanip_args = (arg_struct_t *)malloc(sizeof(arg_struct_t));

    bool IPv6Flag = false;
    if(strcmp(parameterName,PRIMARY_WAN_IPv6_ADDRESS)==0){
        IPv6Flag=true;
    }

    CcspTraceDebug(("%s thread started for %s.\n", __FUNCTION__, IPv6Flag?"IPv6":"IPv4" ));

    if(wanip_args != NULL)
    {
        CcspTraceDebug(("%s wanip_args for %s is valid: %d \n", __FUNCTION__, IPv6Flag?"IPv6":"IPv4", __LINE__)); 
        memset(wanip_args, 0, sizeof(arg_struct_t));
 
        wanip_args->parameterName = strdup(parameterName);
        wanip_args->writeID         = 256;
        wanip_args->newValue = strdup(ip_addrs);
        wanip_args->oldValue = strdup(previous_ip);
        wanip_args->type            = ccsp_string;
        CcspTraceDebug(("%s pthread_create with arguments: %s,%u,%s,%s,%d, LINE: %d\n", __FUNCTION__, wanip_args->parameterName, wanip_args->writeID, wanip_args->newValue, wanip_args->oldValue, wanip_args->type,__LINE__)); 

        if (pthread_create(&threadId, NULL, Set_Notifi_ParamName, (void *) wanip_args) != 0) 
        {
            CcspTraceError(("%s: Error creating thread Set_Notifi_ParamName for %s\n", __FUNCTION__, IPv6Flag?"IPv6":"IPv4"));
            free_args_struct(wanip_args);
            return EVENT_ERROR;
        }
        else{
            CcspTraceInfo(("%s: Created thread Set_Notifi_ParamName for %s\n", __FUNCTION__, IPv6Flag?"IPv6":"IPv4"));
        }
    }
    else
    {
        CcspTraceError(("%s wanip_args NULL.\n", __FUNCTION__ ));
        return EVENT_ERROR;
    }
    return(EVENT_OK);
}
#endif /*RBUS_WAN_IP*/
static void
EvtDispterWanIpAddrsCallback(char *ip_addrs)
{
    static char previous_ip[16] = "0.0.0.0";

#if !defined (RBUS_WAN_IP)
    UNREFERENCED_PARAMETER(previous_ip);
#endif

#ifdef DUAL_CORE_XB3
    CcspTraceInfo(("%s vsystem %d \n", __FUNCTION__,__LINE__)); 
    CcspTraceInfo(("EvtDispterWanIpAddrsCallback - erouter0 IP = %s\n",ip_addrs));
    vsystem("/usr/ccsp/pam/erouter0_ip_sync.sh %s &",ip_addrs);
#endif
// Restricting file access sec_pushown.sh to XB6 and CMXB7
#if defined(_XB6_PRODUCT_REQ_) && !(defined(_XB7_PRODUCT_REQ_) && defined(_COSA_BCM_ARM_)) 
    CcspTraceInfo(("%s vsystem %d \n", __FUNCTION__,__LINE__));
    CcspTraceInfo(("EvtDispterWanIpAddrsCallback - erouter0 IP = %s\n",ip_addrs));
    vsystem("/usr/sbin/sec_pushown.sh --ip4 \"%s\"", ip_addrs);
#endif

     if (strcmp(ip_addrs,"0.0.0.0") != 0 ) {

        CcspTraceInfo(("%s Setting current_wan_ipaddr and restarting firewall %d \n", __FUNCTION__,__LINE__)); 
	    sysevent_set(se_fd, token, "current_wan_ipaddr", ip_addrs, 0);
	    sysevent_set(se_fd, token, "firewall-restart", NULL, 0);
    }
#if defined (RBUS_WAN_IP)
    if (strcmp(previous_ip, ip_addrs) != 0) {
        CcspTraceInfo(("%s New IPv4 address detected: %s, Previous IPv4 address: %s\n", __FUNCTION__, ip_addrs, previous_ip));

        if (publishWanIpAddr(PRIMARY_WAN_IP_ADDRESS, ip_addrs, previous_ip)== RBUS_ERROR_SUCCESS){
            CcspTraceInfo(("%s publishWanIpAddr success for IPv4 : %d \n", __FUNCTION__,__LINE__)); 
        }
        else{
            CcspTraceError(("%s publishWanIpAddr failed for IPv4 : %d \n", __FUNCTION__,__LINE__)); 
        }
  
        int ret = Send_WebPANotification_WANIP(PRIMARY_WAN_IP_ADDRESS, ip_addrs, previous_ip);
        if(ret == EVENT_OK){
            CcspTraceInfo(("%s: Send_WebPANotification_WANIP completed for IPv4 and Set_Notifi_ParamName thread created. %d, ret: %d \n", __FUNCTION__,__LINE__, ret)); 
        }
        else{
            CcspTraceError(("%s: Send_WebPANotification_WANIP failed for IPv4 %d, ret: %d \n", __FUNCTION__,__LINE__, ret)); 
        }
        // Storing new IPv4 address
        strncpy(previous_ip, ip_addrs, sizeof(previous_ip) - 1);
     }
     else
     {
        CcspTraceInfo(("%s IPv4 address remains the same: %s\n", __FUNCTION__, previous_ip));
     }
#endif /*RBUS_WAN_IP*/
}

#if defined (RBUS_WAN_IP)
static void
EvtDispterWanIpv6AddrsCallback(char *ip_addrs)
{
    static char previous_ipv6[40] = "::";

    if (strcmp(previous_ipv6, ip_addrs) != 0) 
    {
        CcspTraceInfo(("%s New IPv6 address detected: %s, Previous IPv6 address: %s\n", __FUNCTION__, ip_addrs, previous_ipv6));
      
        if(publishWanIpAddr(PRIMARY_WAN_IPv6_ADDRESS, ip_addrs, previous_ipv6)== RBUS_ERROR_SUCCESS)
        {
            CcspTraceInfo(("%s publishWanIpAddr success for IPv6 : %d \n", __FUNCTION__,__LINE__)); 
        }
        else{
            CcspTraceError(("%s publishWanIpAddr failed for IPv6 : %d \n", __FUNCTION__,__LINE__)); 
        }

        int ret = Send_WebPANotification_WANIP(PRIMARY_WAN_IPv6_ADDRESS, ip_addrs, previous_ipv6);
        if(ret == EVENT_OK){
            CcspTraceInfo(("%s: Send_WebPANotification_WANIP completed for IPv6 and Set_Notifi_ParamName thread created. %d, ret: %d \n", __FUNCTION__,__LINE__, ret)); 
        }
        else{
            CcspTraceError(("%s:  Send_WebPANotification_WANIP failed for IPv6 %d, ret: %d \n", __FUNCTION__,__LINE__, ret)); 
        }
        //Storing new IPv6 address
        strncpy(previous_ipv6, ip_addrs, sizeof(previous_ipv6) - 1);
     }
     else
     {
            CcspTraceInfo(("%s IPv6 address remains the same: %s\n", __FUNCTION__, previous_ipv6));
     }
}
#endif /*RBUS_WAN_IP*/
/*
 * Initialize sysevnt 
 *   return 0 if success and -1 if failture.
 */
int 
EvtDispterEventInits(void)
{
    int rc;

    snprintf(server_ip, sizeof(server_ip), "127.0.0.1");
    server_port = SE_SERVER_WELL_KNOWN_PORT;

    se_fd = sysevent_open(server_ip, server_port, SE_VERSION, "common_master", &token);
    if (!se_fd) {
        return(EVENT_ERROR);
    }

    /*you can register the event as you want*/

	//register lan-status event
    sysevent_set_options(se_fd, token, "lan-status", TUPLE_FLAG_EVENT);
    rc = sysevent_setnotification(se_fd, token, "lan-status", &async_id[0]);
    if (rc) {
       return(EVENT_ERROR);
    } 

	//register wan-status event
    sysevent_set_options(se_fd, token, "wan-status", TUPLE_FLAG_EVENT);
    rc = sysevent_setnotification(se_fd, token, "wan-status", &async_id[1]);
    if (rc) {
       return(EVENT_ERROR);
    } 

	//register ipv4_wan_ipaddr event
    //sysevent_set_options(se_fd, token, "ipv4_wan_ipaddr", TUPLE_FLAG_EVENT);
    rc = sysevent_setnotification(se_fd, token, "ipv4_wan_ipaddr", &async_id[2]);
    if (rc) {
       return(EVENT_ERROR);
    }
#if defined (RBUS_WAN_IP)
#if defined (_HUB4_PRODUCT_REQ_) || defined (_SR213_PRODUCT_REQ_)
    //register lan_ipaddr_v6 event
    rc = sysevent_setnotification(se_fd, token, "lan_ipaddr_v6", &async_id[3]);
    if (rc) {
       return(EVENT_ERROR);
    }
#else
    //register tr_erouter0_dhcpv6_client_v6addr event
    rc = sysevent_setnotification(se_fd, token, "tr_erouter0_dhcpv6_client_v6addr", &async_id[3]);
    if (rc) {
       return(EVENT_ERROR);
    }
#endif
#endif /*RBUS_WAN_IP*/

#ifdef _HUB4_PRODUCT_REQ_
	//register valid_ula_address event
    sysevent_set_options(se_fd, token, "valid_ula_address", TUPLE_FLAG_EVENT);
    rc = sysevent_setnotification(se_fd, token, "valid_ula_address", &async_id[4]);
    if (rc) {
       return(EVENT_ERROR);
    }
	//register dibblerServer-restart event
    sysevent_set_options(se_fd, token, "dibblerServer-restart", TUPLE_FLAG_EVENT);
    rc = sysevent_setnotification(se_fd, token, "dibblerServer-restart", &async_id[5]);
    if (rc) {
       return(EVENT_ERROR);
    }

#endif
   
    return(EVENT_OK);
}

/*
 * Listen sysevent notification message
 */
int 
EvtDispterEventListen(void)
{
    int     ret = EVENT_TIMEOUT;
    fd_set  rfds;
    int     retval;
    
    FD_ZERO(&rfds);
    FD_SET(se_fd, &rfds);

    retval=select(se_fd+1, &rfds, NULL, NULL, NULL);

    if(retval) {
        se_buffer            msg_buffer;
        se_notification_msg *msg_body = (se_notification_msg *)msg_buffer;
        unsigned int         msg_size;
        token_t              from;
        int                  msg_type;

        msg_size  = sizeof(msg_buffer);
        msg_type = SE_msg_receive(se_fd, msg_buffer, &msg_size, &from);
        // if not a notification message then ignore it
        if (SE_MSG_NOTIFICATION == msg_type) {
            // extract the name and value from the return message data
            int   name_bytes;
            int   value_bytes;
            char *name_str;
            char *value_str;
            char *data_ptr;

            data_ptr   = (char *)&(msg_body->data);
            name_str   = (char *)SE_msg_get_string(data_ptr, &name_bytes);
            data_ptr  += name_bytes;
            value_str =  (char *)SE_msg_get_string(data_ptr, &value_bytes);

            if(!strcmp(name_str, "lan-status"))
            {
                if (!strncmp(value_str, "started", 7))
                {
                    #if defined (_XB7_PRODUCT_REQ_) && !defined (_COSA_BCM_ARM_)
                        vsystem("/usr/sbin/brlan0_uuid.sh");
                    #endif
                    ret = EVENT_LAN_STARTED;
                }
                else if (!strncmp(value_str, "stopped", 7))
                {
                    ret = EVENT_LAN_STOPPED;
                }
            }
            else if(!strcmp(name_str, "wan-status"))
            {
                if (!strncmp(value_str, "started", 7))
                {
                    ret = EVENT_WAN_STARTED;
                }
                else if (!strncmp(value_str, "stopped", 7)) 
                {
                    ret = EVENT_WAN_STOPPED;
                }
            }
            else if(!strcmp(name_str, "ipv4_wan_ipaddr"))
            {
                EvtDispterWanIpAddrsCallback(value_str);
                ret = EVENT_WAN_IPV4_RECD;
            }
#if defined (RBUS_WAN_IP)
#if defined (_HUB4_PRODUCT_REQ_) || defined (_SR213_PRODUCT_REQ_)
            else if(!strcmp(name_str, "lan_ipaddr_v6"))
            {
                EvtDispterWanIpv6AddrsCallback(value_str);
                ret = EVENT_WAN_IPV6_RECD;
            }
#else
            else if(!strcmp(name_str, "tr_erouter0_dhcpv6_client_v6addr"))
            {
                EvtDispterWanIpv6AddrsCallback(value_str);
                ret = EVENT_WAN_IPV6_RECD;
            }
#endif
#endif /*RBUS_WAN_IP*/
#ifdef _HUB4_PRODUCT_REQ_
            else if(!strcmp(name_str, "valid_ula_address"))
            {
                if (!strncmp(value_str, "false", 5))
                {
                    ret = EVENT_VALID_ULA_ADDRESS;
                }
            }
            else if(!strcmp(name_str, "dibblerServer-restart"))
            {
                if (!strncmp(value_str, "restart", 7))
                {
                    ret = EVENT_DIBBLER_SERVER_RESTART;
                }
            }

#endif
        } else {
            CcspTraceWarning(("Received msg that is not a SE_MSG_NOTIFICATION (%d)\n", msg_type));
	    if (  0 != system("pidof syseventd")) {

           	CcspTraceWarning(("%s syseventd not running ,breaking the receive notification loop \n",__FUNCTION__));
		ret = EVENT_HANDLE_EXIT;
	    }
        }
    } else {
        CcspTraceError(("%s: Received no event retval=%d\n", __FUNCTION__, retval));
    }
    return ret;
}

/*
 * Close sysevent
 */
int 
EvtDispterEventClose(void)
{
    /* we are done with this notification, so unregister it using async_id provided earlier */
    sysevent_rmnotification(se_fd, token, async_id[0]);
    sysevent_rmnotification(se_fd, token, async_id[1]);
    sysevent_rmnotification(se_fd, token, async_id[2]);
#if defined (RBUS_WAN_IP)
    sysevent_rmnotification(se_fd, token, async_id[3]);
#endif /*RBUS_WAN_IP*/
#ifdef _HUB4_PRODUCT_REQ_
    sysevent_rmnotification(se_fd, token, async_id[4]);
    sysevent_rmnotification(se_fd, token, async_id[5]);
#endif
    /* close this session with syseventd */
    sysevent_close(se_fd, token);

    return (EVENT_OK);
}

/*
 * check the initialized sysevent status (happened or not happened),
 * if the event happened, call the functions registered for the events previously
 */
int 
EvtDispterCheckEvtStatus(int fd, token_t token)
{
    char evtValue[256] = {0};
    int  returnStatus = ANSC_STATUS_SUCCESS;

    /*lan-status event*/
    if( 0 == sysevent_get(fd, token, "lan-status", evtValue, sizeof(evtValue)) && '\0' != evtValue[0] )
    {
        if (0 == strncmp(evtValue, "started", strlen("started"))) {
            if (ANSC_STATUS_SUCCESS != EvtDispterCallFuncByEvent("lan-status"))
                    returnStatus = ANSC_STATUS_FAILURE;
        }
    }

    /*wan-status*/
    if ( 0 == sysevent_get(fd, token, "wan-status", evtValue, sizeof(evtValue)) && '\0' != evtValue[0])
    {
        if (0 == strncmp(evtValue, "started", strlen("started")))
            if (ANSC_STATUS_SUCCESS != EvtDispterCallFuncByEvent("wan-status"))
                returnStatus = ANSC_STATUS_FAILURE;
    }

    /*dibblerServer-restart*/
#ifdef _HUB4_PRODUCT_REQ_
    if ( 0 == sysevent_get(fd, token, "dibblerServer-restart", evtValue, sizeof(evtValue)) && '\0' != evtValue[0])
    {
        if (0 == strncmp(evtValue, "restart", strlen("restart")))
            if (ANSC_STATUS_SUCCESS != EvtDispterCallFuncByEvent("dibblerServer-restart"))
                returnStatus = ANSC_STATUS_FAILURE;
    }
#endif

    /*ipv4_wan_ipaddr*/
    if ( 0 == sysevent_get(fd, token, "ipv4_wan_ipaddr", evtValue, sizeof(evtValue)) && '\0' != evtValue[0])
    {
        EvtDispterWanIpAddrsCallback(evtValue);
    }
#if defined (RBUS_WAN_IP)
#if defined (_HUB4_PRODUCT_REQ_) || defined (_SR213_PRODUCT_REQ_)
    if ( 0 == sysevent_get(fd, token, "lan_ipaddr_v6", evtValue, sizeof(evtValue)) && '\0' != evtValue[0])
    {
        EvtDispterWanIpv6AddrsCallback(evtValue);
    }
#else
    if ( 0 == sysevent_get(fd, token, "tr_erouter0_dhcpv6_client_v6addr", evtValue, sizeof(evtValue)) && '\0' != evtValue[0])
    {
        EvtDispterWanIpv6AddrsCallback(evtValue);
    }
#endif
#endif /*RBUS_WAN_IP*/
    return returnStatus;
}

/*
 * the sysevent handler in event dispatcher
 */
void *
EvtDispterEventHandler(void *arg)
{
    int ret = EVENT_ERROR;
    UNREFERENCED_PARAMETER(arg);
    while(EVENT_ERROR == EvtDispterEventInits())
    {
        CcspTraceError(("%s: sysevent init failed!\n", __FUNCTION__));
        sleep(1);
    }

    /*first check the events status*/
    EvtDispterCheckEvtStatus(se_fd, token);

    while(1)
    {
        ret = EvtDispterEventListen();
        switch (ret)
        {
            case EVENT_LAN_STARTED:
                EvtDispterCallFuncByEvent("lan-status");
                break;
            case EVENT_LAN_STOPPED:
                break;
            case EVENT_WAN_STARTED:
                EvtDispterCallFuncByEvent("wan-status");
                break;
            case EVENT_WAN_STOPPED:
                break;
            case EVENT_WAN_IPV4_RECD:
                break;
#if defined (RBUS_WAN_IP)
            case EVENT_WAN_IPV6_RECD:
                break;
#endif /*RBUS_WAN_IP*/
#ifdef _HUB4_PRODUCT_REQ_
            case EVENT_VALID_ULA_ADDRESS:
                ValidUlaHandleEventAsync();
                break;
            case EVENT_DIBBLER_SERVER_RESTART:
                EvtDispterCallFuncByEvent("dibblerServer-restart");
                break;
#endif
            default :
                CcspTraceWarning(("The received event status is not expected!\n"));
                break;
        }

        if (EVENT_HANDLE_EXIT == ret) //end this event handling loop
            break;

        sleep(2);
    }

    EvtDispterEventClose();

    return NULL;
}

/*
 * Create a thread to handle the sysevent asynchronously
 */
void
EvtDispterHandleEventAsync(void)
{
    int err;
    pthread_t event_handle_thread;

    err = pthread_create(&event_handle_thread, NULL, EvtDispterEventHandler, NULL);
    if(0 != err)
    {
        CcspTraceError(("%s: create the event handle thread error!\n", __FUNCTION__));
    }
}

int executeCmd(char *cmd)
{
	int l_iSystem_Res;
	l_iSystem_Res = system(cmd);
    if (0 != l_iSystem_Res && ECHILD != errno)
    {
        CcspTraceError(("%s: %s command didnt execute successfully\n", __FUNCTION__,cmd));
        return l_iSystem_Res;
    }
    return 0;
}


#ifdef _HUB4_PRODUCT_REQ_
void* RegenerateUla(void *arg)
{
    char *pIpv6_enable = NULL;
    char *pUla_enable  = NULL;
    char *pUla_prefix  = NULL;
    char *pUla = NULL;

    UNREFERENCED_PARAMETER(arg);

    CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
    pthread_detach(pthread_self());

    if(CCSP_SUCCESS != PSM_Get_Record_Value2(bus_info, g_GetSubsystemPrefix(g_pDslhDmlAgent), PSM_LANMANAGEMENTENTRY_LAN_IPV6_ENABLE, NULL, &pIpv6_enable))
    {
    	if (pIpv6_enable != NULL) {
           bus_info->freefunc(pIpv6_enable);
    	}
        return NULL;
    }

    if(CCSP_SUCCESS != PSM_Get_Record_Value2(bus_info, g_GetSubsystemPrefix(g_pDslhDmlAgent), PSM_LANMANAGEMENTENTRY_LAN_ULA_ENABLE, NULL, &pUla_enable))
    {
    	if (pUla_enable != NULL) {
           bus_info->freefunc(pUla_enable);
    	}
        return NULL;
    }

    if(CCSP_SUCCESS != PSM_Get_Record_Value2(bus_info, g_GetSubsystemPrefix(g_pDslhDmlAgent), PSM_LANMANAGEMENTENTRY_LAN_ULA, NULL, &pUla))
    {
        if(pUla != NULL)
            bus_info->freefunc(pUla);
        return NULL;
    }

    if(CCSP_SUCCESS != PSM_Get_Record_Value2(bus_info, g_GetSubsystemPrefix(g_pDslhDmlAgent), PSM_LANMANAGEMENTENTRY_LAN_ULA_PREFIX, NULL, &pUla_prefix))
    {
        if(pUla_prefix != NULL)
            bus_info->freefunc(pUla_prefix);
        return NULL;
    }

    if ((strncmp(pIpv6_enable, "TRUE", 4 ) == 0) && (strncmp(pUla_enable, "TRUE", 4 ) == 0))
    {
        //Check whether NULL or not
        if( ( NULL != pUla_prefix ) && ( NULL != pUla ) )
        {
            char   acTmpULAPrefix[64] = { 0 },
                   acTmpULA[64]       = { 0 };

            //Copy PSM buffer to local buffer to avoid memory overwrite in calling function
            snprintf( acTmpULAPrefix, sizeof(acTmpULAPrefix), "%s", pUla_prefix );
            snprintf( acTmpULA, sizeof(acTmpULA), "%s", pUla );

            if( ANSC_STATUS_SUCCESS == CosaDmlLanMngm_SetLanIpv6Ula( acTmpULAPrefix, acTmpULA ) )
            {
               sysevent_set( se_fd, token, "valid_ula_address", "true", 0 );
            }
        }
    }
    else
    {
        system("killall zebra");
    }

    if(pIpv6_enable != NULL)
        bus_info->freefunc(pIpv6_enable);

    if(pUla_enable != NULL)
        bus_info->freefunc(pUla_enable);

    if(pUla_prefix != NULL)
        bus_info->freefunc(pUla_prefix);

    if(pUla != NULL)
        bus_info->freefunc(pUla);
    return NULL;
}

/*
 * Create a thread to handle the sysevent asynchronously
 */
void
ValidUlaHandleEventAsync(void)
{
    int err;
    pthread_t event_handle_thread;

    err = pthread_create(&event_handle_thread, NULL, RegenerateUla, NULL);
    if(0 != err)
    {
        CcspTraceError(("%s: create the event handle thread error!\n", __FUNCTION__));
    }
}
#endif
