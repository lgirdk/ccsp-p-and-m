#include <stdio.h>
#include <syscfg/syscfg.h>
#include "ccsp_trace.h"
#include "speedboost_apis.h"
#include "speedboost_scheduler_apis.h"
#include "speedboost_rbus_handler.h"
#include "cosa_drg_common.h"

pthread_mutex_t pvd_lock = PTHREAD_MUTEX_INITIALIZER;

static bool is_pvd_thread_running = false ;
//function to create a thread which runs only if there is any change in the TR181 parameters
//if there is a change in the TR181 parameters, it will run the sysevent set zebra-restart command

SpeedBoost sSpeedBoost;
SpeedBoostType boostType;


void trigger_ra_service_restart() 
{
        pthread_mutex_lock(&pvd_lock);
        if(is_pvd_thread_running)
        {
            CcspTraceInfo(("%s :The thread is already running\n",__FUNCTION__));
            pthread_mutex_unlock(&pvd_lock);
            return;
        }
        else
        {
            pthread_t thread_id;
            is_pvd_thread_running = true;
            pthread_create(&thread_id, NULL, monitor_thread_for_ra_restart, NULL);
        }
        pthread_mutex_unlock(&pvd_lock);
        return;
}

// if there is any change in the pvd tr181 run the zebra-restart command
void* monitor_thread_for_ra_restart(void* arg) 
{
    (void)arg;
    pthread_detach(pthread_self());
    sleep(WAIT_TIME_TO_TRIGGER_RA_RESTART);
    CcspTraceInfo(("%s The PVD parameters have changed, running the command\n",__FUNCTION__));
    if (0 != commonSyseventSet("zebra-restart", ""))
    {
        CcspTraceError(("%s :Failed to set zebra restart sysevent\n",__FUNCTION__));
    }    
    pthread_mutex_lock(&pvd_lock);
    is_pvd_thread_running = false;
    pthread_mutex_unlock(&pvd_lock);
    return NULL;
}

/************************************************************************************

    Get handler for pvd Bool

*************************************************************************************/
bool compareBuffer(const char* buf, bool* result) {
    if ( (strcmp(buf, "0") == 0 ) || ( strcmp(buf, "false") == 0 ) )
    {
        *result = false;
    } 
    else if ( (strcmp(buf, "1") == 0)  || ( strcmp(buf, "true") == 0 ) )
    {
        *result = true;
    } 
    return true;
}

