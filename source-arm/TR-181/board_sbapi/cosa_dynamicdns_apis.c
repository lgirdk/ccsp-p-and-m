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

#if defined(DDNS_BROADBANDFORUM)
#include "cosa_dynamicdns_apis.h"
#include <utctx/utctx.h>
#include <utapi/utapi.h>
#include <utapi/utapi_util.h>
#include <syscfg/syscfg.h>
#include "safec_lib_common.h"

      /* MACROS */
#define  SYSCFG_SERVER_ENABLE_KEY         "ddns_server_enable_%lu"
#define  SYSCFG_SERVER_PROTOCOL_KEY       "ddns_server_protocol_%lu"
#define  SYSCFG_SERVER_CHECKINTERVAL_KEY  "ddns_server_checkinterval_%lu"
#define  SYSCFG_SERVER_RETRYINTERVAL_KEY  "ddns_server_retryinterval_%lu"
#define  SYSCFG_SERVER_MAXRETRIES_KEY     "ddns_server_maxretries_%lu"
#define  SYSCFG_SERVER_SERVERPORT_KEY     "ddns_server_serverport_%lu"
#define  SYSCFG_SERVER_SERVERADDRESS_KEY  "ddns_server_serveraddress_%lu"
#define  SYSCFG_SERVER_SERVICENAME_KEY    "ddns_server_servicename_%lu"
#define  DEFAULT_RETRYINTERVAL            660
#define  DEFAULT_MAXRETRIES               10

#define  SYSCFG_HOST_ENABLE_KEY           "ddns_host_enable_%lu"
#define  SYSCFG_HOST_STATUS_KEY           "ddns_host_status_%lu"
#define  SYSCFG_HOST_NAME_KEY             "ddns_host_name_%lu"
#define  HOST_DISABLED                    5
#define  MAX_HOST_COUNT                   1

typedef struct {
    char ServiceName[64];
    char Name[64];
    char SupportedProtocols[64];
    char Protocol[16];
    char ServerAddress[64];
} DDNS_SERVICE;

DDNS_SERVICE gDdnsServices[] =
{
/*  structure template
    {
        "ServiceName Parameter"
        "Name parameter" //(to be used by UI display)
        "SupportedProtocols"
        "Protocol" //(default)
    },
*/
    {
        "no-ip",
        "No-IP.com",
        "HTTP",
        "HTTP",
        "www.no-ip.com"
    },
    {
        "dyndns",
        "Dyn.com",
        "HTTP",
        "HTTP",
        "www.dyndns.org"
    },
    {
        "duckdns",
        "DuckDNS.org",
        "HTTPS",
        "HTTPS",
        "www.duckdns.org"
    },
    {
        "afraid",
        "FreeDNS.afraid.org",
        "HTTPS",
        "HTTPS",
        "www.freedns.afraid.org"
    },
    {
        "changeip",
        "ChangeIP.com",
        "HTTP",
        "HTTP",
        "www.changeip.com"
    }
};

/***********************************************************************
 APIs for SYSCFG GET and SET

     *UtGetString
     *UtSetString
     *UtGetUlong
     *UtSetUlong
     *UtGetBool
     *UtSetBool
***********************************************************************/

static int UtGetString(const char *path, char *pVal, ULONG length)
{
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_RawGet(&ctx, NULL, (char *)path, pVal, length);

    Utopia_Free(&ctx, 0);
    return ANSC_STATUS_SUCCESS;
}

static int UtSetString(const char *path, char *val)
{
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
       return ANSC_STATUS_FAILURE;

    if (val)
        Utopia_RawSet(&ctx, NULL, (char *)path, val);

    Utopia_Free(&ctx, 1);

    return ANSC_STATUS_SUCCESS;
}

static int UtGetUlong(const char *path, ULONG *pVal)
{
    UtopiaContext ctx;
    char buf[64] = {0};

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_RawGet(&ctx, NULL, (char *)path, buf, sizeof(buf));
    if(buf[0] != '\0')
    {
        *pVal = atoi(buf);
    }

    Utopia_Free(&ctx, 0);

    return ANSC_STATUS_SUCCESS;
}

static int UtSetUlong(const char *path, ULONG val)
{
    UtopiaContext ctx;
    char buf[64] = {0};

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    snprintf(buf, sizeof(buf), "%lu", val);
    Utopia_RawSet(&ctx, NULL, (char *)path, buf);

    Utopia_Free(&ctx, 1);

    return ANSC_STATUS_SUCCESS;
}

static int UtGetBool(const char *path, BOOLEAN *pVal)
{
    UtopiaContext ctx;
    char buf[64] = {0};

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_RawGet(&ctx, NULL, (char *)path, buf, sizeof(buf));
    if (atoi(buf) == 1)
        *pVal = TRUE;
    else
        *pVal = FALSE;

    Utopia_Free(&ctx, 0);

    return ANSC_STATUS_SUCCESS;
}

static int UtSetBool(const char *path, BOOLEAN val)
{
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
       return ANSC_STATUS_FAILURE;

    if (val)
        Utopia_RawSet(&ctx, NULL, (char *)path, "1");
    else
        Utopia_RawSet(&ctx, NULL, (char *)path, "0");

    Utopia_Free(&ctx, 1);

    return ANSC_STATUS_SUCCESS;
}

static int resetDynamicDNSStatus(void)
{
    syscfg_set(NULL, "ddns_client_Status", "1"); /* CLIENT_CONNECTING=1 */
    syscfg_set(NULL, "ddns_host_status_1", "2"); /* HOST_UPDATE_NEEDED=2 */

    return ANSC_STATUS_SUCCESS;
}


//#CLIENT Status
#define CLIENT_CONNECTING 1
#define CLIENT_AUTHENTICATING 2
#define CLIENT_UPDATED 3
#define CLIENT_ERROR_MISCONFIGURED 4
#define CLIENT_ERROR 5
#define CLIENT_DISABLED 6

//#LAST Error Status
#define NO_ERROR 1
#define MISCONFIGURATION_ERROR 2
#define DNS_ERROR 3
#define CONNECTION_ERROR 4
#define AUTHENTICATION_ERROR 5
#define TIMEOUT_ERROR 6
#define PROTOCOL_ERROR 7

//#Host Status
#define HOST_REGISTERED 1
#define HOST_UPDATE_NEEDED 2
#define HOST_UPDATING 3
#define HOST_ERROR 4
#define HOST_DISABLED 5


#define GENERAL_FILE "/tmp/ddns-general.trace"
#define OUTPUT_FILE "/var/tmp/ipupdate.out"
#define UPDATING_CHECK_FILE "/var/tmp/updating_ddns_server.txt"

//For service dyndns: searching keywords in OUTPUT_FILE /var/tmp/ez-ipupdate.out
#define DYNDNS_ERROR_CONNECTING		"error connecting"
#define DYNDNS_AUTHENTICATION_FAIL	"authentication failure"
#define DYNDNS_INVALID_HOSTNAME		"invalid hostname"
#define DYNDNS_MALFORMED_HOSTNAME	"malformed hostname"


//For other service except dyndns: when curl command return !0, search the keywords in GENERAL_FILE /tmp/ddns-general.trace
#define RESOLVE_ERRO "Couldn't resolve host"
#define CONNECTING1_ERROR "Failed to connect to"
#define CONNECTING2_ERROR "connect fail"

//For other service except dyndns: when curl command return 0 searching keywords in /var/tmp/ipupdate.'$server_servicename'
//changeip
#define REGISTER_SUCCESS_changeip "Successful Update"
#define UPDATE_SUCCESS_changeip "Successful Update"
#define HOSTNAME_ERROR_changeip "Hostname pattern does not exist"
#define USERNAME_ERROR_changeip "badauth"
#define PASSWORD_ERROR_changeip "badauth"
#define GENERAL_ERROR_changeip ""
#define TOKEN_ERROR_changeip ""

#define SERVICE_changeip_COM "changeip"

//no-ip
#define REGISTER_SUCCESS_noip "good"
#define UPDATE_SUCCESS_noip "nochg"
#define HOSTNAME_ERROR_noip "nohost"
#define USERNAME_ERROR_noip "badauth"
#define PASSWORD_ERROR_noip "badauth"
#define GENERAL_ERROR_noip ""
#define TOKEN_ERROR_noip ""

//dyndns
#define REGISTER_SUCCESS_dyndns "good"
#define UPDATE_SUCCESS_dyndns "nochg"
#define HOSTNAME_ERROR_dyndns "nohost"
#define USERNAME_ERROR_dyndns "badauth"
#define PASSWORD_ERROR_dyndns "badauth"
#define GENERAL_ERROR_dyndns ""
#define TOKEN_ERROR_dyndns ""

//duckdns
#define REGISTER_SUCCESS_duckdns "OK"
#define UPDATE_SUCCESS_duckdns ""
#define HOSTNAME_ERROR_duckdns ""
#define USERNAME_ERROR_duckdns ""
#define PASSWORD_ERROR_duckdns ""
#define GENERAL_ERROR_duckdns "KO"
#define TOKEN_ERROR_duckdns ""

