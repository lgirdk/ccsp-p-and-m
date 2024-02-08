#ifndef MOCK_COMMON_UTIL_H
#define MOCK_COMMON_UTIL_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <rbus.h>
#include <stdio.h>
#include "ansc_platform.h"
#include <iostream>
#include <pthread.h>
#define RBUS_WAN_IP

extern "C"
{
#include "cosa_rbus_handler_apis.h"
#include "cosa_common_util.h"
}

class rbusCommonUtilInterface {
public:
        virtual ~rbusCommonUtilInterface () {}
        virtual rbusError_t publishWanIpAddr(char*, char*, char* ) = 0;
        virtual int CcspBaseIf_setParameterValues(void* ,const char* ,char* ,int ,unsigned int, parameterValStruct_t*,int ,dbus_bool,char **) = 0;
        virtual FILE* fopen(const char*, const char*) = 0;
        virtual int fclose(FILE *) = 0;
        virtual void free_args_struct(arg_struct_t *) = 0;
        virtual int pthread_detach(pthread_t) = 0;
};

class rbusCommonUtilMock: public rbusCommonUtilInterface {
public:
        virtual ~rbusCommonUtilMock() {}
        MOCK_METHOD3(publishWanIpAddr, rbusError_t(char*, char*, char*));
        MOCK_METHOD9(CcspBaseIf_setParameterValues , int(void* ,const char* ,char* ,int ,unsigned int, parameterValStruct_t*,int ,dbus_bool,char **));
        MOCK_METHOD2(fopen , FILE *(const char*, const char*));
        MOCK_METHOD1(fclose, int (FILE *));
        MOCK_METHOD1(free_args_struct, void(arg_struct_t *));
        MOCK_METHOD1(pthread_detach, int(pthread_t));
};
#endif

