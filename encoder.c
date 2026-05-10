#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "task.h"
#include "button.h"
//#include "swtimers.h"
#include "tmodel.h"
//#include "messages.h"
#include "queue.h"

extern QueueHandle_t xEncoderQueue;

INT8U encoder_turned_a()
{
    return( !(GPIO_PORTA_DATA_R & 0x20));
}

INT8U encoder_turned_b()
{
    return( !(GPIO_PORTA_DATA_R & 0x40));
}

void encoder_task(void *pvParameters)
{
    INT8U a;
    INT8U b;
    while(1)
    {
        if(encoder_turned_a())
        {
            a++;
            xQueueSend(xEncoderQueue, (void *) &a, portMAX_DELAY);
        }

        else if(encoder_turned_b())
        {
            b++;
            xQueueSend(xEncoderQueue, (void *) &b, portMAX_DELAY);
        }
    }
}
