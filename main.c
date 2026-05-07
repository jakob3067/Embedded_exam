#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "systick_frt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "status_led.h"
#include "lcd.h"
<<<<<<< HEAD
#include "key.h"
#include "queue.h"
#include "gpio.h"
=======
#include "menu.h"
#include "coffee.h"
>>>>>>> c1880bb (add coffee and button press)

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3

static void setupHardware(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  // TODO: Put hardware configuration and initialisation in here

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  status_led_init();
  init_gpio();
}





int main(void)
{
    int g = 2; // 0 for espresso, 1 for latte

    setupHardware();

<<<<<<< HEAD
    xTaskCreate( status_led_task_2, "Status_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

    xTaskCreate( status_led_task, "Status_led", USERTASK_STACK_SIZE, NULL, LOW_PRIO, NULL );

    xTaskCreate( lcd_task, "LCD", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);
=======
    //xTaskCreate( lcd_task, "LCD", USERTASK_STACK_SIZE, NULL, HIGH_PRIO, NULL);

    //xTaskCreate( menu_task, "Menu", USERTASK_STACK_SIZE, NULL, MED_PRIO, NULL);

    xTaskCreate( brew_task, "brew", USERTASK_STACK_SIZE, (void*)g, LOW_PRIO, NULL); //

>>>>>>> c1880bb (add coffee and button press)

    vTaskStartScheduler();

	while(1){
	    // Write to the LCD

	}
}
