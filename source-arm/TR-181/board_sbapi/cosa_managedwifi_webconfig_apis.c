#include "cosa_managedwifi_dml.h"
#include "cosa_managedwifi_webconfig_apis.h"
#include "cosa_dhcpv6_apis.h"
#include "secure_wrapper.h"
#include "sysevent/sysevent.h"
#include "cosa_common_util.h"
#include "cosa_drg_common.h"

#define RG_CM_COMMUNICATION_BEGIN "172.31"

extern rbusHandle_t handle;
extern char g_Subsystem[32];

static char *l2netBridgeEnable = "dmsb.l2net.%s.Enable";
static char *l2netBridgeName = "dmsb.l2net.%s.Name";
static char *l2netAlias = "dmsb.l2net.%s.Alias";
static char *l3netV4Addr = "dmsb.l3net.%s.V4Addr";
static char *l2netV4SubnetMask = "dmsb.l3net.%s.V4SubnetMask";
static char *l3netDhcpV4PoolSubnetMask =  "dmsb.dhcpv4.server.pool.%s.SubnetMask";
static char *l3netDhcpV4PoolEnable = "dmsb.dhcpv4.server.pool.%s.Enable";
static char *l3netDhcpV4PoolMinAddr = "dmsb.dhcpv4.server.pool.%s.MinAddress";
static char *l3netDhcpV4PoolMaxAddr = "dmsb.dhcpv4.server.pool.%s.MaxAddress";
static char *l3netDhcpV4PoolLeaseTime = "dmsb.dhcpv4.server.pool.%s.LeaseTime";
static char *l3netIPv6Enable = "dmsb.l3net.%s.IPv6Enable";
static char *l2netWiFiMembers = "dmsb.l2net.%s.Members.OneWiFi";

static ManageWiFiInfo_t sManageWiFiInfo = {{'\0'},{'\0'},{'\0'},{'\0'}, OTHER_UPDATE};
static backupLanconfig_t sBackupLanConfig = {false,{'\0'},false,{'\0'},{'\0'},{'\0'},{'\0'}, {'\0'},false, {'\0'}};
static backupLanconfig_t sCurrentLanConfig = {false,{'\0'},false,{'\0'},{'\0'},{'\0'},{'\0'}, {'\0'},false, {'\0'}};
char * pVapNames[BUFF_LEN_8];
/* static int iRestoreFlag = 0
 * 0th bit : return value 1 mean failure, 0 means success
 * 1st bit : Manage Wifi Enable
 * 2nd bit : Lan IP addr
 * 3rd bit : lan subnet mask
 * 4th bit : dhcp V4 pool subnet mask
 * 5th bit : DhcpV4Pool Enable
 * 6th bit : DhcpV4Pool Start
 * 7th bit : DhcpV4Pool End
 * 8th bit : DhcpV4Pool lease time
 * 9th bit : IPv6 Enable
 *10th bit : alias
*/
static int iRestoreFlag = 0;
pthread_mutex_t manageWifi_exec = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t manageWifi_exec_completed = PTHREAD_COND_INITIALIZER;
pthread_condattr_t manageWifi_attr;

int confirmManageWifiVap(void)
{
    rbusValue_t value;
    int ret = RBUS_ERROR_SUCCESS;
    char *pStrVal = NULL;
    char *saveptr = NULL;
    char aWiFiInterfaces[BUFF_LEN_64] = {'\0'};
    char aWiFiInterfacesPsmStr[BUFF_LEN_64] = {'\0'};
    char aBr106LnfInterfaces[BUFF_LEN_64] = {'\0'};
    char aParamVal[BUFF_LEN_64] = {'\0'};
    int len = 0, lenBr106Str = 0;

    CcspTraceInfo(("%s:%d, Entered \n",__FUNCTION__,__LINE__));
    if (NULL == handle)
    {
        CcspTraceError(("%s:%d, R-Bus handle is NULL\n",__FUNCTION__,__LINE__));
        return -1;
    }
    /* Init rbus variable */
    rbusValue_Init(&value);
    ret = rbus_get(handle,MANAGE_WIFI_INTERFACES, &value);

    if(ret != RBUS_ERROR_SUCCESS )
    {
        CcspTraceError(("%s:%d, Failed to do rbus_get\n",__FUNCTION__,__LINE__));
        return -1;
    }

    pStrVal = rbusValue_ToString(value, NULL, 0);
    CcspTraceInfo(("%s:%d, pStrVal:%s\n",__FUNCTION__,__LINE__,pStrVal));
    if (NULL != pStrVal)
    {
        snprintf (aWiFiInterfacesPsmStr, BUFF_LEN_64,l2netWiFiMembers, BR106_PSM_INDEX);
        CcspTraceInfo(("%s:%d, aWiFiInterfacesPsmStr:%s\n",__FUNCTION__,__LINE__,aWiFiInterfacesPsmStr));
        psmGet (aWiFiInterfacesPsmStr, aBr106LnfInterfaces, BUFF_LEN_64);
        CcspTraceInfo(("%s:%d, br106 Wifi interfaces:%s\n",__FUNCTION__,__LINE__,aBr106LnfInterfaces));
        CcspTraceInfo(("%s:%d, pStrVal:%s\n",__FUNCTION__,__LINE__,pStrVal));
        char * pKeyVal = strtok(pStrVal, ":");
        CcspTraceInfo(("%s:%d, pKeyVal:%s\n",__FUNCTION__,__LINE__,pKeyVal));
        pKeyVal = strtok(NULL, ":");
        CcspTraceInfo(("%s:%d, pKeyVal:%s\n",__FUNCTION__,__LINE__,pKeyVal));

        if (NULL != pKeyVal)
            pKeyVal = strtok_r(pKeyVal, ",", &saveptr);
        static int iCount = -1;
	int iSubStrLen = 0;
        while (pKeyVal != NULL)
        {
            CcspTraceInfo(("%s:%d, pKeyVal:%s\n",__FUNCTION__,__LINE__,pKeyVal));
            snprintf(aWiFiInterfaces + len, BUFF_LEN_64 -len," %s",pKeyVal);
            len = strlen(aWiFiInterfaces);
            if (true == sCurrentLanConfig.bMwEnable)
            {
                if(!removeSubstring(aBr106LnfInterfaces, pKeyVal, true))
                {
                    iSubStrLen = strlen(pKeyVal);
                    iCount++;
                    pVapNames[iCount] = (char*) malloc(iSubStrLen+1);
                    if(NULL != pVapNames[iCount])
                    {
                        strncpy(pVapNames[iCount], pKeyVal, iSubStrLen);
                    }
                    else
                        iCount--;
                }
            }
            else if (false == sCurrentLanConfig.bMwEnable)
            {
                lenBr106Str = strlen(aBr106LnfInterfaces);
		int iVar = 0;
		for (iVar = 0; iVar <= iCount; iVar++)
		{
		    if ((NULL != pVapNames[iVar]) && (!strcmp(pKeyVal,pVapNames[iVar])))
                    {
                        snprintf(aBr106LnfInterfaces + lenBr106Str, BUFF_LEN_64 - lenBr106Str, " %s", pKeyVal);
                        free(pVapNames[iVar]);
			pVapNames[iVar] = NULL;
			break;
                    }
                }
		if (iCount == iVar)
                    iCount = -1;
            }
            pKeyVal = strtok_r(NULL, ",", &saveptr);
        }

        if((true == sCurrentLanConfig.bMwEnable) && ('\0' != aWiFiInterfaces[0]))
        {
            snprintf(aWiFiInterfacesPsmStr, BUFF_LEN_64, l2netWiFiMembers, sManageWiFiInfo.aBridgeIndex);
            CcspTraceInfo(("%s:%d, aWiFiInterfacesPsmStr:%s\n",__FUNCTION__,__LINE__,aWiFiInterfacesPsmStr));
            CcspTraceInfo(("%s:%d, aWiFiInterfaces:%s\n",__FUNCTION__,__LINE__,aWiFiInterfaces));
            if (0 == psmSet(aWiFiInterfacesPsmStr,aWiFiInterfaces))
                strncpy(sBackupLanConfig.aWiFiInterfaces, aWiFiInterfaces, (sizeof(sBackupLanConfig.aWiFiInterfaces)-1));
            snprintf (aWiFiInterfacesPsmStr, BUFF_LEN_64,l2netWiFiMembers, BR106_PSM_INDEX);
            CcspTraceInfo(("%s:%d, aWiFiInterfacesPsmStr:%s\n",__FUNCTION__,__LINE__,aWiFiInterfacesPsmStr));
            CcspTraceInfo(("%s:%d, aBr106LnfInterfaces:%s\n",__FUNCTION__,__LINE__,aBr106LnfInterfaces));
            psmSet(aWiFiInterfacesPsmStr,aBr106LnfInterfaces);
        }
        else if ((false == sCurrentLanConfig.bMwEnable) && ('\0' != aWiFiInterfaces[0]))
        {
            snprintf (aWiFiInterfacesPsmStr, BUFF_LEN_64,l2netWiFiMembers, BR106_PSM_INDEX);
            CcspTraceInfo(("%s:%d, aWiFiInterfacesPsmStr:%s\n",__FUNCTION__,__LINE__,aWiFiInterfacesPsmStr));
            CcspTraceInfo(("%s:%d, aBr106LnfInterfaces:%s\n",__FUNCTION__,__LINE__,aBr106LnfInterfaces));
            psmSet(aWiFiInterfacesPsmStr,aBr106LnfInterfaces);
            snprintf(aWiFiInterfacesPsmStr, BUFF_LEN_64, l2netWiFiMembers, sManageWiFiInfo.aBridgeIndex);
            CcspTraceInfo(("%s:%d, aWiFiInterfacesPsmStr:%s\n",__FUNCTION__,__LINE__,aWiFiInterfacesPsmStr));
            memset(aWiFiInterfaces,0, BUFF_LEN_64);
            CcspTraceInfo(("%s:%d, aWiFiInterfaces:%s\n",__FUNCTION__,__LINE__,aWiFiInterfaces));
            if (0 == psmSet(aWiFiInterfacesPsmStr,aWiFiInterfaces))
                strncpy(sBackupLanConfig.aWiFiInterfaces, aWiFiInterfaces, (sizeof(sBackupLanConfig.aWiFiInterfaces)-1));
        }
        if (sBackupLanConfig.bMwEnable != sCurrentLanConfig.bMwEnable)
        {
            if (true == sCurrentLanConfig.bMwEnable)
                snprintf(aParamVal, BUFF_LEN_64,"true");
            else if (false == sCurrentLanConfig.bMwEnable)
            {
                snprintf(aParamVal, BUFF_LEN_64,"false");
                memset (&sCurrentLanConfig, 0, sizeof(backupLanconfig_t));
                sCurrentLanConfig.bMwEnable = false;
                sCurrentLanConfig.bDhcpServerEnable = false;
                sCurrentLanConfig.bIpv6Enable = false;
            }
            publishEvent(MANAGE_WIFI_ENABLE,aParamVal, RBUS);
        }
        updateBackupConfig();

    }
    else
        CcspTraceError(("%s:%d, pStrVal returned NULL by R-Bus\n",__FUNCTION__,__LINE__));

    /* release rbus variable */
    rbusValue_Release(value);
    free(pStrVal);
    return 0;
}

