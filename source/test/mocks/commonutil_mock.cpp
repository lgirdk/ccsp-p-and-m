#include "commonutil_mock.h"

using namespace std;

extern rbusCommonUtilMock * g_rbusCommonUtilMock;

#ifdef MOCK_COMMON_UTIL_PAM
extern "C" rbusError_t publishWanIpAddr(char* event_name, char* new_val, char* old_val)
{
    if (!g_rbusCommonUtilMock)
    {
        return RBUS_ERROR_SUCCESS;
    }
    return g_rbusCommonUtilMock->publishWanIpAddr(event_name, new_val, old_val);
}
#endif

#ifdef MOCK_COMMON_UTIL_PAM_1
extern "C" int CcspBaseIf_setParameterValues
(
    void* bus_handle,
    const char* dst_component_id,
    char* dbus_path,
    int sessionId,
    unsigned int writeID,
    parameterValStruct_t *val,
    int size,
    dbus_bool commit,
    char ** invalidParameterName
)
{
        if(!g_rbusCommonUtilMock)
        {
                return CCSP_FAILURE;
        }
        return g_rbusCommonUtilMock->CcspBaseIf_setParameterValues(bus_handle,dst_component_id,dbus_path,sessionId,writeID,val,size,commit,invalidParameterName);
}

extern "C" int pthread_detach(pthread_t thread)
{
    if (!g_rbusCommonUtilMock)
    {
        return -1;
    }
    return g_rbusCommonUtilMock->pthread_detach(thread);
}

extern "C" void free_args_struct(arg_struct_t *param)
{
    if (!g_rbusCommonUtilMock)
    {
        return;
    }
    return g_rbusCommonUtilMock->free_args_struct(param);
}

extern "C" FILE* fopen(const char* filename, const char* mode)
{
    if (!g_rbusCommonUtilMock)
    {
        return NULL;
    }
    return g_rbusCommonUtilMock->fopen(filename, mode);
}
extern "C" int fclose(FILE * stream)
{
    if(!g_rbusCommonUtilMock)
    {
        return 0;
    }
    return g_rbusCommonUtilMock->fclose(stream);
}
#endif

