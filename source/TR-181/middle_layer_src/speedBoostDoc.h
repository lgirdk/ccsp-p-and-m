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
#ifndef __SPEEDBOOSTDOC_H__
#define __SPEEDBOOSTDOC_H__

#include <stdio.h>
#include "scheduler_interface.h"

#define ABSOLUTE_SCHEDULE "absolute"
#define RELATIVE_TIME_STR "time"
#define UNIX_TIME_STR "unix_time"
#define INDEXES_STR "indexes"

#define UNUSED_PARAMETER(x) (void)(x)

enum {
    SPEEDBOOST_INFO_OK = 0,
    SPEEDBOOST_INFO_ABSOLUTE_ERR,
    SPEEDBOOST_INFO_ACTIONS_ERR,
    SPEEDBOOST_INFO_TIMEZONE_ERR,
    SPEEDBOOST_INFO_ERR
};


typedef struct speedBoostDoc{
    schedule_info_t * pSchedulerInfo;
    char   *          pSubDocName;
    uint32_t          ui32Version;
    uint16_t          ui16TranscationId;
} speedBoostDoc_t;

/**
 *  This function converts a msgpack buffer into an speedBoostDoc_t structure
 *  if possible.
 *
 *  @param buf the buffer to convert
 *  @param len the length of the buffer in bytes
 *
 *  @return NULL on error, success otherwise
 */
speedBoostDoc_t* speedBoostDocConvert( const void *pBuf, size_t iLen );

/**
 *  This function destroys an speedBoostDoc_t object.
 *
 *  @param sd the speedBoostDoc_t to destroy
 */
void speedBoostDocDestroy( speedBoostDoc_t *pSpeedBoostDoc );

/**
 *  allocate a memory for (size of input_t structure * iActionExecCount) of scheduler
 *
 *  @param s  Number of input_t structures of scheduler
 *
 *  @return pointer to allocated input_t structure, error otherwise
 */

input_t* createScheduleInput(size_t iActionExecCount);

/**
 *  Convert the msgpack map into the doc_t structure.
 *
 *  @param s    the entry pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int processSpeedBoostInfo( schedule_info_t * pSchedulerInfo, msgpack_object_map *pMsgPackObjMap );
#endif /*__SPEEDBOOSTDOC_H__s*/