//Unpack the managedwifi blob data
BOOL unpackAndProcessManagedWifiData(char* pString)
{
    char * decodeMsg =NULL;
    char * wifi_encoded_data = NULL;
    int size =0;
    int retval = 0;
    msgpack_unpack_return unpack_ret = MSGPACK_UNPACK_SUCCESS;
    if (NULL == pString)
    {
        CcspTraceError(("%s:%d, NULL parameter passed\n",__FUNCTION__,__LINE__));
        return FALSE;
    }
    retval = get_base64_decodedbuffer(pString, &decodeMsg, &size);
    if (retval == 0)
    {
        unpack_ret = get_msgpack_unpack_status(decodeMsg,size);
    } 
    else
    {
        if (decodeMsg)
        {
	    free(decodeMsg);
	    decodeMsg = NULL;
        }
        return FALSE;
    }
    if(unpack_ret == MSGPACK_UNPACK_SUCCESS)
    {   
        managedwifidoc_t *md = NULL;
        lanconfigdoc_t *ld = NULL;

        md = managedwifidoc_convert( decodeMsg, size+1 );
        if ( decodeMsg )
        {
            free(decodeMsg);
            decodeMsg = NULL;
        }  

        if (NULL !=md)
        {
            CcspTraceInfo(("The transaction id is %d\n", md->transaction_id));
            CcspTraceInfo(("The version is %lu\n", (long)md->version));
            CcspTraceInfo(("The subdoc_name is %s\n", md->subdoc_name));
            
            //Convert the lanconfig doc
            ld = lanconfigdoc_convert( md->entries[0].value, md->entries[0].value_size );
 
            //wifi_encoded_data is the b64 encoded data obtained after appending subdoc_name, version and transaction_id
            wifi_encoded_data = mg_append_wifi_doc(md->subdoc_name,md->version,md->transaction_id,md->entries[1].value,md->entries[1].value_size);
            CcspTraceInfo(("The wifi_encoded_data is %s\n", wifi_encoded_data));

            execData *execDataManagedWifi = NULL;
            policySequence *sequenceDetails = NULL;     
            execDataManagedWifi = (execData*) malloc (sizeof(execData));
            sequenceDetails = (policySequence*) malloc (sizeof (policySequence) );
            memset(sequenceDetails, 0, sizeof(policySequence));
            sequenceDetails->isExecInSequenceNeeded = 1 ;
            sequenceDetails->numOfComponents = 2 ;
            // Post exec settings
            sequenceDetails->postExecSettings = confirmManageWifiVap;

            sequenceDetails->multiCompExecData = (MultiComp_ExecInfo*) malloc (sequenceDetails->numOfComponents * sizeof(MultiComp_ExecInfo));
            memset(sequenceDetails->multiCompExecData, 0, sequenceDetails->numOfComponents * sizeof(MultiComp_ExecInfo));

            sequenceDetails->multiCompExecData->isMaster = 1 ;
            sequenceDetails->multiCompExecData->comp_exec_data = (void*) ld ;
            CcspTraceInfo(("DEBUG : master data %p\n",sequenceDetails->multiCompExecData->comp_exec_data));
            
            sequenceDetails->multiCompExecData++;
            sequenceDetails->multiCompExecData->isMaster =0 ;

            #if defined (RDK_ONEWIFI)
            strncpy(sequenceDetails->multiCompExecData->CompName,"OneWifi",sizeof(sequenceDetails->multiCompExecData->CompName)-1);
#else
            strncpy(sequenceDetails->multiCompExecData->CompName,"CcspWifiSsp",sizeof(sequenceDetails->multiCompExecData->CompName)-1);
#endif
	    CcspTraceInfo(("DEBUG : wifi_encoded_data %p\n",wifi_encoded_data));
             
            sequenceDetails->multiCompExecData->comp_exec_data = (void*) wifi_encoded_data ;

            CcspTraceInfo(("DEBUG : sequenceDetails->multiCompExecData->comp_exec_data pointer is %p\n",sequenceDetails->multiCompExecData->comp_exec_data));

            sequenceDetails->multiCompExecData--;
            
            if ( execDataManagedWifi != NULL )
            {
                memset(execDataManagedWifi, 0, sizeof(execData));
                execDataManagedWifi->txid = md->transaction_id; 
                execDataManagedWifi->version = (uint32_t)md->version; 
                execDataManagedWifi->numOfEntries = 0;
                strncpy(execDataManagedWifi->subdoc_name,"connectedbuilding",sizeof(execDataManagedWifi->subdoc_name)-1);
                execDataManagedWifi->executeBlobRequest = processManagedWebconfigConfig;
                execDataManagedWifi->rollbackFunc = rollbackLanconfig;
                execDataManagedWifi->freeResources = freeMem_managedwifi;
                execDataManagedWifi->calcTimeout = calculateTimeout;
                execDataManagedWifi->user_data = (void*) sequenceDetails;
                execDataManagedWifi->multiCompRequest =1;       
                PushMultiCompBlobRequest(execDataManagedWifi);
                CcspTraceInfo(("PushBlobRequest complete\n"));
                return TRUE;
            }
            else
            {
                CcspTraceWarning(("execData memory allocation failed\n"));
                lanConfigDoc_destroy(ld);
                connectedbuilding_destroy(md);
                return FALSE;
            }
        }
        return TRUE;
    }
    else
    {
        if ( decodeMsg )
        {
            free(decodeMsg);
            decodeMsg = NULL;
        }
        CcspTraceInfo(("Corrupted managedwifi value\n"));
        return FALSE;
    }
    return TRUE;
}


pErr processManagedWebconfigConfig(void *Data)
{
    pErr execRetVal = NULL;

    if (NULL == Data)
    {
        CcspTraceError(("%s:%d, Data is NULL\n",__FUNCTION__,__LINE__));
        execRetVal = (pErr) malloc (sizeof(Err));
        if (execRetVal != NULL )
        {
            memset(execRetVal,0,sizeof(Err));
            snprintf(execRetVal->ErrorMsg, BUFF_LEN_128,"NULL parameter passed\n");
            execRetVal->ErrorCode = NULL_BLOB_EXEC_POINTER;
        }
        return execRetVal;
    }
    lanconfigdoc_t *mwd = (lanconfigdoc_t *) Data ;

    CcspTraceInfo(("%s : DhcpServerEnable - %d\n",__FUNCTION__,mwd->entries->dhcp_server_enable));
    CcspTraceInfo(("%s : LanIPAddress - %s\n",__FUNCTION__,mwd->entries->lan_ip_address));
    CcspTraceInfo(("%s : LanSubnetMask - %s\n",__FUNCTION__,mwd->entries->lan_subnet_mask));
    CcspTraceInfo(("%s : DhcpStartIPAddress - %s\n",__FUNCTION__,mwd->entries->dhcp_start_ip_address));
    CcspTraceInfo(("%s : DhcpEndIPAddress - %s\n",__FUNCTION__,mwd->entries->dhcp_end_ip_address));
    CcspTraceInfo(("%s : LeaseTime - %s\n",__FUNCTION__,mwd->entries->leasetime));
    CcspTraceInfo(("%s : enable - %d\n",__FUNCTION__,mwd->entries->mwenable));
    CcspTraceInfo(("%s : ipv6enable - %d\n",__FUNCTION__,mwd->entries->ipv6enable));
    CcspTraceInfo(("%s : firewall - %s\n",__FUNCTION__,mwd->entries->firewall));
    CcspTraceInfo(("%s : alias - %s\n",__FUNCTION__,mwd->entries->alias));
    execRetVal = create_manage_wifi_bridge(mwd->entries);
    return execRetVal;
}

void freeMem_managedwifi(void* arg)
{
    execData *blob_exec_data  = (execData*) arg;

    if (NULL == arg)
        return;

    policySequence *sequenceDetails = (policySequence*)blob_exec_data->user_data; 
    /*CID-346806  Null pointer Dereference Fix*/
    if( sequenceDetails == NULL)
    {
        CcspTraceInfo(("sequenceDetails is NULL in %s \n",__FUNCTION__));
        return;
    }
    lanconfigdoc_t *mwd = (lanconfigdoc_t *) sequenceDetails->multiCompExecData->comp_exec_data;
    if ( mwd != NULL )
    {
        lanConfigDoc_destroy( mwd );
        mwd = NULL;
    }
    sequenceDetails->multiCompExecData++;
    char * wifi_encoded_data = (char*) sequenceDetails->multiCompExecData->comp_exec_data;
    
    if ( wifi_encoded_data != NULL )
    {
        free(wifi_encoded_data);  
        wifi_encoded_data = NULL;
    }
    
    sequenceDetails->multiCompExecData--;
    if (sequenceDetails->multiCompExecData)
    {
        free(sequenceDetails->multiCompExecData);
        sequenceDetails->multiCompExecData = NULL;
    }
    
    if (sequenceDetails)
    {
        free(sequenceDetails);
        sequenceDetails = NULL;
    }

    if ( blob_exec_data != NULL )
    {
        free(blob_exec_data);
       	blob_exec_data = NULL ;
    }
}

size_t calculateTimeout(size_t numOfEntries){
    UNREFERENCED_PARAMETER(numOfEntries);
    CcspTraceInfo(("calling calculateTimeout\n"));
    return 30;
}

