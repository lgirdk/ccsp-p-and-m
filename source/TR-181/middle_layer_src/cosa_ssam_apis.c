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

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ansc_platform.h>
#include <ccsp_trace.h>
#include <syscfg/syscfg.h>
#include "cosa_ssam_apis.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <netinet/ether.h>

static const unsigned char rndkey[32] = {
    0x01, 0x54, 0x40, 0x4a, 0x47, 0x1a, 0xe0, 0xab,
    0x04, 0x7f, 0xc5, 0x63, 0x65, 0x14, 0x6e, 0x82,
    0x04, 0x23, 0x12, 0x9d, 0xf9, 0xc3, 0x3a, 0x9d,
    0xff, 0xee, 0xeb, 0x71, 0x45, 0xdc, 0x89, 0xbb,
};

static int get_erouter_mac (struct ether_addr *mac)
{
    FILE *fp;
    char buf[18];
    int rc = -1;

    fp = fopen("/sys/class/net/erouter0/address", "r");
    if (!fp)
        return -1;
    if (!fgets(buf, sizeof(buf), fp))
        goto out;
    if (ether_aton_r(buf, mac))
        rc = 0;
out:
    fclose(fp);

    return rc;
}

#if OPENSSL_VERSION_NUMBER < 0x10010000L

static EVP_ENCODE_CTX *EVP_ENCODE_CTX_new (void)
{
    return (EVP_ENCODE_CTX*)OPENSSL_malloc(sizeof(EVP_ENCODE_CTX));
}

static void EVP_ENCODE_CTX_free (EVP_ENCODE_CTX *ctx)
{
    OPENSSL_free(ctx);
}

#endif

int CosaSetAgentpassword(const char *password)
{
    unsigned char key[32];
    const EVP_CIPHER *cipher = EVP_aes_256_gcm();
    int ivlen = EVP_CIPHER_iv_length(cipher);
    EVP_CIPHER_CTX *ctx = NULL;
    struct ether_addr erouter_mac;
    int rc = -1;
    int i, inlen, outlen, len;
    char *outbuf = NULL;
    char *outbufb64 = NULL;

    inlen = strlen(password);
    outbuf = malloc(inlen + ivlen + 16 + EVP_MAX_BLOCK_LENGTH);
    if (!outbuf)
        goto out;
    outbufb64 = malloc(((inlen + ivlen + 16 + EVP_MAX_BLOCK_LENGTH + 2) / 3) * 4 + 1);
    if (!outbufb64)
        goto out;

    if (get_erouter_mac(&erouter_mac))
        goto out;

    if (RAND_pseudo_bytes(outbuf, ivlen) < 0)
        goto out;

    memcpy(key, rndkey, 32);
    for (i = 0; i < 6; i++)
        key[i] ^= erouter_mac.ether_addr_octet[i];

    ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        goto out;
    if (EVP_EncryptInit(ctx, cipher, key, outbuf) != 1)
        goto out;
    outlen = ivlen + 16;
    if (EVP_EncryptUpdate(ctx, outbuf + outlen, &len, password, inlen) != 1)
        goto out;
    outlen += len;
    if (EVP_EncryptFinal(ctx, outbuf + outlen, &len) != 1)
        goto out;
    outlen += len;
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, outbuf + ivlen);
    outbufb64[EVP_EncodeBlock(outbufb64, outbuf, outlen)] = 0;

    printf("%s\n", outbufb64);

    if (syscfg_set_commit(NULL, "ssam_agentpasswd", outbufb64) != 0) {
        AnscTraceWarning(("Error in syscfg_set for ssam_agentpasswd\n"));
        goto out;
    }

    rc = 0;

out:
    memset(key, 0, sizeof(key));
    if (ctx)
        EVP_CIPHER_CTX_free(ctx);
    if (outbuf)
        free(outbuf);
    if (outbufb64)
        free(outbufb64);
    return rc;
}

