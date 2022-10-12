/*****************************************************************************
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
 ****************************************************************************/

#include <sys/stat.h>
#include <sys/types.h>

#include "cosa_lgi_tunneledstaticip_apis.h"
#include "cosa_lgi_tunneledstaticip_internal.h"


ANSC_HANDLE CosaLgiTunneledStaticIPCreate (void)
{
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) NULL;

    /*
     * We create object by first allocating memory for holding the variables and member functions.
     */
    pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) AnscAllocateMemory(sizeof(COSA_DATAMODEL_LGI_TUNNELEDSTATICIP));

    if (!pMyObject)
    {
        return (ANSC_HANDLE) NULL;
    }

    /*
     * Initialize the common variables and functions for a container object.
     */
    pMyObject->Oid = COSA_DATAMODEL_LGI_TUNNELEDSTATICIP_OID;
    pMyObject->Create = CosaLgiTunneledStaticIPCreate;
    pMyObject->Remove = CosaLgiTunneledStaticIPRemove;
    pMyObject->Initialize = CosaLgiTunneledStaticIPInitialize;

    pMyObject->Initialize ((ANSC_HANDLE) pMyObject);

    return (ANSC_HANDLE) pMyObject;
}

ANSC_STATUS CosaLgiTunneledStaticIPInitialize (ANSC_HANDLE hThisObject)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) hThisObject;

    CosaDmlTunneledStaticIPGetEnable(NULL, &pMyObject->Cfg.Enable);
    CosaDmlTunneledStaticIPGetUsername(NULL, &pMyObject->Cfg.Username);
    CosaDmlTunneledStaticIPGetPassword(NULL, &pMyObject->Cfg.Password);
    CosaDmlTunneledStaticIPGetRadiusInterface(NULL, &pMyObject->Cfg.RadiusInterface);
    CosaDmlTunneledStaticIPGetRadiusAuthServerIPAddr(NULL, &pMyObject->Cfg.RadiusAuthServerIPAddr);
    CosaDmlTunneledStaticIPGetRadiusAuthSecret(NULL, &pMyObject->Cfg.RadiusAuthSecret);
    CosaDmlTunneledStaticIPGetRadiusAuthServerPort(NULL, &pMyObject->Cfg.RadiusAuthServerPort);
    CosaDmlTunneledStaticIPGetNASIdentifier(NULL, &pMyObject->Cfg.NASIdentifier);
    CosaDmlTunneledStaticIPGetRadiusAccServerIPAddr(NULL, &pMyObject->Cfg.RadiusAccServerIPAddr);
    CosaDmlTunneledStaticIPGetRadiusAccSecret(NULL, &pMyObject->Cfg.RadiusAccSecret);
    CosaDmlTunneledStaticIPGetRadiusAccServerPort(NULL, &pMyObject->Cfg.RadiusAccServerPort);

    pMyObject->OldCfg = pMyObject->Cfg;

    CosaDmlTunneledStaticIPRestart(hThisObject);

#ifdef VMB_MODE
    /* TODO: is this correct event to get when erouter is up & running? */
    system("sysevent async ipv4_erouter0_ipaddr /usr/bin/vmbauth.sh");
#endif

    return returnStatus;
}

ANSC_STATUS CosaLgiTunneledStaticIPRemove (ANSC_HANDLE hThisObject)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) hThisObject;

    /* Remove necessary resource */
    
    /* Remove self */
    AnscFreeMemory((ANSC_HANDLE)pMyObject);

    return returnStatus;
}    

ANSC_STATUS CosaDmlTunneledStaticIPRestart (ANSC_HANDLE hThisObject)
{
#ifdef VMB_MODE
    PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP pMyObject = (PCOSA_DATAMODEL_LGI_TUNNELEDSTATICIP) hThisObject;
    FILE *fp;

    mkdir("/tmp/vmb-radius-client", S_IRWXU);

    fp = fopen("/tmp/vmb-radius-client/servers", "w");
    if (!fp)
        return ANSC_STATUS_FAILURE;

    fprintf(fp, "%s %s\n", pMyObject->Cfg.RadiusAuthServerIPAddr, pMyObject->Cfg.RadiusAuthSecret);
    fclose(fp);

    fp = fopen("/tmp/vmb-radius-client/radiusclient.conf", "w");
    if (!fp)
        return ANSC_STATUS_FAILURE;

    fprintf(fp, "nas-identifier %s\n", pMyObject->Cfg.NASIdentifier);
    fprintf(fp, "authserver %s:%d\n", pMyObject->Cfg.RadiusAuthServerIPAddr, pMyObject->Cfg.RadiusAuthServerPort);
    fprintf(fp, "servers /tmp/vmb-radius-client/servers\n");
    fprintf(fp, "dictionary /usr/share/radcli/dictionary\n");
    fprintf(fp, "default_realm\n");
    fprintf(fp, "radius_timeout  10\n");
    fprintf(fp, "radius_retries  3\n");
    fprintf(fp, "bindaddr *\n");
    fclose(fp);

    system("vmbauth.sh");
#endif

    return ANSC_STATUS_SUCCESS;
}