void * ManageWiFiBridgeCreationThread(void * vArg)
{
    UNREFERENCED_PARAMETER(vArg);
    char aMultiNetStatus[BUFF_LEN_64] = {0};
    char aParamVal[BUFF_LEN_32] = {0};
    async_id_t interfaceManageWiFiAsyncid;
    static int syseventManageWiFiFd;
    static token_t syseventManageWiFiToken;
    int err;
    char name[25] = {0}, val[42] = {0}, buf[BUFF_LEN_128], cmd[BUFF_LEN_128];
    int oldtype;
    threadStruct_t * pThreadStruct = (threadStruct_t*) vArg;

    /* allow the thread to be killed at any time */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    pthread_detach(pthread_self());

    syseventManageWiFiFd = sysevent_open("127.0.0.1", SE_SERVER_WELL_KNOWN_PORT, SE_VERSION, "ManageWiFiBridgeHandler", &syseventManageWiFiToken);

    CcspTraceInfo(("%s:%d,pThreadStruct->bMwEnable:%d\n",__FUNCTION__,__LINE__, pThreadStruct->bMwEnable));
    CcspTraceInfo(("%s:%d,aBridgeIndex:%s\n",__FUNCTION__,__LINE__, sManageWiFiInfo.aBridgeIndex));
    snprintf (aMultiNetStatus,BUFF_LEN_64,"multinet_%s-status",sManageWiFiInfo.aBridgeIndex);
    CcspTraceInfo(("%s:%d,aMultiNetStatus:%s\n",__FUNCTION__,__LINE__, aMultiNetStatus));

    if(0 == sysevent_get(syseventManageWiFiFd, syseventManageWiFiToken, aMultiNetStatus, aParamVal,BUFF_LEN_32 ) && '\0' != aParamVal[0])
    {
        CcspTraceInfo(("%s:%d,aParamVal:%s\n",__FUNCTION__,__LINE__, aParamVal));
        CcspTraceInfo(("%s:%d,pThreadStruct->bMwEnable:%d\n",__FUNCTION__,__LINE__, pThreadStruct->bMwEnable));
        if ((!strncmp(aParamVal,"ready", strlen("ready"))) && (true == pThreadStruct->bMwEnable))
        {
            snprintf(aMultiNetStatus, BUFF_LEN_64, "%s%s",sManageWiFiInfo.aKey,sManageWiFiInfo.aBridgeName);
            CcspTraceInfo(("%s:%d,aMultiNetStatus:%s\n",__FUNCTION__,__LINE__, aMultiNetStatus));
            publishEvent(MANAGE_WIFI_LAN_BRIDGE,aMultiNetStatus, RBUS);
            /* wake up the caller if execution is completed in time */
            pthread_cond_signal(&manageWifi_exec_completed);
            if (0 < syseventManageWiFiFd)
                sysevent_close(syseventManageWiFiFd, syseventManageWiFiToken);
            return vArg;
        }
        else if ((!strcmp ((const char*)val, "stopped")) && (false == pThreadStruct->bMwEnable))
        {
            publishEvent(MANAGE_WIFI_LAN_BRIDGE,sManageWiFiInfo.aKey, RBUS);
            CcspTraceInfo(("%s:%d, Restarting the zebra\n",__FUNCTION__,__LINE__));
            if (0 != commonSyseventSet("zebra-restart", ""))
                CcspTraceError(("%s:%d, commonSyseventSet failed for zebra-restart\n",__FUNCTION__,__LINE__));
            CcspTraceInfo(("%s:%d, Restarting the dhcp_server\n",__FUNCTION__,__LINE__));
            int fd = creat("/var/tmp/lan_not_restart", O_WRONLY|O_CREAT);
            if (-1 != fd)
                close(fd);
            else
                CcspTraceError(("%s:%d, Failed to create the /var/tmp/lan_not_restart file\n",__FUNCTION__,__LINE__));
            if (0 != commonSyseventSet("dhcp_server-restart", ""))
                CcspTraceError(("%s:%d, commonSyseventSet failed for dhcp_server-restart\n",__FUNCTION__,__LINE__));
            /* wake up the caller if execution is completed in time */
            pthread_cond_signal(&manageWifi_exec_completed);
            if (0 < syseventManageWiFiFd)
                sysevent_close(syseventManageWiFiFd, syseventManageWiFiToken);
            return vArg;
        }
    }

    sysevent_set_options(syseventManageWiFiFd, syseventManageWiFiToken, aMultiNetStatus, TUPLE_FLAG_EVENT);
    sysevent_setnotification(syseventManageWiFiFd, syseventManageWiFiToken,aMultiNetStatus,  &interfaceManageWiFiAsyncid);

    while(1)
    {
        async_id_t getnotification_asyncid;
        memset(name,0,sizeof(name));
        memset(val,0,sizeof(val));
        memset(cmd,0,sizeof(cmd));
        memset(buf,0,sizeof(buf));

        int namelen = sizeof(name);
        int vallen  = sizeof(val);
        err = sysevent_getnotification(syseventManageWiFiFd, syseventManageWiFiToken, name, &namelen,  val, &vallen, &getnotification_asyncid);

        if (err)
        {
            CcspTraceWarning(("sysevent_getnotification failed with error: %d %s\n", err,__FUNCTION__));
            CcspTraceWarning(("sysevent_getnotification failed name: %s val : %s\n", name,val));
            if ( 0 != v_secure_system("pidof syseventd"))
            {
                CcspTraceWarning(("%s syseventd not running ,breaking the receive notification loop \n",__FUNCTION__));
                break;
            }
        }
        else
        {
            CcspTraceInfo(("%s:%d,Received the Notification :%s:%s\n",__FUNCTION__,__LINE__, aMultiNetStatus,val));
            CcspTraceWarning(("%s Recieved notification event  %s\n",__FUNCTION__,name));
            if(!strcmp((const char*)name,aMultiNetStatus))
            {
                CcspTraceInfo(("%s:%d,val:%s\n",__FUNCTION__,__LINE__, val));
                if(!strcmp((const char*)val, "ready"))
                {
                    snprintf(aMultiNetStatus, BUFF_LEN_64, "%s%s",sManageWiFiInfo.aKey,sManageWiFiInfo.aBridgeName);
                    CcspTraceInfo(("%s:%d,notifying:%s\n",__FUNCTION__,__LINE__, aMultiNetStatus));
                    publishEvent(MANAGE_WIFI_LAN_BRIDGE,aMultiNetStatus, RBUS);
                    if (pThreadStruct->cFlag & (1 << 4))
                    {
                        CcspTraceInfo(("%s:%d, Restarting the zebra\n",__FUNCTION__,__LINE__));
                        if (0 != commonSyseventSet("zebra-restart", ""))
                            CcspTraceError(("%s:%d, commonSyseventSet failed for zebra-restart\n",__FUNCTION__,__LINE__));
                    }
                    if ((pThreadStruct->cFlag & (1 << 3)) || (pThreadStruct->cFlag & (1 << 2)))
                    {
                        CcspTraceInfo(("%s:%d, Restarting the dhcp_server\n",__FUNCTION__,__LINE__));
                        int fd = creat("/var/tmp/lan_not_restart", O_WRONLY|O_CREAT);
                        if (-1 != fd)
                            close(fd);
                        else
                            CcspTraceError(("%s:%d, Failed to create the /var/tmp/lan_not_restart file\n",__FUNCTION__,__LINE__));
                        if (0 != commonSyseventSet("dhcp_server-restart", ""))
                            CcspTraceError(("%s:%d, commonSyseventSet failed for dhcp_server-restart\n",__FUNCTION__,__LINE__));
                    }
                    break;
                }
                else if (!strcmp ((const char*)val, "stopped"))
                {
                    CcspTraceInfo(("%s:%d,notifying:%s\n",__FUNCTION__,__LINE__, sManageWiFiInfo.aKey));
                    publishEvent(MANAGE_WIFI_LAN_BRIDGE,sManageWiFiInfo.aKey, RBUS);
                    CcspTraceInfo(("%s:%d, Restarting the zebra\n",__FUNCTION__,__LINE__));
                    if (0 != commonSyseventSet("zebra-restart", ""))
                        CcspTraceError(("%s:%d, commonSyseventSet failed for zebra-restart\n",__FUNCTION__,__LINE__));
                    CcspTraceInfo(("%s:%d, Restarting the dhcp_server\n",__FUNCTION__,__LINE__));
                    int fd = creat("/var/tmp/lan_not_restart", O_WRONLY|O_CREAT);
                    if (-1 != fd)
                        close(fd);
                    else
                        CcspTraceError(("%s:%d, Failed to create the /var/tmp/lan_not_restart file\n",__FUNCTION__,__LINE__));
                    if (0 != commonSyseventSet("dhcp_server-restart", ""))
                        CcspTraceError(("%s:%d, commonSyseventSet failed for dhcp_server-restart\n",__FUNCTION__,__LINE__));
                    break;
                }
            }
        }
    }
    /* wake up the caller if execution is completed in time */
    pthread_cond_signal(&manageWifi_exec_completed);
    if (0 < syseventManageWiFiFd)
        sysevent_close(syseventManageWiFiFd, syseventManageWiFiToken);
    return vArg;
}

