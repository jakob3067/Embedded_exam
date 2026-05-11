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

extern QueueHandle_t xLCDQueue;
extern QueueHandle_t xButtonQueue;
extern QueueHandle_t xUARTQueue;
extern QueueHandle_t xEncoderQueue;
extern QueueHandle_t xKeyQueue;
extern QueueHandle_t xMenuQueue;
extern QueueHandle_t xPaymentQueue;
extern QueueHandle_t xPaymentStatusQueue;

void payment_task(void *pvParameters)
{
    INT8U *pStr;
    INT8U coffee_type;
    INT8U target_price;
    INT8U key_val;
    INT8U btn_event;
    INT8U coin;
    INT8U cash = 0;
    INT8U success = 1;
    INT8U change = 0;

    while(1)
    {
        if(xQueueReceive(xPaymentQueue, &coffee_type, portMAX_DELAY) == pdTRUE)
        {
            cash = 0;
            success = 0;

            if (coffee_type == 0) target_price = 10;      // Espresso
            else if (coffee_type == 1) target_price = 15; // Latte
            else target_price = 5;                        // Filter

            pStr = (INT8U *)"1.Cash 2.Card";
            xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

            // Wait for key '1' or '2'
            while(1)
            {
                if (xQueueReceive(xKeyQueue, &btn_event, portMAX_DELAY) == pdTRUE)
                {
                    if (btn_event == '1')  // ASCII '1' not integer 1
                    {
                        pStr = (INT8U *)"Insert money";
                        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

                        vTaskDelay(pdMS_TO_TICKS(2000));

                        pStr = (INT8U *)" '*' When done";
                        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);

                        while(1)
                        {
                            if(xQueueReceive(xEncoderQueue, &coin, 0) == pdTRUE)
                            {
                                cash += coin;
                                if(coin == 5)
                                {
                                    pStr = (INT8U *)"MONEY!";
                                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                    vTaskDelay(pdMS_TO_TICKS(1000));
                                    pStr = (INT8U *)"5 received";
                                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                    vTaskDelay(pdMS_TO_TICKS(1000));
                                    pStr = (INT8U *)" '*' When done";
                                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                }
                                else if(coin == 20)
                                {
                                    pStr = (INT8U *)"MONEY!";
                                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                    vTaskDelay(pdMS_TO_TICKS(1000));
                                    pStr = (INT8U *)"20 received";
                                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                    vTaskDelay(pdMS_TO_TICKS(1000));
                                    pStr = (INT8U *)" '*' When done";
                                    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                }
                            }

                            if(xQueueReceive(xKeyQueue, &key_val, 0) == pdTRUE)
                            {
                                if (key_val == '*')
                                {
                                    if( cash == target_price || coffee_type == 2)
                                    {
                                        pStr = (INT8U *)"Payment Received";
                                        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                        success = 1;
                                        vTaskDelay(pdMS_TO_TICKS(2000));
                                        break;
                                    }
                                    else if(cash > target_price && coffee_type != 2)
                                    {
                                        volatile int i;
                                        change = cash - target_price;
                                        pStr = (INT8U *)"Returning change";
                                        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                        vTaskDelay(pdMS_TO_TICKS(1000));
                                        for(i = 0; i < change; i++)
                                        {
                                            // Blink green LED for each 1kr in change
                                            GPIO_PORTF_DATA_R |= 0x08;
                                            vTaskDelay(pdMS_TO_TICKS(200));

                                            GPIO_PORTF_DATA_R &= ~0x08;
                                            vTaskDelay(pdMS_TO_TICKS(200));
                                        }
                                        success = 1;
                                        vTaskDelay(pdMS_TO_TICKS(2000));
                                        break;
                                    }
                                    else
                                    {
                                        pStr = (INT8U *)"Insufficient";
                                        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                                    }
                                }
                            }
                            vTaskDelay(pdMS_TO_TICKS(10));
                        }
                        if(success)
                        {
                            break;
                        }

                    }

                    else if (btn_event == '2')
                    {
                        pStr = (INT8U *)"Card selected";
                        xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);
                        vTaskDelay(pdMS_TO_TICKS(3000));
                        card();
                    }
                }
            }
            xQueueSend(xPaymentStatusQueue, &success, portMAX_DELAY);
        }
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

void card(void)
{
    while(1)
    {
        INT8U *pStr;
        INT8U card_details[16];
        INT8U card_pin[4];
        INT8U display_buf[17];  // buffer for '*' display
        INT8U digit;
        INT8U success = 1;
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
            xQueueSend(xPaymentStatusQueue, &success, portMAX_DELAY);
            return;
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

