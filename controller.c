#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "emp_type.h"
#include "lcd.h"
#include "key.h"
#include "tm4c123gh6pm.h"

// State definitions
typedef enum {
    STATE_IDLE,
    STATE_PAYMENT,
    STATE_READY_TO_BREW,
    STATE_PRODUCTION
} system_state_t;

// Product definitions
typedef struct {
    char* name;
    uint16_t price;
} product_t;

extern QueueHandle_t xKeyQueue;
extern QueueHandle_t xLCDQueue;

void coffee_controller_task(void *pvParameters) {
    system_state_t current_state = STATE_IDLE;
    INT8U received_key;
    uint16_t current_balance = 0;
    product_t selected_product;

    wr_str_LCD((INT8U*)"Select Coffee:");

    while (1) {
        switch (current_state) {
            case STATE_IDLE:
                if (xQueueReceive(xKeyQueue, &received_key, portMAX_DELAY)) {
                    if (received_key == '1') { // Espresso [cite: 15]
                        selected_product.name = "Espresso";
                        selected_product.price = 15;
                        current_state = STATE_PAYMENT;
                    } else if (received_key == '2') { // Latte [cite: 16]
                        selected_product.name = "Latte";
                        selected_product.price = 27;
                        current_state = STATE_PAYMENT;
                    }

                    if (current_state == STATE_PAYMENT) {
                        clr_LCD();
                        wr_str_LCD((INT8U*)selected_product.name);
                        move_LCD(0, 1);
                        wr_str_LCD((INT8U*)"Price: 15 DKK"); // Simplified for now
                    }
                }
                break;

            case STATE_PAYMENT:
                // TODO: Here you will check the Encoder for coins [cite: 21, 22]
                // For now, let's simulate that any key finishes payment
                if (xQueueReceive(xKeyQueue, &received_key, pdMS_TO_TICKS(100))) {
                    current_state = STATE_READY_TO_BREW;
                    clr_LCD();
                    wr_str_LCD((INT8U*)"Insert Cup &");
                    move_LCD(0, 1);
                    wr_str_LCD((INT8U*)"Press Start");
                }
                break;

            case STATE_READY_TO_BREW:
                // Check Switch 1 (Cup) and Switch 2 (Start) [cite: 31]
                // Simulation: if SW2 is pressed but SW1 isn't, show instruction [cite: 31]
                if (!(GPIO_PORTF_DATA_R & 0x10)) { // SW1 pressed (Cup Presence)
                    if (!(GPIO_PORTF_DATA_R & 0x01)) { // SW2 pressed (Start)
                        current_state = STATE_PRODUCTION;
                    }
                }
                break;

            case STATE_PRODUCTION:
                clr_LCD();
                wr_str_LCD((INT8U*)"Grinding...");
                GPIO_PORTF_DATA_R |= 0x0A; // Yellow (Red+Green) for Grinding [cite: 32]
                vTaskDelay(pdMS_TO_TICKS(7500)); // 7.5s [cite: 32]

                clr_LCD();
                wr_str_LCD((INT8U*)"Brewing...");
                GPIO_PORTF_DATA_R &= ~0x08; // Red only for Brewing [cite: 32]
                vTaskDelay(pdMS_TO_TICKS(14000)); // 14s [cite: 32]

                GPIO_PORTF_DATA_R &= ~0x02; // All off
                clr_LCD();
                wr_str_LCD((INT8U*)"Done! Take Cup");
                current_state = STATE_IDLE;
                vTaskDelay(pdMS_TO_TICKS(3000));
                clr_LCD();
                wr_str_LCD((INT8U*)"Select Coffee:");
                break;
        }
    }
}