//afraid,
#define REGISTER_SUCCESS_afraid "Updated"
#define UPDATE_SUCCESS_afraid "has not changed"
#define HOSTNAME_ERROR_afraid ""
#define USERNAME_ERROR_afraid ""
#define PASSWORD_ERROR_afraid ""
#define GENERAL_ERROR_afraid "Unable to locate this record"
#define TOKEN_ERROR_afraid ""

#define REGISTER_SUCCESS(x) REGISTER_SUCCESS_##x
#define UPDATE_SUCCESS(x) UPDATE_SUCCESS_##x
#define HOSTNAME_ERROR(x) HOSTNAME_ERROR_##x
#define USERNAME_ERROR(x) USERNAME_ERROR_##x
#define PASSWORD_ERROR(x) PASSWORD_ERROR_##x
#define GENERAL_ERROR(x) GENERAL_ERROR_##x
#define TOKEN_ERROR(x) TOKEN_ERROR_##x


char register_success(char *service_name, char *buf)
{
    if(0 == strcmp(service_name,"changeip")) {
        strcpy(buf,REGISTER_SUCCESS(changeip));
    } else if(0 == strcmp(service_name,"no-ip")) {
        strcpy(buf,REGISTER_SUCCESS(noip));
    } else if(0 == strcmp(service_name,"dyndns")) {
        strcpy(buf,REGISTER_SUCCESS(dyndns));
    } else if(0 == strcmp(service_name,"duckdns")) {
        strcpy(buf,REGISTER_SUCCESS(duckdns));
    } else if(0 == strcmp(service_name,"afraid")) {
        strcpy(buf,REGISTER_SUCCESS(afraid));
    }
    return buf[0];
}

char update_success(char *service_name, char *buf)
{
    if(0 == strcmp(service_name,"changeip")) {
        strcpy(buf, UPDATE_SUCCESS(changeip));
    } else if(0 == strcmp(service_name,"no-ip")) {
        strcpy(buf, UPDATE_SUCCESS(noip));
    } else if(0 == strcmp(service_name,"dyndns")) {
        strcpy(buf, UPDATE_SUCCESS(dyndns));
    } else if(0 == strcmp(service_name,"duckdns")) {
        strcpy(buf, UPDATE_SUCCESS(duckdns));
    } else if(0 == strcmp(service_name,"afraid")) {
        strcpy(buf, UPDATE_SUCCESS(afraid));
    }
    return buf[0];
}

char hostname_error(char *service_name, char *buf)
{
    if(0 == strcmp(service_name,"changeip")) {
        strcpy(buf, HOSTNAME_ERROR(changeip));
    } else if(0 == strcmp(service_name,"no-ip")) {
        strcpy(buf, HOSTNAME_ERROR(noip));
    } else if(0 == strcmp(service_name,"dyndns")) {
        strcpy(buf, HOSTNAME_ERROR(dyndns));
    } else if(0 == strcmp(service_name,"duckdns")) {
        strcpy(buf, HOSTNAME_ERROR(duckdns));
    } else if(0 == strcmp(service_name,"afraid")) {
        strcpy(buf, HOSTNAME_ERROR(afraid));
    }
    return buf[0];
}

char username_error(char *service_name, char *buf)
{
    if(0 == strcmp(service_name,"changeip")) {
        strcpy(buf, USERNAME_ERROR(changeip));
    } else if(0 == strcmp(service_name,"no-ip")) {
        strcpy(buf, USERNAME_ERROR(noip));
    } else if(0 == strcmp(service_name,"dyndns")) {
        strcpy(buf, USERNAME_ERROR(dyndns));
    } else if(0 == strcmp(service_name,"duckdns")) {
        strcpy(buf, USERNAME_ERROR(duckdns));
    } else if(0 == strcmp(service_name,"afraid")) {
        strcpy(buf, USERNAME_ERROR(afraid));
    }
    return buf[0];
}

char password_error(char *service_name, char *buf)
{
    if(0 == strcmp(service_name,"changeip")) {
        strcpy(buf, PASSWORD_ERROR(changeip));
    } else if(0 == strcmp(service_name,"no-ip")) {
        strcpy(buf, PASSWORD_ERROR(noip));
    } else if(0 == strcmp(service_name,"dyndns")) {
        strcpy(buf, PASSWORD_ERROR(dyndns));
    } else if(0 == strcmp(service_name,"duckdns")) {
        strcpy(buf, PASSWORD_ERROR(duckdns));
    } else if(0 == strcmp(service_name,"afraid")) {
        strcpy(buf, PASSWORD_ERROR(afraid));
    }
    return buf[0];
}

char general_error(char *service_name, char *buf)
{
    if(0 == strcmp(service_name,"changeip")) {
        strcpy(buf, GENERAL_ERROR(changeip));
    } else if(0 == strcmp(service_name,"no-ip")) {
        strcpy(buf, GENERAL_ERROR(noip));
    } else if(0 == strcmp(service_name,"dyndns")) {
        strcpy(buf, GENERAL_ERROR(dyndns));
    } else if(0 == strcmp(service_name,"duckdns")) {
        strcpy(buf, GENERAL_ERROR(duckdns));
    } else if(0 == strcmp(service_name,"afraid")) {
        strcpy(buf, GENERAL_ERROR(afraid));
    }
    return buf[0];
}

char token_error(char *service_name, char *buf)
{
    if(0 == strcmp(service_name,"changeip")) {
        strcpy(buf, TOKEN_ERROR(changeip));
    } else if(0 == strcmp(service_name,"no-ip")) {
        strcpy(buf, TOKEN_ERROR(noip));
    } else if(0 == strcmp(service_name,"dyndns")) {
        strcpy(buf, TOKEN_ERROR(dyndns));
    } else if(0 == strcmp(service_name,"duckdns")) {
        strcpy(buf, TOKEN_ERROR(duckdns));
    } else if(0 == strcmp(service_name,"afraid")) {
        strcpy(buf, TOKEN_ERROR(afraid));
    }
    return buf[0];
}

static char bin2hex (unsigned int a)
{
    a &= 0x0F;

    if ((a >= 0) && (a <= 9))
        return '0' + a;
    if ((a >= 10) && (a <= 15))
        return 'a' + (a - 10);
}

#define FUNC_NAME "update_ddns_server"

