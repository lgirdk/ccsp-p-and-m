/*********************************************************************
 * Copyright 2020 ARRIS Enterprises, LLC.
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

#include "cosa_lgi_wol_apis.h"
#include <syscfg/syscfg.h>
#include <pthread.h>

#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#define WOL_INTERVAL     "wol_interval"
#define WOL_RETRIES      "wol_retries"
#define BUFF_LEN         18
#define SEPERATOR        ':'
typedef unsigned char byte;

#define WOL_MAX          5
#define REPETITIONS_MAX  16
#define OK               0
#define MAC_ADDR_LEN     6
#define IF_NAME_SIZE     16
#define INTERFACE        "brlan0"
struct wol{
       char mac[MAC_ADDR_LEN];
       ULONG retries;
       ULONG interval;
       ULONG id;
       BOOL in_use;
};
static struct wol wol_queue[WOL_MAX];
pthread_t thread[WOL_MAX];
#define WOL_ETHRAW_PACKET     0x0842
#define BUFFER_SIZE           1024

struct  ether_header {
        u_int8_t        ether_dhost[MAC_ADDR_LEN];
        u_int8_t        ether_shost[MAC_ADDR_LEN];
        u_int16_t       ether_type;
};

ULONG createWolThread(ULONG queue_index, struct wol *);

ULONG
CosaDmlSetMACAddress
    (
        ANSC_HANDLE                 hContext,
        char                        *pValue
    )
{
    char if_name[IF_NAME_SIZE] = {0};
    unsigned int mac_address[MAC_ADDR_LEN];
    char mac[MAC_ADDR_LEN] = {0};
    ULONG interval = 0;
    ULONG retries = 0;
    int i = 0;

    strncpy(if_name, INTERFACE, IF_NAME_SIZE-1);
    sscanf(pValue, "%x:%x:%x:%x:%x:%x", &mac_address[0], &mac_address[1], &mac_address[2], &mac_address[3], &mac_address[4], &mac_address[5]);
    for(i = 0; i < MAC_ADDR_LEN ; i++)
    {
        mac[i] = (char)mac_address[i];
    }

    for (i = 0; i < WOL_MAX ; i++)
    {
        if( (wol_queue[i].in_use == TRUE) && (memcmp(wol_queue[i].mac, mac, MAC_ADDR_LEN) == 0) )
        {
            /* If Mac address is already in schedule, send only one packet. */
            sendWolEtherPacket(if_name, mac);
            return ANSC_STATUS_SUCCESS;
        }
        if(wol_queue[i].in_use == FALSE)
        {
            sendWolEtherPacket(if_name, mac);
            CosaDmlGetInterval(NULL, &interval);
            CosaDmlGetRetries(NULL, &retries);
            memcpy(wol_queue[i].mac, mac, MAC_ADDR_LEN);
            wol_queue[i].retries = retries;
            wol_queue[i].interval = interval;
            wol_queue[i].id = i;
            wol_queue[i].in_use = TRUE;
            createWolThread(i, &wol_queue[i]);
            return ANSC_STATUS_SUCCESS;
        }
    }
    CcspTraceError(("Queue is full!!\n"));
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlGetInterval
    (
        ANSC_HANDLE                 hContext,
        ULONG                        *puLong
    )
{
    char buf[BUFF_LEN] = {0};

    if (puLong == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    if (syscfg_init() == 0)
    {
        syscfg_get(NULL, WOL_INTERVAL, buf, sizeof(buf));
        *puLong = atoi(buf);
        return ANSC_STATUS_SUCCESS;
    }
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlSetInterval
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uLong
    )
{
    char buf[BUFF_LEN] = {0};
    snprintf(buf, sizeof(buf) - 1, "%lu", uLong);
    if(syscfg_init() == 0)
    {
        if (syscfg_set(NULL, WOL_INTERVAL, buf) == 0)
        {
            syscfg_commit();
            return ANSC_STATUS_SUCCESS;
        }
    }
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlGetRetries
    (
        ANSC_HANDLE                 hContext,
        ULONG                        *puLong
    )
{
    char buf[BUFF_LEN] = {0};

    if (puLong == NULL)
    {
        return ANSC_STATUS_FAILURE;
    }

    if (syscfg_init() == 0)
    {
        syscfg_get(NULL, WOL_RETRIES, buf, sizeof(buf));
        *puLong = atoi(buf);
        return ANSC_STATUS_SUCCESS;
    }
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
CosaDmlSetRetries
    (
        ANSC_HANDLE                 hContext,
        ULONG                       uLong
    )
{
    char buf[BUFF_LEN] = {0};
    snprintf(buf, sizeof(buf) - 1, "%lu", uLong);
    if(syscfg_init() == 0)
    {
        if (syscfg_set(NULL, WOL_RETRIES, buf) == 0)
        {
            syscfg_commit();
            return ANSC_STATUS_SUCCESS;
        }
    }
    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS
isMacValid(char *MacAddress)
{
    char mac_part[3] = {0};
    byte mac_byte = 0;
    byte mac_idx = 0;

    while(MacAddress[mac_idx] != 0 && mac_idx < BUFF_LEN){
        if(mac_idx != 15 && MacAddress[mac_idx + 2] != SEPERATOR)
            return ANSC_STATUS_FAILURE;
        strncpy(mac_part, MacAddress+mac_idx,2);
        mac_byte = (byte)strtol(mac_part, NULL, 16);
        if(mac_byte == 0 && strcmp(mac_part,"00"))
            return ANSC_STATUS_FAILURE;
        mac_idx += 3;
    }

    if(mac_idx == BUFF_LEN)
        return ANSC_STATUS_SUCCESS;
    return ANSC_STATUS_FAILURE;
}

void wolScheduler(void *arg)
{
    char if_name[IF_NAME_SIZE] = {0};
    struct wol *threadObj = (struct wol *)arg;

    while (threadObj->retries)
    {
        memset(&if_name, 0, sizeof(if_name));
        strncpy(if_name, INTERFACE, IF_NAME_SIZE-1);
        sendWolEtherPacket(if_name, threadObj->mac);
        threadObj->retries--;
        if(threadObj->interval == 0)
        {
            threadObj->interval = 1;
        }
        usleep(threadObj->interval * 1000);
    }
    wol_queue[threadObj->id].in_use = FALSE;
    free(threadObj);
    pthread_exit(NULL);
}

ULONG createWolThread(ULONG queue_index, struct wol *wol_data)
{
    int err;
    pthread_attr_t attr;
    struct wol *threadObj = (struct wol *)malloc(sizeof(struct wol));
    if(!threadObj)
    {
        return -1;
    }
    memcpy(threadObj, wol_data, sizeof(struct wol));

    // Exit if unable to create thread attributes
    err = pthread_attr_init(&attr);
    if (err != 0)
    {
        free(threadObj);
        return -1;
    }

    // Exit if unable to create the thread as detached (as we do not need to wait for it to terminate)
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err != 0)
    {
        free(threadObj);
        return -1;
    }

    // Exit if unable to start a new thread to perform the operation
    err = pthread_create(&thread[queue_index], &attr, (void *)&wolScheduler, threadObj); //threadObj -> MacAddress, retry, interval for that particular mac address.
    if (err != 0)
    {
        free(threadObj);
        return -1;
    }

    pthread_attr_destroy(&attr);
    return err;
}
/*This api is used to generate Wake Up on Lan Magic Packet*/
int sendWolEtherPacket(char *interface_name, char *mac)
{
    int length = 0;
    int i = 0;
    int sockfd;
    char send_buffer[BUFFER_SIZE];
    struct ifreq interface_index;
    struct ifreq interface_mac;
    struct ether_header *eth_head = (struct ether_header *) send_buffer;
    char wol_dst_mac[MAC_ADDR_LEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    struct sockaddr_ll sock_addr;

    if ((interface_name == NULL) || (mac == NULL))
    {
        CcspTraceWarning(("NULL arguments are passed\n"));
        return -1;
    }

    CcspTraceInfo(("Interface name %s\n",interface_name));
    CcspTraceInfo(("Target MAC Address: %02x:%02x:%02x:%02x:%02x:%02x\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]));

    /* RAW socket to send on */
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
        CcspTraceWarning(("Error in Creating RAW socket to send on\n"));
        return sockfd;
    }

    memset(&interface_index, 0, sizeof(struct ifreq));

    strncpy(interface_index.ifr_name, interface_name, IF_NAME_SIZE-1);

    if (ioctl(sockfd, SIOCGIFINDEX, &interface_index) < 0)
    {
        CcspTraceWarning(("SIOCGIFINDEX\n"));
        close(sockfd);
        return -1;
    }

    memset(&interface_mac, 0, sizeof(struct ifreq));

    strncpy(interface_mac.ifr_name, interface_name, IF_NAME_SIZE-1);

    if (ioctl(sockfd, SIOCGIFHWADDR, &interface_mac) < 0)
    {
        CcspTraceWarning(("SIOCGIFHWADDR\n"));
        close(sockfd);
        return -1;
    }

    /* Ethernet header */
    memset(send_buffer, 0, BUFFER_SIZE);
    memcpy(eth_head->ether_shost, &interface_mac.ifr_hwaddr.sa_data, MAC_ADDR_LEN);
    memcpy(eth_head->ether_dhost, wol_dst_mac, MAC_ADDR_LEN);

    /* Ethertype field */
    eth_head->ether_type = htons(WOL_ETHRAW_PACKET);
    length += sizeof(struct ether_header);

    /* Packet data followed by sixteen repetitions of the target computer's 48-bit MAC address */
    send_buffer[length++] = 0xff;
    send_buffer[length++] = 0xff;
    send_buffer[length++] = 0xff;
    send_buffer[length++] = 0xff;
    send_buffer[length++] = 0xff;
    send_buffer[length++] = 0xff;
    for(i = 0; i< REPETITIONS_MAX; i++)
    {
        memcpy(&send_buffer[length], mac, MAC_ADDR_LEN);
        length += MAC_ADDR_LEN;
    }

    /* network device Index */
    sock_addr.sll_ifindex = interface_index.ifr_ifindex;

    /* Address length*/
    sock_addr.sll_halen = MAC_ADDR_LEN;

    /* WOL Destination MAC */
    memcpy(sock_addr.sll_addr, wol_dst_mac, MAC_ADDR_LEN);

    /* Send packet */

    if (sendto(sockfd, send_buffer, length, 0, (struct sockaddr*)&sock_addr, sizeof(struct sockaddr_ll)) < 0)
    {
        CcspTraceWarning(("Send failed\n"));
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 0;
}

void initMac()
{
    memset(wol_queue, 0, sizeof(wol_queue));
}
