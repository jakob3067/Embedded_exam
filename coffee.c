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

/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0
#define Yellow  0xFB

extern QueueHandle_t xLCDQueue;
extern QueueHandle_t xButtonQueue;
extern QueueHandle_t xUARTQueue;
extern QueueHandle_t xEncoderQueue;
extern QueueHandle_t xKeyQueue;
//extern QueueHandle_t xMenuQueue;

/*****************************   Constants   *******************************/
int card_len;
int pin_len;
/*****************************   Variables   *******************************/
INT8U card_details[16];
INT8U card_pin[4];
/*****************************   Functions   *******************************/

void brew(int coffee)
{
    INT8U *pStr;
    INT8U btn_event;
    INT8U coin;
    INT16U cash;
    INT8U cash_buffer[20];


    payment_option();
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
    }
    else if (coffee == 2)
    {
        // Filter Coffee
        INT8U coffee_type = coffee;
        // Write to LCD
        pStr = (INT8U *)"Insert money";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

        while(1)
        {
            if(xQueueReceive(xEncoderQueue, &coin, portMAX_DELAY) == pdTRUE)
            {
                cash = cash + coin;
                if(coin == 5)
                {
                    pStr = (INT8U *)"MONEY!";
                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    pStr = (INT8U *)"5 received";
                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                }
                else if(coin == 20)
                {
                    pStr = (INT8U *)"MONEY!";
                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    pStr = (INT8U *)"20 received";
                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                }
//                sprintf((char *)cash_buffer, "Total: %d", cash);
//                pStr = (INT8U *)cash_buffer;
//                xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
//                vTaskDelay(pdMS_TO_TICKS(100));
            }
            vTaskDelay(pdMS_TO_TICKS(10));
        }


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
                xQueueSend(xLCDQueue, (void *) &pStr, portMAX_DELAY);

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

        GPIO_PORTF_DATA_R &= 0xFD;
        vTaskDelay(pdMS_TO_TICKS(10));
        if( button_pushed( ))
        {
            GPIO_PORTF_DATA_R &= 0xF7;
            vTaskDelay(pdMS_TO_TICKS(1000));
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

void payment_option(void)
{
    INT8U *pStr;
    INT8U btn_event;
    pStr = (INT8U *)"1.Cash 2.Card";
    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

    // Wait for key '1' or '2'
    while(1)
    {
        if (xQueueReceive(xKeyQueue, &btn_event, portMAX_DELAY) == pdTRUE)
        {
            if (btn_event == '1')  // ASCII '1' not integer 1
            {
                pStr = (INT8U *)"Cash selected";
                xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                vTaskDelay(pdMS_TO_TICKS(3000));
                return;  // cash done, exit
            }
            else if (btn_event == '2')
            {
                pStr = (INT8U *)"Card selected";
                xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                vTaskDelay(pdMS_TO_TICKS(3000));
                card();
                return;
            }
        }
    }
}
void card(void)
{
    while(1)
    {
        INT8U *pStr;
        INT8U card_details[16];
        INT8U card_pin[4];
        INT8U display_buf[17];  // buffer for '*' display
        INT8U digit;
        int i;
        pStr = (INT8U *)"Enter card nr:";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
        for (i = 0; i < 16; i++)
        {
            if (xQueueReceive(xKeyQueue, &digit, portMAX_DELAY) == pdTRUE)
            {
                if (digit < '0' || digit > '9')  // Validate input
                {
                    pStr = (INT8U *)"Invalid digit";
                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    i--;  // retry this index
                    continue;
                }

                card_details[i] = digit;
                // Show '*' for each entered digit
                int j;
                for (j = 0; j <= i; j++)
                    display_buf[j] = '*';
                display_buf[i+1] = '\0';
                INT8U *pDisplay = display_buf;
                xQueueSend(xLCDQueue, &pDisplay, portMAX_DELAY);
            }
        }

        // --- PIN entry (4 digits) ---
        pStr = (INT8U *)"Enter PIN:";
        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
        for (i = 0; i < 4; i++)
        {
            if (xQueueReceive(xKeyQueue, &digit, portMAX_DELAY) == pdTRUE)
            {
                if (digit < '0' || digit > '9')  // Validate input
                {
                    pStr = (INT8U *)"Invalid digit";
                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    i--;  // retry this index
                    continue;
                }
                card_pin[i] = digit;
                // Show '*' for each entered digit
                int j;
                for (j = 0; j <= i; j++)
                    display_buf[j] = '*';
                display_buf[i+1] = '\0';
                INT8U *pDisplay = display_buf;
                xQueueSend(xLCDQueue, &pDisplay, portMAX_DELAY);
            }
        }

        if(validate_pay(card_details, card_pin) == 1)
        {
            pStr = (INT8U *)"Payment OK!";
            xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
            vTaskDelay(pdMS_TO_TICKS(3000));
            return;  // success - exit
        }
        else
        {
            pStr = (INT8U *)"Payment failed";
            xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
            vTaskDelay(pdMS_TO_TICKS(3000));
            break;
        }
        // --- Dummy validation ---
        // Always accepts for now - replace with real validation later
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

INT8U validate_pay(INT8U *card_details, INT8U *card_pin)
{
    int card_sum = 0;
    int pin_sum = 0;
    int i;

    for (i = 0; i < 16; i++)
        card_sum += (card_details[i] - '0');

    for (i = 0; i < 4; i++)
        pin_sum += (card_pin[i] - '0');

    // Both even or both odd = valid
    if ((card_sum % 2) == (pin_sum % 2))
        return 1;
    else
        return 0;
}


void brew_task(void *pvParameters)
{
    INT8U coffee_type;

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
            brew(coffee_type);
        }
    }
    //vTaskDelete(NULL);
}



