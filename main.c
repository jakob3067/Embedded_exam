#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"

#include "status_led.h"
#include "lcd.h"
#include "key.h"
<<<<<<< HEAD
#include "button.h"
#include "uart.h"
#include "ui.h"

#include "queue.h"
#include "gpio.h"

#include "menu.h"
#include "coffee.h"

=======
#include "rtcs.h"
#include "queue.h"
#include "gpio.h"
#include "menu.h"
#include "coffee.h"
>>>>>>> 3fe2449 (jfewj)

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
<<<<<<< HEAD
#define QUEUE_LEN 16
=======
#define Q_KEY (USER_QUEUE+3)
>>>>>>> 3fe2449 (jfewj)

// Create queues
QueueHandle_t xLCDQueue;
QueueHandle_t xButtonQueue;
QueueHandle_t xUIQueue;
QueueHandle_t xUARTQueue;

static void setupHardware(void){
  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  status_led_init();
  init_gpio();
  uart0_init(115200, 8, 1, 'n');
}

<<<<<<< HEAD
int main(void)
{
    int g = 2; // 0 for espresso, 1 for latte, 2 for filter coffee
=======
  static void key_lcd_task(void *pvParameters)
  {
    INT8U ch;

    (void)pvParameters;

    move_LCD(0, 1);

    while(1)
    {
      if( get_keyboard( &ch ))
      {
        out_LCD( ch );
      }
      else
      {
        vTaskDelay(pdMS_TO_TICKS(10));
      }
    }
  }





int main(void)
{
    int g = 2; // 0 for espresso, 1 for latte, 2 for filter coffee
    int money = 30; // money global very danger
    TaskHandle_t xBrew = NULL;
>>>>>>> 3fe2449 (jfewj)

    setupHardware();
    xLCDQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U *));
    xButtonQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U *));
    xUARTQueue = xQueueCreate(QUEUE_LEN, sizeof(INT8U));

<<<<<<< HEAD
    xTaskCreate( lcd_task, "lcd", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
    xTaskCreate( button_task, "button", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate( brew_task, "brew", USERTASK_STACK_SIZE, (void*)g, LOW_PRIO, NULL);
    xTaskCreate( uart_log_task, "log", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
    xTaskCreate( ui_task, "ui", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL);
=======
    open_queue( Q_KEY );

    xTaskCreate( status_led_task, "Status_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

    //xTaskCreate( lcd_task, "LCD", USERTASK_STACK_SIZE, NULL, HIGH_PRIO, NULL);

    xTaskCreate( key_task, "Keypad", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);

    xTaskCreate( key_lcd_task, "KeyLCD", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);

    //xTaskCreate( brew_task, "brew", USERTASK_STACK_SIZE, (void*)g, MED_PRIO, &xBrew);

>>>>>>> 3fe2449 (jfewj)

    vTaskStartScheduler();

	while(1){
	    // Write to the LCD

	}
}
