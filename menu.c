/*
 * Menu.c
 *
 *  Created on: May 7, 2026
 *      Author: jakob
 */
/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "emp_type.h"
#include "glob_def.h"
#include "status_led.h"
#include "menu.h"
#include "lcd.h"
#include "coffee.h"
#include "key.h"

/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3

extern QueueHandle_t xMenuQueue;
extern QueueHandle_t xLCDQueue;
extern QueueHandle_t xKeyQueue;

extern INT8U is_brewing;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void menu_task(void *pvParameters)
{
    INT8U *pStr;
    INT8U coffee_type;
    INT8U coffee;
    INT8U *key_val;
    pStr = (INT8U *)"Welcome";
    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

    while(1)
    {
        if(xQueueReceive(xKeyQueue, &pStr, portMAX_DELAY) == pdTRUE)
        {
            while(1)
            {
                pStr = (INT8U *)"Esp, Lat, Fil";
                xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

                if(xQueueReceive(xKeyQueue, &coffee, portMAX_DELAY) == pdTRUE)
                {
                    if(is_brewing == 0)
                    {
                        if(coffee == '1')
                        {
                            coffee_type = 0;
                            xQueueSend(xMenuQueue, &coffee_type, portMAX_DELAY);
                            break;
                        }
                        else if(coffee == '2')
                        {
                            coffee_type = 1;
                            xQueueSend(xMenuQueue, &coffee_type, portMAX_DELAY);
                            break;
                        }
                        else if(coffee == '3')
                        {
                            coffee_type = 2;
                            xQueueSend(xMenuQueue, &coffee_type, portMAX_DELAY);
                            break;
                        }
                        else
                        {
                            pStr = (INT8U *)"Invalid";
                            xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                            break;
                        }
                    }

                }
            }
        }
    }
}

/****************************** End Of Module *******************************/




