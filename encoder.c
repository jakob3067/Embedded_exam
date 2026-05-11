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
    INT8U a = encoder_turned_a();
    INT8U b = encoder_turned_b();
    INT8U coin;
    INT8U total;
    while(1)
    {
        INT8U a_new = encoder_turned_a();
        INT8U b_new = encoder_turned_b();

        if(a == 1 && a_new == 0)
        {
            if(b_new == 1)
            {
                coin = 5;
                xQueueSend(xEncoderQueue, &coin, 0);
            }
            else
            {
                coin = 20;
                xQueueSend(xEncoderQueue, &coin, 0);
            }
//            total = total + coin;
//            xQueueSend(xEncoderQueue, &total, 0);
        }
        else if(a == 0 && a_new == 1)
        {
            if(b_new == 1)
            {
                coin = 20;
                xQueueSend(xEncoderQueue, &coin, 0);
            }
            else
            {
                coin = 5;
                xQueueSend(xEncoderQueue, &coin, 0);
            }
//            total = total + coin;
//            xQueueSend(xEncoderQueue, &total, 0);
        }
        a = a_new;
        b = b_new;

        vTaskDelay(pdMS_TO_TICKS(2));
    }
}
