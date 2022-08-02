/*********************************************************************************
 * Copyright 2022 Liberty Global B.V.
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
 *********************************************************************************/

#include "cosa_lgi_iperf_apis.h"
#include "cosa_deviceinfo_apis.h"

#include <pthread.h>
#include <pty.h>

/* Called under lock */
static int pclose_pid(FILE *fp, pid_t *pid)
{
    int stat;

    fclose(fp);
    while (waitpid(*pid, &stat, 0) == -1)
    {
        if (errno != EINTR)
        {
            stat = -1;
            break;
        }
    }
    *pid = -1;

    return WEXITSTATUS(stat);
}

/* Called under lock */
/* A customized popen is needed to
 * get the pid of the forked process
 */
static FILE *popen_pid(PCOSA_DATAMODEL_LGI_IPERF pObj)
{
    int pid;
    int masterFd;

    pid = forkpty(&masterFd, NULL, NULL, NULL);
    if (pid == -1)
    {
        CcspTraceError(("forkpty failure, %s\n", strerror(errno)));

        return NULL;
    }
    else if (pid == 0)
    {
        /* Child process */
        int i, count;
        BOOL argCandidate = FALSE;
        char **argv, *rest, *token;
        char *cmd = pObj->command;

        /* tokenize the iperf arguments. Count the word and don't assume
         * there is only one space between the arguments
         */
		for (i = 0, count = 0; cmd[i]; i++)
		{
			if (cmd[i] == ' ' || cmd[i] == '\t')
            {
				argCandidate = TRUE;
                continue;
            }
            if (argCandidate)
            {
                argCandidate = FALSE;
                count++;
            }
        }
        if (strlen(cmd) > 0)
            count++;

        /* argv includes count + 2 arguments: #count number of arguments
         * + binary name (iperf) + NULL argument as execvp requires
         */
        argv = calloc(1, sizeof(char *) * (count + 2));
        argv[0] = UTILITY_NAME;
        for (i = 1, token = strtok_r(cmd, " ", &rest); token != NULL; token = strtok_r(NULL, " ", &rest), i++)
        {
            argv[i] = token;
        }

        execvp(argv[0], argv);
    }

    pObj->iperfPid = pid;

    return fdopen(masterFd, "r");
}

/* Called under lock */
static void detect_iperf_state(PCOSA_DATAMODEL_LGI_IPERF pObj, const char *line, BOOL *isCompleted)
{
    /* TODO: Fix connection detection */
    if (strstr(line, "connected to"))
    {
        pObj->cancelTimerThread = TRUE;
        pthread_cond_signal(&pObj->cond);
    }
    else if (strstr(line, "Connection refused"))
    {
        CosaDmlIperfSetDiagnosticsState(pObj, "Error_ConnectionRefused", FALSE);
    }
    else if (strstr(line, "Invalid argument"))
    {
        CosaDmlIperfSetDiagnosticsState(pObj, "Error_CannotResolveHostname", FALSE);
    }
    else if (strstr(line, "iperf Done."))
    {
        *isCompleted = TRUE;
    }
}

static void *cpuThread(void *argp)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = argp;
    ULONG cpuUsage = 0;
    ULONG count = 0;
    ULONG sum = 0;

    CcspTraceInfo(("Creating cpuThread\n"));

    pthread_mutex_lock(&pObj->lock);
    pObj->averageCPUUsage = 0;
    pObj->peakCPUUsage = 0;
    pthread_mutex_unlock(&pObj->lock);

    while (!pObj->cancelCpuThread)
    {
        cpuUsage = COSADmlGetCpuUsage();
        sum += cpuUsage;
        count++;

        pthread_mutex_lock(&pObj->lock);
        pObj->averageCPUUsage = sum / count;
        if (cpuUsage > pObj->peakCPUUsage)
            pObj->peakCPUUsage = cpuUsage;
        pthread_mutex_unlock(&pObj->lock);

    }

    CcspTraceInfo(("Destroying cpuThread\n"));

    return NULL;
}

