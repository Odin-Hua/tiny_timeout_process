#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include "timeout.h"

int32_t callback(void* arg)
{
    uint32_t id = (*(uint32_t*)arg);
    printf("%s: 0x%x\n", __func__, id);
    return 0;
}

int main()
{
    int32_t flag = 0;
    struct timeval tval;

    unit_timeout_init();
    sleep(1);
    while (1) {
        if (flag == 0) {
            /*timeout*/
            gettimeofday(&tval, NULL);
            unit_timeout_event_add(0x01, TIMEOUT_SEC_0_5, callback);
            unit_timeout_event_add(0x02, TIMEOUT_SEC_1_0, callback);
            unit_timeout_event_add(0x04, TIMEOUT_SEC_0_5, callback);
            unit_timeout_event_add(0x05, TIMEOUT_SEC_1_0, callback);
            unit_timeout_event_add(0x07, TIMEOUT_SEC_0_5, callback);
            unit_timeout_event_add(0x08, TIMEOUT_SEC_1_0, callback);
            gettimeofday(&tval, NULL);
            flag = 1;
        }
        if (flag == 1) {
            /*not timeout*/
            unit_timeout_event_add(0xf1, TIMEOUT_SEC_0_5, callback);
            unit_timeout_event_add(0xf2, TIMEOUT_SEC_1_0, callback);
            flag = 2;
        }
        if (flag == 2) {
            usleep(400*1000);
            unit_timeout_event_delete(0xf1);
            usleep(400*1000);
            unit_timeout_event_delete(0xf2);
            sleep(3);
            flag = 0;
            printf("\n====================\n");
        }
        sleep(1);
    }
    return 0;
}

