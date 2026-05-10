/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: leds.c
*
* PROJECT....: ECP
*
* DESCRIPTION: See module specification file (.h-file).
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 050128  KA    Module created.
*
*****************************************************************************/

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


/*****************************    Defines    *******************************/
#define PF0		0		// Bit 0
extern QueueHandle_t xKeyQueue;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void status_led_init(void)
/*****************************************************************************
*   Input    : 	-
*   Output   : 	-
*   Function : 	
*****************************************************************************/
{
  INT8S dummy;
  // Enable the GPIO port that is used for the on-board LED.
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD;

  // Do a dummy read to insert a few cycles after enabling the peripheral.
  dummy = SYSCTL_RCGC2_R;

  GPIO_PORTD_DIR_R |= 0x40;
  GPIO_PORTD_DEN_R |= 0x40;
}


void status_led_task(void *pvParameters)
{
    INT8U key_val;
	
	while(1)
	{
		// Toggle status led
	    if (xQueueReceive(xKeyQueue, &key_val, 0) == pdTRUE)
	    {
	        GPIO_PORTD_DATA_R ^= 0x40;
	        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 ms.
	        GPIO_PORTD_DATA_R ^= 0x40;
	        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 ms.

	    }
	    vTaskDelay(pdMS_TO_TICKS(20));
	}
}



void status_led_task_2(void *pvParameters)
{


    while(1)
    {
        // Toggle status led
        GPIO_PORTD_DATA_R ^= 0x40;
        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 ms.
        GPIO_PORTD_DATA_R ^= 0x40;
        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 ms.
        GPIO_PORTD_DATA_R ^= 0x40;
        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 ms.
        GPIO_PORTD_DATA_R ^= 0x40;
        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 ms.
        GPIO_PORTD_DATA_R ^= 0x40;
        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1000 ms.
        vTaskDelete(NULL);
    }

}
/****************************** End Of Module *******************************/




