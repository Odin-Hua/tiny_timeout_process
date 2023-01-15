#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "timeout.h"

#define SCALE_NUM            (4)
#define EVENT_NUM            (50)
#define MOITOR_TASK_CYCLE_MS (500)
#define HANDLE_TASK_CYCLE_MS (400)
#define MS_TO_US             (1000)
#define INIT_TIMEOUT_SEC     (TIMEOUT_SEC_0_5)

typedef struct {
    int8_t   flag;
    TIMEOUT_ID_e id;
    int32_t (*callback)(void* arg);
} TIMEOUT_EVENT_UNIT_t;

typedef struct {
    uint8_t count;
    TIMEOUT_EVENT_UNIT_t unit[EVENT_NUM];
} TIMEOUT_EVENT_t;

TIMEOUT_EVENT_t g_event[SCALE_NUM] = {0};
TIMEOUT_EVENT_t *p_global_scale;
TIMEOUT_EVENT_t *p_global_timeout;
pthread_t       monitor_thread;
pthread_t       handle_thread;
pthread_mutex_t g_thread_mutex;
TIMEOUT_SEC_e   g_timeout_sec = INIT_TIMEOUT_SEC;

static void *monitor_task(void *arg);
static void *handle_task(void *arg);

int32_t unit_timeout_init(void)
{
    int32_t ret;

    p_global_scale     = &g_event[INIT_TIMEOUT_SEC];
    p_global_timeout   = &g_event[INIT_TIMEOUT_SEC];
    
    ret = pthread_mutex_init(&g_thread_mutex, NULL);
    if (ret != 0) {
        printf("timeout mutex init err !\n");
        return -1;
    }

    ret = pthread_create(&monitor_thread, NULL, monitor_task, NULL);
    if (ret != 0) {
        printf("create monitor thread err !\n");
        return -1;
    }

    ret = pthread_create(&handle_thread, NULL, handle_task, NULL);
    if (ret != 0) {
        printf("create handle thread err !\n");
        return -1;
    }

    return 0;
}

int32_t unit_timeout_event_add(TIMEOUT_ID_e id, TIMEOUT_SEC_e time, int32_t (*callback)(void* arg))
{
    if (time >= (TIMEOUT_SEC_NUM - 2) || id >= TIMEOUT_ID_NUM) {
        printf("%s error input %d %d !\n", __func__, time, id);
        return -1;
    }
    TIMEOUT_EVENT_t *p_event = NULL;
    int32_t num, i;

    pthread_mutex_lock(&g_thread_mutex);
    num = (g_timeout_sec + time + 2) % TIMEOUT_SEC_NUM;
    p_event = &g_event[num];
    p_event->unit[id].flag = 1;
    p_event->unit[id].id   = id;
    p_event->unit[id].callback = callback;
    p_event->count ++;
    pthread_mutex_unlock(&g_thread_mutex);

    return 0;
}

int32_t unit_timeout_event_delete(TIMEOUT_ID_e id)
{
    if (id >= TIMEOUT_ID_NUM) {
        printf("%s error input %d !\n", __func__, id);
        return -1;
    }
    int32_t i = 0;

    pthread_mutex_lock(&g_thread_mutex);
    for (i = 0; i < SCALE_NUM; i ++) { 
        if (g_event[i].unit[id].flag == 1) {
            g_event[i].unit[id].flag = 0;
            g_event[i].count --;
        }
    }
    pthread_mutex_unlock(&g_thread_mutex);

    return 0;
}

static void *monitor_task(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&g_thread_mutex);
        g_timeout_sec ++;
        if (g_timeout_sec == TIMEOUT_SEC_NUM) {
            g_timeout_sec = TIMEOUT_SEC_0_5;
        }
        p_global_scale   = &g_event[g_timeout_sec];
        p_global_timeout = p_global_scale;
        pthread_mutex_unlock(&g_thread_mutex);
        usleep(MOITOR_TASK_CYCLE_MS*MS_TO_US);
    }

    pthread_exit(NULL);

    return NULL;
}

static void *handle_task(void *arg)
{
    int32_t i = 0;

    while (1)
    {
        pthread_mutex_lock(&g_thread_mutex);
        for (i = 0; i < EVENT_NUM; i ++) {
            if (p_global_timeout->count == 0) {
                break;
            }
            if (p_global_timeout->unit[i].flag && 
                p_global_timeout->unit[i].callback != NULL) {
                p_global_timeout->unit[i].callback(&p_global_timeout->unit[i].id);
                p_global_timeout->unit[i].flag = 0;
                p_global_timeout->count --;
            }
        }
        pthread_mutex_unlock(&g_thread_mutex);
        usleep(HANDLE_TASK_CYCLE_MS*MS_TO_US);
    }

    pthread_exit(NULL);

    return NULL;
}