#ifndef _KEY_H
#define _KEY_H

#include "emp_type.h"
#include "FreeRTOS.h"
#include "queue.h"

void key_init(void);
INT8U key_get(void);
BOOLEAN get_keyboard(INT8U *pch);
QueueHandle_t key_queue_handle(void);
void key_create_task(INT8U priority);

#endif
