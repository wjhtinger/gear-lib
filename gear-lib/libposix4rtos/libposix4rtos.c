/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#include "libposix4rtos.h"
#include <stdio.h>

int msleep(unsigned int msec)
{
    vTaskDelay((msec)/portTICK_PERIOD_MS);
    return 0;
}

static void __thread_func(void *arg)
{
    struct pthread_t *t = (struct pthread_t *)arg;
    if (!t->func) {
        printf("thread function is null\n");
        return;
    }
    t->ret = t->func(t->arg);
    vTaskDelete(NULL);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void*), void *arg)
{
    int ret;
    if (thread == NULL) {
        return -1;
    }
    TaskHandle_t *pvCreatedTask = (TaskHandle_t *)calloc(1, sizeof(TaskHandle_t ));
    TaskFunction_t pvTaskCode = __thread_func;
    const char * const pcName = (attr == NULL) ? "thread" : attr->thread_name;
    const uint32_t usStackDepth = (attr == NULL) ? 4096 : attr->stack_depth;
    void * const pvParameters = thread;
    UBaseType_t uxPriority = (attr == NULL) ? 0 : attr->sched_priority;
    thread->func = start_routine;
    thread->arg = arg;
    thread->handle = pvCreatedTask;

    ret = xTaskCreate(pvTaskCode, pcName, usStackDepth,
                      pvParameters, uxPriority, pvCreatedTask);
    return ret;
}

int pthread_join(pthread_t thread, void **retval)
{
    if (!thread.handle)
        return -1;
    free(thread.handle);
    thread.handle = NULL;
    if (retval)
        *retval = thread.ret;
    return 0;
}

int pthread_attr_init(pthread_attr_t *attr)
{
    return 0;
}

void pthread_attr_destroy(pthread_attr_t *attr)
{
}

int pthread_mutex_init(pthread_mutex_t *m, void *attr)
{
    m = xSemaphoreCreateMutex();
    if (!m) {
        return -1;
    }
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *m)
{
    vSemaphoreDelete(*m);
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *m)
{
    xSemaphoreTake(*m, portMAX_DELAY);
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *m)
{
    xSemaphoreGive(*m);
    return 0;
}