#define COMMAND_BUF_LENGTH 512
int  update_ddns_server(void)
{
    int ret;
    char command[COMMAND_BUF_LENGTH],buf[128];

    int dslite_enable=1;
    int dynamic_dns_enable=0;

    int client_enable=0;
    char client_username[64]={0};
    char client_password[64]={0};
    char client_server[64]={0};

    int host_enable=0;
    char host_name[64]={0};

    int server_enabled=0;
    char server_servicename[64]={0};
    int server_index;

    char wan_ipaddr[64]="0.0.0.0";

    token_t se_token;
    int     se_fd = -1;

    struct timeval tv;
    time_t t;
    struct tm *info;

    FILE *output_file;

    BOOLEAN ddns_return_status_success = FALSE;
    char client_status = 0;
    char host_status_1 = 0;
    char client_Lasterror = 0;
    char return_status[16]={0};

    printf("%s: into 0503\n",FUNC_NAME);
    sprintf(buf, "touch %s",UPDATING_CHECK_FILE);
    system(buf);
    memset(buf, 0, sizeof(buf));

    //init syscfg and connect sysevent
    if (syscfg_init() != 0) {
        printf("%s FAILED syscfg_init failed\n",FUNC_NAME);
        ddns_return_status_success = FALSE;
        goto EXIT;
    }
    se_fd = s_sysevent_connect(&se_token);
    if (se_fd < 0) {
        printf("%s: FAILED to connect sysevent\n",FUNC_NAME);
        ddns_return_status_success = FALSE;
        goto EXIT;
    }

    //set initializing status?
    //system("sysevent set ddns_return_status");
    //vsystem("sysevent set ddns_check_maxretries 0"); //TO DO
    //vsystem("sysevent set ddns_updated_time 0");
    //vsystem("sysevent set ddns_failure_time 0");

    //get and check wan ip address
    sysevent_get(se_fd, se_token, "current_wan_ipaddr", wan_ipaddr, sizeof(wan_ipaddr));
    syscfg_set(NULL, "wan_last_ipaddr",wan_ipaddr); //avoid service_check_interval from script
    if(0==strcmp(wan_ipaddr, "0.0.0.0")) {
        ddns_return_status_success = FALSE;
        client_status = CLIENT_ERROR_MISCONFIGURED;
        host_status_1 = HOST_ERROR;
        client_Lasterror = MISCONFIGURATION_ERROR;
        printf("%s: FAILED because wan_ipaddr is 0.0.0.0\n",FUNC_NAME);
        goto EXIT;
    }
    printf("%s 8: wan_ipaddr is %s\n",FUNC_NAME,wan_ipaddr);

    if(syscfg_get( NULL, "arddnsclient_1::Server", client_server, sizeof(client_server)) == 0) {
        printf("%s 1: client_server %s\n",FUNC_NAME,client_server);
    }

    if (sscanf(client_server,"Device.DynamicDNS.Server.%d",&server_index) != 1) {
        ddns_return_status_success = FALSE;
        client_status = CLIENT_DISABLED;
        host_status_1 = HOST_ERROR;
        client_Lasterror = MISCONFIGURATION_ERROR;
        printf("%s: FAILED because client_server is NULL or wrong format\n",FUNC_NAME);
        goto EXIT;
    }
    printf("%s: 2: server_index is %d\n",FUNC_NAME,server_index);

    //get and check if dslite, dynamic_dns, client, server, host are enabled.
    //get and check if server is set to client.
    if(syscfg_get( NULL, "dslite_enable", command, sizeof(command)) == 0) {
        dslite_enable = atol(command);
    }
    else {
        dslite_enable = 0;
    }
    printf("%s: 3: dslite_enable %d\n",FUNC_NAME,dslite_enable);

    if(syscfg_get( NULL, "dynamic_dns_enable", command, sizeof(command)) == 0) {
        dynamic_dns_enable = atol(command);
        printf("%s 4: dynamic_dns_enable %d\n",FUNC_NAME,dynamic_dns_enable);
    }
    if(syscfg_get( NULL, "arddnsclient_1::enable", command, sizeof(command)) == 0) {
        client_enable = atol(command);
        printf("%s 5: client_enable %d\n",FUNC_NAME,client_enable);
    }
    if(syscfg_get( NULL, "ddns_host_enable_1", buf, sizeof(buf)) == 0) {
        host_enable = atol(buf);
        printf("%s 6: host_enable %d\n",FUNC_NAME,host_enable);
    }
    sprintf(command,"ddns_server_enable_%d",server_index);
    if(syscfg_get( NULL, command, buf, sizeof(buf)) == 0) {
        server_enabled = atol(buf);
        printf("%s 7: server_enabled %d\n",FUNC_NAME,server_enabled);
    }

    if((dslite_enable == 1) || (dynamic_dns_enable == 0) || (client_enable == 0) || (host_enable == 0) || (server_enabled == 0)) {
        printf("%s: FAILED because dslite_enable is %d or dynamic_dns_enable is %d or server_enabled %d or host_enable %d\n",FUNC_NAME,dslite_enable,dynamic_dns_enable,server_enabled,host_enable);
        ddns_return_status_success = FALSE;
        client_status = CLIENT_DISABLED;
        host_status_1 = HOST_ERROR;
        client_Lasterror = MISCONFIGURATION_ERROR;
        goto EXIT;
    }

    //get and check Server.Name
    sprintf(command,"ddns_server_servicename_%d",server_index);
    if(syscfg_get( NULL, command, server_servicename, sizeof(server_servicename)) == 0) { //1:no-ip 2:dyndns 3:duckdns 4:afraid 5:changeip
        printf("%s 12: server_servicename %s\n",FUNC_NAME,server_servicename);
    }
    if(server_servicename[0] == '\0') {
        printf("%s: FAILED because server_servicename is NULL\n",FUNC_NAME);
        ddns_return_status_success = FALSE;
        client_status = CLIENT_ERROR_MISCONFIGURED;
        host_status_1 = HOST_ERROR;
        client_Lasterror = MISCONFIGURATION_ERROR;
        goto EXIT;
    }



    //get and check Client.Username
    if(syscfg_get( NULL, "arddnsclient_1::Username", client_username, sizeof(client_username)) == 0) {
        printf("%s 9: client_username %s\n",FUNC_NAME,client_username);
    }
    if(client_username[0] == '\0') {
        printf("%s: FAILED because client_username %s\n",FUNC_NAME,client_username);
        ddns_return_status_success = FALSE;
        client_status = CLIENT_ERROR;
        host_status_1 = HOST_ERROR;
        client_Lasterror = AUTHENTICATION_ERROR;
        strcpy(return_status,"error-auth");
        goto EXIT;
    }

    if(strcmp(server_servicename,"duckdns")) {
        if(syscfg_get( NULL, "arddnsclient_1::Password", client_password, sizeof(client_password)) == 0) {
            int i,j;
            char pwd;
            size_t len = strlen(client_password);
            j=0;
            command[0] = '\0';
            for(i=0;i<len;i++){
                pwd = client_password[i];
                if((pwd!='-') && (pwd != '_') && (pwd!='.') && (pwd!='~') && (!((pwd>='0')&&(pwd<='9'))) && (!((pwd>='A')&&(pwd<='Z'))) && (!((pwd>='a')&&(pwd<='z')))) {
                    command[j] = '%';
                    command[j+1] = bin2hex(pwd >> 4);
                    command[j+2] = bin2hex(pwd & 0x0F);
                    command[j+3] = 0;
                    j+=3;
                } else {
                    command[j] = pwd;
                    command[j+1] = '\0';
                    j++;
                }
            }
            strcpy(client_password, command);
        }
        if(client_password[0] == '\0') {
            printf("%s: FAILED because client_password is NULL %s %s\n",FUNC_NAME,client_password);
            ddns_return_status_success = FALSE;
            client_status = CLIENT_ERROR;
            host_status_1 = HOST_ERROR;
            client_Lasterror = AUTHENTICATION_ERROR;
            strcpy(return_status,"error-auth");
            goto EXIT;
        }
    }

    //get and check Host.hostname
    if(syscfg_get( NULL, "ddns_host_name_1", host_name, sizeof(host_name)) == 0) {
        printf("%s 11: host_name %s\n",FUNC_NAME,host_name);
    }
    if(host_name[0] == '\0') {
        printf("%s: FAILED because hostname is NULL\n",FUNC_NAME);
        ddns_return_status_success = FALSE;
        client_status = CLIENT_ERROR;
        host_status_1 = HOST_ERROR;
        client_Lasterror = DNS_ERROR;
        goto EXIT;
    }

    //remove ipupdate.$server_servicename
    sprintf(command," rm /var/tmp/ipupdate.%s",server_servicename);
    system(command);

    //create the command line
    if(strcmp(server_servicename,"changeip") == 0) {
        sprintf(command,"/usr/bin/curl --interface erouter0 -o /var/tmp/ipupdate.%s --url 'http://nic.changeip.com/nic/update?u=%s&p=%s&hostname=%s&ip=%s' --trace-ascii %s > %s 2>&1",
                server_servicename,client_username,client_password,host_name,wan_ipaddr,GENERAL_FILE,OUTPUT_FILE);
    } else if (strcmp(server_servicename,"dyndns") == 0) {
        sprintf(command, "/usr/bin/curl --interface erouter0 -o /var/tmp/ipupdate.%s --user %s:%s --url 'http://members.dyndns.org/nic/update?hostname=%s&myip=%s' --trace-ascii %s > %s 2>&1",
                server_servicename,client_username,client_password,host_name,wan_ipaddr,GENERAL_FILE,OUTPUT_FILE);
    } else if (strcmp(server_servicename,"afraid") == 0) {
        sprintf(command, "/usr/bin/curl --interface erouter0 -o /var/tmp/ipupdate.%s --user %s:%s --insecure --url 'https://freedns.afraid.org/nic/update?hostname=%s&myip=%s' --trace-ascii %s > %s 2>&1",
                server_servicename,client_username,client_password,host_name,wan_ipaddr,GENERAL_FILE,OUTPUT_FILE);
    } else if(strcmp(server_servicename,"no-ip") == 0) {
        sprintf(command,"/usr/bin/curl --interface erouter0 -o /var/tmp/ipupdate.%s --url 'http://%s:%s@dynupdate.no-ip.com/nic/update?hostname=%s&myip=%s' --trace-ascii %s > %s 2>&1",
                server_servicename,client_username,client_password,host_name,wan_ipaddr,GENERAL_FILE,OUTPUT_FILE);
    } else if(strcmp(server_servicename,"duckdns")==0) {
        sprintf(command, "/usr/bin/curl --interface erouter0 -o /var/tmp/ipupdate.%s -g --insecure --url 'https://www.duckdns.org/update?domains=%s&token=%s&ip=%s&verbose=true' --trace-ascii %s > %s 2>&1",
                server_servicename,host_name,client_username,wan_ipaddr,GENERAL_FILE,OUTPUT_FILE);
    }
    printf("%s: servicename %s, command is %s\n",FUNC_NAME,server_servicename,command);

    //execute command
    ret = system(command);

    //analyze the result of command and set syscfg ddns_client_Lasterror / sysevent ddns_return_status here based on the error
    if(0 == ret) { ///usr/bin/curl succeed
        printf("%s: servicename %s command succeed\n",FUNC_NAME,server_servicename);

        sprintf(buf, "/var/tmp/ipupdate.%s",server_servicename);
        output_file = fopen(buf, "r");
        if (output_file == NULL) {
            ddns_return_status_success = FALSE;
            client_Lasterror = DNS_ERROR;
            strcpy(return_status,"error");
            printf("%s: failed to open %s\n",FUNC_NAME, buf);
            goto EXIT;
        }
        while(fgets(command,COMMAND_BUF_LENGTH, output_file) != NULL) {
            if((register_success(server_servicename,buf) && strstr(command, buf))
                 || (update_success(server_servicename,buf) && strstr(command, buf))) {
                  printf("%s: found succeed register_success or update_success string in file /var/tmp/ipupdate.%s\n",FUNC_NAME,server_servicename);
                  ddns_return_status_success = TRUE;
                  break;
            } else if(hostname_error(server_servicename,buf) && strstr(command, buf)) {
                  printf("%s: found hostname_error string in file /var/tmp/ipupdate.%s\n",FUNC_NAME,server_servicename);
                  ddns_return_status_success = FALSE;
                  client_Lasterror = MISCONFIGURATION_ERROR;
                  strcpy(return_status,"error");
            } else if((username_error(server_servicename,buf) && strstr(command, buf))
                  || (password_error(server_servicename,buf) && strstr(command, buf))
                  || (general_error(server_servicename,buf) && strstr(command, buf))
                  || (token_error(server_servicename,buf) && strstr(command, buf))
				|| (strstr(command, "KO"))) {
                  printf("%s: found username_error or password_error or general_error or token_error string in file /var/tmp/ipupdate.%s\n",FUNC_NAME,server_servicename);
                  ddns_return_status_success = FALSE;
                  client_Lasterror = AUTHENTICATION_ERROR;
                  strcpy(return_status,"error-auth");
            } else {
                  ddns_return_status_success = FALSE;
                  printf("%s: didn't find expected result in file /var/tmp/ipupdate.%s\n",FUNC_NAME,server_servicename);
                  client_Lasterror = AUTHENTICATION_ERROR;
                  strcpy(return_status,"error-auth");
            }
        }
        fclose(output_file);
    } else {    ///usr/bin/curl failed
        CcspTraceWarning(("%s: servicename %s command failed\n",FUNC_NAME,server_servicename));
        printf("%s: servicename %s command failed\n",FUNC_NAME,server_servicename);
        ddns_return_status_success = FALSE;

        output_file = fopen(GENERAL_FILE, "r");
        if (output_file == NULL) {
             client_Lasterror = DNS_ERROR;
             strcpy(return_status,"error");
             printf("%s: failed to open %s\n",FUNC_NAME, GENERAL_FILE);
             goto EXIT;
        }
        while(fgets(command,COMMAND_BUF_LENGTH, output_file) != NULL) {
             if((strstr(command, CONNECTING1_ERROR)) || (strstr(command, CONNECTING2_ERROR))) {
                 printf("%s: found error %s or %s in file %s\n",FUNC_NAME,CONNECTING1_ERROR,CONNECTING2_ERROR, GENERAL_FILE);
                 client_Lasterror = CONNECTION_ERROR;
                 strcpy(return_status,"error-connect");
             } else {
                 printf("%s: found error %s or no keyword in file %s\n",FUNC_NAME,RESOLVE_ERRO,GENERAL_FILE);
                 client_Lasterror = CONNECTION_ERROR;
                 strcpy(return_status,"error");
             }
        }
        fclose(output_file);
    }
 
EXIT:

    if(ddns_return_status_success == TRUE) {
        client_status = CLIENT_UPDATED;
        host_status_1 = HOST_REGISTERED;
        client_Lasterror = NO_ERROR;
        strcpy(return_status,"success");
    }

    sprintf(command, "ddns_return_status%d",server_index);
    if(return_status[0] == 0) {
        sysevent_set(se_fd, se_token, "ddns_return_status", "error", 0);
        sysevent_set(se_fd, se_token, command, "error", 0);
    } else {
        sysevent_set(se_fd, se_token, "ddns_return_status", return_status, 0);
        sysevent_set(se_fd, se_token, command, return_status, 0);
    }

    if((client_status == 0 ) || (client_status > 9))
        client_status = CLIENT_ERROR;

    buf[0] = '0' + client_status;
    buf[1] = '\0';
    syscfg_set(NULL,"ddns_client_Status", buf);

    if((host_status_1 == 0 ) || (host_status_1 > 9))
        host_status_1 = HOST_ERROR;

    buf[0] = '0' + host_status_1;
    buf[1] = '\0';
    syscfg_set(NULL,"ddns_host_status_1", buf);

    if((client_Lasterror == 0 ) || (client_Lasterror > 9))
        client_Lasterror = DNS_ERROR;

    buf[0] = '0' + client_Lasterror;
    buf[1] = '\0';
    syscfg_set(NULL,"ddns_client_Lasterror", buf);

    printf("%s: ddns_return_status_success %d return_status %s, client_status %d host_status_1 %d client_Lasterror %d\n",FUNC_NAME, ddns_return_status_success, return_status,client_status,host_status_1,client_Lasterror);

    //update the sysevent or syscfg based on the analyzing result
    gettimeofday(&tv, NULL);
    t = tv.tv_sec;
    info = localtime(&t);

    if(ddns_return_status_success == TRUE) {
        printf("%s: ddns_return_status_success, update status",FUNC_NAME);
        //system("/etc/utopia/service.d/service_ddns/ddns_success.sh");

        strftime (buf, sizeof(buf), "%m:%d:%y_%H:%M:%S\n",info); //format: 04:09:21_22:15:43
        sysevent_set(se_fd, se_token, "ddns_failure_time", "0", 0);
        sysevent_set(se_fd, se_token, "ddns_updated_time", buf, 0);
        syscfg_set(NULL,"ddns_host_lastupdate_1",buf);
/*
        //to do
        //echo "   rm -f $CHECK_INTERVAL_FILENAME" >> $CHECK_INTERVAL_FILENAME;
        //echo "#! /bin/sh" > $CHECK_INTERVAL_FILENAME;
        //echo "   /etc/utopia/service.d/service_dynamic_dns.sh ${SERVICE_NAME}-check" >> $CHECK_INTERVAL_FILENAME;
        //chmod 700 $CHECK_INTERVAL_FILENAME;
        //break
*/
        syscfg_commit();
        printf("%s: return 0 because everything looks good\n",FUNC_NAME);
        ret = 0;
    } else {

        strftime (buf, sizeof(buf), "%m:%d:%y_%H:%M:%S\n",info); //format: 04:09:21_22:15:43
        sysevent_set(se_fd, se_token, "ddns_failure_time", buf, 0);
        sysevent_set(se_fd, se_token, "ddns_updated_time", "0", 0);

        syscfg_commit();
        printf("%s: return -1 because curl command return !0 or found error message in output file\n",FUNC_NAME);
        ret = -1;
    }

    sprintf(buf, "rm -rf %s",UPDATING_CHECK_FILE);
    system(buf);

    return ret;

#if 0
/*
   #If there is no error-connect for any provider, then delete the $RETRY_SOON_FILENAME.
   RETRY_SOON_NEEDED=0
   ddns_enable_x=`syscfg get ddns_server_enable_${DnsIdx}`
   if [ "1" = "$ddns_enable_x" ]; then
       tmp_status=`sysevent get ddns_return_status${DnsIdx}`
       if [ "error-connect" = "$tmp_status" ] ; then
           sysevent set ddns_return_status
           sysevent set ddns_failure_time `date "+%s"`
           sysevent set ddns_updated_time
           RETRY_SOON_NEEDED=1
           break
       fi
   fi
   if [ "0" = "$RETRY_SONN_NEEDED" ]; then
       rm -f $RETRY_SOON_FILENAME
   fi
*/
#endif
}