pErr create_manage_wifi_bridge(lanconfig_t * pLanConfigParams)
{
    char aParamName[BUFF_LEN_64]  = {0};
    char aParamVal[BUFF_LEN_64] = {0};
    pErr pErrRetVal = NULL;

    /*
     * 0th bit : Manage Wifi Enable
     * 1st bit : Lan IP addr
     * 2nd bit : lan subnet mask, dhcp V4 pool subnet mask
     * 3rd bit : DhcpV4Pool Enable, Start and End address, lease time
     * 4th bit : IPv6 Enable
     * 5th bit : alias
     */
    char cFlag = 0;

    pErrRetVal = (pErr) malloc (sizeof(Err));
    if (NULL == pErrRetVal)
    {
        CcspTraceError(("%s,%d : malloc failed\n",__FUNCTION__,__LINE__));
        return pErrRetVal;
    }
    memset(pErrRetVal,0,sizeof(Err));
    pErrRetVal->ErrorCode = BLOB_EXEC_SUCCESS;

    if (NULL == pLanConfigParams)
    {
        CcspTraceError(("%s: NULL parameter passed\n",__FUNCTION__));
        snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"NULL parameter passed\n");
        pErrRetVal->ErrorCode = NULL_BLOB_EXEC_POINTER;
        return pErrRetVal;
    }

    if (true == pLanConfigParams->mwenable)
    {
        if (0 != validateIpRange(pLanConfigParams->lan_ip_address, pLanConfigParams->dhcp_start_ip_address, pLanConfigParams->dhcp_end_ip_address, pErrRetVal))
            return pErrRetVal;

	if (0 != validateLeaseTime(pLanConfigParams->leasetime, pErrRetVal))
            return pErrRetVal;
    }
    if (pLanConfigParams->mwenable != sBackupLanConfig.bMwEnable)
    {
        sCurrentLanConfig.bMwEnable = pLanConfigParams->mwenable;
        if (true == pLanConfigParams->mwenable)
            snprintf(aParamVal, BUFF_LEN_64,"true");
        else if (false == pLanConfigParams->mwenable)
            snprintf(aParamVal, BUFF_LEN_64,"false");
        snprintf(aParamName, BUFF_LEN_64, l2netBridgeEnable, sManageWiFiInfo.aBridgeIndex);
        if (0 != psmSet(aParamName, aParamVal))
        {
            iRestoreFlag |= (1 << 0);
            CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
            snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Enable\n");
            pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
            return pErrRetVal;
        }
        iRestoreFlag |= (1 << 1);
        cFlag |= (1 << 0);
    }
    else if ((false == pLanConfigParams->mwenable) && (false == sBackupLanConfig.bMwEnable))
    {
        CcspTraceError(("%s: previously MW Enable is false, again received false\n",__FUNCTION__));
        return pErrRetVal;
    }
    else
        sCurrentLanConfig.bMwEnable = pLanConfigParams->mwenable;

    if (NULL != pLanConfigParams->alias)
    {
        if (strcmp(sBackupLanConfig.aAlias, pLanConfigParams->alias))
        {
            snprintf(aParamName, BUFF_LEN_64, l2netAlias, sManageWiFiInfo.aBridgeIndex);
            snprintf(aParamVal, BUFF_LEN_64, "%s", pLanConfigParams->alias);
            if (0 != psmSet(aParamName, aParamVal))
            {
                iRestoreFlag |= (1 << 0);
                CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Alias\n");
                pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
                return pErrRetVal;
            }
            iRestoreFlag |= (1 << 10);
            cFlag |= (1 << 5);
        }
        strncpy(sCurrentLanConfig.aAlias, pLanConfigParams->alias, (sizeof(sCurrentLanConfig.aAlias)-1));
    }

    if (NULL != pLanConfigParams->lan_ip_address)
    {
        if (strcmp(pLanConfigParams->lan_ip_address, sBackupLanConfig.aLanIpAddr))
        {
            snprintf(aParamName, BUFF_LEN_64, l3netV4Addr, sManageWiFiInfo.aBridgeIndex);
            if( 0 != psmSet(aParamName, pLanConfigParams->lan_ip_address))
            {
                iRestoreFlag |= (1 << 0);
                CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Lan ip address\n");
                pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
                return pErrRetVal;
            }
            iRestoreFlag |= (1 << 2);
            cFlag |= (1 << 1);
        }
        strncpy(sCurrentLanConfig.aLanIpAddr, pLanConfigParams->lan_ip_address, (sizeof(sCurrentLanConfig.aLanIpAddr)-1));
    }

    if (NULL != pLanConfigParams->lan_subnet_mask)
    {
        if(strcmp(pLanConfigParams->lan_subnet_mask, sBackupLanConfig.aLanSubnetMask))
        {
            snprintf(aParamName, BUFF_LEN_64, l2netV4SubnetMask, sManageWiFiInfo.aBridgeIndex);
            if( 0 != psmSet(aParamName, pLanConfigParams->lan_subnet_mask))
            {
                iRestoreFlag |= (1 << 0);
                CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Lan subnet mask\n");
                pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
                return pErrRetVal;
            }
            strncpy(sCurrentLanConfig.aLanSubnetMask, pLanConfigParams->lan_subnet_mask, (sizeof(sCurrentLanConfig.aLanSubnetMask)-1));
            iRestoreFlag |= (1 << 3);
            cFlag |= (1 << 2);
            snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolSubnetMask, sManageWiFiInfo.aBridgeIndex);
            if( 0 != psmSet(aParamName, pLanConfigParams->lan_subnet_mask))
            {
                iRestoreFlag |= (1 << 0);
                CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Dhcp range subnet mask\n");
                pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
                return pErrRetVal;
            }
            iRestoreFlag |= (1 << 4);
            cFlag |= (1 << 2);
        }
        strncpy(sCurrentLanConfig.aLanSubnetMask, pLanConfigParams->lan_subnet_mask, (sizeof(sCurrentLanConfig.aLanSubnetMask)-1));
    }

    if(false == pLanConfigParams->mwenable)
    {
        sCurrentLanConfig.bDhcpServerEnable = false;
        iRestoreFlag |= (1 << 5);
        cFlag |= (1 << 3);
    }
    else if (pLanConfigParams->dhcp_server_enable != sBackupLanConfig.bDhcpServerEnable)
    {
        snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolEnable, sManageWiFiInfo.aBridgeIndex);
        if (true == pLanConfigParams->dhcp_server_enable)
            snprintf(aParamVal, BUFF_LEN_64,"true");
        else if (false == pLanConfigParams->dhcp_server_enable)
            snprintf(aParamVal, BUFF_LEN_64,"false");
        if( 0 != psmSet(aParamName, aParamVal))
        {
            iRestoreFlag |= (1 << 0);
            CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
            snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Dhcp Server Enable\n");
            pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
            return pErrRetVal;
        }
        sCurrentLanConfig.bDhcpServerEnable = pLanConfigParams->dhcp_server_enable;
        iRestoreFlag |= (1 << 5);
        cFlag |= (1 << 3);
    }
    else
        sCurrentLanConfig.bDhcpServerEnable = pLanConfigParams->dhcp_server_enable;

    if (NULL != pLanConfigParams->dhcp_start_ip_address)
    {
        if (strcmp(pLanConfigParams->dhcp_start_ip_address, sBackupLanConfig.aDhcpStartIpAdd))
        {
            snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolMinAddr,sManageWiFiInfo.aBridgeIndex);
            if( 0 != psmSet(aParamName, pLanConfigParams->dhcp_start_ip_address))
            {
                iRestoreFlag |= (1 << 0);
                CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Dhcp start ip addr\n");
                pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
                return pErrRetVal;
            }
            iRestoreFlag |= (1 << 6);
            cFlag |= (1 << 3);
        }
        strncpy(sCurrentLanConfig.aDhcpStartIpAdd, pLanConfigParams->dhcp_start_ip_address, (sizeof(sCurrentLanConfig.aDhcpStartIpAdd)-1));
    }

    if (NULL != pLanConfigParams->dhcp_end_ip_address)
    {
        if (strcmp(pLanConfigParams->dhcp_end_ip_address, sBackupLanConfig.aDhcpEndIpAdd))
        {
            snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolMaxAddr,sManageWiFiInfo.aBridgeIndex);
            if( 0 != psmSet(aParamName, pLanConfigParams->dhcp_end_ip_address))
            {
                iRestoreFlag |= (1 << 0);
                CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Dhcp end ip addr\n");
                pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
                return pErrRetVal;
            }
            iRestoreFlag |= (1 << 7);
            cFlag |= (1 << 3);
        }
        strncpy(sCurrentLanConfig.aDhcpEndIpAdd, pLanConfigParams->dhcp_end_ip_address, (sizeof(sCurrentLanConfig.aDhcpEndIpAdd)-1));
    }
    if (NULL != pLanConfigParams->leasetime)
    {
        if (strcmp(pLanConfigParams->leasetime, sBackupLanConfig.aLeaseTime))
        {
            snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolLeaseTime,sManageWiFiInfo.aBridgeIndex);
            if( 0 != psmSet(aParamName, pLanConfigParams->leasetime))
            {
                iRestoreFlag |= (1 << 0);
                CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi Dhcp lease time\n");
                pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
                return pErrRetVal;
            }
            iRestoreFlag |= (1 << 8);
            cFlag |= (1 << 3);
        }
        strncpy(sCurrentLanConfig.aLeaseTime, pLanConfigParams->leasetime, (sizeof(sCurrentLanConfig.aLeaseTime)-1));
    }
    if (false == pLanConfigParams->mwenable)
    {
        sCurrentLanConfig.bIpv6Enable = false;
        iRestoreFlag |= (1 << 9);
        cFlag |= (1 << 4);
    }
    else if (pLanConfigParams->ipv6enable != sBackupLanConfig.bIpv6Enable)
    {
        snprintf(aParamName, BUFF_LEN_64, l3netIPv6Enable, sManageWiFiInfo.aBridgeIndex);
        if (true == pLanConfigParams->ipv6enable)
            snprintf(aParamVal,BUFF_LEN_64, "true");
        else if (false == pLanConfigParams->ipv6enable)
            snprintf(aParamVal,BUFF_LEN_64, "false");
        if( 0 != psmSet(aParamName, aParamVal))
        {
            iRestoreFlag |= (1 << 0);
            CcspTraceError(("%s: psm Set failed\n",__FUNCTION__));
            snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"PSM set failed for Manage WiFi IPv6 Enable\n");
            pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
            return pErrRetVal;
        }
        sCurrentLanConfig.bIpv6Enable = pLanConfigParams->ipv6enable;
        iRestoreFlag |= (1 << 9);
        cFlag |= (1 << 4);
    }
    else
        sCurrentLanConfig.bIpv6Enable = pLanConfigParams->ipv6enable;

    processManageWifiData (&sCurrentLanConfig, cFlag, pErrRetVal);
    if (NULL != pErrRetVal)
    {
        if (BLOB_EXEC_SUCCESS != pErrRetVal->ErrorCode)
            iRestoreFlag |= (1 << 0);
    }
    return pErrRetVal;
}

