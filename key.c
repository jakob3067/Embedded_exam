/*****************************************************************************
* University of Southern Denmark
* Embedded Programming (EMP)
*
* MODULENAME.: key.c
*
* PROJECT....: EMP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 150321  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "emp_type.h"
#include "key.h"
#include "uart.h"

extern QueueHandle_t xKeyQueue;

static INT8U row(INT8U y)
{
    switch(y)
    {
        case 0x08: return 1;  // PE3 = Y1 = row 1 (1,2,3)
        case 0x04: return 2;  // PE2 = Y2 = row 2 (4,5,6)
        case 0x02: return 3;  // PE1 = Y3 = row 3 (7,8,9)
        case 0x01: return 4;  // PE0 = Y4 = row 4 (*,0,#)
    }
    return 0;
}

static INT8U key_catch(INT8U x, INT8U y)
{
    const INT8U matrix[4][3] = {{'1','2','3'},
                                 {'4','5','6'},
                                 {'7','8','9'},
                                 {'*','0','#'}};
    if (x < 1 || x > 3 || y < 1 || y > 4) return 0;
    return matrix[y-1][x-1];
}

void drive_column(INT8U col)
{
    GPIO_PORTA_DATA_R &= ~0x1C;  // all columns LOW first
    switch(col)
    {
        case 1: GPIO_PORTA_DATA_R |= 0x10; break;  // PA4
        case 2: GPIO_PORTA_DATA_R |= 0x08; break;  // PA3
        case 3: GPIO_PORTA_DATA_R |= 0x04; break;  // PA2
    }
}

BOOLEAN check_column(INT8U x, INT8U *pressed_key)
{
    volatile int i;
    INT8U y, r;

    drive_column(x);

    for(i = 0; i < 1000; i++);  // settle time

    // Pull-downs: unpressed = LOW, pressed = HIGH, no inversion needed
    y = GPIO_PORTE_DATA_R & 0x0F;

    // All columns low after reading
    GPIO_PORTA_DATA_R &= ~0x1C;

    if(y == 0)                      { *pressed_key = 0; return 0; }
    if((y & (y - 1)) != 0)         { *pressed_key = 0; return 0; }  // multi-press

    r = row(y);
    if(r == 0)                      { *pressed_key = 0; return 0; }

    *pressed_key = key_catch(x, r);
    return 1;
}

BOOLEAN get_keyboard(INT8U *pch)
{
    if(xKeyQueue == NULL) return 0;
    return (xQueueReceive(xKeyQueue, pch, pdMS_TO_TICKS(20)) == pdTRUE);
}

void key_init(void)
{
    SYSCTL_RCGCGPIO_R |= (1 << 0) | (1 << 4);
    while((SYSCTL_PRGPIO_R & ((1 << 0) | (1 << 4))) == 0);

    // PA2, PA3, PA4 = outputs (columns)
    GPIO_PORTA_DIR_R |= 0x1C;
    GPIO_PORTA_DEN_R |= 0x1C;
    GPIO_PORTA_DATA_R &= ~0x1C;  // start all columns low

    // PE0, PE1, PE2, PE3 = inputs (rows) with pull-downs
    GPIO_PORTE_DIR_R &= ~0x0F;
    GPIO_PORTE_DEN_R |= 0x0F;
    GPIO_PORTE_PDR_R |= 0x0F;   // pull-DOWN to match schematic
}

void key_task(void *pvParameters)
{
    (void)pvParameters;
    INT8U key_val;
    INT8U col;
    INT8U last_key = 0;

    while(1)
    {
        key_val = 0;

        for(col = 1; col <= 3; col++)
        {
            if(check_column(col, &key_val))
                break;
        }

        if(key_val != 0 && key_val != last_key)
        {
            uart0_putc('[');
            uart0_putc(key_val);
            uart0_putc(']');
            uart0_putc('\r');
            uart0_putc('\n');

            xQueueSend(xKeyQueue, &key_val, 0);
            last_key = key_val;
        }

        if(key_val == 0)
            last_key = 0;

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