static void *timerThread(void *argp)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = argp;
    struct timespec ts;

    CcspTraceInfo(("Creating TimerThread\n"));

    pthread_mutex_lock(&pObj->lock);

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += pObj->connectionTimeout;

    while (!pObj->cancelTimerThread)
    {
        if (pthread_cond_timedwait(&pObj->cond, &pObj->lock, &ts) == ETIMEDOUT)
        {
            if (pObj->iperfPid > 0)
            {
                CcspTraceError(("iPerf client connection failure. Test timed out\n"));
                CosaDmlIperfSetDiagnosticsState(pObj, "Error_Timeout", FALSE);
                kill(pObj->iperfPid, SIGTERM);
            }
            break;
        }
    }
    pthread_mutex_unlock(&pObj->lock);

    CcspTraceInfo(("Destroying TimerThread\n"));

    return NULL;
}

static void *iperfThread(void *argp)
{
    PCOSA_DATAMODEL_LGI_IPERF pObj = argp;
    BOOL isCompleted = FALSE;
    FILE *fp;
    int idx = 0, size = 0;
    int err;
    char line[512];
    pthread_t timerTid, cpuTid;

    pthread_mutex_lock(&pObj->lock);

    if (pObj->markedForDestruction)
    {
        pthread_mutex_unlock(&pObj->lock);
        return NULL;
    }

    /* Create iPerf process */
    fp = popen_pid(pObj);
    if (!fp)
    {
        CcspTraceError(("popen failed\n"));
        CosaDmlIperfSetDiagnosticsState(pObj, "Error_Other", FALSE);
        pthread_mutex_unlock(&pObj->lock);
        return NULL;
    }

    /* Reset result buffer */
    if (pObj->result)
    {
        free(pObj->result);
        pObj->result = NULL;
    }

    /* Create CPU thread */
    pObj->cancelCpuThread = FALSE;
    err = pthread_create(&cpuTid, NULL, &cpuThread, pObj);
    if (err)
    {
        CcspTraceError(("pthread_create for CPU stat tracker is failed: %s\n", strerror(err)));
        pthread_mutex_lock(&pObj->lock);
        CosaDmlIperfSetDiagnosticsState(pObj, "Error_Other", FALSE);
        pthread_mutex_unlock(&pObj->lock);
        return NULL;
    }

    /* Create timer thread */
    pObj->cancelTimerThread = FALSE;
    pthread_mutex_unlock(&pObj->lock);

    err = pthread_create(&timerTid, NULL, &timerThread, pObj);
    if (err)
    {
        CcspTraceError(("pthread_create for timeout timer is failed: %s\n", strerror(err)));
        pthread_mutex_lock(&pObj->lock);
        CosaDmlIperfSetDiagnosticsState(pObj, "Error_Other", FALSE);
        pObj->cancelCpuThread = TRUE;
        pthread_mutex_unlock(&pObj->lock);
        pthread_join(cpuTid, NULL);
        return NULL;
    }

    /* Parse iperf output */
    CcspTraceInfo(("Start parsing iPerf output with pid:%d\n", pObj->iperfPid));
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        int lineLen = strlen(line);

        pthread_mutex_lock(&pObj->lock);

        detect_iperf_state(pObj, line, &isCompleted);

        /* Concatenate iperf output to the result string */
        size += (idx == 0 ? lineLen + 1 : lineLen);
        pObj->result = realloc(pObj->result, sizeof(char) * (size));
        memcpy(pObj->result + idx, line, lineLen + 1);
        idx += lineLen;

        pthread_mutex_unlock(&pObj->lock);
    }

    /* iPerf finished */
    pthread_mutex_lock(&pObj->lock);
    CcspTraceInfo(("iPerf finished with pid:%d\n", pObj->iperfPid));

    pclose_pid(fp, &pObj->iperfPid);

    /* Destroy timer thread */
    pObj->cancelTimerThread = TRUE;
    pthread_cond_signal(&pObj->cond);

    /* Destroy CPU thread */
    pObj->cancelCpuThread = TRUE;

    if (isCompleted)
    {
        /* iPerf session is gracefully completed */
        CosaDmlIperfSetDiagnosticsState(pObj, "Completed", FALSE);
    }
    else
    {
        /* Test is not completed. Invalidate CPU stats */
        pObj->peakCPUUsage = 0;
        pObj->averageCPUUsage = 0;

        if (strcmp(pObj->diagnosticsState, "Requested") == 0)
        {
            /* If test is failed due to an unknown error, set state to Error_Other  */
            CosaDmlIperfSetDiagnosticsState(pObj, "Error_Other", FALSE);
        }
    }

    pthread_mutex_unlock(&pObj->lock);

    pthread_join(timerTid, NULL);
    pthread_join(cpuTid, NULL);
}

