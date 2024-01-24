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
#ifndef __MANAGEDWIFI_HELPERS_H__
#define __MANAGEDWIFI_HELPERS_H__
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <trower-base64/base64.h>
#include <msgpack.h>

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */
#define match(p, s) strncmp((p)->key.via.str.ptr, s, (p)->key.via.str.size)
#define member_size(type, member) sizeof(((type *)0)->member)
/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
enum {
    MW_HELPERS_OK = 0,
    MW_HELPERS_OUT_OF_MEMORY,
    MW_HELPERS_INVALID_FIRST_ELEMENT,
    MW_HELPERS_MISSING_WRAPPER
};
typedef int (*process_fn_t)(void *, int, ...);
typedef void (*destroy_fn_t)(void *);

//for wifidoc append
typedef struct mg_wifi_appenddoc_struct{
    char * subdoc_name;
    uint32_t  version;
    uint16_t   transaction_id;
    size_t *count;
}mg_wifi_appenddoc_t;

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/**
 *  Simple helper function that decodes the msgpack, then checks for a few
 *  sanity items (including an optional wrapper map) before calling the process
 *  argument passed in.  This also allocates the structure for the caller.
 *
 *  @param buf          the buffer to decode
 *  @param len          the length of the buffer in bytes
 *  @param struct_size  the size of the structure to allocate and pass to process
 *  @param wrapper      the optional wrapper to look for & enforce
 *  @param expect_type  the type of object expected
 *  @param optional     if the inner wrapper layer is optional
 *  @param process      the process function to call if successful
 *  @param destroy      the destroy function to call if there was an error
 *
 *  @returns the object after process has done it's magic to it on success, or
 *           NULL on error
 */
void* comp_mw_helper_convert( const void *buf, size_t len,
                      size_t struct_size, const char *wrapper,
                      msgpack_object_type expect_type, bool optional,
                      process_fn_t process,
                      destroy_fn_t destroy );

size_t mg_wifi_pack_appenddoc(const mg_wifi_appenddoc_t *appenddocData,void **data);

char * mg_append_wifi_doc(char * subdoc_name, uint32_t version, uint16_t trans_id, char * blob_data, size_t blob_size);

size_t mg_appendWifiEncodedData( void **appendData, void *encodedBuffer, size_t encodedSize, void *metadataPack, size_t metadataSize );

char * base64wifiblobencoder(char * blob_data, size_t blob_size );
#endif