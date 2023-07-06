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
#ifndef __MANAGEDWIFIDOC_H__
#define __MANAGEDWIFIDOC_H__

#include <stdint.h>
#include <stdlib.h>
#include <msgpack.h>


typedef struct
{
    bool mwenable;
    char *   alias;
    bool     dhcp_server_enable;
    char *   lan_ip_address;
    char *   lan_subnet_mask;
    char *   dhcp_start_ip_address;
    char *   dhcp_end_ip_address; 
    char*    leasetime;
    bool     ipv6enable;
    char *   firewall;         
} lanconfig_t;

typedef struct
{
    lanconfig_t * entries;
    size_t    entries_count;	   
} lanconfigdoc_t;

typedef struct
{
    char *name;
    char *value;
    uint32_t   value_size;
    uint16_t type;
} mwparam_t;

typedef struct {
    mwparam_t   *entries;
    size_t      entries_count;
    char *      subdoc_name;
    uint32_t    version;
    uint16_t    transaction_id;
} managedwifidoc_t;

/**
 *  This function converts a msgpack buffer into an managedwifiparam_t structure
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
managedwifidoc_t* managedwifidoc_convert( const void *buf, size_t len );
/**
 *  This function returns a general reason why the conversion failed.
 *
 *  @param errnum the errno value to inspect
 *
 *  @return the constant string (do not alter or free) describing the error
 */
const char* managedwifidoc_strerror( int errnum );
/**
 *  This function destroys an lanconfigdoc_t object.
 *
 *  @param e the lanconfig to destroy
 */
void lanConfigDoc_destroy( lanconfigdoc_t *lwd );

/**
 *  This function converts a msgpack buffer into an lanconfig structure 
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
lanconfigdoc_t* lanconfigdoc_convert( const void *buf, size_t len );

/**
 *  This function destroys an managedwifidoc_t object.
 *
 *  @param e the managedwifidoc to destroy
 */
void connectedbuilding_destroy(managedwifidoc_t *mwd);

#endif
