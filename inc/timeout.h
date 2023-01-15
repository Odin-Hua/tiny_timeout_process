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

int32_t unit_timeout_init(void);
int32_t unit_timeout_event_add(uint32_t id, TIMEOUT_SEC_e time, int32_t (*callback)(void* arg));
int32_t unit_timeout_event_delete(uint32_t id);

#endif
