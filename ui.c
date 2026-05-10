/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "gpio.h"
#include "systick.h"
#include "tmodel.h"
#include "systick.h"
#include "ui.h"
#include "rtc.h"
#include "string.h"
#include "uart.h"
/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT8U i;
INT8U InBuf[128];

/*****************************   Functions   *******************************/


void ui_task(void *pvParameters)
{
    INT8U ch;
    while (1)
    {
        if (uart0_rx_rdy())
        {
            ch = uart0_getc();
            uart0_putc(ch);
            switch (ch)
            {
                case 'r': // Get report
                    break;
                case 'p': // Set price
                    break;
                case 't': // Set time
                {
                    INT8U buf[8];
                    INT8U i;
                    for ( i = 0; i < 8; i++)
                    {
                        while (!uart0_rx_rdy())
                        {
                            vTaskDelay(pdMS_TO_TICKS(1));
                        }
                        ch = uart0_getc();
                        buf[i] = uart0_getc();
                        uart0_putc(ch);
                    }
                    // Parse time from buffer
                    INT8U h = (buf[0] - '0') * 10 + (buf[1] - '0');
                    INT8U m = (buf[3]-'0')*10 + (buf[4]-'0');
                    INT8U s = (buf[6]-'0')*10 + (buf[7]-'0');
                    rtc_set_time( h, m, s );
                }
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/****************************** End Of Module *******************************/












