#ifndef __TIMEOUT_H_
#define __TIMEOUT_H_

#include <stdint.h>

typedef enum {
    TIMEOUT_SEC_0_5 = 0,
    TIMEOUT_SEC_1_0,
    TIMEOUT_SEC_1_5,
    TIMEOUT_SEC_2_0,
    TIMEOUT_SEC_NUM,
} TIMEOUT_SEC_e;

typedef enum {
    TIMEOUT_ID_1 = 0,
    TIMEOUT_ID_2,
    TIMEOUT_ID_3,
    TIMEOUT_ID_4,
    TIMEOUT_ID_NUM,
} TIMEOUT_ID_e;

int32_t unit_timeout_init(void);
int32_t unit_timeout_event_add(TIMEOUT_ID_e id, TIMEOUT_SEC_e time, int32_t (*callback)(void* arg));
int32_t unit_timeout_event_delete(TIMEOUT_ID_e id);

#endif