void initializeSpeedBoostStructVal()
{
    char buf[256] = {0};
    bool result =false;

    char protoV4[256] = {0}, protoV6[256] = {0}, startPortV4[256] = {0}, startPortV6[256] = {0}, endPortV4[256] = {0}, endPortV6[256] = {0};
    char startPortNormalV4[254] = {0}, endPortNormalV4[256] = {0}, startPortNormalV6[256] = {0}, endPortNormalV6[256] = {0};
    memset(buf, 0, sizeof(buf));

    if(syscfg_get(NULL, "Advertisement_pvd_enable", buf, sizeof(buf)) == 0)
    {
        compareBuffer(buf, &result);
        sSpeedBoost.pvd_enabled = result;
    }
    memset(buf, 0, sizeof(buf));
    if(syscfg_get(NULL, "Advertisement_pvd_hflag", buf, sizeof(buf)) == 0)
    {
        compareBuffer(buf, &result);
        sSpeedBoost.pvd_h_flag = result;
    }
    memset(buf, 0, sizeof(buf));
    if(syscfg_get(NULL, "Advertisement_pvd_delay", buf, sizeof(buf)) == 0)
    {
        sSpeedBoost.pvd_delay = atoi(buf);
    }
    memset(buf, 0, sizeof(buf));

    if(syscfg_get(NULL, "Advertisement_pvd_seqNum", buf, sizeof(buf)) == 0)
    {
        sSpeedBoost.pvd_seqNum = atoi(buf);
    }
    memset(buf, 0, sizeof(buf));
    if(syscfg_get(NULL, "Advertisement_pvd_fqdn", buf, sizeof(buf)) == 0)
    {
        strncpy(sSpeedBoost.pvd_fqdn, buf, sizeof(sSpeedBoost.pvd_fqdn));
    }
    memset(buf, 0, sizeof(buf));
    if (syscfg_get(NULL, SYSCFG_SPEEDBOOST_NUM_OF_CONFIG_CLINETS, buf, sizeof(buf)) == 0)
    {
        sSpeedBoost.sClientInfoSpeedBoost.iNumOfConfigOfClients = atoi(buf);
    }
    memset(buf, 0, sizeof(buf));
    if (syscfg_get(NULL, SYSCFG_SPEEDBOOST_CLIENT_COUNT, buf, sizeof(buf)) == 0)
    {
        sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients = atoi(buf);
    }
    memset(buf, 0, sizeof(buf));
    for (int iCount = 1; iCount <= sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients; iCount++)
    {
        char syscfgName[64] = {0};
        snprintf(syscfgName, sizeof(syscfgName), "%s%d", SYSCFG_SPEEDBOOST_CLIENT_MAC_PREFIX, iCount);
        if (syscfg_get(NULL, syscfgName, buf, sizeof(buf)) == 0)
        {
            strncpy(sSpeedBoost.sClientInfoSpeedBoost.cCurrentDeviceList[iCount - 1], buf, sizeof(sSpeedBoost.sClientInfoSpeedBoost.cCurrentDeviceList[iCount - 1]));
        }
    }
    // To initialize the speedboost values
    if(syscfg_get(NULL, "SpeedBoost_ProtoV4", protoV4, sizeof(protoV4)) == 0 &&
       syscfg_get(NULL, "SpeedBoost_ProtoV6", protoV6, sizeof(protoV6)) == 0 &&
       syscfg_get(NULL, "SpeedBoost_Port_StartV4", startPortV4, sizeof(startPortV4)) == 0 &&
       syscfg_get(NULL, "SpeedBoost_Port_StartV6", startPortV6, sizeof(startPortV6)) == 0 &&
       syscfg_get(NULL, "SpeedBoost_Port_EndV4", endPortV4, sizeof(endPortV4)) == 0 &&
       syscfg_get(NULL, "SpeedBoost_Port_EndV6", endPortV6, sizeof(endPortV6)) == 0) 
    {
        snprintf(current_speedboost_value, sizeof(current_speedboost_value),
                 "IPv4 %s %s %s,IPv6 %s %s %s",
                 protoV4, startPortV4, endPortV4, protoV6, startPortV6, endPortV6);
        CcspTraceInfo(("current_speedboost_value: %s\n", current_speedboost_value));
    }
    
    //initialise speedboost normal value
    if(syscfg_get(NULL, "SpeedBoost_ProtoV4", protoV4, sizeof(protoV4)) == 0 &&
       syscfg_get(NULL, "SpeedBoost_ProtoV6", protoV6, sizeof(protoV6)) == 0 &&
       syscfg_get(NULL, "NoSpeedBoost_Port_StartV4", startPortNormalV4, sizeof(startPortNormalV4)) == 0 &&
       syscfg_get(NULL, "NoSpeedBoost_Port_StartV6", startPortNormalV6, sizeof(startPortNormalV6)) == 0 &&
       syscfg_get(NULL, "NoSpeedBoost_Port_EndV4", endPortNormalV4, sizeof(endPortNormalV4)) == 0 &&
       syscfg_get(NULL, "NoSpeedBoost_Port_EndV6", endPortNormalV6, sizeof(endPortNormalV6)) == 0)
    {
    	snprintf(current_normal_speedboost_value, sizeof(current_normal_speedboost_value),
                 "IPv4 %s %s %s,IPv6 %s %s %s",
                 protoV4, startPortNormalV4, endPortNormalV4, protoV6, startPortNormalV6, endPortNormalV6);
    	CcspTraceInfo(("current_normal_speedboost_value: %s\n", current_normal_speedboost_value));
    }
}