void processManageWifiData(backupLanconfig_t * pLanConfig, char cFlag, pErr pErrRetVal)
{
    pthread_t manageWifiBridgeThreadId;
    struct timespec abs_time = {0};
    int pthreadRetValue=0, err = 0;
    char buf[BUFF_LEN_128] = {'\0'};

    if ((NULL == pLanConfig) || (NULL == pErrRetVal))
        return;

    if (cFlag & (1 << 4))
    {
        CcspTraceInfo(("%s:%d, ipv6enable is enabled =%d\n", __FUNCTION__,__LINE__,pLanConfig->bIpv6Enable));
        if (true == pLanConfig->bIpv6Enable)
        {
            syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));
            if (('\0' != sManageWiFiInfo.aBridgeName[0]) && (NULL == strstr(buf, sManageWiFiInfo.aBridgeName)))
                append_interface(sManageWiFiInfo.aBridgeName);
        }
        if (false == pLanConfig->bIpv6Enable)
        {
            syscfg_get( NULL, "IPv6_Interface", buf, sizeof(buf));
            if (('\0' != sManageWiFiInfo.aBridgeName[0]) && (NULL != strstr(buf, sManageWiFiInfo.aBridgeName)))
                remove_interface(sManageWiFiInfo.aBridgeName);
        }
    }

    if (cFlag & (1 << 0))
    {
        threadStruct_t sThread = {false, 0};
        CcspTraceInfo(("%s:%d, bMwEnable :%d\n",__FUNCTION__,__LINE__,pLanConfig->bMwEnable));
        pthread_condattr_init(&manageWifi_attr);
        pthread_condattr_setclock(&manageWifi_attr, CLOCK_MONOTONIC);
        pthread_cond_init(&manageWifi_exec_completed, &manageWifi_attr);

        sThread.bMwEnable = pLanConfig->bMwEnable;
        sThread.cFlag = cFlag;
        pthreadRetValue = pthread_create(&manageWifiBridgeThreadId, NULL, &ManageWiFiBridgeCreationThread, (void*)&sThread);

        if (true == pLanConfig->bMwEnable)
        {
            syscfg_set_commit(NULL, "Manage_WiFi_Enabled", "true");

            CcspTraceInfo(("%s: Setting the multinet-up %s\n",__FUNCTION__,sManageWiFiInfo.aBridgeIndex));
            if (0 != commonSyseventSet("multinet-up", sManageWiFiInfo.aBridgeIndex))
            {
                CcspTraceError(("%s: commonSyseventSet Failed for multinet-Up %s\n", __FUNCTION__, sManageWiFiInfo.aBridgeIndex));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"commonSyseventSet Failed For Multinet-Up \n");
                pErrRetVal->ErrorCode = SYSEVENT_FAILURE;
                if (0 == pthreadRetValue)
                {
                    CcspTraceInfo(("%s:%d, Timedout Cancelling the ManageWiFiThread\n",__FUNCTION__,__LINE__));
                    pthread_cancel(manageWifiBridgeThreadId);
                }
                return ;
            }
        }
        if (false == pLanConfig->bMwEnable)
        {
            syscfg_set_commit(NULL, "Manage_WiFi_Enabled", "false");

            if (0 != commonSyseventSet("multinet-down", sManageWiFiInfo.aBridgeIndex))
            {
                CcspTraceError(("%s: sysevnt_set Failed for multinet-down sysevent %s\n", __FUNCTION__, sManageWiFiInfo.aBridgeIndex));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"commonSyseventSet Failed For Multinet-Down \n");
                pErrRetVal->ErrorCode = SYSEVENT_FAILURE;
                if (0 == pthreadRetValue)
                {
                    CcspTraceInfo(("%s:%d, Timedout Cancelling the ManageWiFiThread\n",__FUNCTION__,__LINE__));
                    pthread_cancel(manageWifiBridgeThreadId);
                }
                return ;
            }
        }
        if (0 == pthreadRetValue)
        {
            clock_gettime(CLOCK_MONOTONIC, &abs_time);
            abs_time.tv_sec += MAX_MANAGE_WIFI_BRIDGE_THREAD_EXEC_TIMEOUT * 2;
            abs_time.tv_nsec += 0;

            pthread_mutex_lock(&manageWifi_exec);
            err = pthread_cond_timedwait(&manageWifi_exec_completed, &manageWifi_exec, &abs_time);
            if (err == ETIMEDOUT)
            {
                CcspTraceInfo(("%s:%d, Timedout Cancelling the ManageWiFiThread\n",__FUNCTION__,__LINE__));
                pthread_cancel(manageWifiBridgeThreadId);
            }
            pthread_mutex_unlock(&manageWifi_exec);
        }
        CcspTraceInfo(("%s:%d, Restarting the firewall\n",__FUNCTION__,__LINE__));
        if (0 != commonSyseventSet("firewall-restart", sManageWiFiInfo.aBridgeIndex))
        {
            CcspTraceError(("%s: commonSyseventSet Failed for Firewall-restart\n", __FUNCTION__));
            snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"commonSyseventSet Failed For Firewall-restart\n");
            pErrRetVal->ErrorCode = SYSEVENT_FAILURE;
            return ;
        }
    }

    if ((!(cFlag & (1 << 0))) && ((cFlag & (1 << 3)) || (cFlag & (1 << 2))))
    {
        CcspTraceInfo(("%s:%d, Restarting the dhcp_server\n",__FUNCTION__,__LINE__));
        int fd = creat("/var/tmp/lan_not_restart", O_WRONLY|O_CREAT);
        if (-1 != fd)
            close(fd);
        else
            CcspTraceError(("%s:%d, Failed to create the /var/tmp/lan_not_restart file\n",__FUNCTION__,__LINE__));
        if (0 != commonSyseventSet("dhcp_server-restart", ""))
        {
            CcspTraceError(("%s:%d, commonSyseventSet failed for dhcp_server-restart\n",__FUNCTION__,__LINE__));
            snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"Failed to set dhcp_server-restart Sysevent\n");
            pErrRetVal->ErrorCode = SYSEVENT_FAILURE;
            return ;
        }
    }
    if ((!(cFlag & (1 << 0))) && (cFlag & (1 << 4)))
    {
        CcspTraceInfo(("%s:%d, Restarting the zebra\n",__FUNCTION__,__LINE__));
        if (0 != commonSyseventSet("zebra-restart", ""))
        {
            CcspTraceError(("%s:%d, commonSyseventSet failed for zebra-restart\n",__FUNCTION__,__LINE__));
            snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"sysevent set failed for zebra-restart \n");
            pErrRetVal->ErrorCode = SYSEVENT_FAILURE;
            return ;
        }
    }
    if ((!(cFlag & (1 << 0))) && ((cFlag & (1 << 1)) || (cFlag & (1 << 2))))
    {
        char aLanIpAddress [BUFF_LEN_32] = {'\0'};
        char aLanSubnetMask[BUFF_LEN_32] = {'\0'};
        CcspTraceInfo(("%s:%d, bridge:%s, Current lan IP:%s and current Lan subnetMask:%s\n",__FUNCTION__,__LINE__,sManageWiFiInfo.aBridgeName,pLanConfig->aLanIpAddr, pLanConfig->aLanSubnetMask));
        if ('\0' == pLanConfig->aLanIpAddr[0])
        {
            strncpy(aLanIpAddress, sBackupLanConfig.aLanIpAddr, (sizeof(aLanIpAddress)-1));
            CcspTraceInfo(("%s:%d, lan IP:%s \n",__FUNCTION__,__LINE__,aLanIpAddress));
        }
        else
            strncpy(aLanIpAddress, pLanConfig->aLanIpAddr, (sizeof(aLanIpAddress)-1));
        if ('\0'== pLanConfig->aLanSubnetMask[0])
        {
            strncpy(aLanSubnetMask, sBackupLanConfig.aLanSubnetMask, (sizeof(aLanSubnetMask)-1));
            CcspTraceInfo(("%s:%d, lan subnetMask :%s \n",__FUNCTION__,__LINE__,aLanSubnetMask));
        }
        else
            strncpy(aLanSubnetMask, pLanConfig->aLanSubnetMask, (sizeof(aLanSubnetMask)-1));
        CcspTraceInfo(("%s:%d Updating the IP, bridge:%s, IP:%s and subnetMask:%s\n",__FUNCTION__,__LINE__,sManageWiFiInfo.aBridgeName,aLanIpAddress, aLanSubnetMask));
        if (0 != v_secure_system("ifconfig %s %s netmask %s up",sManageWiFiInfo.aBridgeName,aLanIpAddress, aLanSubnetMask))
        {
            CcspTraceError(("%s:%d, Failed to udpate the IP and subnetmask\n",__FUNCTION__,__LINE__));
            snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"Failed to update the IP and SubnetMask\n");
            pErrRetVal->ErrorCode = BLOB_EXEC_FAILURE;
            return ;
        }
    }
    return;
}

void getBridgeDetailsFromPsm(void)
{
    char aParamName[BUFF_LEN_64]  = {0};

    /* Update key of manage wifi structure*/
    strncpy(sManageWiFiInfo.aKey, "ManagedWifi:", sizeof(sManageWiFiInfo.aKey)-1);

    psmGet(MANAGE_WIFI_BRIDGE_INDEX, sManageWiFiInfo.aBridgeIndex, BUFF_LEN_8);
    CcspTraceInfo(("%s: aBridgeIndex=%s\n", __FUNCTION__,sManageWiFiInfo.aBridgeIndex));
    if ('\0' == sManageWiFiInfo.aBridgeIndex[0])
    {
        strncpy(sManageWiFiInfo.aBridgeIndex,"17", sizeof(sManageWiFiInfo.aBridgeIndex)-1);
    }

    snprintf(aParamName, BUFF_LEN_64, l2netBridgeName, sManageWiFiInfo.aBridgeIndex);
#if !defined(_64BIT_ARCH_SUPPORT_)
    CcspTraceInfo(("%s: aBridgeName=%d\n", __FUNCTION__,sizeof(sManageWiFiInfo.aBridgeName)));
#else
    CcspTraceInfo(("%s: aBridgeName=%zu\n", __FUNCTION__,sizeof(sManageWiFiInfo.aBridgeName)));
#endif
    CcspTraceInfo(("%s: aParamName=%s\n", __FUNCTION__,aParamName));
    
    /* CID 347167 Unchecked return value fix */
    int ret = psmGet(aParamName, sManageWiFiInfo.aBridgeName, BUFF_LEN_32);
    if (ret != 0) {
        CcspTraceError(("%s:%d, Failed to get bridge details\n",__FUNCTION__,__LINE__));
    } else {
        CcspTraceInfo(("%s: aBridgeName=%s\n", __FUNCTION__,sManageWiFiInfo.aBridgeName));
    }
    if ('\0' == sManageWiFiInfo.aBridgeName[0])
    {
        strncpy(sManageWiFiInfo.aBridgeName,"brlan15", sizeof(sManageWiFiInfo.aBridgeName)-1);
    }
}

void getManageWiFiDetails(ManageWiFiInfo_t * pManageWifiInfo)
{
    if (NULL == pManageWifiInfo)
    {
        CcspTraceError(("%s:%d pManageWifiInfo is NULL\n",__FUNCTION__,__LINE__));
        return;
    }
    if ((true == sBackupLanConfig.bMwEnable) || (true == sCurrentLanConfig.bMwEnable))
    {
        if ('\0' != sManageWiFiInfo.aBridgeName[0])
        {
            strncpy(pManageWifiInfo->aBridgeName,sManageWiFiInfo.aBridgeName, (sizeof(pManageWifiInfo->aBridgeName)-1));
            pManageWifiInfo->aBridgeName[sizeof(pManageWifiInfo->aBridgeName) - 1] = '\0';
            CcspTraceInfo(("%s:%d pManageWifiInfo->aBridgeName:%s\n",__FUNCTION__,__LINE__,pManageWifiInfo->aBridgeName));
        }

        if ('\0' != sManageWiFiInfo.aWiFiInterfaces[0])
        {
            /* CID 347175 : fix */
            strncpy(pManageWifiInfo->aWiFiInterfaces, sManageWiFiInfo.aWiFiInterfaces, (sizeof(pManageWifiInfo->aWiFiInterfaces)-1));
            pManageWifiInfo->aWiFiInterfaces[sizeof(pManageWifiInfo->aWiFiInterfaces) - 1] = '\0';
            CcspTraceInfo(("%s:%d pManageWifiInfo->aWiFiInterfaces:%s\n",__FUNCTION__,__LINE__,pManageWifiInfo->aWiFiInterfaces));
        }
    }
    strncpy(pManageWifiInfo->aKey,sManageWiFiInfo.aKey, (sizeof(pManageWifiInfo->aKey)-1));
    pManageWifiInfo->aKey[sizeof(pManageWifiInfo->aKey) - 1] = '\0';
    CcspTraceInfo(("%s:%d pManageWifiInfo->aKey:%s\n",__FUNCTION__,__LINE__,pManageWifiInfo->aKey));
}

