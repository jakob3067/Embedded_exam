/*
 * coffee.c
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
#include "semphr.h"
#include "emp_type.h"
//#include "glob_def.h"
//#include "binary.h"
#include "status_led.h"
#include "coffee.h"
#include "button.h"


/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0
#define Yellow  0xFB

extern QueueHandle_t xLCDQueue;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void brew(int coffee)
{
    INT8U *pStr;

    if (coffee == 0)
    {
        // Espresso
        /* Yellow light: grinding */
        GPIO_PORTF_DATA_R &= 0xFB;  /* Yellow on */
        // Write "Grinding" to LCD
        pStr = (INT8U *)"Grinding\n";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Duration
        vTaskDelay(pdMS_TO_TICKS(7500));
        GPIO_PORTF_DATA_R |= 0x04; /* Yellow off */

        
        /* Red light: brew */
        GPIO_PORTF_DATA_R &= 0xFD;  /* Red on */
        // Write "Brewing" to LCD
        pStr = (INT8U *)"Brewing\n";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Duration
        vTaskDelay(pdMS_TO_TICKS(14000));
        GPIO_PORTF_DATA_R |= 0x02; /* Red off */
    }
    else if (coffee == 1)
    {
        // Latte
        /* Yellow light: grinding */
        GPIO_PORTF_DATA_R &= 0xFB;  /* Yellow on */
        // Write "Grinding" to LCD
        pStr = (INT8U *)"Grinding\n";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Duration
        vTaskDelay(pdMS_TO_TICKS(7500));
        GPIO_PORTF_DATA_R |= 0x04; /* Yellow off */

        /* Red light: Brewing */
        GPIO_PORTF_DATA_R &= 0xFD;  /* Red on */
        // Write "Brewing" to LCD
        pStr = (INT8U *)"Brewing\n";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
        
        // Duration
        vTaskDelay(pdMS_TO_TICKS(14000));
        GPIO_PORTF_DATA_R |= 0x02; /* Red off */

        /* Green light: Milk frothing */
        GPIO_PORTF_DATA_R &= 0xF7; /* Green on*/
        // Write "Milk frothing" to LCD

        // Duration
        vTaskDelay(pdMS_TO_TICKS(6200));
        GPIO_PORTF_DATA_R |= 0x08; /* Green off */
    }
    else if (coffee == 2)
    {
        // Filter Coffee
        while(1) // While "money"
        {
            /* Yellow light: brewing (for some reason) */
            GPIO_PORTF_DATA_R &= 0xFD;
            // Write "Brewing" to LCD

            // Slow start
            // Rate = 0.6cl/s
            vTaskDelay(pdMS_TO_TICKS(30));

            // After start
            // Rate = 1.45cl/s

        }



    }
    else
    {
        /* Error: invalid coffee type */
        GPIO_PORTF_DATA_R |= 0x02;  /* Red on for error */
        vTaskDelay(pdMS_TO_TICKS(3000));
        GPIO_PORTF_DATA_R &= ~0x02; /* Red off */
    }
}


void brew_task(void *pvParameters)
{
    //turns led of when
    GPIO_PORTF_DATA_R |= 0x04;
    vTaskDelay(pdMS_TO_TICKS(100));
    GPIO_PORTF_DATA_R |= 0x02;
    vTaskDelay(pdMS_TO_TICKS(100));
    GPIO_PORTF_DATA_R |= 0x08;
    vTaskDelay(pdMS_TO_TICKS(100));

    brew((int)pvParameters);

    vTaskDelete(NULL);
}













