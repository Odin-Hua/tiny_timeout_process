#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "timeout.h"
#define SCALE_NUM            (4)
#define EVENT_NUM            (50)
#define MOITOR_TASK_CYCLE_MS (500)
#define HANDLE_TASK_CYCLE_MS (400)
#define MS_TO_US             (1000)
#define INIT_TIMEOUT_SEC     (TIMEOUT_SEC_0_5)

typedef struct {
    int8_t   flag;
    uint32_t id;
    int32_t (*callback)(void* arg);
} TIMEOUT_EVENT_t;

TIMEOUT_EVENT_t g_event[SCALE_NUM][EVENT_NUM] = {0};
TIMEOUT_EVENT_t *p_global_scale;
TIMEOUT_EVENT_t *p_global_timeout;
pthread_t       monitor_thread;
pthread_t       handle_thread;
TIMEOUT_SEC_e   g_timeout_sec = INIT_TIMEOUT_SEC;

static void *monitor_task(void *arg);
static void *handle_task(void *arg);

int32_t unit_timeout_init(void)
{
    int32_t ret;

    p_global_scale     = g_event[INIT_TIMEOUT_SEC];
    p_global_timeout   = g_event[INIT_TIMEOUT_SEC];
    
    ret = pthread_create(&monitor_thread, NULL, monitor_task, NULL);
    if (ret != 0) {
        perror("create monitor thread err: ");
        return -1;
    }

    ret = pthread_create(&handle_thread, NULL, handle_task, NULL);
    if (ret != 0) {
        perror("create handle thread err: ");
        return -1;
    }

    return 0;
}

int32_t unit_timeout_event_add(uint32_t id, TIMEOUT_SEC_e time, int32_t (*callback)(void* arg))
{
    if (time >= (TIMEOUT_SEC_NUM-2)) {
        printf("%s not suppout %d !\n", __func__, time);
        return -1;
    }
    TIMEOUT_EVENT_t *p_event = NULL;
    int32_t num, i;

    num = (g_timeout_sec + time + 2) % TIMEOUT_SEC_NUM;
    // printf("%d %d %d\n", g_timeout_sec, time, num);
    p_event = g_event[num];
    for (i = 0; i < EVENT_NUM; i ++) {
        if (p_event[i].flag == 0) {
            p_event[i].flag = 1;
            p_event[i].id = id;
            p_event[i].callback = callback;
            goto __ADD_SUCCESS;
        }
    }

    printf("%s err!\n", __func__);
    return -1;
__ADD_SUCCESS:
    return 0;
}

int32_t unit_timeout_event_delete(uint32_t id)
{
    int i, j;

    for (i = 0; i < SCALE_NUM; i++) {
        for (j = 0; j < EVENT_NUM; j ++) {
            if (g_event[i][j].flag && g_event[i][j].id == id) {
                g_event[i][j].flag = 0;
            }
        }
    }

    return 0;
}
int32_t task_flag = 1;
static void *monitor_task(void *arg)
{
    int32_t i = 0;

    while (1)
    {
        while (task_flag == 0) {
            usleep(10*MS_TO_US);
        }
        task_flag = 0;
        g_timeout_sec ++;
        if (g_timeout_sec == TIMEOUT_SEC_NUM) {
            g_timeout_sec = TIMEOUT_SEC_0_5;
        }
        p_global_scale   = g_event[g_timeout_sec];
        p_global_timeout = p_global_scale;
        // printf("scale: %x %d\n", p_global_scale, g_timeout_sec);
        task_flag = 1;
        usleep(MOITOR_TASK_CYCLE_MS*MS_TO_US);
    }

    pthread_exit(NULL);
}

static void *handle_task(void *arg)
{
    int32_t i = 0;

    while (1)
    {
        while (task_flag == 0) {
            usleep(10*MS_TO_US);
        }
        task_flag = 0;
        for (i = 0; i < EVENT_NUM; i ++) {
            if (p_global_timeout[i].flag && 
                p_global_timeout[i].callback != NULL) {
                p_global_timeout[i].callback(&p_global_timeout[i].id);
                p_global_timeout[i].flag = 0;
            }
        }
        // printf("timeout: %x\n", p_global_timeout);
        task_flag = 1;
        usleep(HANDLE_TASK_CYCLE_MS*MS_TO_US);
    }

    pthread_exit(NULL);
}