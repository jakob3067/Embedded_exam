#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"

#include "status_led.h"
#include "lcd.h"
#include "key.h"
#include "button.h"
#include "uart.h"
#include "ui.h"
#include "rtc.h"
#include "encoder.h"
#include "menu.h"
#include "coffee.h"
#include "queue.h"
#include "gpio.h"
#include "payment.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
#define QUEUE_LEN 16

void card(void);
INT8U validate_pay(INT8U *card_details, INT8U *card_pin);

INT8U is_brewing;

// Create queues
QueueHandle_t xLCDQueue;
QueueHandle_t xButtonQueue;
QueueHandle_t xUIQueue;
QueueHandle_t xUARTQueue;
QueueHandle_t xEncoderQueue;
QueueHandle_t xMenuQueue;
QueueHandle_t xKeyQueue;
QueueHandle_t xPaymentQueue;
QueueHandle_t xPaymentStatusQueue;

static void setupHardware(void){
  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  status_led_init();
  init_gpio();
  key_init();
  uart0_init(115200, 8, 1, 'n');
}

int main(void)
{
    // int g = 2; // 0 for espresso, 1 for latte, 2 for filter coffee

    setupHardware();
    xLCDQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U *));
    xButtonQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U *));
    xUARTQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    xUIQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    xEncoderQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    xMenuQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    xKeyQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    xPaymentQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));
    xPaymentStatusQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));

    xTaskCreate( lcd_task, "lcd", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate( key_task, "key", USERTASK_STACK_SIZE, NULL, HIGH_PRIO, NULL);
    xTaskCreate( button_task, "button", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate( brew_task, "brew", USERTASK_STACK_SIZE * 2, NULL, MED_PRIO, NULL);
    xTaskCreate( uart_log_task, "log", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate( ui_task, "ui", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate( encoder_task, "encoder", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate( menu_task, "menu", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate( payment_task, "menu", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);

    //payment_option();

    is_brewing = 0;

    vTaskStartScheduler();

	while(1){
	    // Cant touch this
	}
}
