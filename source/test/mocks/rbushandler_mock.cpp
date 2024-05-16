#include "rbushandler_mock.h"
#include <rbus.h>
#include "ansc_platform.h"

using namespace std;

extern rbusHandlerMock * g_rbusHandlerMock;

#ifdef MOCK_RBUS_HANDLER_PAM
extern "C" rbusError_t sendUpdateEvent(char* event_name, void* eventNewData, void* eventOldData, rbusValueType_t rbus_type)
{
    if (!g_rbusHandlerMock)
    {
        return RBUS_ERROR_BUS_ERROR;
    }
    return g_rbusHandlerMock->sendUpdateEvent(event_name, eventNewData, eventOldData, rbus_type);
}

extern "C" char const* rbusProperty_GetName(rbusProperty_t property)
{
    if (!g_rbusHandlerMock)
    {
        return NULL;
    }
    return g_rbusHandlerMock->rbusProperty_GetName(property);
}

extern "C" char const* GetParamName(char const* path)
{
    if (!g_rbusHandlerMock)
    {
        return NULL;
    }
    return g_rbusHandlerMock->GetParamName(path);
}

extern "C" rbusStatus_t rbus_checkStatus()
{
    if (!g_rbusHandlerMock)
    {
        return RBUS_DISABLED;
    }
    return g_rbusHandlerMock->rbus_checkStatus();
}

extern "C" rbusError_t rbus_open(rbusHandle_t* handle, char const* componentName)
{
    if (!g_rbusHandlerMock)
    {
        return RBUS_ERROR_BUS_ERROR;
    }
    return g_rbusHandlerMock->rbus_open(handle, componentName);
}

extern "C" rbusError_t rbus_regDataElements(rbusHandle_t handle, int numDataElements, rbusDataElement_t *elements)
{
    if (!g_rbusHandlerMock)
    {
        return RBUS_ERROR_BUS_ERROR;
    }
    return g_rbusHandlerMock->rbus_regDataElements(handle, numDataElements, elements);
}

extern "C" bool PAM_Rbus_SyseventInit()
{
    if (!g_rbusHandlerMock)
    {
        return false;
    }
    return g_rbusHandlerMock->PAM_Rbus_SyseventInit();
}
#endif

/* TODO:
#ifdef MOCK_RBUS_HANDLER_PAM_1
extern "C" rbusError_t publishDevCtrlNetMode(uint32_t new_val, uint32_t old_val)
{
    if (!g_rbusHandlerMock)
    {
        return RBUS_ERROR_SUCCESS;
    }
    return g_rbusHandlerMock->publishDevCtrlNetMode(new_val, old_val);
}
#endif
*/

#ifdef MOCK_RBUS_HANDLER_PAM_DML
extern "C" ULONG DeviceInfo_GetParamStringValue(ANSC_HANDLE hInsContext, char* ParamName, char* pValue, ULONG* pUlSize)
{
    if (!g_rbusHandlerMock)
    {
        return -1;
    }
    return g_rbusHandlerMock->DeviceInfo_GetParamStringValue(hInsContext, ParamName, pValue, pUlSize);
}

extern "C" void get_uptime(long *uptime)
{
   if (!g_rbusHandlerMock)
   {
        return -1;
   }
   return g_rbusHandlerMock->get_uptime(uptime);
}

extern "C" rbusError_t rbusEvent_Publish(rbusHandle_t handle, rbusEvent_t* eventData)
{
   if (!g_rbusHandlerMock)
   {
        return RBUS_ERROR_BUS_ERROR;
   }
   return g_rbusHandlerMock->rbusEvent_Publish(handle, eventData);
}
#endif


