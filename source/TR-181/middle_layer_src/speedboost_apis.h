#ifndef _COSA_SPEEDBOOST_DML_H
#define _COSA_SPEEDBOOST_DML_H


#include <pthread.h>
#include <stdbool.h>

#define WAIT_TIME_TO_TRIGGER_RA_RESTART 15

#define MAC_ADDR_SIZE               18
#define MAX_MAC_ADDR_COUNT          10

typedef struct _ClientInfoOfSpeedBoost_
{
    int iNumOfConfigOfClients;
    int iNumOfActiveClients;
    char cCurrentDeviceList [MAX_MAC_ADDR_COUNT][MAC_ADDR_SIZE];
}ClientInfoOfSpeedBoost;

void trigger_ra_service_restart();
void* monitor_thread_for_ra_restart(void* arg);

typedef struct SpeedBoost
{
    bool pvd_enabled;
    bool pvd_h_flag ;
    int pvd_delay;
    int pvd_seqNum;
    char pvd_fqdn[256];
    ClientInfoOfSpeedBoost sClientInfoSpeedBoost;
}SpeedBoost;

typedef enum {
    CURRENT_SPEEDBOOST_VALUE = 0,
    CURRENT_NORMAL_SPEEDBOOST_VALUE
} SpeedBoostType;

int getNumOfConfiguredClients(void);

int getNumOfActiveClients(void);

bool getActiveDevicesListOfSpeedBoost(char * pActiveDevicesList, int iLen);


void initializeSpeedBoostStructVal();

int processSpeedBoostValue(const char* str, const char* prev_val, char* target_value, size_t target_size, SpeedBoostType boostType);
#endif
