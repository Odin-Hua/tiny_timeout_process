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
            unit_timeout_event_add(TIMEOUT_ID_1, TIMEOUT_SEC_0_5, callback);
            unit_timeout_event_add(TIMEOUT_ID_2, TIMEOUT_SEC_1_0, callback);
            gettimeofday(&tval, NULL);
            flag = 1;
        }
        if (flag == 1) {
            /*not timeout*/
            unit_timeout_event_add(TIMEOUT_ID_3, TIMEOUT_SEC_0_5, callback);
            unit_timeout_event_add(TIMEOUT_ID_4, TIMEOUT_SEC_1_0, callback);
            flag = 2;
        }
        if (flag == 2) {
            usleep(400*1000);
            unit_timeout_event_delete(TIMEOUT_ID_3);
            usleep(400*1000);
            unit_timeout_event_delete(TIMEOUT_ID_4);
            sleep(3);
            flag = 0;
            printf("\n====================\n");
        }
        sleep(1);
    }
    return 0;
}