int getNumOfConfiguredClients(void)
{
    CcspTraceInfo(("%s:%d, Number of configured clients: %d\n", __FUNCTION__, __LINE__, sSpeedBoost.sClientInfoSpeedBoost.iNumOfConfigOfClients));
    return sSpeedBoost.sClientInfoSpeedBoost.iNumOfConfigOfClients;
}
int getNumOfActiveClients(void)
{
    CcspTraceInfo(("%s:%d, Number of active clients: %d\n", __FUNCTION__, __LINE__, sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients));
    return sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients;
}
bool getActiveDevicesListOfSpeedBoost(char * pActiveDevicesList, int iLen)
{
    if (NULL == pActiveDevicesList)
    {
        CcspTraceError(("%s:%d, NULL parameter passed\n", __FUNCTION__, __LINE__));
        return false;
    }
    if (0 == sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients)
    {
        CcspTraceInfo(("%s:%d, No active devices\n", __FUNCTION__, __LINE__));
        return true;
    }
    if (iLen < sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients * MAC_ADDR_SIZE)
    {
        CcspTraceError(("%s:%d, Insufficient buffer\n", __FUNCTION__, __LINE__));
        return false;
    }
    for (int iCount = 0; iCount < sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients; iCount++)
    {
        //After copying mac address append the pipe symbol
        strncat(pActiveDevicesList, sSpeedBoost.sClientInfoSpeedBoost.cCurrentDeviceList[iCount], MAC_ADDR_SIZE);
        if ((iCount + 1) < sSpeedBoost.sClientInfoSpeedBoost.iNumOfActiveClients)
            strncat(pActiveDevicesList, ",", 1);
    }
    CcspTraceInfo(("%s:%d, Active devices list: %s\n", __FUNCTION__, __LINE__, pActiveDevicesList));
    return true;
}

