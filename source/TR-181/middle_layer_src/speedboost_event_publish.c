#include "speedboost_apis.h"
#include "ccsp_trace.h"
#include "cosa_rbus_handler_apis.h"

extern rbusHandle_t handle;

int Publish_PvdValueOnChange(char *dm_event, void *prev_dm_value, void *dm_value, rbusValueType_t rbus_type)
{
    rbusEvent_t event;
    rbusObject_t rdata;
    rbusValue_t Value, preValue, byVal;
    int rc = -1;
    if(dm_event == NULL || dm_value == NULL)
    {
        CcspTraceInfo(("%s %d - Failed publishing\n", __FUNCTION__, __LINE__));
        return rc;
    }
    rbusValue_Init(&Value);
    rbusValue_Init(&preValue);
    switch(rbus_type)
    {
        case RBUS_BOOLEAN:
            CcspTraceInfo(("%s %d - bool dm_event[%s],prev_dm_value[%d],dm_value[%d]\n", __FUNCTION__, __LINE__, dm_event, *(bool*)prev_dm_value, *(bool*)dm_value));
            rbusValue_SetBoolean(Value, *(bool*)dm_value);
            rbusValue_SetBoolean(preValue, *(bool*)prev_dm_value);
        break;
        case RBUS_INT32:
            CcspTraceInfo(("%s %d - dm_event[%s],prev_dm_value[%d],dm_value[%d]\n", __FUNCTION__, __LINE__, dm_event, *(int*)prev_dm_value, *(int*)dm_value));
            rbusValue_SetInt32(Value, *(int*)dm_value);
            rbusValue_SetInt32(preValue, *(int*)prev_dm_value);
        break;
        case RBUS_STRING:
            CcspTraceInfo(("%s %d - dm_event[%s],prev_dm_value[%s],dm_value[%s]\n", __FUNCTION__, __LINE__, dm_event, (char*)prev_dm_value, (char*)dm_value));
            rbusValue_SetString(Value, (char*)dm_value);
            rbusValue_SetString(preValue, (char*)prev_dm_value);
        break;
        default:
            rbusValue_Release(Value);
            rbusValue_Release(preValue);
            return rc;
    }
    rbusValue_Init(&byVal);
    rbusValue_SetString(byVal, RBUS_COMPONENT_NAME);
    rbusObject_Init(&rdata, NULL);
    rbusObject_SetValue(rdata, "value", Value);
    rbusObject_SetValue(rdata, "oldValue", preValue);
    rbusObject_SetValue(rdata, "by", byVal);
    event.name = dm_event;
    event.data = rdata;
    event.type = RBUS_EVENT_VALUE_CHANGED;

    if(rbusEvent_Publish(handle, &event) != RBUS_ERROR_SUCCESS)
    {
        CcspTraceInfo(("%s %d - event publishing failed for type\n", __FUNCTION__, __LINE__));
    }
    else
    {
        CcspTraceInfo(("%s %d - Successfully Published event for event %s \n", __FUNCTION__, __LINE__, dm_event));
        rc = 0;
    }
    rbusValue_Release(Value);
    rbusValue_Release(preValue);
    rbusValue_Release(byVal);
    rbusObject_Release(rdata);
    
    return rc;
}
