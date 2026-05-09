#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"

#include "status_led.h"
#include "lcd.h"
#include "key.h"
#include "button.h"

#include "queue.h"
#include "gpio.h"

#include "menu.h"
#include "coffee.h"


#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
#define QUEUE_LEN 16

// Create queues
QueueHandle_t xLCDQueue;
QueueHandle_t xButtonQueue;

static void setupHardware(void){
  // TODO: Put hardware configuration and initialisation in here

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  status_led_init();
  init_gpio();
}

int main(void)
{
    int g = 2; // 0 for espresso, 1 for latte, 2 for filter coffee

    setupHardware();
    xLCDQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U *));
    xButtonQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U *));

    xTaskCreate(lcd_task, "lcd", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate( button_task, "button", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate( brew_task, "brew", USERTASK_STACK_SIZE, (void*)g, LOW_PRIO, NULL); //

    vTaskStartScheduler();

	while(1){
	    // Write to the LCD

	}
}
