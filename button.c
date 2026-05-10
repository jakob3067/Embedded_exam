/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: tryk.c
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
* 090215  MoH   Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "task.h"
#include "button.h"
//#include "swtimers.h"
#include "tmodel.h"
//#include "messages.h"
#include "queue.h"

/*****************************    Defines    *******************************/
#define BS_IDLE     0
#define BS_PUSH     1
#define BS_RELEASED  2

extern QueueHandle_t xButtonQueue;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

INT8U    dummy1;
INT16S   dummy2;

/*****************************   Functions   *******************************/
INT8U button_pushed()
{
  return( !(GPIO_PORTF_DATA_R & 0x01) );
}

void button_task(void *pvParameters)
{
	INT8U button_state = BS_IDLE;
	INT8U event = 1;

	while (1){
		switch( button_state )
		{
			case BS_IDLE:
			    vTaskDelay(pdMS_TO_TICKS(10));
				if( button_pushed())		// if button pushed
				{
					button_state = BS_PUSH;
				}
				break;

			case BS_PUSH:
			    event = 1;
				xQueueSend(xButtonQueue, &event, portMAX_DELAY); // Send button press event to queue
				vTaskDelay(pdMS_TO_TICKS(200)); // debounce delay
				while( button_state == BS_PUSH)
				{
					if( !button_pushed()) 		// if button released
					{
                        button_state = BS_RELEASED;
                        break;
					}
					vTaskDelay(pdMS_TO_TICKS(10));
				}
				break;

			case BS_RELEASED:
				event = 0; // Button release event
				xQueueSend(xButtonQueue, &event, portMAX_DELAY); // Send button release event to queue
				vTaskDelay(pdMS_TO_TICKS(200)); // debounce delay
				button_state = BS_IDLE;
				break;
		}
	}
}

/****************************** End Of Module *******************************/