void setManageWiFiDetails(ManageWiFiInfo_t * pManageWifiInfo)
{
    if (NULL != pManageWifiInfo)
    {
        CcspTraceInfo(("%s:%d pManageWifiInfo->aKey:%s\n",__FUNCTION__,__LINE__,pManageWifiInfo->aKey));
        CcspTraceInfo(("%s:%d pManageWifiInfo->aBridgeName:%s\n",__FUNCTION__,__LINE__,pManageWifiInfo->aBridgeName));
        CcspTraceInfo(("%s:%d pManageWifiInfo->aWiFiInterfaces:%s\n",__FUNCTION__,__LINE__,pManageWifiInfo->aWiFiInterfaces));
        strncpy(sManageWiFiInfo.aKey, pManageWifiInfo->aKey,(sizeof(sManageWiFiInfo.aKey)-1));
	strncat(sManageWiFiInfo.aKey, ":", (sizeof(sManageWiFiInfo.aKey)-1));
        if (BRIDGE_NAME == pManageWifiInfo->eUpdateType)
            strncpy(sManageWiFiInfo.aBridgeName, pManageWifiInfo->aBridgeName, (sizeof(sManageWiFiInfo.aBridgeName)-1));
        if (WIFI_INTERFACES == pManageWifiInfo->eUpdateType)
            strncpy(sManageWiFiInfo.aWiFiInterfaces, pManageWifiInfo->aWiFiInterfaces, (sizeof(sManageWiFiInfo.aWiFiInterfaces)-1));
        CcspTraceInfo(("%s:%d sManageWiFiInfo.aKey:%s\n",__FUNCTION__,__LINE__,sManageWiFiInfo.aKey));
        CcspTraceInfo(("%s:%d sManageWiFiInfo.aBridgeName:%s\n",__FUNCTION__,__LINE__,sManageWiFiInfo.aBridgeName));
        CcspTraceInfo(("%s:%d sManageWiFiInfo.aWiFiInterfaces:%s\n",__FUNCTION__,__LINE__,sManageWiFiInfo.aWiFiInterfaces));
    }
}

rbusError_t notifyViaRbus(char * pTr181Param, char *pTr181ParamVal)
{
    int ret = RBUS_ERROR_BUS_ERROR ;
    rbusEvent_t event;
    rbusObject_t data;
    rbusValue_t value;

    rbusValue_Init(&value);
    rbusValue_SetString(value, pTr181ParamVal);

    rbusObject_Init(&data, NULL);
    rbusObject_SetValue(data, pTr181Param, value);

    event.name = pTr181Param;
    event.data = data;
    event.type = RBUS_EVENT_GENERAL;

    if (NULL == handle)
    {
        CcspTraceError(("%s: Rbus handler is NULL\n", __FUNCTION__));
        return ret;
    }
    /* Process the event publish*/
    ret = rbusEvent_Publish(handle, &event);
    if(ret != RBUS_ERROR_SUCCESS)
    {
        if (ret == RBUS_ERROR_NOSUBSCRIBERS)
        {
            ret = RBUS_ERROR_SUCCESS;
            CcspTraceError(("%s: No subscribers found\n", __FUNCTION__));
        }
        else
        {
            CcspTraceError(("Unable to Publish event data %s  rbus error code : %d\n",pTr181Param, ret));
        }
    }
    else
    {
        CcspTraceInfo(("%s : Publish to %s ret value is %d\n", __FUNCTION__,pTr181Param,ret));
    }
    /* release rbus value and object variable */
    rbusValue_Release(value);
    return ret;
}

int readLanConfigFromPSM(backupLanconfig_t *pBackupLanConfig)
{
    char aParamName[BUFF_LEN_64] = {'\0'};
    char aParamVal [BUFF_LEN_32] = {'\0'};
    if (NULL == pBackupLanConfig)
        return -1;

    snprintf(aParamName,BUFF_LEN_64,l2netBridgeEnable, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName,aParamVal,BUFF_LEN_32))
        return -1;
    /*CID: 347105 - Array compared against null - fixed*/
    if ( '\0' != aParamVal[0] )
    {
        if (!strncmp ("true",aParamVal, 4))
            pBackupLanConfig->bMwEnable = true;
        if (!strncmp ("false",aParamVal, 4))
            pBackupLanConfig->bMwEnable = false;
        CcspTraceInfo(("%s:%d, pBackupLanConfig->bMwEnable:%d\n",__FUNCTION__,__LINE__,pBackupLanConfig->bMwEnable));
    }
    snprintf(aParamName, BUFF_LEN_64, l2netAlias, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName, pBackupLanConfig->aAlias, sizeof(pBackupLanConfig->aAlias)))
        return -1;

    snprintf(aParamName, BUFF_LEN_64, l3netV4Addr, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName, pBackupLanConfig->aLanIpAddr, sizeof(pBackupLanConfig->aLanIpAddr)))
        return -1;

    snprintf(aParamName, BUFF_LEN_64, l2netV4SubnetMask, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName, pBackupLanConfig->aLanSubnetMask, sizeof(pBackupLanConfig->aLanSubnetMask)))
        return -1;

    snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolEnable, sManageWiFiInfo.aBridgeIndex);
    memset(aParamVal, 0, BUFF_LEN_32);
    if (0 != psmGet(aParamName, aParamVal, BUFF_LEN_32))
        return -1;

    if ('\0' != aParamVal)
    {
        if (!strncmp ("true",aParamVal, 4))
            pBackupLanConfig->bDhcpServerEnable= true;
        if (!strncmp ("false",aParamVal, 4))
            pBackupLanConfig->bDhcpServerEnable= false;
        CcspTraceInfo(("%s:%d, pBackupLanConfig->bDhcpServerEnable:%d\n",__FUNCTION__,__LINE__,pBackupLanConfig->bDhcpServerEnable));
    }

    snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolMinAddr, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName, pBackupLanConfig->aDhcpStartIpAdd, sizeof(pBackupLanConfig->aDhcpStartIpAdd)))
        return -1;

    snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolMaxAddr, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName, pBackupLanConfig->aDhcpEndIpAdd, sizeof(pBackupLanConfig->aDhcpEndIpAdd)))
        return -1;

    snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolLeaseTime, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName, pBackupLanConfig->aLeaseTime, sizeof(pBackupLanConfig->aLeaseTime)))
        return -1;

    snprintf(aParamName, BUFF_LEN_64, l3netIPv6Enable, sManageWiFiInfo.aBridgeIndex);
    memset(aParamVal, 0, BUFF_LEN_32);
    if (0 != psmGet(aParamName, aParamVal, BUFF_LEN_32))
        return -1;

    /* CID 347105 Array compared against 0 : fix */
    if (aParamVal[0] != '\0')
    {
        if (!strncmp ("true",aParamVal, 4))
            pBackupLanConfig->bIpv6Enable= true;
        if (!strncmp ("false",aParamVal, 4))
            pBackupLanConfig->bIpv6Enable= false;
        CcspTraceInfo(("%s:%d, pBackupLanConfig->bIpv6Enable:%d\n",__FUNCTION__,__LINE__,pBackupLanConfig->bIpv6Enable));
    }

    snprintf(aParamName, BUFF_LEN_64,l2netWiFiMembers, sManageWiFiInfo.aBridgeIndex);
    if (0 != psmGet(aParamName, pBackupLanConfig->aWiFiInterfaces, sizeof(pBackupLanConfig->aWiFiInterfaces)))
        return -1;

    if ('\0' != pBackupLanConfig->aWiFiInterfaces[0])
        strncpy (sManageWiFiInfo.aWiFiInterfaces,pBackupLanConfig->aWiFiInterfaces, (sizeof(sManageWiFiInfo.aWiFiInterfaces)-1));
    return 0;
}

void initManageWiFiBacupStruct(void)
{
    char aManageWiFiEnabled[BUFF_LEN_8] = {0};

    syscfg_get(NULL, "Manage_WiFi_Enabled", aManageWiFiEnabled, BUFF_LEN_8);

    getBridgeDetailsFromPsm();
    if ((!strncmp(aManageWiFiEnabled, "true", 4)) || (!strncmp(aManageWiFiEnabled, "false", 4)))
    {
        if (0 != readLanConfigFromPSM(&sBackupLanConfig))
            CcspTraceError(("%s:%d, Failed to read Manage Wifi Config from PSM\n",__FUNCTION__,__LINE__));
    }
}

/* Callback function to rollback when Lan blob execution fails */
int rollbackLanconfig(void)
{
    pErr pErrRetVal = NULL;

    int retVal = 0;
    /*
     * 0th bit : Manage Wifi Enable
     * 1st bit : Lan IP addr
     * 2nd bit : lan subnet mask, dhcp V4 pool subnet mask
     * 3rd bit : DhcpV4Pool Enable, Start and End address, lease time
     * 4th bit : IPv6 Enable
     * 5th bit : alias
     */
    char cFlag = 0;

    pErrRetVal = (pErr) malloc (sizeof(Err));
    if (NULL == pErrRetVal)
    {
        CcspTraceError(("%s,%d : malloc failed\n",__FUNCTION__,__LINE__));
        return -1;
    }
    memset(pErrRetVal,0,sizeof(Err));
    pErrRetVal->ErrorCode = BLOB_EXEC_SUCCESS;

    CcspTraceInfo((" Entering %s \n",__FUNCTION__));
    if (sCurrentLanConfig.bMwEnable != sBackupLanConfig.bMwEnable)
        cFlag |= (1 << 0);

    if (strcmp(sCurrentLanConfig.aLanIpAddr, sBackupLanConfig.aLanIpAddr))
        cFlag |= (1 << 1);

    if (strcmp(sCurrentLanConfig.aLanSubnetMask, sBackupLanConfig.aLanSubnetMask))
        cFlag |= (1 << 2);

    if (sCurrentLanConfig.bDhcpServerEnable != sBackupLanConfig.bDhcpServerEnable)
        cFlag |= (1 << 3);

    if (strcmp(sCurrentLanConfig.aDhcpStartIpAdd, sBackupLanConfig.aDhcpStartIpAdd))
        cFlag |= (1 << 3);

    if (strcmp(sCurrentLanConfig.aDhcpEndIpAdd, sBackupLanConfig.aDhcpEndIpAdd))
        cFlag |= (1 << 3);

    if (strcmp(sCurrentLanConfig.aLeaseTime, sBackupLanConfig.aLeaseTime))
        cFlag |= (1 << 3);

    if (sCurrentLanConfig.bIpv6Enable != sBackupLanConfig.bIpv6Enable)
        cFlag |= (1 << 4);

    if (strcmp(sCurrentLanConfig.aAlias, sBackupLanConfig.aAlias))
        cFlag |= (1 << 5);

    processManageWifiData (&sBackupLanConfig, cFlag, pErrRetVal);
    restorePreviousPsmValue();
    if (NULL != pErrRetVal)
    {
        retVal = pErrRetVal->ErrorCode;
        free(pErrRetVal);
    }
    return retVal;
}

