/*
 * coffee.c
 *
 *  Created on: May 7, 2026
 *      Author: jakob
 */
/***************************** Include files *******************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
#include "uart.h"
#include "encoder.h"
#include "payment.h"
#include "lcd.h"

/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0
#define Yellow  0xFB

extern QueueHandle_t xLCDQueue;
extern QueueHandle_t xButtonQueue;
extern QueueHandle_t xUARTQueue;
extern QueueHandle_t xEncoderQueue;
extern QueueHandle_t xKeyQueue;
extern QueueHandle_t xMenuQueue;
extern QueueHandle_t xPaymentQueue;
extern QueueHandle_t xPaymentStatusQueue;

/*****************************   Constants   *******************************/
int card_len;
int pin_len;
/*****************************   Variables   *******************************/
INT8U card_details[16];
INT8U card_pin[4];

extern INT8U is_brewing;
/*****************************   Functions   *******************************/

void brew_task(void *pvParameters)
{
    INT8U coffee_type;
    INT8U payment_status;
    INT8U *pStr;

    //turns led of when
    GPIO_PORTF_DATA_R |= 0x04;
    vTaskDelay(pdMS_TO_TICKS(10));
    GPIO_PORTF_DATA_R |= 0x02;
    vTaskDelay(pdMS_TO_TICKS(10));
    GPIO_PORTF_DATA_R |= 0x08;
    vTaskDelay(pdMS_TO_TICKS(10));

    while(1)
    {
        if(xQueueReceive(xMenuQueue, &coffee_type, portMAX_DELAY) == pdTRUE)
        {
            is_brewing = 1;

            xQueueSend(xPaymentQueue, &coffee_type, portMAX_DELAY);

            if(xQueueReceive(xPaymentStatusQueue, &payment_status, portMAX_DELAY) == pdTRUE)
            {
//                if(payment_status == 1)
//                {
//                    brew(coffee_type);
//                }
                pStr = (INT8U *)"Starting brew";
                xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                vTaskDelay(pdMS_TO_TICKS(2000));
                brew(coffee_type);
            }
        }
    }
    //vTaskDelete(NULL);
}

void brew(int coffee)
{
    INT8U *pStr;
    INT8U btn_event;

    if (coffee == 0)
    {
        // Espresso
        INT8U coffee_type = coffee;
        /* Yellow light: grinding */
        GPIO_PORTF_DATA_R &= 0xFB;  /* Yellow on */
        // Write "Grinding" to LCD
        pStr = (INT8U *)"Grinding...";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Duration
        vTaskDelay(pdMS_TO_TICKS(7500));
        GPIO_PORTF_DATA_R |= 0x04; /* Yellow off */

        
        /* Red light: brew */
        GPIO_PORTF_DATA_R &= 0xFD;  /* Red on */
        // Write "Brewing" to LCD
        pStr = (INT8U *)"Brewing...";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Duration
        vTaskDelay(pdMS_TO_TICKS(14000));
        GPIO_PORTF_DATA_R |= 0x02; /* Red off */
        pStr = (INT8U *)"Coffee done!";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Send coffee type to UART
        xQueueSend(xUARTQueue, &coffee_type, portMAX_DELAY);

        is_brewing = 0;
    }
    else if (coffee == 1)
    {
        // Latte
        INT8U coffee_type = coffee;
        /* Yellow light: grinding */
        GPIO_PORTF_DATA_R &= 0xFB;  /* Yellow on */
        // Write "Grinding" to LCD
        pStr = (INT8U *)"Grinding...";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Duration
        vTaskDelay(pdMS_TO_TICKS(7500));
        GPIO_PORTF_DATA_R |= 0x04; /* Yellow off */

        /* Red light: Brewing */
        GPIO_PORTF_DATA_R &= 0xFD;  /* Red on */
        // Write "Brewing" to LCD
        pStr = (INT8U *)"Brewing...";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
        
        // Duration
        vTaskDelay(pdMS_TO_TICKS(14000));
        GPIO_PORTF_DATA_R |= 0x02; /* Red off */

        /* Green light: Milk frothing */
        GPIO_PORTF_DATA_R &= 0xF7; /* Green on*/
        // Write "Milk frothing" to LCD
        pStr = (INT8U *)"Milk frothing...";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        // Duration
        vTaskDelay(pdMS_TO_TICKS(6200));
        GPIO_PORTF_DATA_R |= 0x08; /* Green off */
        pStr = (INT8U *)"Coffee done!";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
        
        // Send coffee type to UART
        xQueueSend(xUARTQueue, &coffee_type, portMAX_DELAY);

        is_brewing = 0;
    }
    else if (coffee == 2)
    {
        // Filter Coffee
        INT8U coffee_type = coffee;
        pStr = (INT8U *)"Hold button";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        while(1) // While "money"
        {
            // Check for button press
            if (xQueueReceive(xButtonQueue, &btn_event, portMAX_DELAY) == pdTRUE)
            {
                if( btn_event == 1)
                {
                /* Yellow light: brewing (for some reason) */
                GPIO_PORTF_DATA_R &= 0xFD;
                // Write "Brewing" to LCD
                pStr = (INT8U *)"Brewing...";
                xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

                // Slow start
                // Rate = 0.6cl/s
                vTaskDelay(pdMS_TO_TICKS(30));

                // After start
                // Rate = 1.45cl/s
                }
                else if( btn_event == 0)
                {
                    // Stop brewing
                    GPIO_PORTF_DATA_R &= ~0xFD; // Red off
                    pStr = (INT8U *)"Coffee done!";
                    xQueueSend(xLCDQueue, (void *) &pStr, portMAX_DELAY);
                    // Give time to read LCD
                    vTaskDelay(pdMS_TO_TICKS(2000));

                    // Send coffee type to UART
                    xQueueSend(xUARTQueue, &coffee_type, portMAX_DELAY);
                    is_brewing = 0;
                    break;
                }
            }
        }
    }

    else
    {
        /* Error: invalid coffee type */
        GPIO_PORTF_DATA_R |= 0x02;  /* Red on for error */
        // Write "Invalid" to LCD
        pStr = (INT8U *)"Invalid";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(3000));
        pStr = (INT8U *)" ";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
        GPIO_PORTF_DATA_R &= ~0x02; /* Red off */
    }
}