int processSpeedBoostValue(const char* str, const char* prev_val, char* target_value, size_t target_size, SpeedBoostType boostType) 
{
    char* strCopy = strdup(str);
    strncpy(target_value, strCopy, target_size - 1);
    char *token, *subtoken, *saveptr1, *saveptr2;
    bool ipv4Valid = false;
    bool ipv6Valid = false;
    char* protocol_ipv4 = NULL;
    char* startPortStr_ipv4 = NULL;
    char* endPortStr_ipv4 = NULL;
    char* protocol_ipv6 = NULL;
    char* startPortStr_ipv6 = NULL;
    char* endPortStr_ipv6 = NULL;

    if(strchr(str, ',') == NULL) 
    {
    	CcspTraceError(("Input string does not contain a comma\n"));
    	strncpy(target_value, prev_val, target_size - 1);
	free(strCopy);
    	return 1;
    }

    for(token = strtok_r(strCopy, ",", &saveptr1); token != NULL; token = strtok_r(NULL, ",", &saveptr1))
    {
        subtoken = strtok_r(token, " ", &saveptr2);
        
        if (subtoken == NULL || (strcasecmp(subtoken, "IPv4") != 0 && strcasecmp(subtoken, "IPv6") != 0))
        {
            CcspTraceError(("IPv4 or IPV6 not found\n"));
            strncpy(target_value, prev_val, target_size - 1);
            free(strCopy);
            return 1;
        }

        char* protocol = strtok_r(NULL, " ", &saveptr2);
        if (protocol == NULL || (strcasecmp(protocol, "TCP") != 0 && strcasecmp(protocol, "UDP") != 0 && strcasecmp(protocol, "BOTH") != 0))
        {
            CcspTraceError(("TCP, UDP or BOTH not found\n"));
            strncpy(target_value, prev_val, target_size - 1);
            free(strCopy);
            return 1;
        }

        char* startPortStr = strtok_r(NULL, " ", &saveptr2);
        if (startPortStr == NULL)
        {
            CcspTraceError(("Start port not found\n"));
            strncpy(target_value, prev_val, target_size - 1);
            free(strCopy);
            return 1;
        }   
        int startPort = atoi(startPortStr);
        if (startPort < 0 || startPort > 65535)
        {
            CcspTraceError(("start port lt 0 or gt 65535\n"));
            strncpy(target_value, prev_val, target_size - 1);
            free(strCopy);
            return 1;
        }
        CcspTraceInfo(("startPortStr: %s, startPort: %d\n", startPortStr, startPort));

        char* endPortStr = strtok_r(NULL, " ", &saveptr2);
        if (endPortStr == NULL)
        {
            CcspTraceError(("End port not found\n"));
            strncpy(target_value, prev_val, target_size - 1);
            free(strCopy);
            return 1;
        }
        int endPort = atoi(endPortStr);
        if (endPort < startPort || endPort > 65535)
        {
            CcspTraceError(("End port lt start port or gt 65535\n"));
            strncpy(target_value, prev_val, target_size - 1);
            free(strCopy);
            return 1;
        }
        CcspTraceInfo(("endPortStr: %s, endPort: %d\n", endPortStr, endPort));
        
        // Update flags so that only if both ipv4 and ipv6 format is correct do syscfg_set_commit
        if (strcasecmp(subtoken, "IPv4") == 0) 
        {
            ipv4Valid = true;
            protocol_ipv4 = protocol;
            startPortStr_ipv4 = startPortStr;
            endPortStr_ipv4 = endPortStr;
        }
        else if (strcasecmp(subtoken, "IPv6") == 0) 
        {
            ipv6Valid = true;
            protocol_ipv6 = protocol;
            startPortStr_ipv6 = startPortStr;
            endPortStr_ipv6 = endPortStr;
        }
	
    }
    // Check if both IPv4 and IPv6 configurations are valid
    //outside the loop since if ipv4 format was correct and ipv6 foramt was wrong, it would still syscfg_set_commit for ipv4 which would be wrong.
    if (ipv4Valid && ipv6Valid) 
    {
      	// Perform commit actions for both IPv4 and IPv6
    	switch (boostType)
        {
            case CURRENT_SPEEDBOOST_VALUE:
	    	CcspTraceInfo(("syscfg_set_commit for current_speedboost_value\n"));
		CcspTraceInfo(("subtoken is IPv4\n"));
	        
                syscfg_set_commit(NULL, "SpeedBoost_ProtoV4", protocol_ipv4);
                syscfg_set_commit(NULL, "SpeedBoost_Port_StartV4", startPortStr_ipv4);
                syscfg_set_commit(NULL, "SpeedBoost_Port_EndV4", endPortStr_ipv4);
		
                CcspTraceInfo(("subtoken is IPv6\n"));
	        
                syscfg_set_commit(NULL, "SpeedBoost_ProtoV6", protocol_ipv6);
	        syscfg_set_commit(NULL, "SpeedBoost_Port_StartV6", startPortStr_ipv6);
	        syscfg_set_commit(NULL, "SpeedBoost_Port_EndV6", endPortStr_ipv6);
            	break;
	    case CURRENT_NORMAL_SPEEDBOOST_VALUE:
	    	CcspTraceInfo(("syscfg_set_commit for current_normal_speedboost_value\n"));
                CcspTraceInfo(("subtoken is IPv4\n"));
                
                syscfg_set_commit(NULL, "NoSpeedBoost_Port_StartV4", startPortStr_ipv4);
                syscfg_set_commit(NULL, "NoSpeedBoost_Port_EndV4", endPortStr_ipv4);
            	
                CcspTraceInfo(("subtoken is IPv6\n"));
                
                syscfg_set_commit(NULL, "NoSpeedBoost_Port_StartV6", startPortStr_ipv6);
                syscfg_set_commit(NULL, "NoSpeedBoost_Port_EndV6", endPortStr_ipv6);
            	break;
            default:
            	printf("Unknown SpeedBoostType\n");
            	break;
	}
    }
    else
    {
	CcspTraceError(("IPv4 or IPv6 port range format is not correct\n"));
	strncpy(target_value, prev_val, target_size - 1);
	free(strCopy);
	return 1;
    }
    free(strCopy);

    //restart the firewall upon successful set of the speedboost port ranges
    return (triggerFirewallRestart() == false) ? 1 : 0;
}