static int remSubstr(char *pMainStr, char *pSubstr)
{
    char *pStr1 = NULL,*pStr2 = NULL;
    int iLen = 0;

    if ((NULL == pMainStr) || (NULL == pSubstr))
        return -1;

    pStr1 = strstr(pMainStr,pSubstr);
    if(pStr1 != NULL)
    {
        iLen = strlen(pSubstr);
        pStr2 = pStr1 + iLen;
        while((*pStr1++ = *pStr2++));
    }
    else
        return -1;
    return 0;
}

int removeSubstring(char * pMainString, char * pSubstring, bool bExtraSpaceRemoval)
{
    char *pSubstr = NULL;
    int iRet = 0;
    if ((NULL == pMainString) || (NULL == pSubstring))
        return -1;

    if (true == bExtraSpaceRemoval)
    {
        int iLenOfSubstr = (strlen(pSubstring) + 2);
        pSubstr = (char*) malloc (iLenOfSubstr);
        if (NULL != pSubstr)
        {
            snprintf(pSubstr, iLenOfSubstr, "%s ",pSubstring);
            if ( 0 != remSubstr(pMainString, pSubstr))
            {
                memset(pSubstr, 0, iLenOfSubstr);
                snprintf(pSubstr, iLenOfSubstr, " %s",pSubstring);
                if ( 0 != remSubstr(pMainString, pSubstr))
                    iRet = -1;
                else
                    iRet = 0;
            }
            free(pSubstr);
        }
    }
    else
        iRet = remSubstr(pMainString, pSubstring);

    if (0 == iRet)
        printf("%s:%d, Substring found in MainString and deleted:%s\n",__FUNCTION__,__LINE__,pMainString);
    else
        printf("%s:%d, Substring:%s is not found in MainString:%s\n",__FUNCTION__,__LINE__,pSubstring,pMainString);
    return iRet;
}

void updateBackupConfig(void)
{
    char aParamName[BUFF_LEN_64] = {'\0'};

    sBackupLanConfig.bMwEnable = sCurrentLanConfig.bMwEnable;
    strncpy (sBackupLanConfig.aAlias, sCurrentLanConfig.aAlias, (sizeof(sBackupLanConfig.aAlias)-1));
    sBackupLanConfig.bDhcpServerEnable = sCurrentLanConfig.bDhcpServerEnable;
    strncpy (sBackupLanConfig.aLanIpAddr,sCurrentLanConfig.aLanIpAddr, (sizeof(sBackupLanConfig.aLanIpAddr)-1));
    strncpy (sBackupLanConfig.aLanSubnetMask,sCurrentLanConfig.aLanSubnetMask, (sizeof(sBackupLanConfig.aLanSubnetMask)-1));
    strncpy (sBackupLanConfig.aDhcpStartIpAdd,sCurrentLanConfig.aDhcpStartIpAdd, (sizeof(sBackupLanConfig.aDhcpStartIpAdd)-1));
    strncpy (sBackupLanConfig.aDhcpEndIpAdd,sCurrentLanConfig.aDhcpEndIpAdd, (sizeof(sBackupLanConfig.aDhcpEndIpAdd)-1));
    strncpy (sBackupLanConfig.aLeaseTime,sCurrentLanConfig.aLeaseTime, (sizeof(sBackupLanConfig.aLeaseTime)-1));
    sBackupLanConfig.bIpv6Enable = sCurrentLanConfig.bIpv6Enable;
    if (false == sCurrentLanConfig.bMwEnable)
    {
        snprintf(aParamName, BUFF_LEN_64, l2netBridgeEnable, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, "false");
        snprintf(aParamName, BUFF_LEN_64, l2netAlias, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName,sBackupLanConfig.aAlias);
        snprintf(aParamName, BUFF_LEN_64, l3netV4Addr, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLanIpAddr);
        snprintf(aParamName, BUFF_LEN_64, l2netV4SubnetMask, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLanSubnetMask);
        snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolSubnetMask, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLanSubnetMask);
        snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolEnable, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, "false");
        snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolMinAddr,sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aDhcpStartIpAdd);
        snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolMaxAddr,sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aDhcpEndIpAdd);
        snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolLeaseTime,sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLeaseTime);
        snprintf(aParamName, BUFF_LEN_64, l3netIPv6Enable, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, "false");
    }
}

void getManageWiFiEnable(BOOL * pWiFiEnable)
{
    if (NULL == pWiFiEnable)
        return;
    *pWiFiEnable = sBackupLanConfig.bMwEnable;
    CcspTraceInfo(("%s:%d, sBackupLanConfig.bMwEnable:%d\n",__FUNCTION__,__LINE__,sBackupLanConfig.bMwEnable));
    CcspTraceInfo(("%s:%d, *pWiFiEnable:%d\n",__FUNCTION__,__LINE__,*pWiFiEnable));
}

void publishEvent(char * pTr181Param, char *pTr181ParamVal, EventType eEventType)
{
    if ((NULL == pTr181Param) && (NULL == pTr181ParamVal))
        return;

    switch(eEventType)
    {
        case RBUS:
        {
            if (RBUS_ERROR_SUCCESS != notifyViaRbus(pTr181Param,pTr181ParamVal))
            {
                CcspTraceError(("%s:%d, Failed to publish the %s:%s via R-BUS\n",__FUNCTION__,__LINE__,pTr181Param, pTr181ParamVal));
            }
            break;
        }
        default:
            break;
    }
}

void restorePreviousPsmValue(void)
{
    char aParamName[BUFF_LEN_64]  = {0};
    char aParamVal[BUFF_LEN_64] = {0};

    if (iRestoreFlag & (1 << 1))
    {
        if (true == sBackupLanConfig.bMwEnable)
            snprintf(aParamVal, BUFF_LEN_64,"true");
        else if (false == sBackupLanConfig.bMwEnable)
            snprintf(aParamVal, BUFF_LEN_64,"false");
        snprintf(aParamName, BUFF_LEN_64, l2netBridgeEnable, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, aParamVal);
    }
    if (iRestoreFlag & (1 << 2))
    {
        snprintf(aParamName, BUFF_LEN_64, l3netV4Addr, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLanIpAddr);
    }

    if (iRestoreFlag & ( 1 << 3))
    {
        snprintf(aParamName, BUFF_LEN_64, l2netV4SubnetMask, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLanSubnetMask);
    }
    if (iRestoreFlag & ( 1 << 4))
    {
        snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolSubnetMask, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLanSubnetMask);
    }
    if (iRestoreFlag & ( 1 << 5))
    {
        snprintf(aParamName, BUFF_LEN_64, l3netDhcpV4PoolEnable, sManageWiFiInfo.aBridgeIndex);
        if (true == sBackupLanConfig.bDhcpServerEnable)
            snprintf(aParamVal, BUFF_LEN_64,"true");
        else if (false == sBackupLanConfig.bDhcpServerEnable)
            snprintf(aParamVal, BUFF_LEN_64,"false");
        psmSet(aParamName, aParamVal);
    }
    if (iRestoreFlag & ( 1 << 6))
    {
        snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolMinAddr,sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aDhcpStartIpAdd);
    }
    if (iRestoreFlag & ( 1 << 7))
    {
        snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolMaxAddr,sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aDhcpEndIpAdd);
    }
    if (iRestoreFlag & ( 1 << 8))
    {
        snprintf(aParamName,BUFF_LEN_64, l3netDhcpV4PoolLeaseTime,sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aLeaseTime);
    }
    if (iRestoreFlag & ( 1 << 9))
    {
        snprintf(aParamName, BUFF_LEN_64, l3netIPv6Enable, sManageWiFiInfo.aBridgeIndex);
        if (true == sBackupLanConfig.bIpv6Enable)
            snprintf(aParamVal, BUFF_LEN_64,"true");
        else if (false == sBackupLanConfig.bIpv6Enable)
            snprintf(aParamVal, BUFF_LEN_64,"false");
        psmSet(aParamName, aParamVal);
    }
    if (iRestoreFlag & ( 1 << 10))
    {
        snprintf(aParamName, BUFF_LEN_64, l2netAlias, sManageWiFiInfo.aBridgeIndex);
        psmSet(aParamName, sBackupLanConfig.aAlias);
    }
}

void readLanDetailsFromSyscfg(LanDetails_t *pLanDetails)
{
    char aBuf[BUFF_LEN_64] = {'\0'};
    int iLenOfBuf = 0;
    if (NULL == pLanDetails)
        return;

    memset(aBuf, 0, sizeof(aBuf));
    if (PRIVATE == pLanDetails->eInterfaceType)
    {
        syscfg_get( NULL, "lan_ipaddr", aBuf, sizeof(aBuf));
        iLenOfBuf = sizeof(pLanDetails->aIpAddr);
        if (('\0' != aBuf[0]) && (1 < iLenOfBuf))
        {
            strncpy(pLanDetails->aIpAddr, aBuf, iLenOfBuf-1);
        }
        memset(aBuf, 0, sizeof(aBuf));
        syscfg_get( NULL, "dhcp_start", aBuf, sizeof(aBuf));
        iLenOfBuf = sizeof(pLanDetails->aStartIpAddr);
        if (('\0' != aBuf[0]) && (1 < iLenOfBuf))
        {
            strncpy(pLanDetails->aStartIpAddr, aBuf, iLenOfBuf-1);
        }
        memset(aBuf, 0, sizeof(aBuf));
        syscfg_get( NULL, "dhcp_end", aBuf, sizeof(aBuf));
        iLenOfBuf = sizeof(pLanDetails->aEndIpAddr);
        if (('\0' != aBuf[0]) && (1 < iLenOfBuf))
        {
            strncpy(pLanDetails->aEndIpAddr, aBuf, iLenOfBuf-1);
        }
    }
}