void CosaGetAgentpassword(char **output, unsigned int *output_size)
{
    unsigned char key[32];
    const EVP_CIPHER *cipher = EVP_aes_256_gcm();
    int ivlen = EVP_CIPHER_iv_length(cipher);
    EVP_CIPHER_CTX *ctx = NULL;
    struct ether_addr erouter_mac;
    int i, inlen, outlen, len;
    char *inbuf = NULL;
    char *outbuf = NULL;
    EVP_ENCODE_CTX *b64 = NULL;
    char input_b64[128];

    *output = NULL;

    if (syscfg_get(NULL, "ssam_agentpasswd", input_b64, sizeof(input_b64)) != 0)
        goto out;

    inlen = strlen(input_b64);
    inbuf = malloc(inlen);
    if (!inbuf)
        goto out;
    b64 = EVP_ENCODE_CTX_new();
    if (!b64)
        goto out;
    EVP_DecodeInit(b64);
    if (EVP_DecodeUpdate(b64, inbuf, &len, input_b64, inlen) < 0)
        goto out;
    inlen = len;
    if (EVP_DecodeFinal(b64, inbuf + inlen, &len) != 1)
        goto out;
    inlen += len;
    outbuf = malloc(inlen + EVP_MAX_BLOCK_LENGTH + 1);
    if (!outbuf)
        goto out;

    if (get_erouter_mac(&erouter_mac))
        goto out;

    memcpy(key, rndkey, 32);
    for (i = 0; i < 6; i++)
        key[i] ^= erouter_mac.ether_addr_octet[i];

    ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        goto out;
    if (EVP_DecryptInit(ctx, cipher, key, inbuf) != 1)
        goto out;
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, inbuf + ivlen);
    if (EVP_DecryptUpdate(ctx, outbuf, &len, inbuf + ivlen + 16, inlen - ivlen - 16) != 1)
        goto out;
    outlen = len;
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, inbuf + ivlen);
    if (EVP_DecryptFinal(ctx, outbuf + outlen, &len) != 1)
        goto out;
    outlen += len;
    outbuf[outlen] = 0;
    *output = outbuf;
    *output_size = outlen;

out:
    memset(key, 0, sizeof(key));
    if (ctx)
        EVP_CIPHER_CTX_free(ctx);
    if (inbuf)
        free(inbuf);
    if (outbuf && !*output)
        free(outbuf);
    if (b64)
        EVP_ENCODE_CTX_free(b64);
}

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
    if (strcmp(buf, "0") != 0) {
        return;
    }

    if (access("/var/sam/.sam.pid", F_OK) == 0) {
        return;
    }

    syscfg_get(NULL, "Customer_Index", buf, sizeof(buf));
    if (buf[0] != 0) {
        cid = atoi(buf);
    }
    else {
        cid = 0;
    }

    if (access("/var/sam", F_OK) != 0) {
        if (mkdir("/var/sam", 0777) != 0) {
            return;
        }
        if (system("mount -t tmpfs -o size=32M tmpfs /var/sam") != 0) {
            return;
        }

        /* Make SSAM specific libnl3.so available in /var/sam */
        if (symlink("/opt/ssam/lib/libnl3.so", "/var/sam/libnl3.so") != 0) {
            return;
        }
    }

    if (access("/var/sam_loader", F_OK) != 0) {
        if (mkdir("/var/sam_loader", 0777) != 0) {
            return;
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
                        "mount -o remount,ro /var/sam_loader");

        if (system(cmd) != 0) {
            return;
        }
    }

    /* Max size of Device.X_LGI-COM_DigitalSecurity.ProvisionedEnvironment is 256 (+1?) */

    syscfg_get(NULL, "ssam_provisionedenv", cmd, sizeof(cmd));
    if (cmd[0]) {
        FILE *fp = fopen("/var/tmp/environment", "w");
        if (fp != NULL) {
            fputs(cmd, fp);
            fclose(fp);
        }
    }

    p = cmd;
    p += sprintf(p, "/usr/bin/sam");

    if (cid == 9) {
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
    unlink("/var/sam/.sam.pid");
    unlink("/var/sam/agent_version");
    unlink("/var/sam/status");
}
