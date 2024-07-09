/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2024 RDK Management
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
***************************************************************/

#include <stddef.h>
#include <msgpack.h>
#include "helpers.h"
#include "ansc_platform.h"
#include "speedBoostDoc.h"
#include "speedboost_webconfig_apis.h"

static int processSpeedBoostDoc( speedBoostDoc_t *pSpeedBoostDoc, int iNum, ... );

speedBoostDoc_t* speedBoostDocConvert( const void *pBuf, size_t iLen)
{
    CcspTraceInfo(("%s:%d, Entry\n", __FUNCTION__, __LINE__));
    if (NULL == pBuf || 0 == iLen)
    {
        CcspTraceError(("%s:%d, NULL parameter passed\n", __FUNCTION__, __LINE__));
        return NULL;
    }
    return helper_convert(pBuf, iLen, sizeof(speedBoostDoc_t), XM_SPEED_BOOST_SUBDOC,
                            MSGPACK_OBJECT_MAP, true,
                           (process_fn_t) processSpeedBoostDoc,
                           (destroy_fn_t) speedBoostDocDestroy );
}


/* See webconfig_scheduler_doc.h for details. */
void speedBoostDocDestroy( speedBoostDoc_t *pSpeedBoostDoc)
{
    if( NULL != pSpeedBoostDoc )
    {
        if( NULL != pSpeedBoostDoc->pSchedulerInfo )
        {
            freeScheduleInfo(pSpeedBoostDoc->pSchedulerInfo);
            pSpeedBoostDoc->pSchedulerInfo = NULL;
        }
        if( NULL != pSpeedBoostDoc->pSubDocName )
        {
            free( pSpeedBoostDoc->pSubDocName );
        }
        free( pSpeedBoostDoc );
    }
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static bool nameMatch(msgpack_object *pMsgPackObjKey, const char *pName)
{
    bool bRes = (0 == strncmp(pMsgPackObjKey->via.str.ptr, pName, pMsgPackObjKey->via.str.size));
    return bRes;
}

static int createInfoActionsTable(schedule_info_t *pScheduleInfo, size_t iCount)
{
    pScheduleInfo->actions = (char **) malloc(iCount * sizeof(char *));
    if (NULL == pScheduleInfo->actions)
    {
        return -1;
    }
    pScheduleInfo->actions_size = iCount;

    memset(pScheduleInfo->actions, 0, iCount * sizeof(char *));

    return 0;
}

input_t* createScheduleInput(size_t iActionExecCount)
{
    input_t *pScheduleInfo = NULL;
    size_t           iSize = 0;
    size_t    iMaxActions  = 255;

    if (iActionExecCount > iMaxActions)
    {
        CcspTraceError(("create_schedule_input() Error Request %d exceeds maximum %d\n",
                     iActionExecCount, iMaxActions));
        return pScheduleInfo;
    }

    iSize = sizeof(input_t) + iActionExecCount * sizeof(uint32_t);

    pScheduleInfo = (input_t*) malloc( iSize );
    if( NULL != pScheduleInfo )
    {
        memset( pScheduleInfo, 0, iSize );
        pScheduleInfo->action_count = iActionExecCount;
    }
    return pScheduleInfo;
}

static int processMap(msgpack_object_map *pMsgPackObjMap, input_t **ppScheduleInput)
{
    uint32_t        ui32Size              = 0;
    msgpack_object * pMsgPackObjKey       = NULL;
    msgpack_object * pMsgPackObjVal       = NULL;
    msgpack_object_kv * pMsgPackObjKeyVal = NULL;
    uint32_t      ui32Count               = 0;
    time_t        entryTime               = 0;
    int           iRetVal                 = 0;

    if ((NULL == ppScheduleInput) || (NULL == pMsgPackObjMap))
    {
        CcspTraceError(("%s:%d, Invalid input parameters\n", __FUNCTION__, __LINE__));
        return -1;
    }

    ui32Size             = pMsgPackObjMap->size;
    pMsgPackObjKey       = &pMsgPackObjMap->ptr->key;
    pMsgPackObjVal       = &pMsgPackObjMap->ptr->val;
    pMsgPackObjKeyVal    = pMsgPackObjMap->ptr;

    *ppScheduleInput = NULL;

    for (ui32Count = 0; ui32Count < ui32Size; ui32Count++)
    {
        if (MSGPACK_OBJECT_STR == pMsgPackObjKey->type && MSGPACK_OBJECT_POSITIVE_INTEGER == pMsgPackObjVal->type && (nameMatch(pMsgPackObjKey, UNIX_TIME_STR) || nameMatch(pMsgPackObjKey, RELATIVE_TIME_STR)))
        {
            entryTime = pMsgPackObjVal->via.u64;
        }
        else if (pMsgPackObjKey->type == MSGPACK_OBJECT_STR && pMsgPackObjVal->type == MSGPACK_OBJECT_NIL)
        {
            *ppScheduleInput = createScheduleInput(0);
        }
        else if (MSGPACK_OBJECT_STR == pMsgPackObjKey->type && MSGPACK_OBJECT_ARRAY == pMsgPackObjVal->type && nameMatch(pMsgPackObjKey, INDEXES_STR))
        {
            msgpack_object *pMsgPackObj = pMsgPackObjVal->via.array.ptr;
            uint32_t ui32ArraySize = 0;

            *ppScheduleInput = createScheduleInput(pMsgPackObjVal->via.array.size);

            if (NULL != (*ppScheduleInput))
            {
                for (; ui32ArraySize < (pMsgPackObjVal->via.array.size); ui32ArraySize++)
                {
                    (*ppScheduleInput)->action_indexes[ui32ArraySize] = pMsgPackObj->via.u64;
                    CcspTraceInfo(("Array Element[%d] = %d, action_index_list[]: %d\n",
                                ui32ArraySize, (uint32_t)pMsgPackObj->via.u64,
                                (*ppScheduleInput)->action_indexes[ui32ArraySize]));
                    pMsgPackObj++;
                }
            }
            else
            {
                iRetVal = -2;
            }
        }
        else
        {
            CcspTraceError(("%s:%d,Unexpected Item in msgpack_object_map\n", __FUNCTION__, __LINE__));
            iRetVal = -1;
            break;
        }

        pMsgPackObjKeyVal++;
        if (NULL == pMsgPackObjKeyVal)
        {
            break;
        }
        pMsgPackObjKey = &pMsgPackObjKeyVal->key;
        pMsgPackObjVal = &pMsgPackObjKeyVal->val;
    }

    if (NULL != *ppScheduleInput)
    {
        (*ppScheduleInput)->time = entryTime;
    }
    if (0 == ui32Size)
    {
        iRetVal = -1;
    }
    return iRetVal;
}

static int decodeScheduleTable(msgpack_object *pMsgPackObjKey, msgpack_object *pMsgPackObjVal, input_t **ppScheduleInput, size_t* pInputSize)
{
    UNUSED_PARAMETER (pMsgPackObjKey);

    if (NULL == pMsgPackObjVal || NULL == ppScheduleInput || NULL == pInputSize)
    {
        CcspTraceError(("%s: Invalid input parameters\n", __FUNCTION__));
        return -1;
    }
    if ( MSGPACK_OBJECT_ARRAY == pMsgPackObjVal->type)
    {
        msgpack_object *pMsgPackObj = pMsgPackObjVal->via.array.ptr;
        int iCount = pMsgPackObjVal->via.array.size;
        int iVar = 0;
        input_t *pScheduleInputTemp = NULL;

        /* An empty list is ok, but an invalid size is an error. */
        if (iCount == 0)
        {
            return 0;
        }
        else if (iCount < 0)
        {
            return -1;
        }

        if (MSGPACK_OBJECT_MAP == pMsgPackObj->type)
        {
            *pInputSize = iCount;
            *ppScheduleInput = (input_t*) malloc(sizeof(input_t) * iCount);
            if (NULL == *ppScheduleInput)
            {
                CcspTraceError(("%s: Memory allocation failed.\n", __FUNCTION__));
                return -1;
            }

            for (iVar = 0; iVar < iCount; iVar++)
            {
                if (0 == processMap(&pMsgPackObj->via.map, &pScheduleInputTemp))
                {
                    (*ppScheduleInput)[iVar].time = pScheduleInputTemp->time;
                    (*ppScheduleInput)[iVar].action_count = pScheduleInputTemp->action_count;
                    for (int j = 0; j < pScheduleInputTemp->action_count; j++)
                    {
                        (*ppScheduleInput)[iVar].action_indexes[j] = pScheduleInputTemp->action_indexes[j];
                    }
                }
                if( NULL != pScheduleInputTemp )
                {
                    free(pScheduleInputTemp);
                }
                pMsgPackObj++;
            }
        }
    }
    return 0;
}

static int decodeActionsTable(msgpack_object *pMsgPackObjKey, msgpack_object *pMsgPackObjVal, schedule_info_t **ppSeheduleInfo)
{
    uint32_t            ui32Var  = 0;
    uint32_t          ui32Count  = 0;
    msgpack_object * pMsgPackObj = NULL;

    UNUSED_PARAMETER (pMsgPackObjKey);

    if (NULL == pMsgPackObjVal || NULL == ppSeheduleInfo)
    {
        CcspTraceError(("decode_actions_table(): Invalid input parameters\n"));
        return -1;
    }
    pMsgPackObj = pMsgPackObjVal->via.array.ptr;
    ui32Count = pMsgPackObjVal->via.array.size;

    if (0 == ui32Count)
    {
        CcspTraceError(("decode_actions_table(): empty action array\n"));
        return -1;
    }

    if (0 != createInfoActionsTable(*ppSeheduleInfo, ui32Count))
    {
        CcspTraceError(("decode_actions_table(): create_info_actions_table() failed\n"));
        return -2;
    }
    for (ui32Var = 0; ui32Var < ui32Count; ui32Var++)
    {
        (*ppSeheduleInfo)->actions[ui32Var] = strndup(pMsgPackObj->via.str.ptr, pMsgPackObj->via.str.size);
        if (NULL == (*ppSeheduleInfo)->actions[ui32Var])
        {
            CcspTraceError(("decode_actions_table(): assigning action strings failed\n"));
            return -1;
        }
        CcspTraceInfo(("%s:%d, actions[%d] = %s\n", __FUNCTION__, __LINE__, ui32Var, (*ppSeheduleInfo)->actions[ui32Var]));
        pMsgPackObj++;
    }
    return 0;
}



/**
 *  Convert the msgpack map into the doc_t structure.
 *
 *  @param s    the entry pointer
 *  @param map  the msgpack map pointer
 *
 *  @return 0 on success, error otherwise
 */
int processSpeedBoostInfo( schedule_info_t * pSchedulerInfo, msgpack_object_map *pMsgPackObjMap )
{
    CcspTraceInfo(("%s:%d, Entry\n", __FUNCTION__, __LINE__));

    int iRetVal = SPEEDBOOST_INFO_OK;

    if (NULL == pSchedulerInfo || NULL == pMsgPackObjMap)
    {
        CcspTraceError(("%s:%d, Invalid input parameters\n", __FUNCTION__, __LINE__));
        return SPEEDBOOST_INFO_ERR;
    }
    msgpack_object_kv *pMsgPackObjKeyVal = pMsgPackObjMap->ptr;
    if (NULL == pMsgPackObjKeyVal)
    {
        CcspTraceError(("%s:%d, pMsgPackObjKeyVal is NULL\n", __FUNCTION__, __LINE__));
        return SPEEDBOOST_INFO_ERR;
    }
    int iSize = pMsgPackObjMap->size;
    msgpack_object *pMsgPackObjKey = &pMsgPackObjKeyVal->key;
    msgpack_object *pMsgPackObjVal = &pMsgPackObjKeyVal->val;

    while ( 0 < iSize--)
    {
        if (NULL == pMsgPackObjKey || NULL == pMsgPackObjVal)
        {
            CcspTraceError(("%s:%d, pMsgPackObjKey or pMsgPackObjVal is NULL\n", __FUNCTION__, __LINE__));
            return SPEEDBOOST_INFO_ERR;
        }
        if ((NULL == pMsgPackObjKey->via.str.ptr)|| (0 == pMsgPackObjKey->via.str.size))
        {
            CcspTraceError(("%s:%d, pMsgPackObjKey->via.str.ptr is NULL\n", __FUNCTION__, __LINE__));
            return SPEEDBOOST_INFO_ERR;
        }
        if (0 == strncmp(pMsgPackObjKey->via.str.ptr, ABSOLUTE_SCHEDULE, pMsgPackObjKey->via.str.size))
        {
            CcspTraceInfo(("%s:%d, Found %s\n", __FUNCTION__, __LINE__, ABSOLUTE_SCHEDULE));
            if (0 != decodeScheduleTable(pMsgPackObjKey, pMsgPackObjVal, &pSchedulerInfo->absolute, &pSchedulerInfo->absolute_size))
            {
                CcspTraceError(("%s:%d, decodeScheduleTable failed\n", __FUNCTION__, __LINE__));
                iRetVal = SPEEDBOOST_INFO_ABSOLUTE_ERR;
            }
        }
        else if (0 == strncmp(pMsgPackObjKey->via.str.ptr, XM_SPEED_BOOST_RULES, pMsgPackObjKey->via.str.size))
        {
            CcspTraceInfo(("%s:%d, Found %s\n", __FUNCTION__, __LINE__, XM_SPEED_BOOST_RULES));
            if(0 != decodeActionsTable(pMsgPackObjKey, pMsgPackObjVal, &pSchedulerInfo))
            {
                CcspTraceError(("%s:%d, decodeActionsTable failed\n", __FUNCTION__, __LINE__));
                if (NULL != pSchedulerInfo->actions)
                {
                    free(pSchedulerInfo->actions);
                    pSchedulerInfo->actions = NULL;
                }
                iRetVal = SPEEDBOOST_INFO_ACTIONS_ERR;
            }
        }
        else
        {
            CcspTraceError(("%s:%d, ignoring unknown object \n", __FUNCTION__, __LINE__));
        }
        pMsgPackObjKeyVal++;
        pMsgPackObjKey = &pMsgPackObjKeyVal->key;
        pMsgPackObjVal = &pMsgPackObjKeyVal->val;
    }

    if( 1 & iSize ) {
    } else {
        errno = HELPERS_OK;
    }

    return (0 == iSize) ? 0 : iRetVal;
}

static int processSpeedBoostDoc( speedBoostDoc_t *pSpeedBoostDoc, int iNum, ... )
{
    CcspTraceInfo(("%s:%d, Entry\n", __FUNCTION__, __LINE__));

    if ((NULL == pSpeedBoostDoc) || (iNum < 1))
    {
        CcspTraceError(("%s:%d, Invalid input parameters\n", __FUNCTION__, __LINE__));
        return -1;
    }
    //To access the variable arguments use va_list
    va_list vaList;
    va_start(vaList, iNum);//start of variable argument loop

    msgpack_object *pMsgPackObj = va_arg(vaList, msgpack_object *);//each usage of va_arg fn argument iterates by one time

    if (NULL == pMsgPackObj)
    {
        CcspTraceError(("%s:%d, pMsgPackObj is NULL\n", __FUNCTION__, __LINE__));
        return -1;
    }
    msgpack_object_map *pMsgPackObjMap = &pMsgPackObj->via.map;

    msgpack_object *pMsgPackObj1 = va_arg(vaList, msgpack_object *);
    if (NULL == pMsgPackObj1)
    {
        CcspTraceError(("%s:%d, pMsgPackObj1 is NULL\n", __FUNCTION__, __LINE__));
        return -1;
    }

    pSpeedBoostDoc->pSubDocName = strndup( pMsgPackObj1->via.str.ptr, pMsgPackObj1->via.str.size );

    msgpack_object *pMsgPackObj2 = va_arg(vaList, msgpack_object *);
    if (NULL == pMsgPackObj2)
    {
        CcspTraceError(("%s:%d, pMsgPackObj2 is NULL\n", __FUNCTION__, __LINE__));
        return -1;
    }
    pSpeedBoostDoc->ui32Version = (uint32_t) pMsgPackObj2->via.u64;

    msgpack_object *pMsgPackObj3 = va_arg(vaList, msgpack_object *);
    if (NULL == pMsgPackObj3)
    {
        CcspTraceError(("%s:%d, pMsgPackObj3 is NULL\n", __FUNCTION__, __LINE__));
        return -1;
    }
    pSpeedBoostDoc->ui16TranscationId = (uint16_t) pMsgPackObj3->via.u64;
    va_end(vaList);//End of variable argument loop

    pSpeedBoostDoc->pSchedulerInfo = (schedule_info_t *) malloc( sizeof(schedule_info_t) );
    if( NULL == pSpeedBoostDoc->pSchedulerInfo )
    {
        CcspTraceError(("%s:%d, Memory allocation failed\n", __FUNCTION__, __LINE__));
        return -1;
    }
    memset( pSpeedBoostDoc->pSchedulerInfo, 0, sizeof(schedule_info_t) );

    int iRet = processSpeedBoostInfo( pSpeedBoostDoc->pSchedulerInfo, pMsgPackObjMap );
    if (SPEEDBOOST_INFO_OK != iRet)
    {
        CcspTraceError(("%s:%d, processSpeedBoostInfo failed\n", __FUNCTION__, __LINE__));
        if (SPEEDBOOST_INFO_ACTIONS_ERR == iRet)
        {
            CcspTraceWarning(("processSpeedBoostInfo empty actions, err:%d\n", SPEEDBOOST_INFO_ACTIONS_ERR));
            pSpeedBoostDoc->pSchedulerInfo = NULL;
            return 0;
        }
        return -1;
    }
    CcspTraceInfo(("%s:%d, Exit\n", __FUNCTION__, __LINE__));
    return 0;
}
