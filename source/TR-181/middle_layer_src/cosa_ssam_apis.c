/*****************************************************************************
 * Copyright 2021 Liberty Global B.V.
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
 ****************************************************************************/

#include <sys/stat.h>
#include <fcntl.h>
#include <ansc_platform.h>
#include <ccsp_trace.h>
#include <syscfg/syscfg.h>
#include "cosa_ssam_apis.h"

/*
   getrandom() was first supported by Linux 3.19 but we have
   kernels older than that, so read from /dev/urandom instead.
*/
static int read_random_bytes (void *buf, size_t len)
{
    int fd, nbytes;

    if ((fd = open("/dev/urandom", O_RDONLY)) < 0) {
        return 0;
    }
    if ((nbytes = read (fd, buf, len)) < 0) {
        nbytes = 0;
    }

    close (fd);

    return nbytes;
}

void ssam_start (void)
{
    char buf[12];
    char cmd[1024];
    int cid, maxdelay;
    unsigned int delay;
    unsigned short r;
    char *p;
    int i;

    syscfg_get(NULL, "ssam_enable", buf, sizeof(buf));
    if (strcmp(buf, "1") != 0) {
        return;
    }

    syscfg_get(NULL, "bridge_mode", buf, sizeof(buf));
    if (strcmp(buf, "1") == 0) {
        return;
    }

    if (access(SSAM_PID_FILE, F_OK) == 0) {
        return;
    }

    if (access(SSAM_PARTITION, F_OK) != 0) {
        if (mkdir(SSAM_PARTITION, 0777) != 0) {
            return;
        }
        if (system("mount -t tmpfs -o size=32M tmpfs " SSAM_PARTITION) != 0) {
            return;
        }

        /* Temp workaround for older versions of ssam-bin */
        if (system("ln -s /etc/certs/amazon.pem " SSAM_PARTITION "/.amazon.pem") != 0) {
            return;
        }
    }

    if (access("/var/sam_loader", F_OK) != 0) {
        if (mkdir("/var/sam_loader", 0777) != 0) {
            return;
        }

        syscfg_get(NULL, "Customer_Index", buf, sizeof(buf));
        if (buf[0] != 0) {
            cid = atoi(buf);
        }
        else {
            cid = 0;
        }

        p = cmd;
        p += sprintf(p, "mount -t tmpfs tmpfs /var/sam_loader && ");

        for (i = 0; i < 3; i++) {
            char key[64];
            snprintf(key, sizeof(key), "/etc/certs/ssam_%d_%d.pem", cid, i + 1);
            if (access(key, F_OK) != 0) {
                cid = 0;
                snprintf(key, sizeof(key), "/etc/certs/ssam_%d_%d.pem", cid, i + 1);
            }
            p += sprintf(p, "ln -sf %s /var/sam_loader/sign_key_%d.pem && ", key, i);
        }

        p += sprintf(p, "ln -sf /etc/certs/sam_key_1.pem /var/sam_loader/sign_key_3.pem && "
                        "ln -sf /etc/certs/sam_key_2.pem /var/sam_loader/sign_key_4.pem && "
                        "ln -sf /etc/certs/amazon.pem /var/sam_loader/amazon.pem && "
                        "mount -o remount,ro /var/sam_loader");

        if (system(cmd) != 0) {
            return;
        }
    }

    p = cmd;
    p += sprintf(p, "/usr/bin/sam");

    syscfg_get(NULL, "ssam_updaterenable", buf, sizeof(buf));
    if (strcmp(buf, "1") == 0) {
        p += sprintf(p, " -r");
    }

    syscfg_get(NULL, "ssam_provisioningmodel", buf, sizeof(buf));
    if (strcmp(buf, "2") == 0) {
        p += sprintf(p, " -s");
    }

    p += sprintf(p, " &");

    maxdelay = 0;
    syscfg_get(NULL, "ssam_maxstartdelay", buf, sizeof(buf));
    if (buf[0] != 0) {
        maxdelay = atoi(buf);
    }
    if (maxdelay <= 0) {
        maxdelay = 30;
    }

    if (read_random_bytes(&r, sizeof(r)) == sizeof(r)) {
        delay = (r % maxdelay) + 1;
    }
    else {
        delay = maxdelay / 2;
    }

    while (delay) {
        delay = sleep(delay);
    }

    if (system(cmd) != 0) {
        return;
    }
}

void ssam_stop (void)
{
    system("killall -s SIGINT sam");
}