/* Called under lock */
void CosaDmlIperfSetDiagnosticsState(PCOSA_DATAMODEL_LGI_IPERF pObj, const char *state, BOOL external)
{
    CcspTraceInfo(("Setting diagnostics state: %s -> %s\n", pObj->diagnosticsState, state));

    strncpy(pObj->diagnosticsState, state, MAX_STATE_LEN);
    if (external)
    {
        pObj->diagnosticsStateChanged = TRUE;
    }
}

/* Called under lock */
ANSC_STATUS CosaDmlIperfGetConnectionTimeout(ULONG *puValue)
{
    char buf[8];

    syscfg_get(NULL, "iperf_connection_timeout", buf, sizeof(buf));
    *puValue = atoi(buf);

    return ANSC_STATUS_SUCCESS;
}

/* Called under lock */
ANSC_STATUS CosaDmlIperfSetConnectionTimeout(PCOSA_DATAMODEL_LGI_IPERF pObj, ULONG uValue)
{
    if (syscfg_set_u_commit(NULL, "iperf_connection_timeout", uValue) != 0)
        return ANSC_STATUS_FAILURE;

    pObj->connectionTimeout = uValue;

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlCreateIperfThread(PCOSA_DATAMODEL_LGI_IPERF pObj)
{
    int err;

    pthread_mutex_lock(&pObj->lock);
    pObj->markedForDestruction = FALSE;
    pthread_mutex_unlock(&pObj->lock);

    err = pthread_create(&pObj->iperfTid, NULL, &iperfThread, pObj);

    return err ? ANSC_STATUS_FAILURE : ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlDestroyIperfThread(PCOSA_DATAMODEL_LGI_IPERF pObj)
{
    pthread_t tid;

    pthread_mutex_lock(&pObj->lock);

    /* Back to back test requests may cause a race condition.
     * "Destroy thread" may be called before the previous
     * "Create thread" cycle successfully starts the iperf, causing
     * pthread_join to wait for iperf completion unnecessarily while
     * destroying the iPerf thread. Below boolean is added as a sync
     * method between threads to eliminate the aforementioned race condition.
     */
    pObj->markedForDestruction = TRUE;

    CcspTraceInfo(("%s:%d Destroying iPerf thread\n", __func__, __LINE__));

    if (pObj->iperfPid > 0)
    {
        CcspTraceInfo(("Killing iperf with pid:%d\n", pObj->iperfPid));
        kill(pObj->iperfPid, SIGTERM);
    }

    pthread_mutex_unlock(&pObj->lock);

    /* Using pthread_join as a sync. mechanism between destroying and creating the iPerf thread */
    pthread_join(pObj->iperfTid, NULL);
    CcspTraceInfo(("Joined iPerf thread\n"));

    return ANSC_STATUS_SUCCESS;
}

/*
 * state = Requested -> restart Iperf thread
 * state = Canceled -> stop iperf thread if running
 * state = None -> stop iperf thread if running
 */
ANSC_STATUS CosaDmlDiagnosticsStateChangeAction(PCOSA_DATAMODEL_LGI_IPERF pObj)
{
    ANSC_STATUS ret = ANSC_STATUS_SUCCESS;
    BOOL isRequested = FALSE;

    if (strcmp(pObj->diagnosticsState, "Requested") == 0)
    {
        /* cache diagnostics state because CosaDmlDestroyIperfThread may change it. */
        isRequested = TRUE;
    }

    if (pObj->iperfThreadExists)
    {
        ret = CosaDmlDestroyIperfThread(pObj);
        if (ret != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("CosaDmlDestroyIperfThread() error\n"));
            goto out;
        }
    }

    pObj->iperfThreadExists = FALSE;

    if (isRequested)
    {
        CcspTraceInfo(("Creating the iPerf thread\n"));
        ret = CosaDmlCreateIperfThread(pObj);
        if (ret != ANSC_STATUS_SUCCESS)
        {
            CcspTraceError(("CosaDmlCreateIperfThread() error\n"));
            goto out;
        }
        pObj->iperfThreadExists = TRUE;
    }
out:
    return ret;
}