/***********************************************************************
 APIs for Object:

    DyanmicDNS.

    *  CosaDmlDynamicDns_GetEnable
    *  CosaDmlDynamicDns_GetsupportedServices
    *  CosaDmlDynamicDns_SetEnable
***********************************************************************/

BOOL
CosaDmlDynamicDns_GetEnable()
{
   char buf[8] = {0};

   if ((!syscfg_init()) &&
       (!syscfg_get(NULL, "dynamic_dns_enable", buf, sizeof(buf))))
   {
       return (strcmp(buf, "1") == 0);
   }
   return 0;
}

ULONG
CosaDmlDynamicDns_GetsupportedServices
    (
        char*  supportedServices
    )
{
    char buf[128] = {0};
    errno_t rc = -1;

    if ((!syscfg_init()) &&
        (!syscfg_get( NULL, "ddns_service_providers_list", buf, sizeof(buf))))
    {
        /* destination buffer supportedServices is declared as an array size of 1024 bytes in the calling func. */
        rc = strcpy_s(supportedServices, 1024, buf);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }
    return -1;
}

ULONG
CosaDmlDynamicDns_SetEnable
    (
        BOOL  bValue
    )
{
   char buf[8] = {0};
   errno_t rc = -1;

   if (!syscfg_init())
   {
       syscfg_get(NULL, "dslite_enable", buf, sizeof(buf));
       if((strcmp(buf, "1") == 0) && bValue == TRUE){
           return -1;
       }
       rc = strcpy_s(buf, sizeof(buf), ((bValue == FALSE) ? "0" : "1"));
       if(rc != EOK)
       {
           ERR_CHK(rc);
           return -1;
       }
       syscfg_set(NULL, "dynamic_dns_enable", buf);
       if(bValue == FALSE)
       {
          syscfg_set(NULL, "arddnsclient_1::enable", buf);
          syscfg_set(NULL, "ddns_host_enable_1", buf);
       }
       syscfg_commit();
       return 0;
   }
   return -1;
}

/***********************************************************************
 APIs for Object:

    DyanmicDNS.

    *  CosaDmlDynamicDns_Client_GetNumberOfEntries
    *  CosaDmlDynamicDns_Client_GetEntryByIndex
    *  CosaDmlDynamicDns_Client_SetValues
    *  CosaDmlDynamicDns_Client_AddEntry
    *  CosaDmlDynamicDns_Client_DelEntry
    *  CosaDmlDynamicDns_Client_GetConf
    *  CosaDmlDynamicDns_Client_SetConf
***********************************************************************/