int isIpInRange(const char *pIpStr, const char *pRangeStartStr, const char *pRangeEndStr)
{
    struct in_addr sIp, sRangeStart, sRangeEnd;

    if ((NULL == pIpStr) || (NULL == pRangeEndStr) || (NULL == pRangeStartStr))
        return -1;

    if (1 != inet_pton(AF_INET, pIpStr, &sIp))
        return -1;
    if (1 != inet_pton(AF_INET, pRangeStartStr, &sRangeStart))
        return -1;
    if (1 != inet_pton(AF_INET, pRangeEndStr, &sRangeEnd))
        return -1;

    uint32_t ip_val = ntohl(sIp.s_addr);
    uint32_t start_val = ntohl(sRangeStart.s_addr);
    uint32_t end_val = ntohl(sRangeEnd.s_addr);

    if (start_val <= end_val)
    {
        return (ip_val >= start_val && ip_val <= end_val);
    }
    else
        return -1;
}

int validateIpRange(char *pIpAddr, char *pStartAddr, char *pEndAddr, pErr pErrRetVal)
{
    LanDetails_t sLanDetails;

    if ((NULL == pIpAddr) || (NULL == pStartAddr) || (NULL == pEndAddr) || (NULL == pErrRetVal))
        return -1;


    if(!strncmp(pIpAddr, RG_CM_COMMUNICATION_BEGIN, strlen(RG_CM_COMMUNICATION_BEGIN)))
    {
        CcspTraceError(("%s:%d, bridge ip begins with 172.31, which is used for RG to CM communication\n",__FUNCTION__, __LINE__));
        snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"Invalid IP for manage Wifi,begins with 172.31.X.X which is used for RG to CM communication\n");
        pErrRetVal->ErrorCode = VALIDATION_FALIED;
        return -1;
    }

    memset(&sLanDetails, 0, sizeof(sLanDetails));
    sLanDetails.eInterfaceType = PRIVATE;

    readLanDetailsFromSyscfg(&sLanDetails);

    CcspTraceInfo(("%s:%d, sLanDetails.aIpAddr:%s\n",__FUNCTION__, __LINE__, sLanDetails.aIpAddr));
    CcspTraceInfo(("%s:%d, sLanDetails.aStartIpAddr:%s\n",__FUNCTION__, __LINE__, sLanDetails.aStartIpAddr));
    CcspTraceInfo(("%s:%d, sLanDetails.aEndIpAddr:%s\n",__FUNCTION__, __LINE__, sLanDetails.aEndIpAddr));
    if (('\0' != sLanDetails.aStartIpAddr[0]) && ('\0' != sLanDetails.aEndIpAddr[0]))
    {
        if ((NULL != pIpAddr) && ('\0' != sLanDetails.aIpAddr[0]) && (strcmp(pIpAddr, sBackupLanConfig.aLanIpAddr)))
        {
            CcspTraceInfo(("%s:%d, pIpAddr:%s, sLanDetails.aIpAddr:%s\n",__FUNCTION__, __LINE__, pIpAddr, sLanDetails.aIpAddr));
            if(!strncmp(pIpAddr, sLanDetails.aIpAddr, strlen(pIpAddr)))
            {
                CcspTraceError(("%s:%d, bridge ip is same as private lan IP\n",__FUNCTION__, __LINE__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"Invalid IP for manage Wifi, it is same as private Lan IP\n");
                pErrRetVal->ErrorCode = INVALID_IP;
                return -1;
            }
            if (0 != isIpInRange(pIpAddr,sLanDetails.aStartIpAddr,sLanDetails.aEndIpAddr))
            {
                CcspTraceError(("%s:%d, Bridge IP is in the range of private lan IP \n",__FUNCTION__, __LINE__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"ManageWiFi bridge IP is in the range of private Lan IP\n");
                pErrRetVal->ErrorCode = INVALID_IP;
                return -1;
            }
        }
        if ((NULL != pStartAddr) && (strcmp(pStartAddr, sBackupLanConfig.aDhcpStartIpAdd)))
        {
            CcspTraceInfo(("%s:%d, pStartAddr:%s\n",__FUNCTION__, __LINE__, pStartAddr));
            if (0 != isIpInRange(pStartAddr,sLanDetails.aStartIpAddr,sLanDetails.aEndIpAddr))
            {
                CcspTraceError(("%s:%d, Manage Wifi dhcp Start Addr is in the range of private lan IP \n",__FUNCTION__, __LINE__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"ManageWiFi dhcp StartAddr is in the range of private Lan IP\n");
                pErrRetVal->ErrorCode = LAN_PARAM_IP_RANGE_INVALID;
                return -1;
            }
        }
        if ((NULL != pEndAddr) && (strcmp(pEndAddr, sBackupLanConfig.aDhcpEndIpAdd)))
        {
            CcspTraceInfo(("%s:%d, pEndAddr:%s\n",__FUNCTION__, __LINE__, pEndAddr));
            if (0 != isIpInRange(pEndAddr,sLanDetails.aStartIpAddr,sLanDetails.aEndIpAddr))
            {
                CcspTraceError(("%s:%d, Manage Wifi dhcp EndAddr is in the range of private lan IP \n",__FUNCTION__, __LINE__));
                snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"ManageWiFi dhcp EndAddr is in the range of private Lan IP\n");
                pErrRetVal->ErrorCode = LAN_PARAM_IP_RANGE_INVALID;
                return -1;
            }
        }
    }
    return 0;
}

int validateLeaseTime(char *pLeaseTime, pErr pErrRetVal)
{
    if ((NULL == pLeaseTime) || (NULL == pErrRetVal))
        return -1;

    char *pStr = pLeaseTime;

    if ('-' == *pStr)
    {
        CcspTraceError(("%s:%d, Lease Time is in negative \n",__FUNCTION__, __LINE__));
        goto Error;
    }
    else if ('0' <= *pStr && '9' >= *pStr)
    {
        int iValid = false;

        if ('0' == *pStr)
        {
            pStr++;
            if ('\0' == *pStr)
                goto Error;
            else if (('a' <= *pStr && 'z' >= *pStr) || ('A' <= *pStr && 'Z' >= *pStr))
                goto Error;
        }
        while (('\0' != *pStr) && ('0' <= *pStr && '9' >= *pStr))
            pStr++;
        if ('\0' != *pStr)
        {
            switch(*pStr)
            {
                case 'D':
                case 'd':
                {
                    iValid = true;
                    CcspTraceInfo(("%s:%d, Days\n",__FUNCTION__, __LINE__));
                    break;
                }
                case 'W':
                case 'w':
                {
                    iValid = true;
                    CcspTraceInfo(("%s:%d, Weeks \n",__FUNCTION__, __LINE__));
                    break;
                }
                case 'H':
                case 'h':
                {
                    iValid = true;
                    CcspTraceInfo(("%s:%d, Hours \n",__FUNCTION__, __LINE__));
                    break;
                }
                case 'M':
                case 'm':
                {
                    int iVal = 0;
                    iValid = true;
                    extractLeaseTime(pLeaseTime, pStr, &iVal);
                    CcspTraceInfo(("%s:%d, %d minutes\n",__FUNCTION__, __LINE__, iVal));
                    /* Lease time is less then 2 minutes is invalid*/
                    if (iVal < 2)
                    {
                        CcspTraceError(("%s:%d, lease time :%d minutes\n",__FUNCTION__, __LINE__, iVal));
                        goto Error;
                    }
                    break;
                }
                case 'S':
                case 's':
                {
                    int iVal = 0;
                    iValid = true;
                    extractLeaseTime(pLeaseTime, pStr, &iVal);
                    CcspTraceInfo(("%s:%d, %d Seconds \n",__FUNCTION__, __LINE__, iVal));
                    /* Lease time is less then 120 seconds is invalid*/
                    if (iVal < 120)
                    {
                        CcspTraceError(("%s:%d, Lease Time :%d seconds\n",__FUNCTION__, __LINE__, iVal));
                        goto Error;
                    }
                    break;
                }
            }
            if ((true == iValid) && ('\0' == *(pStr+1)))
            {
                CcspTraceInfo(("%s:%d, Valid lease Time\n",__FUNCTION__, __LINE__));
                return 0;
            }
            else
            {
                goto Error;
            }
        }
        else
        {
            /* Lease time is less then 120 seconds is invalid*/
            int iVal = atoi(pLeaseTime);
            if (iVal < 120)
            {
                CcspTraceError(("%s:%d, Lease Time :%d seconds\n",__FUNCTION__, __LINE__, iVal));
                goto Error;
            }
        }
    }
    else
    {
        goto Error;
    }

    return 0;

Error:
    CcspTraceError(("%s:%d, Invalid lease time\n",__FUNCTION__, __LINE__));
    snprintf(pErrRetVal->ErrorMsg, BUFF_LEN_128,"Invalid lease time\n");
    pErrRetVal->ErrorCode = VALIDATION_FALIED;
    return -1;
}

void extractLeaseTime(char *pBegin, char *pEnd, int * pLeaseTime)
{
    if((NULL == pBegin) || (NULL == pEnd) || (NULL == pLeaseTime))
        return;

    int iStrLen = pEnd - pBegin;
    iStrLen += 1;
    char *pValue = (char*) malloc (iStrLen);
    if (NULL != pValue)
    {
        memset(pValue, '\0', iStrLen);
        strncpy(pValue, pBegin, iStrLen);
        *pLeaseTime = atoi(pValue);
        free(pValue);
        pValue = NULL;
    }
}

void getManageWiFiAddrRange(LanDetails_t * pManageWiFiAddrDetails)
{
    char aManageWiFiEnabled[BUFF_LEN_8] = {0};

    if (NULL == pManageWiFiAddrDetails)
    {
        CcspTraceError(("%s:%d, NULL parameter passed\n",__FUNCTION__,__LINE__));
        return;
    }
    syscfg_get(NULL, "Manage_WiFi_Enabled", aManageWiFiEnabled, BUFF_LEN_8);

    if ((!strncmp(aManageWiFiEnabled, "true", 4)) && (MANAGE_WIFI == pManageWiFiAddrDetails->eInterfaceType))
    {
        strncpy(pManageWiFiAddrDetails->aIpAddr, sBackupLanConfig.aLanIpAddr, sizeof(pManageWiFiAddrDetails->aIpAddr)-1);
        strncpy(pManageWiFiAddrDetails->aStartIpAddr, sBackupLanConfig.aDhcpStartIpAdd, sizeof(pManageWiFiAddrDetails->aStartIpAddr)-1);
        strncpy(pManageWiFiAddrDetails->aEndIpAddr, sBackupLanConfig.aDhcpEndIpAdd, sizeof(pManageWiFiAddrDetails->aEndIpAddr)-1);
    }
}
