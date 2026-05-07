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

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void brew(int coffee)
{
    if (coffee == 0)
    {
        /* Yellow light: brewing */
        GPIO_PORTF_DATA_R &= 0xFB;  /* Yellow on */
        vTaskDelay(pdMS_TO_TICKS(7500));
        
        /* Red light: ready */
        GPIO_PORTF_DATA_R |= 0x04; /* Yellow off */
        GPIO_PORTF_DATA_R &= 0xFD;  /* Red on */
        vTaskDelay(pdMS_TO_TICKS(14000));
        
        GPIO_PORTF_DATA_R |= 0x02; /* Red off */
    }
    else if (coffee == 1)
    {
        /* Yellow light: brewing */
        GPIO_PORTF_DATA_R &= 0xFB;  /* Yellow on */
        vTaskDelay(pdMS_TO_TICKS(7500));

        /* Red light: ready */
        GPIO_PORTF_DATA_R |= 0x04; /* Yellow off */
        GPIO_PORTF_DATA_R &= 0xFD;  /* Red on */
        vTaskDelay(pdMS_TO_TICKS(14000));

        GPIO_PORTF_DATA_R |= 0x02; /* Red off */
        GPIO_PORTF_DATA_R &= 0xF7; /* Green on*/
        vTaskDelay(pdMS_TO_TICKS(6200));
        GPIO_PORTF_DATA_R |= 0x08; /* Green off */
    }
    else if (coffee == 2)
    {
        while(1)
        {
            GPIO_PORTF_DATA_R &= 0xFD;
            vTaskDelay(pdMS_TO_TICKS(10));
            if( button_pushed( ))
            {
                GPIO_PORTF_DATA_R &= 0xF7;
                vTaskDelay(pdMS_TO_TICKS(1000));

            }
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
    int *coffee_type = (int *)pvParameters;

    //turns led of when
    GPIO_PORTF_DATA_R |= 0x04;
    vTaskDelay(pdMS_TO_TICKS(100));
    GPIO_PORTF_DATA_R |= 0x02;
    vTaskDelay(pdMS_TO_TICKS(100));
    GPIO_PORTF_DATA_R |= 0x08;
    vTaskDelay(pdMS_TO_TICKS(100));

    brew(coffee_type);

    vTaskDelete(NULL);
}