static int g_NrDynamicDnsClient =  0;

static int
DynamicDns_Client_InsGetIndex
    (
        ULONG ins
    )
{
    int i, ins_num, ret = -1;
    UtopiaContext ctx;

    CosaDmlDynamicDns_Client_GetNumberOfEntries();
    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    for (i = 0; i < g_NrDynamicDnsClient; i++)
    {
        Utopia_GetDynamicDnsClientInsNumByIndex(&ctx, i, &ins_num);
        if (ins_num == ins) {
            ret = i;
            break;
        }
    }
    Utopia_Free(&ctx, 0);

    return ret;
}

ULONG
CosaDmlDynamicDns_Client_GetNumberOfEntries(void)
{
    UtopiaContext ctx;

    if(!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    Utopia_GetNumberOfDynamicDnsClient(&ctx, &g_NrDynamicDnsClient);
    Utopia_Free(&ctx, 0);
    return g_NrDynamicDnsClient;
}

ANSC_STATUS
CosaDmlDynamicDns_Client_GetEntryByIndex
    (
        ULONG index,
        COSA_DML_DDNS_CLIENT *pEntry
    )
{
    UtopiaContext ctx;
    DynamicDnsClient_t  DDNSclient = {0};

    if (index >= g_NrDynamicDnsClient || !Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    Utopia_GetDynamicDnsClientByIndex(&ctx, index, &DDNSclient);

    pEntry->InstanceNumber = DDNSclient.InstanceNumber;
    pEntry->Enable = DDNSclient.Enable;
    pEntry->Status = DDNSclient.Status;
    pEntry->LastError = DDNSclient.LastError;
    _ansc_strncpy(pEntry->Alias,    DDNSclient.Alias,    sizeof(pEntry->Alias)-1);
    _ansc_strncpy(pEntry->Username, DDNSclient.Username, sizeof(pEntry->Username)-1);
    _ansc_strncpy(pEntry->Password, DDNSclient.Password, sizeof(pEntry->Password)-1);
    _ansc_strncpy(pEntry->Server,   DDNSclient.Server,   sizeof(pEntry->Server)-1);

    Utopia_Free(&ctx, 0);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Client_SetValues
    (
        ULONG index,
        ULONG ins,
        const char *alias
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (index >= g_NrDynamicDnsClient || !Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    rc = Utopia_SetDynamicDnsClientInsAndAliasByIndex(&ctx, index, ins, alias);
    Utopia_Free(&ctx, !rc);

    return (rc != 0) ? ANSC_STATUS_FAILURE : ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Client_AddEntry
    (
        COSA_DML_DDNS_CLIENT *pEntry
    )
{
    int rc = -1;
    UtopiaContext ctx;
    DynamicDnsClient_t  DDNSclient = {0};

    if (!Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    DDNSclient.InstanceNumber = pEntry->InstanceNumber;
    DDNSclient.Enable         = pEntry->Enable;
    _ansc_strncpy(DDNSclient.Alias,    pEntry->Alias,    sizeof(DDNSclient.Alias)-1);
    _ansc_strncpy(DDNSclient.Username, pEntry->Username, sizeof(DDNSclient.Username)-1);
    _ansc_strncpy(DDNSclient.Password, pEntry->Password, sizeof(DDNSclient.Password)-1);
    _ansc_strncpy(DDNSclient.Server,   pEntry->Server,   sizeof(DDNSclient.Server)-1);

    rc = Utopia_AddDynamicDnsClient(&ctx, &DDNSclient);

    Utopia_GetNumberOfDynamicDnsClient(&ctx, &g_NrDynamicDnsClient);
    Utopia_Free(&ctx, !rc);

#if 1
    if (CosaDmlDynamicDns_GetEnable() && pEntry->Enable == TRUE)
    {
        /* reset the DynamicDNS client and host status before restart*/
        resetDynamicDNSStatus();
        CcspTraceInfo(("%s Going to restart dynamic dns service",__FUNCTION__));
        if(access("/var/tmp/updating_ddns_server.txt", F_OK ) == 0 ) {
            vsystem("/etc/utopia/service.d/service_dynamic_dns.sh dynamic_dns-restart &");
        } else {
            rc = update_ddns_server();
        }
    }
#endif
    return (rc != 0) ? ANSC_STATUS_FAILURE : ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Client_DelEntry
    (
        ULONG ins
    )
{
    int rc = -1;
    UtopiaContext ctx;

    if (!Utopia_Init(&ctx))
        return ANSC_STATUS_FAILURE;

    rc = Utopia_DelDynamicDnsClient(&ctx, ins);
    Utopia_GetNumberOfDynamicDnsClient(&ctx, &g_NrDynamicDnsClient);
    Utopia_Free(&ctx, !rc);

    return (rc != 0) ? ANSC_STATUS_FAILURE : ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Client_GetConf
    (
        ULONG ins,
        COSA_DML_DDNS_CLIENT *pEntry
    )
{
    int index;

    if ((index = DynamicDns_Client_InsGetIndex(ins)) == -1)
    {
        return ANSC_STATUS_FAILURE;
    }

    return CosaDmlDynamicDns_Client_GetEntryByIndex(index, pEntry);
}

ANSC_STATUS
CosaDmlDynamicDns_Client_SetConf
    (
        ULONG ins,
        COSA_DML_DDNS_CLIENT *pEntry
    )
{
    int index, rc = -1;
    char enable_path[sizeof(SYSCFG_HOST_ENABLE_KEY) + 1] = {0};
    BOOLEAN enable = FALSE, isUserconfChanged = FALSE;
    UtopiaContext ctx;
    DynamicDnsClient_t  DDNSclient = {0};
    char client_status[2];

    ULONG InsNumber;
    ANSC_HANDLE pHostnameInsContext = NULL;
    PCOSA_CONTEXT_LINK_OBJECT    pHostLinkObj = NULL;
    COSA_DML_DDNS_HOST           *pHostEntry = NULL;
    bool bReadyUpdate = FALSE;

    if ((index = DynamicDns_Client_InsGetIndex(ins)) == -1 || !Utopia_Init(&ctx))
    {
        return ANSC_STATUS_FAILURE;
    }

    if (index >= g_NrDynamicDnsClient)
    {
        return ANSC_STATUS_FAILURE;
    }

    syscfg_get(NULL,"ddns_client_Status", client_status, sizeof(client_status));
	

    Utopia_GetDynamicDnsClientByIndex(&ctx, index, &DDNSclient);
    if (CosaDmlDynamicDns_GetEnable() && pEntry->Enable == TRUE &&
       ((strcmp(DDNSclient.Username, pEntry->Username) != 0) ||
       (strcmp(DDNSclient.Password, pEntry->Password) != 0) ||
       (strcmp(DDNSclient.Server, pEntry->Server) != 0)) ||
       (atoi(client_status) != CLIENT_UPDATED))
    {
        CcspTraceInfo(("%s UserConf changed \n",__FUNCTION__));
        isUserconfChanged = TRUE;
    }

    DDNSclient.InstanceNumber = pEntry->InstanceNumber;
    DDNSclient.Enable         = pEntry->Enable;
    _ansc_strncpy(DDNSclient.Alias,    pEntry->Alias,    sizeof(DDNSclient.Alias)-1);
    _ansc_strncpy(DDNSclient.Username, pEntry->Username, sizeof(DDNSclient.Username)-1);
    _ansc_strncpy(DDNSclient.Password, pEntry->Password, sizeof(DDNSclient.Password)-1);
    _ansc_strncpy(DDNSclient.Server,   pEntry->Server,   sizeof(DDNSclient.Server)-1);

    rc = Utopia_SetDynamicDnsClientByIndex(&ctx, index, &DDNSclient);
    Utopia_Free(&ctx, !rc);

    snprintf(enable_path, sizeof(enable_path), SYSCFG_HOST_ENABLE_KEY, ins);
    UtGetBool(enable_path, &enable);
    if (enable != pEntry->Enable)
    {
        UtSetBool(enable_path, pEntry->Enable);
        if (pEntry->Enable == TRUE && CosaDmlDynamicDns_GetEnable()) {
            CcspTraceInfo(("%s UserConf changed - Enable changed from false to true \n",__FUNCTION__));
            isUserconfChanged = TRUE;
        }
    }


	pHostnameInsContext = DDNSHostname_GetEntry(NULL, 0, &InsNumber);
	if(pHostnameInsContext != NULL) {
		pHostLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)pHostnameInsContext;
		pHostEntry   = (COSA_DML_DDNS_HOST *)pHostLinkObj->hContext;
		printf(" Client_SetConf pClientEntry->Enable %d Server %s Username %s pHostEntry->Enable %d Hostname %s pHostEntry %08x InsNumber %d \n",pEntry->Enable,pEntry->Server,pEntry->Username,pHostEntry->Enable,pHostEntry->Name,pHostEntry,InsNumber);
	}


    if((pEntry->Enable) && (pEntry->Server[0] != '\0') && (pEntry->Username[0] != '\0') && pHostEntry && (pHostEntry->Enable) && (pHostEntry->Name[0]!='\0')) {
        if(strstr(pHostEntry->Name,"duckdns") == NULL) { //Check whether password is null or not for services other than duckdns
            if(pEntry->Password[0] != '\0') {
                 printf(" READY to verify and update ddns server\n");
                 bReadyUpdate  = TRUE;
            } else {
                    printf("NOT READY to verify and update ddns server\n");
            }
        } else {// for duckdns no need to check password
            printf(" READY to verify and update ddns server\n");
            bReadyUpdate  = TRUE;
        }
    } else {
            printf(" NOT READY to verify and update ddns server\n");
    }

    if ((isUserconfChanged == TRUE) && (bReadyUpdate  == TRUE))
    {
        /* reset the DynamicDNS client and host status before restart*/
        resetDynamicDNSStatus();
        CcspTraceInfo(("%s Going to restart dynamic dns service",__FUNCTION__));
        if(access("/var/tmp/updating_ddns_server.txt", F_OK ) == 0 ) {
            vsystem("/etc/utopia/service.d/service_dynamic_dns.sh dynamic_dns-restart &");
        } else {
            rc = update_ddns_server();
        }
    }

    return (rc != 0) ? ANSC_STATUS_FAILURE : ANSC_STATUS_SUCCESS;
}

ULONG
CosaDmlDynamicDns_GetClientStatus()
{
   char buf[8] = {0};
   unsigned long pVal = 6;

   if ((!syscfg_init()) &&
       (!syscfg_get(NULL, "ddns_client_Status", buf, sizeof(buf))))
   {
       if(buf[0] != '\0')
       {
           pVal = atoi(buf);
       }
   }
   return pVal;
}

ULONG
CosaDmlDynamicDns_GetClientLastError()
{
   char buf[8] = {0};
   unsigned long pVal = 1;

   if ((!syscfg_init()) &&
       (!syscfg_get(NULL, "ddns_client_Lasterror", buf, sizeof(buf))))
   {
      if(buf[0] != '\0')
       {
           pVal = atoi(buf);
       }
   }
   return pVal;
}


/***********************************************************************
 APIs for Object:

    DyanmicDNS.Client.{i}.Host.{i}

    *  CosaDmlDynamicDns_Host_GetNumberOfEntries
    *  CosaDmlDynamicDns_Host_GetEntryByIndex
    *  CosaDmlDynamicDns_Host_SetValues
    *  CosaDmlDynamicDns_Host_AddEntry
    *  CosaDmlDynamicDns_Host_DelEntry
    *  CosaDmlDynamicDns_Host_GetConf
    *  CosaDmlDynamicDns_Host_SetConf
***********************************************************************/
static int g_NrDynamicDnsHost =  0;
COSA_DML_DDNS_HOST *g_DDNSHost = NULL;

void CosaInitializeTr181DdnsHost()
{
    int i = 0;
    errno_t rc = -1;
    g_NrDynamicDnsHost = MAX_HOST_COUNT;
    if(!g_DDNSHost && (g_NrDynamicDnsHost > 0))
    {
        g_DDNSHost = (COSA_DML_DDNS_HOST *)AnscAllocateMemory(g_NrDynamicDnsHost * sizeof(COSA_DML_DDNS_HOST));
        if(g_DDNSHost)
        {
            for(i=0; i<g_NrDynamicDnsHost; i++)
            {
                g_DDNSHost[i].Enable = FALSE;
                g_DDNSHost[i].InstanceNumber = i+1;
                rc = sprintf_s(g_DDNSHost[i].Alias, sizeof(g_DDNSHost[i].Alias), "cpe-ddns-host_%d", i+1);
                if(rc < EOK)
                {
                    ERR_CHK(rc);
                }
                g_DDNSHost[i].Status = HOST_DISABLED;
            }
        }
    }
}

static int
DynamicDns_Host_InsGetIndex
    (
        ULONG ins
    )
{
   int i, ret = -1;

    CosaDmlDynamicDns_Host_GetNumberOfEntries();

    for (i = 0; i < g_NrDynamicDnsHost; i++)
    {
        if (g_DDNSHost[i].InstanceNumber == ins) {
            ret = i;
            break;
        }
    }

    return ret;
}

ULONG
CosaDmlDynamicDns_Host_GetNumberOfEntries(void)
{
    return g_NrDynamicDnsHost;
}

ANSC_STATUS
CosaDmlDynamicDns_Host_GetEntryByIndex
    (
        ULONG index,
        COSA_DML_DDNS_HOST *pEntry
    )
{
    char enable_path[sizeof(SYSCFG_HOST_ENABLE_KEY) + 1] = {0};
    char status_path[sizeof(SYSCFG_HOST_STATUS_KEY) + 1] = {0};
    char name_path[sizeof(SYSCFG_HOST_NAME_KEY) + 1] = {0};

    BOOLEAN enable = FALSE;
    char name[256] = {0};
    ULONG status = HOST_DISABLED;

    if (index >= g_NrDynamicDnsHost || (!g_DDNSHost))
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Get from Syscfg db if present or return the default values */
    snprintf(enable_path, sizeof(enable_path), SYSCFG_HOST_ENABLE_KEY, index + 1);
    snprintf(status_path, sizeof(status_path), SYSCFG_HOST_STATUS_KEY, index + 1);
    snprintf(name_path, sizeof(name_path), SYSCFG_HOST_NAME_KEY, index + 1);

    if ((UtGetBool(enable_path, &enable) == ANSC_STATUS_SUCCESS) &&
        (UtGetUlong(status_path, &status) == ANSC_STATUS_SUCCESS) &&
        (UtGetString(name_path, name, (sizeof(name) - 1)) == ANSC_STATUS_SUCCESS))
    {
        g_DDNSHost[index].Enable = enable;
        _ansc_strncpy(g_DDNSHost[index].Name, name, sizeof(g_DDNSHost[index].Name)-1);
        g_DDNSHost[index].Status = status;
    }
    pEntry->InstanceNumber = g_DDNSHost[index].InstanceNumber;
    pEntry->Enable = g_DDNSHost[index].Enable;
    _ansc_strncpy(pEntry->Alias, g_DDNSHost[index].Alias, sizeof(pEntry->Alias)-1);
    _ansc_strncpy(pEntry->Name,  g_DDNSHost[index].Name,  sizeof(pEntry->Name)-1);
    pEntry->Status = g_DDNSHost[index].Status;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Host_SetValues
    (
        ULONG index,
        ULONG ins,
        const char *alias
    )
{

    if (index >= g_NrDynamicDnsHost || !g_DDNSHost)
    {
        return ANSC_STATUS_FAILURE;
    }

    g_DDNSHost[index].InstanceNumber = ins;
    _ansc_strncpy(g_DDNSHost[index].Alias, alias, sizeof(g_DDNSHost[index].Alias)-1);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Host_AddEntry
    (
        COSA_DML_DDNS_HOST *pEntry
    )
{
    //To fix the issue that CosaDmlDynamicDns_Host_SetConf fails if ins isn't 1. 
    CosaDmlDynamicDns_Host_SetValues(0,pEntry->InstanceNumber, pEntry->Alias);
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Host_DelEntry
    (
        ULONG ins
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Host_GetConf
    (
        ULONG ins,
        COSA_DML_DDNS_HOST *pEntry
    )
{
    int index;

    if ((index = DynamicDns_Host_InsGetIndex(ins)) == -1)
    {
        return ANSC_STATUS_FAILURE;
    }

    return CosaDmlDynamicDns_Host_GetEntryByIndex(index, pEntry);
}

ANSC_STATUS
CosaDmlDynamicDns_Host_SetConf
    (
        ULONG ins,
        COSA_DML_DDNS_HOST *pEntry
    )
{
    ULONG index;
    BOOLEAN isHostchanged = FALSE;

    char enable_path[sizeof(SYSCFG_HOST_ENABLE_KEY) + 1] = {0};
    // char status_path[sizeof(SYSCFG_HOST_STATUS_KEY) + 1] = {0};
    char name_path[sizeof(SYSCFG_HOST_NAME_KEY) + 1] = {0};
	char host_status[2];


	ULONG InsNumber;
	ANSC_HANDLE pClientInsContext = NULL;
    PCOSA_CONTEXT_LINK_OBJECT    pClientLinkObj = NULL;
    COSA_DML_DDNS_CLIENT           *pClientEntry = NULL;
	BOOL bReadyUpdate = FALSE;

    if ((index = DynamicDns_Host_InsGetIndex(ins)) == -1 || (!g_DDNSHost))
    {
        return ANSC_STATUS_FAILURE;
    }

    snprintf(enable_path, sizeof(enable_path), SYSCFG_HOST_ENABLE_KEY, index + 1);
    // snprintf(status_path, sizeof(status_path), SYSCFG_HOST_STATUS_KEY, index + 1);
    snprintf(name_path, sizeof(name_path), SYSCFG_HOST_NAME_KEY, index + 1);

    if(g_DDNSHost[index].Enable != pEntry->Enable)
    {
        isHostchanged = TRUE;
        g_DDNSHost[index].Enable = pEntry->Enable;
        UtSetBool(enable_path, g_DDNSHost[index].Enable);
    }

    // g_DDNSHost[index].Status = pEntry->Status;
    if(strcmp(g_DDNSHost[index].Name, pEntry->Name) != 0)
    {
        isHostchanged = TRUE;
        _ansc_strncpy(g_DDNSHost[index].Name, pEntry->Name, sizeof(g_DDNSHost[index].Name)-1);
        UtSetString(name_path, g_DDNSHost[index].Name);
    }

 	syscfg_get(NULL,"ddns_host_status_1", host_status, sizeof(host_status));
	if(atoi(host_status) != HOST_REGISTERED)
		isHostchanged = TRUE;


	pClientInsContext = DDNSClient_GetEntry(NULL, 0, &InsNumber);
	if(pClientInsContext) {
		pClientLinkObj      = (PCOSA_CONTEXT_LINK_OBJECT)pClientInsContext;
		pClientEntry   = (COSA_DML_DDNS_CLIENT *)pClientLinkObj->hContext;
		printf(" Host_SetConf pClientEntry->Enable %d Server %s Username %s pHostEntry->Enable %d Hostname %s pClientEntry %08x InsNumber %d\n",pClientEntry->Enable,pClientEntry->Server,pClientEntry->Username,pEntry->Enable,pEntry->Name,pClientEntry,InsNumber);
	}


	if((pClientInsContext) && (pClientEntry->Enable) && (pClientEntry->Server[0] != '\0') && (pClientEntry->Username[0] != '\0') && (pEntry->Enable) && (pEntry->Name[0]!='\0')) {
            if(strstr(pEntry->Name,"duckdns") == NULL) { //Check whether password is null or not for services other than duckdns
                if(pClientEntry->Password[0] != '\0') {
                    printf(" READY to verify and update ddns server\n");
                    bReadyUpdate  = TRUE;
                } else {
                    printf("NOT READY to verify and update ddns server\n");
                }
            } else {// for duckdns no need to check password
                    printf(" READY to verify and update ddns server\n");
                    bReadyUpdate  = TRUE;
            }
        } else {
            printf(" NOT READY to verify and update ddns server\n");
	}

    if (bReadyUpdate && CosaDmlDynamicDns_GetEnable() && (g_DDNSHost[index].Enable == TRUE)
            && (isHostchanged == TRUE) && (g_DDNSHost[index].Name[0] != '\0'))
    {
        /* reset the DynamicDNS client and host status before restart*/
        resetDynamicDNSStatus();
        g_DDNSHost[index].Status = 2; /* HOST_UPDATE_NEEDED=2 */
        CcspTraceInfo(("%s Going to restart dynamic dns service",__FUNCTION__));
      	if(access("/var/tmp/updating_ddns_server.txt", F_OK ) == 0 ) {
            vsystem("/etc/utopia/service.d/service_dynamic_dns.sh dynamic_dns-restart &");
        } else {
            if(0!=update_ddns_server()) {
				printf("update_ddns: CosaDmlDynamicDns_Host_SetConf return ANSC_STATUS_FAILURE\n");
                return ANSC_STATUS_FAILURE;
			}
        }
    }
    return ANSC_STATUS_SUCCESS;
}

/* To get the Last successful update of this name to the Dynamic DNS Server. */
ULONG
CosaDmlDynamicDns_GetHostLastUpdate
    (
        char*  lastUpdate
    )
{
    char buf[128] = {0};
    errno_t rc = -1;

    if ((!syscfg_init()) &&
        (!syscfg_get( NULL, "ddns_host_lastupdate_1", buf, sizeof(buf))))
    {
        /* destination buffer lastUpdate is defined as array size of 256 bytes in calling func. */
        rc = strcpy_s(lastUpdate, 256, buf);
        if(rc != EOK)
        {
            ERR_CHK(rc);
            return -1;
        }
        return 0;
    }
    return -1;
}

/***********************************************************************
 APIs for Object:

    DyanmicDNS.Server.{i}

    *  CosaDmlDynamicDns_Server_GetNumberOfEntries
    *  CosaDmlDynamicDns_Server_GetEntryByIndex
    *  CosaDmlDynamicDns_Server_SetValues
    *  CosaDmlDynamicDns_Server_AddEntry
    *  CosaDmlDynamicDns_Server_DelEntry
    *  CosaDmlDynamicDns_Server_GetConf
    *  CosaDmlDynamicDns_Server_SetConf
***********************************************************************/

static int g_NrDynamicDnsServer =  0;
COSA_DML_DDNS_SERVER  *g_DDNSServer = NULL;

void CosaInitializeTr181DdnsServiceProviderList()
{
    char service_list[128] = {0};
    errno_t rc = -1;

    if ((!syscfg_init()) &&
        (!syscfg_get( NULL, "ddns_service_providers_list", service_list, sizeof(service_list))))
    {
        ULONG index = 0;
        char enable_path[sizeof(SYSCFG_SERVER_ENABLE_KEY) + 1] = {0};
        char servicename_path[sizeof(SYSCFG_SERVER_SERVICENAME_KEY) +1] = {0};
        char serveraddress_path[sizeof(SYSCFG_SERVER_SERVERADDRESS_KEY) +1] = {0};
        g_NrDynamicDnsServer = sizeof(gDdnsServices)/sizeof(DDNS_SERVICE);
        g_DDNSServer = (COSA_DML_DDNS_SERVER *)AnscAllocateMemory(g_NrDynamicDnsServer * sizeof(COSA_DML_DDNS_SERVER));
       for(index = 0; index<g_NrDynamicDnsServer; index++)
        {
            g_DDNSServer[index].Enable = FALSE;
            g_DDNSServer[index].InstanceNumber = index+1;
            rc = sprintf_s(g_DDNSServer[index].Alias, sizeof(g_DDNSServer[index].Alias), "cpe-ddns-server_%lu", index+1);
            if(rc < EOK)
            {
                ERR_CHK(rc);
            }
            if (strstr(service_list, gDdnsServices[index].ServiceName))
            {
                g_DDNSServer[index].Enable = TRUE;
                snprintf(enable_path, sizeof(enable_path), SYSCFG_SERVER_ENABLE_KEY, index+1);
                UtSetBool(enable_path, g_DDNSServer[index].Enable);

                snprintf(g_DDNSServer[index].ServiceName, sizeof(g_DDNSServer[index].ServiceName), gDdnsServices[index].ServiceName);
               snprintf(servicename_path, sizeof(servicename_path), SYSCFG_SERVER_SERVICENAME_KEY, index + 1);
                UtSetString(servicename_path, g_DDNSServer[index].ServiceName);

                snprintf(g_DDNSServer[index].Name, sizeof(g_DDNSServer[index].Name), gDdnsServices[index].Name);
                snprintf(g_DDNSServer[index].SupportedProtocols, sizeof(g_DDNSServer[index].SupportedProtocols), gDdnsServices[index].SupportedProtocols);
                snprintf(g_DDNSServer[index].Protocol, sizeof(g_DDNSServer[index].Protocol), gDdnsServices[index].Protocol);

                snprintf(g_DDNSServer[index].ServerAddress, sizeof(g_DDNSServer[index].ServerAddress), gDdnsServices[index].ServerAddress);
                snprintf(serveraddress_path, sizeof(serveraddress_path), SYSCFG_SERVER_SERVERADDRESS_KEY, index + 1);
                UtSetString(serveraddress_path, g_DDNSServer[index].ServerAddress);
            }
        }
    }
}

static int
DynamicDns_Server_InsGetIndex
    (
        ULONG ins
    )
{
    int i, ret = -1;

    CosaDmlDynamicDns_Server_GetNumberOfEntries();

    for (i = 0; i < g_NrDynamicDnsServer; i++)
    {
        if (g_DDNSServer[i].InstanceNumber == ins) {
            ret = i;
            break;
        }
    }

    return ret;
}

ULONG
CosaDmlDynamicDns_Server_GetNumberOfEntries(void)
{
    return g_NrDynamicDnsServer;
}

ANSC_STATUS
CosaDmlDynamicDns_Server_GetEntryByIndex
    (
        ULONG index,
        COSA_DML_DDNS_SERVER *pEntry
    )
{
    char enable_path[sizeof(SYSCFG_SERVER_ENABLE_KEY) + 1] = {0};
    char protocol_path[sizeof(SYSCFG_SERVER_PROTOCOL_KEY) + 1] = {0};
    char checkinterval_path[sizeof(SYSCFG_SERVER_CHECKINTERVAL_KEY) + 1] = {0};
    char retryinterval_path[sizeof(SYSCFG_SERVER_RETRYINTERVAL_KEY) + 1] = {0};
    char maxretries_path[sizeof(SYSCFG_SERVER_MAXRETRIES_KEY) + 1] = {0};
    char serverport_path[sizeof(SYSCFG_SERVER_SERVERPORT_KEY) + 1] = {0};
    char serveraddress_path[sizeof(SYSCFG_SERVER_SERVERADDRESS_KEY) + 1] = {0};

    char protocol[8] = {0}, serveraddress[256] = {0};
    ULONG checkinterval = 0, retryinterval = DEFAULT_RETRYINTERVAL, maxretries = DEFAULT_MAXRETRIES, serverport = 0;
   BOOLEAN enable = FALSE;

    if (index >= g_NrDynamicDnsServer || (!g_DDNSServer))
    {
        return ANSC_STATUS_FAILURE;
    }

    /* Get from Syscfg db if present or return the default values */
    snprintf(enable_path, sizeof(enable_path), SYSCFG_SERVER_ENABLE_KEY, index + 1);
    snprintf(protocol_path, sizeof(protocol_path), SYSCFG_SERVER_PROTOCOL_KEY, index + 1);
    snprintf(checkinterval_path, sizeof(checkinterval_path), SYSCFG_SERVER_CHECKINTERVAL_KEY, index + 1);
    snprintf(retryinterval_path, sizeof(retryinterval_path), SYSCFG_SERVER_RETRYINTERVAL_KEY, index + 1);
    snprintf(maxretries_path, sizeof(maxretries_path), SYSCFG_SERVER_MAXRETRIES_KEY, index + 1);
    snprintf(serverport_path, sizeof(serverport_path), SYSCFG_SERVER_SERVERPORT_KEY, index + 1);
    snprintf(serveraddress_path, sizeof(serveraddress_path), SYSCFG_SERVER_SERVERADDRESS_KEY, index + 1);

    if ((UtGetBool(enable_path, &enable) == ANSC_STATUS_SUCCESS) &&
        (UtGetString(protocol_path, protocol, (sizeof(protocol) - 1)) == ANSC_STATUS_SUCCESS) &&
        (UtGetString(serveraddress_path, serveraddress, (sizeof(serveraddress) - 1)) == ANSC_STATUS_SUCCESS) &&
        (UtGetUlong(checkinterval_path, &checkinterval) == ANSC_STATUS_SUCCESS) &&
        (UtGetUlong(retryinterval_path, &retryinterval) == ANSC_STATUS_SUCCESS) &&
        (UtGetUlong(maxretries_path, &maxretries) == ANSC_STATUS_SUCCESS) &&
        (UtGetUlong(serverport_path, &serverport) == ANSC_STATUS_SUCCESS))
    {
         g_DDNSServer[index].Enable = enable;
         g_DDNSServer[index].CheckInterval = checkinterval;
         g_DDNSServer[index].RetryInterval = retryinterval;
         g_DDNSServer[index].MaxRetries = maxretries;
         g_DDNSServer[index].ServerPort = serverport;
         if (!AnscEqualString(protocol, "", TRUE))
         {
             _ansc_strncpy(g_DDNSServer[index].Protocol, protocol, sizeof(g_DDNSServer[index].Protocol)-1);
        }
         _ansc_strncpy(g_DDNSServer[index].ServerAddress, serveraddress, sizeof(g_DDNSServer[index].ServerAddress)-1);
    }

    /*Copy the values */
    pEntry->InstanceNumber = g_DDNSServer[index].InstanceNumber;
    pEntry->ServerPort = g_DDNSServer[index].ServerPort;
    pEntry->CheckInterval = g_DDNSServer[index].CheckInterval;
    pEntry->RetryInterval = g_DDNSServer[index].RetryInterval;
    pEntry->MaxRetries = g_DDNSServer[index].MaxRetries;
    pEntry->Enable = g_DDNSServer[index].Enable;
    _ansc_strncpy(pEntry->ServerAddress, g_DDNSServer[index].ServerAddress, sizeof(pEntry->ServerAddress)-1);
    _ansc_strncpy(pEntry->Protocol,      g_DDNSServer[index].Protocol,      sizeof(pEntry->Protocol)-1);
    _ansc_strncpy(pEntry->Alias,         g_DDNSServer[index].Alias,         sizeof(pEntry->Alias)-1);
    _ansc_strncpy(pEntry->Name,          g_DDNSServer[index].Name,          sizeof(pEntry->Name)-1);
    _ansc_strncpy(pEntry->ServiceName,   g_DDNSServer[index].ServiceName,   sizeof(pEntry->ServiceName)-1);
    _ansc_strncpy(pEntry->SupportedProtocols, g_DDNSServer[index].SupportedProtocols, sizeof(pEntry->SupportedProtocols)-1);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Server_SetValues
    (
        ULONG index,
        ULONG ins,
        const char *alias
    )
{

    if (index >= g_NrDynamicDnsServer || !g_DDNSServer)
    {
        return ANSC_STATUS_FAILURE;
    }

    g_DDNSServer[index].InstanceNumber = ins;
    _ansc_strncpy(g_DDNSServer[index].Alias, alias, sizeof(g_DDNSServer[index].Alias)-1);

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Server_AddEntry
    (
        COSA_DML_DDNS_SERVER *pEntry
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Server_DelEntry
    (
        ULONG ins
    )
{
    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS
CosaDmlDynamicDns_Server_GetConf
    (
        ULONG ins,
        COSA_DML_DDNS_SERVER *pEntry
    )
{
    int index;

    if ((index = DynamicDns_Server_InsGetIndex(ins)) == -1)
    {
        return ANSC_STATUS_FAILURE;
    }

    return CosaDmlDynamicDns_Server_GetEntryByIndex(index, pEntry);
}

ANSC_STATUS
CosaDmlDynamicDns_Server_SetConf
    (
        ULONG ins,
        COSA_DML_DDNS_SERVER *pEntry
    )
{
    ULONG index;
    char enable_path[sizeof(SYSCFG_SERVER_ENABLE_KEY) + 1] = {0};
    char protocol_path[sizeof(SYSCFG_SERVER_PROTOCOL_KEY) + 1] = {0};
    char checkinterval_path[sizeof(SYSCFG_SERVER_CHECKINTERVAL_KEY) + 1] = {0};
    char retryinterval_path[sizeof(SYSCFG_SERVER_RETRYINTERVAL_KEY) + 1] = {0};
    char maxretries_path[sizeof(SYSCFG_SERVER_MAXRETRIES_KEY) + 1] = {0};
    char serverport_path[sizeof(SYSCFG_SERVER_SERVERPORT_KEY) + 1] = {0};
    char serveraddress_path[sizeof(SYSCFG_SERVER_SERVERADDRESS_KEY) + 1] = {0};
    char servicename_path[sizeof(SYSCFG_SERVER_SERVICENAME_KEY) +1] = {0};

    if ((index = DynamicDns_Server_InsGetIndex(ins)) == -1 || (!g_DDNSServer))
    {
        return ANSC_STATUS_FAILURE;
    }

    snprintf(enable_path, sizeof(enable_path), SYSCFG_SERVER_ENABLE_KEY, index + 1);
    snprintf(protocol_path, sizeof(protocol_path), SYSCFG_SERVER_PROTOCOL_KEY, index + 1);
    snprintf(checkinterval_path, sizeof(checkinterval_path), SYSCFG_SERVER_CHECKINTERVAL_KEY, index + 1);
    snprintf(retryinterval_path, sizeof(retryinterval_path), SYSCFG_SERVER_RETRYINTERVAL_KEY, index + 1);
    snprintf(maxretries_path, sizeof(maxretries_path), SYSCFG_SERVER_MAXRETRIES_KEY, index + 1);
    snprintf(serverport_path, sizeof(serverport_path), SYSCFG_SERVER_SERVERPORT_KEY, index + 1);
    snprintf(serveraddress_path, sizeof(serveraddress_path), SYSCFG_SERVER_SERVERADDRESS_KEY, index + 1);
    snprintf(servicename_path, sizeof(servicename_path), SYSCFG_SERVER_SERVICENAME_KEY, index + 1);

    g_DDNSServer[index].ServerPort = pEntry->ServerPort;
    g_DDNSServer[index].CheckInterval = pEntry->CheckInterval;
    g_DDNSServer[index].RetryInterval = pEntry->RetryInterval;
    g_DDNSServer[index].MaxRetries = pEntry->MaxRetries;
    g_DDNSServer[index].InstanceNumber = pEntry->InstanceNumber;
    g_DDNSServer[index].Enable         = pEntry->Enable;
    _ansc_strncpy(g_DDNSServer[index].ServerAddress, pEntry->ServerAddress, sizeof(g_DDNSServer[index].ServerAddress)-1);
    _ansc_strncpy(g_DDNSServer[index].Protocol,      pEntry->Protocol,      sizeof(g_DDNSServer[index].Protocol)-1);
    _ansc_strncpy(g_DDNSServer[index].ServiceName,      pEntry->ServiceName,      sizeof(g_DDNSServer[index].ServiceName)-1);

    /* Set syscfg variable */
    UtSetBool(enable_path, g_DDNSServer[index].Enable);
    UtSetString(protocol_path, g_DDNSServer[index].Protocol);
    UtSetString(serveraddress_path, g_DDNSServer[index].ServerAddress);
    UtSetString(servicename_path, g_DDNSServer[index].ServiceName);
    UtSetUlong(checkinterval_path, g_DDNSServer[index].CheckInterval);
    UtSetUlong(retryinterval_path, g_DDNSServer[index].RetryInterval);
    UtSetUlong(maxretries_path, g_DDNSServer[index].MaxRetries);
    UtSetUlong(serverport_path, g_DDNSServer[index].ServerPort);

    return ANSC_STATUS_SUCCESS;
}
#endif
